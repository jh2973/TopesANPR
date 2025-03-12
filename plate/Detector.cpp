#include "detector.h"


PLATEDetector::PLATEDetector(const wchar_t* modelPath, const cv::Size& inputSize, const bool& isGPU) {
	env = Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, "ONNX_DETECTION");
	sessionOptions = Ort::SessionOptions();

	std::vector<std::string> availableProviders = Ort::GetAvailableProviders();
	auto cudaAvailable = std::find(availableProviders.begin(), availableProviders.end(), "CUDAExecutionProvider");
	OrtCUDAProviderOptions cudaOption;

	if (isGPU && (cudaAvailable == availableProviders.end()))
	{
		std::cout << "GPU is not supported by your ONNXRuntime build. Fallback to CPU." << std::endl;
		std::cout << "Inference device: CPU" << std::endl;
	}
	else if (isGPU && (cudaAvailable != availableProviders.end()))
	{
		std::cout << "Inference device: GPU" << std::endl;
		sessionOptions.AppendExecutionProvider_CUDA(cudaOption);
	}
	else
	{
		std::cout << "Inference device: CPU" << std::endl;
	}
	session = Ort::Session(env, modelPath, sessionOptions);

	Ort::AllocatorWithDefaultOptions allocator;

	Ort::TypeInfo inputTypeInfo = session.GetInputTypeInfo(0);
	std::vector<int64_t> inputTensorShape = inputTypeInfo.GetTensorTypeAndShapeInfo().GetShape();
	this->isDynamicInputShape = true;

	inputNames.push_back(session.GetInputName(0, allocator));
	outputNames.push_back(session.GetOutputName(0, allocator));

	this->inputImageShape = cv::Size2f(inputSize);
}

void PLATEDetector::getBestClassInfo(std::vector<float>::iterator it, const int& numClasses, float& bestConf, int& bestClassId) {
	bestClassId = 5;
	bestConf = 0;

	for (int i = 5; i < numClasses + 5; i++) {
		if (it[i] > bestConf) {
			bestConf = it[i];
			bestClassId = i - 5;
		}
	}
}



void PLATEDetector::preprocessing(cv::Mat& image, float*& blob, std::vector<int64_t>& inputTensorShape, int strides) {
	cv::Mat resizedImage, floatImage;


	resizedImage = utils::letterbox(image, this->inputImageShape, cv::Scalar(114, 114, 114), false, false, true, strides);



	inputTensorShape[2] = resizedImage.rows;
	inputTensorShape[3] = resizedImage.cols;

	resizedImage.convertTo(floatImage, CV_32FC3, 1 / 255.0);
	blob = new float[floatImage.cols * floatImage.rows * floatImage.channels()];
	cv::Size floatImageSize{ floatImage.cols, floatImage.rows };

	// hwc -> chw
	std::vector<cv::Mat> chw(floatImage.channels());
	for (int i = 0; i < floatImage.channels(); ++i)
	{
		chw[i] = cv::Mat(floatImageSize, CV_32FC1, blob + i * floatImageSize.width * floatImageSize.height);
	}
	cv::split(floatImage, chw);
}

float IoU(const Detection& box1, const Detection& box2) {
	// Calculate the coordinates of the intersection rectangle.
	float x1 = max(box1.box.x - box1.box.width / 2, box2.box.x - box2.box.width / 2);
	float y1 = max(box1.box.y - box1.box.height / 2, box2.box.y - box2.box.height / 2);
	float x2 = min(box1.box.x + box1.box.width / 2, box2.box.x + box2.box.width / 2);
	float y2 = min(box1.box.y + box1.box.height / 2, box2.box.y + box2.box.height / 2);

	// Calculate the area of the intersection rectangle.
	float intersection_area = max(x2 - x1, 0.0f) * max(y2 - y1, 0.0f);

	// Calculate the area of both bounding boxes.
	float box1_area = box1.box.width * box1.box.height;
	float box2_area = box2.box.width * box2.box.height;

	// Calculate the IoU by dividing the intersection area by the union area.
	float iou = intersection_area / (box1_area + box2_area - intersection_area);

	return iou;
}

std::vector<Detection> PLATEDetector::postprocessing(const cv::Size& resizedImageShape,
	const cv::Size& originalImageShape,
	std::vector<Ort::Value>& outputTensors,
	const float& confThreshold, const float& iouThreshold)
{
	std::vector<cv::Rect> boxes;
	std::vector<float> confs;
	std::vector<int> classIds;

	auto* rawOutput = outputTensors[0].GetTensorData<float>();
	std::vector<int64_t> outputShape = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();
	size_t count = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
	std::vector<float> output(rawOutput, rawOutput + count);


	int numClasses = (int)outputShape[2] - 5;
	int elementsInBatch = (int)(outputShape[1] * outputShape[2]);

	// only for batch size = 1
	for (auto it = output.begin(); it != output.begin() + elementsInBatch; it += outputShape[2])
	{
		float clsConf = it[4];

		if (clsConf > confThreshold)
		{
			int centerX = (int)(it[0]);
			int centerY = (int)(it[1]);
			int width = (int)(it[2]);
			int height = (int)(it[3]);
			int left = centerX - width / 2;
			int top = centerY - height / 2;

			float objConf;
			int classId;
			this->getBestClassInfo(it, numClasses, objConf, classId);

			float confidence = clsConf * objConf;

			boxes.emplace_back(left, top, width, height);
			confs.emplace_back(confidence);
			classIds.emplace_back(classId);
		}
	}

	std::vector<int> indices;
	cv::dnn::NMSBoxes(boxes, confs, confThreshold, iouThreshold, indices);
	// std::cout << "amount of NMS indices: " << indices.size() << std::endl;

	std::vector<Detection> detections;

	for (int idx : indices)
	{
		Detection det;
		det.box = cv::Rect(boxes[idx]);
		utils::scaleCoords(resizedImageShape, det.box, originalImageShape);

		det.conf = confs[idx];
		det.classId = classIds[idx];
		detections.emplace_back(det);
	}

	return detections;
}



std::vector<Detection> PLATEDetector::detect(cv::Mat& image, float confThreshold, float iouThreshold, int strides) {
	cv::Size fix_size = this->inputImageShape;

	float* blob = nullptr;

	//std::cout << "model input image size : " << image.size() << std::endl;



	std::vector<int64_t> inputTensorShape{ 1, 3, -1, -1 };
	this->preprocessing(image, blob, inputTensorShape, strides);

	size_t inputTensorSize = utils::vectorProduct(inputTensorShape);

	std::vector<float> inputTensorValues(blob, blob + inputTensorSize);

	std::vector<Ort::Value> inputTensors;

	Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(
		OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);

	inputTensors.push_back(Ort::Value::CreateTensor<float>(
		memoryInfo, inputTensorValues.data(), inputTensorSize,
		inputTensorShape.data(), inputTensorShape.size()
		));






	std::vector<Ort::Value> outputTensors = this->session.Run(Ort::RunOptions{ nullptr },
		inputNames.data(),
		inputTensors.data(),
		1,
		outputNames.data(),
		1);




	cv::Size resizedShape = cv::Size((int)inputTensorShape[3], (int)inputTensorShape[2]);
	std::vector<Detection> result = this->postprocessing(resizedShape,
		image.size(),
		outputTensors,
		confThreshold, iouThreshold);

	delete[] blob;

	std::sort(result.begin(), result.end(), [](const Detection& a, const Detection& b) {
		return a.conf > b.conf;
		});

	std::vector<Detection> filteredResults;
	for (const auto& res : result) {
		bool keepResult = true;
		for (const auto& filteredResult : filteredResults) {
			float iou = IoU(res, filteredResult);
			if (iou >= 0.60) {
				// IoU가 threshold 이상이면서 conf가 작은 결과를 제거
				if (res.conf < filteredResult.conf) {
					keepResult = false;
					break;
				}
			}
		}
		if (keepResult) {
			filteredResults.push_back(res);
		}
	}

	return filteredResults;
}

void PLATEDetector::warmup()
{
	cv::Mat warm_input(640, 640, CV_8UC3);
	cv::randu(warm_input, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
	std::vector<Detection> warmup = this->detect(warm_input, 0.20, 0.45, 32);
}
#pragma once
#include <onnxruntime_cxx_api.h>
#include <utility>
#include <vector>

#include "utils.h"




class PLATEDetector
{
public:


	explicit PLATEDetector(std::nullptr_t) {};

	PLATEDetector(const wchar_t* modelPath, const cv::Size& inputSize, const bool& isGPU);
	std::vector<Detection> detect(cv::Mat& image, float confThreshold, float iouThreshold, int strides);
	void warmup();

private:
	Ort::Env env{ nullptr };
	Ort::SessionOptions sessionOptions{ nullptr };
	Ort::Session session{ nullptr };

	void preprocessing(cv::Mat& image, float*& blob, std::vector<int64_t>& inputTensorShape, int strides);

	std::vector<Detection> postprocessing(const cv::Size& resizedImageShape,
		const cv::Size& originalImageShape,
		std::vector<Ort::Value>& outputTensors,
		const float& confThreshold, const float& iouThreshold);

	static void getBestClassInfo(std::vector<float>::iterator it, const int& numClasses,
		float& bestConf, int& bestClassId);

	std::vector<const char*> inputNames;
	std::vector<const char*> outputNames;
	bool isDynamicInputShape{};
	cv::Size2f inputImageShape;
	std::vector<float> confThresholds;
	float iouThreshold;
	bool is_he;
};

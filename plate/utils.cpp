#include "utils.h"
#include <variant>

std::vector<std::string> classNames0 = { "LP_car1", "LP_car2", "LP_bike" };
std::vector<std::string> classNames1 = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "가", "나", "다", "라", "마", "거", "너", "더", "러", "머", "고", "노", "도", "로", "모",
	"구", "누", "두", "루", "무", "버", "서", "어", "저", "보", "소", "오", "조", "부", "수", "우", "주", "허", "하", "호", "바", "사", "아",
	"자", "배", "국", "합", "육", "해", "공", "서울", "경기", "인천", "강원", "충남", "대전", "충북", "부산", "울산", "대구", "경북", "경남",
	"전남", "광주", "전북", "제주", "세종" };

std::vector<std::string> classNames2 = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "가", "나", "다", "라", "마", "거", "너", "더", "러", "머", "고", "노", "도", "로", "모",
	"구", "누", "두", "루", "무", "버", "서", "어", "저", "보", "소", "오", "조", "부", "수", "우", "주", "허", "바", "사", "아", "자", "배",
	"터", "퍼", "커", "흐", "츠", "브", "으", "스", "느", "르", "호", "투", "즈", "코", "포", "후", "쿠", "푸", "카", "타", "파", "하", "초",
	"서울", "경기", "인천", "강원", "충남", "대전", "충북", "부산", "울산", "대구", "경북", "경남", "전남", "광주", "전북", "제주", "세종",
	"서울", "경기", "인천", "강원", "충남", "대전", "충북", "부산", "울산", "대구", "경북", "경남", "전남", "광주", "전북", "제주", "세종"
};

std::vector<std::string> classNames3 = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"가", "나", "다", "라", "마", "바", "사", "아", "자", "차", "카", "타", "파", "하",
	"거", "너", "더", "러", "머", "버", "서", "어", "저", "처", "커", "터", "퍼", "허",
	"서울", "부산", "대구", "인천", "광주", "대전", "울산", "경기", "강원", "충북", "충남", "전북", "전남", "경북", "경남", "제주", "세종",
	"강남", "강동", "강북", "강서", "관악", "광진", "구로", "금천", "노원", "도봉", "동대문", "동작", "마포", "서대문", "서초", "성동", "성북",
	"송파", "양천", "영등포", "용산", "은평", "종로", "중", "중랑", "금정", "기장", "남", "동", "동래", "부산진", "북", "사상", "사하", "서",
	"수영", "연제", "영도", "해운대", "달서", "달성", "수성", "강화", "계양", "남동", "미추홀", "부평", "연수", "옹진", "광산", "대덕", "유성",
	"울주", "가평", "고양", "과천", "광명", "구리", "군포", "김포", "남양주", "동두천", "부천", "성남", "수원", "시흥", "안산", "안성", "안양",
	"양주", "양평", "여주", "연천", "오산", "용인", "의왕", "의정부", "이천", "파주", "평택", "포천", "하남", "화성", "강릉", "고성", "동해",
	"삼척", "속초", "양구", "양양", "영월", "원주", "인제", "정선", "철원", "춘천", "태백", "평창", "홍천", "화천", "횡성", "괴산", "단양", "보은",
	"영동", "옥천", "음성", "제천", "증평", "진천", "청주", "충주", "계룡", "공주", "금산", "논산", "당진", "보령", "부여", "서산", "서천", "아산",
	"예산", "천안", "청양", "태안", "홍성", "고창", "군산", "김제", "남원", "무주", "부안", "순창", "완주", "익산", "임실", "장수", "전주", "정읍",
	"진안", "강진", "고흥", "곡성", "광양", "구례", "나주", "담양", "목포", "무안", "보성", "순천", "신안", "여수", "영광", "영암", "완도", "장성",
	"장흥", "진도", "함평", "해남", "화순", "경산", "경주", "고령", "구미", "군위", "김천", "문경", "봉화", "상주", "성주", "안동", "영덕", "영양",
	"영주", "영천", "예천", "울릉", "울진", "의성", "청도", "청송", "칠곡", "포항", "거제", "거창", "김해", "남해", "밀양", "사천", "산청", "양산",
	"의령", "진주", "창녕", "창원", "통영", "하동", "함안", "함양", "합천", "서귀포" };

size_t utils::vectorProduct(const std::vector<int64_t>& vector) {
	if (vector.empty())
		return 0;

	size_t product = 1;
	for (const auto& element : vector)
		product *= element;

	return product;
}

std::vector<std::string> utils::loadNames(const std::string& path) {
	// load class names
	std::vector<std::string> classNames;
	std::ifstream infile(path);
	if (infile.good())
	{
		std::string line;
		while (getline(infile, line))
		{
			if (line.back() == '\r')
				line.pop_back();
			classNames.emplace_back(line);
		}
		infile.close();
	}
	else
	{
		std::cerr << "ERROR: Failed to access class name path: " << path << std::endl;
	}

	return classNames;
}

cv::Mat utils::letterbox(cv::Mat& im, cv::Size new_shape, cv::Scalar color, bool _auto, bool scaleFill, bool scaleup, int stride) {
	cv::Size shape = im.size();
	float r = min((float)new_shape.height / (float)shape.height, (float)new_shape.width / (float)shape.width);
	if (!scaleup) {
		r = min(r, 1.0f);
	}

	cv::Size new_unpad = cv::Size(cvRound(shape.width * r), cvRound(shape.height * r));
	float dw = new_shape.width - new_unpad.width;
	float dh = new_shape.height - new_unpad.height;

	if (_auto) {
		dw = fmod(dw, (float)stride);
		dh = fmod(dh, (float)stride);
	}
	else if (scaleFill) {
		dw = 0.0;
		dh = 0.0;
		new_unpad = new_shape;
		r = (float)new_shape.width / (float)shape.width, (float)new_shape.height / (float)shape.height;
	}
	dw /= 2;
	dh /= 2;
	cv::Mat padded_im;
	if (shape != new_unpad) {
		cv::resize(im, padded_im, new_unpad, cv::INTER_LINEAR);
	}
	else {
		padded_im = im;
	}
	int top = int(cvRound(dh - 0.1));
	int bottom = int(cvRound(dh + 0.1));
	int left = int(cvRound(dw - 0.1));
	int right = int(cvRound(dw + 0.1));
	cv::copyMakeBorder(padded_im, padded_im, top, bottom, left, right, cv::BORDER_CONSTANT, color);

	return padded_im;
}

void utils::scaleCoords(const cv::Size& imageShape, cv::Rect& coords, const cv::Size& imageOriginalShape) {
	float gain = min((float)imageShape.height / (float)imageOriginalShape.height,
		(float)imageShape.width / (float)imageOriginalShape.width);

	int pad[2] = { (int)(((float)imageShape.width - (float)imageOriginalShape.width * gain) / 2.0f),
				  (int)(((float)imageShape.height - (float)imageOriginalShape.height * gain) / 2.0f) };


	coords.x = (int)std::round(((float)(coords.x - pad[0]) / gain));
	coords.y = (int)std::round(((float)(coords.y - pad[1]) / gain));

	coords.width = (int)std::round(((float)coords.width / gain));
	coords.height = (int)std::round(((float)coords.height / gain));

	// // clip coords, should be modified for width and height
	coords.x = utils::clip(coords.x, 0, imageOriginalShape.width);
	coords.y = utils::clip(coords.y, 0, imageOriginalShape.height);
	coords.width = utils::clip(coords.width, -coords.x, imageOriginalShape.width - coords.x);
	coords.height = utils::clip(coords.height, -coords.y, imageOriginalShape.height - coords.y);
}

KIST_res utils::plate_sort2(std::vector<Detection> input_orin, int type, Detection plate, cv::Point top_left, cv::Mat crop) {
	KIST_res out;
	std::string plate_num = "";
	std::vector<Plate_Info> words;

	int H = plate.box.height, W = plate.box.width;

	if (type == 0) { // LP_car1
		std::vector<Detection> input;
		for (Detection det : input_orin) {
			float word_area = float(det.box.width) * float(det.box.height);
			if (word_area > 100)
				input.push_back(det);
		}

		std::vector<Detection> ins;
		for (Detection det : input) {
			float w_ratio = float(det.box.height) / float(det.box.width);

			//if (w_ratio > 1.20 && w_ratio < 1.98)
			if (w_ratio > 1.00 && w_ratio < 2.28)
				ins.push_back(det);
		}

		if (ins.size() < 7 || ins.size() > 9) {
			plate_num = "Not_Complete";  // Done
		}
		else {
			std::vector<Detection> regs;
			std::vector<Detection> ins2;
			std::vector<Detection> koreans;
			std::vector<Detection> heads;
			std::vector<Detection> tails;

			for (Detection det : ins) {
				if (det.classId >= 55)
					regs.push_back(det);
				else
					ins2.push_back(det);
			}

			for (Detection det : ins2) {
				if (det.classId >= 10 && det.classId <= 54) {
					koreans.push_back(det);
				}
			}

			if (koreans.size() > 0) {
				std::sort(koreans.begin(), koreans.end(), [](const Detection& a, const Detection& b) {
					return a.conf > b.conf;
					});

				for (Detection det : ins2) {
					if (det.box.x < koreans[0].box.x)
						heads.push_back(det);
					else if (det.box.x > koreans[0].box.x)
						tails.push_back(det);
				}
			}
			else {
				float kor_thresh = float(W) * 0.42;

				for (Detection det : ins) {
					if (det.box.x < kor_thresh)
						heads.push_back(det);
					else
						tails.push_back(det);
				}
			}

			Plate_Info c_word;
			if (regs.size() > 0) {
				std::sort(regs.begin(), regs.end(), [](const Detection& a, const Detection& b) {
					return a.conf > b.conf;
					});
				plate_num += classNames1[regs[0].classId];
				c_word.word = classNames1[regs[0].classId];
				c_word.box = cv::Rect(regs[0].box.x + plate.box.x + top_left.x, regs[0].box.y + plate.box.y + top_left.y, regs[0].box.width, regs[0].box.height);
				c_word.conf = regs[0].conf;
				words.push_back(c_word);
			}

			if (heads.size() > 0) {
				std::sort(heads.begin(), heads.end(), [](const Detection& a, const Detection& b) {
					return a.box.x < b.box.x;
					});

				for (Detection det : heads) {
					plate_num += classNames1[det.classId];
					c_word.word = classNames1[det.classId];
					c_word.box = cv::Rect(det.box.x + plate.box.x + top_left.x, det.box.y + plate.box.y + top_left.y, det.box.width, det.box.height);
					c_word.conf = det.conf;
					words.push_back(c_word);
				}
			}
			else {
				plate_num += "XX";
				c_word.word = "XX";
				c_word.conf = -1.0f;
				words.push_back(c_word);
			}

			if (koreans.size() > 0) {
				plate_num += classNames1[koreans[0].classId];
				c_word.word = classNames1[koreans[0].classId];
				c_word.box = cv::Rect(koreans[0].box.x + plate.box.x + top_left.x, koreans[0].box.y + plate.box.y + top_left.y, koreans[0].box.width, koreans[0].box.height);
				c_word.conf = koreans[0].conf;
				words.push_back(c_word);
			}
			else {
				plate_num += "XX";
				c_word.word = "XX";
				c_word.conf = -1.0f;
				words.push_back(c_word);
			}

			if (tails.size() > 0) {
				std::sort(tails.begin(), tails.end(), [](const Detection& a, const Detection& b) {
					return a.box.x < b.box.x;
					});

				for (Detection det : tails) {
					plate_num += classNames1[det.classId];
					c_word.word = classNames1[det.classId];
					c_word.box = cv::Rect(det.box.x + plate.box.x + top_left.x, det.box.y + plate.box.y + top_left.y, det.box.width, det.box.height);
					c_word.conf = det.conf;
					words.push_back(c_word);
				}
			}
			else {
				plate_num += "XXXX";
				c_word.word = "XXXX";
				c_word.conf = -1.0f;
				words.push_back(c_word);
			}
		}
	}
	else if (type == 1) {

		std::vector<Detection> input;
		for (Detection det : input_orin) {
			float word_area = float(det.box.width) * float(det.box.height);
			if (word_area > 100)
				input.push_back(det);
		}

		if (input.size() < 7 || input.size() > 8) {
			plate_num = "Not_Complete";  // Done
		}
		else {
			std::vector<Detection> regs;
			std::vector<Detection> ins2;
			std::vector<Detection> ins3;
			std::vector<Detection> koreans;
			std::vector<Detection> heads;
			std::vector<Detection> tails;

			for (Detection det : input) {
				if (det.classId >= 88)
					regs.push_back(det);
			}

			if (regs.size() == 0) {
				for (Detection det : input) {
					if (det.classId >= 71 && det.classId <= 87)
						regs.push_back(det);
					else
						ins2.push_back(det);
				}

				for (Detection det : ins2) {
					if (det.classId >= 10 && det.classId <= 70)
						koreans.push_back(det);
					else {
						ins3.push_back(det);
					}
				}

				for (Detection det : ins3) {
					float w_pos = float(det.box.y) + float(det.box.height);

					if (w_pos / H < 0.5)
						heads.push_back(det);
					else
						tails.push_back(det);
				}
			}
			else {
				for (Detection det : input) {
					if (det.classId <= 70)
						ins2.push_back(det);
				}

				for (Detection det : ins2) {
					if (det.classId >= 10 && det.classId <= 70)
						koreans.push_back(det);
					else {
						ins3.push_back(det);
					}
				}

				if (koreans.size() > 0) {
					std::sort(koreans.begin(), koreans.end(), [](const Detection& a, const Detection& b) {
						return a.conf > b.conf;
						});

					for (Detection det : ins3) {
						if (det.box.x < koreans[0].box.x)
							heads.push_back(det);
						else if (det.box.x > koreans[0].box.x)
							tails.push_back(det);
					}
				}
				else {
					float kor_thresh = float(W) * 0.35;

					for (Detection det : ins3) {
						if (det.box.x < kor_thresh)
							heads.push_back(det);
						else
							tails.push_back(det);
					}
				}
			}

			Plate_Info c_word;
			if (regs.size() > 0) {
				std::sort(regs.begin(), regs.end(), [](const Detection& a, const Detection& b) {
					return a.conf > b.conf;
					});

				plate_num += classNames2[regs[0].classId];
				c_word.word = classNames2[regs[0].classId];
				c_word.box = cv::Rect(regs[0].box.x + plate.box.x + top_left.x, regs[0].box.y + plate.box.y + top_left.y, regs[0].box.width, regs[0].box.height);
				c_word.conf = regs[0].conf;
				words.push_back(c_word);
			}

			if (heads.size() > 0) {
				std::sort(heads.begin(), heads.end(), [](const Detection& a, const Detection& b) {
					return a.box.x < b.box.x;
					});

				for (Detection det : heads) {
					plate_num += classNames2[det.classId];
					c_word.word = classNames2[det.classId];
					c_word.box = cv::Rect(det.box.x + plate.box.x + top_left.x, det.box.y + plate.box.y + top_left.y, det.box.width, det.box.height);
					c_word.conf = det.conf;
					words.push_back(c_word);
				}
			}
			else {
				plate_num += "XX";
				c_word.word = "XX";
				c_word.conf = -1.0f;
				words.push_back(c_word);
			}

			if (koreans.size() > 0) {
				std::sort(koreans.begin(), koreans.end(), [](const Detection& a, const Detection& b) {
					return a.conf > b.conf;
					});

				plate_num += classNames2[koreans[0].classId];
				c_word.word = classNames2[koreans[0].classId];
				c_word.box = cv::Rect(koreans[0].box.x + plate.box.x + top_left.x, koreans[0].box.y + plate.box.y + top_left.y, koreans[0].box.width, koreans[0].box.height);
				c_word.conf = koreans[0].conf;
				words.push_back(c_word);
			}
			else {
				plate_num += "X";
				c_word.word = "X";
				c_word.conf = -1.0f;
				words.push_back(c_word);
			}

			if (tails.size() > 0) {
				std::sort(tails.begin(), tails.end(), [](const Detection& a, const Detection& b) {
					return a.box.x < b.box.x;
					});

				for (Detection det : tails) {
					plate_num += classNames2[det.classId];
					c_word.word = classNames2[det.classId];
					c_word.box = cv::Rect(det.box.x + plate.box.x + top_left.x, det.box.y + plate.box.y + top_left.y, det.box.width, det.box.height);
					c_word.conf = det.conf;
					words.push_back(c_word);
				}
			}
			else {
				plate_num += "XXXX";
				c_word.word = "XXXX";
				c_word.conf = -1.0f;
				words.push_back(c_word);
			}
		}
	}
	else { // LP_bike
		std::vector<Detection> input;
		for (Detection det : input_orin) {
			input.push_back(det);
		}

		int bike_number = 7;
		if (hasDetectionWithClassID(input, 54))
			bike_number = 6;


		if (input.size() < bike_number - 2) {
			plate_num = "Not_Complete";  // Done
		}

		else {
			if (hasDetectionWithClassID(input, 45) && hasDetectionWithClassID(input, 42)) { // 경기 광주 가 1234
				std::vector<Detection> reg1_boxes, reg2_boxes, kor_boxes, num_boxes;
				Detection reg1, reg2;
				for (Detection det : input) {
					if (det.classId == 45)
						reg1 = det;
					if (det.classId == 42)
						reg2 = det;
					if (det.classId >= 38 && det.classId <= 54)
						reg1_boxes.push_back(det);
					else if (det.classId >= 55)
						reg2_boxes.push_back(det);
					else if (det.classId >= 10 && det.classId <= 37)
						kor_boxes.push_back(det);
					else
						num_boxes.push_back(det);
				}
				std::sort(reg2_boxes.begin(), reg2_boxes.end(), [](const Detection& a, const Detection& b) {
					return a.conf > b.conf;
					});
				std::sort(kor_boxes.begin(), kor_boxes.end(), [](const Detection& a, const Detection& b) {
					return a.conf > b.conf;
					});
				std::sort(num_boxes.begin(), num_boxes.end(), [](const Detection& a, const Detection& b) {
					return a.box.x < b.box.x;
					});

				Plate_Info c_word;
				plate_num += classNames3[reg1.classId];
				c_word.word = classNames3[reg1.classId];
				c_word.box = cv::Rect(reg1.box.x + plate.box.x + top_left.x, reg1.box.y + plate.box.y + top_left.y, reg1.box.width, reg1.box.height);
				c_word.conf = reg1.conf;
				words.push_back(c_word);

				plate_num += classNames3[reg2.classId];
				c_word.word = classNames3[reg2.classId];
				c_word.box = cv::Rect(reg2.box.x + plate.box.x + top_left.x, reg2.box.y + plate.box.y + top_left.y, reg2.box.width, reg2.box.height);
				c_word.conf = reg2.conf;
				words.push_back(c_word);

				if (kor_boxes.size() > 0) {
					plate_num += classNames3[kor_boxes[0].classId];
					c_word.word = classNames3[kor_boxes[0].classId];
					c_word.box = cv::Rect(kor_boxes[0].box.x + plate.box.x + top_left.x, kor_boxes[0].box.y + plate.box.y + top_left.y, kor_boxes[0].box.width, kor_boxes[0].box.height);
					c_word.conf = kor_boxes[0].conf;
					words.push_back(c_word);
				}
				else {
					plate_num += "X";
					c_word.word = "X";
					c_word.box = cv::Rect(0, 0, 0, 0);
					c_word.conf = -1.0f;
					words.push_back(c_word);
				}

				for (Detection det : num_boxes) {
					plate_num += classNames3[det.classId];
					c_word.word = classNames3[det.classId];
					c_word.box = cv::Rect(det.box.x + plate.box.x + top_left.x, det.box.y + plate.box.y + top_left.y, det.box.width, det.box.height);
					c_word.conf = det.conf;
					words.push_back(c_word);
				}

			}
			else if (hasDetectionWithClassID(input, 54)) { // 세종 가 1234
				std::vector<Detection> reg1_boxes, reg2_boxes, kor_boxes, num_boxes;
				for (Detection det : input) {
					if (det.classId >= 38 && det.classId <= 54)
						reg1_boxes.push_back(det);
					else if (det.classId >= 55)
						reg2_boxes.push_back(det);
					else if (det.classId >= 10 && det.classId <= 37)
						kor_boxes.push_back(det);
					else
						num_boxes.push_back(det);
				}

				std::sort(reg1_boxes.begin(), reg1_boxes.end(), [](const Detection& a, const Detection& b) {
					return a.conf > b.conf;
					});
				std::sort(reg2_boxes.begin(), reg2_boxes.end(), [](const Detection& a, const Detection& b) {
					return a.conf > b.conf;
					});
				std::sort(kor_boxes.begin(), kor_boxes.end(), [](const Detection& a, const Detection& b) {
					return a.conf > b.conf;
					});
				std::sort(num_boxes.begin(), num_boxes.end(), [](const Detection& a, const Detection& b) {
					return a.box.x < b.box.x;
					});

				Plate_Info c_word;
				if (reg1_boxes[0].classId == 54) {
					plate_num += classNames3[reg1_boxes[0].classId];
					c_word.word = classNames3[reg1_boxes[0].classId];
					c_word.box = cv::Rect(reg1_boxes[0].box.x + plate.box.x + top_left.x, reg1_boxes[0].box.y + plate.box.y + top_left.y, reg1_boxes[0].box.width, reg1_boxes[0].box.height);
					c_word.conf = reg1_boxes[0].conf;
					words.push_back(c_word);

					if (kor_boxes.size() > 0) {
						plate_num += classNames3[kor_boxes[0].classId];
						c_word.word = classNames3[kor_boxes[0].classId];
						c_word.box = cv::Rect(kor_boxes[0].box.x + plate.box.x + top_left.x, kor_boxes[0].box.y + plate.box.y + top_left.y, kor_boxes[0].box.width, kor_boxes[0].box.height);
						c_word.conf = kor_boxes[0].conf;
						words.push_back(c_word);
					}
					else {
						plate_num += "X";
						c_word.word = "X";
						c_word.box = cv::Rect(0, 0, 0, 0);
						c_word.conf = -1.0f;
						words.push_back(c_word);
					}

					for (Detection det : num_boxes) {
						plate_num += classNames3[det.classId];
						c_word.word = classNames3[det.classId];
						c_word.box = cv::Rect(det.box.x + plate.box.x + top_left.x, det.box.y + plate.box.y + top_left.y, det.box.width, det.box.height);
						c_word.conf = det.conf;
						words.push_back(c_word);
					}

				}
				else {
					if (reg1_boxes.size() > 0) {
						plate_num += classNames3[reg1_boxes[0].classId];
						c_word.word = classNames3[reg1_boxes[0].classId];
						c_word.box = cv::Rect(reg1_boxes[0].box.x + plate.box.x + top_left.x, reg1_boxes[0].box.y + plate.box.y + top_left.y, reg1_boxes[0].box.width, reg1_boxes[0].box.height);
						c_word.conf = reg1_boxes[0].conf;
						words.push_back(c_word);
					}
					else {
						plate_num += "X";
						c_word.word = "X";
						c_word.box = cv::Rect(0, 0, 0, 0);
						c_word.conf = -1.0f;
						words.push_back(c_word);
					}

					if (reg2_boxes.size() > 0) {
						plate_num += classNames3[reg2_boxes[0].classId];
						c_word.word = classNames3[reg2_boxes[0].classId];
						c_word.box = cv::Rect(reg2_boxes[0].box.x + plate.box.x + top_left.x, reg2_boxes[0].box.y + plate.box.y + top_left.y, reg2_boxes[0].box.width, reg2_boxes[0].box.height);
						c_word.conf = reg2_boxes[0].conf;
						words.push_back(c_word);
					}
					else {
						plate_num += "X";
						c_word.word = "X";
						c_word.box = cv::Rect(0, 0, 0, 0);
						c_word.conf = -1.0f;
						words.push_back(c_word);
					}

					if (kor_boxes.size() > 0) {
						plate_num += classNames3[kor_boxes[0].classId];
						c_word.word = classNames3[kor_boxes[0].classId];
						c_word.box = cv::Rect(kor_boxes[0].box.x + plate.box.x + top_left.x, kor_boxes[0].box.y + plate.box.y + top_left.y, kor_boxes[0].box.width, kor_boxes[0].box.height);
						c_word.conf = kor_boxes[0].conf;
						words.push_back(c_word);
					}
					else {
						plate_num += "X";
						c_word.word = "X";
						c_word.box = cv::Rect(0, 0, 0, 0);
						c_word.conf = -1.0f;
						words.push_back(c_word);
					}

					for (Detection det : num_boxes) {
						plate_num += classNames3[det.classId];
						c_word.word = classNames3[det.classId];
						c_word.box = cv::Rect(det.box.x + plate.box.x + top_left.x, det.box.y + plate.box.y + top_left.y, det.box.width, det.box.height);
						c_word.conf = det.conf;
						words.push_back(c_word);
					}

				}
			}
			else if (countDetectionWithClassID(input, 53) == 2) { // 제주 제주 가 1234            
				std::vector<Detection> reg1_boxes, kor_boxes, num_boxes;
				for (Detection det : input) {
					if (det.classId >= 38 && det.classId <= 54)
						reg1_boxes.push_back(det);
					else if (det.classId >= 10 && det.classId <= 37)
						kor_boxes.push_back(det);
					else if (det.classId < 10)
						num_boxes.push_back(det);
				}
				std::sort(reg1_boxes.begin(), reg1_boxes.end(), [](const Detection& a, const Detection& b) {
					return a.conf > b.conf;
					});
				std::sort(kor_boxes.begin(), kor_boxes.end(), [](const Detection& a, const Detection& b) {
					return a.conf > b.conf;
					});
				std::sort(num_boxes.begin(), num_boxes.end(), [](const Detection& a, const Detection& b) {
					return a.box.x < b.box.x;
					});

				Plate_Info c_word;

				plate_num += classNames3[reg1_boxes[0].classId];
				c_word.word = classNames3[reg1_boxes[0].classId];
				c_word.box = cv::Rect(reg1_boxes[0].box.x + plate.box.x + top_left.x, reg1_boxes[0].box.y + plate.box.y + top_left.y, reg1_boxes[0].box.width, reg1_boxes[0].box.height);
				c_word.conf = reg1_boxes[0].conf;
				words.push_back(c_word);

				plate_num += classNames3[reg1_boxes[1].classId];
				c_word.word = classNames3[reg1_boxes[1].classId];
				c_word.box = cv::Rect(reg1_boxes[1].box.x + plate.box.x + top_left.x, reg1_boxes[1].box.y + plate.box.y + top_left.y, reg1_boxes[1].box.width, reg1_boxes[1].box.height);
				c_word.conf = reg1_boxes[1].conf;
				words.push_back(c_word);

				if (kor_boxes.size() > 0) {
					plate_num += classNames3[kor_boxes[0].classId];
					c_word.word = classNames3[kor_boxes[0].classId];
					c_word.box = cv::Rect(kor_boxes[0].box.x + plate.box.x + top_left.x, kor_boxes[0].box.y + plate.box.y + top_left.y, kor_boxes[0].box.width, kor_boxes[0].box.height);
					c_word.conf = kor_boxes[0].conf;
					words.push_back(c_word);
				}
				else {
					plate_num += "X";
					c_word.word = "X";
					c_word.box = cv::Rect(0, 0, 0, 0);
					c_word.conf = -1.0f;
					words.push_back(c_word);
				}

				for (Detection det : num_boxes) {
					plate_num += classNames3[det.classId];
					c_word.word = classNames3[det.classId];
					c_word.box = cv::Rect(det.box.x + plate.box.x + top_left.x, det.box.y + plate.box.y + top_left.y, det.box.width, det.box.height);
					c_word.conf = det.conf;
					words.push_back(c_word);
				}
			}
			else {
				std::vector<Detection> reg1_boxes, reg2_boxes, kor_boxes, num_boxes;
				for (Detection det : input) {
					if (det.classId >= 38 && det.classId <= 54)
						reg1_boxes.push_back(det);
					else if (det.classId >= 55)
						reg2_boxes.push_back(det);
					else if (det.classId >= 10 && det.classId <= 37)
						kor_boxes.push_back(det);
					else
						num_boxes.push_back(det);
				}

				std::sort(reg1_boxes.begin(), reg1_boxes.end(), [](const Detection& a, const Detection& b) {
					return a.conf > b.conf;
					});
				std::sort(reg2_boxes.begin(), reg2_boxes.end(), [](const Detection& a, const Detection& b) {
					return a.conf > b.conf;
					});
				std::sort(kor_boxes.begin(), kor_boxes.end(), [](const Detection& a, const Detection& b) {
					return a.conf > b.conf;
					});
				std::sort(num_boxes.begin(), num_boxes.end(), [](const Detection& a, const Detection& b) {
					return a.box.x < b.box.x;
					});

				Plate_Info c_word;
				if (reg1_boxes.size() > 0) {
					plate_num += classNames3[reg1_boxes[0].classId];
					c_word.word = classNames3[reg1_boxes[0].classId];
					c_word.box = cv::Rect(reg1_boxes[0].box.x + plate.box.x + top_left.x, reg1_boxes[0].box.y + plate.box.y + top_left.y, reg1_boxes[0].box.width, reg1_boxes[0].box.height);
					c_word.conf = reg1_boxes[0].conf;
					words.push_back(c_word);
				}
				else {
					plate_num += "X";
					c_word.word = "X";
					c_word.box = cv::Rect(0, 0, 0, 0);
					c_word.conf = -1.0f;
					words.push_back(c_word);
				}

				if (reg2_boxes.size() > 0) {
					plate_num += classNames3[reg2_boxes[0].classId];
					c_word.word = classNames3[reg2_boxes[0].classId];
					c_word.box = cv::Rect(reg2_boxes[0].box.x + plate.box.x + top_left.x, reg2_boxes[0].box.y + plate.box.y + top_left.y, reg2_boxes[0].box.width, reg2_boxes[0].box.height);
					c_word.conf = reg2_boxes[0].conf;
					words.push_back(c_word);
				}
				else {
					plate_num += "X";
					c_word.word = "X";
					c_word.box = cv::Rect(0, 0, 0, 0);
					c_word.conf = -1.0f;
					words.push_back(c_word);
				}

				if (kor_boxes.size() > 0) {
					plate_num += classNames3[kor_boxes[0].classId];
					c_word.word = classNames3[kor_boxes[0].classId];
					c_word.box = cv::Rect(kor_boxes[0].box.x + plate.box.x + top_left.x, kor_boxes[0].box.y + plate.box.y + top_left.y, kor_boxes[0].box.width, kor_boxes[0].box.height);
					c_word.conf = kor_boxes[0].conf;
					words.push_back(c_word);
				}
				else {
					plate_num += "X";
					c_word.word = "X";
					c_word.box = cv::Rect(0, 0, 0, 0);
					c_word.conf = -1.0f;
					words.push_back(c_word);
				}

				for (Detection det : num_boxes) {
					plate_num += classNames3[det.classId];
					c_word.word = classNames3[det.classId];
					c_word.box = cv::Rect(det.box.x + plate.box.x + top_left.x, det.box.y + plate.box.y + top_left.y, det.box.width, det.box.height);
					c_word.conf = det.conf;
					words.push_back(c_word);
				}
			}
		}
	}

	out.class_ID = type;
	out.Category = classNames0[type];
	out.class_box = cv::Rect(plate.box.x + top_left.x, plate.box.y + top_left.y, plate.box.width, plate.box.height);
	out.class_conf = plate.conf;
	out.License = plate_num;
	out.word_inf = words;
	out.avg_pixel[0] = cv::mean(crop)[0];
	out.avg_pixel[1] = cv::mean(crop)[1];
	out.avg_pixel[2] = cv::mean(crop)[2];

	return out;
}


bool utils::hasDetectionWithClassID(std::vector<Detection>& detections, int targetClassID) {
	return std::any_of(detections.begin(), detections.end(), [targetClassID](const Detection& det) {
		return det.classId == targetClassID;
		});
}

int utils::countDetectionWithClassID(const std::vector<Detection>& detections, int targetClassID) {
	return std::count_if(detections.begin(), detections.end(), [targetClassID](const Detection& det) {
		return det.classId == targetClassID;
		});
}

template <typename T>
T utils::clip(const T& n, const T& lower, const T& upper)
{
	return max(lower, min(n, upper));
}

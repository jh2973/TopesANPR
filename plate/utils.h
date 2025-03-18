#pragma once
#include <codecvt>
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <windows.h>

struct Detection
{
	cv::Rect box;
	float conf{};
	int classId{};
};

struct Plate_Info
{
	std::string word;
	float conf;
	cv::Rect box;
};

struct Helmet_Info {
	cv::Rect helmet_box;
	float helmet_conf;
	std::string w_Helmet;
};

struct KIST_res
{
	int class_ID;
	std::string Category; // Object class (Motorcycle, LP_car1, LP_car2, LP_bike, w_Helmet, wo_Helmet)        
	cv::Rect class_box;
	float class_conf = -1.0;
	int is_bike;
	cv::Rect bike_box;
	float bike_conf;
	std::string License;  // Number of plate
	std::vector<Plate_Info> word_inf;  // word, conf, box[4] for every words	
	std::vector<Helmet_Info> helmet_inf;
	double avg_pixel[3];
	double plate_angle = -1;
};

struct KIST_res2
{
	int class_ID;
	std::string Category;
};

namespace utils
{
	size_t vectorProduct(const std::vector<int64_t>& vector);
	std::vector<std::string> loadNames(const std::string& path);
	cv::Mat letterbox(cv::Mat& img, cv::Size new_shape, cv::Scalar color, bool _auto, bool scaleFill, bool scaleup, int stride);
	void scaleCoords(const cv::Size& imageShape, cv::Rect& box, const cv::Size& imageOriginalShape);
	KIST_res plate_sort2(std::vector<Detection> ins, int type, Detection det, cv::Point top_left, cv::Mat crop);
	bool hasDetectionWithClassID(std::vector<Detection>& detections, int targetClassID);
	int countDetectionWithClassID(const std::vector<Detection>& detections, int targetClassID);

	template <typename T>
	T clip(const T& n, const T& lower, const T& upper);


}




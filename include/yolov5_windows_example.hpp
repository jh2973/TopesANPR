//#ifdef HAILODLL_EXPORTS
#ifdef HAILODLLV00001_EXPORTS
#define CV_DLL __declspec(dllexport)
#else
#define CV_DLL __declspec(dllimport)

#endif 

#include <future>
#include <opencv2/opencv.hpp>
#include "yolov5_post_processing.hpp"

extern "C" CV_DLL void HailoDLL_init(std::string * hef_paths, size_t * frame_width, size_t * frame_height, int hef_cnt);
extern CV_DLL std::vector<std::vector<DetectionObject>> HailoDLL_infer(cv::Mat* input_image, int* hef_idx, int* cls_num, float* conf_thd, int hef_num);
extern "C" CV_DLL void HailoRelease();
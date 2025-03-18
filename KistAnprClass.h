#pragma once

#include "CLocalInfoSet.h"
#include "CFileManager.h"

//KIST 번호인식
#include <opencv2/opencv.hpp>
#include "plate/Detector.h"
#include "plate/utils.h"
#include "include/yolov5_windows_example.hpp"

#include "plate/Wcommon.h"
#include "plate/Wdisp.h"

class KistAnprClass
{
public:
	KistAnprClass(LPVOID set, LPVOID pManage);
	~KistAnprClass();


private:
	CLocalInfoSet* m_pEnforceSet;
	CFileManager* m_pFileManage;

	//char*	fileBuf;
	BYTE*	fileBuf;
	BYTE*	CarImageBuf;		//차량 이미지 JPG
	BYTE*	CarImageBuf2;		//JPG 컬러->흑백
	BYTE*	CarImageBuf_RAW;	//차량 이미지 RAW
	BYTE*	CarImageBuf_BMP;	//차량 이미지 BMP
	char	CarPlate[20];		//차량번호
	char	CarImageSize[8];	//차량 이미지 사이즈

	char	currentDirectory[128], readFileName[128], fullFilePath_Ori[128], fullFilePath_Dest[128], fullFilePath_Exception[128];

	int temp_idx;


public:
	void	Anpr_Proc_Normal();
	void	Anpr_Proc_Inspection();
	void	CalculAngle(std::vector<KIST_res>* outputs);
	BOOL	LoadDataFile_NEW(CString strFileName_Ori, CString strFileName_Dest, CString strFileName_Exception);
	void	OLD_Plate_Init(int index);
	BOOL	LoadDataFile_OLD(CString strFileName_Ori, CString strFileName_Dest, CString strFileName_Exception);
	BITMAP	GetBitMap(cv::Mat inputImage);
	void	CreateBitmapInfo(int w, int h, int bpp);
	void	conv_color2grayOne(BYTE* orgBuf, BYTE* ConvBuf, int w, int h);
	long	SizeConv(char* ImageSize);

	BYTE*	RawToBmpRotationDataBuf; //  8비트 이미지 로테이션에 필요.

	//KIST 번호인식
	const wchar_t* detPath = L"./models/detection_module_F.onnx";
	const wchar_t* car1Path = L"./models/ALPR_car1_F.onnx";
	const wchar_t* car2Path = L"./models/ALPR_car2_F.onnx";
	const wchar_t* bikePath = L"./models/ALPR_bike_F.onnx";
	const wchar_t* helmetPath = L"./models/detection_helmet_F.onnx";

	bool is_GPU = false;

	PLATEDetector* detector0{ nullptr }, * detector1{ nullptr }, * detector2{ nullptr }, * detector3{ nullptr }, * detector4{ nullptr };
	PLATEDetector* detector_test{ nullptr };

	float conf_thres0 = 0.60f;
	float conf_thres1 = 0.65f;
	float conf_thres2 = 0.65f;
	float conf_thres3 = 0.65f;
	float conf_thres4 = 0.60f;

	int	nTotalCount, nPassCount, nFailCount;

	int find_CorrectBike(cv::Rect plate_box, std::vector<Detection> bikes, cv::Point top_left);
	std::vector<KIST_res> KIST_Detection(PLATEDetector* detector0, PLATEDetector* detector1, PLATEDetector* detector2, PLATEDetector* detector3, PLATEDetector* detector4, cv::Point top_left, cv::Point bot_right,
		cv::Mat input_image, float conf_thres0, float conf_thres1, float conf_thres2, float conf_thres3, float conf_thres4);

	BOOL Read_JPG_Memory(int j_size, BYTE* jpeg_buf, int& width, int& height, int& nchannels);
	BOOL Read_JPG_Raw_Memory(int j_size, BYTE* jpeg_buf, int& width, int& height, BYTE* pixel_buf, int& nchannels);
	BOOL Convert_RawToBmp(BYTE* sourceBuf, int nWidth, int nHeight, int bitCnt, BYTE* DesBuf, int& Dessize);
	int tGetBMP(char* fn, int& w, int& h, PIXEL** img);
	int tGetBMP2(char* fn, int& w, int& h, PIXEL** img);
};


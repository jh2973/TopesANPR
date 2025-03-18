#include "KistAnprClass.h"
#include "turbojpeg.h"

#define	FILE_MAX_SIZE	2000000
#define IMAGE_MAX_SIZE	1500 * 1400 * 3

#define ImageWidth  1500
#define ImageHeight 1400

typedef struct _RECOG_PROC
{
	int w;				// 영상넓이
	int h;				// 영상높이
	int avr_w;			// 번호판 평균 너비
	RECT FIND_R;		// 번호판 탐색 영역 
	BYTE** source_img;	// 입력 영상
	BYTE** tmp_img;		// 임시 저장용 
	BYTE** cut_img;		// 번호판 탐색에 사용되는 영상
	BYTE** f_img; 		// 번호판 탐색 저장용
	int find_plate;		// 번호판 탐색 여부
	int pl_attr;		// 번호판 종류
	int pw;				// 번호판 너비
	int ph;				// 번호판 높이
	BYTE** p_img;		// 번호판 영상
	char plate_char[16];// 인식결과 
	RECT pr;			// 번호판 실제 위치
	RECT num_r;			// 번호판 수자 위치
	int gray;			// 번호판 레벨
	int bin_th;
} RECOG_PROC;
RECOG_PROC rp;

typedef struct _BMPHD
{
	long    biSize;
	long    biWidth;
	long    biHeight;
	short   biPlanes;
	short   biBitCount;
	long    biCompression;
	long    biSizeImage;
	long    biXPelsPerMeter;
	long    biYPelsPerMeter;
	long    biClrUsed;
	long    biClrImportant;
} BMPHD;

void InitFastFindPlate();
void InitPlate(int n_of_thread, int iw, int ih, char* code);
void FastRecognizePlate(
	RECOG_PROC* rp,
	int RETRY,
	int N_OF_CPLATE,
	int ROTATION,
	float x_slope,
	int VERIFY_POS,
	int RECOG_AUTOBYE,
	int thread
);
int SuccProc(char* res);

void StopThread() {}

extern "C" __declspec(dllexport) void get_image(char* fn, int* w, int* h, unsigned char** p_img);


KistAnprClass::KistAnprClass(LPVOID set, LPVOID pManage)
{
	m_pEnforceSet = (CLocalInfoSet*)set;
	m_pFileManage = (CFileManager*)pManage;

	//fileBuf = new char[FILE_MAX_SIZE];
	fileBuf = new BYTE[FILE_MAX_SIZE];
	CarImageBuf = new BYTE[IMAGE_MAX_SIZE];
	CarImageBuf_RAW = new BYTE[1500 * 1400 * 3];
	CarImageBuf_BMP = new BYTE[1500 * 1400 * 3];
	
	ZeroMemory(fileBuf, FILE_MAX_SIZE);
	ZeroMemory(CarImageBuf, IMAGE_MAX_SIZE);
	ZeroMemory(CarImageBuf_RAW, 1500 * 1400 * 3);
	ZeroMemory(CarImageBuf_BMP, 1500 * 1400 * 3);
	ZeroMemory(CarPlate, sizeof(CarPlate));
	ZeroMemory(CarImageSize, sizeof(CarImageSize));

	ZeroMemory(currentDirectory, sizeof(currentDirectory));
	ZeroMemory(readFileName, sizeof(readFileName));
	ZeroMemory(fullFilePath_Ori, sizeof(fullFilePath_Ori));
	ZeroMemory(fullFilePath_Dest, sizeof(fullFilePath_Dest));
	ZeroMemory(fullFilePath_Exception, sizeof(fullFilePath_Exception));

	wsprintf(currentDirectory, "%s", m_pEnforceSet->cOriginPath);

	detector0 = new PLATEDetector(detPath, cv::Size(640, 640), is_GPU);
	detector1 = new PLATEDetector(car1Path, cv::Size(256, 256), is_GPU);
	detector2 = new PLATEDetector(car2Path, cv::Size(256, 256), is_GPU);
	detector3 = new PLATEDetector(bikePath, cv::Size(160, 160), is_GPU);
	detector4 = new PLATEDetector(helmetPath, cv::Size(320, 320), is_GPU);
	detector_test = new PLATEDetector(detPath, cv::Size(640, 640), is_GPU);

	RawToBmpRotationDataBuf = new BYTE[1500 * 1400 * 3];

	nTotalCount = 0, nPassCount = 0, nFailCount = 0;

	temp_idx = 0;
}


KistAnprClass::~KistAnprClass()
{
	
}


void KistAnprClass::Anpr_Proc_Normal()
{
	CFileFind	finder;
	CString		file_name;

	char	logMessage[128];
	ZeroMemory(logMessage, sizeof(logMessage));

	wsprintf(currentDirectory, "%s", m_pEnforceSet->cOriginPath);
	SetCurrentDirectory(currentDirectory);
	BOOL exist = finder.FindFile("*.*");
	while (exist) {
		exist = finder.FindNextFile();
		file_name = finder.GetFileName();
		
		if(finder.IsDirectory())
			continue;

		if ((file_name != ".") && (file_name != "..")) {

			ZeroMemory(readFileName, sizeof(readFileName));
			ZeroMemory(fullFilePath_Ori, sizeof(fullFilePath_Ori));
			ZeroMemory(fullFilePath_Dest, sizeof(fullFilePath_Dest));
			ZeroMemory(fullFilePath_Exception, sizeof(fullFilePath_Exception));

			wsprintf(readFileName, "%s", (LPCTSTR)file_name);
			wsprintf(fullFilePath_Ori, "%s/%s", m_pEnforceSet->cOriginPath, readFileName);
			wsprintf(fullFilePath_Dest, "%s/%s", m_pEnforceSet->cDestPath, readFileName);
			wsprintf(fullFilePath_Exception, "%s/%s", m_pEnforceSet->cExceptionPath, readFileName);

			if (m_pEnforceSet->nANPRType == 1)	//OLD
			{
				LoadDataFile_OLD(fullFilePath_Ori, fullFilePath_Dest, fullFilePath_Exception);
			}
			if (m_pEnforceSet->nANPRType == 2)	//NEW
			{
				LoadDataFile_NEW(fullFilePath_Ori, fullFilePath_Dest, fullFilePath_Exception);
			}
			else
			{
				wsprintf(logMessage, "[Anpr_Proc_Normal] ANPR Type 확인 필요 !!\n");
				OutputDebugString(logMessage);
			}

			exist = FALSE;
		}
	}
}


void KistAnprClass::Anpr_Proc_Inspection()
{
	CFileFind	finder;
	CString		fold_name, file_name;
	CString		FullFoldName_ori[3];
	CString		FullFoldName_dest[3];
	CString		FullFoldName_exception[3];
	int			index = 0;

	char	logMessage[128];
	ZeroMemory(logMessage, sizeof(logMessage));

	wsprintf(currentDirectory, "%s", m_pEnforceSet->cOriginPath);
	SetCurrentDirectory(currentDirectory);
	BOOL exist = finder.FindFile("*.*");
	while (exist) {
		exist = finder.FindNextFile();
		fold_name = finder.GetFileName();

		if (finder.IsDirectory())
		{
			if (fold_name.Find("Vol_") != -1)
			{
				FullFoldName_ori[index].Format("%s\\%s", currentDirectory, fold_name);

				FullFoldName_dest[index].Format("%s_OK", currentDirectory);
				CreateDirectory(FullFoldName_dest[index], NULL);
				FullFoldName_dest[index].Format("%s_OK\\%s", currentDirectory, fold_name);
				CreateDirectory(FullFoldName_dest[index], NULL);

				FullFoldName_exception[index].Format("%s_NG", currentDirectory);
				CreateDirectory(FullFoldName_exception[index], NULL);
				FullFoldName_exception[index].Format("%s_NG\\%s", currentDirectory, fold_name);
				CreateDirectory(FullFoldName_exception[index], NULL);

				index++;
			}
		}
	}

	//처리중인 폴더의 총 파일 수
	int count = 0;
	for (int i = 0; i < index; i++)
	{		
		BOOL bWorking = finder.FindFile(FullFoldName_ori[i] + "/*.*");
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if (finder.IsDots())
				continue;
			count++;
		}
		finder.Close();
	}	
	nTotalCount += count;
	if (count == 0)
		nTotalCount = 0;

	for (int i = 0; i < index; i++)
	{
		SetCurrentDirectory(FullFoldName_ori[i]);
		exist = finder.FindFile("*.*");
		while (exist) {
			exist = finder.FindNextFile();
			file_name = finder.GetFileName();

			if (finder.IsDirectory())
				continue;

			if ((file_name != ".") && (file_name != "..")) {

				ZeroMemory(readFileName, sizeof(readFileName));
				ZeroMemory(fullFilePath_Ori, sizeof(fullFilePath_Ori));
				ZeroMemory(fullFilePath_Dest, sizeof(fullFilePath_Dest));
				ZeroMemory(fullFilePath_Exception, sizeof(fullFilePath_Exception));

				wsprintf(readFileName, "%s", (LPCTSTR)file_name);
				wsprintf(fullFilePath_Ori, "%s\\%s", FullFoldName_ori[i], readFileName);
				wsprintf(fullFilePath_Dest, "%s\\%s", FullFoldName_dest[i], readFileName);
				wsprintf(fullFilePath_Exception, "%s\\%s", FullFoldName_exception[i], readFileName);

				if (m_pEnforceSet->nANPRType == 1)	//OLD
				{
					LoadDataFile_OLD(fullFilePath_Ori, fullFilePath_Dest, fullFilePath_Exception);
				}
				else if (m_pEnforceSet->nANPRType == 2)	//NEW
				{
					LoadDataFile_NEW(fullFilePath_Ori, fullFilePath_Dest, fullFilePath_Exception);
				}
				else
				{
					wsprintf(logMessage, "[Anpr_Proc_Inspection] ANPR Type 확인 필요 !!\n");
					OutputDebugString(logMessage);
				}

				//exist = FALSE;
			}
		}
	}
}

void KistAnprClass::CalculAngle(std::vector<KIST_res>* outputs)
{
	for (KIST_res& res : *outputs)
	{
		if (res.word_inf.size() > 5)
		{
			int min_x = 10000, min_y = 10000;
			int max_x = -10000, max_y = -10000;

			for (Plate_Info plf : res.word_inf)
			{
				if (min_x > plf.box.x)
				{
					min_x = plf.box.x + plf.box.width / 2;
					min_y = plf.box.y + plf.box.height / 2;
				}
				if (max_x < plf.box.x)
				{
					max_x = plf.box.x + plf.box.width / 2;
					max_y = plf.box.y + plf.box.height / 2;
				}
			}
			double dx = max_x - min_x;
			double dy = max_y - min_y;
			double angle = std::atan2(dy, dx) * 180.0 / CV_PI;
			res.plate_angle = abs(angle);
		}
	}
}


BOOL KistAnprClass::LoadDataFile_NEW(CString strFileName_Ori, CString strFileName_Dest, CString strFileName_Exception)
{
	FILE* file;
	time_t	lTime = 0;
	CFile fp;
	LONG nFileSize = 0;

	char logMessage[256];
	char DisplayMsg[256];
	char JpgFileName[256];
	char cPartRecogFile[64];
	int width = 0, height = 0, nchannels = 0;
	BYTE ViolationCode = 0xff;

	ZeroMemory(logMessage, sizeof(logMessage));
	ZeroMemory(DisplayMsg, sizeof(DisplayMsg));

	try {
		if (fp.Open(strFileName_Ori, CFile::modeRead))
		{
			nFileSize = (LONG)fp.GetLength();

			if (nFileSize > FILE_MAX_SIZE)
				return FALSE;

			fp.Read(fileBuf, nFileSize);
			fp.Close();
		}
		else
			return FALSE;


		//'아' 규격
		UINT nCarImgSz = 0;
		if (m_pEnforceSet->nProtocolType == 1)
		{
			CopyMemory(&CarPlate, &fileBuf[31], 12);		
			CopyMemory(&CarImageSize, &fileBuf[113], 7);
			nCarImgSz = SizeConv(&CarImageSize[0]);

			if (nCarImgSz < 0)
			{
				wsprintf(logMessage, "[LoadDataFile_NEW] '타'규격 >>> 위반 파일 확인 필요 !!\n");
				OutputDebugString(logMessage);
				return FALSE;
			}

			CopyMemory(CarImageBuf, &fileBuf[120], nCarImgSz);
		}
		//'타' 규격
		else if (m_pEnforceSet->nProtocolType == 2)
		{
			ViolationCode = fileBuf[0];
			
			if (ViolationCode == 0x00 || ViolationCode == 0x01)	//통과차량 or 속도위반
			{
				CopyMemory(&CarPlate, &fileBuf[4], 20);				
				nCarImgSz = (fileBuf[106] << 24) + (fileBuf[107] << 16) + (fileBuf[108] << 8) + fileBuf[109];

				if (nCarImgSz < 0)
				{
					wsprintf(logMessage, "[LoadDataFile_NEW] '타'규격 >>> 0x00 || 0x01 위반 파일 확인 필요 !!\n");
					OutputDebugString(logMessage);
					return FALSE;
				}

				CopyMemory(CarImageBuf, &fileBuf[110], nCarImgSz);	
			}
			else if (ViolationCode == 0x03 || ViolationCode == 0x07)	//신호위반, 차로위반
			{
				CopyMemory(&CarPlate, &fileBuf[4], 20);
				nCarImgSz = ((UINT)fileBuf[123] << 24) + ((UINT)fileBuf[124] << 16) + ((UINT)fileBuf[125] << 8) + (UINT)fileBuf[126];

				if (nCarImgSz < 0)
				{
					wsprintf(logMessage, "[LoadDataFile_NEW] '타'규격 >>> 0x03 위반 파일 확인 필요 !!\n");
					OutputDebugString(logMessage);
					return FALSE;
				}

				CopyMemory(CarImageBuf, &fileBuf[127], nCarImgSz);

			}
			else if (ViolationCode == 0x09)	//안전모 미착용
			{
				CopyMemory(&CarPlate, &fileBuf[4], 20);
				nCarImgSz = (fileBuf[104] << 24) + (fileBuf[105] << 16) + (fileBuf[106] << 8) + fileBuf[107];

				if (nCarImgSz < 0)
				{
					wsprintf(logMessage, "[LoadDataFile_NEW] '타'규격 >>> 0x09 위반 파일 확인 필요 !!\n");
					OutputDebugString(logMessage);
					return FALSE;
				}

				CopyMemory(CarImageBuf, &fileBuf[108], nCarImgSz);
			}
		}

		//read Jpg header
		Read_JPG_Memory(nCarImgSz, CarImageBuf, width, height, nchannels);

		char	cProgramFilePath[128];
		GetCurrentDirectory(sizeof(cProgramFilePath), cProgramFilePath);

		ZeroMemory(cPartRecogFile, sizeof(cPartRecogFile));
		wsprintf(cPartRecogFile, "c://jpg");
		cPartRecogFile[0] = cProgramFilePath[0];
		CreateDirectory(cPartRecogFile, NULL);

		ZeroMemory(JpgFileName, sizeof(JpgFileName));
		wsprintf(JpgFileName, "%s/0000.jpg", cPartRecogFile);
		
		//Image File Save
		file = fopen(JpgFileName, "wb");
		if (file != NULL) {
			fwrite(CarImageBuf, nCarImgSz, 1, file);
			fclose(file);
		}
		//----------------------------------------------------//

		//TEST
		//width = 1400;
		//height = 1800;

		cv::Mat input_image0 = cv::imread(JpgFileName);
		//cv::Mat input_image0(m_pEnforceSet->nImageHeight, m_pEnforceSet->nImageWidth, CV_8UC3, CarImageBuf);
		//cv::imshow("aa", input_image0);
		//cv::waitKey(2000);

		//번호인식
		std::vector<KIST_res> output;
		std::vector<cv::Mat> plates_im;
		cv::Mat input_image;

		cv::Point top_left(0, 0);
		cv::Point bot_right(width, height);

		cv::cvtColor(input_image0, input_image, cv::COLOR_BGR2RGB);	//COLOR_GRAY2RGB

		try {
			output = KIST_Detection(
				detector0, detector1, detector2, detector3, detector4,
				top_left, bot_right, input_image,
				conf_thres0, conf_thres1, conf_thres2, conf_thres3, conf_thres4
			);

			for (int i = 0; i < output.size(); i++) {
				if ((output[i].class_box.width % 4) > 0)
				{
					int temp = output[i].class_box.width % 4;
					output[i].class_box.width = output[i].class_box.width - temp + 4;
				}
			}

			for (KIST_res res : output) {
				if (res.class_conf != -1.0f) {
					cv::Mat cropped = input_image(cv::Rect(res.class_box));
					plates_im.push_back(cropped);
				}
			}
		}
		catch (...) {

			wsprintf(DisplayMsg, "ERROR: Plate ENF ANPR proc.[choi prof.] [%s][%s]\n", (LPCTSTR)strFileName_Ori, &CarPlate[0]);
			time(&lTime);
			m_pFileManage->WriteRunnginLogMessage(lTime, DisplayMsg);
			m_pFileManage->WriteFailLogMessage(lTime, DisplayMsg);

			DeleteFile(JpgFileName);
		}

		DeleteFile(JpgFileName);

		//차량번호인식 실패 !!
		if (output.size() == 0)
		{
			nFailCount++;

			if (CopyFile(strFileName_Ori, strFileName_Exception, FALSE) != 0)
				DeleteFile(strFileName_Ori);

			wsprintf(DisplayMsg, "[NEW] ===== [FAIL] =====  차량번호인식 실패 !! : [%s][%s]\n", (LPCTSTR)strFileName_Ori, &CarPlate[0]);
			time(&lTime);
			m_pFileManage->WriteRunnginLogMessage(lTime, DisplayMsg);
			m_pFileManage->WriteFailLogMessage(lTime, DisplayMsg);

			//Image File Save
			if (m_pEnforceSet->nProtocolType == 1)
			{
				strFileName_Exception.Format("%s.jpg", strFileName_Exception);
				file = fopen(strFileName_Exception, "wb");
				if (file != NULL) {
					fwrite(CarImageBuf, nCarImgSz, 1, file);
					fclose(file);
				}
			}
			else if (m_pEnforceSet->nProtocolType == 2)
			{
				if (strFileName_Exception.Find("DAT") != -1)
				{
					strFileName_Exception.Replace("DAT", "jpg");
					file = fopen(strFileName_Exception, "wb");
					if (file != NULL) {
						fwrite(CarImageBuf, nCarImgSz, 1, file);
						fclose(file);
					}
				}
			}

			return FALSE;
		}			

		std::string oldNumber = "";
		for (int i = 0; i < 20; i++)
		{
			if (CarPlate[i] != 0x20 && CarPlate[i] != 0x00)
			{
				oldNumber += CarPlate[i];
			}
		}

		BOOL conform_plate = FALSE;	//차량번호 일치 여부
		BOOL conform_helmet = TRUE;	//헬멧 착용유무

		//TEST
		//conform_plate = TRUE;

		for (KIST_res res : output)
		{
			if (oldNumber == res.License)	//차량번호 일치
			{				
				if (ViolationCode == 0x09 && res.is_bike == -1)
				{
					conform_helmet = TRUE;
				}				
				else if (ViolationCode == 0x09 && res.helmet_inf.size() == 0)
				{
					conform_helmet = TRUE;
				}
				else if (ViolationCode == 0x09 && res.helmet_inf.size() > 0)	//안전모 미착용
				{
					for (Helmet_Info helmet : res.helmet_inf)
					{
						if (helmet.w_Helmet == "wo_Helmet")
						{
							conform_helmet = FALSE;
						}
					}
				}

				conform_plate = TRUE;
			}
		}

		if (ViolationCode == 0x09 )
		{
			if (conform_plate == TRUE && conform_helmet == FALSE)	//결과 : 안전모 미착용 --> Pass(미착용 하였으므로 위반 처리)
			{		
				nPassCount++;

				if (CopyFile(strFileName_Ori, strFileName_Dest, FALSE) != 0)
					DeleteFile(strFileName_Ori);

				if (output[0].helmet_inf.size() == 0)
				{
					wsprintf(DisplayMsg, "[NEW] 안전모 [Success] --->  FileName :[%s], PlateNum:[%s], isBike:[%d], Helmet_Size:[%d]", 
						(LPCTSTR)strFileName_Ori, oldNumber.c_str(), output[0].is_bike, output[0].helmet_inf.size());
				}					
				else if (output[0].helmet_inf.size() == 1)
				{
					wsprintf(DisplayMsg, "[NEW] 안전모 [Success] --->  FileName :[%s], PlateNum:[%s], isBike:[%d], Helmet_Size:[%d], Helmet1:[%s]",
						(LPCTSTR)strFileName_Ori, oldNumber.c_str(), output[0].is_bike, output[0].helmet_inf.size(), output[0].helmet_inf[0].w_Helmet.c_str());
				}
				else if (output[0].helmet_inf.size() == 2)
				{
					wsprintf(DisplayMsg, "[NEW] 안전모 [Success] --->  FileName :[%s], PlateNum:[%s], isBike:[%d], Helmet_Size:[%d], Helmet1:[%s], Helmet2:[%s]",
						(LPCTSTR)strFileName_Ori, oldNumber.c_str(), output[0].is_bike, output[0].helmet_inf.size(), output[0].helmet_inf[0].w_Helmet.c_str(), output[0].helmet_inf[1].w_Helmet.c_str());
				}

				//Image File Save
				if (m_pEnforceSet->nTestMode == 1)
				{
					if (strFileName_Dest.Find("DAT") != -1)
					{
						strFileName_Dest.Replace("DAT", "jpg");
						file = fopen(strFileName_Dest, "wb");
						if (file != NULL) {
							fwrite(CarImageBuf, nCarImgSz, 1, file);
							fclose(file);
						}
					}
				}
			}
			else //결과 : 안전모 착용 --> NG (착용하였으므로 예외 파일로 처리)
			{
				nFailCount++;

				if (CopyFile(strFileName_Ori, strFileName_Exception, FALSE) != 0)
					DeleteFile(strFileName_Ori);

				if (output[0].helmet_inf.size() == 0)
				{
					wsprintf(DisplayMsg, "[NEW] 안전모 [Fail] --->  FileName :[%s], PlateNum:[%s], isBike:[%d], Helmet_Size:[%d]",
						(LPCTSTR)strFileName_Ori, oldNumber.c_str(), output[0].is_bike, output[0].helmet_inf.size());
				}
				else if (output[0].helmet_inf.size() == 1)
				{
					wsprintf(DisplayMsg, "[NEW] 안전모 [Fail] --->  FileName :[%s], PlateNum:[%s], isBike:[%d], Helmet_Size:[%d], Helmet1:[%s]",
						(LPCTSTR)strFileName_Ori, oldNumber.c_str(), output[0].is_bike, output[0].helmet_inf.size(), output[0].helmet_inf[0].w_Helmet.c_str());
				}
				else if (output[0].helmet_inf.size() == 2)
				{
					wsprintf(DisplayMsg, "[NEW] 안전모 [Fail] --->  FileName :[%s], PlateNum:[%s], isBike:[%d], Helmet_Size:[%d], Helmet1:[%s], Helmet2:[%s]",
						(LPCTSTR)strFileName_Ori, oldNumber.c_str(), output[0].is_bike, output[0].helmet_inf.size(), output[0].helmet_inf[0].w_Helmet.c_str(), output[0].helmet_inf[1].w_Helmet.c_str());
				}

				time(&lTime);
				m_pFileManage->WriteFailLogMessage(lTime, DisplayMsg);

				//Image File Save
				if (m_pEnforceSet->nProtocolType == 2)
				{
					if (strFileName_Exception.Find("DAT") != -1)
					{
						strFileName_Exception.Replace("DAT", "jpg");
						file = fopen(strFileName_Exception, "wb");
						if (file != NULL) {
							fwrite(CarImageBuf, nCarImgSz, 1, file);
							fclose(file);
						}
					}
				}
			}
		}
		else if (conform_plate == TRUE)	//차량번호 일치
		{	
			//번호판 기울기 값을 이용한 필터링 기능
			if (m_pEnforceSet->nPlateAngle != 0 && output[0].plate_angle >= m_pEnforceSet->nPlateAngle)
			{
				nFailCount++;

				if (CopyFile(strFileName_Ori, strFileName_Exception, FALSE) != 0)
					DeleteFile(strFileName_Ori);

				sprintf(DisplayMsg, "[NEW] ===== [FAIL] =====  FileName:[%s], Old:[%s] -> New:[%s], PLATE_ANGLE:[%.2f]",
					(LPCTSTR)strFileName_Ori, oldNumber.c_str(), output[0].License.c_str(), output[0].plate_angle);

				time(&lTime);
				m_pFileManage->WriteFailLogMessage(lTime, DisplayMsg);

				//Image File Save
				if (m_pEnforceSet->nProtocolType == 1)
				{
					strFileName_Exception.Format("%s.jpg", strFileName_Exception);
					file = fopen(strFileName_Exception, "wb");
					if (file != NULL) {
						fwrite(CarImageBuf, nCarImgSz, 1, file);
						fclose(file);
					}
				}					
				else if (m_pEnforceSet->nProtocolType == 2)
				{
					if (strFileName_Exception.Find("DAT") != -1)
					{
						strFileName_Exception.Replace("DAT", "jpg");		
						file = fopen(strFileName_Exception, "wb");
						if (file != NULL) {
							fwrite(CarImageBuf, nCarImgSz, 1, file);
							fclose(file);
						}
					}
				}				
			}
			else
			{
				nPassCount++;

				if (CopyFile(strFileName_Ori, strFileName_Dest, FALSE) != 0)
					DeleteFile(strFileName_Ori);

				wsprintf(DisplayMsg, "[NEW] Success... FileName :[%s], PlateNum:[%s]",
					(LPCTSTR)strFileName_Ori, oldNumber.c_str());
			}			
		}
		else //차량번호 미일치
		{			
			nFailCount++;

			if (CopyFile(strFileName_Ori, strFileName_Exception, FALSE) != 0)
				DeleteFile(strFileName_Ori);

			wsprintf(DisplayMsg, "[NEW] ===== [FAIL] =====  FileName:[%s], Old:[%s] -> New:[%s]", 
				(LPCTSTR)strFileName_Ori, oldNumber.c_str(), output[0].License.c_str());

			time(&lTime);
			m_pFileManage->WriteFailLogMessage(lTime, DisplayMsg);

			//Image File Save			
			if (m_pEnforceSet->nProtocolType == 1)
			{
				strFileName_Exception.Format("%s.jpg", strFileName_Exception);
				file = fopen(strFileName_Exception, "wb");
				if (file != NULL) {
					fwrite(CarImageBuf, nCarImgSz, 1, file);
					fclose(file);
				}
			}				
			else if (m_pEnforceSet->nProtocolType == 2)
			{
				if (strFileName_Exception.Find("DAT") != -1)
				{
					strFileName_Exception.Replace("DAT", "jpg");					
					file = fopen(strFileName_Exception, "wb");
					if (file != NULL) {
						fwrite(CarImageBuf, nCarImgSz, 1, file);
						fclose(file);
					}
				}				
			}
		}

		time(&lTime);
		m_pFileManage->WriteRunnginLogMessage(lTime, DisplayMsg);

		return TRUE;
	}
	catch (...) {

		wsprintf(DisplayMsg, "[Try~catch] ERROR: LoadDataFile_NEW() : [%s]\n", (LPCTSTR)strFileName_Ori);
		time(&lTime);
		m_pFileManage->WriteRunnginLogMessage(lTime, DisplayMsg);
		m_pFileManage->WriteFailLogMessage(lTime, DisplayMsg);

		if (file != NULL)
			fclose(file);

		DeleteFile(JpgFileName);
	}
}


void KistAnprClass::OLD_Plate_Init(int index)
{
	rp.source_img = cmatrix(ImageWidth, ImageWidth * 3);
	rp.f_img = cmatrix(ImageWidth, ImageHeight);
	rp.cut_img = cmatrix(ImageWidth, ImageHeight);
	rp.tmp_img = cmatrix(ImageWidth + 400, ImageHeight + 400);   //motion

	rp.p_img = new BYTE * [700];
	rp.p_img = rp.p_img + 100;

	int i;
	for (i = -100; i < 600; i++) {
		rp.p_img[i] = new BYTE[700];
		rp.p_img[i] = rp.p_img[i] + 100;
	}

	char code[80];
	sprintf(code, "topes.com1");

	if (index == 0) {
		InitFastFindPlate();
		InitPlate(1, ImageWidth, ImageHeight, code);
	}
}


BOOL KistAnprClass::LoadDataFile_OLD(CString strFileName_Ori, CString strFileName_Dest, CString strFileName_Exception)
{
	CFile fp;
	LONG nFileSize = 0;
	FILE* file;
	time_t	lTime = 0;

	char logMessage[128];
	char DisplayMsg[128];
	char BmpFileName[256];
	char cPartRecogFile[64];
	int width = 0, height = 0, nchannels = 0;

	BYTE ViolationCode = 0xff;

	ZeroMemory(logMessage, sizeof(logMessage));
	ZeroMemory(DisplayMsg, sizeof(DisplayMsg));

	try {
		if (fp.Open(strFileName_Ori, CFile::modeRead))
		{
			nFileSize = (LONG)fp.GetLength();

			if (nFileSize > FILE_MAX_SIZE)
				return FALSE;

			fp.Read(fileBuf, nFileSize);
			fp.Close();
		}
		else
			return FALSE;

		//'아' 규격
		UINT nCarImgSz = 0;
		if (m_pEnforceSet->nProtocolType == 1)
		{
			CopyMemory(&CarPlate, &fileBuf[31], 12);
			CopyMemory(&CarImageSize, &fileBuf[113], 7);
			nCarImgSz = SizeConv(&CarImageSize[0]);

			if (nCarImgSz < 0)
			{
				wsprintf(logMessage, "[LoadDataFile_OLD] '아'규격 이미지 사이즈 확인 필요 !!\n");
				OutputDebugString(logMessage);
			}

			CopyMemory(CarImageBuf, &fileBuf[120], nCarImgSz);
		}
		//'타' 규격
		else if (m_pEnforceSet->nProtocolType == 2)
		{
			ViolationCode = fileBuf[0];

			if (ViolationCode == 0x00 || ViolationCode == 0x01)	//통과차량 or 속도위반
			{
				CopyMemory(&CarPlate, &fileBuf[4], 20);
				nCarImgSz = (fileBuf[106] << 24) + (fileBuf[107] << 16) + (fileBuf[108] << 8) + fileBuf[109];

				if (nCarImgSz < 0)
				{
					wsprintf(logMessage, "[LoadDataFile_OLD] '타'규격 >>> 0x00 || 0x01 위반 파일 확인 필요 !!\n");
					OutputDebugString(logMessage);
					return FALSE;
				}

				CopyMemory(CarImageBuf, &fileBuf[110], nCarImgSz);
			}
			else if (ViolationCode == 0x03 || ViolationCode == 0x07)	//신호위반, 차로위반
			{
				CopyMemory(&CarPlate, &fileBuf[4], 20);
				nCarImgSz = ((UINT)fileBuf[123] << 24) + ((UINT)fileBuf[124] << 16) + ((UINT)fileBuf[125] << 8) + (UINT)fileBuf[126];

				if (nCarImgSz < 0)
				{
					wsprintf(logMessage, "[LoadDataFile_OLD] '타'규격 >>> 0x03 || 0x07 위반 파일 확인 필요 !!\n");
					OutputDebugString(logMessage);
					return FALSE;
				}

				CopyMemory(CarImageBuf, &fileBuf[127], nCarImgSz);
			}
		}

		ZeroMemory(CarImageBuf_RAW, 1500 * 1400 * 3);
		ZeroMemory(CarImageBuf_BMP, 1500 * 1400 * 3);
		//Read_JPG_Raw_Memory(nCarImgSz, CarImageBuf, width, height, CarImageBuf_RAW, nchannels);

		//테스트1
		//strFileName_Exception.Replace("DAT", "jpg");
		//file = fopen(strFileName_Exception, "wb");
		//if (file != NULL) {
		//	fwrite(CarImageBuf, nCarImgSz, 1, file);
		//	fclose(file);
		//}
		////Color to Gray
		//conv_color2grayOne(CarImageBuf_BMP, CarImageBuf_RAW, 1500, 1400);		
		
		char	cProgramFilePath[128];
		GetCurrentDirectory(sizeof(cProgramFilePath), cProgramFilePath);

		ZeroMemory(cPartRecogFile, sizeof(cPartRecogFile));
		wsprintf(cPartRecogFile, "c://jpg");
		cPartRecogFile[0] = cProgramFilePath[0];
		CreateDirectory(cPartRecogFile, NULL);

		ZeroMemory(BmpFileName, sizeof(BmpFileName));
		wsprintf(BmpFileName, "%s/0000.jpg", cPartRecogFile);
		//wsprintf(BmpFileName, "%s/jpg/%04d.jpg", cPartRecogFile, temp_idx++);
		file = fopen(BmpFileName, "wb");
		if (file != NULL) {
			fwrite(CarImageBuf, nCarImgSz, 1, file);
			fclose(file);
		}		

		//TEST
		cv::Mat img1 = cv::imread(BmpFileName);
		cv::Mat img2;
		cv::cvtColor(img1, img2, cv::COLOR_BGR2GRAY);
		//cv::imshow("111", img2);
		//cv::waitKey();
		//cv::destroyAllWindows();
		// 
		int width = img2.cols;
		int height = img2.rows;
		int bpp = img2.channels();
		int imagesize = width * height * bpp;

		std::vector<int> params_jpg;
		params_jpg.push_back(cv::IMWRITE_JPEG_QUALITY);
		params_jpg.push_back(95);
		//wsprintf(BmpFileName, "%s/jpg/%04d.jpg", cPartRecogFile, temp_idx++);
		cv::imwrite(BmpFileName, img2, params_jpg);	//흑백 JPG 저장

		//DeleteFile(strFileName_Ori);
		//return FALSE;

		//FILE* fp = fopen(BmpFileName, "rb");
		//fseek(fp, 0, SEEK_END);
		//nCarImgSz = ftell(fp);
		//fclose(fp);

		//FILE* fp1 = fopen(BmpFileName, "rb");
		//int totalFileSize = fread(CarImageBuf, sizeof(char), nCarImgSz, fp1);
		//fclose(fp);

		//Read_JPG_Raw_Memory(nCarImgSz, CarImageBuf, width, height, CarImageBuf_RAW, nchannels);

		////이 부분 검증 필요함.
		//if (m_pEnforceSet->nProtocolType == 1)
		//	nchannels = 1;

		cv::Mat matImg;
		matImg = cv::imdecode(cv::Mat(1, nCarImgSz, CV_8UC1, CarImageBuf), cv::IMREAD_GRAYSCALE);
		width = matImg.cols;
		height = matImg.rows;
		bpp = matImg.channels();
		imagesize = width * height * bpp;
		//cv::imshow("111", matImg);
		//cv::waitKey();
		//cv::destroyAllWindows();
		nchannels = 1;

		int sizea = 0;
		Convert_RawToBmp(matImg.data, width, height, 8 * nchannels, CarImageBuf_BMP, sizea);
		//Convert_RawToBmp(CarImageBuf_RAW, width, height, 8 * nchannels, CarImageBuf_BMP, sizea);
		//Convert_RawToBmp(img2.data, width, height, 8 * bpp, CarImageBuf_BMP, sizea);

		//TEST
		//cv::Mat matImg2 (height, width, CV_8UC3, CarImageBuf_BMP);
		//width = matImg2.cols;
		//height = matImg2.rows;
		//bpp = matImg2.channels();
		//imagesize = width * height * bpp;
		//cv::imshow("222", matImg2);
		//cv::waitKey();
		//cv::destroyAllWindows();

		ZeroMemory(BmpFileName, sizeof(BmpFileName));
		wsprintf(BmpFileName, "%s/0000.bmp", cPartRecogFile);
		file = fopen(BmpFileName, "wb");
		if (file != NULL) {
			fwrite(CarImageBuf_BMP, sizea, 1, file);
			fclose(file);
		}

		//cv::rotate(matImg, matImg, cv::ROTATE_180);
		//cv::flip(matImg, matImg, 1);
		//cv::imwrite(BmpFileName, matImg);

		//if (tGetBMP(BmpFileName, rp.w, rp.h, rp.source_img) != 0)
		get_image(BmpFileName, &rp.w, &rp.h, rp.source_img);
		//ZeroMemory(rp.source_img[0], width * height);
		//CopyMemory(rp.source_img[0], matImg.data, width * height);
		//tGetBMP(BmpFileName, rp.w, rp.h, rp.source_img);
		{
			rp.w = width;
			rp.h = height;
			rp.avr_w = 156;
			rp.FIND_R.left = 0;
			rp.FIND_R.top = 0;
			rp.FIND_R.right = rp.w;
			rp.FIND_R.bottom = rp.h;

			Rect(rp.FIND_R.left / 2, rp.FIND_R.top / 2,
				rp.FIND_R.right / 2, rp.FIND_R.bottom / 2, YELLOW);

			FastRecognizePlate(
				&rp,// 인식용 데이터
				2,	// RETRY 여부
				5,  // 번호판 후보 갯수
				0,  // y방향 기울어진 번호판 처리 여부
				0,	// x방향 기울어진 정도
				0,	// 번호판 내 숫자 위치 보정
				0,	// 오토바이 인식 여부
				0	// thread index
			);
		}

		if (SuccProc(rp.plate_char) == TRUE) {
			char	regionName[8];
			ZeroMemory(regionName, sizeof(regionName));
			CopyMemory(regionName, rp.plate_char, 4);
			int result = _stricmp(regionName, "전국");
			if (result == 0) FillMemory(rp.plate_char, 4, 0x20);
		}
		else //번호인식 실패
		{
			if (CopyFile(strFileName_Ori, strFileName_Exception, FALSE) != 0)
				DeleteFile(strFileName_Ori);
		
			wsprintf(DisplayMsg, "[OLD] ===== [번호인식 실패] ===== FileName :[%s][%s]\n", (LPCTSTR)strFileName_Ori, rp.plate_char);

			time(&lTime);
			m_pFileManage->WriteRunnginLogMessage(lTime, DisplayMsg);
			m_pFileManage->WriteFailLogMessage(lTime, DisplayMsg);

			//Image File Save			
			strFileName_Exception.Format("%s.jpg", strFileName_Exception);
			file = fopen(strFileName_Exception, "wb");
			if (file != NULL) {
				fwrite(CarImageBuf, nCarImgSz, 1, file);
				fclose(file);
			}

			nFailCount++;

			return TRUE;
		}

		//위반파일 차량번호
		std::string oldNumber = "";
		for (int i = 0; i < 20; i++)
		{
			if (CarPlate[i] != 0x20 && CarPlate[i] != 0x00)
			{
				oldNumber += CarPlate[i];
			}
		}

		//지역코드 Type 번호인식 차량번호
		std::string newNumber = "";
		for (int i = 0; i < 16; i++)
		{
			if (rp.plate_char[i] == 0)
				break;

			if (rp.plate_char[i] != 0x20 && rp.plate_char[i] != 0)
			{
				newNumber += rp.plate_char[i];
			}				
		}

		if (oldNumber == newNumber)
		{
			nPassCount++;

			if (CopyFile(strFileName_Ori, strFileName_Dest, FALSE) != 0)
				DeleteFile(strFileName_Ori);

			wsprintf(DisplayMsg, "[OLD] Success... FileName :[%s], PlateNum:[%s]\n",
				(LPCTSTR)strFileName_Ori, oldNumber.c_str());
		}
		else
		{
			nFailCount++;

			if (CopyFile(strFileName_Ori, strFileName_Exception, FALSE) != 0)
				DeleteFile(strFileName_Ori);

			wsprintf(DisplayMsg, "[OLD] ===== [FAIL] =====  FileName :[%s], Old :[%s] -> New :[%s]\n",
				(LPCTSTR)strFileName_Ori, oldNumber.c_str(), newNumber.c_str());

			time(&lTime);
			m_pFileManage->WriteFailLogMessage(lTime, DisplayMsg);

			//Image File Save			
			strFileName_Exception.Format("%s.jpg", strFileName_Exception);
			file = fopen(strFileName_Exception, "wb");
			if (file != NULL) {
				fwrite(CarImageBuf, nCarImgSz, 1, file);
				fclose(file);
			}
		}

		time(&lTime);
		m_pFileManage->WriteRunnginLogMessage(lTime, DisplayMsg);

		return TRUE;
	}
	catch (...) {

		wsprintf(DisplayMsg, "ERROR: LoadDataFile_OLD() : [%s]\n", (LPCTSTR)strFileName_Ori);
		time(&lTime);
		m_pFileManage->WriteRunnginLogMessage(lTime, DisplayMsg);
		m_pFileManage->WriteFailLogMessage(lTime, DisplayMsg);

		if (file != NULL)
			fclose(file);
	}
}


//BITMAP KistAnprClass::GetBitMap(cv::Mat inputImage)
//{
//	cv::Size size = inputImage.size();
//	BITMAP bitmap(size.width, size.height, inputImage.step1(), PixelFormat24bppRGB, inputImage.data);
//
//	return bitmap;
//}


void KistAnprClass::CreateBitmapInfo(int w, int h, int bpp)
{
	BITMAPINFO* m_pBitmapInfo;
	if (m_pBitmapInfo != NULL)
	{
		delete m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}

	if (bpp == 8)
		m_pBitmapInfo = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
	else // 24 or 32bit
		m_pBitmapInfo = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO)];

	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = bpp;
	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = 0;
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	if (bpp == 8)
	{
		for (int i = 0; i < 256; i++)
		{
			m_pBitmapInfo->bmiColors[i].rgbBlue = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbGreen = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbRed = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbReserved = 0;
		}
	}

	m_pBitmapInfo->bmiHeader.biWidth = w;
	m_pBitmapInfo->bmiHeader.biHeight = -h;
}


void KistAnprClass::conv_color2grayOne(BYTE* orgBuf, BYTE* ConvBuf, int w, int h)
{
	int	 image_w = w;
	int  image_h = h;

	int		hidth = image_h - 1;
	for (int i = 0; i < image_h; i++) {
		for (int j = 0; j < image_w; j++)
			ConvBuf[(hidth - i) * image_w + j] = 0.299 * orgBuf[i * 3 * image_w + 3 * j] +
			0.587 * orgBuf[i * 3 * image_w + 3 * j + 1] +
			0.114 * orgBuf[i * 3 * image_w + 3 * j + 2];
	}
}


long KistAnprClass::SizeConv(char* ImageSize)
{
	long nReturnSize = 0;
	nReturnSize += (ImageSize[0] - 48) * 1000000;
	nReturnSize += (ImageSize[1] - 48) * 100000;
	nReturnSize += (ImageSize[2] - 48) * 10000;
	nReturnSize += (ImageSize[3] - 48) * 1000;
	nReturnSize += (ImageSize[4] - 48) * 100;
	nReturnSize += (ImageSize[5] - 48) * 10;
	nReturnSize += (ImageSize[6] - 48);

	return nReturnSize;
}


std::vector<KIST_res> KistAnprClass::KIST_Detection(PLATEDetector* detector0, PLATEDetector* detector1, PLATEDetector* detector2, PLATEDetector* detector3, PLATEDetector* detector4, cv::Point top_left, cv::Point bot_right,
	cv::Mat input_image, float conf_thres0, float conf_thres1, float conf_thres2, float conf_thres3, float conf_thres4) {
	
	cv::Point Top_left(0, 0);
	std::vector<Detection> result;
	result = detector0->detect(input_image, conf_thres0, 0.45, 32);

	std::vector<Detection> res_bike, res_car1_plate, res_car2_plate, res_bike_plate;
	std::vector<KIST_res> output_vis, output;

	std::vector<int> bike_indexes;
	int bike_index = 0;
	for (Detection det : result) {
		if (det.classId == 0) {			// Bike
			res_bike.emplace_back(det);
			bike_indexes.push_back(bike_index);
			bike_index += 1;
		}
		else if (det.classId == 1) {    // Car1 plate
			res_car1_plate.emplace_back(det);
		}
		else if (det.classId == 2) {    // Car2 plate
			res_car2_plate.emplace_back(det);
		}
		else if (det.classId == 3) {    // Bike plate
			res_bike_plate.emplace_back(det);
		}
	}

	for (Detection det : res_car1_plate) {
		cv::Mat crop = input_image(cv::Rect(det.box));
		std::vector<Detection> result2 = detector1->detect(crop, conf_thres1, 0.45, 32);

		KIST_res out = utils::plate_sort2(result2, 0, det, Top_left, crop);
		float x_pos = out.class_box.x + out.class_box.width / 2;
		float y_pos = out.class_box.y + out.class_box.height / 2;
		if ((x_pos >= top_left.x && x_pos <= bot_right.x) && (y_pos >= top_left.y && y_pos <= bot_right.y)) {
			output.emplace_back(out);
		}
	}

	for (Detection det : res_car2_plate) {
		cv::Mat crop = input_image(cv::Rect(det.box));
		std::vector<Detection> result2 = detector2->detect(crop, conf_thres2, 0.45, 32);
		KIST_res out = utils::plate_sort2(result2, 1, det, Top_left, crop);
		float x_pos = out.class_box.x + out.class_box.width / 2;
		float y_pos = out.class_box.y + out.class_box.height / 2;
		if ((x_pos >= top_left.x && x_pos <= bot_right.x) && (y_pos >= top_left.y && y_pos <= bot_right.y)) {
			output.emplace_back(out);
		}
	}

	for (Detection det : res_bike_plate) {
		cv::Mat crop = input_image(cv::Rect(det.box));
		std::vector<Detection> result2 = detector3->detect(crop, conf_thres3, 0.45, 32);
		KIST_res out = utils::plate_sort2(result2, 2, det, Top_left, crop);

		int ii_bike = find_CorrectBike(out.class_box, res_bike, Top_left);

		bike_indexes.erase(std::remove(bike_indexes.begin(), bike_indexes.end(), ii_bike), bike_indexes.end());
		if (ii_bike != -1) {
			out.is_bike = 1;
			out.bike_box = cv::Rect(res_bike[ii_bike].box.x + Top_left.x, res_bike[ii_bike].box.y + Top_left.y, res_bike[ii_bike].box.width, res_bike[ii_bike].box.height);
			out.bike_conf = res_bike[ii_bike].conf;

			float m_y_pos = float(res_bike[ii_bike].box.height) / 2; //((float)res_bike[ii_bike].box.y) + ((float)res_bike[ii_bike].box.height) / 2;
			cv::Mat crop_bike = input_image(cv::Rect(res_bike[ii_bike].box));
			std::vector<Detection> result3 = detector4->detect(crop_bike, conf_thres4, 0.45, 32);

			std::vector<Helmet_Info> helmets;
			for (Detection h_det : result3) {
				float h_y_pos = ((float)h_det.box.y) + ((float)h_det.box.height) / 2;
				if (h_y_pos < m_y_pos) {
					Helmet_Info helmet;
					helmet.helmet_box = cv::Rect(h_det.box.x + res_bike[ii_bike].box.x + Top_left.x, h_det.box.y + res_bike[ii_bike].box.y + Top_left.y, h_det.box.width, h_det.box.height);
					helmet.helmet_conf = h_det.conf;
					if (h_det.classId == 0)
						helmet.w_Helmet = "w_Helmet";
					else
						helmet.w_Helmet = "wo_Helmet";
					helmets.push_back(helmet);
				}
			}
			out.helmet_inf = helmets;
		}
		else {
			out.is_bike = -1;
			out.bike_conf = -1.0f;
		}
		float x_pos = out.class_box.x + out.class_box.width / 2;
		float y_pos = out.class_box.y + out.class_box.height / 2;
		if ((x_pos >= top_left.x && x_pos <= bot_right.x) && (y_pos >= top_left.y && y_pos <= bot_right.y)) {
			output.emplace_back(out);
		}
	}

	for (int wo_plate_bike : bike_indexes) {
		KIST_res out;
		out.class_ID = -1;
		out.Category = "None";
		out.class_conf = -1.0f;
		out.License = "None";
		out.is_bike = 1;
		out.bike_box = cv::Rect(res_bike[wo_plate_bike].box.x + Top_left.x, res_bike[wo_plate_bike].box.y + Top_left.y, res_bike[wo_plate_bike].box.width, res_bike[wo_plate_bike].box.height);
		out.bike_conf = res_bike[wo_plate_bike].conf;
		float m_y_pos = ((float)res_bike[wo_plate_bike].box.height) / 2; //((float)res_bike[wo_plate_bike].box.y) + ((float)res_bike[wo_plate_bike].box.height) / 2;
		cv::Mat crop_bike = input_image(cv::Rect(res_bike[wo_plate_bike].box));
		std::vector<Detection> result3 = detector4->detect(crop_bike, conf_thres4, 0.45, 32);

		std::vector<Helmet_Info> helmets;

		for (Detection h_det : result3) {
			float h_y_pos = ((float)h_det.box.y) + ((float)h_det.box.height) / 2;
			if (h_y_pos < m_y_pos) {
				Helmet_Info helmet;
				helmet.helmet_box = cv::Rect(h_det.box.x + res_bike[wo_plate_bike].box.x + Top_left.x, h_det.box.y + res_bike[wo_plate_bike].box.y + Top_left.y, h_det.box.width, h_det.box.height);
				helmet.helmet_conf = h_det.conf;
				if (h_det.classId == 0)
					helmet.w_Helmet = "w_Helmet";
				else
					helmet.w_Helmet = "wo_Helmet";
				helmets.push_back(helmet);
			}
		}
		out.helmet_inf = helmets;
		float x_pos = out.class_box.x + out.class_box.width / 2;
		float y_pos = out.class_box.y + out.class_box.height / 2;
		if ((x_pos >= top_left.x && x_pos <= bot_right.x) && (y_pos >= top_left.y && y_pos <= bot_right.y)) {
			output.emplace_back(out);
		}
	}

	CalculAngle(&output);

	return output;
}


int KistAnprClass::find_CorrectBike(cv::Rect plate_box, std::vector<Detection> bikes, cv::Point top_left) {
	int index = -1;

	float thres_area = -1.0f;
	for (int i = 0; i < bikes.size(); i++) {
		cv::Rect bike_box(bikes[i].box.x + top_left.x, bikes[i].box.y + top_left.y, bikes[i].box.width, bikes[i].box.height);
		cv::Rect intersection = plate_box & bike_box;

		if (intersection.area() > 0.8)
			index = i;
	}

	return index;
}


BOOL KistAnprClass::Read_JPG_Memory(int j_size, BYTE* jpeg_buf, int& width, int& height, int& nchannels)
{
	tjhandle _jpegDecompressor = tjInitDecompress();

	tjDecompressHeader2(_jpegDecompressor, jpeg_buf, j_size, &width, &height, &nchannels);

	tjDestroy(_jpegDecompressor);

	return(TRUE);
}


BOOL KistAnprClass::Read_JPG_Raw_Memory(int j_size, BYTE* jpeg_buf,	int& width, int& height, BYTE* pixel_buf, int& nchannels)
{
	tjhandle _jpegDecompressor = tjInitDecompress();

	tjDecompressHeader2(_jpegDecompressor, jpeg_buf, j_size, &width, &height, &nchannels);

	if (m_pEnforceSet->nProtocolType == 2)
		tjDecompress2(_jpegDecompressor, jpeg_buf, j_size, pixel_buf, width, 0/*pitch*/, height, TJPF_BGR, TJFLAG_FASTDCT);
	else
		tjDecompress2(_jpegDecompressor, jpeg_buf, j_size, pixel_buf, width, 0/*pitch*/, height, TJPF_GRAY, TJFLAG_FASTDCT);	

	tjDestroy(_jpegDecompressor);

	return(TRUE);
}


#define WIDTHBYTES(bits)   (((bits)+31)/32*4)
//BYTE* RawToBmpRotationDataBuf; //  8비트 이미지 로테이션에 필요.

BOOL KistAnprClass::Convert_RawToBmp(BYTE* sourceBuf, int nWidth, int nHeight, int bitCnt, BYTE* DesBuf, int& Dessize)
{
	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
	RGBQUAD rgb[256];

	memset(&fh, 0, sizeof(BITMAPFILEHEADER));
	memset(&ih, 0, sizeof(BITMAPINFOHEADER));
	memset(&rgb, 0, sizeof(RGBQUAD) * 256);

	fh.bfType = 19778;
	if (bitCnt == 24)
		fh.bfSize = (nWidth * nHeight * 3) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // BGR 일때
	else if (bitCnt == 8)
		fh.bfSize = (nWidth * nHeight) + (sizeof(RGBQUAD) * 256) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	else 
		return false;

	ih.biSize = sizeof(BITMAPINFOHEADER);
	ih.biWidth = nWidth;
	ih.biHeight = nHeight;
	ih.biPlanes = 1;
	ih.biBitCount = bitCnt; // BGR = 24, MONO8 = 8

	if (bitCnt == 8) {
		for (int i = 0; i < 256; i++)
		{
			rgb[i].rgbBlue = i;
			rgb[i].rgbGreen = i;
			rgb[i].rgbRed = i;
			rgb[i].rgbReserved = 0;
		}
	}

	int rwsize = WIDTHBYTES(ih.biBitCount * ih.biWidth);

	int rawSize = 0;
	if (bitCnt == 8) 
		rawSize = nWidth * nHeight;
	else if (bitCnt == 24) 
		rawSize = nWidth * nHeight * 3;

	if (bitCnt == 24) {
		int aarwsize = WIDTHBYTES(ih.biBitCount * nWidth);
		for (int i = 0; i < nHeight / 2; i++) {
			for (int j = 0; j < ih.biWidth * 3; j++) {
				char temp = sourceBuf[i * aarwsize + j];
				sourceBuf[i * aarwsize + j] = sourceBuf[(nHeight - i - 1) * aarwsize + j];
				sourceBuf[(nHeight - i - 1) * aarwsize + j] = temp;
			}
		}
	}
	else if (bitCnt == 8) {
		ZeroMemory(RawToBmpRotationDataBuf, 1500 * 1400);
		for (int i = 0; i < ih.biHeight; i++)
		{
			for (int j = 0; j < rwsize; j++)
			{
				RawToBmpRotationDataBuf[i * rwsize + j] = sourceBuf[(ih.biHeight - i - 1) * rwsize + j];
			}
		}
	}

	if (bitCnt == 8)
	{
		int len = nWidth * nHeight;
		Dessize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256) + len;
	}
	else if (bitCnt == 24)
	{
		int len = nWidth * nHeight * 3;
		Dessize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + len;
	}

	long idx = 0;

	CopyMemory(&DesBuf[idx], &fh, sizeof(BITMAPFILEHEADER));
	idx = sizeof(BITMAPFILEHEADER);

	CopyMemory(&DesBuf[idx], &ih, sizeof(BITMAPINFOHEADER));
	idx += sizeof(BITMAPINFOHEADER);

	if (bitCnt == 8) {
		CopyMemory(&DesBuf[idx], rgb, sizeof(RGBQUAD) * 256);
		idx += (sizeof(RGBQUAD) * 256);

		int len = nWidth * nHeight;
		CopyMemory(&DesBuf[idx], RawToBmpRotationDataBuf, len);
	}
	else if (bitCnt == 24) {
		int len = nWidth * nHeight * 3;
		CopyMemory(&DesBuf[idx], sourceBuf, len);
	}

	if (*DesBuf == NULL) {
		return false;
	}

	return true;
}


int KistAnprClass::tGetBMP(char* fn, int& w, int& h, PIXEL** img)
{
	BMPHD buf;

	if (strcmp(fn, "") == 0) return 0;

	FILE* fp = fopen(fn, "rb");
	if (!fp) return 0;

	fseek(fp, 14L, SEEK_SET);
	fread(&buf, sizeof(BMPHD), 1, fp);
	fseek(fp, 1024L, SEEK_CUR);

	//h = buf.biHeight-256;
	int oh = buf.biHeight;  //oh = 1524;
	int ow = buf.biWidth;

	if (ow > 4096 || oh > 4096) {
		printf("input image size error");
		exit(0);
	}

	w = ow;
	h = oh;
	if (w > ImageWidth) w = ImageWidth;
	if (h > ImageHeight) h = ImageHeight;

	BYTE ibuf[10000];
	for (int i = h - 1; i >= 0; i--) {
		fread(ibuf, ow, 1, fp);
		for (int j = 0; j < w; j++)
			img[i][j] = ibuf[j];
	}

	fclose(fp);

	return 1;
}
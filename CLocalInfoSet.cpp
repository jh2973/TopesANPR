#include "CLocalInfoSet.h"
#include "define.h"


char	cSettingFile[128];
CLocalInfoSet::CLocalInfoSet()
{
	//char	cSettingFile[128];

	GetCurrentDirectory(sizeof(cSettingFile), cSettingFile);
	wsprintf(cSettingFile, "%s", CONFIG_FILE);
	cSettingFile[0] = 'C';	//>>>>>>>>>>>>>>>> 테스트 중에만 사용해야 함!!
	pProfile1 = new CProfileLoad(cSettingFile);

	nProtocolType = 2;	// 'Topcam-2100'
	nMode = 1;			// Local
	nANPRType = 2;		// 'NEW'
	nAutoStart = 0;		// 'OFF'
	nImageWidth = 0;
	nImageHeight = 0;
	nTestMode = 0;

	ZeroMemory(cOriginPath, sizeof(cOriginPath));
	ZeroMemory(cDestPath, sizeof(cDestPath));
	ZeroMemory(cExceptionPath, sizeof(cExceptionPath));

	GetReadLocalSetInfo();
}



CLocalInfoSet::~CLocalInfoSet()
{
	delete pProfile1;
}



void CLocalInfoSet::GetReadLocalSetInfo()
{
	pProfile1->GetINIFileParameterInt("General", "PROTICOL_TYPE", &nProtocolType);
	pProfile1->GetINIFileParameterInt("General", "MODE", &nMode);
	pProfile1->GetINIFileParameterInt("General", "ANPR_TYPE", &nANPRType);
	pProfile1->GetINIFileParameterInt("General", "AUTO_START", &nAutoStart);

	pProfile1->GetINIFileParameterInt("General", "IMAGE_WIDTH", &nImageWidth);
	pProfile1->GetINIFileParameterInt("General", "IMAGE_HEIGHT", &nImageHeight);

	nTestMode = 0;
	pProfile1->GetINIFileParameterInt("General", "TEST_MODE", &nTestMode);

	ZeroMemory(cOriginPath, sizeof(cOriginPath));
	if (!pProfile1->GetINIFileParameterStr("General", "ORIGIN_PATH", cOriginPath))
		pProfile1->DisplayINIFileLoadError("settings.ini [General => ORIGIN_PATH]");

	ZeroMemory(cDestPath, sizeof(cDestPath));
	if (!pProfile1->GetINIFileParameterStr("General", "DESTINATION_PATH", cDestPath))
		pProfile1->DisplayINIFileLoadError("settings.ini [General => DESTINATION_PATH]");

	ZeroMemory(cExceptionPath, sizeof(cExceptionPath));
	if (!pProfile1->GetINIFileParameterStr("General", "EXCEPTION_PATH", cExceptionPath))
		pProfile1->DisplayINIFileLoadError("settings.ini [General => EXCEPTION_PATH]");	

	ZeroMemory(cLogFilePath, sizeof(cLogFilePath));
	if (!pProfile1->GetINIFileParameterStr("General", "LOGFILE_PATH", cLogFilePath))
		pProfile1->DisplayINIFileLoadError("settings.ini [General => LOGFILE_PATH]");
}



void CLocalInfoSet::SaveLocalInfo()
{
	SetINIFileParameterInt("General", "PROTICOL_TYPE", nProtocolType);
	SetINIFileParameterInt("General", "MODE", nMode);
	SetINIFileParameterInt("General", "ANPR_TYPE", nANPRType);
	SetINIFileParameterInt("General", "AUTO_START", nAutoStart);

	SetINIFileParameterInt("General", "IMAGE_WIDTH", nImageWidth);
	SetINIFileParameterInt("General", "IMAGE_HEIGHT", nImageHeight);

	SetINIFileParameterStr("General", "ORIGIN_PATH", cOriginPath);
	SetINIFileParameterStr("General", "DESTINATION_PATH", cDestPath);
	SetINIFileParameterStr("General", "EXCEPTION_PATH", cExceptionPath);
	SetINIFileParameterStr("General", "LOGFILE_PATH", cLogFilePath);
}



BOOL CLocalInfoSet::SetINIFileParameterInt(char* lpszSectionName, char* lpszItmeName, int nValue)
{
	BOOL		bResult;
	char		GetResultNo[16];

	ZeroMemory(GetResultNo, sizeof(GetResultNo));
	wsprintf(GetResultNo, "%d", nValue);
	bResult = WritePrivateProfileString((LPCTSTR)lpszSectionName,
										(LPCTSTR)lpszItmeName,
										(LPCTSTR)GetResultNo,
										(LPCTSTR)cSettingFile);
	return TRUE;
}



BOOL CLocalInfoSet::SetINIFileParameterStr(char* lpszSectionName, char* lpszItmeName, char* lpszValue)
{
	BOOL			bResult;

	bResult = WritePrivateProfileString((LPCTSTR)lpszSectionName,
		(LPCTSTR)lpszItmeName,
		(LPCTSTR)lpszValue,
		(LPCTSTR)cSettingFile);
	return TRUE;
}
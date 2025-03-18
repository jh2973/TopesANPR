#pragma once

#include "CProfileLoad.h"

class CLocalInfoSet
{
private:
	CProfileLoad* pProfile1;

	void GetReadLocalSetInfo();
	BOOL SetINIFileParameterInt(char* lpszSectionName, char* lpszItmeName, int nValue);
	BOOL SetINIFileParameterStr(char* lpszSectionName, char* lpszItmeName, char* lpszValue);

public:
	void SaveLocalInfo();

public:
	CLocalInfoSet();
	virtual ~CLocalInfoSet();

	int		nProtocolType;
	int		nMode;
	int		nANPRType;
	int		nAutoStart;
	int		nImageWidth;
	int		nImageHeight;
	int		nTestMode;
	int		nPlateAngle;
	char	cOriginPath[128];
	char	cDestPath[128];
	char	cExceptionPath[128];
	char	cLogFilePath[128];
};
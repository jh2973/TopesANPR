#include "CFileManager.h"

CFileManager::CFileManager(CLocalInfoSet* set)
{
	m_pEnforceSet = set;
}



CFileManager::~CFileManager()
{
	
}



//========================================================================================
// Make System Log (Index가 틀릴 경우 남김)
//========================================================================================
void CFileManager::WriteRunnginLogMessage(time_t lTime, char* message)
{
	char	logMessage[512];
	char	fileName[128];
	struct	tm* logTime;

	if (lTime < 0)
	{
		ZeroMemory(logMessage, sizeof(logMessage));
		wsprintf(logMessage, "[WriteRunnginLogMessage] lTime 값이 0보다 작다 !! - message:[%s] \n", message);
		OutputDebugString(logMessage);
		return;
	}

	// 시간 표시
	logTime = localtime(&lTime);
	ZeroMemory(logMessage, sizeof(logMessage));
	wsprintf(logMessage, "%02d:%02d:%02d : %s \n", logTime->tm_hour, logTime->tm_min, logTime->tm_sec, message);

	ZeroMemory(fileName, sizeof(fileName));
	wsprintf(fileName, "%s/%d%02d%02d.log", m_pEnforceSet->cLogFilePath, logTime->tm_year + 1900, logTime->tm_mon + 1, logTime->tm_mday);
	int	size = strlen(logMessage);

	FILE* file;
	file = fopen(fileName, "a");
	if (file != NULL) {
		fwrite(logMessage, size, 1, file);
		fclose(file);
	}
}

void CFileManager::WriteFailLogMessage(time_t lTime, char* message)
{
	char	logMessage[512];
	char	fileName[128];
	struct	tm* logTime;

	if (lTime < 0)
	{
		ZeroMemory(logMessage, sizeof(logMessage));
		wsprintf(logMessage, "[WriteRunnginLogMessage] lTime 값이 0보다 작다 !! - message:[%s] \n", message);
		OutputDebugString(logMessage);
		return;
	}

	// 시간 표시
	logTime = localtime(&lTime);
	ZeroMemory(logMessage, sizeof(logMessage));
	wsprintf(logMessage, "%02d:%02d:%02d : %s \n", logTime->tm_hour, logTime->tm_min, logTime->tm_sec, message);

	ZeroMemory(fileName, sizeof(fileName));
	wsprintf(fileName, "%s/%d%02d%02d_FAIL.log", m_pEnforceSet->cLogFilePath, logTime->tm_year + 1900, logTime->tm_mon + 1, logTime->tm_mday);
	int	size = strlen(logMessage);

	FILE* file;
	file = fopen(fileName, "a");
	if (file != NULL) {
		fwrite(logMessage, size, 1, file);
		fclose(file);
	}
}
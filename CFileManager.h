#pragma once

#include "CLocalInfoSet.h"

class CFileManager
{
public:
	CFileManager(CLocalInfoSet* set);
	virtual ~CFileManager();

	void WriteRunnginLogMessage(time_t lTime, char* message);
	void WriteFailLogMessage(time_t lTime, char* message);

private:
	CLocalInfoSet* m_pEnforceSet;
};


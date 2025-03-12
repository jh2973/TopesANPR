#include <stdio.h>
#include <string.h>

#include "CProfileLoad.h"

//=====================================================================================
// ������
//=====================================================================================
CProfileLoad::CProfileLoad (char *fileName)
{
	ZeroMemory (m_szIniFileName, sizeof(m_szIniFileName));
	wsprintf (m_szIniFileName, "%s", fileName);
}

//=====================================================================================
// �Ҹ���
//=====================================================================================
CProfileLoad::~CProfileLoad()
{

}

//=====================================================================================
// Not Found INI Parameter
//=====================================================================================
void CProfileLoad::DisplayINIFileLoadError(char *pDispText)
{
	MessageBox( NULL, pDispText, "INI_File Error", MB_OK | MB_ICONWARNING);
}

//=====================================================================================
// INI File�� ���� ���� �б� (Integer)
//=====================================================================================
BOOL CProfileLoad::GetINIFileParameterInt (char *lpszSectionName, char *lpszItmeName, PINT pnResult)
{
 	char			GetResultNo[8];
    DWORD			dwResult; 

	// Get INI File Info
    dwResult = GetPrivateProfileString((LPCTSTR) lpszSectionName,
                                       (LPCTSTR) lpszItmeName,
                                       (LPCTSTR)(""),
                                       (LPTSTR)(GetResultNo),
                                       sizeof(GetResultNo),
                                       (LPCTSTR) m_szIniFileName);

	// ���� �׸��� ���� ��� 0 
	if (dwResult <= 0)	return FALSE;

	*pnResult = atoi(GetResultNo);
	return TRUE;
}

//=====================================================================================
// INI File�� ���� ���� �б� (Float)
//=====================================================================================
BOOL CProfileLoad::GetINIFileParameterFloat (char *lpszSectionName, char *lpszItmeName, PFLOAT pnResult)
{
 	char			GetResultNo[8];
    DWORD			dwResult; 

	// Get INI File Info
    dwResult = GetPrivateProfileString((LPCTSTR) lpszSectionName,
                                       (LPCTSTR) lpszItmeName,
                                       (LPCTSTR)(""),
                                       (LPTSTR)(GetResultNo),
                                       sizeof(GetResultNo),
                                       (LPCTSTR) m_szIniFileName);

	// ���� �׸��� ���� ��� 0 
	if (dwResult <= 0)	return FALSE;

	*pnResult = (float)atof(GetResultNo);
	return TRUE;
}

//=====================================================================================
// INI File�� ���� ���� �б� (Float)
//=====================================================================================
BOOL CProfileLoad::GetINIFileParameterLong (char *lpszSectionName, char *lpszItmeName, PLONG pnResult)
{
 	char			GetResultNo[8];
    DWORD			dwResult; 

	// Get INI File Info
    dwResult = GetPrivateProfileString((LPCTSTR) lpszSectionName,
                                       (LPCTSTR) lpszItmeName,
                                       (LPCTSTR)(""),
                                       (LPTSTR)(GetResultNo),
                                       sizeof(GetResultNo),
                                       (LPCTSTR) m_szIniFileName);

	// ���� �׸��� ���� ��� 0 
	if (dwResult <= 0)	return FALSE;

	*pnResult = (long)atol(GetResultNo);
	return TRUE;
}

//=====================================================================================
// INI File�� ���� ���� �б� (String)
//=====================================================================================
BOOL CProfileLoad::GetINIFileParameterStr (char *lpszSectionName, char *lpszItmeName, PSTR pcResult)
{
 	char			GetResultNo[32];
    DWORD			dwResult; 

	// Get INI File Info
	ZeroMemory (GetResultNo, sizeof(GetResultNo));
    dwResult = GetPrivateProfileString((LPCTSTR) lpszSectionName,
                                       (LPCTSTR) lpszItmeName,
                                       (LPCTSTR)(""),
                                       (LPTSTR)(GetResultNo),
                                       sizeof(GetResultNo),
                                       (LPCTSTR) m_szIniFileName);

	// ���� �׸��� ���� ��� 0 
	if (dwResult <= 0)	return FALSE;

	CopyMemory (pcResult, GetResultNo, strlen (GetResultNo));
	return TRUE;
}

//=====================================================================================
// INI File�� ���� ���� ���� (String)
//=====================================================================================
BOOL CProfileLoad::SetINIFileParameterStr (char *lpszSectionName, char *lpszItmeName, char *lpszValue)
{
	BOOL			bResult;

   	bResult = WritePrivateProfileString((LPCTSTR) lpszSectionName, 
										(LPCTSTR) lpszItmeName,
										(LPCTSTR) lpszValue, 
										(LPCTSTR) m_szIniFileName);
	return TRUE;
}

//=====================================================================================
// INI File�� ���� ���� ���� (Integer)
//=====================================================================================
BOOL CProfileLoad::SetINIFileParameterInt (char *lpszSectionName, char *lpszItmeName, int nValue)
{
	BOOL		bResult;
	char		GetResultNo[16];

	ZeroMemory (GetResultNo, sizeof(GetResultNo));
	wsprintf (GetResultNo, "%d", nValue);
   	bResult = WritePrivateProfileString((LPCTSTR) lpszSectionName, 
										(LPCTSTR) lpszItmeName,
										(LPCTSTR) GetResultNo, 
										(LPCTSTR) m_szIniFileName);
	return TRUE;
}

//=====================================================================================
// INI File�� ���� ���� ���� (Integer)
//=====================================================================================
BOOL CProfileLoad::SetINIFileParameterFloat (char *lpszSectionName, char *lpszItmeName, float fValue)
{
	BOOL		bResult;
	char		GetResultNo[16];

	ZeroMemory (GetResultNo, sizeof(GetResultNo));
	sprintf (GetResultNo, "%3.1f", fValue);
   	bResult = WritePrivateProfileString((LPCTSTR) lpszSectionName, 
										(LPCTSTR) lpszItmeName,
										(LPCTSTR) GetResultNo, 
										(LPCTSTR) m_szIniFileName);
	return TRUE;
}

//========================================================================================
// ���� �޸𸮿��� ���� Item���� ���� �۾� ��ȯ
//  GetToken
//========================================================================================
void CProfileLoad::GetTokenCurrentDir( LPSTR lpszVar, LPSTR lpszParam, ... )
{
	char szVar[1024];

	ZeroMemory( szVar, sizeof(szVar) );
	lstrcpy( szVar, lpszVar );

	if ( szVar ) {		// Data Receive
		va_list argv;
		va_start( argv, lpszParam );	// Initialize variable arguments (lpszParam Buffer is NULL)
		LPSTR lpBuffer = lpszParam;
		LPSTR s = szVar;				// szVar�� ���� �޸� ����
		s = strtok( szVar, "\\" );		// Find the next token in a string (szVar���� " "ã��)

		if( !s )	
			return;					
		// ���̻� ���� Parm�� ���� ���

		while( s && lpBuffer )	{
			lstrcpy( lpBuffer, s );
			lpBuffer = va_arg( argv, LPSTR );	// type va_arg( va_list arg, type );
			s = strtok( NULL, "\\" );
		}
		va_end( argv );							// Reset variable arguments
	}
}
#if !defined(AFX_STDAFX_H__57C6F3A1_FEBA_4EBC_A4B7_190D1875002C__INCLUDED_)
#define AFX_STDAFX_H__57C6F3A1_FEBA_4EBC_A4B7_190D1875002C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__57C6F3A1_FEBA_4EBC_A4B7_190D1875002C__INCLUDED_)



class CProfileLoad
{
	private :
		char	m_szIniFileName[128];
		void	GetTokenCurrentDir( LPSTR lpszVar, LPSTR lpszParam, ... );

	public :
		// 持失切 社瑚切
		CProfileLoad (char *fileName);
		virtual ~CProfileLoad();

		//char	m_ExcutePath[32];
		void DisplayINIFileLoadError(char *pDispText);
		BOOL GetINIFileParameterStr (char *lpszSectionName, char *lpszItmeName, PSTR pcResult);
		BOOL GetINIFileParameterInt (char *lpszSectionName, char *lpszItmeName, PINT pnResult);
		BOOL GetINIFileParameterFloat (char *lpszSectionName, char *lpszItmeName, PFLOAT pnResult);
		BOOL GetINIFileParameterLong (char *lpszSectionName, char *lpszItmeName, PLONG pnResult);

		BOOL SetINIFileParameterStr (char *lpszSectionName, char *lpszItmeName, char *lpszValue);
		BOOL SetINIFileParameterInt (char *lpszSectionName, char *lpszItmeName, int nValue);
		BOOL SetINIFileParameterFloat (char *lpszSectionName, char *lpszItmeName, float fValue);
};
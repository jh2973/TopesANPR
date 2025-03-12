
// KistAnprAppDlg.h: 헤더 파일
//

#pragma once

#include "CLocalInfoSet.h"
#include "CFileManager.h"
#include "KistAnprClass.h"

// CKistAnprAppDlg 대화 상자
class CKistAnprAppDlg : public CDialogEx
{
// 생성입니다.
public:
	CKistAnprAppDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KISTANPRAPP_DIALOG };
#endif

private:
	KistAnprClass* m_pKistAnprClass;
	BOOL	bIsFirst;

public:
	void	WriteLogMessage(char* pLogData);
	void	SaveSettings();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	CLocalInfoSet* m_pEnforceSet;
	CFileManager* m_pFileManage;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOriginFolder();
	afx_msg void OnBnClickedDestFolder();
	afx_msg void OnBnClickedExceptionFolder();
	afx_msg void OnSelchangeComboProtocol();
	CComboBox m_combo_protocol;
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	CButton m_btn_start;
	CButton m_btn_stop;
	CComboBox m_ANPR_Type;
	afx_msg void OnSelchangeComboAnprType();
	afx_msg void OnSelchangeComboMode();
	CComboBox m_combo_mode;
	afx_msg void OnBnClickedCntClear();
};

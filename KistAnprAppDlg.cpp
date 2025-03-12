
// KistAnprAppDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "KistAnprApp.h"
#include "KistAnprAppDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CKistAnprAppDlg 대화 상자
CKistAnprAppDlg::CKistAnprAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KISTANPRAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CKistAnprAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, COMBO_PROTOCOL, m_combo_protocol);
	DDX_Control(pDX, COMBO_MODE, m_combo_mode);
	DDX_Control(pDX, BTN_START, m_btn_start);
	DDX_Control(pDX, BTN_STOP, m_btn_stop);
	DDX_Control(pDX, COMBO_ANPR_TYPE, m_ANPR_Type);
}


BEGIN_MESSAGE_MAP(CKistAnprAppDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(BTN_ORIGIN_FOLDER, &CKistAnprAppDlg::OnBnClickedOriginFolder)
	ON_BN_CLICKED(BTN_DEST_FOLDER, &CKistAnprAppDlg::OnBnClickedDestFolder)
	ON_BN_CLICKED(BTN_EXCEPTION_FOLDER, &CKistAnprAppDlg::OnBnClickedExceptionFolder)
	ON_CBN_SELCHANGE(COMBO_PROTOCOL, &CKistAnprAppDlg::OnSelchangeComboProtocol)
	ON_CBN_SELCHANGE(COMBO_MODE, &CKistAnprAppDlg::OnSelchangeComboMode)
	ON_BN_CLICKED(BTN_START, &CKistAnprAppDlg::OnBnClickedStart)
	ON_BN_CLICKED(BTN_STOP, &CKistAnprAppDlg::OnBnClickedStop)
	ON_CBN_SELCHANGE(COMBO_ANPR_TYPE, &CKistAnprAppDlg::OnSelchangeComboAnprType)	
	ON_BN_CLICKED(BTN_CNT_CLEAR, &CKistAnprAppDlg::OnBnClickedCntClear)
END_MESSAGE_MAP()


// CKistAnprAppDlg 메시지 처리기

BOOL CKistAnprAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	m_pEnforceSet = new CLocalInfoSet();
	m_pFileManage = new CFileManager(m_pEnforceSet);
	m_pKistAnprClass = new KistAnprClass(m_pEnforceSet, m_pFileManage);

	((CButton*)GetDlgItem(EDIT_PATH_ORIGIN))->SetWindowText(m_pEnforceSet->cOriginPath);
	((CButton*)GetDlgItem(EDIT_PATH_DEST))->SetWindowText(m_pEnforceSet->cDestPath);
	((CButton*)GetDlgItem(EDIT_PATH_EXCEPTION))->SetWindowText(m_pEnforceSet->cExceptionPath);

	//Protocol
	m_combo_protocol.AddString("Topcam-2016");
	m_combo_protocol.AddString("Topcam-2100");
	m_combo_protocol.SetCurSel(m_pEnforceSet->nProtocolType - 1);

	//Mode
	m_combo_mode.AddString("Local Mode");
	m_combo_mode.AddString("Center Mode");
	m_combo_mode.AddString("Inspection Mode");
	m_combo_mode.SetCurSel(m_pEnforceSet->nMode - 1);

	//ANPR Type
	m_ANPR_Type.AddString("Old ANPR");
	m_ANPR_Type.AddString("New ANPR");
	m_ANPR_Type.SetCurSel(m_pEnforceSet->nANPRType - 1);

	//폴더 생성
	CreateDirectory(m_pEnforceSet->cOriginPath, NULL);
	CreateDirectory(m_pEnforceSet->cDestPath, NULL);
	CreateDirectory(m_pEnforceSet->cExceptionPath, NULL);
	CreateDirectory(m_pEnforceSet->cLogFilePath, NULL);

	m_btn_start.EnableWindow(TRUE);
	m_btn_stop.EnableWindow(FALSE);

	//Init
	m_pKistAnprClass->OLD_Plate_Init(0);

	//Start
	if (m_pEnforceSet->nAutoStart == 1)
	{
		OnBnClickedStart();
	}

	bIsFirst = FALSE;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.


void CKistAnprAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CKistAnprAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CKistAnprAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (nIDEvent == 1)
	{
		if (bIsFirst == FALSE)
		{
			SaveSettings();
			bIsFirst = TRUE;
		}

		if(m_pEnforceSet->nMode == 1 || m_pEnforceSet->nMode == 2)	
			m_pKistAnprClass->Anpr_Proc_Normal();		//로컬모드 or 센터모드
		else
			m_pKistAnprClass->Anpr_Proc_Inspection();	//검사모드

		char	cResultPass[8], cResultFail[8];
		wsprintf(cResultPass, "%d대", m_pKistAnprClass->nPassCount);
		wsprintf(cResultFail, "%d대", m_pKistAnprClass->nFailCount);

		((CButton*)GetDlgItem(EDIT_PASS_CNT))->SetWindowText(cResultPass);
		((CButton*)GetDlgItem(EDIT_FAIL_CNT))->SetWindowText(cResultFail);
	}		

	CDialogEx::OnTimer(nIDEvent);
}


void CKistAnprAppDlg::WriteLogMessage(char* pLogData)
{
	char	LogMsg[2048];
	char	fileName[1024];

	OutputDebugString(pLogData);

	// 시간 표시
	SYSTEMTIME	LocalTime;
	GetLocalTime(&LocalTime);
	ZeroMemory(LogMsg, sizeof(LogMsg));
	wsprintf(LogMsg, "%02d:%02d:%02d - %s \n", LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond, pLogData);

	FILE* file;
	ZeroMemory(fileName, sizeof(fileName));
	wsprintf(fileName, "%s/%d%02d%02d.log", m_pEnforceSet->cLogFilePath, LocalTime.wYear, LocalTime.wMonth, LocalTime.wDay);

	int	size = strlen(LogMsg);

	file = fopen(fileName, "a");
	if (file != NULL) {
		fwrite(LogMsg, size, 1, file);
		fclose(file);
	}
}


void CKistAnprAppDlg::OnBnClickedOriginFolder()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BROWSEINFO BrInfo;
	TCHAR szBuffer[128];

	::ZeroMemory(&BrInfo, sizeof(BROWSEINFO));
	::ZeroMemory(szBuffer, 128);

	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.lpszTitle = _T("원본 파일 경로를 선택하세요");
	BrInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pItemIdList = ::SHBrowseForFolder(&BrInfo);
	::SHGetPathFromIDList(pItemIdList, szBuffer);

	if(szBuffer[0] == 0)
		SetDlgItemText(EDIT_PATH_ORIGIN, m_pEnforceSet->cOriginPath);
	else
	{
		SetDlgItemText(EDIT_PATH_ORIGIN, szBuffer);
		::ZeroMemory(m_pEnforceSet->cOriginPath, 128);
		CopyMemory(m_pEnforceSet->cOriginPath, szBuffer, 128);
	}
}


void CKistAnprAppDlg::OnBnClickedDestFolder()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BROWSEINFO BrInfo;
	TCHAR szBuffer[128];

	::ZeroMemory(&BrInfo, sizeof(BROWSEINFO));
	::ZeroMemory(szBuffer, 128);

	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.lpszTitle = _T("처리 파일 경로를 선택하세요");
	BrInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pItemIdList = ::SHBrowseForFolder(&BrInfo);
	::SHGetPathFromIDList(pItemIdList, szBuffer);

	if (szBuffer[0] == 0)
		SetDlgItemText(EDIT_PATH_DEST, m_pEnforceSet->cDestPath);
	else
	{
		SetDlgItemText(EDIT_PATH_DEST, szBuffer);
		::ZeroMemory(m_pEnforceSet->cDestPath, 128);
		CopyMemory(m_pEnforceSet->cDestPath, szBuffer, 128);
	}
		
}


void CKistAnprAppDlg::OnBnClickedExceptionFolder()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BROWSEINFO BrInfo;
	TCHAR szBuffer[128];

	::ZeroMemory(&BrInfo, sizeof(BROWSEINFO));
	::ZeroMemory(szBuffer, 128);

	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.lpszTitle = _T("예외 파일 경로를 선택하세요");
	BrInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pItemIdList = ::SHBrowseForFolder(&BrInfo);
	::SHGetPathFromIDList(pItemIdList, szBuffer);

	if (szBuffer[0] == 0)
		SetDlgItemText(EDIT_PATH_EXCEPTION, m_pEnforceSet->cExceptionPath);
	else
	{
		SetDlgItemText(EDIT_PATH_EXCEPTION, szBuffer);
		::ZeroMemory(m_pEnforceSet->cExceptionPath, 128);
		CopyMemory(m_pEnforceSet->cExceptionPath, szBuffer, 128);
	}		
}


void CKistAnprAppDlg::OnSelchangeComboProtocol()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pEnforceSet->nProtocolType = ((CComboBox*)GetDlgItem(COMBO_PROTOCOL))->GetCurSel() + 1;
}


void CKistAnprAppDlg::OnSelchangeComboMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pEnforceSet->nMode = ((CComboBox*)GetDlgItem(COMBO_MODE))->GetCurSel() + 1;
}


void CKistAnprAppDlg::OnBnClickedStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	SetTimer(1, 1000, NULL);

	m_btn_start.EnableWindow(FALSE);
	m_btn_stop.EnableWindow(TRUE);

	m_combo_protocol.EnableWindow(FALSE);
	m_combo_mode.EnableWindow(FALSE);
	m_ANPR_Type.EnableWindow(FALSE);
	((CButton*)GetDlgItem(BTN_ORIGIN_FOLDER))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(BTN_DEST_FOLDER))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(BTN_EXCEPTION_FOLDER))->EnableWindow(FALSE);

	m_pEnforceSet->nAutoStart = 1;
	m_pEnforceSet->SaveLocalInfo();
}


void CKistAnprAppDlg::OnBnClickedStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	KillTimer(1);

	bIsFirst = FALSE;

	m_btn_start.EnableWindow(TRUE);
	m_btn_stop.EnableWindow(FALSE);

	m_combo_protocol.EnableWindow(TRUE);
	m_combo_mode.EnableWindow(TRUE);
	m_ANPR_Type.EnableWindow(TRUE);
	((CButton*)GetDlgItem(BTN_ORIGIN_FOLDER))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(BTN_DEST_FOLDER))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(BTN_EXCEPTION_FOLDER))->EnableWindow(TRUE);

	m_pEnforceSet->nAutoStart = 0;
	m_pEnforceSet->SaveLocalInfo();
}


void CKistAnprAppDlg::OnSelchangeComboAnprType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pEnforceSet->nANPRType = ((CComboBox*)GetDlgItem(COMBO_ANPR_TYPE))->GetCurSel() + 1;
}


void CKistAnprAppDlg::SaveSettings()
{
	m_pEnforceSet->nProtocolType = ((CComboBox*)GetDlgItem(COMBO_PROTOCOL))->GetCurSel() + 1;
	m_pEnforceSet->nANPRType = ((CComboBox*)GetDlgItem(COMBO_ANPR_TYPE))->GetCurSel() + 1;

	GetDlgItem(EDIT_PATH_ORIGIN)->GetWindowText((LPSTR)m_pEnforceSet->cOriginPath, 128);
	GetDlgItem(EDIT_PATH_DEST)->GetWindowText((LPSTR)m_pEnforceSet->cDestPath, 128);
	GetDlgItem(EDIT_PATH_EXCEPTION)->GetWindowText((LPSTR)m_pEnforceSet->cExceptionPath, 128);
	
	//m_pEnforceSet->nAutoStart = 1;
	m_pEnforceSet->SaveLocalInfo();
}


void CKistAnprAppDlg::OnBnClickedCntClear()
{
	m_pKistAnprClass->nPassCount = 0;
	m_pKistAnprClass->nFailCount = 0;

	char	cResultPass[8], cResultFail[8];
	wsprintf(cResultPass, "%d대", m_pKistAnprClass->nPassCount);
	wsprintf(cResultFail, "%d대", m_pKistAnprClass->nFailCount);

	((CButton*)GetDlgItem(EDIT_PASS_CNT))->SetWindowText(cResultPass);
	((CButton*)GetDlgItem(EDIT_FAIL_CNT))->SetWindowText(cResultFail);
}

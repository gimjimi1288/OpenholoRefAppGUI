﻿// Tab_MESH_R.cpp: 구현 파일
//

#include "stdafx.h"
#include "OpenholoRefAppGUI.h"
#include "OpenholoRefAppGUIDlg.h"
#include "Tab_RECON.h"
#include "Console.h"
#include "afxdialogex.h"
#include <ophRec.h>
#include "Dialog_BMP_Viewer.h"
#include "Dialog_Progress.h"
#include "Dialog_Prompt.h"
#define KEY_NAME L"RECON"

// CTab_RECON 대화 상자

IMPLEMENT_DYNAMIC(CTab_RECON, CDialogEx)

CTab_RECON::CTab_RECON(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_RECON, pParent)
	, m_from(0)
	, m_to(0)
	, m_step(0)
	, m_bReal(false)
	, m_bImag(false)
	, m_bPhase(false)
	, m_bAmpli(false)
{

}

CTab_RECON::~CTab_RECON()
{
}

void CTab_RECON::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FROM, m_from);
	DDX_Text(pDX, IDC_EDIT_TO, m_to);
	DDX_Text(pDX, IDC_EDIT_STEP, m_step);
}


BEGIN_MESSAGE_MAP(CTab_RECON, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_LOAD_PHASE, &CTab_RECON::OnBnClickedBtnLoadPhase)
	ON_BN_CLICKED(IDC_BTN_LOAD_AMPLITUDE, &CTab_RECON::OnBnClickedBtnLoadAmplitude)
	ON_BN_CLICKED(IDC_BTN_LOAD_FRINGE, &CTab_RECON::OnBnClickedBtnLoadFringe)
	ON_BN_CLICKED(IDC_BTN_LOAD_REAL, &CTab_RECON::OnBnClickedBtnLoadReal)
	ON_BN_CLICKED(IDC_BTN_LOAD_IMAG, &CTab_RECON::OnBnClickedBtnLoadImaginary)
	ON_WM_CREATE()
	ON_MESSAGE(RECONSTRUCT, &CTab_RECON::OnReconstruct)
	ON_MESSAGE(SAVE_IMG, &CTab_RECON::OnSaveIMG)
	ON_BN_CLICKED(IDC_READ_CONFIG_REC, &CTab_RECON::OnBnClickedReadConfigRec)
	ON_BN_CLICKED(IDC_RADIO_POSITION, &CTab_RECON::OnBnClickedRadioPosition)
	ON_BN_CLICKED(IDC_RADIO_FOCUS, &CTab_RECON::OnBnClickedRadioFocus)
END_MESSAGE_MAP()


UINT CallFuncRECON(void* param)
{
	parammeter *pParam = (parammeter *)param;
	((ophRec*)pParam->pInst)->ReconstructImage();
	pParam->pDialog->m_bFinished = TRUE;

	ophRec *pRecon = ((ophRec *)pParam->pInst);
	Complex<Real> **pp = pRecon->getComplexField();

	Console::getInstance()->SetColor(Console::Color::YELLOW, Console::Color::BLACK);
	for (uint i = 0; i < pRecon->getContext().waveNum; i++) {
		printf("=> Complex Field[%d][0] = %.15e / %.15e \n", i, pp[i][0][_RE], pp[i][0][_IM]);
	}
	Console::getInstance()->ResetColor();
	delete pParam;

	return 1;
}

// CTab_RECON 메시지 처리기

void CTab_RECON::ReloadContents(int opt)
{
	GetDlgItem(IDC_BTN_LOAD_FRINGE)->ShowWindow(opt == 0 ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BTN_LOAD_PHASE)->ShowWindow(opt == 1 ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BTN_LOAD_AMPLITUDE)->ShowWindow(opt == 1 ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BTN_LOAD_REAL)->ShowWindow(opt == 2 ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BTN_LOAD_IMAG)->ShowWindow(opt == 2 ? SW_SHOW : SW_HIDE);
}

void CTab_RECON::OnBnClickedBtnLoadPhase()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TCHAR szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);

	LPTSTR szFilter = L"BMP File (*.bmp) |*.bmp|";

	CFileDialog FileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	CString szImgPath = AfxGetApp()->GetProfileString(KEY_NAME, L"Img Path", szCurPath);
	FileDialog.m_ofn.lpstrInitialDir = szImgPath;
	CString path;
	if (FileDialog.DoModal() == IDOK)
	{
		CString ext = FileDialog.GetFileExt();
		if (!ext.CompareNoCase(L"bmp")) {
			path = FileDialog.GetPathName();
		}
		else return;
	}

	SetCurrentDirectory(szCurPath);
	if (!path.GetLength()) return;
	AfxGetApp()->WriteProfileString(KEY_NAME, L"Img Path", path.Left(path.ReverseFind('\\') + 1));

	m_szPhase = path;
	m_bPhase = true;

	if (m_bPhase && m_bAmpli)
	{
		if (!m_rec->readImagePNA(CW2A(m_szPhase), CW2A(m_szAmpli)))
		{
			AfxMessageBox(L"BMP load failed : Please show LOG.");
			return;
		}

		AfxGetMainWnd()->SendMessage(LOAD_DATA, LOAD_DATA, 0);
	}
}

void CTab_RECON::OnBnClickedBtnLoadAmplitude()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TCHAR szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);

	LPTSTR szFilter = L"BMP File (*.bmp) |*.bmp|";

	CFileDialog FileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	CString szImgPath = AfxGetApp()->GetProfileString(KEY_NAME, L"Img Path", szCurPath);
	FileDialog.m_ofn.lpstrInitialDir = szImgPath;
	CString path;
	if (FileDialog.DoModal() == IDOK)
	{
		CString ext = FileDialog.GetFileExt();
		if (!ext.CompareNoCase(L"bmp")) {
			path = FileDialog.GetPathName();
		}
		else return;
	}

	SetCurrentDirectory(szCurPath);
	if (!path.GetLength()) return;
	AfxGetApp()->WriteProfileString(KEY_NAME, L"Img Path", path.Left(path.ReverseFind('\\') + 1));

	m_szAmpli = path;
	m_bAmpli = true;

	if (m_bPhase && m_bAmpli)
	{
		if (!m_rec->readImagePNA(CW2A(m_szPhase), CW2A(m_szAmpli)))
		{
			AfxMessageBox(L"BMP load failed : Please show LOG.");
			return;
		}

		AfxGetMainWnd()->SendMessage(LOAD_DATA, LOAD_DATA, 0);
	}
}

void CTab_RECON::OnBnClickedBtnLoadFringe()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TCHAR szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);

	LPTSTR szFilter = L"BMP File (*.bmp) |*.bmp|";

	CFileDialog FileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	CString szImgPath = AfxGetApp()->GetProfileString(KEY_NAME, L"Img Path", szCurPath);
	FileDialog.m_ofn.lpstrInitialDir = szImgPath;
	CString path;
	if (FileDialog.DoModal() == IDOK)
	{
		CString ext = FileDialog.GetFileExt();
		if (!ext.CompareNoCase(L"bmp")) {
			path = FileDialog.GetPathName();
		}
		else return;
	}

	SetCurrentDirectory(szCurPath);
	if (!path.GetLength()) return;
	AfxGetApp()->WriteProfileString(KEY_NAME, L"Img Path", path.Left(path.ReverseFind('\\') + 1));
	//_tcscpy_s(m_argParamRGBimg, path.GetBuffer());

	if (!m_rec->readImage(CW2A(path)))
	{
		AfxMessageBox(L"BMP load failed : Please show LOG.");
		return;
	}

	AfxGetMainWnd()->SendMessage(LOAD_DATA, LOAD_DATA, 0);
}

void CTab_RECON::OnBnClickedBtnLoadReal()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TCHAR szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);

	LPTSTR szFilter = L"BMP File (*.bmp) |*.bmp|";

	CFileDialog FileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	CString szImgPath = AfxGetApp()->GetProfileString(KEY_NAME, L"Img Path", szCurPath);
	FileDialog.m_ofn.lpstrInitialDir = szImgPath;
	CString path;
	if (FileDialog.DoModal() == IDOK)
	{
		CString ext = FileDialog.GetFileExt();
		if (!ext.CompareNoCase(L"bmp")) {
			path = FileDialog.GetPathName();
		}
		else return;
	}

	SetCurrentDirectory(szCurPath);
	if (!path.GetLength()) return;
	AfxGetApp()->WriteProfileString(KEY_NAME, L"Img Path", path.Left(path.ReverseFind('\\') + 1));

	m_szReal = path;
	m_bReal = true;

	if (m_bReal && m_bImag)
	{
		if (!m_rec->readImageRNI(CW2A(m_szReal), CW2A(m_szImag)))
		{
			AfxMessageBox(L"BMP load failed : Please show LOG.");
			return;
		}

		AfxGetMainWnd()->SendMessage(LOAD_DATA, LOAD_DATA, 0);
	}
}


void CTab_RECON::OnBnClickedBtnLoadImaginary()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	TCHAR szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);

	LPTSTR szFilter = L"BMP File (*.bmp) |*.bmp|";

	CFileDialog FileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	CString szImgPath = AfxGetApp()->GetProfileString(KEY_NAME, L"Img Path", szCurPath);
	FileDialog.m_ofn.lpstrInitialDir = szImgPath;
	CString path;
	if (FileDialog.DoModal() == IDOK)
	{
		CString ext = FileDialog.GetFileExt();
		if (!ext.CompareNoCase(L"bmp")) {
			path = FileDialog.GetPathName();
		}
		else return;
	}

	SetCurrentDirectory(szCurPath);
	if (!path.GetLength()) return;
	AfxGetApp()->WriteProfileString(KEY_NAME, L"Img Path", path.Left(path.ReverseFind('\\') + 1));

	m_szImag = path;
	m_bImag = true;

	if (m_bReal && m_bImag)
	{
		if (!m_rec->readImageRNI(CW2A(m_szReal), CW2A(m_szImag)))
		{
			AfxMessageBox(L"BMP load failed : Please show LOG.");
			return;
		}

		AfxGetMainWnd()->SendMessage(LOAD_DATA, LOAD_DATA, 0);
	}
}


int CTab_RECON::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_rec = new ophRec();
	return 0;
}


void CTab_RECON::OnBnClickedReadConfigRec()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	TCHAR szCurPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurPath);

	LPTSTR szFilter = L"XML File (*.xml) |*.xml|";

	CFileDialog FileDialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	CString szCfgPath = AfxGetApp()->GetProfileString(KEY_NAME, L"Config Path", szCurPath);
	FileDialog.m_ofn.lpstrInitialDir = szCfgPath;
	CString path;
	if (FileDialog.DoModal() == IDOK)
	{
		CString ext = FileDialog.GetFileExt();
		if (!ext.CompareNoCase(L"xml")) path = FileDialog.GetPathName();
		else return;
	}

	SetCurrentDirectory(szCurPath);
	if (!path.GetLength()) return;
	AfxGetApp()->WriteProfileString(KEY_NAME, L"Config Path", path.Left(path.ReverseFind('\\') + 1));

	if (!m_rec->readConfig(CW2A(path))) {
		AfxMessageBox(L"it is not xml config file for ophRec.");
		return;
	}

	auto context = m_rec->getContext();
	auto config = m_rec->getConfig();

	m_from = config.SimulationFrom;
	m_to = config.SimulationTo;
	m_step = config.SimulationStep;
	//auto config = m_pDepthMap->getConfig();

	//m_nearDepth = m_pDepthMap->getNearDepth();
	//m_farDepth = m_pDepthMap->getFarDepth();
	//m_numDepth = m_pDepthMap->getNumOfDepth();

	//m_bConfig = true;
	//if (m_bDimg && m_bRGBimg) GetDlgItem(IDC_GENERATE_DM)->EnableWindow(TRUE);
	//GetDlgItem(IDC_LOAD_D_IMG_DM)->EnableWindow(TRUE);
	//GetDlgItem(IDC_LOAD_RGB_IMG_DM)->EnableWindow(TRUE);


	COpenholoRefAppDlg *pParent = (COpenholoRefAppDlg *)AfxGetMainWnd();
	pParent->SetWaveNum(context.waveNum);
	pParent->SetWaveLength(context.wave_length);
	pParent->SetPixelNum(context.pixel_number[_X], context.pixel_number[_Y]);
	pParent->SetPixelPitch(context.pixel_pitch[_X], context.pixel_pitch[_Y]);
	//pParent->SetShift(context.shift[_X], context.shift[_Y], context.shift[_Z]);
	pParent->SendMessage(LOAD_CFG, LOAD_CFG, 0);

	UpdateData(FALSE);
}


LRESULT CTab_RECON::OnReconstruct(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	COpenholoRefAppDlg *dlg = (COpenholoRefAppDlg *)AfxGetMainWnd();
	OphRecConfig config = m_rec->getConfig();
	config.SimulationFrom = m_from;
	config.SimulationTo = m_to;
	config.SimulationStep = m_step;
	config.SimulationMode = ((CButton *)GetDlgItem(IDC_RADIO_FOCUS))->GetCheck() ? 0 : 1;
	config.ViewingWindow = dlg->UseVW();
	config.SimulationPos[_X] = ((CButton *)GetDlgItem(IDC_CHECK_X))->GetCheck();
	config.SimulationPos[_Y] = ((CButton *)GetDlgItem(IDC_CHECK_Y))->GetCheck();
	config.SimulationPos[_Z] = ((CButton *)GetDlgItem(IDC_CHECK_Z))->GetCheck();
	m_rec->setConfig(config);

	//m_pDepthMap->setMode(!dlg->UseGPGPU());
	//m_pDepthMap->setViewingWindow(dlg->UseVW());

	dlg->ForegroundConsole();

	Dialog_Progress progress;

	BOOL bIsFinish = FALSE;

	parammeter *pParam = new parammeter;
	pParam->pInst = m_rec;
	pParam->pDialog = &progress;

	CWinThread* pThread = AfxBeginThread(CallFuncRECON, pParam);
	progress.m_bGen = false;
	progress.DoModal();
	progress.DestroyWindow();
	//MakeFileName();
	return TRUE;
}

LRESULT CTab_RECON::OnSaveIMG(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your control notification handler code here
	COpenholoRefAppDlg *pParent = ((COpenholoRefAppDlg *)AfxGetMainWnd());

	TCHAR szSelPath[MAX_PATH] = { 0, };
	TCHAR szCurPath[MAX_PATH] = { 0, };
	GetCurrentDirectory(MAX_PATH, szCurPath);
	CString path;
	CString szImgsPath = AfxGetApp()->GetProfileString(KEY_NAME, L"Images Path", szCurPath);
	CFolderPickerDialog dlg(szImgsPath, OFN_FILEMUSTEXIST, NULL, 0);
	if (dlg.DoModal() == IDOK) {
		path = dlg.GetPathName();
	}
	else
		return FALSE;

	SetCurrentDirectory(szCurPath);
	if (!path.GetLength()) return FALSE;
	AfxGetApp()->WriteProfileString(KEY_NAME, L"Images Path", path.Left(path.ReverseFind('\\') + 1));
	SetCurrentDirectory(szCurPath);

	_tcscpy_s(m_resultPath, path.GetBuffer());

	if (!path.GetLength()) return FALSE;
	m_rec->SaveImage(CW2A(path));
	//auto size = m_rec->getEncodeSize();
	//int ch = m_pDepthMap->getContext().waveNum;
	//m_pDepthMap->save(CW2A(path), 8 * ch, nullptr, size[_X], size[_Y]);

	pParent->OpenExplorer(path);
	return TRUE;
}


BOOL CTab_RECON::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	((CButton *)GetDlgItem(IDC_RADIO_FOCUS))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_CHECK_X))->SetCheck(TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CTab_RECON::OnBnClickedRadioPosition()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_CHECK_X)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_Y)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_Z)->EnableWindow(TRUE);
}


void CTab_RECON::OnBnClickedRadioFocus()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_CHECK_X)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_Y)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_Z)->EnableWindow(FALSE);
}

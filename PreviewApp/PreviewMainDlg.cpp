
// PreviewMainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PreviewApp.h"
#include "PreviewMainDlg.h"
#include "afxdialogex.h"
#include <string>
#include <iostream>
#include <vector>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPreviewMainDlg dialog



CPreviewMainDlg::CPreviewMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPreviewMainDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPreviewMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPreviewMainDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CPreviewMainDlg::OnBnClickedButtonOpen)
END_MESSAGE_MAP()


// CPreviewMainDlg message handlers

BOOL CPreviewMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	pGenerator = PreviewGenerator(previewWindow);
	//previewWindow->ShowWindow(SW_SHOW);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPreviewMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPreviewMainDlg::OnSize(UINT nType, int w, int h)
{
	
}
void CPreviewMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPreviewMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPreviewMainDlg::OnBnClickedButtonOpen()
{
	
	previewWindow = GetDlgItem(IDC_PREVIEW_BOX);
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE);
	CString fileNameFull;
	CString fileExt;
	
	if (dlg.DoModal() == IDOK)
	{
		fileNameFull = dlg.GetPathName();		
		fileExt = dlg.GetFileExt();
		IStream	*iStream;
		GetStreamFromPath(fileNameFull, &iStream);
		previewWindow->ShowWindow(SW_HIDE);
		pGenerator.previewControl = previewWindow;
		HRESULT hr = pGenerator.BuildPreview(iStream, fileExt);

		//BuildPreview(iStream, fileExt);
	}
	previewWindow->ShowWindow(SW_SHOW);
}
HRESULT CPreviewMainDlg::GetStreamFromPath(CString filePath, IStream **ppStream)
{
	if (NULL != hGlobal)
	{
		GlobalFree(hGlobal);
		hGlobal = NULL;
	}
	HRESULT hr = E_FAIL;
	HANDLE hFile = CreateFile(filePath, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwSize = GetFileSize(hFile, NULL);
		hGlobal = GlobalAlloc(GPTR, dwSize);
		BYTE * pByte = (BYTE *)GlobalLock(hGlobal);
		if (pByte)
		{
			ReadFile(hFile, pByte, dwSize, &dwSize, NULL);
			GlobalUnlock(hGlobal);
			hr = CreateStreamOnHGlobal(hGlobal, TRUE, ppStream);
		}
		CloseHandle(hFile);
	}
	return hr;
}












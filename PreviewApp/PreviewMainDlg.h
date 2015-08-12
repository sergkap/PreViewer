
// PreviewMainDlg.h : header file
//
#include <vector>
#include "PreviewGenerator.h"
//// Media Foundation 
//#include <mfapi.h>
//#include <mfidl.h>
//#include <mfreadwrite.h>
//#include <mferror.h>

#pragma once
#define SAFERELEASE( X ) { if( X ) { X->Release(); X = NULL; } }
using namespace std;
// CPreviewMainDlg dialog
class CPreviewMainDlg : public CDialogEx
{
// Construction
public:
	CPreviewMainDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PREVIEWAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int w, int h);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpen();
private:
	CWnd					*previewWindow;
	PreviewGenerator		pGenerator;
	HRESULT CPreviewMainDlg::GetStreamFromPath(CString filePath, IStream **ppStream);
	HGLOBAL					hGlobal;
};

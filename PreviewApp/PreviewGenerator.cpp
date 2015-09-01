#include "stdafx.h"
#include "PreviewGenerator.h"
#include <wingdi.h>


PreviewGenerator::PreviewGenerator()
{
	iPHandler = NULL;
}


PreviewGenerator::~PreviewGenerator()
{
}

PreviewGenerator::PreviewGenerator(CWnd *pCntrl)
{
	previewControl = pCntrl;
	iPHandler = NULL;
}
//Public
HRESULT PreviewGenerator::BuildPreview(IStream *iStream, CString fileExt)
{
	HRESULT hr = E_FAIL;
//	std::vector<CString> res;
	GetClsidsFromExt(fileExt, clsidList);

	for (int i = 0; i < clsidList.size(); i++)
	{
		CLSID cls;
		CLSIDFromString((LPWSTR)(LPCTSTR)clsidList[i], &cls);
		hr = ShowPreviewWithPreviewHandler(iStream, cls);
		if (hr == S_OK)
			return hr;
		hr = ShowPreviewWithThumbnailProvider(iStream, cls);
		if (hr == S_OK)
			return hr;

		//hr = ShowPreviewWithPreviewHandler(filePath, cls);
		//if (hr == S_OK)
		//	return hr;
		//hr = ShowPreviewWithThumbnailProvider(filePath, cls);
		//if (hr == S_OK)
		//	return hr;

	}
	return hr;
}
HRESULT PreviewGenerator::BuildPreview(CString filePath, CString fileExt)
{
	HRESULT hr = E_FAIL;
	//std::vector<CString> res;
	//GetClsidsFromExt(fileExt, res);

	for (int i = 0; i < clsidList.size(); i++)
	{
		CLSID cls;
		CLSIDFromString((LPWSTR)(LPCTSTR)clsidList[i], &cls);
		//hr = ShowPreviewWithPreviewHandler(iStream, cls);
		//if (hr == S_OK)
		//	return hr;
		//hr = ShowPreviewWithThumbnailProvider(iStream, cls);
		//if (hr == S_OK)
		//	return hr;

		hr = ShowPreviewWithPreviewHandler(filePath, cls);
		if (hr == S_OK)
			return hr;
		hr = ShowPreviewWithThumbnailProvider(filePath, cls);
		if (hr == S_OK)
			return hr;

	}
	return hr;
}


//Private


void PreviewGenerator::GetClsidsFromExt(CString ext, std::vector<CString> &retVal)
{
	std::vector<CString> res;
	DWORD size = 1024;
	TCHAR buff[1024];
	LPCWSTR extra = L"{8895B1C6-B41F-4C1C-A562-0D564250836F}";
	CString cs = L"";
	cs.Format(L".%s", ext);
	HRESULT hr = AssocQueryString(ASSOCF_VERIFY, ASSOCSTR_SHELLEXTENSION, cs, extra, buff, &size);
	if (hr == S_OK)
		res.push_back(buff);
	extra = L"{BB2E617C-0920-11d1-9A0B-00C04FC2D6C1}";
	hr = AssocQueryString(ASSOCF_VERIFY, ASSOCSTR_SHELLEXTENSION, cs, extra, buff, &size);
	if (hr == S_OK)
	{
		std::vector<CString>::iterator it = find(res.begin(), res.end(), buff);
		if (it == res.end())
			res.push_back(buff);
	}
	extra = L"{e357fccd-a995-4576-b01f-234630154e96}";
	hr = AssocQueryString(ASSOCF_VERIFY, ASSOCSTR_SHELLEXTENSION, cs, extra, buff, &size);
	if (hr == S_OK)
	{
		std::vector<CString>::iterator it = find(res.begin(), res.end(), buff);
		if (it == res.end())
			res.push_back(buff);
	}
	extra = L"{534A1E02-D58F-44f0-B58B-36CBED287C7C}";
	res.push_back(extra);
	//hr = AssocQueryString(ASSOCF_VERIFY, ASSOCSTR_SHELLEXTENSION, cs, extra, buff, &size);
	//if (hr == S_OK)
	//{
	//	std::vector<CString>::iterator it = find(res.begin(), res.end(), buff);
	//	if (it == res.end())
	//		res.push_back(buff);
	//}
	
	PERCEIVED     ptype;
	PERCEIVEDFLAG pflag;
	PWSTR         ppszType;
	cs = L"";
	WCHAR wcData[MAX_PATH];
	LONG cData = sizeof(wcData);
	cs.Format(L".%s", ext);
	hr = RegQueryValue(HKEY_CLASSES_ROOT, cs, wcData, &cData);
	if (hr == S_OK)
	{
		cs.Format(L"%s\\shellex\\{8895b1c6-b41f-4c1c-a562-0d564250836f}", wcData);
		cData = sizeof(wcData);
		hr = RegQueryValue(HKEY_CLASSES_ROOT, cs, wcData, &cData);
		if (hr == S_OK)
			res.push_back(wcData);
	}

	cs.Format(L".%s", ext);
	hr = AssocGetPerceivedType(cs, &ptype, &pflag, &ppszType);
	if (hr == S_OK)
	{
		cs.Format(L"SystemFileAssociations\\%s\\ShellEx\\{e357fccd-a995-4576-b01f-234630154e96}", ppszType);
		hr = RegQueryValue(HKEY_CLASSES_ROOT, cs, wcData, &cData);
		if (hr == S_OK)
		{
			std::vector<CString>::iterator it = find(res.begin(), res.end(), wcData);
			if (it == res.end())
				res.push_back(wcData);
		}

		cs.Format(L"SystemFileAssociations\\%s\\ShellEx\\{BB2E617C-0920-11d1-9A0B-00C04FC2D6C1}", ppszType);
		hr = RegQueryValue(HKEY_CLASSES_ROOT, cs, wcData, &cData);
		if (hr == S_OK)
		{
			std::vector<CString>::iterator it = find(res.begin(), res.end(), wcData);
			if (it == res.end())
				res.push_back(wcData);
		}
		cs.Format(L"SystemFileAssociations\\%s\\ShellEx\\ContextMenuHandlers\\ShellVideoSlideshow", ppszType);
		hr = RegQueryValue(HKEY_CLASSES_ROOT, cs, wcData, &cData);
		if (hr == S_OK)
		{
			std::vector<CString>::iterator it = find(res.begin(), res.end(), wcData);
			if (it == res.end())
				res.push_back(wcData);
		}
	}

	retVal.swap(res);
}

/*************************************************************************************/
/********************************WITH IStream*****************************************/
/*************************************************************************************/
HRESULT PreviewGenerator::DoPreview()
{
	HRESULT hr;
	previewControl->ShowWindow(SW_HIDE);
	RECT	pRect;
	previewControl->GetWindowRect(&pRect);
	pRect.right -= (pRect.left + 10);
	pRect.bottom -= (pRect.top + 10);
	pRect.top = 10;
	pRect.left = 5;
	
	hr = iPHandler->SetWindow(previewControl->m_hWnd, &pRect);
	hr = iPHandler->DoPreview();
	previewControl->ShowWindow(SW_SHOW);
	return hr;
}

HRESULT PreviewGenerator::ShowPreviewWithPreviewHandler(IStream *stream, CLSID cls)
{
	DWORD dwRet = ERROR_BAD_ARGUMENTS;
	
	if (iPHandler != NULL)
	{
		iPHandler->Unload();
		SAFERELEASE(iPHandler);
	}
	IInitializeWithStream	*iIStream;
	HRESULT hr = CoCreateInstance(cls, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IPreviewHandler, (LPVOID*)&iPHandler);
	if (hr == S_OK)
	{
		hr = iPHandler->QueryInterface(IID_IInitializeWithStream, (LPVOID*)&iIStream);
		if (hr != S_OK)
			return hr;
	}
	else
		return hr;
	if (iIStream)
	{
		hr = iIStream->Initialize(stream, STGM_READ);
		if (hr != S_OK)
			return hr;
	}
	if (iPHandler)
		return DoPreview();
	return E_FAIL;
}
HRESULT PreviewGenerator::ShowPreviewWithThumbnailProvider(IStream *stream, CLSID cls)
{
	IThumbnailProvider *iTP;
	IInitializeWithStream	*iIStream;

	HRESULT hr = CoCreateInstance(cls, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IThumbnailProvider, (LPVOID*)&iTP);
	if (hr == S_OK)
	{
		hr = iTP->QueryInterface(IID_IInitializeWithStream, (LPVOID*)&iIStream);
		IInitializeWithFile *pInitFile;
		hr = iTP->QueryInterface(IID_IInitializeWithFile, (void**)&pInitFile);
		if (hr != S_OK)
			return hr;
		if (iIStream)
		{
			hr = iIStream->Initialize(stream, STGM_READ);
			HBITMAP bmp;
			WTS_ALPHATYPE type;
			CRect pRect;
			previewControl->GetWindowRect(pRect);
			int size = pRect.Width();
			if (pRect.Height()<pRect.Width())
				size = pRect.Height();

			hr = iTP->GetThumbnail(size, &bmp, &type);
			if (hr == S_OK)
			{
				DrawBitMap(bmp);
			}
			else
				return hr;
		}
		else
			return E_FAIL;
	}
	else
		return hr;
	return S_OK;
}

void PreviewGenerator::DrawBitMap(HBITMAP bmp)
{
	PAINTSTRUCT 	*ps;
	HDC 			hdcMem;
	HGDIOBJ 		oldBitmap;
	CRect rc;
	previewControl->GetWindowRect(rc);
	previewControl->ShowWindow(SW_HIDE);
	HDC hdc = previewControl->GetDC()->GetSafeHdc();
	

	hdcMem = CreateCompatibleDC(hdc);
	oldBitmap = SelectObject(hdcMem, bmp);
	SIZE newSize = rc.Size();
	BITMAP bmpInfo;
	GetObject(bmp, sizeof(bmpInfo), &bmpInfo);
	SIZE bitmapSize;
	bitmapSize.cx = bmpInfo.bmWidth;
	bitmapSize.cy = bmpInfo.bmHeight;

	int X = 0;
	int Y = 0;

	X = rc.Width() / 2;
	Y += rc.Height() / 2;
	X -= bitmapSize.cx / 2;
	Y -= bitmapSize.cy / 2;
	previewControl->ShowWindow(SW_SHOW);
	BitBlt(hdc, X, Y, newSize.cx, newSize.cy, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteDC(hdcMem);
	
}
HRESULT PreviewGenerator::ShowPreviewWithPreviewHandler(CString filePath, CLSID cls)
{
	DWORD dwRet = ERROR_BAD_ARGUMENTS;
	IInitializeWithFile		*iIFile;
	if (iPHandler != NULL)
	{
		iPHandler->Unload();
		SAFERELEASE(iPHandler);
	}

	HRESULT hr = CoCreateInstance(cls, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IPreviewHandler, (LPVOID*)&iPHandler);
	if (hr == S_OK)
	{
		hr = iPHandler->QueryInterface(IID_IInitializeWithFile, (LPVOID*)&iIFile);
		if (hr != S_OK)
			return hr;
	}
	else
		return hr;
	if (iIFile)
		hr = iIFile->Initialize(filePath, STGM_READ);

	if (iPHandler)
		return DoPreview();
	
	return E_FAIL;
}

HRESULT PreviewGenerator::ShowPreviewWithThumbnailProvider(CString filePath, CLSID clsID)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	IShellItemImageFactory *imageFactory;
	hr = SHCreateItemFromParsingName(filePath, NULL, IID_PPV_ARGS(&imageFactory));
	if (hr == S_OK)
	{
		CRect pRect;
		previewControl->GetWindowRect(pRect);

		int s = pRect.Height();
		if (s>pRect.Width())
			s = pRect.Width();
		SIZE size = { s, s };
		HBITMAP btmap;
		hr = imageFactory->GetImage(size, SIIGBF_RESIZETOFIT, &btmap);
		if (hr == S_OK)
		{
			DrawBitMap(btmap);
		}
		imageFactory->Release();

	}
	CoUninitialize();
	return hr;
	/*
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	int nSize = 256;
	HBITMAP g_hThumbnail;
	IShellItem *psi;
	hr = SHCreateItemFromParsingName(filePath, NULL, IID_PPV_ARGS(&psi));
	if (hr == S_OK)
	{
		IThumbnailProvider *pThumbProvider;
		hr = psi->BindToHandler(NULL, clsID, IID_PPV_ARGS(&pThumbProvider));
		if (hr == S_OK)
		{
			WTS_ALPHATYPE wtsAlpha;
			hr = pThumbProvider->GetThumbnail(nSize, &g_hThumbnail, &wtsAlpha);
		}
		else
		{
			
		}
	}
	return hr;
	*/
}








/*
HRESULT CPreviewMainDlg::BuildPreview(IStream *iStream, CString fileExt)
{
	HRESULT hr = E_FAIL;
	std::vector<CString> res;
	GetClsidsFromExt(fileExt, res);
	
	for (int i = 0; i < res.size(); i++)
	{
		CLSID cls;
		CLSIDFromString((LPWSTR)(LPCTSTR)res[i], &cls);
		hr = ShowPreviewWithPreviewHandler(iStream, cls);
		if (hr == S_OK)
			return hr;
		hr = ShowPreviewWithThumbnailProvider(iStream, cls);
		if (hr == S_OK)
			return hr;

		//hr = ShowPreviewWithPreviewHandler(filePath, cls);
		//if (hr == S_OK)
		//	return hr;
		//hr = ShowPreviewWithThumbnailProvider(filePath, cls);
		//if (hr == S_OK)
		//	return hr;

		//CreateThumbnailFromPath(filePath);
	}
	return hr;
}

void CPreviewMainDlg::GetClsidsFromExt(CString ext, std::vector<CString> &retVal)
{
	std::vector<CString> res;
	DWORD size = 1024;
	TCHAR buff[1024];
	LPCWSTR extra = L"{8895B1C6-B41F-4C1C-A562-0D564250836F}";
	CString cs = L"";
	cs.Format(L".%s", ext);
	HRESULT hr = AssocQueryString(ASSOCF_VERIFY, ASSOCSTR_SHELLEXTENSION, cs, extra, buff, &size);
	if (hr == S_OK)
		res.push_back(buff);
	extra = L"{BB2E617C-0920-11d1-9A0B-00C04FC2D6C1}";
	hr = AssocQueryString(ASSOCF_VERIFY, ASSOCSTR_SHELLEXTENSION, cs, extra, buff, &size);
	if (hr == S_OK)
	{
		std::vector<CString>::iterator it = find(res.begin(), res.end(), buff);
		if (it == res.end())
			res.push_back(buff);
	}
	extra = L"{e357fccd-a995-4576-b01f-234630154e96}";
	hr = AssocQueryString(ASSOCF_VERIFY, ASSOCSTR_SHELLEXTENSION, cs, extra, buff, &size);
	if (hr == S_OK)
	{
		std::vector<CString>::iterator it = find(res.begin(), res.end(), buff);
		if (it == res.end())
			res.push_back(buff);
	}

	PERCEIVED     ptype;
	PERCEIVEDFLAG pflag;
	PWSTR         ppszType;
	cs = L"";
	WCHAR wcData[MAX_PATH];
	LONG cData = sizeof(wcData);
	cs.Format(L".%s", ext);
	hr = RegQueryValue(HKEY_CLASSES_ROOT, cs, wcData, &cData);
	if (hr == S_OK)
	{
		cs.Format(L"%s\\shellex\\{8895b1c6-b41f-4c1c-a562-0d564250836f}", wcData);
		cData = sizeof(wcData);
		hr = RegQueryValue(HKEY_CLASSES_ROOT, cs, wcData, &cData);
		if (hr == S_OK)
			res.push_back(wcData);
	}

	cs.Format(L".%s", ext);
	hr = AssocGetPerceivedType(cs, &ptype, &pflag, &ppszType);
	if (hr == S_OK)
	{
		cs.Format(L"SystemFileAssociations\\%s\\ShellEx\\{e357fccd-a995-4576-b01f-234630154e96}", ppszType);
		hr = RegQueryValue(HKEY_CLASSES_ROOT, cs, wcData, &cData);
		if (hr == S_OK)
		{
			std::vector<CString>::iterator it = find(res.begin(), res.end(), wcData);
			if (it == res.end())
				res.push_back(wcData);
		}

		cs.Format(L"SystemFileAssociations\\%s\\ShellEx\\{BB2E617C-0920-11d1-9A0B-00C04FC2D6C1}", ppszType);
		hr = RegQueryValue(HKEY_CLASSES_ROOT, cs, wcData, &cData);
		if (hr == S_OK)
		{
			std::vector<CString>::iterator it = find(res.begin(), res.end(), wcData);
			if (it == res.end())
				res.push_back(wcData);
		}
		cs.Format(L"SystemFileAssociations\\%s\\ShellEx\\ContextMenuHandlers\\ShellVideoSlideshow", ppszType);
		hr = RegQueryValue(HKEY_CLASSES_ROOT, cs, wcData, &cData);
		if (hr == S_OK)
		{
			std::vector<CString>::iterator it = find(res.begin(), res.end(), wcData);
			if (it == res.end())
				res.push_back(wcData);
		}
	}

	retVal.swap(res);
}


HRESULT CPreviewMainDlg::DoPreview()
{
	HRESULT hr;
	previewWindow->ShowWindow(SW_HIDE);
	RECT	pRect;
	previewWindow->GetWindowRect(&pRect);
	pRect.right -= (pRect.left + 10);
	pRect.bottom -= (pRect.top + 10);
	pRect.top = 10;
	pRect.left = 5;
	previewWindow->ShowWindow(SW_SHOW);
	hr = iPHandler->SetWindow(previewWindow->m_hWnd, &pRect);
	hr = iPHandler->DoPreview();
	return hr;
}

HRESULT CPreviewMainDlg::ShowPreviewWithPreviewHandler(IStream *stream, CLSID cls)
{
	DWORD dwRet = ERROR_BAD_ARGUMENTS;
	previewWindow->ShowWindow(SW_HIDE);
	SAFERELEASE(iIStream);
	if (iPHandler != NULL)
	{
		iPHandler->Unload();
		SAFERELEASE(iPHandler);
	}

	HRESULT hr = CoCreateInstance(cls, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IPreviewHandler, (LPVOID*)&iPHandler);
	if (hr == S_OK)
	{
		hr = iPHandler->QueryInterface(IID_IInitializeWithStream, (LPVOID*)&iIStream);
		if (hr != S_OK)
			return hr;
	}
	else
		return hr;
	if (iIStream)
	{
		hr = iIStream->Initialize(stream, STGM_READ);
		if (hr != S_OK)
			return hr;
	}
	if (iPHandler)
		return DoPreview();
	return E_FAIL;
}
HRESULT CPreviewMainDlg::ShowPreviewWithThumbnailProvider(IStream *stream, CLSID cls)
{
	IThumbnailProvider *iTP;
	
	HRESULT hr = CoCreateInstance(cls, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IThumbnailProvider, (LPVOID*)&iTP);
	if (hr == S_OK)
	{
		hr = iTP->QueryInterface(IID_IInitializeWithStream, (LPVOID*)&iIStream);
		IInitializeWithFile *pInitFile;
		hr = iTP->QueryInterface(IID_IInitializeWithFile, (void**)&pInitFile);
		if (hr != S_OK)
			return hr;
		if (iIStream)
		{
			hr = iIStream->Initialize(stream, STGM_READ);
			HBITMAP bmp;
			WTS_ALPHATYPE type;
			CRect pRect;
			previewWindow->GetWindowRect(pRect);
			int size = pRect.Width();
			if (pRect.Height()<pRect.Width())
				size = pRect.Height();

			hr = iTP->GetThumbnail(size, &bmp, &type);
			if (hr == S_OK)
			{
				DrawBitMap(bmp);
			}
			else
				return hr;
		}
		else
			return E_FAIL;
	}
	else
		return hr;
	return S_OK;
}

HRESULT CPreviewMainDlg::GetThumbnailFromVideoFile(IStream *stream)
{
	IMFSourceReader *m_pReader;
	IMFByteStream *ppByteStream;
	IMFAttributes *pAttributes;
	HRESULT hr = MFCreateAttributes(&pAttributes, 1);
	if (SUCCEEDED(hr))
	{
		hr = pAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
	}
	hr = MFCreateMFByteStreamOnStream(stream, &ppByteStream);
	if (SUCCEEDED(hr))
	{
		hr = MFCreateSourceReaderFromByteStream(ppByteStream, NULL, &m_pReader);
		if (SUCCEEDED(hr))
		{
			DWORD dwMediaTypeIndex = 0;
			DWORD dwStreamIndex = MF_SOURCE_READER_FIRST_VIDEO_STREAM;
			while (SUCCEEDED(hr))
			{
				IMFMediaType *pType = NULL;
				hr = m_pReader->GetNativeMediaType(dwStreamIndex, dwMediaTypeIndex, &pType);
				if (hr == MF_E_NO_MORE_TYPES)
				{
					hr = S_OK;
					break;
				}
				else if (SUCCEEDED(hr))
				{
					pType->Release();
				}
				++dwMediaTypeIndex;
			}
		}
	}
	return S_OK;
}



HRESULT CPreviewMainDlg::ShowPreviewWithPreviewHandler(CString filePath, CLSID cls)
{
	previewWindow->ShowWindow(SW_HIDE);
	DWORD dwRet = ERROR_BAD_ARGUMENTS;
	SAFERELEASE(iIFile);
	if (iPHandler != NULL)
	{
		iPHandler->Unload();
		SAFERELEASE(iPHandler);
	}

	HRESULT hr = CoCreateInstance(cls, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IPreviewHandler, (LPVOID*)&iPHandler);
	if (hr == S_OK)
	{
		hr = iPHandler->QueryInterface(IID_IInitializeWithFile, (LPVOID*)&iIFile);
		if (hr != S_OK)
			return hr;
	}
	else
		return hr;
	if (iIFile)
		hr = iIFile->Initialize(filePath, STGM_READ);

	if (iPHandler)
		return DoPreview();
	return E_FAIL;
}

HRESULT CPreviewMainDlg::ShowPreviewWithThumbnailProvider(CString filePath, CLSID clsID)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	int nSize = 256;
	HBITMAP g_hThumbnail;
	IShellItem *psi;
	hr = SHCreateItemFromParsingName(filePath, NULL, IID_PPV_ARGS(&psi));
	if (hr == S_OK)
	{
		IThumbnailProvider *pThumbProvider;
		hr = psi->BindToHandler(NULL, clsID, IID_PPV_ARGS(&pThumbProvider));
		if (hr == S_OK)
		{
			WTS_ALPHATYPE wtsAlpha;
			hr = pThumbProvider->GetThumbnail(nSize, &g_hThumbnail, &wtsAlpha);
		}
	}
	return E_FAIL;
}

void CPreviewMainDlg::CreateThumbnailFromPath(CString File)
{
	IBindCtx *m_pBindContext;
	IShellItemImageFactory *pShellItemImageFactory = NULL;
	HRESULT hr = CreateBindCtx(0, &m_pBindContext);
	hr = SHCreateItemFromParsingName(
		File,
		m_pBindContext,
		IID_PPV_ARGS(&pShellItemImageFactory)
		);
	if (FAILED(hr))
	{
		return;
	}
	HBITMAP iconImage = NULL;
	SIZE iconSize;
	iconSize.cx = 300;
	iconSize.cy = 300;
	hr = pShellItemImageFactory->GetImage(iconSize, 0x0, &iconImage);
	if (FAILED(hr))
	{
		return;
	}

	BITMAPINFO bi = { 0 };
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	// Get the bitmap info header.
	HDC memoryDC = CreateCompatibleDC(NULL);
	if (0 == GetDIBits(
		memoryDC,   // hdc
		iconImage,  // hbmp
		0,          // uStartScan
		0,          // cScanLines
		NULL,       // lpvBits
		&bi,
		DIB_RGB_COLORS
		))
	{
		hr = E_FAIL;
		return;
	}
	DrawBitMap(iconImage);
}

void CPreviewMainDlg::DrawBitMap(HBITMAP bmp)
{
	PAINTSTRUCT 	*ps;
	HDC 			hdcMem;
	HGDIOBJ 		oldBitmap;
	CRect rc;
	previewWindow->GetWindowRect(rc);
	previewWindow->ShowWindow(SW_HIDE);
	HDC hdc = previewWindow->GetDC()->GetSafeHdc();
	previewWindow->ShowWindow(SW_SHOW);

	hdcMem = CreateCompatibleDC(hdc);
	oldBitmap = SelectObject(hdcMem, bmp);
	SIZE newSize = rc.Size();
	BITMAP bmpInfo;
	GetObject(bmp, sizeof(bmpInfo), &bmpInfo);
	SIZE bitmapSize;
	bitmapSize.cx = bmpInfo.bmWidth;
	bitmapSize.cy = bmpInfo.bmHeight;

	int X = 0;
	int Y = 0;

	X = rc.Width() / 2;
	Y += rc.Height() / 2;
	X -= bitmapSize.cx / 2;
	Y -= bitmapSize.cy / 2;

	BitBlt(hdc, X, Y, newSize.cx, newSize.cy, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteDC(hdcMem);
}


SIZE repairSize(SIZE controlSize, HBITMAP bmp)
{

	BITMAP bmpInfo;
	GetObject(bmp, sizeof(bmpInfo), &bmpInfo);
	SIZE bitmapSize;
	bitmapSize.cx = bmpInfo.bmWidth;
	bitmapSize.cy = bmpInfo.bmHeight;
	SIZE newSize;
	int maxW = controlSize.cx - 10;
	int maxH = controlSize.cy - 10;
	float scale = 1.0;
	if (bitmapSize.cy > bitmapSize.cx)
	{
		scale = (float)bitmapSize.cy / (float)controlSize.cy;
		maxW = bitmapSize.cx / scale;
	}
	else
	{
		scale = (float)bitmapSize.cx / ((float)controlSize.cx);
		maxH = bitmapSize.cy / scale;
	}
	newSize.cx = maxW;
	newSize.cy = maxH;
	if (newSize.cx > controlSize.cx || newSize.cy > controlSize.cy)
	{
		if (newSize.cy > controlSize.cy)
		{
			scale = (float)bitmapSize.cy / ((float)controlSize.cy);
			maxW = bitmapSize.cx / scale;
			maxH = controlSize.cy - 10;
		}
		else
		{
			scale = (float)bitmapSize.cx / ((float)controlSize.cx);
			maxH = bitmapSize.cy / scale;
			maxW = controlSize.cx;
		}
	}
	newSize.cx = maxW;
	newSize.cy = maxH;
	return newSize;
}
HBITMAP ScaleBitmapInt(HBITMAP hBitmap,
	WORD wNewWidth,
	WORD wNewHeight)
{

	// Create a memory DC compatible with the display
	CDC sourceDC, destDC;
	sourceDC.CreateCompatibleDC(NULL);
	destDC.CreateCompatibleDC(NULL);

	// Get logical coordinates
	BITMAP bm;
	::GetObject(hBitmap, sizeof(bm), &bm);

	// Create a bitmap to hold the result
	HBITMAP hbmResult = ::CreateCompatibleBitmap(CClientDC(NULL),
		wNewWidth, wNewHeight);

	// Select bitmaps into the DCs
	HBITMAP hbmOldSource = (HBITMAP)::SelectObject(sourceDC.m_hDC, hBitmap);
	HBITMAP hbmOldDest = (HBITMAP)::SelectObject(destDC.m_hDC, hbmResult);



	destDC.StretchBlt(0, 0, wNewWidth, wNewHeight, &sourceDC,
		0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

	// Reselect the old bitmaps
	::SelectObject(sourceDC.m_hDC, hbmOldSource);
	::SelectObject(destDC.m_hDC, hbmOldDest);

	return hbmResult;

}

/*
LSTATUS __stdcall RegGetStringValue(HKEY hKey, LPCTSTR lpszName, CString& rstrValue)
{
	DWORD dw = 0;
	LSTATUS ret = ::RegQueryValueEx(hKey, lpszName, NULL, NULL, NULL, &dw);
	if (ret != ERROR_SUCCESS)
	{
		rstrValue.Empty();
		return ret;
	}
	dw++;
	ret = ::RegQueryValueEx(hKey, lpszName, NULL, NULL, (LPBYTE)rstrValue.GetBuffer(dw), &dw);
	rstrValue.ReleaseBuffer(dw);
	return ret;
}
static bool __stdcall _GetThumbnailProviderCLSID(HKEY hKeyParent, CLSID* pclsid)
{
	HKEY hKey;
	bool bRet = false;
	if (::RegOpenKeyEx(hKeyParent, _T("ShellEx\\{E357FCCD-A995-4576-B01F-234630154E96}"), 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		CString str;
		if (::RegGetStringValue(hKey, NULL, str) == ERROR_SUCCESS)
			bRet = SUCCEEDED(::CLSIDFromString((LPOLESTR)(LPCOLESTR)str, pclsid));
		::RegCloseKey(hKey);
	}
	return bRet;
}
static bool __stdcall _GetThumbnailProviderCLSIDWithKeyName(HKEY hKeyParent, LPCTSTR lpszKey, CLSID* pclsid)
{
	HKEY hKey;
	bool bRet = false;
	if (::RegOpenKeyEx(hKeyParent, lpszKey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		bRet = _GetThumbnailProviderCLSID(hKey, pclsid);
		::RegCloseKey(hKey);
	}
	return bRet;
}

STDAPI MyCreateThumbnailProviderFromFileName(CString filePath, IThumbnailProvider** ppProvider)
{
	// extract ShellEx
	LPCWSTR lpszExt = wcsrchr(filePath, L'.');
	if (!lpszExt)
		lpszExt = L"Unknown";
	HKEY hKey;
	bool b = false;
	CLSID clsid;
	CString strKey = filePath;
	if (::RegOpenKeyEx(HKEY_CLASSES_ROOT, lpszExt, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		b = _GetThumbnailProviderCLSID(hKey, &clsid);
		if (!b)
		{
			if (::RegGetStringValue(hKey, NULL, strKey) == ERROR_SUCCESS)
				b = _GetThumbnailProviderCLSIDWithKeyName(HKEY_CLASSES_ROOT, strKey, &clsid);
		}
		::RegCloseKey(hKey);
	}
	if (!b)
		return E_FAIL;
	return ::CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IThumbnailProvider, (void**)ppProvider);
}
*/
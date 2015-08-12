#include <vector>
#pragma once
#define SAFERELEASE( X ) { if( X ) { X->Release(); X = NULL; } }
class PreviewGenerator
{
public:
	PreviewGenerator();
	PreviewGenerator(CWnd *pControl);
	~PreviewGenerator();
	
	CWnd					*previewControl;
	HRESULT PreviewGenerator::BuildPreview(IStream *iStream, CString fileExt);
private:
	void PreviewGenerator::DrawBitMap(HBITMAP bmp);
	void PreviewGenerator::GetClsidsFromExt(CString ext, std::vector<CString> &retVal);

	IPreviewHandler			*iPHandler;
	HRESULT PreviewGenerator::DoPreview();

	HRESULT PreviewGenerator::ShowPreviewWithPreviewHandler(IStream *stream, CLSID cls);
	HRESULT PreviewGenerator::ShowPreviewWithPreviewHandler(CString filePath, CLSID cls);
	HRESULT PreviewGenerator::ShowPreviewWithThumbnailProvider(IStream *stream, CLSID cls);
	HRESULT PreviewGenerator::ShowPreviewWithThumbnailProvider(CString filePath, CLSID clsID);

	


	/*CWnd					*pImage;
	IPreviewHandler			*iPHandler;
	IInitializeWithFile		*iIFile;
	IInitializeWithStream	*iIStream;

	//RECT					previewRect;

	HRESULT CPreviewMainDlg::DoPreview();

	HRESULT CPreviewMainDlg::BuildPreview(IStream *iStream, CString fileExt);
	HRESULT CPreviewMainDlg::GetStreamFromPath(CString filePath, IStream **ppStream);
	HRESULT CPreviewMainDlg::ShowPreviewWithPreviewHandler(IStream *stream, CLSID clsID);
	HRESULT CPreviewMainDlg::ShowPreviewWithPreviewHandler(CString filePath, CLSID clsID);
	HRESULT CPreviewMainDlg::ShowPreviewWithThumbnailProvider(IStream *stream, CLSID clsID);
	HRESULT CPreviewMainDlg::ShowPreviewWithThumbnailProvider(CString filePath, CLSID clsID);

	HRESULT CPreviewMainDlg::GetThumbnailFromVideoFile(IStream *stream);

	void CPreviewMainDlg::CreateThumbnailFromPath(CString File);
	void CPreviewMainDlg::GetClsidsFromExt(CString ext, std::vector<CString> &retVal);
	void CPreviewMainDlg::DrawBitMap(HBITMAP bmp);*/
};


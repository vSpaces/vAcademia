#include "StdAfx.h"
#include "../../include/DesktopMan.h"
#include "../../include/view/DrawWindow.h"
#include "../../include/view/viewstatic.h"
#include "../../include/IDesktopMan.h"
#include "../../Include/TextureLoader.h"
#include "../../Include/screenRes.h"

#include "../../include/selectors/WndSelector.h"
#include "../../include/view/drawwindow.h"

#include "../../VASEncoder/vase.h"
#include "../../VASEncoder/VASEBitmap.h"
#include "../../VASEncoder/VASEFramesDifPack.h"
#include "../../VASEncoder/ImageCompressor.h"

#include "..\..\Cscl3DWS\rm\rm.h"
#include "..\..\include\view\drawwindow.h"

#include <Uxtheme.h>

#define     SelectBitmap(hdc, hbm)  ((HBITMAP)SelectObject((hdc), (HGDIOBJ)(HBITMAP)(hbm)))
#define SETWTEXT(x,y) ::SetWindowTextW(x.m_hWnd,y);

CDrawWindow::CDrawWindow(oms::omsContext* apContext, bool aUseNewCodec):
MP_STRING_INIT(staticQualityEngText),
MP_STRING_INIT( staticResolutionEngText) ,
MP_STRING_INIT( staticDeviceEngText),
MP_STRING_INIT( windowCaptionEngText),
MP_STRING_INIT( buttonSetEngText),
MP_STRING_INIT( buttonCancelEngText)
{
	hBitmap = 0;
	useNewCodec = aUseNewCodec;
	context = apContext;
	m_lastResolutionNum = 0;
	m_lastResolution.width = 0;
	m_lastResolution.height = 0;
	m_pCurrentLayout = NULL;
	m_buttonSetRect.SetRect(0,0,0,0);
	m_buttonCancelRect.SetRect(0,0,0,0);
	m_selectedDeviceNum = 0;

	ptMinTrackSize.x = 465;
	ptMinTrackSize.y = 200;

	staticQualityEngText = "Quality:";
	staticResolutionEngText = "Resolution:";
	staticDeviceEngText = "Device:";
	windowCaptionEngText = "Preview";
	buttonSetEngText = "Apply";
	buttonCancelEngText = "Cancel";

	wchar_t wcsLanguage[100] = L"";
	context->mpApp->GetLanguage((wchar_t*)wcsLanguage, 99);
	if (!wcscmp(wcsLanguage, L"eng"))
		m_englishVersion = true;
	else
		m_englishVersion = false;
	m_resolutionNowChanging = false;

	m_layoutForWebCam = MP_NEW( CConfirmWebCamLayout);
	m_layoutForImage = MP_NEW( CConfirmLayout);
	m_layoutForTexture = MP_NEW( CConfirmTextureBoardLayout);

	m_currentMode = 0;
}

CDrawWindow::~CDrawWindow()
{
}

void CDrawWindow::OnFinalMessage(HWND /*hWnd*/)
{
	MP_DELETE_THIS;
}

void CDrawWindow::Release()
{
	if( hBitmap != 0)
	{
		DeleteObject( hBitmap);
		hBitmap = 0;
	}
	m_pCurrentLayout = NULL;

}

void CDrawWindow::UpdateViewPosition()
{
	CWindow wndView = GetDlgItem( IDC_VIEW_AREA);
	RECT rcView;
	wndView.GetWindowRect( &rcView);
	ScreenToClient( &rcView);
	mImageView.MoveWindow( &rcView, TRUE);
}

LRESULT CDrawWindow::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	UpdateViewPosition();
	bHandled = FALSE;
	Invalidate(TRUE);
	return 0;
}

LRESULT CDrawWindow::OnSizing(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
/*	RECT* lpRect = (RECT*)lParam;
	if( lpRect)
	{
		if( ((lpRect->right - lpRect->left)) % 2 == 1)	lpRect->right--;
		if( ((lpRect->bottom - lpRect->top)) % 2 == 1)	lpRect->bottom--;
	}
*/	return FALSE;
}

LRESULT CDrawWindow::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	HICON hIcon = LoadIcon(_Module.m_hInstResource, MAKEINTRESOURCE( IDD_CONFIRM_DIALOG));
	SetIcon( hIcon, TRUE);
	SetIcon( hIcon, FALSE);
	ModifyStyle(0, WS_CLIPCHILDREN);
	if( mImageView.Create( m_hWnd, NULL, NULL, WS_CHILD | WS_BORDER, 0))
	{
		mImageView.SetBitmap( hBitmap);
		mImageView.SetText("");
		mImageView.ShowWindow(SW_SHOW);
	}
	/*CTrackBarCtrl sliderCtrl( GetDlgItem( IDC_SLIDER_QUALITY));
	sliderCtrl.SetRange(0,99);
	sliderCtrl.SetTicFreq(10);
	sliderCtrl.SetPos( 30);*/

	CBitmap	backBitmap;
	staticQuality.SubclassWindow( GetDlgItem( IDC_STATIC_QUALITY));
	backBitmap.LoadBitmap( MAKEINTRESOURCE( IDB_BACKTEXTURE));
	staticQuality.m_hbrNormalBk.DeleteObject();
	staticQuality.m_clrNormalText = RGB(200,227,255);
	staticQuality.m_hbrNormalBk.CreatePatternBrush( backBitmap.m_hBitmap);

	staticResolution.SubclassWindow( GetDlgItem( IDC_STATIC_RESOLUTION));
	staticResolution.m_hbrNormalBk.DeleteObject();
	staticResolution.m_clrNormalText = RGB(200,227,255);
	staticResolution.m_hbrNormalBk.CreatePatternBrush( backBitmap.m_hBitmap);

	staticDevice.SubclassWindow( GetDlgItem( IDC_STATICDEVICE));
	staticDevice.m_hbrNormalBk.DeleteObject();
	staticDevice.m_clrNormalText = RGB(200,227,255);
	staticDevice.m_hbrNormalBk.CreatePatternBrush( backBitmap.m_hBitmap);

	sliderQuality.SubclassWindow( GetDlgItem( IDC_SLIDER_QUALITY));
	sliderQuality.m_hbrNormalBk.DeleteObject();
	sliderQuality.m_clrNormalText = RGB(200,227,255);
	sliderQuality.m_hbrNormalBk.CreatePatternBrush( backBitmap.m_hBitmap);
	sliderQuality.m_hbrDisabledBk.DeleteObject();
	sliderQuality.m_clrDisabledText = RGB(200,227,255);
	sliderQuality.m_hbrDisabledBk.CreatePatternBrush( backBitmap.m_hBitmap);
	sliderQuality.SetRange(1,20);
	sliderQuality.SetTicFreq(1);
	sliderQuality.SetPos( 9);

	buttonSet.SubclassWindow( GetDlgItem( IDOK));
	buttonSet.SetTextColor( RGB(0,35,99));

	buttonCancel.SubclassWindow( GetDlgItem( IDCANCEL));
	buttonCancel.SetTextColor( RGB(0,35,99));

	SetWindowTheme(GetDlgItem( IDC_RADIO_SCALE), L"", L"");
	SetWindowTheme(GetDlgItem( IDC_RADIO_FIT), L"", L"");

	scaleRadio.SubclassWindow( GetDlgItem( IDC_RADIO_FIT));
	scaleRadio.m_hbrNormalBk.DeleteObject();
	scaleRadio.m_clrNormalText = RGB(200,227,255);
	scaleRadio.m_clrDisabledText = RGB(200,227,255);
	scaleRadio.m_hbrNormalBk.CreatePatternBrush( backBitmap.m_hBitmap);
	scaleRadio.SetCheck(true);

	fitRadio.SubclassWindow( GetDlgItem( IDC_RADIO_SCALE));
	fitRadio.m_hbrNormalBk.DeleteObject();
	fitRadio.m_clrNormalText = RGB(200,227,255);
	scaleRadio.m_clrDisabledText = RGB(200,227,255);
	fitRadio.m_hbrNormalBk.CreatePatternBrush( backBitmap.m_hBitmap);
	

	if (m_englishVersion)
	{
		SETWTEXT(fitRadio, L"Fit");
		SETWTEXT(scaleRadio, L"Scale");
		SETWTEXT(staticQuality, L"Quality");
		SETWTEXT(staticResolution, L"Resolution:");
		SETWTEXT(staticDevice, L"Device:");
		SETWTEXT(buttonSet, L"Apply");
		SETWTEXT(buttonCancel, L"Cancel");
		::SetWindowText(m_hWnd, "Preview");

	}
	else
	{
		SETWTEXT(staticQuality, L"Качество:");
		SETWTEXT(staticResolution, L"Разрешение:");
		SETWTEXT(staticDevice, L"Устройство:");
		SETWTEXT(buttonSet, L"Установить");
		SETWTEXT(buttonCancel, L"Отменить");
		::SetWindowText(m_hWnd, "Предпросмотр");	
	}


	UpdatePreviewWithQuality();

	m_layoutForImage->Apply();
	m_layoutForWebCam->Apply();
	m_layoutForTexture->Apply();
	int mode = m_currentMode;
	SetLayout(NORMAL_PREVIEW_MODE);
	SetLayout(mode);
	
	SetBitmap( IDB_BACKTEXTURE);

	// Спрячем кнопку изменения размеров диалога
	HWND hwnd = ::GetWindow( m_hWnd, GW_CHILD | GW_HWNDFIRST );
	while( hwnd)
	{
		hwnd = ::GetWindow( hwnd, GW_HWNDNEXT );
		CComString className;
		if( ::GetClassName(hwnd, className.GetBufferSetLength(16), 15) && className == "ScrollBar")
				break;
	}
	if( hwnd)
		::ShowWindow( hwnd, SW_HIDE);

	CSize wndSize(512, 384);
	if( IsLargeFonts())
		wndSize = CSize (612, 384);
	SetWindowPos( 0, 0, 0, wndSize.cx, wndSize.cy, SWP_NOMOVE | SWP_SHOWWINDOW);
	UpdateViewPosition();
	bHandled = FALSE;
	return 0;
}

LRESULT CDrawWindow::OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	ShowWindow( SW_HIDE);
	int mode = SCALE_MODE_FIT;
	if (scaleRadio.GetCheck())
		mode = SCALE_MODE_SCALE;

	InvokeEx<int, &IPreviewWindowEvents::OnPreviewWindowOK>(mode);
	bHandled = TRUE;
	return 1;
}

LRESULT CDrawWindow::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ShowWindow( SW_HIDE);
	Invoke<&IPreviewWindowEvents::OnPreviewWindowCancel>();
	return 1;
}

LRESULT CDrawWindow::OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	ShowWindow( SW_HIDE);
	Invoke<&IPreviewWindowEvents::OnPreviewWindowCancel>();
	bHandled = TRUE;
	return 1;
}

LRESULT CDrawWindow::OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
	
	lpMMI->ptMinTrackSize =  ptMinTrackSize;
	return 1;
}

int CDrawWindow::GetQuality()
{
	return sliderQuality.GetPos()*5;
}

//BOOL SaveToFile(LPCSTR fn, HBITMAP hBitmap);
bool CDrawWindow::ShowPreview(IDesktopSelector* apSelector, IWebCameraSelector* webCamSelector)
{
	m_webCamSelector = webCamSelector;
	bool res = ShowPreview(apSelector);
	// если окно создалось успешно
	if (res)
		FillDeviceList();
	return res;
}

bool CDrawWindow::ShowPreview(IDesktopSelector* apSelector)
{
	if( !apSelector)
		return false;

	pIDesktopFrame = apSelector->GetCurrentFrame();
	if( pIDesktopFrame == NULL)
	{
		apSelector->RecapFrame();
		pIDesktopFrame = apSelector->GetCurrentFrame();
		if( pIDesktopFrame == NULL && m_webCamSelector == NULL)
			return false;
	}

	if (pIDesktopFrame != NULL)
	{
		HBITMAP hCapturedImage = pIDesktopFrame->GetHBitmap();
		if( hCapturedImage == 0)
			return false;

		HDC hScreenDC = ::GetDC( 0);
		if( hScreenDC)
		{
			HDC hBackHDC = CreateCompatibleDC( hScreenDC);
			if( hBackHDC)
			{
				HBITMAP oldBitmap = SelectBitmap( hBackHDC, hCapturedImage);
	//SaveToFile("c:\\1.bmp", hCapturedImage);
				BITMAPINFO bmpInfo;
				ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
				bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
				GetDIBits(hBackHDC,hCapturedImage,0,0,NULL,&bmpInfo,DIB_RGB_COLORS); 
				ShowBitmapFromHDC( hBackHDC, bmpInfo.bmiHeader.biWidth, bmpInfo.bmiHeader.biHeight);

				SelectBitmap( hBackHDC, oldBitmap);
				DeleteDC( hBackHDC);
			}
			::ReleaseDC( 0, hScreenDC);
		}
		DeleteObject(hCapturedImage);
	}
	else
	{ // даем пользователю выбор из нескольких устройств
		if( !IsWindow())
		{
			if( Create( GetActiveWindow(), 0))
			{
				ShowWindow( SW_SHOWNORMAL);
				CenterWindow( 0);
			}
		}
		SetWindowPos( HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

		UpdatePreviewWithQuality();
	}
	EnableOnWebCamChanged();
	mImageView.SetText("");

	if (pIDesktopFrame == NULL)
	{ // даем пользователю выбор из нескольких устройств
		if (m_englishVersion)
			mImageView.SetText("Please chose device");
		else
			mImageView.SetText("Пожалуйста, выберите устройство");
		DisableOnWebCamChanged();
		EnableAfterResolutionError();
	}

	CComboBox webcamList = GetDlgItem(IDC_DEVICELIST);
	CComboBox selectResolution = GetDlgItem(IDC_RESOLUTIONS);
	::ShowWindow( selectResolution, SW_HIDE);
	::ShowWindow( staticResolution, SW_HIDE);
	::ShowWindow( webcamList, SW_HIDE);
	::ShowWindow( staticDevice, SW_HIDE);
	
	
		
	return true;
}

void CDrawWindow::ShowBitmapFromHDC(HDC ahHDC, int aiWidth, int aiHeight)
{
	if( hBitmap != 0)
	{
		DeleteObject( hBitmap);
		hBitmap = 0;
	}
	HDC hScreenDC = ::GetDC( 0);
	if( hScreenDC)
	{
		HDC hBackHDC = CreateCompatibleDC( hScreenDC);
		if( hBackHDC)
		{
			hBitmap = CreateCompatibleBitmap( hScreenDC
				, aiWidth
				, aiHeight);

			HBITMAP hOldBitmap = SelectBitmap( hBackHDC, hBitmap);
			SetLastError(0);
			::BitBlt( hBackHDC, 0, 0, aiWidth, aiHeight, ahHDC, 0, 0, SRCCOPY);
			int ii=GetLastError();
			SelectBitmap( hBackHDC, hOldBitmap);
			DeleteDC( hBackHDC);
		}
		::ReleaseDC(0, hScreenDC);
	}

	if( !IsWindow())
	{
		if( Create( GetActiveWindow(), 0))
		{
			ShowWindow( SW_SHOWNORMAL);
			CenterWindow( 0);
		}
	}
	else
	{
		mImageView.SetBitmap( hBitmap);
		mImageView.ShowWindow(SW_SHOW);
		UpdatePreviewWithQuality();
	}
	SetWindowPos( HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

LRESULT CDrawWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDrawWindow::OnNMReleasedcaptureSliderQuality(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	UpdatePreviewWithQuality();
	return 0;
}

LRESULT CDrawWindow::OnQualityScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if( lParam == (LPARAM)sliderQuality.m_hWnd)
	{
		UpdateQualityLabel();
	}
	return 0;
}

void CDrawWindow::UpdateQualityLabel()
{
	int quality = GetQuality();
	CComString sQualityText;

	wchar_t wcsLanguage[100] = L"";
	context->mpApp->GetLanguage((wchar_t*)wcsLanguage, 99);
	if (m_englishVersion)
		sQualityText.Format("Quality: %d%%", quality);
	else
		sQualityText.Format("Качество: %d%%", quality);
	staticQuality.SetWindowText( sQualityText.GetBuffer());
}

HBITMAP CDrawWindow::CreateDIBSection(unsigned int auWidth, unsigned int auHeight, 
									  unsigned int auBPP, void** lppBits, bool abReverseHMode)
{
	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = auWidth;
	bmpInfo.bmiHeader.biHeight = abReverseHMode ? auHeight : -((int)auHeight);
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = auBPP;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	HDC screenDC = ::GetDC(0);
	HBITMAP hbm = ::CreateDIBSection(screenDC, &bmpInfo
		, DIB_RGB_COLORS,(void **)lppBits, NULL, 0);
	::ReleaseDC( 0, screenDC);

	return hbm;
}

void CDrawWindow::DoPaint(CDCHandle dc)
{
	ATLASSERT(FALSE);
}

void CDrawWindow::UpdatePreviewWithQuality()
{
	int quality = GetQuality();

	VASEBitmap	apBitmap;
	VASEBitmap	apEasyPackedBitmap;
	CTextureLoader  textureLoader;
	CImageCompressor compressor;

	if (pIDesktopFrame == NULL)
	{
		if( hBitmap != 0)
		{
			DeleteObject( hBitmap);
			hBitmap = 0;
		}
		void *p=NULL;
		hBitmap = CreateDIBSection(1, 1, 24, &p, true);
		mImageView.SetBitmap(hBitmap);
		UpdateQualityLabel();
		return;
	}

	if (!useNewCodec)
	{
		int channelsCount = 4; 
		compressor.CompressImageData(pIDesktopFrame->GetWidth(), pIDesktopFrame->GetHeight(), 
			channelsCount, pIDesktopFrame->GetData(), quality,	&apBitmap);

		const void* jpegData = NULL;
		unsigned int jpegDataSize = 0;
		apBitmap.GetBuffer( &jpegData, &jpegDataSize);

		textureLoader.LoadFromMemory( (void*)jpegData, jpegDataSize);

		if( hBitmap != 0)
		{
			DeleteObject( hBitmap);
			hBitmap = 0;
		}
		void* pBits = 0;
		hBitmap = CreateDIBSection (textureLoader.GetWidth(), textureLoader.GetHeight()
			, 24, &pBits, false);
		memcpy( pBits, textureLoader.GetData(), textureLoader.GetDataSize());

		mImageView.SetBitmap(hBitmap);
		mImageView.ShowWindow(SW_SHOW);
	}
	else
	{
		textureLoader.InitNewCodec(context->mpRM->GetTaskManager(), context);
		MP_NEW_P2( waveletEncoder, VASWaveletEncoder, context->mpRM->GetTaskManager(), context);
		void *p=NULL;
		const void *p2=NULL;
		unsigned int DataSize = 0;

		waveletEncoder->SetEncoderQuality(quality);

		if (!waveletEncoder->EncodeDIBBitmap(pIDesktopFrame->GetData(), pIDesktopFrame->GetWidth(), pIDesktopFrame->GetHeight()
			, 32, &apBitmap, &apEasyPackedBitmap))
			return;
		apBitmap.GetBuffer(&p2, &DataSize);
		//apEasyPackedBitmap.GetBuffer(&p2, &DataSize);
		if (!textureLoader.LoadFromMemoryNewCodec((void*)p2, DataSize, apEasyPackedBitmap.GetSize(), false))
			return;

		if( hBitmap != 0)
		{
			DeleteObject( hBitmap);
			hBitmap = 0;
		}
		hBitmap = CreateDIBSection(pIDesktopFrame->GetWidth(), pIDesktopFrame->GetHeight(), 24, &p, true);
		memcpy(p, textureLoader.GetWaveletsData(), textureLoader.GetWaveletsDataSize());
		mImageView.SetBitmap(hBitmap);
		mImageView.ShowWindow(SW_SHOW);
		SAFE_DELETE(waveletEncoder);
	}
	UpdateQualityLabel();
}

void CDrawWindow::DisableOnWebCamChanged()
{
	CComboBox selectResolution = GetDlgItem(IDC_RESOLUTIONS);
	selectResolution.EnableWindow(false);
	CComboBox deviceList = GetDlgItem(IDC_DEVICELIST);
	deviceList.EnableWindow(false);
	buttonSet.EnableWindow(false);
	sliderQuality.EnableWindow(false);
}
void CDrawWindow::EnableOnWebCamChanged()
{
	buttonSet.EnableWindow(true);
	sliderQuality.EnableWindow(true);
	CComboBox selectResolution = GetDlgItem(IDC_RESOLUTIONS);
	selectResolution.EnableWindow(true);
	CComboBox deviceList = GetDlgItem(IDC_DEVICELIST);
	deviceList.EnableWindow(true);
}
void CDrawWindow::EnableAfterResolutionError()
{
	CComboBox selectResolution = GetDlgItem(IDC_RESOLUTIONS);
	selectResolution.EnableWindow(true);
	CComboBox deviceList = GetDlgItem(IDC_DEVICELIST);
	deviceList.EnableWindow(true);
}


LRESULT CDrawWindow::OnSelectResolutionChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_englishVersion)
		mImageView.SetText("Please wait, changing resolution");
	else
		mImageView.SetText("Подождите идет смена разрешения");
	DisableOnWebCamChanged();
	CComboBox webcamResolutions = GetDlgItem(IDC_RESOLUTIONS);
	int selectNumber = webcamResolutions.GetCurSel();
	m_lastResolutionNum = selectNumber;
	m_resolutionNowChanging = true;
	std::vector<Resolution> supportedResolutions = m_webCamSelector->GetWebCameraResolutions(m_selectedDeviceNum);
	m_lastResolution = supportedResolutions[selectNumber];
	m_webCamSelector->SetResolution(supportedResolutions[selectNumber], m_selectedDeviceNum, true);
	return 0;
}

void CDrawWindow::FillDeviceList()
{
	if (!m_webCamSelector)
		return;
	CComboBox webcamList;
	webcamList = GetDlgItem(IDC_DEVICELIST);
	while (webcamList.GetCount() > 0)
	{
		webcamList.DeleteString(0);
	}
	for (int i=0; i < m_webCamSelector->GetWebNumsCameraDevices(); i++)
	{
		std::string str = m_webCamSelector->GetWebCameraName(i);
		webcamList.AddString(str.c_str());
	}
	if (pIDesktopFrame != NULL)
	{
		webcamList.SetCurSel(m_selectedDeviceNum);
		FillResolutionList(m_selectedDeviceNum);
	}
	else
		FillResolutionList(-1);
	::ShowWindow( webcamList, SW_SHOW);
	::ShowWindow( staticDevice, SW_SHOW);
}

void CDrawWindow::FillResolutionList(int numDevice)
{
	if (!m_webCamSelector)
		return;
	std::vector<Resolution> supportedResolutions;
	std::vector<Resolution>::iterator it;
	CComboBox webcamResolutionList;
	webcamResolutionList = GetDlgItem(IDC_RESOLUTIONS);
	while (webcamResolutionList.GetCount() > 0)
	{
		webcamResolutionList.DeleteString(0);
	}
	webcamResolutionList.AddString(" ");
	webcamResolutionList.DeleteString(0);
	
	if (m_webCamSelector->GetWebNumsCameraDevices() != 0 && numDevice >= 0)
	{
		supportedResolutions = m_webCamSelector->GetWebCameraResolutions(numDevice);
		if (supportedResolutions.size() != 0)
		{
			for (it = supportedResolutions.begin(); it != supportedResolutions.end(); it++)
			{
				char buf[100];
				std::string str;
				_itoa_s((*it).width, buf, 100, 10);
				str += buf;
				str += "x";
				_itoa_s((*it).height, buf, 100, 10);
				str += buf;
				webcamResolutionList.AddString(str.c_str());
			}
			webcamResolutionList.SetCurSel(m_lastResolutionNum);
		}
	}
	::ShowWindow( webcamResolutionList, SW_SHOW);
	::ShowWindow( staticResolution, SW_SHOW);
}

void CDrawWindow::OnSetResolutionCompleted(IDesktopSelector* apSelector, int resultCode)
{
	int SUCCESSFULL = 0x01;
	int INIT_DIRECTSHOW_ERROR = 0x02;
	int INIT_WEBCAM_ERROR = 0x03;
	int ENUMERATE_RESOLUTION_ERROR = 0x04;
	int WEBCAMERA_NOTFOUND = 0x05;
	int WEBCAMERA_NOTRESPONCE = 0x06;

	if (resultCode == SUCCESSFULL)
		mImageView.SetText("");
	else if (resultCode == INIT_DIRECTSHOW_ERROR)
	{
		if (m_englishVersion)
			mImageView.SetText("DirectShow initialize error");
		else
			mImageView.SetText("Ошибка инициализации directShow");
	}
	else if (resultCode == INIT_WEBCAM_ERROR)
	{
		if (m_englishVersion)
			mImageView.SetText("Web Camera initialize error");
		else
			mImageView.SetText("Ошибка инициализации веб-камеры");
	}
	else if (resultCode == ENUMERATE_RESOLUTION_ERROR)
	{
		if (m_englishVersion)
			mImageView.SetText("Get web camera resolutions error");
		else
		mImageView.SetText("Ошибка получения доступных разрешений веб-камеры");
	}
	else if (resultCode == WEBCAMERA_NOTFOUND)
	{
		if (m_englishVersion)
			mImageView.SetText("Web camera not found");
		else
			mImageView.SetText("Веб-камера не найдена");
	}
	else if (resultCode == WEBCAMERA_NOTRESPONCE)
	{
		if (m_englishVersion)
			mImageView.SetText("Web camera not response");
		else
			mImageView.SetText("Веб-камера не отвечает");
	}

	if (resultCode == SUCCESSFULL)
	{
		if (apSelector != NULL)
			ShowPreview(apSelector, m_webCamSelector);
	}
	else
	{
		if( hBitmap != 0)
		{
			DeleteObject( hBitmap);
			hBitmap = 0;
		}
		void* pBits = 0;
		hBitmap = CreateDIBSection (1, 1, 24, &pBits, false);
		mImageView.SetBitmap(hBitmap);
		mImageView.ShowWindow(SW_SHOW);
//произошла ошибка при смене разрешения, даем возможность установить другое
		if (m_resolutionNowChanging )
		{
			if (resultCode != WEBCAMERA_NOTRESPONCE)
				EnableAfterResolutionError();
			m_lastResolutionNum = 0;
			m_lastResolution.width = 0;
			m_lastResolution.height = 0;
		}
	}

	//смена разрешения завершится здесь
	m_resolutionNowChanging = false;
}

BOOL CDrawWindow::ApplyLayout( CBaseLayoutT* apLayout)
{
	if( !apLayout)
		return FALSE;
	//if( m_pCurrentLayout == apLayout)
	//	return TRUE;
	if( m_pCurrentLayout)
	{
		m_pCurrentLayout->UnBind();
	}
	m_pCurrentLayout = apLayout;
	m_pCurrentLayout->Bind( m_hWnd);
	m_pCurrentLayout->UpdateLayout();

	RECT rcView;
	::GetWindowRect(m_hWnd, &rcView);
	CSize wndSize(rcView.right-rcView.left, rcView.bottom-rcView.top);
	if (m_pCurrentLayout == (CBaseLayoutT*)m_layoutForImage || m_pCurrentLayout == (CBaseLayoutT*)m_layoutForTexture)
	{
		if (m_buttonSetRect.Width() != 0 && m_buttonSetRect.Height() != 0)
			buttonSet.ResizeClient(m_buttonSetRect.Width(), m_buttonSetRect.Height());
		if (m_buttonCancelRect.Width() != 0 && m_buttonCancelRect.Height() != 0)
			buttonCancel.ResizeClient(m_buttonCancelRect.Width(), m_buttonCancelRect.Height());
		wndSize.cx--;

		if (m_pCurrentLayout == (CBaseLayoutT*)m_layoutForTexture) {
			sliderQuality.SetPos(9);
		}
	}
	else
	{
		buttonSet.GetWindowRect(&m_buttonSetRect);
		buttonCancel.GetWindowRect(&m_buttonCancelRect);
		wndSize.cx++;
		buttonSet.ResizeClient(72, 25);
		buttonCancel.ResizeClient(72, 25);
	}
 	SetWindowPos( 0, 0, 0, wndSize.cx, wndSize.cy, SWP_NOMOVE | SWP_SHOWWINDOW);
 	UpdateViewPosition();
 	Invalidate(FALSE);
	return TRUE;
}

BOOL CDrawWindow::SetLayout( int aMode)
{
	m_currentMode = aMode;
	::ShowWindow( scaleRadio, SW_HIDE); 
	::ShowWindow( fitRadio, SW_HIDE);
	
	if (aMode == NORMAL_PREVIEW_MODE)	{
		ptMinTrackSize.x = 465;
		ptMinTrackSize.y = 200;
		scaleRadio.SetCheck(false);
		fitRadio.SetCheck(false);
		return ApplyLayout( m_layoutForImage);
	}
	else if (aMode == WEBCAM_PREVIEW_MODE) {
		ptMinTrackSize.x = 300;
		ptMinTrackSize.y = 250;
		scaleRadio.SetCheck(false);
		fitRadio.SetCheck(false);
		return ApplyLayout( m_layoutForWebCam);
	}
	else if (aMode == TEXTURE_PREVIEW_MODE) {
		ptMinTrackSize.x = 465;
		ptMinTrackSize.y = 200;
		fitRadio.SetCheck(false);
		scaleRadio.SetCheck(true);
		::ShowWindow( scaleRadio, SW_SHOW);
		::ShowWindow( fitRadio, SW_SHOW);
		return ApplyLayout( m_layoutForTexture);
	}
	return FALSE;
}
LRESULT CDrawWindow::OnCbnSelchangeDevicelist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CComboBox deviceList = GetDlgItem(IDC_DEVICELIST);
	m_selectedDeviceNum = deviceList.GetCurSel();
	m_lastResolutionNum = 0;
	DisableOnWebCamChanged();
	if (m_webCamSelector)
	{
		if (!m_webCamSelector->SetActiveDevice(m_selectedDeviceNum))
		{
			if (m_englishVersion)
				mImageView.SetText("Device initialize error");
			else
				mImageView.SetText("Ошибка инициализации устройства");
			return 0;
		}
	}
	FillResolutionList(m_selectedDeviceNum);

	if (m_englishVersion)
		mImageView.SetText("Please wait, changing device");
	else
		mImageView.SetText("Подождите идет смена устройства");
	return 0;
}

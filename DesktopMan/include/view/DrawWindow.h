#pragma once

//#include "DialogLayout.h"
#include "viewstatic.h"
#include "UI/ColoredControls.h"
#include "UI/WTButton.h"
#include "SharingContext.h"
#include ".\PreviewLayouts.h"
#include <string>

const int NORMAL_PREVIEW_MODE = 0;
const int WEBCAM_PREVIEW_MODE = 1;
const int TEXTURE_PREVIEW_MODE = 2;


const int SCALE_MODE_FIT = 11;
const int SCALE_MODE_SCALE = 12;

class CDesktopManager;
class CWTButtonCtrl;
class CColoredButtonCtrl;

struct IPreviewWindowEvents
{
	virtual	void OnPreviewWindowOK(int aScaleMode) = 0;
	virtual	void OnPreviewWindowCancel() = 0;
};

class CDrawWindow :
	public CDialogImpl<CDrawWindow>,
	public CUpdateUI<CDrawWindow>,
	public CDialogLayout<CDrawWindow>,
	public CColoredDialog<CDrawWindow>,
	public CEventSource<IPreviewWindowEvents>/*
	public COffscreenDrawRect< CDrawWindow >*/
{
public:
	CDrawWindow(oms::omsContext* apContext, bool aUseNewCodec = true);
	virtual ~CDrawWindow();

	enum { IDD = IDD_CONFIRM_DIALOG};

	BEGIN_LAYOUT_MAP()
/*		LAYOUT_CONTROL(IDOK,  LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_BOTTOM)
		LAYOUT_CONTROL(IDCANCEL, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_BOTTOM)
		LAYOUT_CONTROL(IDC_SLIDER_QUALITY, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_BOTTOM)
		LAYOUT_CONTROL(IDC_STATIC_QUALITY, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_BOTTOM)
		LAYOUT_CONTROL(IDC_RESOLUTIONS, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_BOTTOM)
		LAYOUT_CONTROL(IDC_STATIC_RESOLUTION, LAYOUT_ANCHOR_RIGHT | LAYOUT_ANCHOR_BOTTOM)
		LAYOUT_CONTROL(IDC_VIEW_AREA, LAYOUT_ANCHOR_ALL)
*/	END_LAYOUT_MAP()

public:
	bool ShowPreview(IDesktopSelector* apSelector);
	bool ShowPreview(IDesktopSelector* apSelector, IWebCameraSelector* webCamSelector);
	int	GetQuality();

public:
	BEGIN_UPDATE_UI_MAP(CDrawWindow)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CDrawWindow)
		NOTIFY_HANDLER(IDC_SLIDER_QUALITY, NM_RELEASEDCAPTURE, OnNMReleasedcaptureSliderQuality)
		COMMAND_HANDLER(IDC_RESOLUTIONS, CBN_SELCHANGE, OnSelectResolutionChanged)
		COMMAND_HANDLER(IDC_DEVICELIST, CBN_SELCHANGE, OnCbnSelchangeDevicelist)
		CHAIN_MSG_MAP(CUpdateUI<CDrawWindow>)
		MESSAGE_HANDLER(WM_SIZING, OnSizing)
		CHAIN_MSG_MAP(CDialogLayout<CDrawWindow>)
		MESSAGE_HANDLER(WM_HSCROLL, OnQualityScroll)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		CHAIN_MSG_MAP(CColoredDialog<CDrawWindow>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	
public:
	void Release();
	void DoPaint(CDCHandle dc);

	void _DrawFrame(HDC hDC) const
	{
		SIZE m_sizeBorder = {10,10};
		// Repaint frame borders (except the border on the right)
		CDCHandle dc = hDC;
		HBRUSH hBrushBorder = ::GetSysColorBrush(COLOR_ACTIVEBORDER);
		HBRUSH hBrushGrey = ::GetSysColorBrush(COLOR_BTNFACE);
		RECT rcWin;
		GetWindowRect(&rcWin);
		RECT rcLeft = { 0, m_sizeBorder.cy, m_sizeBorder.cx, (rcWin.bottom - rcWin.top) - m_sizeBorder.cy };
		dc.FillRect(&rcLeft, /*m_pane.iDirection == AUTOHIDE_LEFT ? hBrushGrey : */hBrushBorder);
		RECT rcBottom = { 0, (rcWin.bottom - rcWin.top) - m_sizeBorder.cy, (rcWin.right - rcWin.left) - m_sizeBorder.cx, (rcWin.bottom - rcWin.top) };
		dc.FillRect(&rcBottom, /*m_pane.iDirection == AUTOHIDE_LEFT ? hBrushBorder : */hBrushGrey);
		if( false/*m_pane.iDirection == AUTOHIDE_LEFT */) {
			RECT rcTop = { 0, 0, (rcWin.right - rcWin.left) - m_sizeBorder.cx, m_sizeBorder.cy };
			dc.FillRect(&rcTop, hBrushBorder);
		}
		else {
			RECT rcRight = { (rcWin.right - rcWin.left) - m_sizeBorder.cx, 0, (rcWin.right - rcWin.left), (rcWin.bottom - rcWin.top) };
			dc.FillRect(&rcRight, hBrushBorder);
		}
	}

protected:
	void ShowBitmapFromHDC(HDC ahHDC, int aiWidth, int aiHeight);
	void	UpdateViewPosition();
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSizing(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnSelectResolutionChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	void OnFinalMessage(HWND /*hWnd*/);

	CViewStaticCtrl	mImageView;
	HBITMAP	hBitmap;
	IDesktopFrame* pIDesktopFrame;
	oms::omsContext*	context;
	// UI

	CColoredStaticCtrl		staticQuality;
	CColoredStaticCtrl		staticResolution;
	CColoredStaticCtrl		staticDevice;
	CColoredTrackBarCtrl	sliderQuality;
	CColoredCheckboxCtrl	checkCodec;
	CColoredCheckboxCtrl	checkRemoteAccess;
	CWTButtonCtrl			buttonSet;
	CWTButtonCtrl			buttonCancel;

	CColoredOptionCtrl		scaleRadio;
	CColoredOptionCtrl		fitRadio;

public:
	BOOL SetLayout( int aMode);
	void OnSetResolutionCompleted(IDesktopSelector* apSelector, int resultCode);
	Resolution			m_lastResolution;

private:
	void	UpdateQualityLabel();
	void	UpdatePreviewWithQuality();
	LRESULT OnQualityScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void	ChangeQuality();

	LRESULT OnNMReleasedcaptureSliderQuality(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	HBITMAP CreateDIBSection( unsigned int auWidth, unsigned int auHeight, unsigned int auBPP, void** lppBits, bool abReverseHMode);
	void FillResolutionList(int numDevice);
	BOOL ApplyLayout( CBaseLayoutT* apLayout);
	void FillDeviceList();
	void DisableOnWebCamChanged();
	void EnableOnWebCamChanged();
	void EnableAfterResolutionError();


	IWebCameraSelector* m_webCamSelector;
	int			m_lastResolutionNum;

	CBaseLayoutT*			m_pCurrentLayout;
	CConfirmWebCamLayout*	m_layoutForWebCam;
	CConfirmLayout*			m_layoutForImage;
	CConfirmTextureBoardLayout* m_layoutForTexture;
	CRect					m_buttonSetRect;
	CRect					m_buttonCancelRect;
	int						m_selectedDeviceNum;
	bool					m_resolutionNowChanging;

	MP_STRING staticQualityEngText;
	MP_STRING staticResolutionEngText ;
	MP_STRING staticDeviceEngText;
	MP_STRING windowCaptionEngText;
	MP_STRING buttonSetEngText;
	MP_STRING buttonCancelEngText;

	bool m_englishVersion;

	POINT ptMinTrackSize;
	int m_currentMode;
public: 
	bool	useNewCodec;
	LRESULT OnCbnSelchangeDevicelist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
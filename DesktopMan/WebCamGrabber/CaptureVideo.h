#if !defined(AFX_CAPTUREVIDEO_H__057736B5_B61B_4850_8D82_E181E0B25B61__INCLUDED_)
#define AFX_CAPTUREVIDEO_H__057736B5_B61B_4850_8D82_E181E0B25B61__INCLUDED_

//--------------------------------------------------------------------------//
// Copyright DILLIP KUMAR KARA 2004
// You may do whatever you want with this code, as long as you include this
// copyright notice in your implementation files.
// Comments and bug Reports: codeguru_bank@yahoo.com
//--------------------------------------------------------------------------//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaptureVideo.h : header file
#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

#include <Qedit.h>
#include <strmif.h>
//#define DIRECT3D_VERSION         0x0900
//#include <d3d9.h>
//#include <Vmr9.h>
#include <dshow.h>
#include "errorCodes.h"
//
#include <string>
struct ISampleGrabber;
enum PLAYSTATE {STOPPED, PAUSED, RUNNING, INIT};

#define WM_GRAPHNOTIFY  WM_USER+1

#ifndef SAFE_RELEASE

#define SAFE_RELEASE( x )  \
    if ( NULL != x )       \
    {                      \
        x->Release( );     \
        x = NULL;          \
    }
#endif


// 1B6C4281-0353-11D1-905F-0000C0CC16B0
const GUID PIN_CATEGORY_WEBCAMERAVIDEOIN= { 0x1B6C4281, 0x0353, 0x11d1, { 0x90, 0x5f, 0x00, 0x00, 0xc0, 0xcc, 0x16, 0xB0 } };

/////////////////////////////////////////////////////////////////////////////
// CCaptureVideo window
//class CSampleDeviceDialog;

class CCaptureVideo //: public CWnd
{
// Construction
public:
	CCaptureVideo();

// Attributes
public:
	void StopCapture();
	void SetVideoWindowHandle(HWND ahWnd);
	bool StartWebcam( unsigned int auIndex = 0);
	bool RealRunWebcam( unsigned int auIndex = 0);
	bool StartCompositeVideo( unsigned int auIndex = 0);
	bool StartSVideo( unsigned int auIndex = 0);
	bool StartTVTuner( unsigned int auIndex = 0);
	void SetFrameSize( unsigned int auWidth, unsigned int auHeight);
	void RemoveGraphFromRot(DWORD pdwRegister);
	void UnIntializeVideo();
	HRESULT InitializeVideo();
	std::string	GetWebCamName( unsigned int auIndex);
	std::string	GetDisplayDeviceName( unsigned int auIndex);

	char* CCaptureVideo::WC2MB2(const wchar_t* apwc){
		int iSLen=(int)wcslen(apwc);
		int convSize = iSLen*MB_CUR_MAX+1;
		char* pch=new char[convSize];
		size_t newLen = 0;
		wcstombs_s(&newLen, pch, convSize, apwc, iSLen);
		pch[convSize-1]=0;
		return pch;
	}

// Operations
public:
	HBITMAP MakeSnapshot(bool isFirstShot);

// Implementation
public:
	void ResizeVideoWindow();
	void ResizeVideoWindowMoniker();
	virtual ~CCaptureVideo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCaptureVideo)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	//afx_msg HRESULT OnGraphNotify(WPARAM wp , LPARAM lp);
	//DECLARE_MESSAGE_MAP()
protected:

	bool StartTunerMode( unsigned int auIndex = 0, LONG alMode = PhysConn_Video_Composite);
	void CloseInterfacesMoniker();
	//----------------For Webcam-----------------//
	void RemoveGraphFromRotMoniker(DWORD pdwRegister);
	HRESULT AddGraphToRotMoniker(IUnknown *pUnkGraph, DWORD *pdwRegister);
	HRESULT SetupVideoWindowMoniker();
	HRESULT HandleGraphEventMoniker();
	HRESULT FindCaptureDeviceCategoryMonikers(IMoniker** ppMoniker, unsigned int auInCount, unsigned int* apuOutCount);
	HRESULT AddCaptureMonikerToGraph(IMoniker* pMoniker);
	HRESULT AddSampleGrabFilterToGraph(IFilterGraph2* apFilterGraph);
	HRESULT CaptureVideoByMoniker( unsigned int auIndex = 0);
	HRESULT GetInterfacesMoniker();
	HRESULT	SetupCaptureFrameSize();
	void	SetupInterlace();
	HRESULT SetVideoFormat(GUID format);
	

	//------------------Video--------------------//
	void DisplayMesg(TCHAR* szFormat, ...);
	LRESULT ClearInterfaces(WPARAM wp, LPARAM lp);
	void CloseInterfaces();
	HRESULT AddGraphToRot(IUnknown* pUnkGraph, DWORD* pdwRegister);

public:
	HRESULT CaptureVideo( unsigned int auIndex = 0);
	HRESULT HandleGraphEvent();
	HRESULT ChangePreviewState(int nShow);
	HRESULT SetupVideoWindow();
	HRESULT FindCaptureDevice(IBaseFilter** ppSrcFilter);
	HRESULT FindCaptureCategoryDevices(const GUID& categoryGuid, IBaseFilter** ppSrcFilter, unsigned int auInCount, unsigned int* apuOutCount);
	HRESULT GetInterfaces();
	bool RealRunTVTuner();
	ICaptureGraphBuilder2*	GetICaptureGraphBuilder2TV()
	{
		//need release ref after using!!!
	//	if( m_pCapture)
	//		m_pCapture->AddRef();
		return m_pCapture;
	}
	ICaptureGraphBuilder2*	GetICaptureGraphBuilder2WC()
	{
	//	if( m_pCaptureMoniker)
	//		m_pCaptureMoniker->AddRef();
		return m_pCaptureMoniker;
	}
	IBaseFilter*	GetSourceFilter();

	//------------------Device info--------------------//
	unsigned int GetVideoCamerasCount();
	unsigned int GetTVTunersCount();
		
private:
	UINT chSVideo, chCompVideo , chWebCam , chFullScreen , chAlwaysOnTop ;

	int nVSourceCount;
	int nAnalogCount;

	CBrush	m_emptyBrush;
	DWORD	m_dwGraphRegister;
	HWND	m_hApp;
	bool	m_frameSizeWasSet;
	CSize	m_frameSize;
	//------------------Video--------------------//
	IVideoWindow* m_pVW ; 
	IMediaControl* m_pMC ;
	IMediaEventEx* m_pME ;
	IGraphBuilder* m_pGraph ;
	ICaptureGraphBuilder2* m_pCapture;
	IBaseFilter* pSrcF;
	PLAYSTATE m_psCurrent; 
	
	//----------------For Webcam-----------------//
	IVideoWindow* m_pVWMoniker ;
	IMediaControl* m_pMCMoniker ;
	IMediaEventEx* m_pMEMoniker ;
	ICaptureGraphBuilder2* m_pCaptureMoniker; 
	IBaseFilter* m_pSourceWCFilter;
	IBaseFilter* m_pSGFilter;
	IFilterGraph2* m_pFilterGraph ;// IFilterGraph2 provides AddSourceFileForMoniker()

	//----------------Image capturing-----------------//
	ISampleGrabber*	m_pGrab;

	BOOL bVideo , bWebCam;
	bool bIsCOMInit;
	int vType;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPTUREVIDEO_H__057736B5_B61B_4850_8D82_E181E0B25B61__INCLUDED_)

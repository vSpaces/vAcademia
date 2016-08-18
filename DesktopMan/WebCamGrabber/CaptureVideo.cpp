// CaptureVideo.cpp : implementation file
//
//--------------------------------------------------------------------------//
// Copyright DILLIP KUMAR KARA 2004
// You may do whatever you want with this code, as long as you include this
// copyright notice in your implementation files.
// Comments and bug Reports: codeguru_bank@yahoo.com
//--------------------------------------------------------------------------//

#include "stdafx.h"
#include "CaptureVideo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaptureVideo
#define AfxMessageBox(x) MessageBox(0,x,"",MB_OK);
#define MAX_WEBCAMERAS_TO_DETECT 10

//  Free an existing media type (ie free resources it holds)
void WINAPI FreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);

		// Strictly unnecessary but tidier
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

// general purpose function to delete a heap allocated AM_MEDIA_TYPE structure
// which is useful when calling IEnumMediaTypes::Next as the interface
// implementation allocates the structures which you must later delete
// the format block may also be a pointer to an interface to release
void WINAPI DeleteMediaType(AM_MEDIA_TYPE *pmt)
{
	// allow NULL pointers for coding simplicity

	if (pmt == NULL) {
		return;
	}

	FreeMediaType(*pmt);
	CoTaskMemFree((PVOID)pmt);
}

CCaptureVideo::CCaptureVideo()
{
	// Initialization 
	m_hApp=NULL;
	m_dwGraphRegister=0;
	nAnalogCount =0; // Counting Analog devices
	m_pVW = NULL;
	m_pMC = NULL;
	m_pME = NULL;
	m_pGraph = NULL;  // IFilterGraph2 provides AddSourceFileForMoniker()
	m_pCapture = NULL;
	pSrcF = NULL;
	m_frameSizeWasSet = false;
	
	/// For Webcam
	m_pVWMoniker = NULL;
	m_pMCMoniker = NULL;
	m_pMEMoniker = NULL;
	m_pCaptureMoniker = NULL; 
	m_pFilterGraph = NULL;
	m_pSourceWCFilter = NULL;
	m_pSGFilter = NULL;

	// For image capturing
	m_pGrab = NULL;

	m_frameSize.cx = -1;
	m_frameSize.cy = -1;
	
	bIsCOMInit = false;
}

CCaptureVideo::~CCaptureVideo()
{
}

HRESULT CCaptureVideo::InitializeVideo()
{
	if (bIsCOMInit)
		return S_OK;

	HRESULT hr = NULL ;
	nVSourceCount =0;
	vType = 0;  
	
	//COM Library Intialization
	hr = CoInitialize(NULL);
	if(FAILED(hr))	/*, COINIT_APARTMENTTHREADED)))*/
    {
		//A thread must call CoUninitialize once for each successful call it has made to the CoInitialize
		//or CoInitializeEx function, including any call that returns S_FALSE
		if (S_FALSE == hr)
			bIsCOMInit = true;
        return hr ;
    }
	bIsCOMInit = true;
	return hr;
}

void CCaptureVideo::UnIntializeVideo()
{
	if( vType == 1) // If WebCam
		CloseInterfacesMoniker();
	CloseInterfaces(); 
	//A thread must call CoUninitialize once for each successful call it has made to the CoInitialize
	//or CoInitializeEx function, including any call that returns S_FALSE
	if (bIsCOMInit)
		CoUninitialize();
	bIsCOMInit = false;
	m_frameSizeWasSet = false;
	bVideo = FALSE;
}

void CCaptureVideo::SetVideoWindowHandle(HWND ahWnd)
{
	m_hApp = ahWnd;
	if( vType == 1) // If WebCam
	{
		SetupVideoWindowMoniker();
	}
	else	if( vType == 0) // If Tuner
	{
		SetupVideoWindow();
	}
}

HRESULT CCaptureVideo::GetInterfaces()
{
    HRESULT hr;
	
	if(m_pGraph == NULL){
		// Create the filter graph
		hr = CoCreateInstance (CLSID_FilterGraph, NULL, CLSCTX_INPROC,
							   IID_IGraphBuilder, (void **) &m_pGraph);
		if (FAILED(hr))
			return hr;
	}
	else
		return E_FAIL ;

	if(m_pCapture == NULL) {
		// Create the capture graph builder
		hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
							   IID_ICaptureGraphBuilder2, (void **) &m_pCapture);
		if (FAILED(hr))
			return hr;
	}
	else
		return E_FAIL ;
    
    // Obtain interfaces for media control and Video Window
    hr = m_pGraph->QueryInterface(IID_IMediaControl,(LPVOID *) &m_pMC);
    if (FAILED(hr))
        return hr;
	
    hr = m_pGraph->QueryInterface(IID_IVideoWindow, (LPVOID *) &m_pVW);
    if (FAILED(hr))
        return hr;
	
    hr = m_pGraph->QueryInterface(IID_IMediaEvent, (LPVOID *) &m_pME);
    if (FAILED(hr))
        return hr;
	
    // Set the window handle used to process graph events
    hr = m_pME->SetNotifyWindow((OAHWND)m_hApp, WM_GRAPHNOTIFY, 0);
	
    return hr;
}

HRESULT CCaptureVideo::FindCaptureCategoryDevices(const GUID& categoryGuid, IBaseFilter** ppSrcFilter, unsigned int auInCount, unsigned int* apuOutCount)
{
	HRESULT hr;
	IBaseFilter * pSrc = NULL;
	CComPtr <IMoniker> pMoniker =NULL;
	ULONG cFetched;

	if (!ppSrcFilter)
		return E_POINTER;

	// Create the system device enumerator  
	CComPtr <ICreateDevEnum> pDevEnum =NULL;

	hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
		IID_ICreateDevEnum, (void **) &pDevEnum);
	if (FAILED(hr))
	{
	//	DisplayMesg(TEXT("Couldn't create system enumerator!  hr=0x%x"), hr);
		return hr;
	}

	// Create an enumerator for the video capture devices
	CComPtr <IEnumMoniker> pClassEnum = NULL;

	hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
	if (FAILED(hr))
	{
	//	DisplayMesg(TEXT("Couldn't create class enumerator!  hr=0x%x"), hr);
		return hr;
	}

	// If there are no enumerators for the requested type, then 
	// CreateClassEnumerator will succeed, but pClassEnum will be NULL.
	if (pClassEnum == NULL)
	{
		//       MessageBox(m_hApp,TEXT("No video capture device was detected.\r\n\r\n")
		//                TEXT("This sample requires a video capture device, such as a USB WebCam,\r\n")
		//                TEXT("to be installed and working properly.  The sample will now close."),
		//                TEXT("No Video Capture Hardware"), MB_OK | MB_ICONINFORMATION);
		return E_FAIL;
	}

	// Use the first video capture device on the device list.
	// Note that if the Next() call succeeds but there are no monikers,
	// it will return S_FALSE (which is not a failure).  Therefore, we
	// check that the return code is S_OK instead of using SUCCEEDED() macro.

	//To Find the Analog Capture Devices....

	unsigned int			foundCount = 0;
	BOOL         Found = false;
	IPin        *pP = 0;
	IEnumPins   *pins=0;
	ULONG        n;
	PIN_INFO     pinInfo;
	Found	= FALSE;
	IKsPropertySet *pKs=0;
	GUID guid;
	DWORD dw;
	BOOL fMatch = FALSE;

	while(S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched)))
	//if(S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched)))
	{	
		// Bind Moniker to a filter object
		hr = pMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)&pSrc);
		if (FAILED(hr))
		{
		//	DisplayMesg(TEXT("Couldn't bind moniker to filter object!  hr=0x%x"), hr);
			return hr;
		}
		if(SUCCEEDED(pSrc->EnumPins(&pins)))
		{    
			Found = false;
			while(!Found && (S_OK == pins->Next(1, &pP, &n)))
			{
				//vType ++;
				if(S_OK == pP->QueryPinInfo(&pinInfo))
				{

					if(pinInfo.dir == PINDIR_INPUT)
					{
						// is this pin an ANALOGVIDEOIN input pin?
						if(pP->QueryInterface(IID_IKsPropertySet,
							(void **)&pKs) == S_OK)
						{
							if(pKs->Get(AMPROPSETID_Pin,
								AMPROPERTY_PIN_CATEGORY, NULL, 0,
								&guid, sizeof(GUID), &dw) == S_OK)
							{
								if(IsEqualGUID( guid, categoryGuid)/*PIN_CATEGORY_ANALOGVIDEOIN*/)
								{
									if( foundCount < auInCount)
									{
										ppSrcFilter[foundCount] = pSrc;
										foundCount++;
										Found = true;
									}
								}
							}
							pKs->Release();
						}
					}
					pinInfo.pFilter->Release();
				}
				pP->Release();

				if(!Found)
				{
					pSrc->Release();
					break;
				}
			}
			pins->Release();
		}
		pMoniker.Release();
	}

	// Copy the found filter pointer to the output parameter.
	// Do NOT Release() the reference, since it will still be used
	// by the calling function.
	if( apuOutCount)
		*apuOutCount = foundCount;

	return hr;
}

HRESULT CCaptureVideo::ChangePreviewState(int nShow)
{
	HRESULT hr=S_OK;
    
    // If the media control interface isn't ready, don't call it
    if (!m_pMC)
        return S_OK;
    
    if (nShow)
    {
        if (m_psCurrent != RUNNING)
        {
            // Start previewing video data
            hr = m_pMC->Run();
            m_psCurrent = RUNNING;
        }
    }
    else
    {
        // Stop previewing video data
        hr = m_pMC->StopWhenReady();
        m_psCurrent = STOPPED;
    }

    return hr;
}

HRESULT CCaptureVideo::HandleGraphEvent()
{
    LONG evCode, evParam1, evParam2;
    HRESULT hr=S_OK;

    if (!m_pME)
        return E_POINTER;

    while(SUCCEEDED(m_pME->GetEvent(&evCode, (LONG_PTR *) &evParam1, 
                   (LONG_PTR *) &evParam2, 0)))
    {
        //
        // Free event parameters to prevent memory leaks associated with
        // event parameter data.  While this application is not interested
        // in the received events, applications should always process them.
        //
        hr = m_pME->FreeEventParams(evCode, evParam1, evParam2);
    }

    return hr;
}

HRESULT CCaptureVideo::CaptureVideo( unsigned int auIndex)
{
	HRESULT hr;
    IBaseFilter *pSrcFilter=NULL;

    // Get DirectShow interfaces
    hr = GetInterfaces();
    if (FAILED(hr))
    {
//        DisplayMesg(TEXT("Failed to get video interfaces!  hr=0x%x"), hr);
        return hr;
    }

    // Attach the filter graph to the capture graph
    hr = m_pCapture->SetFiltergraph(m_pGraph);
    if (FAILED(hr))
    {
 //       DisplayMesg(TEXT("Failed to set capture filter graph!  hr=0x%x"), hr);
        return hr;
    }

    // Use the system device enumerator and class enumerator to find
    // a video capture/preview device, such as a desktop USB video camera.

	// web-cameras monikers enumeration
	IBaseFilter*	filters[MAX_WEBCAMERAS_TO_DETECT];
	ZeroMemory( &filters[0], sizeof(filters));
	unsigned int found = 0;

	if( SUCCEEDED( hr = FindCaptureCategoryDevices(PIN_CATEGORY_ANALOGVIDEOIN, &filters[0], MAX_WEBCAMERAS_TO_DETECT, &found)))
	{
		if( auIndex < found)
		{
			pSrcFilter = filters[ auIndex];
			for(unsigned int i=0; i<found; i++)
				if( i != auIndex)
					SAFE_RELEASE( filters[ i]);
		}
		else
		{
			return E_NOINTERFACE;
		}
	}
	else
	{
		return hr;
	}
    if (FAILED(hr))
    {
        // Don't display a message because FindCaptureDevice will handle it
        return hr;
    }


    // Add Capture filter to our graph.
    hr = m_pGraph->AddFilter(pSrcFilter, L"Video Capture");
    if (FAILED(hr))
    {
//         DisplayMesg(TEXT("Couldn't add the capture filter to the graph!  hr=0x%x\r\n\r\n") 
//             TEXT("If you have a working video capture device, please make sure\r\n")
//             TEXT("that it is connected and is not being used by another application.\r\n\r\n"), hr);
        pSrcFilter->Release();
        return hr;
    }

	IFilterGraph2*	ifgf2 = NULL;
	hr = m_pGraph->QueryInterface(IID_IFilterGraph2, (void**)&ifgf2);
	if( ifgf2)
	{
		hr = AddSampleGrabFilterToGraph(ifgf2);
		if (FAILED(hr))
		{
			return hr;
		}
		ifgf2->Release();
	}

	pSrcF = pSrcFilter;

	IBaseFilter* sgf = NULL;
	m_pGrab->QueryInterface( IID_IBaseFilter, (void**)&sgf);
    // Render the preview pin on the video capture filter
    // Use this instead of m_pGraph->RenderFile
    hr = m_pCapture->RenderStream (&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
                                   pSrcFilter, sgf, NULL);

    if (FAILED(hr))
    {
//         DisplayMesg(TEXT("Couldn't render the video capture stream.  hr=0x%x\r\n")
//             TEXT("The capture device may already be in use by another application.\r\n\r\n")
//             TEXT("The sample will now close."), hr);
        pSrcFilter->Release();
        return hr;
    }
	if( sgf)
		sgf->Release();
	
    // Now that the filter has been added to the graph and we have
    // rendered its stream, we can release this reference to the filter.
    pSrcFilter->Release();

	//SetupInterlace();

	if( m_hApp != 0)
	{
		// Set video window style and position
		hr = SetupVideoWindow();
		if (FAILED(hr))
		{
			//DisplayMesg(TEXT("Couldn't initialize video window!  hr=0x%x"), hr);
			return hr;
		}
	}
	else
	{
		if( m_pVW)
			m_pVW->put_Visible(OAFALSE);
	}
	
#ifdef REGISTER_FILTERGRAPH
    // Add our graph to the running object table, which will allow
    // the GraphEdit application to "spy" on our graph
    hr = AddGraphToRot(m_pGraph, &m_dwGraphRegister);
    if (FAILED(hr))
    {
     //   DisplayMesg(TEXT("Failed to register filter graph with ROT!  hr=0x%x"), hr);
        m_dwGraphRegister = 0;
    }
#endif
	m_pVW->put_AutoShow(OAFALSE);
    // Start previewing video data
 //   hr = m_pMC->Run();
    if (FAILED(hr))
    {
       // DisplayMesg(TEXT("Couldn't run the graph!  hr=0x%x"), hr);
        return hr;
    }

	OAFilterState state;
	while( VFW_S_STATE_INTERMEDIATE == m_pMC->GetState(0, &state))
		Sleep(100);
	
    // Remember current state
    m_psCurrent = RUNNING;
        
    return S_OK;
}

void CCaptureVideo::RemoveGraphFromRot(DWORD pdwRegister)
{
	IRunningObjectTable *pROT;

    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) 
    {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}

void CCaptureVideo::CloseInterfaces()
{
	   // Stop previewing data
    if (m_pMC)
        m_pMC->StopWhenReady();

    m_psCurrent = STOPPED;

    // Stop receiving events
    if (m_pME)
        m_pME->SetNotifyWindow(NULL, WM_GRAPHNOTIFY, 0);

    // Relinquish ownership (IMPORTANT!) of the video window.
    // Failing to call put_Owner can lead to assert failures within
    // the video renderer, as it still assumes that it has a valid
    // parent window.
    if(m_pVW)
    {
        m_pVW->put_Visible(OAFALSE);
        m_pVW->put_Owner(NULL);
    }
	
#ifdef REGISTER_FILTERGRAPH
    // Remove filter graph from the running object table   
    if (m_dwGraphRegister)
        RemoveGraphFromRot(m_dwGraphRegister);
#endif

    // Release DirectShow interfaces
    SAFE_RELEASE(m_pMC);
    SAFE_RELEASE(m_pME);
    SAFE_RELEASE(m_pVW);
    SAFE_RELEASE(m_pGraph);
    SAFE_RELEASE(m_pCapture);
}

void CCaptureVideo::CloseInterfacesMoniker()
{
	// Stop previewing data
    if (m_pMCMoniker)
		m_pMCMoniker->Stop();
      // m_pMCMoniker->StopWhenReady();

    // Stop receiving events
    if (m_pMEMoniker)
        m_pMEMoniker->SetNotifyWindow(NULL, WM_GRAPHNOTIFY, 0);

    if(m_pVWMoniker)
    {
        m_pVWMoniker->put_Visible(OAFALSE);
        m_pVWMoniker->put_Owner(NULL);
    }
	
#ifdef REGISTER_FILTERGRAPH
    // Remove filter graph from the running object table   
    if (m_dwGraphRegister)
        RemoveGraphFromRotMoniker(m_dwGraphRegister);
#endif
		
	SAFE_RELEASE(m_pGrab);
	SAFE_RELEASE(m_pSourceWCFilter);
	SAFE_RELEASE(m_pSGFilter);
	
    // Release DirectShow interfaces
    SAFE_RELEASE(m_pMCMoniker);
    SAFE_RELEASE(m_pMEMoniker);
    SAFE_RELEASE(m_pVWMoniker);
    SAFE_RELEASE(m_pCaptureMoniker);
    SAFE_RELEASE(m_pFilterGraph);
}

LRESULT CCaptureVideo::ClearInterfaces(WPARAM wp, LPARAM lp)
{
	if( vType == 1) // If WebCam
		CloseInterfacesMoniker();
	CloseInterfaces(); 
	bVideo = FALSE;
	return 0L ;
}

HRESULT CCaptureVideo::GetInterfacesMoniker()
{
    HRESULT hr;
	
	if(m_pFilterGraph == NULL)
	{
		// Create the filter graph
		hr = CoCreateInstance (CLSID_FilterGraph, NULL, CLSCTX_INPROC,
							   IID_IFilterGraph2, (void **) &m_pFilterGraph);
		if (FAILED(hr))
			return hr;
	}
	else
	{
		return E_FAIL ;
	}
	
	if(m_pCaptureMoniker == NULL)
	{
		// Create the capture graph builder
		hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
							   IID_ICaptureGraphBuilder2, (void **) &m_pCaptureMoniker);
		if (FAILED(hr))
			return hr;
    }
	else
		return E_FAIL ;

    // Obtain interfaces for media control and Video Window
    hr = m_pFilterGraph->QueryInterface(IID_IMediaControl,(LPVOID *) &m_pMCMoniker);
    if (FAILED(hr))
        return hr;
	
    hr = m_pFilterGraph->QueryInterface(IID_IVideoWindow, (LPVOID *) &m_pVWMoniker);
    if (FAILED(hr))
        return hr;

    hr = m_pFilterGraph->QueryInterface(IID_IMediaEvent, (LPVOID *) &m_pMEMoniker);
    if (FAILED(hr))
        return hr;
	
    // Set the window handle used to process graph events
    hr = m_pMEMoniker->SetNotifyWindow((OAHWND)m_hApp, WM_GRAPHNOTIFY, 0);
	
    return hr;
}

std::string	CCaptureVideo::GetWebCamName( unsigned int auIndex)
{
	HRESULT hr = 0;

	// web-cameras monikers enumeration
	IMoniker*	pMoniker = NULL;
	IMoniker*	monikers[MAX_WEBCAMERAS_TO_DETECT];
	ZeroMemory( &monikers[0], sizeof(monikers));
	unsigned int found = 0;

	if( SUCCEEDED( hr = FindCaptureDeviceCategoryMonikers(&monikers[0], MAX_WEBCAMERAS_TO_DETECT, &found)))
	{
		if( auIndex < found)
		{
			pMoniker = monikers[ auIndex];
			for(unsigned int i=0; i<found; i++)
				if( i != auIndex)
					SAFE_RELEASE( monikers[ i]);
		}
		else
		{
			return "WebCamera";
		}
	}
	else
	{
		return "WebCamera";
	}

	if( pMoniker == NULL)
	{
		return "WebCamera";
	}

	IPropertyBag *pPropBag;
	hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
	if (SUCCEEDED(hr))
	{
		// To retrieve the filter's friendly name, do the following:
		VARIANT varName;
		VariantInit(&varName);
		hr = pPropBag->Read(L"FriendlyName", &varName, 0);
		if (SUCCEEDED(hr))
		{
			std::string str = CW2A(varName.bstrVal);
			VariantClear(&varName);
			SAFE_RELEASE(pMoniker);
			return str;
		}
		VariantClear(&varName);
	}
	return "WebCamera";
}

std::string	CCaptureVideo::GetDisplayDeviceName( unsigned int auIndex)
{
	HRESULT hr = 0;

	// web-cameras monikers enumeration
	IMoniker*	pMoniker = NULL;
	IMoniker*	monikers[MAX_WEBCAMERAS_TO_DETECT];
	ZeroMemory( &monikers[0], sizeof(monikers));
	unsigned int found = 0;

	if( SUCCEEDED( hr = FindCaptureDeviceCategoryMonikers(&monikers[0], MAX_WEBCAMERAS_TO_DETECT, &found)))
	{
		if( auIndex < found)
		{
			pMoniker = monikers[ auIndex];
			for(unsigned int i=0; i<found; i++)
				if( i != auIndex)
					SAFE_RELEASE( monikers[ i]);
		}
		else
		{
			return "none";
		}
	}
	else
	{
		return "none";
	}

	if( pMoniker == NULL)
	{
		return "none";
	}

	// Get the display name of the moniker
	LPOLESTR strMonikerName=0;
	hr = pMoniker->GetDisplayName(NULL, NULL, &strMonikerName);
	if (!FAILED(hr))
	{
		return WC2MB2((wchar_t*)strMonikerName);
	}
	return "none";
}

HRESULT CCaptureVideo::CaptureVideoByMoniker( unsigned int auIndex)
{
	HRESULT hr = 0;

    // Get DirectShow interfaces
    hr = GetInterfacesMoniker();
    if (FAILED(hr))
    {
        return hr;
    }

	// web-cameras monikers enumeration
	IMoniker*	pMoniker = NULL;
	IMoniker*	monikers[MAX_WEBCAMERAS_TO_DETECT];
	ZeroMemory( &monikers[0], sizeof(monikers));
	unsigned int found = 0;

	if( SUCCEEDED( hr = FindCaptureDeviceCategoryMonikers(&monikers[0], MAX_WEBCAMERAS_TO_DETECT, &found)))
	{
		if( auIndex < found)
		{
			pMoniker = monikers[ auIndex];
			for(unsigned int i=0; i<found; i++)
				if( i != auIndex)
					SAFE_RELEASE( monikers[ i]);
		}
		else
		{
			return E_NOINTERFACE;
		}
	}
	else
	{
		return hr;
	}

	if( pMoniker == NULL)
	{
		//AfxMessageBox("Webcam(USB) is not detected.!") ;
		return hr;
	}

    hr = AddCaptureMonikerToGraph(pMoniker);

	// Release the IMoniker interface for the capture source filter
	pMoniker->Release();
	pMoniker = NULL;

	if (FAILED(hr))
    {
        return hr;
    }

	if( m_hApp != 0)
	{
		// Set video window style and position
		hr = SetupVideoWindowMoniker();
		if (FAILED(hr))
			return hr;
	}
	else
	{
		if( m_pVW)
			m_pVW->put_Visible(OAFALSE);
	}

#ifdef REGISTER_FILTERGRAPH
    // Add our graph to the running object table, which will allow
    // the GraphEdit application to "spy" on our graph
    hr = AddGraphToRotMoniker(m_pFilterGraph, &m_dwGraphRegister);
    if (FAILED(hr))
    {
        //DisplayMesg(TEXT("Failed to register filter graph with ROT!  hr=0x%x"), hr);
        m_dwGraphRegister = 0;
    }
#endif

   	hr = m_pVWMoniker->put_AutoShow(OAFALSE);

    return S_OK;
}

bool FillAxMediaType( AM_MEDIA_TYPE& amt)
{
	BYTE	bpp = 24;
	ZeroMemory(&amt, sizeof(AM_MEDIA_TYPE));
	switch( bpp)
	{
		//case 16:	mt.subtype = MEDIASUBTYPE_RGB565;	break;
	case 16:	amt.subtype = MEDIASUBTYPE_RGB24;	break;
	case 24:	amt.subtype = MEDIASUBTYPE_RGB24;	break;
	case 32:	amt.subtype = MEDIASUBTYPE_RGB32;	break;
	default:	amt.subtype = MEDIASUBTYPE_RGB32;	break;
	}
	amt.majortype = MEDIATYPE_Video;
	//amt.subtype = MEDIASUBTYPE_RGB32;	//?? - распаковка на текстуру D3DFMT_A8R8G8B8
	//amt.subtype = MEDIASUBTYPE_YUYV;
	//amt.formattype = FORMAT_VideoInfo; 
	return true;
}

HRESULT CCaptureVideo::AddSampleGrabFilterToGraph(IFilterGraph2* apFilterGraph)
{
	if( m_pGrab != NULL)
		return S_OK;
	if( !apFilterGraph)
		return E_FAIL;

	HRESULT hr = S_OK;
	//////////////////////////////////////////////////////////////////////////
	// Construct filter to grab video frames
	if( FAILED( hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID *)&m_pSGFilter)))
		return hr;

	hr = m_pSGFilter->QueryInterface(IID_ISampleGrabber, (void **)&m_pGrab);
	if( FAILED( hr))
		return hr;

	hr = apFilterGraph->AddFilter(m_pSGFilter, L"Grabber");
	if( FAILED( hr))
		return hr;
 
	hr = SetVideoFormat(MEDIASUBTYPE_RGB24);
	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT CCaptureVideo::AddCaptureMonikerToGraph(IMoniker *pMoniker)
{
	USES_CONVERSION;

    HRESULT hr;

    // Get the display name of the moniker
    LPOLESTR strMonikerName=0;
    hr = pMoniker->GetDisplayName(NULL, NULL, &strMonikerName);
    if (FAILED(hr))
    {
        return hr;
    }

    // Create a bind context needed for working with the moniker
    IBindCtx *pContext=0;
    hr = CreateBindCtx(0, &pContext);
    if (FAILED(hr))
    {
        return hr;
    }
	
    hr = m_pFilterGraph->AddSourceFilterForMoniker(pMoniker, pContext, 
                                             strMonikerName, &m_pSourceWCFilter);
    if (FAILED(hr))
    {
        return hr;
    }

    // Attach the filter graph to the capture graph
    hr = m_pCaptureMoniker->SetFiltergraph(m_pFilterGraph);
    if (FAILED(hr))
    {
        return hr;
    }
	
	hr = AddSampleGrabFilterToGraph( m_pFilterGraph);
	if (FAILED(hr))
	{
		return hr;
	}

//	if( m_frameSizeWasSet)
	{
		hr = SetupCaptureFrameSize();
		// по какой-то причине поддерживаемое разрешение не хочет уcтанавливаться
		if (FAILED(hr))
		{
			return hr;
		}
	}

    // Render the preview pin on the video capture filter
    // Use this instead of m_pGraph->RenderFile
	hr = m_pCaptureMoniker->RenderStream (&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                   m_pSourceWCFilter, m_pSGFilter, NULL);

	if (FAILED(hr))
	{
	 //         "The device may already be in use."
		return hr ;
	}

	hr = m_pGrab->SetBufferSamples(TRUE);
	if( FAILED( hr))
		return hr;
	hr = m_pGrab->SetOneShot(TRUE);
	if( FAILED( hr))
		return hr;
	
    SAFE_RELEASE(pContext);
	
    return hr;
}

HRESULT	CCaptureVideo::SetupCaptureFrameSize()
{
	//??
	int minW = 0, minH = 0;
	CComPtr<ICaptureGraphBuilder2>	pBuilder = GetICaptureGraphBuilder2WC();
	if( pBuilder)
	{
		CComPtr<IBaseFilter>	pSource = GetSourceFilter();
		if( pSource)
		{
			CComPtr<IAMStreamConfig> pConfig;
			HRESULT hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
				NULL, pSource,
				IID_IAMStreamConfig, (void **)&pConfig);

			// проверяем является ли устанавливаемое разрешение поддерживаемым
			int iCount = 0;
			int iSize = sizeof(VIDEO_STREAM_CONFIG_CAPS);
			bool isSupportedResolution = false;
			pConfig->GetNumberOfCapabilities( &iCount, &iSize);

			for (int i=0; i<iCount; i++)
			{
				AM_MEDIA_TYPE *pmt = 0;
				VIDEO_STREAM_CONFIG_CAPS caps;
				if (FAILED(pConfig->GetStreamCaps(i, &pmt, (byte*)&caps)))
					continue;

				if (m_frameSizeWasSet && caps.InputSize.cx == m_frameSize.cx && caps.InputSize.cy == m_frameSize.cy) 
				{
					isSupportedResolution = true;
					break;
				}
				if (minW > caps.InputSize.cx || minW == 0)
					minW = caps.InputSize.cx;
				if (minH > caps.InputSize.cy || minH == 0)
					minH = caps.InputSize.cy;
			}
			// если размеры еще не установлены берем самое минимальное разрешение
			if (!m_frameSizeWasSet)
			{
				m_frameSize.cx = minW;
				m_frameSize.cy = minH;
				isSupportedResolution = true;
			}
			// разрешение не поддерживается
			if (!isSupportedResolution)
				return E_FAIL;

			AM_MEDIA_TYPE *pmt = 0;
			if (FAILED(pConfig->GetFormat(&pmt)))
				return E_FAIL;

			ATLASSERT(pmt->formattype == FORMAT_VideoInfo);

			VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;

			int FPS = 7;

			if (FPS != -1.0)
				pvi->AvgTimePerFrame = (LONGLONG)(10000000/FPS);
			if (m_frameSize.cx != -1)
				pvi->bmiHeader.biWidth = m_frameSize.cx;
			if (m_frameSize.cy != -1)
				pvi->bmiHeader.biHeight = m_frameSize.cy;

			hr = pConfig->SetFormat(pmt);

			DeleteMediaType(pmt);
			return hr;
		}
	}
	return E_FAIL;
}

void	CCaptureVideo::SetupInterlace()
{
	//??
	CComPtr<ICaptureGraphBuilder2>	pBuilder = GetICaptureGraphBuilder2TV();
	if( pBuilder)
	{
		CComPtr<IBaseFilter>	pSource = GetSourceFilter();
		if( pSource)
		{
			//!!!!!!!!!!!!!!!!!!!
// 			CComPtr<IVMRDeinterlaceControl9> pConfig;
// 			HRESULT hr = pBuilder->FindInterface(&PIN_CATEGORY_PREVIEW,
// 				NULL, pSource,
// 				IID_IVMRDeinterlaceControl9, (void**)&pConfig);
// 			int ii=0;

			/*AM_MEDIA_TYPE *pmt = 0;
			if (FAILED(pConfig->GetFormat(&pmt)))
				return;

			ATLASSERT(pmt->formattype == FORMAT_VideoInfo2);

			VIDEOINFOHEADER2  *pvi = (VIDEOINFOHEADER2*)pmt->pbFormat;
			pvi->dwInterlaceFlags = 0;

			hr = pConfig->SetFormat(pmt);

			DeleteMediaType(pmt);*/
		}
	}
}

HRESULT CCaptureVideo::SetVideoFormat(GUID format)
{
	if (!m_pGrab)
		return E_POINTER;
	AM_MEDIA_TYPE mt;
	memset(&mt,0,sizeof(mt));
	mt.majortype = MEDIATYPE_Video;
	mt.formattype = FORMAT_VideoInfo;
	mt.subtype = format;

	return m_pGrab->SetMediaType(&mt);
}

HRESULT CCaptureVideo::FindCaptureDeviceCategoryMonikers(IMoniker** ppMoniker, unsigned int auInCount, unsigned int* apuOutCount)
{
	HRESULT hr;
	ULONG cFetched;

	if (!ppMoniker)
		return E_POINTER;

	// Create the system device enumerator
	CComPtr <ICreateDevEnum> pDevEnum = NULL;

	hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
		IID_ICreateDevEnum, (void ** ) &pDevEnum);
	if (FAILED(hr))
	{
	//	DisplayMesg(TEXT("Couldn't create system enumerator!  hr=0x%x"), hr);
		return hr;
	}

	// Create an enumerator for the video capture devices
	CComPtr <IEnumMoniker> pClassEnum = NULL;

	hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
	if (FAILED(hr))
	{
	//	DisplayMesg(TEXT("Couldn't create class enumerator!  hr=0x%x"), hr);
		return hr;
	}

	// If there are no enumerators for the requested type, then 
	// CreateClassEnumerator will succeed, but pClassEnum will be NULL.
	if (pClassEnum == NULL)
	{
		//AfxMessageBox("No video capture device was detected.",MB_OK | MB_ICONINFORMATION) ;
		return E_FAIL;
	}

	// Note that if the Next() call succeeds but there are no monikers,
	// it will return S_FALSE (which is not a failure).  Therefore, we
	// check that the return code is S_OK instead of using SUCCEEDED() macro.

	/// For finding Digital Capture Devices ...

	unsigned int	foundCount = 0;
	BOOL        devicePinFound = false;
	IPin        *pP = 0;
	IEnumPins   *pins=0;
	ULONG        n;
	PIN_INFO     pinInfo;
	IKsPropertySet *pKSPropertySet = 0;
	GUID guid;
	DWORD dw;
	BOOL fMatch = FALSE;
	IMoniker *pMoniker = NULL;
	IBaseFilter *pSrc;

	while (S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched)))
	{	
		devicePinFound = false;
		hr = pMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)&pSrc);
		if (FAILED(hr))
		{
		//	DisplayMesg(TEXT("Couldn't bind moniker to filter object!  hr=0x%x"), hr);
			//return hr;
			continue;
		}
		else
			pSrcF = pSrc;

		if(SUCCEEDED(pSrc->EnumPins(&pins)))
		{     
			while(!devicePinFound && (S_OK == pins->Next(1, &pP, &n)))
			{
				/*
				IEnumMediaTypes *penum;
				pP->EnumMediaTypes(&penum);
				AM_MEDIA_TYPE *pmt;
				GUID fmt;
				ULONG cbReturned;
				// ищем только RGB uncompressed formats
				while(penum->Next(1,&pmt,&cbReturned)==S_OK && cbReturned == 1){
					if (IsEqualGUID(pmt->subtype,MEDIASUBTYPE_RGB24)){					
						fmt = MEDIASUBTYPE_RGB24;
					} 
					DeleteMediaType(pmt);
				}
				*/

				if(S_OK == pP->QueryPinInfo(&pinInfo))
				{
					//if(pinInfo.dir == PINDIR_INPUT)
					{
						// is this pin an ANALOGVIDEOIN input pin?
						if(pP->QueryInterface(IID_IKsPropertySet, (void **)&pKSPropertySet) == S_OK)
						{
							if(pKSPropertySet->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0,
											&guid, sizeof(GUID), &dw) == S_OK)
							{
								//для получения устройства категории PIN_CATEGORY_ANALOGVIDEOIN пользуемся FindCaptureCategoryDevices()
								//для всех остальных считаем что это usb webcamera
								if( !IsEqualGUID( guid, PIN_CATEGORY_ANALOGVIDEOIN))
								{
									if( foundCount < auInCount)
									{
									//	SelectBestCompatibleFormat(pP);
										ppMoniker[foundCount] = pMoniker;
										devicePinFound = TRUE;
										foundCount++;
									}
								}
							}
							pKSPropertySet->Release();
						}
					}
					pinInfo.pFilter->Release();
				}
				pP->Release();
			}
			pins->Release();
		}
		
		if( !devicePinFound)
		{
			pMoniker->Release();
		}
		
		
	} // End of While Loop

	if( apuOutCount)
		*apuOutCount = foundCount;

	return S_OK;
}

void CCaptureVideo::SetFrameSize( unsigned int auWidth, unsigned int auHeight)
{
	m_frameSize.cx = auWidth;
	m_frameSize.cy = auHeight;
	m_frameSizeWasSet = true;
//	SetupCaptureFrameSize();
}

bool CCaptureVideo::StartSVideo(unsigned int auIndex)
{
	return StartTunerMode( auIndex, PhysConn_Video_SVideo);
}

bool CCaptureVideo::StartCompositeVideo( unsigned int auIndex)
{
	return StartTunerMode( auIndex, PhysConn_Video_Composite);
}

bool CCaptureVideo::StartTVTuner(unsigned int auIndex)
{
	return StartTunerMode( auIndex, PhysConn_Video_Tuner);
}

bool CCaptureVideo::StartTunerMode( unsigned int auIndex, LONG alMode)
{

	if(vType == 1)	//If Webcam
		CloseInterfacesMoniker();

	vType = 0; // Initializing current capture device to Video

	if( m_pCapture == NULL)
	{
		HRESULT hr=	CaptureVideo();
		if(FAILED(hr))
		{
			CloseInterfaces();
			return false;
		}
	}

	IAMCrossbar *pX;

	if ( m_pCapture == NULL)
		return false;

	if(pSrcF == NULL)
		return false;

	HRESULT hr;
	hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Interleaved, pSrcF,
		IID_IAMCrossbar, (void **)&pX);
	if(hr != NOERROR)
		hr = m_pCapture->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Video, pSrcF,
		IID_IAMCrossbar, (void **)&pX);

	LONG lInpin, lOutpin;
	hr = pX->get_PinCounts(&lOutpin , &lInpin); 

	BOOL IPin=TRUE; LONG pIndex=0 , pRIndex=0 , pType=0;

	while( pIndex < lInpin)
	{
		hr = pX->get_CrossbarPinInfo( IPin , pIndex , &pRIndex , &pType); 

		if( pType == alMode)
			break;
		pIndex++;
	}

	BOOL OPin=FALSE; LONG pOIndex=0 , pORIndex=0 , pOType=0;
	while( pOIndex < lOutpin)
	{
		hr = pX->get_CrossbarPinInfo( OPin , pOIndex , &pORIndex , &pOType); 
		if( pOType == PhysConn_Video_VideoDecoder)
			break;
	}

	hr = pX->Route(pOIndex,pIndex); 
	pX->Release();

	if(FAILED(hr))
		return false;

	return true;
}

bool CCaptureVideo::RealRunTVTuner()
{
	HRESULT hr = m_pMC->Run();
	if (FAILED(hr))
		return false;
	return true;
}

bool CCaptureVideo::RealRunWebcam( unsigned int auIndex)
{
	HRESULT hr = m_pMCMoniker->Run();
	if (FAILED(hr))
		return false;
	return true;
}

bool CCaptureVideo::StartWebcam( unsigned int auIndex)
{
	// If Previous mode is Video then Close all interfaces
	if( vType == 0 )	
		CloseInterfaces();
	vType = 1; // Initializing current capture device to WebCam

	// Initializing Interfaces and Capture using WebCam
	HRESULT hr = CaptureVideoByMoniker( auIndex);
	if(FAILED(hr))
	{
		//AfxMessageBox("No Webcam Found.!") ;
		return false;
	}
	return true;
}

void CCaptureVideo::StopCapture()
{
	if( vType == 1) // If WebCam
		CloseInterfacesMoniker();
	CloseInterfaces(); 
	m_frameSizeWasSet = false;
	bVideo = FALSE;
}

HBITMAP CCaptureVideo::MakeSnapshot(bool isFirstShot)
{
	if( !m_pGrab)
		return 0;

	HRESULT	hr=0;
	
	long EvCode;
	if (isFirstShot)
		hr = m_pMEMoniker->WaitForCompletion(5000, &EvCode);
	
	AM_MEDIA_TYPE MediaType;

	hr=m_pGrab->GetConnectedMediaType(&MediaType); 
	if( FAILED( hr))
		return 0;

	// Get a pointer to the video header.
	VIDEOINFOHEADER *pVideoHeader = (VIDEOINFOHEADER*)MediaType.pbFormat;
	// The video header contains the bitmap information. 
	// Copy it into a BITMAPINFO structure.
	BITMAPINFO BitmapInfo;
	ZeroMemory(&BitmapInfo, sizeof(BitmapInfo));
	CopyMemory(&BitmapInfo.bmiHeader, &(pVideoHeader->bmiHeader), sizeof(BITMAPINFOHEADER));
	// Create a DIB from the bitmap header, and get a pointer to the buffer.
	unsigned long size = 0;
	void *buffer = NULL;
	HBITMAP hBitmap = 0;
	
	hBitmap = CreateDIBSection(0, &BitmapInfo, DIB_RGB_COLORS, &buffer, NULL, 0);
	
	if( hBitmap == 0)
		return 0;

	hr = m_pGrab->GetCurrentBuffer((long*)&size, NULL);
	if( SUCCEEDED(hr))
	{
		if( size == pVideoHeader->bmiHeader.biSizeImage)
		{
			hr = m_pGrab->GetCurrentBuffer((long*)&size, (long *)buffer);
		}
		else
		{
			void*	tbuffer = calloc(size, 1);
			hr = m_pGrab->GetCurrentBuffer((long*)&size, (long *)tbuffer);
			memcpy( buffer, tbuffer,size>pVideoHeader->bmiHeader.biSizeImage?pVideoHeader->bmiHeader.biSizeImage:size);
			free(tbuffer);
		}
	}

	if (isFirstShot)
	{
		hr = m_pGrab->SetOneShot(FALSE);
		m_pMCMoniker->Stop();
		m_pMCMoniker->Run();
	}

	return hBitmap;
}

IBaseFilter*	CCaptureVideo::GetSourceFilter()
{
	if( vType == 1) // If WebCam
	{
		if( m_pSourceWCFilter)
		{
			//m_pSourceWCFilter->AddRef();
			return m_pSourceWCFilter;
		}
	}
	else	if( vType == 0) // If Video
	{
		if( pSrcF)
		{
			//pSrcF->AddRef();
			return pSrcF;
		}
	}
	else	// unknown device
	{
		ATLASSERT( FALSE);
	}
	return NULL;
}

//------------------Device info--------------------//
unsigned int CCaptureVideo::GetVideoCamerasCount()
{
	IMoniker*	monikers[MAX_WEBCAMERAS_TO_DETECT];
	unsigned int found = 0;

	if( SUCCEEDED( FindCaptureDeviceCategoryMonikers(&monikers[0], MAX_WEBCAMERAS_TO_DETECT, &found)))
	{
		for(unsigned int i=0; i<found; i++)
			SAFE_RELEASE( monikers[i]);
		return found;
	}
	return 0;
}

unsigned int CCaptureVideo::GetTVTunersCount()
{
	IBaseFilter*	filters[MAX_WEBCAMERAS_TO_DETECT];
	unsigned int found = 0;

	if( SUCCEEDED( FindCaptureCategoryDevices(PIN_CATEGORY_ANALOGVIDEOIN, &filters[0], MAX_WEBCAMERAS_TO_DETECT, &found)))
	{
		for(unsigned int i=0; i<found; i++)
			SAFE_RELEASE( filters[i]);
		return found;
	}
	return 0;
}




void CCaptureVideo::ResizeVideoWindowMoniker()
{	
	// Resize the video preview window to match owner window size
	if (m_pVWMoniker)
	{	
		RECT rc;
		// Make the preview video fill our window
		::GetClientRect(m_hApp,&rc);
		m_pVWMoniker->SetWindowPosition(0, 0, rc.right, rc.bottom);
	}		
}

void CCaptureVideo::ResizeVideoWindow()
{
	// Resize the video preview window to match owner window size
	if (m_pVW)
	{
		RECT rc;

		// Make the preview video fill our window
		::GetClientRect(m_hApp,&rc);
		m_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);
	}
}

void CCaptureVideo::DisplayMesg(TCHAR *szFormat, ...)
{
	TCHAR szBuffer[1024];  // Large buffer for long filenames or URLs
	const size_t NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
	const int LASTCHAR = NUMCHARS - 1;

	// Format the input string
	va_list pArgs;
	va_start(pArgs, szFormat);

	// Use a bounded buffer size to prevent buffer overruns.  Limit count to
	// character size minus one to allow for a NULL terminating character.
	_vsnprintf_s(szBuffer, NUMCHARS - 1, szFormat, pArgs);
	va_end(pArgs);

	// Ensure that the formatted string is NULL-terminated
	szBuffer[LASTCHAR] = TEXT('\0');

	::MessageBox(NULL, szBuffer, TEXT("LiveVideo Message"), MB_OK | MB_ICONERROR);

}


HRESULT CCaptureVideo::AddGraphToRotMoniker(IUnknown *pUnkGraph, DWORD *pdwRegister)
{
/*	IMoniker * pMoniker;
	IRunningObjectTable *pROT;
	WCHAR wsz[128];
	HRESULT hr;

	if (!pUnkGraph || !pdwRegister)
		return E_POINTER;

	if (FAILED(GetRunningObjectTable(0, &pROT)))
		return E_FAIL;

	wsprintfW(wsz, L"FilterGraph %08x pid %08x\0", (DWORD_PTR)pUnkGraph, 
		GetCurrentProcessId());

	hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if (SUCCEEDED(hr)) 
	{
		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph, 
			pMoniker, pdwRegister);
		pMoniker->Release();
	}

	pROT->Release();
	return hr;*/
	return E_FAIL;
}

void CCaptureVideo::RemoveGraphFromRotMoniker(DWORD pdwRegister)
{
	IRunningObjectTable *pROT;

	if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) 
	{
		pROT->Revoke(pdwRegister);
		pROT->Release();
	}
}

HRESULT CCaptureVideo::HandleGraphEventMoniker()
{
	LONG evCode, evParam1, evParam2;
	HRESULT hr=S_OK;

	if (!m_pMEMoniker)
		return E_POINTER;

	// Process all queued events
	while(SUCCEEDED(m_pMEMoniker->GetEvent(&evCode, (LONG_PTR *) &evParam1, 
		(LONG_PTR *) &evParam2, 0)))
	{
		// Insert event processing code here, if desired
		switch (evCode)
		{
			// When the user closes the capture window, close the app.
		case EC_DEVICE_LOST:
		case EC_ERRORABORT:
		case EC_USERABORT:
			DisplayMesg(TEXT("Received an error event: 0x%x.  Closing app.\0"), evCode);
			::PostMessage(m_hApp, WM_CLOSE, 0, 0);
			break;
		}
		hr = m_pMEMoniker->FreeEventParams(evCode, evParam1, evParam2);        
	}

	return hr;
}

HRESULT CCaptureVideo::SetupVideoWindowMoniker()
{
	HRESULT hr;

	// Set the video window to be a child of the main window
	hr = m_pVWMoniker->put_Owner((OAHWND)m_hApp);
	if (FAILED(hr))
		return hr;

	// Set video window style
	hr = m_pVWMoniker->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	if (FAILED(hr))
		return hr;

	ResizeVideoWindowMoniker();

	// Make the video window visible, now that it is properly positioned
	hr = m_pVWMoniker->put_Visible(OATRUE);
	if (FAILED(hr))
		return hr;

	hr = m_pVWMoniker->put_MessageDrain((OAHWND)m_hApp);

	return hr;
}


HRESULT CCaptureVideo::AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
{
/*	IMoniker * pMoniker;
	IRunningObjectTable *pROT;
	WCHAR wsz[128];
	HRESULT hr;

	if (!pUnkGraph || !pdwRegister)
		return E_POINTER;

	if (FAILED(GetRunningObjectTable(0, &pROT)))
		return E_FAIL;

	wsprintfW(wsz, L"FilterGraph %08x pid %08x\0", (DWORD_PTR)pUnkGraph, 
		GetCurrentProcessId());

	hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if (SUCCEEDED(hr)) 
	{
		// Use the ROTFLAGS_REGISTRATIONKEEPSALIVE to ensure a strong reference
		// to the object.  Using this flag will cause the object to remain
		// registered until it is explicitly revoked with the Revoke() method.
		//
		// Not using this flag means that if GraphEdit remotely connects
		// to this graph and then GraphEdit exits, this object registration 
		// will be deleted, causing future attempts by GraphEdit to fail until
		// this application is restarted or until the graph is registered again.
		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph, 
			pMoniker, pdwRegister);
		pMoniker->Release();
	}

	pROT->Release();
	return hr;*/
	return E_FAIL;
}

HRESULT CCaptureVideo::SetupVideoWindow()
{
	HRESULT hr;

	// Set the video window to be a child of the main window
	hr = m_pVW->put_Owner((OAHWND)m_hApp);
	if (FAILED(hr))
		return hr;

	// Set video window style
	hr = m_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	if (FAILED(hr))
		return hr;

	hr = m_pVW->put_Width(640) ;
	hr = m_pVW->put_Height(480) ;

	// Use helper function to position video window in client rect 
	// of main application window
	ResizeVideoWindow();

	// Make the video window visible, now that it is properly positioned
	hr = m_pVW->put_Visible(OATRUE);
	if (FAILED(hr))
		return hr;
	hr = m_pVW->put_MessageDrain((OAHWND)m_hApp);

	return hr;
}
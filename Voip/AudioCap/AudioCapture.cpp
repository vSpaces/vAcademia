#include "stdafx.h"
#include "AudioCapture.h"
#include "InputDevice.h"
#include "SoundVolumeController.h"

CAudioCapture::CAudioCapture( HWND hWnd)
{
	ATLASSERT( hWnd);
	if ( !hWnd)
		return;
	m_hWnd = hWnd;

	m_sPinNameArray[ 0] = "stereo mix";
	m_sPinNameArray[ 1] = "stereo mixer";
	m_sPinNameArray[ 2] = "стерео микшер";	
	m_sPinNameArray[ 3] = "микшер";
	m_sPinNameArray[ 4] = "mixer";
	m_sPinNameArray[ 5] = "mix";
	m_sPinNameArray[ 6] = "микрофон";
	m_sPinNameArray[ 7] = "microphone";
	m_sPinNameArray[ 8] = "mic";
	
	m_pSoundVolumeController = NULL;
	m_dMixLevel = -1.0;
}

CAudioCapture::~CAudioCapture()
{
	if ( m_pSoundVolumeController != NULL)
	{
		m_pSoundVolumeController->restore();
		delete m_pSoundVolumeController;
		m_pSoundVolumeController = NULL;
	}
	Close();
}

//////////////////////////////////////////////////////////////////////////

BOOL CAudioCapture::InitializeCapture()
{
	CoInitialize( NULL);
	SetDefaults();

	if ( FAILED( GetInterfaces()))
		return S_FALSE;

	if ( FAILED( FillLists()))
		return S_FALSE;
	
	return TRUE;
}

HRESULT CAudioCapture::FillLists()
{
	HRESULT hr;

	// Enumerate and display the audio input devices installed in the system
	hr = EnumFiltersWithMonikerToList( NULL, &CLSID_AudioInputDeviceCategory);
	if ( FAILED( hr))
		return hr;
	
	//OnSelectInputDevice();
	return hr;
}

HRESULT CAudioCapture::GetInterfaces()
{
	HRESULT hr;

	// Create the filter graph.
	hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder, (void **)&m_pGB);
	if ( FAILED( hr) || !m_pGB)
		return E_NOINTERFACE;

	// Create the capture graph builder.
	hr = CoCreateInstance( CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, 
		IID_ICaptureGraphBuilder2, (void **)&m_pCapture);
	if ( FAILED( hr) || !m_pCapture)
		return E_NOINTERFACE;

	// Associate the filter graph with the capture graph builder
	hr = m_pCapture->SetFiltergraph( m_pGB);    
	if ( FAILED( hr))
		return hr;

	// Get useful interfaces from the graph builder
	//JIF( hr = m_pGB->QueryInterface( IID_IMediaControl, (void **)&m_pMC));
	//JIF( hr = m_pGB->QueryInterface( IID_IMediaEventEx, (void **)&m_pME));

	hr = m_pGB->QueryInterface( IID_IMediaControl, (void **)&m_pMC);
	hr = m_pGB->QueryInterface( IID_IMediaEventEx, (void **)&m_pME);

	// Have the graph signal events via window callbacks
	hr = m_pME->SetNotifyWindow( ( OAHWND) m_hWnd, WM_GRAPHNOTIFY, RECORD_EVENT);

//CLEANUP:
	return hr;
}

HRESULT CAudioCapture::EnumFiltersWithMonikerToList( ICreateDevEnum *pSysDevEnum, 
													 const GUID *clsid)
{
	HRESULT hr;
	IEnumMoniker *pEnumCat = NULL;

	// Instantiate the system device enumerator if it doesn't exist
	if ( pSysDevEnum == NULL)
	{
		hr = CoCreateInstance( CLSID_SystemDeviceEnum, NULL, 
			CLSCTX_INPROC, IID_ICreateDevEnum, 
			(void **) &pSysDevEnum);
		if ( FAILED( hr))
			return hr;
	}

	// Enumerate all filters of the selected category  
	hr = pSysDevEnum->CreateClassEnumerator( *clsid, &pEnumCat, 0);
	if ( SUCCEEDED( hr))
	{
		// Enumerate all filters using the category enumerator
		hr = EnumFiltersAndMonikersToList( pEnumCat);

		SAFE_RELEASE( pEnumCat);
	}

	pSysDevEnum->Release();
	return hr;
}

HRESULT CAudioCapture::EnumFiltersAndMonikersToList( IEnumMoniker *pEnumCat)
{
	HRESULT hr = S_OK;
	IMoniker *pMoniker = 0;
	ULONG cFetched = 0;
	VARIANT varName = {0};
	int nFilters = 0;

	// If there are no filters of a requested type, show default string
	if ( !pEnumCat)
	{
		//ListFilters.AddString( TEXT( "<< No entries >>\0"));
		return S_FALSE;
	}

	// Enumerate all items associated with the moniker
	while( pEnumCat->Next( 1, &pMoniker, &cFetched) == S_OK)
	{
		IPropertyBag *pPropBag;
		ATLASSERT( pMoniker);

		// Associate moniker with a file
		hr = pMoniker->BindToStorage( 0, 0, IID_IPropertyBag, 
										(void **) &pPropBag);
		ATLASSERT( SUCCEEDED( hr));
		ATLASSERT( pPropBag);
		if ( FAILED( hr))
			continue;

		// Read filter name from property bag
		varName.vt = VT_BSTR;
		hr = pPropBag->Read( L"FriendlyName", &varName, 0);
		if ( FAILED( hr))
			continue;

		// Get filter name (converting BSTR name to a CString)
		CComString str( varName.bstrVal);
		SysFreeString( varName.bstrVal);
		nFilters++;

		// Add the filter name and moniker to the listbox
		AddFilterToListWithMoniker( str.GetString(), pMoniker);

		// Cleanup interfaces
		SAFE_RELEASE( pPropBag);

		// Intentionally DO NOT release the pMoniker, since it is
		// stored in a listbox for later use
	}

	return hr;
}

void CAudioCapture::AddFilterToListWithMoniker( const TCHAR *szFilterName, IMoniker *pMoniker)
{
	if ( !szFilterName)
		return;

	CInputDevice *pInputFilter = new CInputDevice( szFilterName, pMoniker);
	m_inputDevicesList.push_back( pInputFilter);	
}

HRESULT CAudioCapture::EnumPinsOnFilter( IBaseFilter *pFilter, PIN_DIRECTION PinDir)
{
	HRESULT hr;
	IEnumPins  *pEnum = NULL;
	IPin *pPin = NULL;

	// Clear the specified listbox (input or output)
	m_inputPinsList.clear();	

	// Verify filter interface
	if ( !pFilter)
		return E_NOINTERFACE;

	// Get pin enumerator
	hr = pFilter->EnumPins( &pEnum);
	if ( FAILED( hr))
	{
		//Listbox.AddString(TEXT("<ERROR>\0"));
		return hr;
	}

	pEnum->Reset();

	// Enumerate all pins on this filter
	while(( hr = pEnum->Next(1, &pPin, 0)) == S_OK)
	{
		PIN_DIRECTION PinDirThis;

		hr = pPin->QueryDirection( &PinDirThis);
		if ( FAILED( hr))
		{
			//Listbox.AddString(TEXT("<ERROR>\0"));
			pPin->Release();
			continue;
		}

		// Does the pin's direction match the requested direction?
		if ( PinDir == PinDirThis)
		{
			PIN_INFO pininfo = {0};

			// Direction matches, so add pin name to listbox
			hr = pPin->QueryPinInfo( &pininfo);
			if ( SUCCEEDED( hr))
			{
				CComString str( pininfo.achName);
				//Listbox.AddString(str);
				m_inputPinsList.push_back( pininfo.achName);
			}

			// The pininfo structure contains a reference to an IBaseFilter,
			// so you must release its reference to prevent resource a leak.
			pininfo.pFilter->Release();
		}
		pPin->Release();
	}
	pEnum->Release();

	return hr;
}

HRESULT CAudioCapture::ActivateSelectedInputPin( TCHAR *sPinName, BOOL &bPinFinded) 
{
	HRESULT hr = S_OK;
//	IPin *pPin = 0;

	// How many pins are in the input pin list?
	int nPins = m_inputPinsList.size();
	//int nActivePin = m_ListInputPins.GetCurSel();

	bPinFinded = FALSE;

	if ( nPins == 0)
		return S_FALSE;

	PPin *pPinList = new PPin[ nPins];
	PAMAudioInputMixer *pPinMixerList = new PAMAudioInputMixer[ nPins];	
	
	int pinIndex = -1;

	// Activate the selected input pin and deactivate all others
	for ( int i = 0; i < nPins; i++)
	{
		pPinList[ i] = NULL;
		// Get this pin's interface
		hr = GetPin( m_pInputDevice, PINDIR_INPUT, i, &pPinList[ i]);
		if ( SUCCEEDED( hr))
		{
			pPinMixerList[ i] = NULL;
			hr = pPinList[ i]->QueryInterface( IID_IAMAudioInputMixer, (void **) &pPinMixerList[ i]);
			if ( SUCCEEDED( hr))
			{				
				CComString &cmsPinName = m_inputPinsList[ i];
				cmsPinName.MakeLower();
				if ( cmsPinName.Find( sPinName) > -1)
				{					
					bPinFinded = TRUE;
					pinIndex = i;
				}				
			}
			else
				pPinMixerList[ i] = NULL;
		}
		else
			pPinList[ i] = NULL;
	}

	for ( int i = 0; i < nPins; i++)
	{
		if ( i == pinIndex)
		{
			hr = SetInputPinProperties( pPinMixerList[ i]);
			pPinMixerList[ i]->put_Enable( TRUE);
			
			HRESULT hr = pPinMixerList[ i]->get_MixLevel( &m_dMixLevel);

			if ( SUCCEEDED( hr))
				hr = pPinMixerList[ i]->put_MixLevel( 0.10);

			if ( hr == S_FALSE || FAILED( hr))
			{
				m_dMixLevel = -1.0;
				CComString &cmsPinName = m_inputPinsList[ pinIndex];
				cmsPinName.MakeLower();

				if ( cmsPinName.Find( m_sPinNameArray[ 0]) > -1 || cmsPinName.Find( m_sPinNameArray[ 1]) > -1 
					|| cmsPinName.Find( m_sPinNameArray[ 2]) > -1 || cmsPinName.Find( m_sPinNameArray[ 3]) > -1
					|| cmsPinName.Find( m_sPinNameArray[ 4]) > -1 || cmsPinName.Find( m_sPinNameArray[ 5]) > -1)
				{					
					if ( m_pSoundVolumeController == NULL)
						m_pSoundVolumeController = new CSoundVolumeController();
					m_pSoundVolumeController->setMicVolume( 0.10);
				}
			}
		}
		else
			pPinMixerList[ i]->put_Enable( FALSE);

		if ( pPinMixerList[ i] != NULL)
			hr = pPinMixerList[ i]->Release();
		
		if ( pPinList[ i] != NULL)
			pPinList[ i]->Release();
	}

	return hr;
}

HRESULT CAudioCapture::SaveUserInputPinsSetting()
{
	m_userSettingInputPinsSaver.clear();

	HRESULT hr = S_OK;
	IPin *pPin = 0;
	IAMAudioInputMixer *pPinMixer;

	// How many pins are in the input pin list?
	int nPins = m_inputPinsList.size();	

	// Activate the selected input pin and deactivate all others
	for ( int i = 0; i < nPins; i++)
	{
		// Get this pin's interface
		hr = GetPin( m_pInputDevice, PINDIR_INPUT, i, &pPin);
		if ( SUCCEEDED( hr))
		{
			hr = pPin->QueryInterface( IID_IAMAudioInputMixer, (void **) &pPinMixer);
			if ( SUCCEEDED( hr))
			{				
				BOOL bEnabled;
				pPinMixer->get_Enable( &bEnabled);
				m_userSettingInputPinsSaver.push_back( bEnabled);

				pPinMixer->Release();
			}

			// Release pin interfaces
			pPin->Release();
		}
	}
	return hr;
}

HRESULT CAudioCapture::RestoreUserInputPinsSetting() 
{
	HRESULT hr = S_OK;
	IPin *pPin = 0;
	IAMAudioInputMixer *pPinMixer;

	if ( m_userSettingInputPinsSaver.size() < 1)
		return hr;

	// How many pins are in the input pin list?
	int nPins = m_inputPinsList.size();
	int j = 0;	

	// Activate the selected input pin and deactivate all others
	for ( int i = 0; i < nPins; i++)
	{
		// Get this pin's interface
		hr = GetPin( m_pInputDevice, PINDIR_INPUT, i, &pPin);
		if ( SUCCEEDED( hr))
		{
			hr = pPin->QueryInterface( IID_IAMAudioInputMixer, (void **) &pPinMixer);
			if ( SUCCEEDED( hr))
			{
				// If this is our selected pin, enable it
				if ( j >= m_userSettingInputPinsSaver.size())
				{
					ATLASSERT( j < m_userSettingInputPinsSaver.size());
					return hr;
				}
				BOOL bEnabled = FALSE;
				
				pPinMixer->get_Enable( &bEnabled);
				if ( bEnabled && !m_userSettingInputPinsSaver[ j])
				{
					if ( m_dMixLevel != -1.0)
						pPinMixer->put_MixLevel( m_dMixLevel);
					else
						m_pSoundVolumeController->restore();
				}
				hr = pPinMixer->put_Enable( m_userSettingInputPinsSaver[ j]);
				j++;
				pPinMixer->Release();
			}

			// Release pin interfaces
			pPin->Release();
		}
	}
	m_userSettingInputPinsSaver.clear();

	return hr;
}

/*HRESULT AddFilterPinsToLists (IGraphBuilder *pGB, CListBox& m_ListFilters,
							  CListBox& m_ListPinsInput, CListBox& m_ListPinsOutput) 
{
	HRESULT hr;
	IBaseFilter *pFilter = NULL;
	TCHAR szNameToFind[128];

	// Read the current filter name from the list box
	int nCurSel = m_ListFilters.GetCurSel();
	m_ListFilters.GetText(nCurSel, szNameToFind);

	// Read the current list box name and find it in the graph
	pFilter = FindFilterFromName(pGB, szNameToFind);
	if (!pFilter)
		return E_UNEXPECTED;

	// Now that we have filter information, enumerate pins by direction
	// and add their names to the appropriate listboxes
	hr = EnumPinsOnFilter(pFilter, PINDIR_INPUT, m_ListPinsInput);

	if (SUCCEEDED(hr))
	{
		hr = EnumPinsOnFilter(pFilter, PINDIR_OUTPUT, m_ListPinsOutput);
	}

	// Must release the filter interface returned from FindFilterByName()
	pFilter->Release();

	return hr;
}*/


//////////////////////////////////////////////////////////////////////////

void CAudioCapture::ClearFilterListWithMoniker()
{
	IMoniker *pMoniker = NULL;

	int nCount = m_inputDevicesList.size();

	// Delete any CLSID pointers that were stored in the listbox item data
	for ( int i = 0; i < nCount; i++)
	{
		pMoniker = (IMoniker *) (( CInputDevice *) m_inputDevicesList[ i])->GetMoniker();
		if ( pMoniker != 0)
		{
			pMoniker->Release();
			pMoniker = NULL;
		}
	}

	// Clean up	
	m_inputDevicesList.clear();
}

void CAudioCapture::ClearLists()
{
	m_inputDevicesList.clear();
	m_inputPinsList.clear();
}

void CAudioCapture::FreeInterfaces()
{
	// Disable event callbacks
	if ( m_pME)
		m_pME->SetNotifyWindow((OAHWND)NULL, 0, 0);

	// Release and zero DirectShow interface pointers
	SAFE_RELEASE( m_pMC);
	SAFE_RELEASE( m_pME);
	SAFE_RELEASE( m_pSplitter);
	SAFE_RELEASE( m_pFileWriter);
	SAFE_RELEASE( m_pWAVDest);
	SAFE_RELEASE( m_pInputDevice);
	SAFE_RELEASE( m_pGB);
	SAFE_RELEASE( m_pCapture);
}

void CAudioCapture::FreePlaybackInterfaces()
{
	HRESULT hr;

	// Disable event callbacks
	if ( m_pMEPlayback)
		hr = m_pMEPlayback->SetNotifyWindow( (OAHWND) NULL, 0, 0);

	// Release and zero DirectShow interface pointers
	SAFE_RELEASE( m_pMEPlayback);
	SAFE_RELEASE( m_pMCPlayback);
	SAFE_RELEASE( m_pGBPlayback);
}


void CAudioCapture::ReleaseCapture()
{
	ClearFilterListWithMoniker();
	ClearLists();

	DestroyPreviewStream();
	DestroyCaptureStream();

	FreeInterfaces();
	FreePlaybackInterfaces();
}

HRESULT CAudioCapture::ResetCapture()
{
	ClearLists();
	FreeInterfaces();
	return GetInterfaces();
}

HRESULT CAudioCapture::RenderCaptureStream()
{
	USES_CONVERSION;
	HRESULT hr=0;
	IFileSinkFilter2 *pFileSink;

	// Create the WAVDest filter
	hr = CoCreateInstance( CLSID_WavDest, NULL, CLSCTX_INPROC,
		IID_IBaseFilter, (void **)&m_pWAVDest);
	if ( FAILED( hr))
	{
		/*MessageBox(TEXT("Couldn't create a WAVDest filter.  This sample filter is installed ")
			TEXT("with the DirectX SDK in the DirectShow\\Filters\\WavDest ")
			TEXT("directory.\n\nPlease build this filter (with Visual C++) and ")
			TEXT("register it by running 'Tools->Register Control' once the ")
			TEXT("filter finishes building.\nThis helper filter is used ")
			TEXT("when writing .WAV files to disk, and this sample requires it.\n\n")
			TEXT("You may still audition the audio input, but you will not be ")
			TEXT("able to write the data to a file."),
			TEXT("Error loading WAVDest helper filter!"));*/
		return E_NOINTERFACE; 
	}

	// Create the FileWriter filter
	hr = CoCreateInstance( CLSID_FileWriter, NULL, CLSCTX_INPROC,
		IID_IFileSinkFilter2, (void **)&pFileSink);
	if ( FAILED( hr))
		return E_NOINTERFACE; 

	// Get the file sink interface from the File Writer
	hr = pFileSink->QueryInterface( IID_IBaseFilter, (void **)&m_pFileWriter);
	if ( FAILED( hr))
		return E_NOINTERFACE; 

	// Add the WAVDest filter to the graph
	hr = m_pGB->AddFilter( m_pWAVDest, L"WAV Dest");
	if ( FAILED( hr))
		return hr;

	// Add the FileWriter filter to the graph
	hr = m_pGB->AddFilter( (IBaseFilter *)m_pFileWriter, L"File Writer");
	if ( FAILED( hr))
		return hr;

	// Set filter to always overwrite the file
	hr = pFileSink->SetMode( AM_FILE_OVERWRITE);

	// Set the output filename, which must be a wide string
	TCHAR *szEditFile = m_sFileName.GetBuffer();
	WCHAR wszFilename[ MAX_PATH];
	
	//m_StrFilename.GetWindowText(szEditFile, MAX_PATH);
	if ( szEditFile[ 0] == TEXT('\0'))
		wcsncpy( wszFilename, T2W( DEFAULT_FILENAME), NUMELMS( wszFilename) - 1);
	else
		wcsncpy( wszFilename, T2W( szEditFile), NUMELMS( wszFilename) - 1);

	wszFilename[ MAX_PATH - 1] = 0;    // Ensure NULL termination

	hr = pFileSink->SetFileName( wszFilename, NULL);
	if ( FAILED( hr))
		return hr; 

	// Get the pin interface for the capture pin
	IPin *pPin = 0;
	if ( m_pSplitter)
		hr = GetPin( m_pSplitter, PINDIR_OUTPUT, 0, &pPin);
	else
		hr = GetPin( m_pInputDevice, PINDIR_OUTPUT, 0, &pPin);

	// Connect the new filters
	if ( pPin)
	{
		hr = m_pGB->Render( pPin);
		pPin->Release();
	}

	// Release the FileSinkFilter2 interface, since it's no longer needed
	SAFE_RELEASE( pFileSink);
	return hr;
}


HRESULT CAudioCapture::DestroyCaptureStream()
{
	HRESULT hr = 0;

	// Destroy Audio renderer filter, if it exists
	if ( m_pWAVDest)
	{
		hr = m_pGB->RemoveFilter( m_pWAVDest);
		SAFE_RELEASE( m_pWAVDest);
	}

	// Destroy Smart Tee filter, if it exists
	if ( m_pFileWriter)
	{
		hr = m_pGB->RemoveFilter( m_pFileWriter);
		SAFE_RELEASE( m_pFileWriter);
	}

	return hr;
}


HRESULT CAudioCapture::RenderPreviewStream()
{
	USES_CONVERSION;
	HRESULT hr=0;
	WCHAR wszFilter[ 128];

	// If we've already configured the stream, just exit
	if ( m_pRenderer)
		return S_OK;

	// Set the requested audio properties - buffer sizes, channels, freq, etc.
	hr = SetAudioProperties();

	// Render the preview stream
	hr = m_pCapture->RenderStream(
		&PIN_CATEGORY_PREVIEW, 
		&MEDIATYPE_Audio, 
		m_pInputDevice, 
		NULL,   // No compression filter.
		NULL    // Default renderer.
		);

	// Some capture sources will have only a Capture pin (no Preview pin).
	// In that case, the Capture Graph builder will automatically insert
	// a SmartTee filter, which will split the stream into a Capture stream
	// and a Preview stream.  In that case, it's not an error.
	if ( hr == VFW_S_NOPREVIEWPIN)
	{
		wcscpy( wszFilter, T2W( TEXT( "Smart Tee\0")));

		// Get the interface for the Splitter filter for later use
		hr = m_pGB->FindFilterByName( wszFilter, &m_pSplitter);
		if ( FAILED( hr))
			return hr;
	}
	else if ( FAILED( hr))
		return hr;

	// Get the interface for the audio renderer filter for later use
	wcscpy( wszFilter, T2W( TEXT( "Audio Renderer\0")));
	hr = m_pGB->FindFilterByName( wszFilter, &m_pRenderer);   

	return hr;
}

HRESULT CAudioCapture::DestroyPreviewStream()
{
	HRESULT hr=0;

	// Destroy Smart Tee filter, if it exists
	if ( m_pSplitter)
	{
		hr = m_pGB->RemoveFilter( m_pSplitter);
		SAFE_RELEASE( m_pSplitter);
	}

	// Destroy Audio renderer filter, if it exists
	if ( m_pRenderer)
	{
		hr = m_pGB->RemoveFilter( m_pRenderer);
		SAFE_RELEASE( m_pRenderer);
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////

HRESULT CAudioCapture::selectInputDevice( BOOL &bInputSelected)
{
	HRESULT hr = S_FALSE;
	IMoniker *pMoniker = 0;

	// Release and delete any previous capture filter graph
	//if ( m_pInputDevice)
	//	hr = ResetCapture();

	//ATLASSERT( SUCCEEDED( hr));
	//if ( FAILED( hr))
	//	return hr;

	ATLASSERT( m_inputDevicesList.size() > 0);
	if ( m_inputDevicesList.size() < 1)
		return S_FALSE;
	
	// Read the stored moniker pointer from the list box's item data	
	CInputDevice *pInputFilter = m_inputDevicesList[ m_iActiveDeviceIndex];
	
	ATLASSERT( pInputFilter);
	if ( !pInputFilter)
		return S_FALSE;

	pMoniker = pInputFilter->GetMoniker();

	ATLASSERT( pMoniker);
	if ( !pMoniker)
		return S_FALSE;

	// Use the moniker to create the specified audio capture device
	hr = pMoniker->BindToObject( 0, 0, IID_IBaseFilter, (void**) &m_pInputDevice);   
	if ( FAILED( hr))
		return hr;

	// Add the capture filter to the filter graph
	hr = m_pGB->AddFilter( m_pInputDevice, L"Audio Capture");
	if ( FAILED( hr))
		return hr;

	// List and display the available input pins on the capture filter.
	// Many devices offer multiple pins for input (microphone, CD, LineIn, etc.)
	hr = EnumPinsOnFilter( m_pInputDevice, PINDIR_INPUT);
	if ( FAILED( hr))
		return hr;

	hr = SaveUserInputPinsSetting();
	// Enable the properties button, if supported by the selected input device
	//m_btnProperties.EnableWindow(SupportsPropertyPage(m_pInputDevice));

	// Select the first available input pin by default
	//m_ListInputPins.SetCurSel(0);
	hr = selectInputPins( bInputSelected);	

	// Now display the existing filters in the graph for diagnostic help
	//UpdateFilterLists( m_pGB);
	return hr;
}

HRESULT CAudioCapture::selectInputPins( BOOL &bInputPinFinded)
{
	HRESULT hr;

	// Make sure that playback is stopped
	Stop();

	// Select the capture device's input pin
	BOOL bPinFinded = FALSE;
	int index = 0;

	if ( m_inputPinsList.size() == 0)
		return S_FALSE;

	hr = ActivateSelectedInputPin( m_sPinName.GetBuffer(), bPinFinded);

	/*
	if ( FAILED( hr))
		return hr;
	
	*/
	while ( !bPinFinded && index < VARIAN_PIN_COUNT)
	{
		m_sPinName = m_sPinNameArray[ index];
		hr = ActivateSelectedInputPin( m_sPinName.GetBuffer(), bPinFinded); 
		index++;
    }
/*
	if ( FAILED( hr))
		return hr;
*/
	bInputPinFinded = bPinFinded;
	return bPinFinded ? S_OK : S_FALSE;
}

HRESULT CAudioCapture::UpdateFilterLists( IGraphBuilder *pUpdateGB)
{
	// Now display the existing filters in the graph for diagnostic help
	//AddGraphFiltersToList( pUpdateGB, m_inputDevicesList);

	// Refresh the filter/pin display lists
	//m_ListFilters.SetCurSel(0);
	//OnSelchangeListFilters();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////

void CAudioCapture::SetDefaults()
{
	// Zero DirectShow interface pointers (sanity check)
	m_pGB = m_pGBPlayback	= 0;
	m_pMC = m_pMCPlayback	= 0;
	m_pME = m_pMEPlayback	= 0;
	m_pCapture				= 0;
	m_pInputDevice			= 0;
	m_pSplitter				= 0;
	m_pRenderer				= 0;
	m_pWAVDest				= 0;
	m_pFileWriter			= 0;
	m_bPausedRecording		= FALSE;
	m_iActiveDeviceIndex	= 0;
	m_bCheckAudition		= FALSE;
	m_bCheckWriteFile		= FALSE;
	
	m_sPinName				= m_sPinNameArray[ 0];
	m_sFileName				= "c:/test.wav";
}

//////////////////////////////////////////////////////////////////////////

HRESULT CAudioCapture::HandleGraphEvent()
{
	LONG evCode, evParam1, evParam2;
	HRESULT hr = S_OK;

	if ( !m_pME)
		return S_OK;

	// Read all events currently on the event queue
	while( SUCCEEDED( m_pME->GetEvent( &evCode, (LONG_PTR *) &evParam1, 
			(LONG_PTR *) &evParam2, 0)))
	{
		if ( EC_DEVICE_LOST == evCode)
		{
			// Device was removed
			if ( evParam2 == 0)
			{
				Stop();
				ReleaseCapture();
				SetDefaults();
				//MessageBeep(0);
				//MessageBox(TEXT("The selected input device was removed!\0"),
				//	TEXT("DirectShow Audio Capture Sample\0"), 
				//	MB_OK | MB_ICONEXCLAMATION);
			}
			// Device was reattached
			else
			{
				// Restart from scratch
				InitializeCapture();
				//MessageBeep(0);
				//MessageBox(TEXT("The audio input device has been reattached!\0"),
				//	TEXT("DirectShow Audio Capture Sample\0"), 
				//	MB_OK | MB_ICONINFORMATION);
			}
		}

		// Free event parameters to prevent memory leaks
		hr = m_pME->FreeEventParams( evCode, evParam1, evParam2);
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////

HRESULT CAudioCapture::GetInputDevices( LPSTR *sInputDevices)
{
	ATLASSERT( m_inputDevicesList.size() > 0);
	if ( m_inputDevicesList.size() < 1)
		return E_FAIL;
	
	CComString sIndex = CComString();
	sIndex.Format( "%d|", m_iActiveDeviceIndex);

	strcpy( *sInputDevices, sIndex.GetBuffer());

	for ( int i = 0; i < m_inputDevicesList.size(); i++)
	{
		CInputDevice *pInputFilter = m_inputDevicesList[ i];
		//sInputDevices[ i] = (LPTSTR) pInputFilter->GetName();
		strcat( *sInputDevices, (LPCSTR) pInputFilter->GetName());
		if ( i < m_inputDevicesList.size() - 1)
			strcat( *sInputDevices, "|");
	}
	
	return S_OK;
}

HRESULT CAudioCapture::SelectInputDevice( int iActiveDeviceIndex, BOOL bContinue)
{
	int index = m_iActiveDeviceIndex;
	m_iActiveDeviceIndex = iActiveDeviceIndex;
	BOOL bInputPinFinded = FALSE;	
	m_sPinName = m_sPinNameArray[ 0];
	HRESULT hr = selectInputDevice( bInputPinFinded);

	if ( !bContinue)
		return bInputPinFinded ? hr : ( m_iActiveDeviceIndex = index, E_FAIL);

	m_iActiveDeviceIndex = 0;

	while ( !bInputPinFinded && m_iActiveDeviceIndex < m_inputDevicesList.size())
	{
		hr = selectInputDevice( bInputPinFinded);
		if ( FAILED( hr))
			return hr;
		if ( !bInputPinFinded)
			m_iActiveDeviceIndex++;
	}

	//m_iActiveDeviceIndex = m_iActiveDeviceIndex > iActiveDeviceIndex ? m_iActiveDeviceIndex - 1 : m_iActiveDeviceIndex;

	if ( bInputPinFinded)
	{		
		return hr;
	}
	m_iActiveDeviceIndex = index;
	
	return E_FAIL;
}

HRESULT CAudioCapture::RestoreUserInputDevice()
{
	return RestoreUserInputPinsSetting();
}

HRESULT CAudioCapture::Record()
{
	HRESULT hr;

	// Don't rebuild the graphs if we're returning from paused state
	if ( m_bPausedRecording)
	{
		if ( m_pMC)
			hr = m_pMC->Run();

		m_bPausedRecording = FALSE;
		//Say(TEXT("Auditioning/recording audio..."));
		return hr;
	}

	// If we're currently playing back the recorded file, stop playback
	// and clear the interfaces before beginning recording.
	FreePlaybackInterfaces(); 

	if ( m_bCheckAudition)
		hr = RenderPreviewStream();

	if ( m_bCheckWriteFile)
		hr = RenderCaptureStream();

	// Update the graph filter display to show rendered graph
	//UpdateFilterLists(m_pGB);

	if ( m_pMC)
	{
		hr = m_pMC->Run();
		//Say(TEXT("Auditioning/recording audio..."));
	}
	return hr;
}

HRESULT CAudioCapture::Pause()
{
	HRESULT hr;

	// We share the functionality of the Pause button with both
	// Record and Playback.  If the Playback interfaces aren't NULL,
	// then we're in the middle of playback, so pause playback.
	// Otherwise, pause recording.
	if ( m_pMCPlayback)
	{
		hr = m_pMCPlayback->Pause();        
		//Say(TEXT("Paused audio playback"));
	}
	else
	{
		if ( m_pMC)
		{
			hr = m_pMC->Pause();
			//Say(TEXT("Paused audio recording"));
			m_bPausedRecording = TRUE;
		}
	}
	return hr;
}

HRESULT CAudioCapture::Stop()
{
	HRESULT hr = S_FALSE;
	// Stop recording
	if ( m_pMC)
		hr = m_pMC->StopWhenReady();

	// Destroy the existing filter graph
	DestroyPreviewStream();
	DestroyCaptureStream();
	m_bPausedRecording = FALSE;

	// Update the graph filter display to show rendered graph
	//UpdateFilterLists( m_pGB);
	return hr;
}

void CAudioCapture::Close()
{
	if ( m_pMC)
		m_pMC->Stop();

	RestoreUserInputPinsSetting();
	ReleaseCapture();
	CoUninitialize();
}

void CAudioCapture::SetCheckAudition( BOOL bCheckAudition)
{
	m_bCheckAudition = bCheckAudition;
}

void CAudioCapture::SetCheckWriteFile( BOOL bCheckWriteFile)
{
	m_bCheckWriteFile = bCheckWriteFile;
}

void CAudioCapture::SetPinName( LPCTSTR sPinName)
{
	m_sPinName = sPinName;
	BOOL bPinFinded = FALSE;
	selectInputPins( bPinFinded);
}

void CAudioCapture::SetFileName( LPCTSTR sFileName)
{
	m_sFileName = sFileName;
}

BOOL CAudioCapture::GetCheckAudition()
{
	return m_bCheckAudition;
}

BOOL CAudioCapture::GetCheckWriteFile()
{
	return m_bCheckWriteFile;
}

LPCTSTR CAudioCapture::GetPinName()
{
	return m_sPinName.GetString();
}

LPCTSTR CAudioCapture::GetFileName()
{
	return m_sFileName.GetString();
}

//////////////////////////////////////////////////////////////////////////

HRESULT CAudioCapture::GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
{
	CComPtr< IEnumPins > pEnum;
	*ppPin = NULL;

	if ( !pFilter)
		return E_POINTER;

	HRESULT hr = pFilter->EnumPins( &pEnum);
	if ( FAILED( hr)) 
		return hr;

	ULONG ulFound;
	IPin *pPin;
	hr = E_FAIL;

	while ( S_OK == pEnum->Next( 1, &pPin, &ulFound))
	{
		PIN_DIRECTION pindir = ( PIN_DIRECTION) 3;

		pPin->QueryDirection( &pindir);
		if ( pindir == dirrequired)
		{
			if ( iNum == 0)
			{
				*ppPin = pPin;  // Return the pin's interface
				hr = S_OK;      // Found requested pin, so clear error
				break;
			}
			iNum--;
		} 

		pPin->Release();
	} 

	return hr;
}

void CAudioCapture::UtilDeleteMediaType( AM_MEDIA_TYPE *pmt)
{
	// Allow NULL pointers for coding simplicity
	if ( pmt == NULL) 
	{
		return;
	}

	// Free media type's format data
	if ( pmt->cbFormat != 0) 
	{
		CoTaskMemFree( (PVOID)pmt->pbFormat);

		// Strictly unnecessary but tidier
		pmt->cbFormat = 0;
		pmt->pbFormat = NULL;
	}

	// Release interface
	if ( pmt->pUnk != NULL) 
	{
		pmt->pUnk->Release();
		pmt->pUnk = NULL;
	}

	// Free media type
	CoTaskMemFree( (PVOID) pmt);
}


HRESULT CAudioCapture::SetAudioProperties()
{
	HRESULT hr=0;
	IPin *pPin=0;
	IAMBufferNegotiation *pNeg = 0;
	IAMStreamConfig *pCfg = 0;
	int nFrequency = 0;

	// Determine audio properties
	int nChannels = 2;//m_btnMono.GetCheck() ? 1 : 2;
	int nBytesPerSample = 16;//m_btn8BIT.GetCheck() ? 1 : 2;

	// Determine requested frequency
	//if ( IsDlgButtonChecked( IDC_RADIO_11KHZ))      
	//	nFrequency = 11025;
	//else if (IsDlgButtonChecked(IDC_RADIO_22KHZ)) 
	//	nFrequency = 22050;
	//else 
		nFrequency = 44100;

	// Find number of bytes in one second
	long lBytesPerSecond = (long) ( nBytesPerSample * nFrequency * nChannels);

	// Set to 50ms worth of data    
	long lBufferSize = (long) ( ( float) lBytesPerSecond * DEFAULT_BUFFER_TIME);

	// Get the buffer negotiation interface for each pin,
	// since there could be both a Capture and a Preview pin.
	for ( int i = 0; i < 2; i++)
	{
		hr = GetPin( m_pInputDevice, PINDIR_OUTPUT, i, &pPin);
		if ( SUCCEEDED( hr))
		{
			// Get buffer negotiation interface
			hr = pPin->QueryInterface( IID_IAMBufferNegotiation, (void **) &pNeg);
			if ( FAILED( hr))
			{
				pPin->Release();
				break;
			}

			// Set the buffer size based on selected settings
			ALLOCATOR_PROPERTIES prop = {0};
			prop.cbBuffer = lBufferSize;
			prop.cBuffers = 6;
			prop.cbAlign = nBytesPerSample * nChannels;
			hr = pNeg->SuggestAllocatorProperties( &prop);
			pNeg->Release();

			// Now set the actual format of the audio data
			hr = pPin->QueryInterface( IID_IAMStreamConfig, (void **)&pCfg);
			if ( FAILED( hr))
			{
				pPin->Release();
				break;
			}            

			// Read current media type/format
			AM_MEDIA_TYPE *pmt = {0};
			hr = pCfg->GetFormat( &pmt);

			if ( SUCCEEDED( hr))
			{
				// Fill in values for the new format
				WAVEFORMATEX *pWF = (WAVEFORMATEX *) pmt->pbFormat;
				pWF->nChannels = (WORD) nChannels;
				pWF->nSamplesPerSec = nFrequency;
				pWF->nAvgBytesPerSec = lBytesPerSecond;
				pWF->wBitsPerSample = (WORD) (nBytesPerSample * 8);
				pWF->nBlockAlign = (WORD) (nBytesPerSample * nChannels);

				// Set the new formattype for the output pin
				hr = pCfg->SetFormat( pmt);
				UtilDeleteMediaType(pmt);
			}

			// Release interfaces
			pCfg->Release();
			pPin->Release();
		}
		// No more output pins on this filter
		else
			break;
	}

	return hr;
}

HRESULT CAudioCapture::SetInputPinProperties( IAMAudioInputMixer *pPinMixer) 
{
	HRESULT hr = 0;
	BOOL bLoudness = 0, bMono = 0;
	double dblBass = 0, dblBassRange = 0, dblMixLevel = 0, 
		dblPan = 0, dblTreble = 0, dblTrebleRange = 0;

	// Read current settings for debugging purposes.  Many of these interfaces
	// will not be implemented by the input device's driver, so they will
	// return E_NOTIMPL.  In that case, just ignore the values.
	hr = pPinMixer->get_Bass( &dblBass);
	hr = pPinMixer->get_BassRange( &dblBassRange);
	hr = pPinMixer->get_Loudness( &bLoudness);
	hr = pPinMixer->get_MixLevel( &dblMixLevel);
	hr = pPinMixer->get_Mono( &bMono);
	hr = pPinMixer->get_Pan( &dblPan);
	hr = pPinMixer->get_Treble( &dblTreble);
	hr = pPinMixer->get_TrebleRange( &dblTrebleRange);

	// Manipulate desired values here (mono/stereo, pan, etc.)

	// Since some of these methods may fail, just return success.
	// This function is for demonstration purposes only.
	return S_OK;
}

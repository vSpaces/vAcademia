#pragma once

#include <dshow.h>
#include <vector>

#include "AudioCap.h"

#define RECORD_EVENT        0
#define PLAYBACK_EVENT      1
#define WM_GRAPHNOTIFY  WM_APP + 1
#define DEFAULT_BUFFER_TIME ((float) 0.05)  /* 50 milliseconds*/
#define DEFAULT_FILENAME    TEXT("c:\\test.wav\0")

#define VARIAN_PIN_COUNT	9

static const GUID CLSID_WavDest =
{ 0x3c78b8e2, 0x6c4d, 0x11d1, { 0xad, 0xe2, 0x0, 0x0, 0xf8, 0x75, 0x4b, 0x99 } };


class CInputDevice;
class CSoundVolumeController;

class AUDIOCAP_API CAudioCapture
{
public:
	CAudioCapture( HWND hWnd);
	virtual ~CAudioCapture();

public:
	BOOL InitializeCapture();

protected:
	HRESULT GetInterfaces();
	HRESULT FillLists();
	HRESULT EnumFiltersWithMonikerToList( ICreateDevEnum *pSysDevEnum, 
										const GUID *clsid);
	HRESULT EnumFiltersAndMonikersToList( IEnumMoniker *pEnumCat);
	void AddFilterToListWithMoniker( const TCHAR *szFilterName, IMoniker *pMoniker);

	HRESULT EnumPinsOnFilter (IBaseFilter *pFilter, PIN_DIRECTION PinDir);
	
	HRESULT ActivateSelectedInputPin( TCHAR *sPinName, BOOL &bPinFinded);
	HRESULT SaveUserInputPinsSetting();
	HRESULT RestoreUserInputPinsSetting();

protected:
	void ClearFilterListWithMoniker();
	void ClearLists();
	void FreeInterfaces();
	void FreePlaybackInterfaces();

public:
	void ReleaseCapture();
	HRESULT ResetCapture();
	HRESULT RenderCaptureStream();
	HRESULT DestroyCaptureStream();
	HRESULT RenderPreviewStream();
	HRESULT DestroyPreviewStream();

protected:
	HRESULT selectInputDevice( BOOL &bInputSelected);
	HRESULT selectInputPins( BOOL &bInputPinFinded);
	HRESULT UpdateFilterLists( IGraphBuilder *pUpdateGB);

protected:
	void SetDefaults();

public:
	HRESULT HandleGraphEvent();

public:
	HRESULT GetInputDevices( LPSTR *sInputDevices);
	HRESULT SelectInputDevice( int iActiveDeviceIndex, BOOL bContinue = FALSE);
	HRESULT RestoreUserInputDevice();
	HRESULT Record();
	HRESULT Pause();
	HRESULT Stop();
	void Close();

	void SetCheckAudition( BOOL bCheckAudition);
	void SetCheckWriteFile( BOOL bCheckWriteFile);
	void SetPinName( LPCTSTR sPinName);
	void SetFileName( LPCTSTR sFileName);
	
	BOOL GetCheckAudition();
	BOOL GetCheckWriteFile();
	LPCTSTR GetPinName();
	LPCTSTR GetFileName();
protected:
	HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin);
	void UtilDeleteMediaType( AM_MEDIA_TYPE *pmt);
	HRESULT SetAudioProperties();
	HRESULT SetInputPinProperties( IAMAudioInputMixer *pPinMixer);

protected:
	HWND m_hWnd;
	BOOL m_bPausedRecording;

	IGraphBuilder *m_pGB, *m_pGBPlayback;
	IMediaControl *m_pMC, *m_pMCPlayback;
	IMediaEventEx *m_pME, *m_pMEPlayback;

	ICaptureGraphBuilder2 *m_pCapture;

	IBaseFilter *m_pInputDevice, *m_pRenderer, *m_pFileWriter,
				*m_pSplitter,    *m_pWAVDest;

	typedef std::vector<CInputDevice*> InputDevicesList;
	InputDevicesList m_inputDevicesList;

	typedef std::vector<CComString> InputPinsList;
	InputPinsList m_inputPinsList;

	int m_iActiveDeviceIndex;
	BOOL m_bCheckAudition;
	BOOL m_bCheckWriteFile;
	CComString m_sPinName;
	CComString m_sPinNameArray[ VARIAN_PIN_COUNT];
	CComString m_sFileName;

	typedef std::vector<BOOL> UserSettingInputPinsSaver;
	UserSettingInputPinsSaver m_userSettingInputPinsSaver;

	typedef IAMAudioInputMixer* PAMAudioInputMixer;
	typedef IPin* PPin;

	CSoundVolumeController *m_pSoundVolumeController;

	double m_dMixLevel;
};

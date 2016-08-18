#pragma once

//#include "CaptureVideo.h"
#include ".\rtbasedevice.h"

class RTTVTunerDevice: public CRTBaseDevice
{
public:
	RTTVTunerDevice( unsigned int auTunerIndex);
	~RTTVTunerDevice(void);

	typedef unsigned int	TUNER_MODE;

	static const int DEVICE_COMPOSITE;
	static const int DEVICE_SVIDEO;
	static const int DEVICE_TVTUNER;

	typedef MP_VECTOR<TUNER_MODE>		vecTunerModes;
	typedef vecTunerModes::iterator		vecTunerModesIt;

public:
	// Создает диалоговое окно с параметрами устройства
	HWND	CreateDialogPanel( HWND hParent);

	// реализация ivideodevice
public:
	HBITMAP	GetSnapshotFrame(bool isFirstFrame);
	// устанавливает размер изображения
	bool	SetFrameSize( unsigned int auWidth, unsigned int auHeight)
	{
		return false;
	}
	vecResolutions&	GetSupportedResolutions()
	{
		resolutions.clear();
		return resolutions;
	}

public:
	BOOL	SetVideoMode( TUNER_MODE auMode);
	LPCTSTR GetVideoModeName( TUNER_MODE auMode);
	vecTunerModes&	GetSupportedVideoModes()
	{
		return supportedModes;
	}

	int PreparationForStart();
	void UpdateCameraIndex(unsigned int auCameraIndex);

	virtual int	StartImpl(HWND ahWnd);
	virtual void	StopImpl();

private:
	BOOL	StartSelectedTunerMode();
	//void	EnumerateResolutions();

private:
//	CSampleDeviceDialog	dialog;
	CCaptureVideo		videoCap;
	unsigned int		m_uiTunerIndex;
	unsigned int		m_uiCurrentMode;

	vecResolutions		resolutions;
	vecTunerModes		supportedModes;
};
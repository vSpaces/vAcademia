
#pragma once

#define MAX_CRASH_COUNT			5
#define MAX_CRASH_TIME			20000

#define MIN_TIME_FOR_RESTART	2000

#include "rm.h"
#include "oms_context.h"

class CVideoCaptureSupport : public rm::IVideoCaptureSupport
{
public:
	CVideoCaptureSupport(omsContext* context);
	~CVideoCaptureSupport();

	bool StartCapture(unsigned int eventID);
	bool EndCapture();

	bool GetPictureData(void** data, unsigned int& size, unsigned int& width, unsigned int& height);
	void FreePictureData(void* data);

	void SetVideoSize(int width, int height);

	void Update();

	char* GetVideoURL();

	bool CanVideoBeStarted();

	void OnScreenSizeChanged();
	    
private:
	void PrepareConnectionString();
	bool LoadAndCheckSettings(bool isGenerateError = true);
	void LoadSettings();
	void CheckFFMpeg();
	void RestartCapture();

	void PatchTextureIfNeeded();
	void PrepareScreenString();

	std::wstring GetStringWithVideoFPS();
	int GetVideoFPS();

	int GetDirectShowFilterError(bool isGenerateError = true);

	void CreateSignalFile();
	void DeleteSignalFile();

	std::wstring GetSignalFilePath();

	enum DeviceType
	{
		DEVICE_TYPE_VIDEO = 0,
		DEVICE_TYPE_AUDIO = 1,
	};
	int GetDirectShowFilterErrorByName(const std::string& filterName, DeviceType deviceType, bool isGenerateError);

	std::wstring GetCapturingFFMpegExecuteString();
	std::wstring GetStreamingFFMpegExecuteString();
	std::wstring GetPipeForwarderExecuteString();

	bool IsFrequentCrashSequence();
	void RegisterCrash();

	void SetAppropriateProcessesAffinity();
	
	void CreatePBOIfNeeded();
	bool IsPBOEnabled();

	void UpdateSizes();

	void CloseProcess(HANDLE* hndl);
	void TerminateProcesses();

	HANDLE Execute(std::wstring execString);

	void CheckProcess(HANDLE process);

	CRITICAL_SECTION m_cs;
	HANDLE m_capturingFFMpegProcess;
	HANDLE m_streamingFFMpegProcess;
	HANDLE m_pipeForwarderProcess;
	bool m_isWorkingNow;
	bool m_isOk;

	unsigned char* m_data;
	unsigned int m_dataSize;
	unsigned int m_width;
	unsigned int m_height;

	unsigned int m_textureWidth;
	unsigned int m_textureHeight;
	unsigned char m_channelCount;

	__int64 m_lastFrameTime;

	MP_STRING m_serverConnectionStringTemplate;
	MP_STRING m_serverConnectionString;
	MP_STRING m_screenString;
	MP_STRING m_pipeForwardExecuteString;
	MP_STRING m_streamingExecuteTemplate;
	MP_STRING m_capturingExecuteTemplate;

	MP_WSTRING m_videoPath;

	HANDLE m_mutex;

	unsigned int m_captureWidth;
	unsigned int m_captureHeight;

	unsigned int m_eventID;

	bool m_isPerformanceChecked;
	unsigned int m_startFPS;
	__int64 m_startTime;

	omsContext* m_context;

	__int64 m_crashesTime[MAX_CRASH_COUNT];

	CPBO* m_pbo;
	unsigned int m_pboSize;
	bool m_isPBODisabled;

	unsigned int m_maxTimeBetweenFrames;

	bool m_needToShowConsole;	
	bool m_needToRestart;
	bool m_notRunReally;
	unsigned int m_needToRestartTime;
	unsigned int m_runID;
};
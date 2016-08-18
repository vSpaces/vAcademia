
#include "StdAfx.h"
#include "IniFile.h"
#include "FileFunctions.h"
#include "StringFunctions.h"
#include "PlatformDependent.h"
#include "DirectShowFunctions.h"
#include "VideoCaptureSupport.h"
#include "PBO.h"
#include "XMLPropertiesList.h"

#define EXECUTE_INTERVAL		500

#define MAX_NORMAL_PROGRAM_FPS	50
#define MAX_VIDEO_FPS			15

#define MIN_NORMAL_PROGRAM_FPS	20
#define MIN_VIDEO_FPS			3

#define	GOOD_FPS				30

#define MAX_PBO_TIME			200000

#define HALF_CHECKING_PERIOD	30000
#define CHECKING_PERIOD			(HALF_CHECKING_PERIOD * 2)

#define FILTERS_OK						0
#define FILTER_NOT_EXISTS			    1
#define FILTER_NOT_REGISTERED		    2
#define VIDEO_FILTER_ERROR_MASK			0x10
#define AUDIO_FILTER_ERROR_MASK			0x20
#define VIDEO_FILTER_NOT_EXISTS			(VIDEO_FILTER_ERROR_MASK | FILTER_NOT_EXISTS)
#define VIDEO_FILTER_NOT_REGISTERED		(VIDEO_FILTER_ERROR_MASK | FILTER_NOT_REGISTERED)
#define AUDIO_FILTER_NOT_EXISTS			(AUDIO_FILTER_ERROR_MASK | FILTER_NOT_EXISTS)
#define AUDIO_FILTER_NOT_REGISTERED		(AUDIO_FILTER_ERROR_MASK | FILTER_NOT_REGISTERED)
#define FILTER_TYPE_ERROR_MASK			0xF0

#define VIDEO_FILTER_NAME				"vAcademiaCapture"
#define AUDIO_FILTER_NAME				"vAcademiaAudioCapture"
#define	FFMPEG_NAME						L"ffmpeg.exe"

CVideoCaptureSupport::CVideoCaptureSupport(omsContext* context):
	MP_STRING_INIT(m_serverConnectionStringTemplate),
	MP_STRING_INIT(m_serverConnectionString),
	MP_STRING_INIT(m_screenString),
	MP_STRING_INIT(m_pipeForwardExecuteString),
	MP_STRING_INIT(m_streamingExecuteTemplate),
	MP_STRING_INIT(m_capturingExecuteTemplate),
	MP_WSTRING_INIT(m_videoPath),
	m_capturingFFMpegProcess(NULL),		
	m_streamingFFMpegProcess(NULL),		
	m_pipeForwarderProcess(NULL),
	m_isPerformanceChecked(false),	
	m_maxTimeBetweenFrames(100),	
	m_needToShowConsole(false),
	m_needToRestart(false),
	m_needToRestartTime(0),
	m_notRunReally(false),
	m_isPBODisabled(true),
	m_isWorkingNow(false),			
	m_lastFrameTime(0),
	m_context(context),
	m_captureHeight(0),	
	m_captureWidth(0),
	m_startTime(0),
	m_mutex(NULL),
	m_dataSize(0),
	m_isOk(false),
	m_startFPS(0),
	m_data(NULL),
	m_pboSize(0),		
	m_eventID(0),
	m_pbo(NULL),
	m_height(0),
	m_width(0),
	m_runID(0)
{	
	InitializeCriticalSection(&m_cs);

	for (int i = 0; i < MAX_CRASH_COUNT; i++)
	{
		m_crashesTime[i] = 0;
	}

	DeleteSignalFile();
}

void CVideoCaptureSupport::PrepareConnectionString()
{
	m_serverConnectionString = m_serverConnectionStringTemplate;
	StringReplace(m_serverConnectionString, "{EVENT_ID}", IntToStr(m_eventID));
}

int CVideoCaptureSupport::GetDirectShowFilterError(bool isGenerateError)
{
	int res = GetDirectShowFilterErrorByName(VIDEO_FILTER_NAME, DEVICE_TYPE_VIDEO, isGenerateError);
	if (res != FILTERS_OK){
		return (VIDEO_FILTER_ERROR_MASK | res);
	}

	res = GetDirectShowFilterErrorByName(AUDIO_FILTER_NAME, DEVICE_TYPE_AUDIO, isGenerateError);
	if (res != FILTERS_OK){
		return (AUDIO_FILTER_ERROR_MASK | res);
	}
	return res;
}

int CVideoCaptureSupport::GetDirectShowFilterErrorByName(const std::string& filterName, DeviceType deviceType, bool isGenerateError)
{
	std::wstring pathToFilter = GetApplicationRootDirectory();
	USES_CONVERSION;
	pathToFilter += A2W(filterName.c_str());
	pathToFilter += FILTER_EXTENSION;

	if (!IsFileExists(pathToFilter))
	{
		if (isGenerateError)
		{
			TryToRegisterDirectShowFilterSilently(filterName);
		}

		if (!IsFileExists(pathToFilter))
		{
			return FILTER_NOT_EXISTS;
		}
	}	

	if(deviceType == DEVICE_TYPE_AUDIO)
		g->lw.WriteLn("Audio filter exists");
	else
		g->lw.WriteLn("Video filter exists");
	DSDeviceType dsDeviceType = DS_DEVICE_TYPE_VIDEO;
	if(deviceType == DEVICE_TYPE_AUDIO)
		dsDeviceType = DS_DEVICE_TYPE_AUDIO;	

	if (!IsDeviceRegistered(filterName, dsDeviceType))
	{
		if (isGenerateError)
		{
			TryToRegisterDirectShowFilterSilently(filterName);
		}

		if (!IsDeviceRegistered(filterName, dsDeviceType))
		{
			return FILTER_NOT_REGISTERED;
		}
	}
		
	if(deviceType == DEVICE_TYPE_AUDIO)
		g->lw.WriteLn("Audio filter registered");
	else
		g->lw.WriteLn("Video filter registered");

	return FILTERS_OK;
}

bool CVideoCaptureSupport::LoadAndCheckSettings(bool isGenerateError)
{
	LoadSettings();
	PrepareConnectionString();

	if (!m_isOk)
	{
		g->lw.WriteLn("Video capture error: invalid settings, server connection string template not found.");
		if (isGenerateError)
		{
			m_context->mpSM->OnVideoCaptureError(VIDEO_CAPTURE_SETTINGS_ERROR);
		}
		return false;
	}

	int dsError = GetDirectShowFilterError(isGenerateError);

	int iVideoCaptureError = -1;
	switch (dsError)
	{
	case VIDEO_FILTER_NOT_EXISTS:
		iVideoCaptureError = VIDEO_CAPTURE_VIDEO_FILTER_NOT_EXISTS_ERROR;
		break;
	case VIDEO_FILTER_NOT_REGISTERED:
		iVideoCaptureError = VIDEO_CAPTURE_VIDEO_FILTER_NOT_REGISTERED_ERROR;
		break;
	case AUDIO_FILTER_NOT_EXISTS:
		iVideoCaptureError = VIDEO_CAPTURE_AUDIO_FILTER_NOT_EXISTS_ERROR;
		break;
	case AUDIO_FILTER_NOT_REGISTERED:
		iVideoCaptureError = VIDEO_CAPTURE_AUDIO_FILTER_NOT_REGISTERED_ERROR;
		break;
	}

	if (dsError != FILTERS_OK)
	{
		m_isOk = false;
	}

	if(iVideoCaptureError >= 0)
	{
		if (isGenerateError)
		{
			m_context->mpSM->OnVideoCaptureError(iVideoCaptureError);
		}
		return false;
	}

	std::wstring pathToFFMpeg = GetApplicationRootDirectory();
	pathToFFMpeg += FFMPEG_NAME;
	
	if (!IsFileExists(pathToFFMpeg))
	{
		if (isGenerateError)
		{
			m_context->mpSM->OnVideoCaptureError(VIDEO_CAPTURE_FFMPEG_NOT_FOUND_ERROR);
		}
		m_isOk = false;
		return false;
	}	

	return true;
}

void CVideoCaptureSupport::SetAppropriateProcessesAffinity()
{
	int coreCount = g->ci.GetProcessorCount();
	if (coreCount > 1)
	{
		int threadMask = 1;
		for (unsigned int k = 0; k < coreCount; k++)
		{
			threadMask *= 2;
		}
		threadMask -= 2;
		SetProcessAffinityMask(m_capturingFFMpegProcess, threadMask);
		SetProcessAffinityMask(m_streamingFFMpegProcess, threadMask);
		SetProcessAffinityMask(m_pipeForwarderProcess, threadMask);
	}
}

std::wstring CVideoCaptureSupport::GetSignalFilePath()
{
	std::wstring path = GetApplicationDataDirectory();
	path += L"\\start_video_capture";

	return path;
}

void CVideoCaptureSupport::CreateSignalFile()
{
	std::wstring path = GetSignalFilePath();

	FILE* fl = _wfopen(path.c_str(), L"wb");	
	if (fl)
	{		
		fclose(fl);
	}

	m_mutex = CreateMutex(NULL, false, "Local\\videoStreamingStarted");
}

void CVideoCaptureSupport::DeleteSignalFile()
{
	std::wstring path = GetSignalFilePath();

	_wremove(path.c_str());
}

HANDLE CVideoCaptureSupport::Execute(std::wstring execString)
{
	STARTUPINFOW startupInfo; 
	PROCESS_INFORMATION processInfo; 
	memset(&startupInfo, 0, sizeof(startupInfo)); 
	memset(&processInfo, 0, sizeof(processInfo)); 
	startupInfo.cb = sizeof(startupInfo);
	
	if (CreateProcessW(NULL, (LPWSTR)execString.c_str(), NULL, NULL, TRUE, m_needToShowConsole ? CREATE_NEW_CONSOLE : CREATE_NO_WINDOW, NULL, 
			GetApplicationRootDirectory().c_str(), &startupInfo, &processInfo))
	{
		CloseHandle(processInfo.hThread);
		return processInfo.hProcess;
	}
	else
	{
		return NULL;
	}
}

bool CVideoCaptureSupport::StartCapture(unsigned int eventID)
{
	if (m_needToRestart == false)
	{
		m_videoPath = L"";

		if (-1 == eventID)
		{	
			wchar_t pwcPath[3000];
			if (m_context->mpApp->GetLocalFilePath(L"*.avi", L"", false, pwcPath, 2995, L"video.avi"))
			{	
				m_videoPath = pwcPath;
				m_videoPath = L"\"" + m_videoPath + L"\"";
				g->lw.WriteLn("video path ", m_videoPath);
				if (m_videoPath.size() == 0)
				{
					g->lw.WriteLn("no video");
					return false;
				}
			}
			else
			{
				g->lw.WriteLn("no video");
				return false;
			}
		}
	}

	
	if (0 == eventID)
	{
		m_context->mpSM->OnVideoCaptureError(VIDEO_CAPTURE_NO_EVENT_ID_ERROR);
		return false;
	}

	m_runID++;

	CreateSignalFile();	

	m_startFPS = g->ne.GetAverageFPS();
 
	m_eventID = eventID;

	LoadAndCheckSettings();
		
	if ((!m_isWorkingNow) && (m_isOk))
	{
		std::wstring pipeString = GetPipeForwarderExecuteString();
		g->lw.WriteLn(L"Start pipe forwarding with string: ", pipeString);

		std::wstring ffMpegStringCapture = GetCapturingFFMpegExecuteString();
		g->lw.WriteLn(L"Start video capture with string: ", ffMpegStringCapture);

		std::wstring ffMpegStringStreaming = GetStreamingFFMpegExecuteString();
		g->lw.WriteLn(L"Start video streaming with string: ", ffMpegStringStreaming);

		if (!m_notRunReally)
		{
			m_pipeForwarderProcess = Execute(pipeString);
			if (m_pipeForwarderProcess)
			{
				m_capturingFFMpegProcess = Execute(ffMpegStringCapture);
				if (m_capturingFFMpegProcess)
				{				
					m_streamingFFMpegProcess = Execute(ffMpegStringStreaming);
				}
			}

			if ((m_pipeForwarderProcess) && (m_capturingFFMpegProcess) && (m_streamingFFMpegProcess))
			{
				//SetAppropriateProcessesAffinity();
			}
			else
			{
				m_context->mpSM->OnVideoCaptureError(VIDEO_CAPTURE_FFMPEG_NOT_EXECUTED_ERROR);
				TerminateProcesses();
				DeleteSignalFile();
				return false;
			}
		}

		m_isWorkingNow = true;
		m_isPerformanceChecked = false;
		m_startTime = g->tp.GetTickCount() + HALF_CHECKING_PERIOD;		

		DeleteSignalFile();
		return true;
	}
	else
	{
		DeleteSignalFile();
		return false;
	}
}

void CVideoCaptureSupport::CloseProcess(HANDLE* hndl)
{
	if ((hndl) && (*hndl != NULL))
	{
		rtl_TerminateOtherProcess(*hndl, 0);
		CloseHandle(*hndl);
		*hndl = NULL; 
	}
}

void CVideoCaptureSupport::TerminateProcesses()
{
	CloseProcess(&m_capturingFFMpegProcess);
	CloseProcess(&m_streamingFFMpegProcess);
	CloseProcess(&m_pipeForwarderProcess);
}

bool CVideoCaptureSupport::EndCapture()
{
	if ((m_isWorkingNow) && (m_isOk))
	{
		g->lw.WriteLn("End video capture");

		if (m_mutex != NULL)
		{
			CloseHandle(m_mutex);
		}

		m_isWorkingNow = false; 

		TerminateProcesses();

		return true;
	}
	else
	{
		return false;

	}
}

void CVideoCaptureSupport::RegisterCrash()
{
	for (int i = 1; i < MAX_CRASH_COUNT; i++)
	{
		m_crashesTime[i - 1] = m_crashesTime[i];
	}

	m_crashesTime[MAX_CRASH_COUNT - 1] = g->ne.time;
}

bool CVideoCaptureSupport::IsFrequentCrashSequence()
{
	for (int i = 0; i < MAX_CRASH_COUNT; i++)
	if (0 == m_crashesTime[i])
	{
		return false;
	}

	if (m_crashesTime[MAX_CRASH_COUNT - 1] - m_crashesTime[0] > MAX_CRASH_TIME)
	{
		return false;
	}

	return true;
}

void CVideoCaptureSupport::CheckProcess(HANDLE process)
{
	unsigned int exitCode = STILL_ACTIVE;	
	if (GetExitCodeProcess(process, (LPDWORD)&exitCode))
	{
		if (exitCode != STILL_ACTIVE)
		{						
			RegisterCrash();

			if (IsFrequentCrashSequence())
			{
				EndCapture();
				if (m_context)
				{
					m_context->mpSM->OnVideoCaptureError(VIDEO_CAPTURE_UNKNOWN_ERROR);
				}
			}
			else
			{
				m_needToRestart = true;
				m_needToRestartTime = g->ne.time;				
			}
		}		
	}	
}

void CVideoCaptureSupport::CheckFFMpeg()
{
	if (m_notRunReally)
	{
		return;
	}

	if ((m_isWorkingNow) && (m_needToRestart))
	{
		if (g->ne.time - m_needToRestartTime > MIN_TIME_FOR_RESTART)
		{
			g->lw.WriteLn("Try to restart FFMpeg");
			RestartCapture();
			m_needToRestart = false;
		}
		else
		{
			return;
		}
	}

	CheckProcess(m_capturingFFMpegProcess);
	CheckProcess(m_streamingFFMpegProcess);
	CheckProcess(m_pipeForwarderProcess);
}

void CVideoCaptureSupport::RestartCapture()
{
	EndCapture();
	StartCapture(m_eventID);
}

void CVideoCaptureSupport::Update()
{
	if (m_isWorkingNow)
	{
		if (g->ne.time - m_lastFrameTime < m_maxTimeBetweenFrames)
		{
			return;
		}

		if (IsIconic(g->ne.ghWnd))
		{
			return;
		}

		// прошло более минуты с начала видеотрансл€ции
		if (m_videoPath.size() == 0)
		if (!m_isPerformanceChecked)
		if (g->tp.GetTickCount() - m_startTime > CHECKING_PERIOD)
		{
			// фпс упал в 2 раза или больше
			if (g->ne.GetAverageFPS() <= m_startFPS / 2)
			if (g->ne.GetAverageFPS() < GOOD_FPS)
			{
				m_context->mpSM->OnVideoCaptureError(VIDEO_CAPTURE_PERFORMANCE_IS_LOW);
			}

			m_isPerformanceChecked = true;
		}		

		CheckFFMpeg();		

		unsigned char* data = NULL;
		CSprite::GetCurrentScreen(data, m_textureWidth, m_textureHeight, m_channelCount);			
		
		EnterCriticalSection(&m_cs);

		if (m_data)
		{
			MP_DELETE_ARR(m_data);
		}				
		
		m_data = data;	
		if (m_data)
		if ((m_data[0] == 0) && (m_data[1] == 0) && (m_data[2] == 0))			
		{
			g->lw.WriteLn("[videocapture] blackscreen error!!!");
		}					

		LeaveCriticalSection(&m_cs);

		UpdateSizes();

		m_lastFrameTime = g->ne.time;
	}
}

void CVideoCaptureSupport::PatchTextureIfNeeded()
{
	if (g->gi.GetVendorID() != VENDOR_INTEL)
	{
		return;
	}

	int channelCount = m_channelCount;
	int size = m_textureWidth * m_textureHeight * channelCount;
	unsigned char tmp;
	for (int i = 0; i < size; i += channelCount)
	{
		tmp = m_data[i + 2];
		m_data[i + 2] = m_data[i];
		m_data[i] = tmp;
	}
}

void CVideoCaptureSupport::UpdateSizes()
{
	EnterCriticalSection(&m_cs);	
	m_width = m_textureWidth;
	m_height = m_textureHeight;
	int oldDataSize = m_dataSize;
	m_dataSize = m_channelCount * m_width * m_height;				
	if (oldDataSize != m_dataSize)
	{
		if (m_data)
		{
			MP_DELETE_ARR(m_data);
		}	
	}
	LeaveCriticalSection(&m_cs);
}

bool CVideoCaptureSupport::IsPBOEnabled()
{
	return false;
}

void CVideoCaptureSupport::SetVideoSize(int width, int height)
{
	m_captureWidth = width;
	m_captureHeight = height;

	PrepareScreenString();
}

void CVideoCaptureSupport::CreatePBOIfNeeded()
{			
}


bool CVideoCaptureSupport::GetPictureData(void** data, unsigned int& size, unsigned int& width,
										  unsigned int& height)
{
	EnterCriticalSection(&m_cs);

	width = m_width;
	height = m_height;

	if ((!m_data) || (0 == m_dataSize))
	{
		*data = NULL;
		size = 0;
		
		LeaveCriticalSection(&m_cs);
		return false;
	}

	*data = m_data;
	size = m_dataSize;	

	if (!IsPBOEnabled())
	{
		m_data = NULL;
	}

	LeaveCriticalSection(&m_cs);

	return true;
}

void CVideoCaptureSupport::FreePictureData(void* data)
{
	if (!IsPBOEnabled())
	if (data)
	{
		MP_DELETE_ARR(data);
	}
}

void CVideoCaptureSupport::LoadSettings()
{
	if (!m_isOk)
	{
		CXMLPropertiesList list(L"videocapture_settings.xml", ERROR_IF_NOT_FOUND);
		if (!list.IsLoaded())
		{
			m_isOk = false;
			return;
		}
		
		m_serverConnectionStringTemplate = list.GetString("server_template");
		m_pipeForwardExecuteString = list.GetString("pipe_forward_execute");
		m_streamingExecuteTemplate = list.GetString("streaming_execute_template");
		m_capturingExecuteTemplate = list.GetString("capturing_execute_template");
		if (0 == m_captureWidth)
		{
			m_captureWidth = rtl_atoi(list.GetString("width").c_str());
		}
		if (0 == m_captureHeight)
		{
			m_captureHeight = rtl_atoi(list.GetString("height").c_str());
		}
		m_needToShowConsole = (list.GetString("console") == "true");		
		m_notRunReally = (list.GetString("not_run_really") == "true");
				
		m_isOk = (m_serverConnectionStringTemplate.size() > 0);		
	}
}

void CVideoCaptureSupport::OnScreenSizeChanged()
{
	if (m_isWorkingNow)
	{
		RestartCapture();
	}
}

void CVideoCaptureSupport::PrepareScreenString()
{	
	float koef1 = (float)m_captureWidth / (float)g->stp.GetCurrentWidth();
	float koef2 = (float)m_captureHeight / (float)g->stp.GetCurrentHeight();
	float koef = (koef1 < koef2) ? koef1 : koef2;

	int width = (int)(g->stp.GetCurrentWidth() * koef);
	int height = (int)(g->stp.GetCurrentHeight() * koef);
	
	if (m_videoPath != L"")
	{
		RECT rct;		
		GetClientRect(g->ne.ghWnd, &rct);
		width = rct.right - rct.left;
		height = rct.bottom - rct.top;
	}

	if (width%2 != 0)
	{
		width++;
	}
	if (height%2 != 0)
	{
		height++;
	}	

	g->lw.WriteLn("Size of video capture image: ", width, "x", height);
	
	m_screenString = IntToStr(width);
	m_screenString += "x";
	m_screenString += IntToStr(height);	
}

int CVideoCaptureSupport::GetVideoFPS()
{
	int avgFPS = g->ne.GetAverageFPS();

	if (g->ci.GetProcessorCount() == 1)
	{
		return MIN_VIDEO_FPS;
	}

	if (avgFPS > MAX_NORMAL_PROGRAM_FPS)
	{
		return MAX_VIDEO_FPS;
	}
	else if (avgFPS < MIN_NORMAL_PROGRAM_FPS)
	{
		return MIN_VIDEO_FPS;
	}
	else
	{
		float koef = (float)(avgFPS - MIN_NORMAL_PROGRAM_FPS) / (float)(MAX_NORMAL_PROGRAM_FPS - MIN_NORMAL_PROGRAM_FPS);
		return (int)(MIN_VIDEO_FPS + (MAX_VIDEO_FPS - MIN_VIDEO_FPS) * koef);
	}
}

std::wstring CVideoCaptureSupport::GetStringWithVideoFPS()
{
	int videoFPS = GetVideoFPS();
	m_maxTimeBetweenFrames = 1000 / videoFPS;
	return IntToWStr(videoFPS);
}

std::wstring CVideoCaptureSupport::GetCapturingFFMpegExecuteString()
{
	PrepareScreenString();

	std::wstring execString = GetApplicationRootDirectory();
	USES_CONVERSION;
	execString += A2W( m_capturingExecuteTemplate.c_str());

	StringReplaceW(execString, L"{DEBUG}", m_needToShowConsole ? L" -loglevel debug -report " : L"");
	StringReplaceW(execString, L"{SIZE}", A2W( m_screenString.c_str()));
	StringReplaceW(execString, L"{FPS}", GetStringWithVideoFPS());
	StringReplaceW(execString, L"{NUM}", IntToWStr(m_runID));
	StringReplaceW(execString, L"{QUALITY}", (m_videoPath.size() == 0) ? L"1000k" : L"10000k");
	StringReplaceW(execString, L"'", L"\"");

	return execString;
}

std::wstring CVideoCaptureSupport::GetStreamingFFMpegExecuteString()
{
	PrepareScreenString();

	std::wstring execString = GetApplicationRootDirectory();
	USES_CONVERSION;
	execString += A2W( m_streamingExecuteTemplate.c_str());

	StringReplaceW(execString, L"{FORMAT}", (m_videoPath.size() == 0) ? L"flv" : L"mpegts");
	StringReplaceW(execString, L"{SERVER}", (m_videoPath.size() == 0) ? A2W( m_serverConnectionString.c_str()) : m_videoPath);
	StringReplaceW(execString, L"{NUM}", IntToWStr(m_runID));

	return execString;
}

std::wstring CVideoCaptureSupport::GetPipeForwarderExecuteString()
{
	PrepareScreenString();

	std::wstring execString = GetApplicationRootDirectory();
	USES_CONVERSION;
	execString += A2W( m_pipeForwardExecuteString.c_str());

	StringReplaceW(execString, L"{NUM}", IntToWStr(m_runID));

	return execString;
}

char* CVideoCaptureSupport::GetVideoURL()
{
	return (char *)m_serverConnectionString.c_str();
}

bool CVideoCaptureSupport::CanVideoBeStarted()
{
	return LoadAndCheckSettings(false);
}

CVideoCaptureSupport::~CVideoCaptureSupport()
{
	DeleteCriticalSection(&m_cs);	
}
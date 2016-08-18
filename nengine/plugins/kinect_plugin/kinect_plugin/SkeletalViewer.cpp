#include "stdafx.h"
#include "TimeMeasurer.h"
#include "SimpleLogWriter.h"
#include "../../../ncommon/TimeProcessor.h"
#include "SkeletalViewer.h"
#include "NuiSkeleton.h"
#include <math.h>
#include <vector>
#include <nuisensor.h>

std::vector<int> headBones;
std::vector<int> leftFootBones;
std::vector<int> rightFootBones;
std::vector<int> leftArmBones;
std::vector<int> rightArmBones;
std::vector<int> hipBones;
std::vector<int> hipsBones;

#define COLOR_THRESHOLD				75

CTimeProcessor* g_timeProcessor = NULL;
CSimpleLogWriter* g_log = NULL;

CSkeletalViewerApp::CSkeletalViewerApp():
    m_hNextSkeletonEvent(NULL),
    m_hThNuiProcess(NULL),
	m_hThNuiProcess2(NULL),
	m_hThNuiProcess3(NULL),
    m_hEvNuiProcessStop(NULL),
	m_hEvFTProcessStop(NULL),
	m_isFoundSkeleton(false),
	m_isFaceTracking(true),
	m_isSkeletonTracking(false),
	m_pitch(0),
	m_yaw(0),
	m_roll(0),
	m_isSeated(false),
	m_samePositionCount(0),
	m_reportedPitchAverage(0),
	m_reportedRollAverage(0),
	m_reportedYawAverage(0),
	m_isFacingUser(false),
	m_isTwoSkeletonsFound(false),
	m_globalRotationX(0),
	m_globalRotationY(0),
	m_globalRotationZ(0),
	m_globalRotationW(0),
	m_videoFrames(10),
	m_depthTimeStamp(UINT_MAX / 2),
	m_AUcount(0),
	m_isFaceTrackingSuccessful(false),
	m_otherColorKoef1Arr(NULL),
	m_otherColorKoef2Arr(NULL)
{
	m_skeleton.x = 0;
	m_skeleton.y = 0;
	m_skeleton.z = 0;

	m_otherColorKoef1 = 6;
	m_otherColorKoef2 = 5;
	m_mainColorKoef = 10;

	m_qualityFlags = 0;

	memset(m_coords, 0, sizeof(m_coords));
	memset(m_exportCoords, 0, sizeof(m_exportCoords));
	
	headBones.push_back(NUI_SKELETON_POSITION_SHOULDER_CENTER);
	headBones.push_back(NUI_SKELETON_POSITION_HEAD);
	
	leftFootBones.push_back(NUI_SKELETON_POSITION_HIP_LEFT);
	leftFootBones.push_back(NUI_SKELETON_POSITION_KNEE_LEFT);
	leftFootBones.push_back(NUI_SKELETON_POSITION_ANKLE_LEFT);
	leftFootBones.push_back(NUI_SKELETON_POSITION_FOOT_LEFT);
			
	rightFootBones.push_back(NUI_SKELETON_POSITION_HIP_RIGHT);
	rightFootBones.push_back(NUI_SKELETON_POSITION_KNEE_RIGHT);
	rightFootBones.push_back(NUI_SKELETON_POSITION_ANKLE_RIGHT);
	rightFootBones.push_back(NUI_SKELETON_POSITION_FOOT_RIGHT);
			
	leftArmBones.push_back(NUI_SKELETON_POSITION_SHOULDER_CENTER);
	leftArmBones.push_back(NUI_SKELETON_POSITION_SHOULDER_LEFT);
	leftArmBones.push_back(NUI_SKELETON_POSITION_ELBOW_LEFT);
	leftArmBones.push_back(NUI_SKELETON_POSITION_WRIST_LEFT);
	leftArmBones.push_back(NUI_SKELETON_POSITION_HAND_LEFT);
			
	rightArmBones.push_back(NUI_SKELETON_POSITION_SHOULDER_CENTER);
	rightArmBones.push_back(NUI_SKELETON_POSITION_SHOULDER_RIGHT);
	rightArmBones.push_back(NUI_SKELETON_POSITION_ELBOW_RIGHT);
	rightArmBones.push_back(NUI_SKELETON_POSITION_WRIST_RIGHT);
	rightArmBones.push_back(NUI_SKELETON_POSITION_HAND_RIGHT);
		
	hipBones.push_back(NUI_SKELETON_POSITION_HIP_CENTER);

	hipsBones.push_back(NUI_SKELETON_POSITION_HIP_CENTER);
	hipsBones.push_back(NUI_SKELETON_POSITION_HIP_RIGHT);
	hipsBones.push_back(NUI_SKELETON_POSITION_HIP_LEFT);
	hipsBones.push_back(NUI_SKELETON_POSITION_SHOULDER_CENTER);
	hipsBones.push_back(NUI_SKELETON_POSITION_SHOULDER_LEFT);
	hipsBones.push_back(NUI_SKELETON_POSITION_SHOULDER_RIGHT);

	m_hNextColorFrameEvent = NULL;
	m_pVideoStreamHandle = NULL;
	m_hNextDepthFrameEvent = NULL;
	m_pDepthStreamHandle = NULL;
	m_isVideoEnabled = false;
	m_videoData = NULL;
	m_videoDataSize = 0;
	m_isVideoFrameOld = true;
	m_colorTimeStamp = NULL;

	m_depthData = NULL;
	m_depthDataSize = 0;

	m_mappedDepthData = NULL;

	m_depthPixels = NULL;
	m_depthPoints = NULL;
	m_depthPixelsSize = 0;

	memset(m_rotation, 0, sizeof(m_rotation));
	memset(m_translations, 0, sizeof(m_translations));
	memset(m_AUcoeffs, 0, sizeof(m_AUcoeffs));

	m_playerIndex = 0;

	m_channelID = 2;
	m_otherChannelID1 = 0;
	m_otherChannelID2 = 1;

	m_specificPixelsCount = 0;

	m_debugMode = DEBUG_MODE_NO_DEBUG;

	g_timeProcessor = new CTimeProcessor();
	g_timeProcessor->Init();
	g_timeProcessor->Start();
	g_log = new CSimpleLogWriter();
}

void CSkeletalViewerApp::SetDebugMode(unsigned int debugMode)
{
	m_debugMode = debugMode;
}

int CSkeletalViewerApp::GetPlayerIndex()
{
	return m_playerIndex;
}

void CSkeletalViewerApp::GetGlobalRotation(float& rotX, float& rotY, float& rotZ, float& rotW)
{
	EnterCriticalSection(&m_cs);
	rotX = m_globalRotationX;
	rotY = m_globalRotationY;
	rotZ = m_globalRotationZ;
	rotW = m_globalRotationW;
	LeaveCriticalSection(&m_cs);
}

bool CSkeletalViewerApp::SetSeatedMode(bool isSeated)
{
	m_isSeated = isSeated;
	EnterCriticalSection(&m_cs);
	HRESULT hr = NuiSkeletonTrackingEnable( m_hNextSkeletonEvent, m_isSeated ? NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT : 0);
	LeaveCriticalSection(&m_cs);
	if( FAILED( hr ) )
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CSkeletalViewerApp::SetTrackingParams(bool isSkeletonTracking, bool isFaceTracking)
{
	m_isSkeletonTracking = isSkeletonTracking;
	m_isFaceTracking = isFaceTracking;
}

bool CSkeletalViewerApp::GetHeadPosition(float& pitch, float& yaw, float& roll)
{
	if (!m_isFaceTracking)
	{
		return false;
	}

	m_pitch = pitch;
	m_yaw = yaw;
	m_roll = roll;

	return true;
}

bool CSkeletalViewerApp::Init()
{
	HRESULT res = Nui_Init();
	if( FAILED( res ) )
    {
		return false;
	}
	else
	{
		NuiCameraElevationSetAngle(0);
		return true;
	}
}

void CSkeletalViewerApp::UnInit()
{
	Nui_UnInit();
}

void CSkeletalViewerApp::SetRotations(const float pitchDegrees, const float yawDegrees, const float rollDegrees)
{
    if (true)
    {
        float smoothingFactor = 1.0f;
        m_samePositionCount++;
        smoothingFactor /= m_samePositionCount;
        smoothingFactor = max(smoothingFactor, 0.002f);

        m_reportedPitchAverage += smoothingFactor*(pitchDegrees-m_reportedPitchAverage);
        m_reportedYawAverage += smoothingFactor*(-yawDegrees-m_reportedYawAverage);
        m_reportedRollAverage += smoothingFactor*(rollDegrees-m_reportedRollAverage);
    }

    m_pitch = (pitchDegrees-m_reportedPitchAverage)/180.0f;
    m_yaw = (-yawDegrees-m_reportedYawAverage)/180.0f;
    m_roll = (rollDegrees-m_reportedRollAverage)/180.0f;
    m_isFacingUser = (fabsf(m_pitch) < 0.2f && fabsf(m_yaw) < 0.2f);    
}



HRESULT CSkeletalViewerApp::Nui_Init()
{
    HRESULT              hr;

	// создание события, связанного с данными о скелете

	// параметры: 
	// - дескриптор безопасности
	// - событие сброшено вручную
	// - начальное состояние события
	// - имя события

    m_hNextSkeletonEvent = CreateEvent( NULL, TRUE, FALSE, NULL );    	
	m_hNextColorFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hNextDepthFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	// инициализация датчиков кинект
	// перед вызовом любых других функций NuiXxx
    hr = NuiInitialize( 
        NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON | NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_HIGH_QUALITY_COLOR);
    if( FAILED( hr ) )
    {
		//printf("Failed NuiInitialization");
        return hr;
    }

	// включение отслеживания за скелетом (можно откл в любой момент)
	// параметры:
	// - дескриптор события, кторый устанавливается, когда фрейм с данными скелета готов к применению
	// - набор флагов для управл-я скелетом. SDK не исп-ет
    hr = NuiSkeletonTrackingEnable( m_hNextSkeletonEvent, m_isSeated ? NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT : 0 );
	
    if( FAILED( hr ) )
    {
		//printf("NuiSkeletonTrackingEnable");
        return hr;
    }

    // Start the Nui processing thread
	// событие, когда процесс останавливаетя
    m_hEvNuiProcessStop = CreateEvent(NULL,FALSE,FALSE,NULL);

	// поток, для обработки данных из кинекта
	// создаем новый поток и сразу же начиает выполнятся метод Nui_ProcessThread 

	InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_videoCS);
	InitializeCriticalSection(&m_depthCS);

    m_hThNuiProcess = CreateThread(NULL,0,Nui_SkeletonProcessThread,this,0,NULL);

	INuiSensor * pSensor;
	NuiCreateSensorByIndex(0, &pSensor);
	pSensor->NuiGetCoordinateMapper(&m_pMapper);
    return hr;
}

void CSkeletalViewerApp::EnableVideo(bool isEnabled)
{	
	if ((!m_isVideoEnabled) && (isEnabled))
	{		
		EnterCriticalSection(&m_cs);
		m_hThNuiProcess2 = CreateThread(NULL,0,Nui_DepthProcessThread,this,0,NULL);
		HRESULT hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, 0, 2, m_hNextColorFrameEvent, &m_pVideoStreamHandle);	
		HRESULT hr2 = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, NUI_IMAGE_RESOLUTION_640x480, 0, 2, m_hNextDepthFrameEvent, &m_pDepthStreamHandle);	
		
		LeaveCriticalSection(&m_cs);
	}

	m_isVideoEnabled = isEnabled;
}

void CSkeletalViewerApp::EnableFaceTracking()
{
	EnableVideo(true);
	if(!m_hThNuiProcess3)
	{
		if(!m_hEvFTProcessStop)
		{
			m_hEvFTProcessStop = CreateEvent(NULL,FALSE,FALSE,NULL);
		}
		m_hThNuiProcess3 = CreateThread(NULL,0,Nui_FaceProcessThread,this,0,NULL);
	}
}

void CSkeletalViewerApp::DisableFaceTracking()
{
	if(m_hEvFTProcessStop!=NULL)
    {
        // Signal the thread
        SetEvent(m_hEvFTProcessStop);

        // Wait for thread to stop
        if(m_hThNuiProcess3 != NULL)
		{
			WaitForSingleObject(m_hThNuiProcess3,INFINITE);
			CloseHandle(m_hThNuiProcess3);
		}
        CloseHandle(m_hEvFTProcessStop);
    }
	m_hThNuiProcess3 = NULL;
	m_hEvFTProcessStop = NULL;
	m_isFaceTrackingSuccessful = false;
}

unsigned int CSkeletalViewerApp::GetNextVideoFrameSize()
{
	return m_videoDataSize;
}

void CSkeletalViewerApp::GetNextVideoFrame(void** data)
{
	//выбор подходящего кадра
	unsigned short index = GetBestMatchingColorFrameIndex();
	EnterCriticalSection(&m_videoCS);
	memcpy(*data, m_videoData[index], m_videoDataSize);
	m_isVideoFrameOld = true;
	LeaveCriticalSection(&m_videoCS);
}

unsigned short CSkeletalViewerApp::GetBestMatchingColorFrameIndex()
{
	unsigned short index = 0;
	long long delta = abs(m_colorTimeStamp[0] - m_depthTimeStamp);
	for(unsigned short i = 1; i < m_videoFrames; ++i)
	{
		if(abs(m_colorTimeStamp[i] - m_depthTimeStamp) < delta)
		{
			delta = abs(m_colorTimeStamp[i] - m_depthTimeStamp);
			index = i;
		}
	}
	return index;
}

unsigned int CSkeletalViewerApp::GetNextDepthFrameSize()
{
	return m_depthDataSize;
}

void CSkeletalViewerApp::GetNextDepthFrame(void** data)
{
	EnterCriticalSection(&m_depthCS);
	memcpy(*data, m_depthData, m_depthDataSize);
	LeaveCriticalSection(&m_depthCS);
}

void CSkeletalViewerApp::GetNextMappedDepthFrame(void** data)
{
	if(!m_mappedDepthData)
	{
		return;
	}
	memcpy(*data, m_mappedDepthData, m_depthDataSize);
}

bool CSkeletalViewerApp::IsNextVideoFrameAvailable()
{
	return (m_isVideoFrameOld == false);
}

void CSkeletalViewerApp::Nui_UnInit( )
{
    // Stop the Nui processing thread

	EnterCriticalSection(&m_videoCS);
	EnterCriticalSection(&m_depthCS);
	EnterCriticalSection(&m_cs);

	NuiSkeletonTrackingDisable();
	DisableFaceTracking();

    if(m_hEvNuiProcessStop!=NULL)
    {
        // Signal the thread
        SetEvent(m_hEvNuiProcessStop);

        // Wait for thread to stop
        if(m_hThNuiProcess!=NULL)
        {
            WaitForSingleObject(m_hThNuiProcess,INFINITE);
            CloseHandle(m_hThNuiProcess);
        }
		if(m_hThNuiProcess2!=NULL)
        {
            WaitForSingleObject(m_hThNuiProcess2,INFINITE);
            CloseHandle(m_hThNuiProcess2);
        }
        CloseHandle(m_hEvNuiProcessStop);
    }	

	if (m_videoData != NULL)
	{
		for(unsigned short i = 0; i < m_videoFrames; ++i)
		{
			delete [] m_videoData[i];
		}
		delete[] m_videoData;
		m_videoData = NULL;
	}

	if (m_colorTimeStamp != NULL)
	{
		delete[] m_colorTimeStamp;
		m_colorTimeStamp = NULL;
	}

	if (m_depthData != NULL)
	{
		delete[] m_depthData;
		m_depthData = NULL;
	}

	if (m_mappedDepthData != NULL)
	{
		delete[] m_mappedDepthData;
		m_mappedDepthData = NULL;
	}

	if (m_depthPixels != NULL)
	{
		delete[] m_depthPixels;
		m_depthPixels = NULL;
	}

	if (m_depthPoints != NULL)
	{
		delete[] m_depthPoints;
		m_depthPoints = NULL;
	}

	if( m_hNextSkeletonEvent && ( m_hNextSkeletonEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextSkeletonEvent );
        m_hNextSkeletonEvent = NULL;
    }

	if( m_hNextColorFrameEvent && ( m_hNextColorFrameEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextColorFrameEvent );
        m_hNextColorFrameEvent = NULL;
    }

	if( m_hNextDepthFrameEvent && ( m_hNextDepthFrameEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextDepthFrameEvent );
        m_hNextDepthFrameEvent = NULL;
    }

    NuiShutdown( );

	LeaveCriticalSection(&m_videoCS);
	LeaveCriticalSection(&m_depthCS);
	LeaveCriticalSection(&m_cs);

	//DeleteCriticalSection(&m_cs);
	//DeleteCriticalSection(&m_videoCS);
}

bool CSkeletalViewerApp::GetHeadPoints(FT_VECTOR3D* hint)
{
	if(m_isFoundSkeleton)
	{
		hint[0].x = m_coords[3 * NUI_SKELETON_POSITION_HEAD];
        hint[0].y = m_coords[3 * NUI_SKELETON_POSITION_HEAD + 1];
        hint[0].z = m_coords[3 * NUI_SKELETON_POSITION_HEAD + 2];
		hint[1].x = m_coords[3 * NUI_SKELETON_POSITION_SHOULDER_CENTER];
		hint[1].y = m_coords[3 * NUI_SKELETON_POSITION_SHOULDER_CENTER + 1];
        hint[1].z = m_coords[3 * NUI_SKELETON_POSITION_SHOULDER_CENTER + 2];
		return true;
	}
	return false;
}

void SetCorrectThreadAffinity()
{
	_SYSTEM_INFO nfo;
	GetSystemInfo(&nfo);
	if (nfo.dwNumberOfProcessors > 1)
	{
		int threadMask = 1;
		for (unsigned int k = 0; k < nfo.dwNumberOfProcessors; k++)
		{
			threadMask *= 2;
		}

		threadMask -= 2;

		SetThreadAffinityMask(GetCurrentThread(), threadMask);
	}
}

DWORD WINAPI CSkeletalViewerApp::Nui_FaceProcessThread(LPVOID pParam)
{
	SetCorrectThreadAffinity();
	
	CSkeletalViewerApp *pthis=(CSkeletalViewerApp *) pParam;
	HANDLE              hEvent=pthis->m_hEvFTProcessStop;
    DWORD               nEventResult;

	FT_CAMERA_CONFIG videoConfig = {640, 480, NUI_CAMERA_COLOR_NOMINAL_FOCAL_LENGTH_IN_PIXELS};
    FT_CAMERA_CONFIG depthConfig = {640, 480, NUI_CAMERA_DEPTH_NOMINAL_FOCAL_LENGTH_IN_PIXELS * 2.0f};
	IFTFaceTracker* pFaceTracker = FTCreateFaceTracker(NULL);
	if (!pFaceTracker)
    {
        MessageBoxW(NULL, L"Could not create the face tracker.\n", L"Face Tracker Initialization Error\n", MB_OK);
        return 2;
    }
	HRESULT hr = pFaceTracker->Initialize(&videoConfig, &depthConfig, NULL, NULL);
	if (FAILED(hr))
    {
        MessageBoxW(NULL, L"Could not initialize the face tracker.\n", L"Face Tracker Initialization Error\n", MB_OK);
        return 2;
    }
	IFTResult* pFTResult = NULL;
	hr = pFaceTracker->CreateFTResult(&pFTResult);
	if (FAILED(hr) || !pFTResult)
    {
        MessageBoxW(NULL, L"Could not create the IFTResult.\n", L"Face Tracker Initialization Error\n", MB_OK);
        return 2;
    }

	IFTImage * color = FTCreateImage();
	IFTImage * depth = FTCreateImage();

	while(!pthis->m_videoData || !pthis->m_videoData[pthis->m_videoFrames - 1] || !pthis->m_depthData)
	{
		Sleep(100);
	}
	depth->Attach(depthConfig.Width, depthConfig.Height, pthis->m_depthData, FTIMAGEFORMAT_UINT16_D13P3, 640 * 2);
	FT_VECTOR3D headPoints[2];//Head and neck coordinates taken from skeleton

	bool isFaceTracked = false;
	while(1)
    {
        // Wait for an event to be signalled
        nEventResult=WaitForSingleObject(hEvent, 75);

        // If the stop event, stop looping and exit
        if(nEventResult==0)
            break;
		
		color->Attach(videoConfig.Width, videoConfig.Height, pthis->m_videoData[pthis->GetBestMatchingColorFrameIndex()],
			FTIMAGEFORMAT_UINT8_B8G8R8X8, 640 * 4);
		FT_SENSOR_DATA sensorData(color, depth);
		FT_VECTOR3D* hint = NULL;
		if(pthis->GetHeadPoints(headPoints))
		{
			hint = headPoints;
		}
		// Do face tracking
		if(!isFaceTracked)
		{
			// Initiate face tracking.
			// This call is more expensive and searches the input frame for a face.	
			hr = pFaceTracker->StartTracking(&sensorData, NULL, hint, pFTResult);
		}
		else
		{
			// Continue tracking. It uses a previously known face position.
			// This call is less expensive than StartTracking()
			hr = pFaceTracker->ContinueTracking(&sensorData, hint, pFTResult);
		}
		isFaceTracked = SUCCEEDED(hr) && SUCCEEDED(pFTResult->GetStatus());
		pthis->m_isFaceTrackingSuccessful = isFaceTracked;
		if(isFaceTracked)
		{
			float scale;
			pFTResult->Get3DPose(&scale, pthis->m_rotation, pthis->m_translations);
			float * au = NULL;
			pthis->m_AUcount = 6;
			pFTResult->GetAUCoefficients(&au, &pthis->m_AUcount);
			memcpy(pthis->m_AUcoeffs, au, sizeof(float) * pthis->m_AUcount);
		}
		else
		{
			pFTResult->Reset();
		}
	}
	pFTResult->Release();
	color->Release();
	depth->Release();
	pFaceTracker->Release();
	return 0;
}

// датчики кинект поставляют кадры регулярно
DWORD WINAPI CSkeletalViewerApp::Nui_SkeletonProcessThread(LPVOID pParam)
{
	SetCorrectThreadAffinity();

    CSkeletalViewerApp *pthis=(CSkeletalViewerApp *) pParam;
    HANDLE                hEvents[3];//4
    int                   nEventIdx;

    // Configure events to be listened on
    hEvents[0]=pthis->m_hEvNuiProcessStop;
	hEvents[1]=pthis->m_hNextSkeletonEvent;
	hEvents[2]=pthis->m_hNextColorFrameEvent;
	//hEvents[3]=pthis->m_hNextDepthFrameEvent;

    // Main thread loop
    while(1)
    {
        // Wait for an event to be signalled
        nEventIdx=WaitForMultipleObjects(sizeof(hEvents)/sizeof(hEvents[0]),hEvents,FALSE,100);

        // If the stop event, stop looping and exit
        if(nEventIdx==0)
            break;            
      
        // Process signal events
       if (1 == nEventIdx)
       {
		   {
				CTimeMeasurer(BLOCK_ID_CAPTURE_SKELETON, g_timeProcessor, g_log);
				pthis->Nui_GotSkeletonAlert( );
		   }
       }
	   else if (2 == nEventIdx)
	   {
		   {
				CTimeMeasurer(BLOCK_ID_CAPTURE_VIDEO_AND_DEPTH, g_timeProcessor, g_log);				
				pthis->Nui_GotColorAlert();
				Sleep(50);
		   }
	   }
	   /*else if (3 == nEventIdx)
	   {
		   pthis->Nui_GotDepthAlert();
	   }*/
    }

    return (0);
}

DWORD WINAPI CSkeletalViewerApp::Nui_DepthProcessThread(LPVOID pParam)
{
	SetCorrectThreadAffinity();

	CSkeletalViewerApp *pthis = (CSkeletalViewerApp *) pParam;
	HANDLE hEvents[2];
	hEvents[0]=pthis->m_hEvNuiProcessStop;
	hEvents[1] = pthis->m_hNextDepthFrameEvent;
	int nEventIdx;

	while(true)
	{
		nEventIdx = WaitForMultipleObjects(sizeof(hEvents)/sizeof(hEvents[0]),hEvents,FALSE,100);
		if(nEventIdx==0)
            break;     
		if(nEventIdx == 1)
		{
			pthis->Nui_GotDepthAlert();
			Sleep(50);
		}
	}

	return (0);
}

bool CSkeletalViewerApp::Nui_GotColorAlert( )
{
	if (!m_isVideoEnabled)
	{
		return false;
	}

    NUI_IMAGE_FRAME* pimageFrame;
    bool processedFrame = true;

    HRESULT hr = NuiImageStreamGetNextFrame( m_pVideoStreamHandle, 0, (const NUI_IMAGE_FRAME**)&pimageFrame );

    if ( FAILED( hr ) )
    {
        return false;
    }

	EnterCriticalSection(&m_videoCS);
    INuiFrameTexture * pTexture = pimageFrame->pFrameTexture;
    NUI_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    if ( LockedRect.Pitch != 0 )
    {
		m_videoDataSize = LockedRect.size;

		if (!m_videoData)
		{
			m_videoData = new unsigned char*[m_videoFrames];
			m_colorTimeStamp = new long long[m_videoFrames];
			for(unsigned short i = 0; i < m_videoFrames; ++i)
			{
				m_colorTimeStamp[i] = 0;
				m_videoData[i] = NULL;
			}
		}
		else
		{
			//сдвигаем фреймы в буфере
			delete [] m_videoData[0];
			for(unsigned short i = 1; i < m_videoFrames; ++i)
			{
				m_colorTimeStamp[i - 1] = m_colorTimeStamp[i];
				m_videoData[i - 1] = m_videoData[i];
			}
		}
		m_videoData[m_videoFrames - 1] = new unsigned char[m_videoDataSize];
		memcpy(m_videoData[m_videoFrames - 1], static_cast<BYTE *>(LockedRect.pBits), LockedRect.size);
		m_colorTimeStamp[m_videoFrames - 1] = pimageFrame->liTimeStamp.QuadPart;
		m_isVideoFrameOld = false;

		if (m_debugMode == DEBUG_MODE_NO_DEBUG)
		{
			UpdateSpecificPixelsCount<DEBUG_MODE_NO_DEBUG>();
		}
		else if (m_debugMode == DEBUG_MODE_SHOW_MARKERS)
		{
			UpdateSpecificPixelsCount<DEBUG_MODE_SHOW_MARKERS>();
		}
		else if (m_debugMode == DEBUG_MODE_SHOW_MARKERS_BY_CATEGORY)
		{
			UpdateSpecificPixelsCount<DEBUG_MODE_SHOW_MARKERS_BY_CATEGORY>();
		}		
			
        //m_pDrawColor->Draw(  );
    }
	LeaveCriticalSection(&m_videoCS);

    pTexture->UnlockRect( 0 );

    NuiImageStreamReleaseFrame( m_pVideoStreamHandle, pimageFrame );

    return processedFrame;
}


bool CSkeletalViewerApp::Nui_GotDepthAlert( )
{
	if (!m_isVideoEnabled)
	{
		return false;
	}

    NUI_IMAGE_FRAME* pimageFrame;
    bool processedFrame = true;

    HRESULT hr = NuiImageStreamGetNextFrame( m_pDepthStreamHandle, 0, (const NUI_IMAGE_FRAME**)&pimageFrame );

    if ( FAILED( hr ) )
    {
        return false;
    }

	m_depthTimeStamp = pimageFrame->liTimeStamp.QuadPart;

    INuiFrameTexture * pTexture = pimageFrame->pFrameTexture;
    NUI_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    if ( LockedRect.Pitch != 0 )
    {
		EnterCriticalSection(&m_depthCS);
		m_depthDataSize = LockedRect.size;
		if (!m_depthData)
		{
			m_depthData = (unsigned short *)new unsigned char[m_depthDataSize];
		}
		memcpy(m_depthData, static_cast<BYTE *>(LockedRect.pBits), LockedRect.size);
        //m_pDrawColor->Draw(  );
		LeaveCriticalSection(&m_depthCS);
    }

    pTexture->UnlockRect( 0 );

    NuiImageStreamReleaseFrame( m_pDepthStreamHandle, pimageFrame );

	MapDepthFrameToColorFrame();

    return processedFrame;
}


float GetPointsTrackingQuality(NUI_SKELETON_DATA& skel, std::vector<int>& bonesIDs)
{
	float res = 0;

	std::vector<int>::iterator it = bonesIDs.begin();
	std::vector<int>::iterator itEnd = bonesIDs.end();

	for ( ; it != itEnd; it++)
	{
		NUI_SKELETON_POSITION_TRACKING_STATE bone0State = skel.eSkeletonPositionTrackingState[*it];

		if (bone0State == NUI_SKELETON_POSITION_NOT_TRACKED)
		{
			res += 0;			
		}
		else if (bone0State == NUI_SKELETON_POSITION_INFERRED)
		{
			res += 1;			
		}
		else
		{
			res += 2;
		}
	}

	res /= (float)(bonesIDs.size() * 2);

    return res;
}

void CSkeletalViewerApp::Nui_GotSkeletonAlert( )
{
	EnterCriticalSection(&m_cs);

	// в фрейме данные о всех скелетах
	// от 2 активно отслеживаемых до 4 пассивно отслеживаемых
	// хранятся в SkeletonData[i] - в сумме 20 точек

    NUI_SKELETON_FRAME SkeletonFrame;

	// получаем фрем о скелетом
	// параметры:
	// - количество миллисекунд ожидания кадра
	// - указатель на место, в которое вернуть 
    HRESULT hr = NuiSkeletonGetNextFrame( 0, &SkeletonFrame );
	m_isFoundSkeleton = false;
	if (hr != S_OK)
	{
		LeaveCriticalSection(&m_cs);
		m_qualityFlags = 0;
		return;
	}
	
	// выравниваем фрейм для отображаения

	NUI_TRANSFORM_SMOOTH_PARAMETERS * pSmoothingParams = new NUI_TRANSFORM_SMOOTH_PARAMETERS();
	pSmoothingParams->fCorrection = 0.0f;
	pSmoothingParams->fSmoothing  = 0.8f;
	pSmoothingParams->fPrediction = 0.5f;
	pSmoothingParams->fJitterRadius = 0.1f;
	pSmoothingParams->fMaxDeviationRadius = 0.1f;

    NuiTransformSmooth(&SkeletonFrame, pSmoothingParams);

	int skeletonCount = 0;
	for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
	if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
	{
		skeletonCount++;
	}

	if (skeletonCount != 1)
	{
		m_isTwoSkeletonsFound = true;
		LeaveCriticalSection(&m_cs);
		m_qualityFlags = 0;
		return;
	}

	m_isTwoSkeletonsFound = false;

	for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
	{
		// eTrackingState - указвает на то, что структура _NUI_SKELETON_DATA содержит данные для всех точек (скелет) или просто общие данные
		// NUI_SKELETON_POSITION_ONLY  - Position есть (описывает центр масс для скелета), но SkeletonPosition, eSkeletonPositionTrackingState, dwQualityFlags нет 
		// NUI_SKELETON_TRACKED, где заданы:
		//   SkeletonPosition - позиция каждого элемента скелета, 
		//   eSkeletonPositionTrackingState - массив, с указанием будет отслеживатся соответсвующая позиция или нет
		//   dwQualityFlags - какая часть скелета за кадром
        if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
        {			 
			const NUI_SKELETON_DATA & skeleton = SkeletonFrame.SkeletonData[i];
 			NUI_SKELETON_BONE_ORIENTATION boneOrientations[NUI_SKELETON_POSITION_COUNT];
			NuiSkeletonCalculateBoneOrientations(&skeleton, boneOrientations);
			m_globalRotationX = boneOrientations[NUI_SKELETON_POSITION_HIP_CENTER].absoluteRotation.rotationQuaternion.x;
			m_globalRotationY = boneOrientations[NUI_SKELETON_POSITION_HIP_CENTER].absoluteRotation.rotationQuaternion.y;
			m_globalRotationZ = boneOrientations[NUI_SKELETON_POSITION_HIP_CENTER].absoluteRotation.rotationQuaternion.z;
			m_globalRotationW = boneOrientations[NUI_SKELETON_POSITION_HIP_CENTER].absoluteRotation.rotationQuaternion.w;

			for (int j = 0; j< NUI_SKELETON_POSITION_COUNT*3; j = j+3)
			{
				int index = j/3;
				m_coords[j] =  SkeletonFrame.SkeletonData[i].SkeletonPositions[index].x;
				m_coords[j+1] =  SkeletonFrame.SkeletonData[i].SkeletonPositions[index].y;
				m_coords[j+2] =  SkeletonFrame.SkeletonData[i].SkeletonPositions[index].z;				
			}

			// 2D координаты скелета
			for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++)//was i
			{
				m_points[j] = SkeletonToScreen(skeleton.SkeletonPositions[j], KINECT_VIDEO_WIDTH, KINECT_VIDEO_HEIGHT );
			}

		
			m_headQuality = GetPointsTrackingQuality(SkeletonFrame.SkeletonData[i], headBones);
			m_leftFootQuality = GetPointsTrackingQuality(SkeletonFrame.SkeletonData[i], leftFootBones);
			m_rightFootQuality = GetPointsTrackingQuality(SkeletonFrame.SkeletonData[i], rightFootBones);
			m_leftArmQuality = GetPointsTrackingQuality(SkeletonFrame.SkeletonData[i], leftArmBones);
			m_rightArmQuality = GetPointsTrackingQuality(SkeletonFrame.SkeletonData[i], rightArmBones);
			m_hipQuality = GetPointsTrackingQuality(SkeletonFrame.SkeletonData[i], hipBones);
			m_hipsQuality = GetPointsTrackingQuality(SkeletonFrame.SkeletonData[i], hipsBones);

            m_isFoundSkeleton = true; 
			m_qualityFlags = SkeletonFrame.SkeletonData[i].dwQualityFlags;
			m_playerIndex = i;
			break;
        }
    }
	LeaveCriticalSection(&m_cs);
 }

float CSkeletalViewerApp::GetBipTrackingQuality()
{
	EnterCriticalSection(&m_cs);
	float res = m_hipQuality;
	LeaveCriticalSection(&m_cs);
	return res;
}

float CSkeletalViewerApp::GetBipsTrackingQuality()
{
	EnterCriticalSection(&m_cs);
	float res = m_hipsQuality;
	LeaveCriticalSection(&m_cs);
	return res;
}

float CSkeletalViewerApp::GetHeadTrackingQuality()
{
	EnterCriticalSection(&m_cs);
	float res = m_headQuality;
	LeaveCriticalSection(&m_cs);
	return res;	
}

float CSkeletalViewerApp::GetLeftArmTrackingQuality()
{
	EnterCriticalSection(&m_cs);
	float res = m_leftArmQuality;
	LeaveCriticalSection(&m_cs);
	return res;	
}

float CSkeletalViewerApp::GetRightArmTrackingQuality()
{
	EnterCriticalSection(&m_cs);
	float res = m_rightArmQuality;
	LeaveCriticalSection(&m_cs);
	return res;	
}

float CSkeletalViewerApp::GetLeftFootTrackingQuality()
{
	EnterCriticalSection(&m_cs);
	float res = m_leftFootQuality;
	LeaveCriticalSection(&m_cs);
	return res;	
}

float CSkeletalViewerApp::GetRightFootTrackingQuality()
{
	EnterCriticalSection(&m_cs);
	float res = m_rightFootQuality;
	LeaveCriticalSection(&m_cs);
	return res;	
}


bool CSkeletalViewerApp::IsFoundSkeleton()
{
	return m_isFoundSkeleton;
}

bool CSkeletalViewerApp::IsTwoSkeletonsFound()
{
	return m_isTwoSkeletonsFound;
}

bool CSkeletalViewerApp::IsSkeletonClippedRight()
{
	return ((m_qualityFlags & NUI_SKELETON_QUALITY_CLIPPED_RIGHT) > 0);
}

bool CSkeletalViewerApp::IsSkeletonClippedLeft()
{
	return ((m_qualityFlags & NUI_SKELETON_QUALITY_CLIPPED_LEFT) > 0);
}

bool CSkeletalViewerApp::IsSkeletonClippedTop()
{
	return ((m_qualityFlags & NUI_SKELETON_QUALITY_CLIPPED_TOP) > 0);
}

bool CSkeletalViewerApp::IsSkeletonClippedBottom()
{
	return ((m_qualityFlags & NUI_SKELETON_QUALITY_CLIPPED_BOTTOM) > 0);
}

 float*	CSkeletalViewerApp::GetSkeletonXYZ()
 {
	 //m_coords[0] = m_skeleton.x;
	 //m_coords[1] = m_skeleton.y;
	 //m_coords[2] = m_skeleton.z;
	 EnterCriticalSection(&m_cs);

	 memcpy(m_exportCoords,  m_coords, sizeof(m_coords));

	 LeaveCriticalSection(&m_cs);

	 return m_exportCoords; 
 }

 void CSkeletalViewerApp::SetSpecificPixelsChannel(char channelID)
 {
	m_channelID = channelID;
	m_otherChannelID1 = 0;
	m_otherChannelID2 = 1;
	if (m_channelID == 0)
	{
		m_otherChannelID1 = 1;
		m_otherChannelID2 = 2;
	}
	else if (m_channelID == 1)
	{
		m_otherChannelID1 = 0;
		m_otherChannelID2 = 2;
	}


	if (m_channelID != 3 && m_otherColorKoef1Arr && m_otherColorKoef2Arr) // no marker
	{
		m_otherColorKoef1 = m_otherColorKoef1Arr[m_channelID];
	 	m_otherColorKoef2 = m_otherColorKoef2Arr[m_channelID];
	}
	else
	{
		m_otherColorKoef1 = 0;
		m_otherColorKoef2 = 0;
	}
 }

unsigned int CSkeletalViewerApp::GetSpecificPixelsCount()
{
	return m_specificPixelsCount;
	//return 5000;
}

template <int T>
void CSkeletalViewerApp::UpdateSpecificPixelsCount()
{	
	CTimeMeasurer(BLOCK_ID_FIND_PIXELS, g_timeProcessor, g_log);

	unsigned int count = 0;

	if ((m_videoDataSize == 0) || (m_depthDataSize == 0))
	{
		m_specificPixelsCount = 0;
		return;
	}

	int k = 0;

	const int blueIndex = 0;
	for (int y = 0; y < KINECT_VIDEO_HEIGHT; y++)
	{
		for (int x = 0; x < KINECT_VIDEO_WIDTH; x++, k++)			
		{
			unsigned int i = y * KINECT_VIDEO_WIDTH * 4 + x * 4;
			m_videoData[m_videoFrames - 1][i + blueIndex] *= 0.9f;
		}
	}

	/*int y1 = m_points[NUI_SKELETON_POSITION_HIP_LEFT].y;
	if (m_points[NUI_SKELETON_POSITION_HIP_RIGHT].y > y1)
	{
		y1 = m_points[NUI_SKELETON_POSITION_HIP_RIGHT].y;
	}
	int y2 = m_points[NUI_SKELETON_POSITION_SHOULDER_CENTER].y;
	y1 = y2 + (y1 - y2) * 1.3f;
	y2 = (y1 * 1 + y2 * 3) / 4;*/

	if(!m_mappedDepthData)
	{
		return;
	}

	const int alphaIndex = 3;
	k = 0;
	for (int y = 0; y < KINECT_VIDEO_HEIGHT; y++)
	{
		for (int x = 0; x < KINECT_VIDEO_WIDTH; x++, k++)			
		{
			unsigned short value = m_mappedDepthData[k] << 13;
			value = value >> 13;
			unsigned int i = y * KINECT_VIDEO_WIDTH * 4 + x * 4;
			m_videoData[m_videoFrames - 1][i + alphaIndex] = 255;
			
			if (value == m_playerIndex + 1)				
			{
				if (i < m_videoDataSize)
				{
					m_videoData[m_videoFrames - 1][i + alphaIndex] = 0;
					int c1 = m_videoData[m_videoFrames - 1][i + m_otherChannelID2];
					c1 *= m_mainColorKoef;

					int c2 = m_videoData[m_videoFrames - 1][i + m_otherChannelID1];
					c2 *= m_mainColorKoef;

					int c3 = m_videoData[m_videoFrames - 1][i + m_channelID];
					c3 *= m_otherColorKoef1;

					int c4 = m_videoData[m_videoFrames - 1][i + m_channelID];
					c4 *= m_otherColorKoef2;

					bool cond1 = ((m_videoData[m_videoFrames - 1][i + m_channelID] > COLOR_THRESHOLD) && (c1 < c3) && (c2 < c4));
					bool cond2 = ((m_videoData[m_videoFrames - 1][i + m_channelID] > COLOR_THRESHOLD) && (c1 < c4) && (c2 < c3));

					if (cond1 || cond2)
					{
						if (T == DEBUG_MODE_SHOW_MARKERS)
						{
							m_videoData[m_videoFrames - 1][i + m_channelID] = 0;//255;
							m_videoData[m_videoFrames - 1][i + m_otherChannelID1] = 0;
							m_videoData[m_videoFrames - 1][i + m_otherChannelID2] = 255;//0;
						}
						else if (T == DEBUG_MODE_SHOW_MARKERS_BY_CATEGORY)
						{
							if (cond1)
							{
								m_videoData[m_videoFrames - 1][i + m_channelID] = 255;
								m_videoData[m_videoFrames - 1][i + m_otherChannelID1] = 255;
								m_videoData[m_videoFrames - 1][i + m_otherChannelID2] = 255;
							}
							else if (cond2)
							{
								m_videoData[m_videoFrames - 1][i + m_channelID] = 0;
								m_videoData[m_videoFrames - 1][i + m_otherChannelID1] = 0;
								m_videoData[m_videoFrames - 1][i + m_otherChannelID2] = 255;
							}							
						}

						count++;						
					}
					else if (T == DEBUG_MODE_SHOW_MARKERS_BY_CATEGORY)
					{
						m_videoData[m_videoFrames - 1][i + m_channelID] = m_videoData[m_videoFrames - 1][i + m_channelID] / 2;
						m_videoData[m_videoFrames - 1][i + m_otherChannelID1] = m_videoData[m_videoFrames - 1][i + m_channelID] / 2;
						m_videoData[m_videoFrames - 1][i + m_otherChannelID2] = m_videoData[m_videoFrames - 1][i + m_channelID] / 2;
					}
				}
			}	
		}
	}

	m_specificPixelsCount = count;	
}

void CSkeletalViewerApp::MapDepthFrameToColorFrame(void)
{
	if (!m_depthPixels)
	{
		m_depthPixelsSize = 640 * 480;//equal to depth frame resolution
		m_depthPixels = new NUI_DEPTH_IMAGE_PIXEL[m_depthPixelsSize];
	}
	for(unsigned int i = 0; i < m_depthPixelsSize; ++i)
	{
		m_depthPixels[i].depth = m_depthData[i] >> 3;
		m_depthPixels[i].depth <<= 3;
		m_depthPixels[i].playerIndex = m_depthData[i] << 13;
		m_depthPixels[i].playerIndex >>= 13;
	}
	if (!m_depthPoints)
	{
		m_depthPointsSize = 640 * 480;//equal to image frame resolution
		m_depthPoints = new NUI_DEPTH_IMAGE_POINT[m_depthPointsSize];
	}
	HRESULT hr = m_pMapper->MapColorFrameToDepthFrame(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_RESOLUTION_640x480,
		m_depthPixelsSize, m_depthPixels, m_depthPointsSize, m_depthPoints);
	if(FAILED(hr))
	{
		return;
	}
	if(!m_mappedDepthData)
	{
		m_mappedDepthData = new unsigned short [m_depthPointsSize];
	}
	for(unsigned int i = 0; i < m_depthPointsSize; ++i)
	{
		unsigned int index = m_depthPoints[i].y * KINECT_VIDEO_WIDTH + m_depthPoints[i].x;
		if(index < m_depthPixelsSize)
		{
			m_mappedDepthData[i] = m_depthData[index];
		}
		else
		{
			m_mappedDepthData[i] = 0;
		}
	}
}

void CSkeletalViewerApp::GetAUCoeffs(float * coeffs, unsigned int * count)
{
	memcpy(coeffs, m_AUcoeffs, sizeof(m_AUcoeffs));
	*count = m_AUcount;
}

void CSkeletalViewerApp::GetHeadRotation(float * rotation, float * translation)
{
	memcpy(rotation, m_rotation, sizeof(m_rotation));//pitch, yaw, roll
	memcpy(translation, m_translations, sizeof(m_translations));//x, y, z
}

/// <summary>
/// Converts a skeleton point to screen space
/// </summary>
/// <param name="skeletonPoint">skeleton point to tranform</param>
/// <param name="width">width (in pixels) of output buffer</param>
/// <param name="height">height (in pixels) of output buffer</param>
/// <returns>point in screen-space</returns>
D2D1_POINT_2F CSkeletalViewerApp::SkeletonToScreen( Vector4 skeletonPoint, int width, int height )
{
	LONG x, y;
	USHORT depth;

	// calculate the skeleton's position on the screen
	// NuiTransformSkeletonToDepthImage returns coordinates in NUI_IMAGE_RESOLUTION_320x240 space
	NuiTransformSkeletonToDepthImage( skeletonPoint, &x, &y, &depth);
	
	LONG _x, _y;
	NuiImageGetColorPixelCoordinatesFromDepthPixel(NUI_IMAGE_RESOLUTION_640x480, NULL, x, y, depth, &_x, &_y);

	/*float screenPointX = static_cast<float>(x * width) / 320;
	float screenPointY = static_cast<float>(y * height) / 240;

	return D2D1::Point2F(screenPointX, screenPointY);*/
	return D2D1::Point2F(_x, _y);
}

float* CSkeletalViewerApp::GetSkeletonXY()
{
	for (int j = 0; j < NUI_SKELETON_POSITION_COUNT * 2; j = j + 2)
	{
		int index = j / 2;
		m_skeletonCoords[j] = m_points[index].x;
		m_skeletonCoords[j + 1] = m_points[index].y;
	}

	return m_skeletonCoords; 
}

void CSkeletalViewerApp::SetColorDetectionKoefs(int mainColorKoef, int* otherColorKoef1, int* otherColorKoef2)
{
	m_mainColorKoef = mainColorKoef;
	m_otherColorKoef1Arr = otherColorKoef1;
	m_otherColorKoef2Arr = otherColorKoef2;

	if (m_channelID != 3) // no marker
	{
		m_otherColorKoef1 = otherColorKoef1[m_channelID];
	 	m_otherColorKoef2 = otherColorKoef2[m_channelID];
		m_mainColorKoef = mainColorKoef;
	}
	else
	{
		m_otherColorKoef1 = 0;
		m_otherColorKoef2 = 0;
		m_mainColorKoef = 20;
	}
}
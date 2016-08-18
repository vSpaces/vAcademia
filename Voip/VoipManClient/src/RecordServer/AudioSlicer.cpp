// AudioSlicer.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "serverInfo.h"
#include "../../include/RecordServer/AudioSlicer.h"
#include "../../include/RecordServer/RecordServer.h"
#include <Mmsystem.h>

/************************************************************************/
// Функции нарезки звука по 10 секунд
/************************************************************************/
CAudioSlicer::CAudioSlicer( IVoipManager* apVolumeControl
							, CRecordServerConnection* apRecordServerConnection)
{
	ATLASSERT( apVolumeControl);
	ATLASSERT( apRecordServerConnection);
	volumeControl = apVolumeControl;
	recordServerConnection = apRecordServerConnection;

	userID = 0;
	silenceTime = 0;
	talkingTime = 0;
	talkingEvent.Create("TalkingEvent", TRUE);

	Start();
}

CAudioSlicer::~CAudioSlicer()
{
	SignalStop();
	talkingEvent.SetEvent();
	Stop();
}

DWORD CAudioSlicer::Run()
{
	unsigned int sleepTime = 0;
	while (!ShouldStop())
	{
		//! Дождемся начала речи
		talkingEvent.WaitForEvent();

		sleepTime = GetCurrentSleepTime();
		//talkingTime += sleepTime;
		int timeBeforeSleep = timeGetTime();
		Sleep( sleepTime);

		talkingTime += timeGetTime() - timeBeforeSleep;
		if( ShouldStop())
			break;

		// синхронизация не требуется
		if( talkingTime < CHECKTIME_NORMAL_DURATION)
			continue;

/*		// принудительная синхронизация по молчанию
		if( talkingTime < CHECKTIME_TOOLONG_DURATION)
		{
			if( volumeControl->GetCurrentInputLevel() <= volumeControl->GetMicActivationLevel())
			{
				silenceTime += sleepTime;
				if( silenceTime > WAIT_SILENCE_TIME)
				{
					ForceSynchronization();
					continue;
				}
			}
		}*/
		if( talkingTime >= CHECKTIME_TOOLONG_DURATION)	// принудительная синхронизация
			ForceSynchronization();
	}
	return 0;
}

void CAudioSlicer::ForceSynchronization()
{
	recordServerConnection->ForceSynchronization( userID);
	OnStartTalking( userID);
}

unsigned int CAudioSlicer::GetCurrentSleepTime()
{
	if( talkingTime < CHECKTIME_NORMAL_DURATION)
		return CHECKTIME_NORMAL_DURATION / 3;

	return 100;
}

// реализация IAudioSlicerIn
void	CAudioSlicer::OnStartTalking( int uiUserID)
{
	userID = uiUserID;
	talkingTime = 0;
	silenceTime = 0;
	talkingEvent.SetEvent();
}

void	CAudioSlicer::Reset()
{
	talkingTime = 0;
	silenceTime = 0;
}

void	CAudioSlicer::OnStopTalking()
{
	if( !ShouldStop())
		talkingEvent.ResetEvent();
}
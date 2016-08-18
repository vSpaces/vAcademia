#ifndef __AUDIO_SLICER_H_
#define __AUDIO_SLICER_H_

#include "ViskoeThread.h"

// Периоды синхронизации
// время постоянного разговора после которого начинается синхронизация
#define CHECKTIME_NORMAL_DURATION		15000
// время постоянного разговора до которого будет синхронизация в момент молчания пользователя
//								и после которого обязательно будет синхронизация, даже если пользователь что-то говорит
#define CHECKTIME_TOOLONG_DURATION		21000

#define WAIT_SILENCE_TIME				500

class CRecordServerConnection;

class CAudioSlicer : public ViskoeThread::CThreadImpl<CAudioSlicer>
{
public:
	CAudioSlicer( IVoipManager* apVolumeControl, CRecordServerConnection* apRecordServerConnection);
	~CAudioSlicer();

	// реализация CThreadImpl<CCaptureWindow>
public:
	DWORD Run();

public:
	void	Reset();
	void	OnStartTalking( int uiUserID);
	void	OnStopTalking();

private:
	unsigned int GetCurrentSleepTime();
	void ForceSynchronization();

private:
	unsigned int	silenceTime;
	unsigned int	talkingTime;
	int				userID;
	CEvent			talkingEvent;
	IVoipManager*	volumeControl;
	CRecordServerConnection* recordServerConnection;
};

#endif	// __AUDIO_SLICER_H_
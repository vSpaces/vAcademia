#pragma once

class CMicrophoneControllerThread : public PThread
{
public:
	CMicrophoneControllerThread( CVoiceIPClient *voiceIPClient):
	PThread ( 1000,  NoAutoDeleteThread,  NormalPriority,  "MicrophoneControllerThread")
	{
		this->voiceIPClient = voiceIPClient;
		Resume();
	}

	void Main()
	{
		while ( !voiceIPClient->isMicrEnabled())
		{
			Sleep( 100);
		}
		voiceIPClient->muteMicrophoneCmd( true);
		//((CTestVoiceIPClient *) voiceIPClient)->startSoundLog();
	}

protected:
	CVoiceIPClient *voiceIPClient;
};
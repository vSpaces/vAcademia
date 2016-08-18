#pragma once

#include "../VoipManager.h"
#include "VoiceIPClient.h"
#include "VoipBase.h"

class CMicrophoneControllerThread;
class CWavePlaying;

class CVoipManagerH323 : public CVoIPBase, public CVoipManager
{
public:
	CVoipManagerH323(oms::omsContext* aContext);
	~CVoipManagerH323(void);

	// реализация voip::IVoipManager
public:
	//! Подключиться к определенному серверу голосовой связи
	voipError	Connect( LPCSTR alpcServer, LPCSTR alpcGroupName
						, LPCSTR alpcUserName, LPCSTR alpcPassword = NULL, SHORT aiAvatarID = -1);

	//! Отключиться от текущего сервера голосовой связи
	voipError	Disconnect();

	//! Создать говорящего бота с номером
	voipError	CreateServerTalkingBot( unsigned int aClientSideObjectID);

	//! Удалить говорящего бота с номером
	voipError	DestroyServerTalkingBot( unsigned int aClientSideObjectID);

public:
	// обработка событий
	virtual void onCommand( CVoiceError *voiceError){}
	virtual void onCommand( CVoiceError::VOIP_ERRORS iErrorCode, LPCTSTR sCmd, LPCTSTR sPar){}
	virtual void OnConnectionEstablished( BOOL bSuccess);
	virtual void OnConnectionCleared( BOOL bError){}
	virtual bool Call( char* apProcName, char* aArgTypes, ...){ return true;}

protected:
	CVoiceIPClient	voiceClient;

	CMicrophoneControllerThread *micrCtrl;
	//CUpdaterLexThread *updaterLex;
	CWavePlaying *wavePlaying;
	//CTestPlay3DSoundThread *testPlay3DSound;
};

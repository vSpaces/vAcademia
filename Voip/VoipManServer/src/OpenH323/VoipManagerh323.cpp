#include "StdAfx.h"

#ifdef USE_OPENH323_LIB

#include "..\..\include\OpenH323\VoipManagerH323.h"
#include "..\..\include\MicCtrlThread.h"
#include "ptclib\pstun.h"

#pragma comment(lib, "voipcodec.lib")
#pragma comment(lib, "AvSoundLib.lib")
#ifdef DEBUG
	#pragma comment(lib, "VoIPEndPointD.lib")
	#pragma comment(lib, "openh323d.lib")
	#pragma comment(lib, "ptlibd.lib")
	#pragma comment(lib, "ptclibd.lib")
#else
	#pragma comment(lib, "VoIPEndPoint.lib")
	#pragma comment(lib, "openh323.lib")
	#pragma comment(lib, "ptlib.lib")
	#pragma comment(lib, "ptclib.lib")
#endif

void print2screen( TCHAR *s)
{
	TCHAR dest[MAX_PATH];
	CharToOem( s, dest);
	printf( "%s\n", dest);
}

CVoipManagerH323::CVoipManagerH323(oms::omsContext* aContext) : CVoipManager( aContext)
{
	voiceClient.create( this, CVoiceIPClient::VOIP_NORMAL, NULL, NULL);
	voiceIPClient = &voiceClient;
}

CVoipManagerH323::~CVoipManagerH323(void)
{
	
}

int i=0;
//! Подключиться к определенному серверу голосовой связи
voipError	CVoipManagerH323::Connect( LPCSTR alpcServer, LPCSTR alpcGroupName
									, LPCSTR alpcUserName, LPCSTR alpcPassword, SHORT aiAvatarID)
{
	i++;
	if( i != 1)
		return ERROR_NOERROR;
	if ( voiceClient.isConnected())
		voiceClient.disconnect();

	voiceClient.setConnectParams( alpcServer, "0_0__test"/*alpcGroupName*/, alpcUserName, alpcPassword, aiAvatarID);

	if( voiceClient.connect())
		return ERROR_NOERROR;

	return ERROR_NOERROR;
}

//! Отключиться от текущего сервера голосовой связи
voipError	CVoipManagerH323::Disconnect()
{
	return ERROR_NOERROR;
}

//! Создать говорящего бота с номером
voipError	CVoipManagerH323::CreateServerTalkingBot( unsigned int aClientSideObjectID)
{
	return ERROR_NOERROR;
}

//! Удалить говорящего бота с номером
voipError	CVoipManagerH323::DestroyServerTalkingBot( unsigned int aClientSideObjectID)
{
	return ERROR_NOERROR;
}

void CVoipManagerH323::OnConnectionEstablished( BOOL bSuccess)
{
	LPCTSTR error = voiceClient.voiceError->getErrorMSG();
	int ii=bSuccess;

	if( !bSuccess)
		return;

	CComString msg = voiceIPClient->getMSG();
	printf( "Connect: %s", msg.GetBuffer());	
	MP_NEW_P( micrCtrl_, CMicrophoneControllerThread, voiceIPClient);
	micrCtrl = micrCtrl_;
	/*
	MP_NEW_P2( updaterLex_, CUpdaterLexThread, voiceIPClient, this);
	updaterLex = updaterLex_
	if ( wavePlayng == NULL)
	{
		MP_NEW_P( wavePlayng_, CWavePlaying, this);
		wavePlayng = wavePlayng_;
	}
	wavePlayng->start( "connecting.wav");
	*/

	H323EndPoint *endpoint = voiceIPClient->getEndPoint();
	PSTUNClient *stun = endpoint->GetSTUN();
	if ( stun != NULL)
	{
		PString typeName = stun->GetNatTypeName();
		printf( "stun typeName = %s\n", typeName.GetPointer());
		PTRACE( 3, "stun typeName = " << typeName);
		PString sType = PSTUNClient::GetNatTypeString( stun->GetNatType());
		printf( "stun sType = %s\n", sType.GetPointer());
		PTRACE( 3, "stun sType = " << sType);
	}
	else
	{
		printf( "stun is NULL\n");
		PTRACE( 3, "stun is NULL");
	}

	printf( "To quite from app enter key 'Esc'\n");

	print2screen( "Вы успешно подключились к серверу:");		
	PString psPlayDevice = endpoint->GetSoundChannelPlayDevice();
	CComString sPlayDevice = CComString( "Устройство воспроизведения звука: ") + CComString( psPlayDevice.GetPointer());		
	PString psRecordDevice = endpoint->GetSoundChannelRecordDevice();
	CComString sRecordDevice = CComString( "Устройство записи звука: ") + CComString( psRecordDevice.GetPointer());
	print2screen( sPlayDevice.GetBuffer());
	print2screen( sRecordDevice.GetBuffer());
	int pos = msg.Find( "Started logical channel: receiving ");
	int pos2 = msg.Find( '\n', pos);
	CComString s = msg.Left( pos2);
	s = "Кодек декодирования данных: " + s.Right( pos2 - pos - strlen( "Started logical channel: receiving "));
	print2screen( s.GetBuffer());
	pos = msg.Find( "Started logical channel: sending ");
	pos2 = msg.Find( '\n', pos);
	s = msg.Left( pos2);
	s = "Кодек кодирования данных: " + s.Right( pos2 - pos - strlen( "Started logical channel: sending "));
	print2screen( s.GetBuffer());
	print2screen( "Для выключение микрофона нажмите клавишу 'X' или 'x'");
	print2screen( "Для включение микрофона нажмите клавишу 'M' или 'm'");
	print2screen( "Для выхода нажмите клавишу 'Esc'");
}

#endif
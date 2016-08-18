#pragma once

#include "RecordServerBase.h"

class CSelfRecordServerConnection : public CRecordServerConnectionBase
{
public:
	CSelfRecordServerConnection (oms::omsContext* aContext, cs::imessagehandler* apHandler, ivoiceconnectionlistender* apConnectionListener);
	~CSelfRecordServerConnection ();

	//! Реализация isessionlistener
public:
	void OnConnectLost();
	void OnConnectRestored();

public:
	//! Запускает на сервере голосового клиента для записи звука
	bool PrepareSelfRecording( LPCSTR alpcVoipServerAddress, LPCSTR alpcRoomName, LPCSTR alpcUserName);

	//! Начинает запись of self voice
	bool ResumeSelfRecording( );

	//! Останавливает запись of self voice
	bool PauseSelfRecording( );

	//! Воспроизводит запись of self voice
	bool PlaySelfRecording( );

	//! Заканчивает запись общения для текущей группы
	bool StopSelfRecording();

	//! Возвращает true - если сейчас открыта сессия записи своего звука
	bool IsRecordingInProgress();

	//! Возвращает токен для данной сессии
	unsigned int GetSessionToken()
	{
		return sessionToken;
	}

private:
	//! 
	bool StartRecordingBase( LPCSTR alpcVoipServerAddress, LPCSTR alpcRoomName, LPCSTR alpcUserName, unsigned int aRecordID);

	//!
	unsigned int GenerateNextToken();

protected:
	//! Адрес голосового записи
	CComString		voipServerIP;
	//! Имя комнаты, которую пишем
	CComString		roomName;
	//! Номер записи, которую пишем
	unsigned int	recordID;
	//! Имя моего пользователя
	CComString		userName;
	//! Токен сессии
	unsigned int	sessionToken;

	//! Флаг состояния
	bool	recordingInProgress;
};
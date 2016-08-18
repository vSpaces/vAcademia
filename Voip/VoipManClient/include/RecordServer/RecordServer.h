#pragma once

#include "RecordServerBase.h"

class CRecordServerConnection : public CRecordServerConnectionBase
{
public:
	CRecordServerConnection(oms::omsContext* aContext, cs::imessagehandler* apHandler, ivoiceconnectionlistender* apConnectionListener);
	~CRecordServerConnection();

	//! Реализация isessionlistener
public:
	void OnConnectLost();
	void OnConnectRestored();

public:
	//! Копирует файл с именем alpcFileName из папки записи auSourceRecordID в папку текущей записи
	virtual bool CopyAudioFile( LPCTSTR alpcFileName, unsigned int auSourceRecordID);

	//! Пишется ли сейчас звук (мной)
	bool IsRecordingInProgress();

	//! Пишется ли сейчас звук (но не мной)
	bool IsRecordListeningInProgress();

	//! Начинает запись общения для текущей группы
	bool StartRecording( LPCSTR alpcVoipServerAddress, LPCSTR alpcRoomName, LPCSTR alpcUserName, unsigned int aRecordID);

	//! Заканчивает запись общения для текущей группы
	bool StopRecording(unsigned int aRecordID);

	//! Начинает получать событие начала и окончания записи звука
	bool StartListenRecording( unsigned int aRecordID);

	//! Заканчивает получать событие начала и окончания записи звука
	bool StopListenRecording( unsigned int aRecordID);

	//! Удалить запись (отсылает запрос на сервер)
	bool DeleteRecord(unsigned int aRecordID);

	//! Удалить записи (отсылает запрос на сервер)
	bool DeleteRecords( unsigned int *pRecIDs, unsigned int count);

	//! Переводит запись в режим паузы. При этом пишуший клиент закрывается, но
	//! связь с записывающим сервером не разрывается. При продолжении записи
	//! клиент будет запущен на текущем записывающем сервере.
	bool PauseRecording();

	//! Запускает запись клиентом в текущей группе на текущем записывающем сервере.
	//! При этом клиент должен подключаться к новому голосовому серверу
	bool ResumeRecording(LPCSTR alpcVoipServerAddress, LPCSTR alpcRoomName);

	//! Возвращает номер записи, которую он сейчас пишет
	unsigned int	GetRecordID(){ return recordID;}

	//! Возвращает номер записи, которую он сейчас слушает
	unsigned int	GetListenRecordID(){ return listenRecordID;}

	//! Посылает событие синхронизации
	bool ForceSynchronization( int uiUserID);

protected:
	//! Вызывается после соединения с сервером
	virtual void OnAfterConnected();

private:
	//! 
	bool StartRecordingBase( LPCSTR alpcVoipServerAddress, LPCSTR alpcRoomName, LPCSTR alpcUserName, unsigned int aRecordID);

protected:
	//! Адрес голосового записи
	CComString		voipServerIP;
	//! Имя комнаты, которую пишем
	CComString		roomName;
	//! Номер записи, которую пишем
	unsigned int	recordID;
	//! Имя моего пользователя
	CComString		userName;

	//! Номер записи, которую пишет кто-то, а мы получаем только события
	unsigned int	listenRecordID;

	//! Флаг состояния
	bool	recordingInProgress;
	bool	recordListeningInProgress;
};
#pragma once

#include "imessagehandler.h"
#include "iclientsession.h"
#include "serverInfo.h"
#include "../../../Common/ViskoeThread.h"
#include "messagesStorage.h"

class CAsyncConnection : public ViskoeThread::CThreadImpl<CAsyncConnection>
{
public:
	CAsyncConnection(	voip::voipServerSessionType aSessionType
						, oms::omsContext* aContext
						, cs::imessagehandler* apHandler
						, cs::isessionlistener* apSessionListener);
	~CAsyncConnection();

	//! Реализация CThreadImpl
public:
	DWORD Run();

public:
	//! Подключается к серверу
	bool	AsyncConnect( LPCTSTR alpcServerIP, unsigned int auiServerPort);

	//! Отключается от сервера
	voipError	Disconnect();

	//! Проверка на установленное соединение с сервером
	bool	IsAlreadyConnectedTo( LPCTSTR alpcServerIP, unsigned int auiServerPort);

	//! Подключен или нет
	bool	IsConnected();

	//! Отправка сообщения на сервер
	void Send(unsigned char aType, const unsigned char FAR *aData, int aDataSize);

protected:
	void SaveAsyncMessage(unsigned char aType, const unsigned char FAR *aData, int aDataSize);
	void ClearAsyncMessages();

	//! Адрес сервера записи
	//! Порт сервера записи
	cs::CServerInfo serverInfo;
	bool			connectionRequestIsActive;
	bool			connectionIsDestroyed;

	//! Сокетное соединение с сервером записи
	cs::iclientsession* clientSession;
	//! Обработчик сообщений от сервера
	cs::imessagehandler* messageHandler;
	//! Обработчик событий потери и восстановления связи
	cs::isessionlistener* sessionListener;
	//! Контекст, что тут еще сказать :)
	oms::omsContext* context;
	//! Соединение для воспроизведения или для записи
	voipServerSessionType		sessionType;

	//! Критическая секция на сессию
	CCriticalSection			csSession;
	CEvent						evConnect;
	MP_VECTOR<VoipMessage*>	asyncMessages;
};
#pragma once

#include "imessagehandler.h"
#include "iclientsession.h"
#include "serverInfo.h"
#include "../ConnectionListener.h"
#include "../messagetypes.h"
#include "../AsyncConnection.h"

class CRecordServerConnectionBase : cs::isessionlistener
{
public:
	CRecordServerConnectionBase (oms::omsContext* aContext, voip::voipServerSessionType aSessionType, cs::imessagehandler* apHandler, ivoiceconnectionlistender* apConnectionListener);
	~CRecordServerConnectionBase ();

	//! Реализация isessionlistener
public:
	virtual void OnConnectLost() {};
	virtual void OnConnectRestored() {};

public:
	//! Подключается к серверу
	voipError	Connect( LPCTSTR alpcServerIP, unsigned int auiServerPort);

	//! Отключается от сервера
	voipError	Disconnect();

	//! Возвращает true - если имеется соединение с сервером
	bool IsConnected();

protected:
	//! Вызывается после соединения с сервером
	virtual void OnAfterConnected(){ }

protected:
	CAsyncConnection serverConnection;

	//! Контекст, что тут еще сказать :)
	oms::omsContext* context;

	//! Обработка потери связи
	ivoiceconnectionlistender* connectionListener;
};
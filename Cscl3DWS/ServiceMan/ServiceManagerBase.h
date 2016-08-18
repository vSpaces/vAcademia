#pragma once
#include "serviceman.h"
#include "imessagehandler.h"
#include "iclientsession.h"
#include <map>
#include "ViskoeThread.h"
#include "ILogWriter.h"


class CServiceQuery
{
	BYTE *mpBuffer;
	unsigned long miSize;
public:
	unsigned int typeMessage;
	CServiceQuery(unsigned int aTypeMessage, BYTE *aBuffer, unsigned long dwSize)
	{
		typeMessage = aTypeMessage;
		miSize = dwSize;
		mpBuffer = MP_NEW_ARR( BYTE, dwSize);
		memcpy(mpBuffer, aBuffer, dwSize);
	}

	BYTE *GetData()
	{
		return mpBuffer;
	}

	unsigned int GetDataSize()
	{
		return miSize;
	}

	~CServiceQuery()
	{
		if(mpBuffer != NULL)
			MP_DELETE_ARR( mpBuffer);
	}
};

enum ECONNECTON_STATE	{ECS_DISCONNECTED, ECS_CONNECTING, ECS_CONNECTED};

enum ESEND_TYPE	{EST_SendSignalQuery, EST_SendQuery, EST_Send};

class CServiceManagerBase
{
public:
	CServiceManagerBase( omsContext* aContext);
	virtual ~CServiceManagerBase();

	// Подключись к серверу синхронизации
	virtual void Connect() = 0;
	// Отключись от сервера синхронизации
	virtual void Disconnect();
	// Удалить менеджер синхронизации
	virtual void Release();
	// Получить описание JSON сервиса
	virtual omsresult GetJSONService(unsigned int aRmmlID, const wchar_t* apwcServiceName);
	// Вызвать метод
	virtual omsresult ExecMethod(int requestID, unsigned int rmmlID, const wchar_t* apwcJSONMethodDescription);
	// Установить интерфейс, принимающий обратные вызовы от ServiceManager-а
	virtual bool SetCallbacks(service::IServiceManCallbacks* apCallbacks);
	// Подтвердить регистрацию сервиса на клиенте
	virtual omsresult RegisterService(int pendingID, unsigned int rmmlID);
	// удалить самого себя
	virtual void Destroy() = 0;

protected:
	omsContext* pContext;
	cs::iclientsession* pClientSession;
	service::IServiceManCallbacks* pCallbacks;

	void OnGetService( BYTE* aData, int aDataSize);
	void OnExecMethodResults( BYTE* aData, int aDataSize);
	void OnGetServiceByMethod( BYTE* aData, int aDataSize);
	void OnServiceSendNotify( BYTE* aData, int aDataSize);
	void OnServiceLoadingFailed( BYTE* aData, int aDataSize);

public:
	void OnConnectLost();
	void OnConnectRestored();
	void ConnectWorldServer(cs::imessagehandler*  handler, cs::isessionlistener *sessionListener);
	void Connect(cs::imessagehandler*  handler, cs::isessionlistener *sessionListener);
	void ConnectImpl(cs::imessagehandler*  handler, cs::isessionlistener *sessionListener, bool abIsWorldServer);

public:
	bool SendOnMyClientSession( BYTE aSendType, BYTE aType, const BYTE FAR *aData, int aDataSize);
protected:
	CCriticalSection	csFactory;

private:
	void SetSessionImpl(cs::iclientsession* apClientSession, cs::imessagehandler*  handler, cs::isessionlistener *sessionListener);
	bool BeforeConnect();
	ECONNECTON_STATE	connectionState;
	bool disconnectRequired;
};



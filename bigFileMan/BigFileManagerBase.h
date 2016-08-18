#pragma once
#include "serviceman.h"
#include "imessagehandler.h"
#include "iclientsession.h"
#include <map>
#include "Mutex.h"
#include "ILogWriter.h"


class CBigFileQuery
{
	BYTE *mpBuffer;
	unsigned long miSize;
public:
	unsigned int typeMessage;
	CBigFileQuery(unsigned int aTypeMessage, BYTE *aBuffer, unsigned long dwSize)
	{
		typeMessage = aTypeMessage;
		miSize = dwSize;
		mpBuffer = new BYTE[dwSize];
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

	~CBigFileQuery()
	{
		if(mpBuffer!=NULL)
			delete[] mpBuffer;
	}
};

class CBigFileManagerBase :
	public service::IBaseServiceManager
{
	bool isConnected;
public:
	CBigFileManagerBase( omsContext* aContext);
	virtual ~CBigFileManagerBase();

	// Подключись к серверу синхронизации
	virtual void Connect() = 0;
	// Отключись от сервера синхронизации
	virtual void Disconnect();
	// Удалить менеджер синхронизации
	virtual void Release();
	// Получить описание JSON сервиса
	virtual omsresult GetJSONService(unsigned int aRmmlID, const wchar_t* apwCBigFileName);
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

protected:
	/*typedef std::map<unsigned int, CBigFileQuery*> CBigFileQueryMap;
	CBigFileQueryMap oServiceQueryMap;
	
	void addServicePacket(unsigned int aTypeMessage, unsigned int aiPacketID, BYTE *aData, unsigned int dwSize);
	void clearServicePacket(unsigned int aiPacketID);

	HANDLE hReadySendEvent;
	
	static DWORD WINAPI SendAllNotSendingServiceMessage_( VOID* param);
	void SendAllNotSendingServiceMessage();*/
	//void SetCountRestoreService(unsigned int aiMinToSignal);
	CMutex mutex;
private:
	void AfterConnect(cs::imessagehandler*  handler, cs::isessionlistener *sessionListener);
	bool BeforeConnect();

};



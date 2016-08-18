#pragma once

#include "IDesktopMan.h"
#include "iclientsession.h"
#include "imessagehandler.h"
#include "SharingContext.h"
#include <map>

class CSharingConnection;

struct ISharingConnectionMessageHandler
{
	virtual void HandleMessage( CSharingConnection* aConnection, unsigned short aSessionID
								, BYTE aType, BYTE* aData, int aDataSize) = 0;
};

struct ISharingConnectionEvents
{
	virtual void OnConnected() = 0;
	virtual void OnConnectLost() = 0;
	virtual void OnConnectRestored() = 0;
};

//////////////////////////////////////////////////////////////////////////
class CSharingConnection : public cs::isessionlistener
							, public CEventSource<ISharingConnectionEvents>
							, public cs::imessagehandler
{
	friend class CSharingConnectionFactory;
	friend class CSharingSessionFactory;
public:
	CSharingConnection( SHARING_CONTEXT* apContext);
	virtual ~CSharingConnection();
	virtual int HandleMessage( BYTE aType, BYTE* aData, int aDataSize, DWORD aCheckSum);

public:
	void SetMessageHandler( ISharingConnectionMessageHandler* apISharingConnectionMessageHandler);
	void Send(unsigned short aSessionID, unsigned char aType, const unsigned char FAR *aData, int aDataSize);
	bool SendWithWait(unsigned short aSessionID, unsigned char aType, const unsigned char FAR *aData, int aDataSize);
	void SendQuery(unsigned short aSessionID, unsigned char aType, const unsigned char FAR *aData, int aDataSize);
	bool Send(std::string aSendingType, unsigned short aSessionID, unsigned char aType, const unsigned char FAR *aData, int aDataSize);
	desktop::sharingError Connect( LPCSTR alpcServerAddress, unsigned int aiSharingPort);
	bool IsConnected();
	void Unsubscribe(void * p);
	LPCSTR GetConnectionName()
	{
		return connectionName.GetBuffer();
	}

public:
	// isessionlistener
	void OnConnectLost();
	void OnConnectRestored();

protected:
	void SetConnectionName( LPCSTR alpcName)
	{
		if( alpcName)
			connectionName = alpcName;
	}
private:
	int GetWaitingMessageCount( unsigned short anID);

private:
	struct SessionMessage
	{
		unsigned short id;
		bool waiting;
	};
	CCriticalSection	csFactory;
	cs::iclientsession*	clientSession;
	SHARING_CONTEXT*	context;
	bool				connectIsLost;
	ISharingConnectionMessageHandler* messageHandler;
	CComString			connectionName;
	MP_VECTOR<SessionMessage> sendQueue;
};
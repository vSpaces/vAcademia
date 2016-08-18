#include "StdAfx.h"
#include "..\include\SharingConnection.h"
#include "serverInfo.h"
#include "..\include\CSHelper.h"

CSharingConnection::CSharingConnection( SHARING_CONTEXT* apContext): MP_VECTOR_INIT(sendQueue)
{
	context = apContext;
	clientSession = NULL;
	connectIsLost = false;
	messageHandler = NULL;
	csFactory.Init();
}

CSharingConnection::~CSharingConnection()
{
	// ??
	ATLASSERT( context->gcontext->mpComMan);
	if( context->gcontext->mpComMan)
		context->gcontext->mpComMan->DestroyServerSession( &clientSession);
	csFactory.Term();
}

void CSharingConnection::Send(unsigned short aSessionID, unsigned char aType, const unsigned char FAR *aData, int aDataSize)
{
	Send( "Send", aSessionID, aType, aData, aDataSize);
}

bool CSharingConnection::SendWithWait(unsigned short aSessionID, unsigned char aType, const unsigned char FAR *aData, int aDataSize)
{
	return Send( "SendWithWait", aSessionID, aType, aData, aDataSize);
}

void CSharingConnection::SendQuery(unsigned short aSessionID, unsigned char aType, const unsigned char FAR *aData, int aDataSize)
{
	Send( "SendQuery", aSessionID, aType, aData, aDataSize);
}

bool CSharingConnection::Send(std::string aSendingType, unsigned short aSessionID,  unsigned char aType, const unsigned char FAR *aData, int aDataSize)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	if( !clientSession)
		return false;

	// при отсутсвии связи не посылаем данные
	if( !IsConnected())
		return false;
	//

	unsigned int clientSessionQueueSize = clientSession->GetSendQueueSize();

	while (sendQueue.size() > clientSessionQueueSize)
	{
		sendQueue.erase(sendQueue.begin());
	}

	SessionMessage message;
	message.id = aSessionID;
	message.waiting = false;
	if (aSendingType == "Send")
	{
		sendQueue.push_back(message);
		clientSession->Send(aType, aData, aDataSize);
	}
	else if (aSendingType == "SendQuery")
	{
		sendQueue.push_back(message);
		clientSession->SendQuery(aType, aData, aDataSize);
	}
	else if (aSendingType == "SendWithWait")
	{
		if (GetWaitingMessageCount(aSessionID) >= 2)
			return false;
		message.waiting = true;
		sendQueue.push_back(message);
		clientSession->Send(aType, aData, aDataSize);
	}

	return true;
}

int CSharingConnection::GetWaitingMessageCount( unsigned short anID)
{
	int count = 0;
	for (unsigned int i=0; i<sendQueue.size(); i++)
	{
		if (sendQueue[i].id == anID && sendQueue[i].waiting)
			count++;
	}
	return count;
}

sharingError CSharingConnection::Connect( LPCSTR alpcServerAddress, unsigned int aiSharingPort)
{
	ATLASSERT( context);
	ATLASSERT( context->gcontext->mpComMan);
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS

	cs::CServerInfo	serverInfo;
	serverInfo.sIP = alpcServerAddress;
	serverInfo.tcpPort = aiSharingPort;
	context->gcontext->mpComMan->CreateSharingServerSession( &clientSession, &serverInfo);

	if ( clientSession == NULL)
		return ERROR_CONNECTION_FAILED;

	clientSession->SetSessionListener( this);
	clientSession->SetMessageHandler( this);
	
	Invoke<&ISharingConnectionEvents::OnConnected>();
	return ERROR_NOERROR;
}

bool CSharingConnection::IsConnected()
{
	return (clientSession!=NULL && !connectIsLost) ? true : false;
}

void CSharingConnection::OnConnectLost()
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	connectIsLost = true;
	Invoke<&ISharingConnectionEvents::OnConnectLost>();	
	if(context != NULL && context->gcontext->mpSM != NULL)
		context->gcontext->mpSM->OnServerDisconnected(rmml::ML_SERVER_MASK_SHARING, NULL);
}

void CSharingConnection::OnConnectRestored()
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	connectIsLost = false;
	Invoke<&ISharingConnectionEvents::OnConnectRestored>();	
	if(context != NULL && context->gcontext->mpSM != NULL)
		context->gcontext->mpSM->OnServerConnected(rmml::ML_SERVER_MASK_SHARING, true);
}

void CSharingConnection::SetMessageHandler( ISharingConnectionMessageHandler* apISharingConnectionMessageHandler)
{
	messageHandler = apISharingConnectionMessageHandler;
}

int CSharingConnection::HandleMessage( BYTE aType, BYTE* aData, int aDataSize, DWORD aCheckSum)
{
	if( !messageHandler)
		return 0;

	if (aDataSize < 4)
		return HM_NOT_ENOUGH_DATA;

	int nSize = (*(int*) aData);
	if (aDataSize - 4 < nSize)
		return HM_NOT_ENOUGH_DATA;

	unsigned short sessionID = *(unsigned short*)(aData + 4);
	messageHandler->HandleMessage( this, sessionID, aType, aData + 4 + sizeof(unsigned short), nSize - sizeof(unsigned short) );

	return nSize + 4;
}

void CSharingConnection::Unsubscribe(void * p)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	CEventSource<ISharingConnectionEvents>::Unsubscribe((ISharingConnectionEvents *)p);
}
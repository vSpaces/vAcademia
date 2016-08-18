#include "StdAfx.h"
#include "..\include\SharingSessionBase.h"
#include "..\protocol\SessionStartOut.h"
#include "..\include\SharingSession.h"

unsigned short CSharingSessionBase::INCORRECT_SESSION_ID = 0xFFFF;

CSharingSessionBase::CSharingSessionBase( SHARING_CONTEXT* apContext)
{
	isPausedSession = true;
	recordID = 0;
	isFileSharing = false;
	connection = NULL;
	context = apContext;
	sessionStarted = false;
	connectionConnected = false;
	sessionID = INCORRECT_SESSION_ID;
	m_isFirstStartFileSession = true;
	canRemoteAccess = false;
	seekFileTime = 0;
}

CSharingSessionBase::~CSharingSessionBase()
{
	// ??
	if( connection)
		connection->Unsubscribe(this);
}

sharingError CSharingSessionBase::StartSession()
{
	if( !sessionStarted)
	{
		sessionStarted = true;
		if(connectionConnected)
			OnSessionStarted();
	}
	return ERROR_NOERROR;
}

void CSharingSessionBase::StopSession(bool withNotificationServer)
{
	if( sessionStarted)
	{
		sessionStarted = false;
		if (withNotificationServer)
		SendTerminateSessionRequest();
		OnSessionStopped();
	}
}

void CSharingSessionBase::Send(unsigned char aType, const unsigned char FAR *aData, int aDataSize)
{
	if (aType == 17){
		if (context->gcontext->mpLogWriter)
		{
			context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN] ST_RemoteControl");
		}
	}

	if( connection)
		connection->Send(sessionID, aType, aData, aDataSize);
}

bool CSharingSessionBase::SendWithWait(unsigned char aType, const unsigned char FAR *aData, int aDataSize)
{
	if( connection)
		return connection->SendWithWait(sessionID, aType, aData, aDataSize);
	return false;
}

void CSharingSessionBase::SendQuery(unsigned char aType, const unsigned char FAR *aData, int aDataSize)
{
	if( connection)
		connection->SendQuery(sessionID, aType, aData, aDataSize);
}

void CSharingSessionBase::SetConnection( CSharingConnection*	aConnection)
{
	ATLASSERT( aConnection);
	if( !aConnection)
		return;

	if( connection)
		connection->Unsubscribe( this);

	connection = aConnection;
	connection->Subscribe( this);

	if( connection->IsConnected())
		OnConnected();
}

void CSharingSessionBase::SetFileSharing()
{
	isFileSharing = true;
}

bool CSharingSessionBase::IsFileSharingSession()
{
	return isFileSharing;
}
bool CSharingSessionBase::IsConnectionConnected()
{
	return connectionConnected;
}
void CSharingSessionBase::OnConnected()
{
	connectionConnected = true;
	if( sessionStarted)
		OnSessionStarted();
}

void CSharingSessionBase::OnConnectLost()
{
	
}

void CSharingSessionBase::OnConnectRestored()
{
	
}

void CSharingSessionBase::SetSessionID( unsigned short aSessionID)
{
	sessionID = aSessionID;
}

unsigned short CSharingSessionBase::GetSessionID()
{
	return sessionID;
}

void CSharingSessionBase::SendTerminateSessionRequest()
{
	ATLASSERT( FALSE);
}

void CSharingSessionBase::SetUnicSessionName(LPCSTR alpcName)
{
	// имя сессии уникальное для сервера
	unicSessionName = alpcName;
}

LPCSTR CSharingSessionBase::GetUnicSessionName()
{
	return unicSessionName.GetBuffer();
}

void CSharingSessionBase::SetSeekFileTime(unsigned int aSeekTime)
{
	seekFileTime = aSeekTime;
}

unsigned int CSharingSessionBase::GetSeekFileTime()
{
	return seekFileTime;
}

void CSharingSessionBase::SetServerVersion(unsigned int aServerVersion) 
{
	serverVersion = aServerVersion;
}

unsigned int CSharingSessionBase::GetServerVersion()
{
	return serverVersion;
}

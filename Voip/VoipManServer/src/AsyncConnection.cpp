// VoipMan.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "serverInfo.h"
#include "../../include/AsyncConnection.h"


/************************************************************************/
// Функции записи звука  
/************************************************************************/
CAsyncConnection::CAsyncConnection(voip::voipServerSessionType aSessionType
								   , oms::omsContext* aContext
								   , cs::imessagehandler* apHandler
								   , cs::isessionlistener* apSessionListener)
		: sessionType( aSessionType)
		, context(aContext)
		, messageHandler(apHandler)
		, sessionListener(apSessionListener)
{
	evConnect.Create(NULL, TRUE, FALSE, NULL);
	csSession.Init();

	clientSession = NULL;

	connectionRequestIsActive = false;
	connectionIsDestroyed = false;

	ViskoeThread::CThreadImpl<CAsyncConnection>::Start();
}

CAsyncConnection::~CAsyncConnection()
{
	// посылаем сигнал на остановку
	SignalStop();
	// отпускаем поток, если он ждет событие
	evConnect.SetEvent();
	// ждем пока поток завершится
	Stop();
	// отключаем сокет
	Disconnect();
	// закрываем сессию
	csSession.Term();
}

//! Подключается к серверу
bool	CAsyncConnection::AsyncConnect( LPCTSTR alpcServerIP, unsigned int auiServerPort)
{
	Disconnect();

	ATLASSERT( !IsConnected());

	csSession.Lock();
	ClearAsyncMessages();
	serverInfo.sIP = alpcServerIP;
	serverInfo.tcpPort = auiServerPort;
	connectionRequestIsActive = true;
	connectionIsDestroyed = false;
	evConnect.SetEvent();
	csSession.Unlock();

	return true;
}

//! Отключается от сервера
voipError CAsyncConnection::Disconnect()
{
	csSession.Lock();
	if( clientSession)
	{
		clientSession->SetSessionListener(NULL);
		clientSession->SetMessageHandler(NULL);
		ATLASSERT( context);
		ATLASSERT( context->mpComMan);
		if( context->mpComMan)
		{
			context->mpComMan->DestroyServerSession( &clientSession);
			clientSession = NULL;
		}
	}
	connectionIsDestroyed = true;
	csSession.Unlock();
	return ERROR_NOERROR;
}

//! Проверка на установленное соединение с сервером
bool	CAsyncConnection::IsAlreadyConnectedTo( LPCTSTR alpcServerIP, unsigned int auiServerPort)
{
	csSession.Lock();
	if( alpcServerIP == serverInfo.sIP && auiServerPort == serverInfo.tcpPort)
	{
		if( clientSession != NULL && clientSession->GetStatus() == 1/*connected*/)
		{
			csSession.Unlock();
			return true;
		}
	}
	csSession.Unlock();
	return false;
}


void CAsyncConnection::SaveAsyncMessage(unsigned char aType, const unsigned char FAR *aData, int aDataSize)
{
	MP_NEW_P3( message, VoipMessage, aType, aData, aDataSize);	
	asyncMessages.push_back( message);
}

void CAsyncConnection::ClearAsyncMessages()
{
	for( unsigned int i=0; i<asyncMessages.size(); i++)
	{
		VoipMessage* voipMessage = asyncMessages[i];
		MP_DELETE_UNSAFE( voipMessage);
	}
	asyncMessages.clear();
}

void CAsyncConnection::Send(unsigned char aType, const unsigned char FAR *aData, int aDataSize)
{
	csSession.Lock();
	if( clientSession != NULL)
		clientSession->Send( aType, aData, aDataSize);
	else
	{
		SaveAsyncMessage( aType, aData, aDataSize);
	}
	csSession.Unlock();
}

// Connection thread
DWORD CAsyncConnection::Run()
{
	while (!ShouldStop())
	{
		evConnect.WaitForEvent();
		if( ShouldStop())
			break;

		// Запоминаем параметры запрашиваемого подключения
		cs::CServerInfo serverInfoToConnect;
		csSession.Lock();
		ATLASSERT( !clientSession);
		connectionRequestIsActive = false;
		serverInfoToConnect = serverInfo;
		csSession.Unlock();

		// Подключаемся.. длительная операция
		cs::iclientsession* newClientSession = NULL;
		if( sessionType == VSST_RECORDING)
		{
			context->mpComMan->CreateVoipRecordServerSession( &newClientSession, &serverInfoToConnect);
		}
		else	if( sessionType == VSST_PLAYING)
		{
			context->mpComMan->CreateVoipPlayServerSession( &newClientSession, &serverInfoToConnect);
		}
		else	if( sessionType == VSST_SELF_RECORDING)
		{
			context->mpComMan->CreateVoipSelfRecordingServerSession( &newClientSession, &serverInfoToConnect);
		}

		clientSession = newClientSession;

		if ( newClientSession != NULL)
		{
			newClientSession->SetMessageHandler( messageHandler);
			newClientSession->SetSessionListener( sessionListener);

			if( sessionListener)
			{
				if( newClientSession->GetStatus() == 0/*not connected*/)
					sessionListener->OnConnectLost();
				else
					sessionListener->OnConnectRestored();
			}
		}

		// Проверяем, что за время подключения не пришло новых запросов
		csSession.Lock();
		if( !connectionRequestIsActive && !connectionIsDestroyed)
		{
			// successfully connected
			if( clientSession)
			{
				for( unsigned int i=0; i<asyncMessages.size(); i++)
				{
					VoipMessage* message = asyncMessages[i];
					clientSession->Send( message->type, message->data, message->dataSize);
					MP_DELETE_UNSAFE( message);
				}
				asyncMessages.clear();
			}
			evConnect.ResetEvent();
		}
		else
		{
			clientSession = NULL;

			// new request received
			newClientSession->SetMessageHandler( NULL);
			newClientSession->SetSessionListener( NULL);
			context->mpComMan->DestroyServerSession( &newClientSession);

			if( connectionIsDestroyed)
				evConnect.ResetEvent();
		}
		csSession.Unlock();
	}

	return 0;
}

//! Подключен или нет
bool CAsyncConnection::IsConnected()
{
	if (clientSession == NULL || clientSession->GetStatus() == 0)
		return false;
	return true;
}
// ClientSession.cpp: implementation of the CClientSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Ws2tcpip.h>
#include <QOS.h>
#include "ThreadAffinity.h"
#include "ClientSession.h"
#include "consts.h"
#include "../mdump/mdump.h"
#include "CrashHandlerInThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "QueryPackageIn.h"
#include "QueryPackageOut.h"

using namespace ResManProtocol;

#define LOCAL_IP_STRING	("127.0.0.1")

#define DESIRED_WINSOCK_VERSION 0x0101  // we'd like winsock ver 1.1...
#define MINIMUM_WINSOCK_VERSION 0x0001  // ...but we'll take ver 1.0

typedef INT	SOCKERR;        			// A socket error code

#define LOCK_TIMEOUT			10000

unsigned int CClientSession::STATUS_CONNECTED = 1;
unsigned int CClientSession::STATUS_NOTCONNECTED = 0;
#ifdef DEBUG_SENDPACKETINFO
bool bEmulSignError = false;
int iCountValidMessEmulSignError = 1;
int iNumValidMessEmulSignError = 0;
#endif

#define RECV_BUF_SIZE 1024

// раскоментить дефайн для эмуляции разрыва при получении большого пакета с данными о файлах ROOT_MULTI_FILE_INFO
//#define LOGIN_ERROR_EMULATION_RES_SERVER
// раскоментить дефайн для эмуляции разрыва при получении пакета с ответом на пакет авторизации
//#define LOGIN_ERROR_EMULATION_LOGIN_SERVER

// Конструктор
CClientSession::CClientSession( LPCSTR aSessionName, USHORT aSignature):
				MP_MAP_INIT( oSendedQueriesMap),
				MP_MAP_INIT( oSyncQueryMap),
				MP_LIST_INIT( sendedInfoPackets),
				MP_LIST_INIT( receivedInfoPackets),
				MP_LIST_INIT( oSendQueue),
				MP_LIST_INIT( oSendUDPQueue),
				MP_MAP_INIT( oSendUDPQueueMap)
				
{
	pThreadContextObject = NULL;
	m_stackSize = 64000;
	sckRecvUDPServer = INVALID_SOCKET;
	sckSendUDPServer = INVALID_SOCKET;
	bRunning = true;
	hSendEvent = CreateEvent( NULL, TRUE, FALSE, NULL);
	hSendUDPEvent = CreateEvent( NULL, TRUE, FALSE, NULL);
	hConnectEvent = CreateEvent( NULL, TRUE, FALSE, NULL);
	dwBufferCapacity = 0;
	dwBufferSize = 0;
	pBuffer = NULL;
	//lQueryNum = 0;
	ulSessionID = 0;
	m_emulateSignatureError = false;
	sSessionKey = "";
	bConnected = false;
	bErrorConnection = false;
	hSendThread = NULL;
	hSendUDPThread = NULL;
	hRecvThread = NULL;
	hRecvUDPThread = NULL;
	pImmediatelySendData = NULL;
	pImmediatelySendUDPData = NULL;
	InitializeCriticalSection( &csSendQueue);
	InitializeCriticalSection( &csSendQueueStat);
	InitializeCriticalSection( &csSendUDPQueue);
	InitializeCriticalSection( &csBuffer);
	InitializeCriticalSection( &csBufferStat);
	pMessageHandler = NULL;
	pSessionListener = NULL;
	pServerInfo = NULL;

	sSessionName = aSessionName;
	uSignature = aSignature;
	messageID = 0;
	bLogDetailedInformation = false;
	bFirstSendAfterConnectionWasExecuted = false;
	bFirstReceiveAfterConnectionWasExecuted = false;
	m_connectionIsFreezed = false;

	m_numbersOfMessage = 0;
	m_countHandleDataCalls = 0;

	m_handle = GetCurrentThread();

	receiveSpeed = 0;
	sendSpeed = 0;

	instantaneousReceiveSpeed = 0;
	instantaneousSendSpeed = 0;

	receivedSize = 0;
	sendedSize = 0;

	createTime = GetTickCount();

	m_pLW = NULL;

	mbWriteLogEnabled = false;

	m_connectionSessionID = 0;

	m_connectionManager = NULL;
	m_MaxQueueSize = -1;
	oldType = 0;
	m_errorCode = 0;
	pThreadContextObjectForThreadSaver = NULL;
	m_sequentialAlivePacketsCount = 0;
	m_pInfoMan = NULL;
	m_lastRecvMsgID = 0;
	iSessionInfoIndex = 0;
	pSessionsInfoMan = NULL;
}

void CClientSession::SetSignature(USHORT _uSignature)
{
	uSignature = _uSignature;
}

USHORT CClientSession::GetSignature()
{
	return uSignature;
}

BOOL CClientSession::IsThreadTerminated()
{
	DWORD exitCode = 0;

	BOOL res = GetExitCodeThread(m_handle, &exitCode);

	if (!res)
	{
		m_errorCode = GetLastError();
	}

	return (exitCode != STILL_ACTIVE); 
}

DWORD CClientSession::GetExitCode()
{
	DWORD exitCode = 0;

	BOOL res = GetExitCodeThread(m_handle, &exitCode);

	if (!res)
	{
		m_errorCode = GetLastError();
	}

	return exitCode;
}

// Деструктор
CClientSession::~CClientSession()
{
	mutexLog.lock();
	if (m_pLW != NULL)
	{
		CLogValue log("~CClientSession");
		m_pLW->WriteLn(log);
	}
	mutexLog.unlock();

	messageID = 0;
	pMessageHandler = NULL;
	pSessionListener = NULL;
	mutexLog.lock();
	m_pLW = NULL;
	mutexLog.unlock();
	Close();
	bRunning = false;
	if (pImmediatelySendData != NULL)
		MP_DELETE( pImmediatelySendData);

	pImmediatelySendData = NULL;

	threadContextObjectLock.lock();
	if (pThreadContextObject != NULL)
	{
		pThreadContextObject->reset();
		pThreadContextObject->destroy();
		pThreadContextObject = NULL;
	}
	threadContextObjectLock.unlock();

	ReleaseThreads();

	DeleteCriticalSection( &csBuffer);
	DeleteCriticalSection( &csSendUDPQueue);
	DeleteCriticalSection( &csSendQueue);
	CloseHandle( hSendEvent);
	hSendEvent = NULL;
	if ( pBuffer != NULL)
	{
		MP_DELETE_ARR( pBuffer);
		pBuffer = NULL;
	}
}

void CClientSession::SetSessionID( unsigned long long aSessionID)
{
	ulSessionID = aSessionID;
}

void CClientSession::SetSessionKey( CComString aSessionKey)
{
	sSessionKey = aSessionKey;
}

void CClientSession::ReleaseThreads()
{
	bRunning = false;

	if(hSendUDPThread!=NULL)
	{
		SetEvent( hSendUDPEvent);
		WaitForSingleObject( hSendUDPThread, 2000);
		mutexClose.lock();
		if(hSendUDPThread!=NULL)
		{
			CloseHandle(hSendUDPThread);
			hSendUDPThread = NULL;
		}
		mutexClose.unlock();		
	}

	if(hRecvUDPThread!=NULL)
	{
		WaitForSingleObject( hRecvUDPThread, 2000);
		mutexClose.lock();
		if(hRecvUDPThread!=NULL)
		{
			CloseHandle(hRecvUDPThread);
			hRecvUDPThread = NULL;
		}
		mutexClose.unlock();
	}

	if(hSendThread!=NULL)
	{
		SetEvent( hSendEvent);
		WaitForSingleObject( hSendThread, 2000);

		mutexClose.lock();
		if(hSendThread!=NULL)
		{
			//StopThread(hSendThread);
			CloseHandle(hSendThread);
			hSendThread = NULL;
		}
		mutexClose.unlock();
	}

	if( hRecvThread != NULL)
	{
		WaitForSingleObject( hRecvThread, 2000);
		mutexClose.lock();
		if(hRecvThread != NULL)
		{
			//StopThread(hRecvThread);
			CloseHandle(hRecvThread);
			hRecvThread = NULL;
		}
		mutexClose.unlock();
	}
}
// Открывает сессию
bool CClientSession::Open(cs::CServerInfo *aServerInfo, unsigned long long aSessionID, CComString aSessionKey, ILogWriter *am_pLW, CConnectionManager* aConnectionManager, bool aIsAsynchConnect)
{
	mutexLog.lock();
	m_pLW = am_pLW;
	mutexLog.unlock();
	if (m_pLW != NULL)
	{
		m_pLW->WriteLn("CClientSession::Open ", aServerInfo->sIP.GetBuffer(), " ", aServerInfo->tcpPort);
	}

	Close();
	bRunning = false;
	ReleaseThreads();
	if (m_pLW != NULL)
	{
		m_pLW->WriteLn("CClientSession::ReleaseThreads ended ", aServerInfo->sIP.GetBuffer(), " ", aServerInfo->tcpPort);
	}
	serverInfo = *aServerInfo;
	pServerInfo = &serverInfo;
	ulSessionID = aSessionID;
	sSessionKey = aSessionKey;
	if (m_connectionManager == NULL)
	{
		m_connectionManager = aConnectionManager;
		m_connectionManager->Subscribe(this);
		m_connectionSessionID = m_connectionManager->getSessionID();
	}

	CreateSocket(aIsAsynchConnect);

	return true;
}

bool CClientSession::IsIPAddress( LPCSTR alpHost)
{
	ATLASSERT( alpHost);
	int strLength = strlen( alpHost);
	if( strLength == 0)	return false;
	for ( int i=0; i<strLength; i++)
	{
		char ch = alpHost[i];
		if( ch!='.' || ch < '0' || ch > '9')	return false;
	}
	return true;
}

CComString CClientSession::GetIPViaHostName( LPCSTR alpHost)
{
	ATLASSERT( alpHost);
	if( !alpHost)	return LOCAL_IP_STRING;

	CComString shost(alpHost);
	WORD wVersionRequested;
	WSADATA wsaData;
	PHOSTENT hostinfo;
	CComString	ip( alpHost);
	wVersionRequested = MAKEWORD( 2, 0 );

	if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{
		if((hostinfo = gethostbyname(alpHost)) != NULL)
		{
			ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
		}
		WSACleanup( );
	}
	ATLASSERT( !ip.IsEmpty());
	if( ip.IsEmpty())	
		return LOCAL_IP_STRING;
	return ip;
}

bool CClientSession::CreateSocket(bool aIsAsynchConnect)
{
	ATLASSERT( pServerInfo);
	if( pServerInfo->sIP.IsEmpty())	return false;

	SOCKERR serr;
	WSADATA wsadata;

	//  Initialize the sockets library
	serr = WSAStartup(DESIRED_WINSOCK_VERSION, &wsadata);
	//serr = WSAStartup(wVersionRequested, &wsadata);
	if (serr != 0) 
	{		
		mutexLog.lock();
		if (m_pLW != NULL)
		{
			CLogValue log("Socket initialization error");
			m_pLW->Write(log);
		}
		mutexLog.unlock();
		return false; 
	}

	if (wsadata.wVersion < MINIMUM_WINSOCK_VERSION) 
	{
		mutexLog.lock();
		if (m_pLW != NULL)
		{
			CLogValue log("The minimum WinSock version - 1.0");
			m_pLW->Write(log);
		}
		mutexLog.unlock();
		return false; 
	}

	if( !IsIPAddress( pServerInfo->sIP))
		pServerInfo->sIP = GetIPViaHostName( pServerInfo->sIP);
	
	ATLASSERT( bRunning == false);
	ATLASSERT( hSendThread == NULL);
	ATLASSERT( hRecvThread == NULL);

	threadContextObjectLock.lock();
	if (pThreadContextObject != NULL)
	{
		pThreadContextObject->reset();
		pThreadContextObject->destroy();
		pThreadContextObject = NULL;
	}
	threadContextObjectLock.unlock();

	// запускаем потоки
	// ждем создания предыдущих потоков
	threadContextObjectLock.lock();
	MP_NEW_V4(pThreadContextObject, ThreadContextObject, (cm::IConnectionManager*)m_connectionManager, m_pLW, this, 3);
	pThreadContextObjectForThreadSaver = pThreadContextObject;
	bRunning = true;
	
	CSimpleThreadStarter sendThreadStarter(__FUNCTION__);
	sendThreadStarter.SetStackSize(m_stackSize);
	sendThreadStarter.SetRoutine(sendThread);
	sendThreadStarter.SetParameter(pThreadContextObjectForThreadSaver);
	hSendThread = sendThreadStarter.Start();	

	CSimpleThreadStarter recvThreadStarter(__FUNCTION__);
	recvThreadStarter.SetStackSize(m_stackSize);
	recvThreadStarter.SetRoutine(recvThread);
	recvThreadStarter.SetParameter(pThreadContextObjectForThreadSaver);
	hRecvThread = recvThreadStarter.Start();

	threadContextObjectLock.unlock();

	// если подключение синхронное ждем окончания попытки подключения в потоке получения данных
	if (!aIsAsynchConnect)
	{
		ResetEvent(hConnectEvent);
		WaitForSingleObject(hConnectEvent, MAX_WAIT_CONNECT_TIME);
	}

	return bConnected;
}

int CClientSession::Connect(int aConnectResult)
{
	if (m_handle != GetCurrentThread())
	{
		mutexLog.lock();
		if (m_pLW != NULL)
		{
			CLogValue log("Connect:: Синхронный доступ в CClientSession::Connect()");
			m_pLW->Write(log);
		}
		mutexLog.unlock();
	}

	bFirstSendAfterConnectionWasExecuted = false;
	bFirstReceiveAfterConnectionWasExecuted = false;
	m_sequentialAlivePacketsCount = 0;

	if ( aConnectResult == CONNECTION_NO_ERROR)
	{
		bConnected = true;
		bErrorConnection = false;
		if(ulSessionID > 0/*for valid sessions*/ || ulSessionID == SERVICE_WORLD_SESSION_ID)
		{
			// Send Handshakes
			int len = sSessionKey.GetLength();
			if (sSessionKey != "")
			{
				unsigned int aDataSize = sizeof( ulSessionID) + sizeof(int) + len*2;
				unsigned int aDataRemains = aDataSize;
				//BYTE* pNewData = new BYTE[ aDataSize];
				BYTE* pNewData = MP_NEW_ARR(BYTE, aDataSize);
				rtl_memcpy(pNewData, aDataRemains, (BYTE*)&ulSessionID, sizeof( ulSessionID));
				aDataRemains -= sizeof( ulSessionID);

				rtl_memcpy(pNewData+sizeof( ulSessionID), aDataRemains, (BYTE*)&len, sizeof(len));
				aDataRemains -= sizeof(len);
				USES_CONVERSION;
				rtl_memcpy(pNewData+sizeof( ulSessionID)+sizeof(int), aDataRemains, A2W(sSessionKey.GetBuffer()), len*2);
				SendImmediately( 254, pNewData, sizeof( ulSessionID) + sizeof(len)+len*2);

				MP_DELETE_ARR( pNewData);
			}
			else
			{
				SendImmediately( 0, (BYTE*)&ulSessionID, sizeof( ulSessionID));
			}

			ResendLostQueries();

			if (pSessionListener != NULL)
				pSessionListener->OnConnectRestored();

			mutexLog.lock();
			if(m_pLW!=NULL)
			{
				CLogValue log("[RESTORE AFTER ERROR] [SESSION] ", sSessionName.GetString(), "  Connection with Server - ", pServerInfo->sIP.GetString(), ":", (int)pServerInfo->tcpPort, " was restored, pSessionListener = ", (unsigned int)pSessionListener);
				m_pLW->WriteLnLPCSTR(log.GetData());
			}
			mutexLog.unlock();
		}
		if(pServerInfo->udpRecvPort>0 && pServerInfo->udpSendPort>0)
			CreateUDPSockets();
	}
	else
	{
		mutexLog.lock();
		if (m_pLW != NULL)
		{
			CLogValue log("[SESSION] CClientSession::Connect failed for session ", sSessionName.GetString());
			m_pLW->WriteLnLPCSTR(log.GetData());
		}
		mutexLog.unlock();
		bConnected = false;	
	}

	return aConnectResult;
}

//************************************
// Method:    Close
// FullName:  CClientSession::Close
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: bool bError
// Закрывает сокетную сессию. Флаг bError - что сессия закрыта в результате определения ошибки сокетного соединения,
// например когда recv возвращает значение < 0
//************************************
void CClientSession::Close(bool bError)
{
	mutexLog.lock();
	if (m_pLW != NULL && pServerInfo != NULL)
	{
		CLogValue log("CClientSession::Close for server = ", pServerInfo->sIP.GetBuffer(), " port ", pServerInfo->tcpPort);
		m_pLW->WriteLn(log);
	}
	mutexLog.unlock();

	mutexRestore.lock();
	if(!bError)
		ulSessionID = 0;
	mutexRestore.unlock();

	mutexLog.lock();
	if( IsDetailedLogMode())
	{
		m_pLW->WriteLn("ml2, sn = ", sSessionName.GetBuffer());
	}
	mutexLog.unlock();

	mutex.lock();

	// если сессия сейчас соединенияется, то надо прервать соединение
	if (m_connectionManager != NULL)
		m_connectionManager->abortConnect(m_connectionSessionID);

	bool IsConnectionValid = false;
	threadContextObjectLock.lock();

	bool success = false;
	int startTime = GetTickCount();
	while ((!success) || (GetTickCount() - startTime > LOCK_TIMEOUT))
	{
		if (pThreadContextObject != NULL)
		{
			if (!pThreadContextObject->IsDestroyingNow())
			{
				if (pThreadContextObject->TryToLock())
				{
					IsConnectionValid = pThreadContextObject->pConnection != NULL && pThreadContextObject->pConnection->isSocketValid();
					pThreadContextObject->Unlock();
					success = true;
				}
				else
				{
					Sleep(1);
				}
			}
			else
			{
				success = true;
			}
		}
		else
		{
			success = true;
		}
	}
	threadContextObjectLock.unlock();

	SignalAllWaitingsEvents();

	bConnected = false;

	if (IsConnectionValid)
	{
		if(sckSendUDPServer!=INVALID_SOCKET)
		{
			shutdown( sckSendUDPServer, SD_BOTH);
			closesocket( sckSendUDPServer);
			sckSendUDPServer = INVALID_SOCKET;
		}
		if(sckRecvUDPServer!=INVALID_SOCKET)
		{
			shutdown( sckRecvUDPServer, SD_BOTH);
			closesocket( sckRecvUDPServer);
			sckRecvUDPServer = INVALID_SOCKET;
		}

		threadContextObjectLock.lock();
		if (pThreadContextObject != NULL)
		{
			pThreadContextObject->Lock();
			if (pThreadContextObject->pConnection != NULL)
				pThreadContextObject->pConnection->shutdown();
			pThreadContextObject->Unlock();
		}
		threadContextObjectLock.unlock();

		dwBufferSize = 0;
		dwBufferCapacity = 0;

		std::map<WORD, CSendData *>::iterator iter2 = oSendUDPQueueMap.begin(), iter2_end = oSendUDPQueueMap.end();
		for ( ; iter2 != iter2_end; ++iter2)
		{
			CSendData *sendData = (CSendData *) iter2->second;
			if ( sendData)
			{
				MP_DELETE( sendData);
				sendData = NULL;
			}
		}
		oSendUDPQueueMap.clear();
		
		if (bError)
		{
			bErrorConnection = true;
			
			if (pSessionListener != NULL)
				pSessionListener->OnConnectLost();
			mutex.unlock();
			mutexLog.lock();
			if(m_pLW!=NULL)
			{
				CLogValue log("[ERROR][SESSION] ", sSessionName.GetString(), "  Connection with Server - ", pServerInfo->sServerName.GetString(), " (", pServerInfo->sIP.GetString(), ":", (int)pServerInfo->tcpPort, ") was lost");
				m_pLW->WriteLnLPCSTR(log.GetData());
			}
			mutexLog.unlock();
			return;
		}
		if(hSendEvent!=NULL)
			SetEvent( hSendEvent);
	}

	EnterCriticalSection( &csSendQueue);
	std::map<WORD, CSyncQuery *>::iterator iter = oSyncQueryMap.begin(), iter_end = oSyncQueryMap.end();
	for ( ; iter != iter_end; ++iter)
	{
		CSyncQuery *syncQuery = (CSyncQuery *) iter->second;
		if ( syncQuery)
		{
			MP_DELETE( syncQuery);
			syncQuery = NULL;
		}
	}
	oSyncQueryMap.clear();
	
	// !! try catch - не помог. Лучше пусть протекает. Надо сделать флажок, который удаление идентифицирует - и проверять oSendedQueriesMap.

	// TODO - тут иногда падает при вызове delete sendData; На бету - ставлю try catch - Alex
	
	std::map<WORD, CSendData *>::iterator iter2 = oSendedQueriesMap.begin(), iter2_end = oSendedQueriesMap.end();
	//try
	{
		for ( ; iter2 != iter2_end; iter2++)
		{
			CSendData *sendData = (CSendData *) iter2->second;
			if ( sendData != NULL)
			{
#ifdef DEBUG_SENDPACKETINFO	
				ATLASSERT( !sendData->bDeleted);
				sendData->bDeleted = 1;
#else
				MP_DELETE( sendData);
#endif
				sendData = NULL;
			}
		}
	}
	//catch(...)
	{
		//??
	}
	
	oSendedQueriesMap.clear();
	LeaveCriticalSection( &csSendQueue);
	mutex.unlock();
	mutexLog.lock();
	if( IsDetailedLogMode())
	{
		m_pLW->WriteLn("mul2, sn = ", sSessionName.GetBuffer());
	}
	mutexLog.unlock();
}

CClientSession::CSendData::CSendData()
{
	bQuery = false;
	bType = 0;
	wPacketID = 0;
	dwSize = 0;
	pData = NULL;
#ifdef DEBUG_SENDPACKETINFO	
	bDeleted = 0;
#endif
}

CClientSession::CSendData::CSendData( USHORT aSignature, WORD aPacketID,
									 int aMessageSize, BYTE aType, const BYTE FAR *aData, int aDataSize, bool aQuery)
{
#ifdef DEBUG_SENDPACKETINFO	
	bDeleted = 0;
#endif
	bType = aType;
	wPacketID = aPacketID;
	DWORD dwHeaderSize = 5; 
	DWORD dwBodySize = 0;
	if (aMessageSize < 0)
	{
		dwHeaderSize += 4;
		dwBodySize = aDataSize;
	}
	else
		dwBodySize = (DWORD)aMessageSize;
	
	dwSize = dwHeaderSize + dwBodySize;
	pData = MP_NEW_ARR(BYTE, dwSize);

	int i = 0;
	
	*((WORD* )(pData + i)) = aSignature;
	i += sizeof( aSignature);
	*((BYTE* )(pData + i)) = aType;
	i += sizeof( aType);
	// номер пакета
	*((WORD* )(pData + i)) = aPacketID;
	i += sizeof( aPacketID);
	//
	
	if (aMessageSize < 0)
	{
		*((DWORD*)(pData + i)) = dwBodySize;
		i += sizeof( dwBodySize);
	}
	if (dwBodySize > 0)
		rtl_memcpy( pData + i, dwSize-i, aData, dwBodySize);
	bQuery = aQuery;
}

CClientSession::CSendData::~CSendData()
{
	if (pData != NULL)
		MP_DELETE_ARR( pData);
}
// 0 - STATUS_DISCONNECTED
// 1 - STATUS_CONNECTED
int CClientSession::GetStatus()
{
	if (bConnected)
		return (int)STATUS_CONNECTED;
	else
		return (int)STATUS_NOTCONNECTED;
}
int CClientSession::GetSendQueueSize()
{	
	if (m_handle != GetCurrentThread())
	{
		mutexLog.lock();
		if (m_pLW != NULL)
		{
			CLogValue log("GetSendQueueSize:: Синхронный доступ в CClientSession::Connect()");
			m_pLW->Write(log);
		}
		mutexLog.unlock();
	}

	EnterCriticalSection( &csSendQueue);
		int sizeOfQueue =  oSendQueue.size();
	LeaveCriticalSection( &csSendQueue);
	
	return sizeOfQueue;
}

// Отправляет сообщения
void CClientSession::Send( BYTE aType, const BYTE FAR *aData, int aDataSize)
{
	SendMessageImpl( aType, aData, aDataSize);
}

WORD CClientSession::IncMessageID()
{
	EnterCriticalSection( &csSendQueue);
	messageID++;
	if(messageID==0)
		messageID++;
	LeaveCriticalSection( &csSendQueue);
	return messageID;
}

void CClientSession::SendSignalQuery(unsigned char aType, const unsigned char FAR *aData, int aDataSize)
{
	RemoveSpecifiedTypeMessages( aType);

	WORD messageIdToSend = IncMessageID();
	SendMessageImpl( aType, aData, aDataSize, true, messageIdToSend, EPM_LIFO);
}

// Удаляет запрос из очередей
void CClientSession::RemoveQueryByType(unsigned char aType)
{
	RemoveSpecifiedTypeMessages( aType);
}

// Отправляет сообщения c сохранением отправленных для ожидания ответа на них
void CClientSession::SendQuery( BYTE aType, const BYTE FAR *aData, int aDataSize)
{
	WORD messageIdToSend = IncMessageID();
	SendMessageImpl( aType, aData, aDataSize, true, messageIdToSend);
}

void MakeDumpOnBigMessage( int aiDataSize)
{
	if( aiDataSize > 100000000 /*100 mb*/)
		make_dump( false);
}

void CClientSession::RemoveSpecifiedTypeMessages(BYTE aType)
{
	EnterCriticalSection( &csSendQueue);
	CSendQueue::iterator it = oSendQueue.begin();
	for (; it != oSendQueue.end(); )
	{
		if( (*it)->bType == aType)
		{
			CSendData* pSendData = *it;
			if( pSendData->wPacketID != 0)
			{
				CSendQueueMapIter iter = oSendedQueriesMap.find( pSendData->wPacketID);
				if(iter != oSendedQueriesMap.end())
				{
					oSendedQueriesMap.erase( iter);
				}

				mutex.lock();
				CSyncQueryMap::iterator iterSq = oSyncQueryMap.find( pSendData->wPacketID);
				if(iterSq != oSyncQueryMap.end())
				{
					oSyncQueryMap.erase( iterSq);
				}
				mutex.unlock();
			}
			MP_DELETE( pSendData);
			it = oSendQueue.erase( it);
		}
		else
		{
			it++;
		}
	}
	if( pImmediatelySendData != NULL && pImmediatelySendData->bType == aType)
	{
		MP_DELETE( pImmediatelySendData);
		pImmediatelySendData = NULL;
	}

	LeaveCriticalSection( &csSendQueue);
}

void CClientSession::SetMaxQueueSize( int aMaxQueueSize)
{
	m_MaxQueueSize = aMaxQueueSize;
}

int CClientSession::QueueSizeIsBig()
{
	if( m_MaxQueueSize == -1)
		return 0;

	if ((int)oSendQueue.size() > m_MaxQueueSize)
	{
		return (oSendQueue.size() - m_MaxQueueSize);
	}	

	return 0;
}

void CClientSession::DecreaseQueueSize( int aCountElement)
{	
	if( aCountElement < 1 || aCountElement > (int)oSendQueue.size())
		return;
	CSendQueue::iterator iter = oSendQueue.begin();
	while( aCountElement > 0)
	{
		iter++;
		aCountElement--;
	}
	oSendQueue.erase( oSendQueue.begin(), iter);
}

void CClientSession::SendMessageImpl(BYTE aType, const BYTE FAR *aData, int aDataSize, bool abQuery, WORD aMessageID, EPacketProcessMethod aProcessMethod)
{
	int iMessageSize = GetOutMessageSize( aType);
	if (iMessageSize > aDataSize)
	{
		mutexLog.lock();
		if (m_pLW != NULL)
		{
			CLogValue log("Wrong out message size.");
			m_pLW->Write(log);
		}
		mutexLog.unlock();
		return;
	}

	MakeDumpOnBigMessage( aDataSize);

	//CSendData* pSendData = new CSendData( uSignature, aMessageID, iMessageSize, aType, aData, aDataSize, abQuery);
	MP_NEW_P7( pSendData, CSendData, uSignature, aMessageID, iMessageSize, aType, aData, aDataSize, abQuery);

	EnterCriticalSection( &csSendQueue);	
	if( aProcessMethod == EPM_FIFO)
		oSendQueue.push_back( pSendData);
	else if( aProcessMethod == EPM_LIFO)
		oSendQueue.push_front( pSendData);
	else
	{
		ATLASSERT( FALSE);
	}
	DecreaseQueueSize( QueueSizeIsBig());

	SetEvent( hSendEvent);
	LeaveCriticalSection( &csSendQueue);
}

void CClientSession::ChangeConnection()
{
	Close(true);
}

void CClientSession::EmulateBreak( /*int auConnectionMask*/)
{
	m_emulateSignatureError = true;
}

void CClientSession::SendImmediately(BYTE aType, const BYTE FAR *aData, int aDataSize)
{
	int iMessageSize = GetOutMessageSize( aType);
	if (iMessageSize > aDataSize)
	{
		mutexLog.lock();
		if (m_pLW != NULL)
		{
			CLogValue log("SendImmediately:: Wrong out message size.");
			m_pLW->Write(log);
		}
		mutexLog.unlock();
		return;
	}

	WORD auMessageIdToSend = IncMessageID();

	EnterCriticalSection( &csSendQueue);
	if (pImmediatelySendData != NULL)
	{
		MP_DELETE( pImmediatelySendData);
	}

	MakeDumpOnBigMessage( aDataSize);

	//pImmediatelySendData = new CSendData( uSignature, auMessageIdToSend, iMessageSize, aType, aData, aDataSize);
	MP_NEW_V6( pImmediatelySendData, CSendData, uSignature, auMessageIdToSend, iMessageSize, aType, aData, aDataSize);
	SetEvent( hSendEvent);
	LeaveCriticalSection( &csSendQueue);
}

void CClientSession::SendImmediatelyQuery(BYTE aType, const BYTE FAR *aData, int aDataSize)
{
	int iMessageSize = GetOutMessageSize( aType);
	if (iMessageSize > aDataSize)
	{
		mutexLog.lock();
		if (m_pLW != NULL)
		{
			CLogValue log("SendImmediatelyQuery:: Wrong out message size.");
			m_pLW->Write(log);
		}
		mutexLog.unlock();
		return;
	}

	WORD auMessageIdToSend = IncMessageID();

	EnterCriticalSection( &csSendQueue);
	if (pImmediatelySendData != NULL)
	{
		MP_DELETE( pImmediatelySendData);
	}

	MakeDumpOnBigMessage( aDataSize);

	//pImmediatelySendData = new CSendData( uSignature, auMessageIdToSend, iMessageSize, aType, aData, aDataSize); 
	MP_NEW_V6( pImmediatelySendData, CSendData, uSignature, auMessageIdToSend, iMessageSize, aType, aData, aDataSize);
	oSendedQueriesMap.insert( std::pair<WORD, CSendData *>(messageID, pImmediatelySendData));
	SetEvent( hSendEvent);
	LeaveCriticalSection( &csSendQueue);
}

// Устанавливает режим детализированного лога
void CClientSession::SetDetailedLogModeEnabled( bool abEnabled)
{
	bLogDetailedInformation = abEnabled;
}

// Нужно и можно ли писать детальный лог
bool CClientSession::IsDetailedLogMode( )
{
	if( !bLogDetailedInformation)
		return false;

	if( m_pLW == NULL)
		return false;

	return true;
}

//************************************
// Method:    ResendLostQueries
// FullName:  CClientSession::ResendLostQueries
// Access:    private 
// Returns:   void
// Qualifier: Перепосылает запросы, отправляемые через SendQuery - если на них не был получен ответ.
//************************************
void CClientSession::ResendLostQueries()
{
	EnterCriticalSection( &csSendQueue);
	CSendQueueMap::iterator it = oSendedQueriesMap.begin();
	for (; it != oSendedQueriesMap.end(); it++)
	{
		CSendData *pSendData = it->second;
#ifdef DEBUG_SENDPACKETINFO	
		if( !pSendData->bDeleted)
#endif			
			oSendQueue.push_back( pSendData);
	}
	oSendedQueriesMap.clear();
	DecreaseQueueSize( QueueSizeIsBig());
	SetEvent( hSendEvent);
	LeaveCriticalSection( &csSendQueue);
}

void CClientSession::SignalAllWaitingsEvents()
{
	mutexLog.lock();
	if( IsDetailedLogMode())
	{
		m_pLW->WriteLn("SetAllSynchEvents Begin ", sSessionName.GetBuffer());
	}
	mutexLog.unlock();

	std::map<WORD, CSyncQuery *>::iterator iter = oSyncQueryMap.begin(), iter_end = oSyncQueryMap.end();
	for ( ; iter != iter_end; ++iter)
	{
		CSyncQuery *syncQuery = (CSyncQuery *) iter->second;
		if ( syncQuery)
		{
			syncQuery->signal();
		}
	}

	mutexLog.lock();
	if( IsDetailedLogMode())
	{
		m_pLW->WriteLn("SetAllSynchEvents End ", sSessionName.GetBuffer());
	}
	mutexLog.unlock();
}

CClientSession::CSyncQuery*	CClientSession::GetWaitingSyncQueryWithoutLock( WORD aiQueryID)
{
	CSyncQueryMap::iterator it = oSyncQueryMap.find( aiQueryID);
	if( it != oSyncQueryMap.end())
	{
		CSyncQuery* foo = it->second;
		return foo;
	}
	return NULL;
}

int CClientSession::SendAndReceive(BYTE aType, const BYTE FAR *aData, DWORD aDataSize, BYTE **aAnswerData, DWORD *aAnswerDataSize, DWORD timeout /* = INFINITE */, int* aQueryID/* = NULL*/)
{
	mutexLog.lock();
	if( IsDetailedLogMode())
	{
		m_pLW->WriteLn("SendAndReceive1: aType = ", aType, " , sn = ", sSessionName.GetBuffer(), ", bConnected = ", bConnected);
	}

	if( IsDetailedLogMode())
	{
		m_pLW->WriteLn("SendAndReceive1.2");
	}

	if( IsDetailedLogMode())
	{
		m_pLW->WriteLn("ml4, sn = ", sSessionName.GetBuffer());
	}
	mutexLog.unlock();
	mutex.lock();

	int aCountHandleDataCalls = m_countHandleDataCalls;
	WORD auMessageIdToSend = IncMessageID();

	CSyncQuery *pSyncQuery = MP_NEW( CSyncQuery);
	pSyncQuery->reset();
	oSyncQueryMap.insert( std::pair<int, CSyncQuery *>( auMessageIdToSend, pSyncQuery));	
	/*CSyncQuery *_tempSyncQuery = (CSyncQuery *) oSyncQueryMap[ auMessageIdToSend];
	ATLASSERT( _tempSyncQuery == pSyncQuery);*/
	SendMessageImpl( aType, aData, aDataSize, true, auMessageIdToSend);

	WORD _lQueryNum = auMessageIdToSend;
	mutex.unlock();

	mutexLog.lock();
	if( IsDetailedLogMode())
	{
		m_pLW->WriteLn("mul4, sn = ", sSessionName.GetBuffer());
	}

	if( IsDetailedLogMode())
	{
		m_pLW->WriteLn("SendAndReceive2: _lQueryNum = ", _lQueryNum, ", sn = ", sSessionName.GetBuffer());
	}
	mutexLog.unlock();

	if (aQueryID != NULL)
		*aQueryID = _lQueryNum;

	// Ждем ответа от сервера
	int res = 0;
	while ( true)
	{
		res = WaitForSingleObject( pSyncQuery->event, timeout);
		mutex.lock();
		if( bErrorConnection)
		{
			mutex.unlock();

			mutexLog.lock();
			if(m_pLW!=NULL)
				m_pLW->WriteLn("HM_SYNCH_DATA105 connection error when waiting for", sSessionName.GetBuffer());
			mutexLog.unlock();
			break;
		}
		if ( res == WAIT_TIMEOUT)
		{
			// тут надо проверить, что на этот запрос был хотя бы небольшой ответ
			CSyncQuery* foo = GetWaitingSyncQueryWithoutLock( _lQueryNum);

			if( foo != NULL)
			{
				// если были данные по пакету ИЛИ (были хоть какие-то данные И обрабатывается какое-то предыдущее сообщение )
				if( foo->resetUpdated() 
					|| ((aCountHandleDataCalls != m_countHandleDataCalls) && (m_lastRecvMsgID <= _lQueryNum)))
				{
					// были получены данные за таймаут. Ждем дальше
					mutex.unlock();

					mutexLog.lock();
					if(m_pLW!=NULL)
						m_pLW->WriteLn("HM_SYNCH_DATA103 SLOW NETWORK CONNECTION  ID = ", _lQueryNum," for ", sSessionName.GetBuffer(), " m_lastRecvMsgID = ", m_lastRecvMsgID);
					mutexLog.unlock();
					continue;
				}
			}

			mutex.unlock();

			mutexLog.lock();
			if(m_pLW!=NULL)
			{
				if( foo != NULL)
					m_pLW->WriteLn("HM_SYNCH_DATA106 pSynchData IS found after timeout ID = ", _lQueryNum," for ", sSessionName.GetBuffer());
				else
					m_pLW->WriteLn("HM_SYNCH_DATA106 pSynchData NOT found after timeout ID = ", _lQueryNum," for ", sSessionName.GetBuffer());
			}
			mutexLog.unlock();

			break;
		}
		mutex.unlock();
		break;
	}

	mutex.lock();

	CSyncQuery *_pSyncQuery = GetWaitingSyncQueryWithoutLock( _lQueryNum);
	if ( res == WAIT_TIMEOUT || bErrorConnection)
	{
		if(_pSyncQuery != NULL)
		{
			MP_DELETE( _pSyncQuery); 
			_pSyncQuery = NULL;
			pSyncQuery = NULL;
			oSyncQueryMap.erase( _lQueryNum);

			mutexLog.lock();
			if( IsDetailedLogMode())
			{
				m_pLW->WriteLn("[ERROR][SESSION] delq num = ", _lQueryNum);
			}
			mutexLog.unlock();
		}
		if(res == WAIT_TIMEOUT && bErrorConnection)
		{
			if(m_pLW!=NULL)
				m_pLW->WriteLn("[ERROR][SESSION] ", sSessionName.GetString(), "  SIGNAL TO ALL EVENT ABOUT DISCONNECTED");

			SignalAllWaitingsEvents();

		}
		mutex.unlock();
		if( IsDetailedLogMode())
		{
			m_pLW->WriteLn("mul5, sn = ", sSessionName.GetBuffer()," num = ", _lQueryNum);
		}
		return qeTimeOut;
	}	

	if(_pSyncQuery == NULL)
	{
		oSyncQueryMap.erase( _lQueryNum);	
		mutex.unlock();

		mutexLog.lock();
		if( IsDetailedLogMode())
			m_pLW->WriteLn("[ERROR][SESSION] ", sSessionName.GetString(), "  _pSyncQuery==NULL");

		if( IsDetailedLogMode())
			m_pLW->WriteLn("mul6, sn = ", sSessionName.GetBuffer());
		mutexLog.unlock();

		return qeReadError;
	}

	ATLASSERT( _pSyncQuery == pSyncQuery);

	*aAnswerDataSize = _pSyncQuery->pData->getSize();
	*(aAnswerData) = MP_NEW_ARR(BYTE, *aAnswerDataSize);
	if(!_pSyncQuery->pData->readData(aAnswerData, *aAnswerDataSize, FALSE))
	{
		MP_DELETE( _pSyncQuery); 
		pSyncQuery = NULL;
		oSyncQueryMap.erase( _lQueryNum);	
		mutex.unlock();

		mutexLog.lock();
		if(m_pLW!=NULL)
			m_pLW->WriteLn("[ERROR][SESSION] ", sSessionName.GetString(), "  qeReadError");

		if( IsDetailedLogMode())
			m_pLW->WriteLn("mul7, sn = ", sSessionName.GetBuffer(), " num = ", _lQueryNum);
		mutexLog.unlock();
		return qeReadError;
	}

	MP_DELETE( pSyncQuery); 
	pSyncQuery = NULL;	
	oSyncQueryMap.erase( _lQueryNum);
	mutex.unlock();
	if( IsDetailedLogMode())
		m_pLW->WriteLn("mul8, sn = ", sSessionName.GetBuffer(), " num = ", _lQueryNum);

	return qeNoError;		
}

void CClientSession::RemovePacketFromSendedQueriesMap( CSendData* pSendData)
{
	if( pSendData->wPacketID != 0)
	{
		CSendQueueMapIter iter = oSendedQueriesMap.find( pSendData->wPacketID);
		if(iter != oSendedQueriesMap.end())
		{
			oSendedQueriesMap.erase( iter);
		}
	}
}

void CClientSession::PushPacketsToMultiPacket(bool& abImmediateSended)
{
	abImmediateSended = false;

	if (pImmediatelySendData != NULL)
	{
		oMultiPacket.AddImmediatelyData( pImmediatelySendData);
		RemovePacketFromSendedQueriesMap( pImmediatelySendData);
		pImmediatelySendData = NULL;
		abImmediateSended = true;
	}

	while (!oSendQueue.empty() && oMultiPacket.AddData( *oSendQueue.begin()))
	{
		CSendData* pSendData = *oSendQueue.begin();
		RemovePacketFromSendedQueriesMap( pSendData);
		oSendQueue.pop_front();
	}
}

void CClientSession::RestorePacketsFromMultiPacket()
{
	CSendData *pSendData = oMultiPacket.PopImmediatelyData();
	if (pSendData != NULL)
	{
		// Если была отправка с приоритетом - восстановим пает, если его не заменили новым
		if( !pImmediatelySendData)
		{
			pImmediatelySendData = pSendData;
		}
		else
		{
#ifdef DEBUG_SENDPACKETINFO	
			ATLASSERT( !pSendData->bDeleted);
			pSendData->bDeleted = 2;
#else
			MP_DELETE( pSendData);
#endif
		}
	}

	/*int dwSizeAll = 0;
	if( pSendData != NULL)
		dwSizeAll = pSendData->dwSize;*/

	while ( (pSendData = oMultiPacket.PopData()) != NULL)
	{		
		oSendQueue.push_front( pSendData);
	}
	DecreaseQueueSize( QueueSizeIsBig());

	/*dwSizeAll += oMultiPacket.GetDataSize();

	CSendQueue::iterator iter = oSendQueue.begin();
	for (;iter != oSendQueue.end();iter++)
	{
		CSendData* pSendData1 = *iter;
		dwSizeAll += pSendData1->dwSize;
	}

	int dwSizeMapAll = 0;
	CSendQueueMapIter iter1 = oSendedQueriesMap.begin();
	if(iter1 != oSendedQueriesMap.end())
	{
		CSendData* pSendData1 = iter1->second;
		dwSizeMapAll += pSendData1->dwSize;
	}*/

	ATLASSERT( oMultiPacket.GetDataSize() == 0);
}

void CClientSession::RemovePacketsFromMultiPacket()
{
	CSendData *pSendData = oMultiPacket.PopImmediatelyData();
	if (pSendData != NULL)
		HandleSentPacket( pSendData);

	while ( (pSendData = oMultiPacket.PopData()) != NULL)
		HandleSentPacket( pSendData);

	ATLASSERT( oMultiPacket.GetDataSize() == 0);
}

void CClientSession::HandleSentPacket( CSendData* pSendData)
{
	if( !pSendData)
		return;

	if (mbWriteLogEnabled)
	{
		std::string session = GetSessionName();
		m_pInfoMan->TraceSessionPacketInfo( session, pSendData->bType, pSendData->dwSize, true);
	}

	if (pSendData->bQuery)
	{
#ifdef DEBUG_SENDPACKETINFO
		int n = 0;
		if(pSendData->bDeleted)
		{
			n = 1;
		}
		ATLASSERT( !pSendData->bDeleted);
#endif
		EnterCriticalSection( &csSendQueue);
		oSendedQueriesMap.insert( std::pair<WORD, CSendData *>(pSendData->wPacketID, pSendData));
		LeaveCriticalSection( &csSendQueue);
	}
	else
	{
#ifdef DEBUG_SENDPACKETINFO
		pSendData->bDeleted = 3;
		mutexLog.lock();
		if(m_pLW!=NULL)
		{
			CLogValue log("[CLSESSION] del num packet=", pSendData->wPacketID);
			m_pLW->WriteLnLPCSTR(log.GetData());
		}
		mutexLog.unlock();
#else
		MP_DELETE( pSendData);
#endif
	}
}

DWORD WINAPI CClientSession::sendThread( VOID* aParams)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();

	ThreadContextObject* context = (ThreadContextObject*)aParams;

	CoInitialize(NULL);

	context->Lock();
	context->sendThreadCreated();
	if (context->isValid())
	{
		if (context->pLW)
		{
			DWORD threadId = GetCurrentThreadId();
			CLogValue log("CClientSession::sendThread thread created. ThreadID = ", threadId);
			context->pLW->WriteLn(log);
		}
	}
	context->Unlock();

	while (true)
	{
		bool isRuning = false;
		HANDLE sendEvent = INVALID_HANDLE_VALUE;

		context->Lock();
		if (context->isValid())
		{
			isRuning = context->pSession->bRunning;
			sendEvent = context->pSession->hSendEvent;
		}
		context->Unlock();

		if (!isRuning)
			break;

		if (sendEvent == INVALID_HANDLE_VALUE || WaitForSingleObject( sendEvent, INFINITE) == WAIT_FAILED)
			break;

		bool	abImmediatelySend = false;

		////////////////////////////////////////////////////////////////////////////////////
		// 1. Выбираем пакет для отправки. После выбора этого пакета не должно быть ни в каких очередях.
		// Для них он - пропал.
		bool isContinue = false;
		bool isBreak = false;
		context->Lock();
		if (context->isValid())
		{
			EnterCriticalSection( &context->pSession->csSendQueue);

			context->pSession->PushPacketsToMultiPacket( abImmediatelySend);

			LeaveCriticalSection( &context->pSession->csSendQueue);

			// 1.1. Если не нашли чего посылать, то возвращаемся и ждем дальше
			if ( context->pSession->oMultiPacket.IsEmpty())
			{
				context->pSession->mutexRestore.lock();
				if(context->pSession->ulSessionID == INCORRECT_SESSION_ID 
					&& !context->pSession->bConnected 
					&& !context->pSession->bErrorConnection)
				{
					isBreak = true;
				}
				context->pSession->mutexRestore.unlock();

				if (!isBreak)
				{
					EnterCriticalSection( &context->pSession->csSendQueue);
					if (context->pSession->oSendQueue.empty())
						ResetEvent( context->pSession->hSendEvent);
					LeaveCriticalSection( &context->pSession->csSendQueue);

					isContinue = true;
				}
			}
		}
		context->Unlock();

		if (isBreak)
			break;

		if (isContinue)
			continue;
		// end of 1



		////////////////////////////////////////////////////////////////////////////////////
		// 2. Проверяем сокет
		bool IsConnetionValid = false;
		context->Lock();
		if (context->isValid())
			IsConnetionValid = context->pConnection != NULL && context->pConnection->isSocketValid();
		context->Unlock();

		if (!IsConnetionValid)
		{
			// вернем неотправленный пакет в очереди
			bool isIncorrectSessionID = false;
			context->Lock();
			if (context->isValid())
			{
				EnterCriticalSection( &context->pSession->csSendQueue);

				context->pSession->RestorePacketsFromMultiPacket();

				LeaveCriticalSection( &context->pSession->csSendQueue);

				context->pSession->mutexRestore.lock();
				if( context->pSession->ulSessionID == INCORRECT_SESSION_ID)
					isIncorrectSessionID = true;
				context->pSession->mutexRestore.unlock();

				if (!isIncorrectSessionID)
				{
					EnterCriticalSection( &context->pSession->csSendQueue);
					if ( context->pSession->oSendQueue.empty())
						ResetEvent( context->pSession->hSendEvent);
					LeaveCriticalSection( &context->pSession->csSendQueue);
				}
			}
			context->Unlock();

			if (isIncorrectSessionID)
				break;

			Sleep( 1000);
			continue;
		}
		// end of 2

		// 3. Посылаем пакет

		int socketErrorCode = 0;

		byte* pMultiPacketData = NULL;
		unsigned int uMultiPacketDataSize = 0;
		
		bool createMPResult = false;
		context->Lock();
		if (context->isValid())
			createMPResult = context->pSession->oMultiPacket.CreateMultiPacket( &pMultiPacketData, &uMultiPacketDataSize);
		context->Unlock();

		if( createMPResult)
		{
			bool isFirstSendAfterConnectionWasExecuted = false;
			context->Lock();
			if (context->isValid())
				isFirstSendAfterConnectionWasExecuted = context->pSession->bFirstSendAfterConnectionWasExecuted;
			context->Unlock();

			if( !isFirstSendAfterConnectionWasExecuted)
			{
				Sleep( SOCKS_INTERVAL);

				context->Lock();
				if (context->isValid())
					context->pSession->bFirstSendAfterConnectionWasExecuted = true;
				context->Unlock();
			}

			IConnection* connection = NULL;

			context->Lock();
			if (context->isValid())
			{
				context->sending.lock();
				connection = context->pConnection;
			}
			context->Unlock();

			if (connection != NULL)
				socketErrorCode = connection->send( (char*)pMultiPacketData, uMultiPacketDataSize, 0);

			context->Lock();
			if (context->isValid())
				context->sending.unlock();
			context->Unlock();
		}

		// 4. Оцениваем результат посылки
		context->Lock();
		if (context->isValid())
		{
			if (socketErrorCode == SOCKET_ERROR)
			{
				// возникла ошибка передачи
				socketErrorCode = WSAGetLastError();
				context->pSession->mutexRestore.lock();
				if( context->pSession->ulSessionID > 0 && !context->pSession->bErrorConnection)
				{
					if( context->pLW)
					{
						CLogValue log("[ERROR][SESSION] ", context->pSession->sSessionName.GetString(), "  SOCKET_ERROR at send. ErrorCode = ", socketErrorCode, "  data size==", uMultiPacketDataSize);
						context->pLW->WriteLnLPCSTR(log.GetData());
					}
					context->pSession->mutexRestore.unlock();
					context->pSession->Close(true);
				}
				else
				{
					context->pSession->mutexRestore.unlock();
				}

				// вернем неотправленный пакет в очереди
				EnterCriticalSection( &context->pSession->csSendQueue);

				context->pSession->RestorePacketsFromMultiPacket();

				LeaveCriticalSection( &context->pSession->csSendQueue);
			}
			else
			{
				// данные успешно переданы

				// 1. Пишем статистику
				EnterCriticalSection( &context->pSession->csSendQueueStat);
				context->pSession->sendedSize += uMultiPacketDataSize;
				packetInfo packet(uMultiPacketDataSize, GetTickCount());
				context->pSession->sendedInfoPackets.push_back(packet);

				DWORD time = GetTickCount();
				std::list<packetInfo>::iterator it = context->pSession->sendedInfoPackets.begin();
				while ( it != context->pSession->sendedInfoPackets.end() && (time - it->time) > 5000)
				{
					context->pSession->sendedInfoPackets.pop_front();
					it = context->pSession->sendedInfoPackets.begin();
				}
				LeaveCriticalSection( &context->pSession->csSendQueueStat);

				// 2.
				context->pSession->RemovePacketsFromMultiPacket();

				EnterCriticalSection( &context->pSession->csSendQueue);
				if (context->pSession->oSendQueue.empty())
					ResetEvent( context->pSession->hSendEvent);
				LeaveCriticalSection( &context->pSession->csSendQueue);
			}
		}
		context->Unlock();
	}

	context->destroy();

	CoUninitialize();
	return 0;
}

DWORD WINAPI CClientSession::recvThread( VOID* aParams)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();

	ThreadContextObject* context = (ThreadContextObject*)aParams;

	context->Lock();
	{
		context->recvThreadCreated();
		if (context->isValid())
		{
			if (context->pLW)
			{
				DWORD threadId = GetCurrentThreadId();
				CLogValue log("CClientSession::recvThread thread created. ThreadID = ", threadId);
				context->pLW->WriteLn(log);
			}
		}
	}
	context->Unlock();

	CoInitialize(NULL);

	IConnection* connection = NULL;
	BYTE btBuff[ RECV_BUF_SIZE];
	int bytesRecved = 0;

	int connectResult = CONNECTION_SOCKET_ERROR;

	cm::IConnectionManager* connectionManager = NULL;
	CServerInfo* pServerInfo1 = NULL;
	DWORD connectionSessionID = 0;
	context->Lock();
	if (context->isValid())
	{
		connectionManager = context->pConnectionManager;
		pServerInfo1 = context->pSession->pServerInfo;
		connectionSessionID = context->pSession->m_connectionSessionID;
	}
	context->Unlock();

	if (connectionManager != NULL && pServerInfo1 != NULL)
		connection = connectionManager->getConnection(pServerInfo1, connectResult, connectionSessionID);

	if (connection != NULL)
	{
		// на валидность не проверяем, т.к. нужно установить ссылку на connection чтобы он удалился при удалении context
		context->Lock();
		context->pConnection = connection;
		context->Unlock();
	}

	context->Lock();
	if (context->isValid())
	{
		context->pSession->Connect(connectResult);
		SetEvent(context->pSession->hConnectEvent);
	}
	context->Unlock();

	bool isRuning = false;
	bool IsSocketValid = false;
	while (true)
	{
		isRuning = false;
		context->Lock();
		if (context->isValid())
		{
			isRuning = context->pSession->bRunning;
			IsSocketValid = context->pConnection != NULL && context->pConnection->isSocketValid();
		}
		context->Unlock();

		if (!isRuning)
			break;

		if (!IsSocketValid)
		{
			Sleep( 1000);

			bool isConnectionIsFreezed = false;
			context->Lock();
			if (context->isValid())
				isConnectionIsFreezed = context->pSession->m_connectionIsFreezed;
			context->Unlock();

			if(isConnectionIsFreezed)
				continue;

			bool isIncorrectSessionID = false;
			context->Lock();
			if (context->isValid())
			{
				context->pSession->mutexRestore.lock();
				if( context->pSession->ulSessionID == INCORRECT_SESSION_ID)
					isIncorrectSessionID = true;
				context->pSession->mutexRestore.unlock();
			}
			context->Unlock();
			if (isIncorrectSessionID)
				break;

			context->Lock();
			if (context->isValid())
			{
				if ( context->pSession->bErrorConnection || !context->pSession->bConnected) // alex, добавил bConnected - иначе не восстанавливал связь при изначально неработающем сервере
				{
					if (context->pConnection != NULL)
					{
						context->pConnection->shutdown();
						context->sending.lock();
						context->pConnection->destroy();
						context->pConnection = NULL;
						context->sending.unlock();
					}
				}
			}
			context->Unlock();

			context->Lock();
			if (context->isValid())
			{
				connectionManager = context->pConnectionManager;
				pServerInfo1 = context->pSession->pServerInfo;
				connectionSessionID = context->pSession->m_connectionSessionID;
			}
			context->Unlock();

			if (connectionManager != NULL && pServerInfo1 != NULL)
				connection = connectionManager->getConnection(pServerInfo1, connectResult, connectionSessionID);

			if (connection != NULL)
			{
				// на валидность не проверяем, т.к. нужно установить ссылку на connection чтобы он удалился при удалении context
				context->Lock();
				context->pConnection = connection;
				context->Unlock();
			}

			context->Lock();
			if (context->isValid())
				context->pSession->Connect(connectResult);
			context->Unlock();

			continue;
		}

		bool isFirstReceiveAfterConnectionWasExecuted = false;
		context->Lock();
		if (context->isValid())
			isFirstReceiveAfterConnectionWasExecuted = context->pSession->bFirstReceiveAfterConnectionWasExecuted;
		context->Unlock();

		if( !isFirstReceiveAfterConnectionWasExecuted)
		{
			Sleep( SOCKS_INTERVAL);

			context->Lock();
			if (context->isValid())
				context->pSession->bFirstReceiveAfterConnectionWasExecuted = true;
			context->Unlock();
		}

		connection = NULL;
		context->Lock();
		if (context->isValid())
			connection = context->pConnection;
		context->Unlock();

		bytesRecved = 0;
		if (connection != NULL)
			bytesRecved = connection->recv( (char*)&btBuff[0], RECV_BUF_SIZE, 0);

		if (bytesRecved <= 0)
		{
			context->Lock();
			if (context->isValid())
			{
				context->pSession->mutexRestore.lock();
				IsSocketValid = context->pConnection != NULL && context->pConnection->isSocketValid();
				if( context->pSession->ulSessionID > 0 && (!context->pSession->bErrorConnection || IsSocketValid))
				{
					if (context->pLW != NULL && context->pSession->pServerInfo != NULL)
					{
						int iError = WSAGetLastError();
						CLogValue log("[ERROR][SESSION] ", context->pSession->sSessionName.GetString(), "  Socket error: ", iError, " (", context->pSession->pServerInfo->sIP.GetString(), ":", (int)context->pSession->pServerInfo->tcpPort, ")");
						context->pLW->WriteLnLPCSTR(log.GetData());
					}
					context->pSession->mutexRestore.unlock();
					context->pSession->Close(true);
				}
				else
				{
					context->pSession->mutexRestore.unlock();
				}
			}
			context->Unlock();

			continue;
		}

		if (bytesRecved > 0)
		{
			CClientSession* pSession = NULL;
			context->Lock();
			if (context->isValid())
				pSession = context->pSession;
			context->Unlock();

			// здесь может упасть, если handleReceiveData обрабатывался долго - завис, а сессию уже удалили, т.е. на закрытии плеера
			if (pSession != NULL)
				pSession->handleReceiveData( (BYTE*)&btBuff[0], bytesRecved);
		}
	}

	context->destroy();

	CoUninitialize();
	return 0;
}

void CClientSession::SetMessageHandler(cs::imessagehandler* pMesHandler)
{
	pMessageHandler = pMesHandler;
}

void CClientSession::SetSessionListener(cs::isessionlistener* aListener)
{
	pSessionListener = aListener;
}

bool CClientSession::IsSyncServerSession()
{
	return (uSignature == 0x4554 && sSessionName == "SyncServerSession") ? true : false;
}

void CClientSession::CheckLongPacketDataLost( BYTE* aData, unsigned int aDataSize)
{
	if( DataIsAlivePacket( aData, aDataSize))
	{
		m_sequentialAlivePacketsCount++;
		if( m_sequentialAlivePacketsCount == 2)
		{
			if( dwBufferSize > IN_ALIVE_MESSAGE_SIZE)
			{
				mutexLog.lock();
				if( IsDetailedLogMode())
				{
					m_pLW->WriteLn("Data portion is lost for ", sSessionName.GetBuffer(), " . Two sequencial ALIVE messages received with non empty buffer. Emulate break");
				}
				mutexLog.unlock();

				EmulateBreak(/*0*/);
			}
		}
	}
	else
	{
		m_sequentialAlivePacketsCount = 0;
	}
}

bool CClientSession::DataIsAlivePacket( BYTE* aData, int aDataSize)
{
	if( aDataSize != IN_ALIVE_MESSAGE_SIZE)
		return false;

	if( uSignature != (*(USHORT*)aData))
		return false;

	if( aData[ 2] != IN_ALIVE_MESSAGE)
		return false;

	return true;
}

#ifdef LOGIN_ERROR_EMULATION_RES_SERVER
static unsigned int gg = 0;
static unsigned int ff = 0;
#endif

#ifdef LOGIN_ERROR_EMULATION_LOGIN_SERVER
static unsigned int ls_gg = 0;
static unsigned int ls_ff = 0;
#endif

void CClientSession::handleReceiveData( BYTE* aData, unsigned int aDataSize)
{
	m_countHandleDataCalls++;

	EnterCriticalSection( &csBuffer);

#ifdef LOGIN_ERROR_EMULATION_RES_SERVER
	if( !ff && pServerInfo && pServerInfo->tcpPort == 11123)
	{
		if(dwBufferSize > 100000 && aDataSize != 13)
		{
			if( gg == 0)
			{
				LeaveCriticalSection( &csBuffer);
				return;
			}
		}
	}
#endif

#ifdef LOGIN_ERROR_EMULATION_LOGIN_SERVER
	if( !ls_ff && pServerInfo && pServerInfo->tcpPort == 13333)
	{
		if(aDataSize > 21)
		{
			if( ls_gg == 0)
			{
				aData[21] = aData[21] + 1;
			}
		}
	}
#endif

	if( IsDetailedLogMode())
	{
		//m_pLW->WriteLn("handleReceiveData for resSocket aDataSize = ", aDataSize, " recv speed is = ",  GetReceiveSpeed(), " send speed is = ", GetSendSpeed());
	}

	// Проверяет на то, что в длинном пакете не дошли данные. В этом случае идут только аливы, а размер буфера имеет больший размер.
	CheckLongPacketDataLost( aData, aDataSize);

	/*if( IsSyncServerSession())
	{
		m_pLW->WriteLn( "Received bytes count = ", aDataSize);
	}*/

	// Принимаем данные - копируем данные в буфер
	CopyToBuffer( aData, aDataSize);

	/*if( IsSyncServerSession())
	{
		m_pLW->WriteLn( "Buffer size = ", dwBufferSize);
	}*/

	EnterCriticalSection( &csBufferStat);
	receivedSize += aDataSize;
	packetInfo packet(aDataSize, GetTickCount());
	receivedInfoPackets.push_back(packet);

	DWORD time = GetTickCount();
	std::list<packetInfo>::iterator it = receivedInfoPackets.begin();
	std::list<packetInfo>::iterator end = receivedInfoPackets.end();
	while ( it != end && (time - it->time) > 5000)
	{
		receivedInfoPackets.pop_front();
		it = receivedInfoPackets.begin();
	}
	LeaveCriticalSection( &csBufferStat);

	unsigned int iHeaderSize = 9;
	while (dwBufferSize >= iHeaderSize)
	{
		// Формируем сообщение
		// по сигнатуре проверяем достоверность полученного сообщения
#ifdef DEBUG_SENDPACKETINFO
		bool bESE = false;
		if( bEmulSignError)
		{
			iNumValidMessEmulSignError++;
			if(iNumValidMessEmulSignError>iCountValidMessEmulSignError)
			{
				iNumValidMessEmulSignError = 0;
				bESE = true;
				bEmulSignError = false;
			}
		}
		if (bESE || !CheckSignature())
#else
		if ( m_emulateSignatureError || !CheckSignature())
#endif
		{
			m_emulateSignatureError = false;

			bool IsSocketValid = false;
			threadContextObjectLock.lock();
			if (pThreadContextObject != NULL)
			{
				pThreadContextObject->Lock();
				IsSocketValid = pThreadContextObject->pConnection != NULL && pThreadContextObject->pConnection->isSocketValid();
				pThreadContextObject->Unlock();
			}
			threadContextObjectLock.unlock();

			if( IsSocketValid || bErrorConnection)
				OnSignatureError();
			RemoveMessage( dwBufferSize);
			break;
		}

		// Определяем тип сообщения
		int n = 2;
		BYTE nType = pBuffer[ n];
		n += 1;
		WORD recvMessageID = *(WORD*)(pBuffer + n);
		n += 2;
		DWORD checkSum = *(DWORD*)(pBuffer + n);
		n += 4;

		BYTE* pData = pBuffer + n;
		long dwDataSize = dwBufferSize - n;
		int iMessageSize = HM_WRONG_TYPE;

		if (nType == IN_ALIVE_MESSAGE)
		{
			iMessageSize = OnAlive();
		}
		else
		{
			if (dwDataSize < 4)
			{
				iMessageSize = HM_NOT_ENOUGH_DATA;
			}
			else
			{
				iMessageSize = (*(int*) pData);
				if ( iMessageSize > 1024*1024*100 || iMessageSize < 0) // 100мб таких огромных пакетов не должно быть или длинна меньше 0 - порча заголовка
				{
					OnCheckSumError( checkSum, 0, NULL, 0);
					break;
				}
				if (dwDataSize < iMessageSize + 4)
				{
					iMessageSize = HM_NOT_ENOUGH_DATA;
					if (pMessageHandler != NULL && pMessageHandler->IsSynchMessage(nType))
						iMessageSize = HM_NOT_ENOUGH_DATA_SYNCH;
				}
			}

			if( iMessageSize == HM_NOT_ENOUGH_DATA_SYNCH)
			{
				//??
				mutex.lock();
				CSyncQuery *pSyncQuery = GetWaitingSyncQueryWithoutLock( recvMessageID);

				if( pSyncQuery != NULL)
				{
					bool isUpdated = pSyncQuery->isUpdated();
					pSyncQuery->setUpdated();
					mutex.unlock();
					if( !isUpdated)
					{
						mutexLog.lock();
						if(m_pLW!=NULL)
							m_pLW->WriteLn("HM_SYNCH_DATA100 data updated for sn = ", sSessionName.GetBuffer(), " qn = ", recvMessageID);
						mutexLog.unlock();
					}
				}
				else
					mutex.unlock();

				if( pSyncQuery == NULL)
				{
					mutexLog.lock();
					if(m_pLW!=NULL)
						m_pLW->WriteLn("HM_SYNCH_DATA101 not found", ", sn = ", sSessionName.GetBuffer(), " qn = ", recvMessageID);
					mutexLog.unlock();
				}
			}
			else if (iMessageSize != HM_NOT_ENOUGH_DATA)
			{
				DWORD actualChecksum = CalculateChecksum( pData + 4, iMessageSize);
 				if (checkSum > 0 && pData == NULL || pData != NULL && checkSum != actualChecksum)
				{
					iMessageSize += iHeaderSize;
					mutexLog.lock();
					if(m_pLW!=NULL)
					{
						CLogValue log("[ERROR][SESSION] ", sSessionName.GetString(), " - Checksum Error - : current messageType is ", nType);
						m_pLW->WriteLnLPCSTR( log.GetData());
					}
					mutexLog.unlock();

					/*if( IsSyncServerSession())
					{
						iMessageSize -= iHeaderSize;
						// MessageBoxW(0, L"Ничего не закрываем", L"Зовем Леху, надо отладить", MB_OK);
					}
					else
					{*/
						OnCheckSumError( checkSum, actualChecksum, pBuffer, iMessageSize + 4);
						RemoveMessage( iMessageSize);
					//}

					//if( uSignature == 0x4554)
						//MessageBox(0, "Call Max please", "Checksum.", MB_OK);

					
					break;
				}

		
#if LOGSESSSIONINFO
				DWORD timeHandleBegin = GetTickCount();
#endif
				oldType = nType;
				// Обрабатываем сообщение - вызываем соответствующий обработчик		
				if (mbWriteLogEnabled)
				{
					std::string session = GetSessionName();
					m_pInfoMan->TraceSessionPacketInfo( session, nType, aDataSize, false);
				}

				// если получили ответ на запрос - забудем про этот запрос
				RemoveQueryIfAnswerMessage( recvMessageID);

				if (pMessageHandler != NULL)
					iMessageSize = pMessageHandler->HandleMessage(nType, pData, dwDataSize, checkSum);
#if LOGSESSSIONINFO
				DWORD timeHandleEnd = GetTickCount();
				if( pSessionsInfoMan != NULL)
					pSessionsInfoMan->AddSessionInfo( iSessionInfoIndex, nType, timeHandleEnd - timeHandleBegin);
#endif				
			}
		}

		if (iMessageSize == HM_NOT_ENOUGH_DATA || iMessageSize == HM_NOT_ENOUGH_DATA_SYNCH)
			break;
		
		if (iMessageSize == HM_WRONG_TYPE)
		{
			iMessageSize = dwBufferCapacity;
		}
		else if (iMessageSize == HM_SYNCH_DATA)
		{
#if LOGSESSSIONINFO
			DWORD timeHandleBegin = GetTickCount();
#endif
			m_lastRecvMsgID = recvMessageID;

			mutexLog.lock();
			if( IsDetailedLogMode())
				m_pLW->WriteLn("HM_SYNCH_DATA1 received", ", sn = ", sSessionName.GetBuffer());
			mutexLog.unlock();

			CQueryPackageIn queryIn(pData, dwDataSize);
			if(!queryIn.Analyse())
			{
				mutexLog.lock();
				if(m_pLW!=NULL)
					m_pLW->WriteLn("[ANALYSE FALSE] [SESSION] ", sSessionName.GetString(), "  CQueryPackageIn");
				mutexLog.unlock();
				ATLASSERT( FALSE);
				break;
			}
			DWORD dwSize = queryIn.GetPackageSize();
			WORD queryNum = recvMessageID;

			mutexLog.lock();
			if( IsDetailedLogMode())
				m_pLW->WriteLn("HM_SYNCH_DATA2 received dwSize = ", dwSize, " ,queryNum = ", queryNum, ", sn = ", sSessionName.GetBuffer());

			iMessageSize = dwSize + iHeaderSize + 4/*packet size*/;
			if( IsDetailedLogMode())
				m_pLW->WriteLn("ml9, sn = ", sSessionName.GetBuffer());
			mutexLog.unlock();
			mutex.lock();

			CSyncQuery *pSyncQuery = GetWaitingSyncQueryWithoutLock( queryNum);
			if( pSyncQuery != NULL)
			{
				pSyncQuery->replaceData(queryIn.DetachPacketData());
				pSyncQuery->signal();				
				mutex.unlock();
				mutexLog.lock();
				if( IsDetailedLogMode())
					m_pLW->WriteLn("HM_SYNCH_DATA3 found", ", sn = ", sSessionName.GetBuffer());
				mutexLog.unlock();
			}
			else
			{
				mutex.unlock();
				mutexLog.lock();
				if( IsDetailedLogMode())
					m_pLW->WriteLn("HM_SYNCH_DATA4 not found", ", sn = ", sSessionName.GetBuffer());
				mutexLog.unlock();
			}

			mutexLog.lock();
			if( IsDetailedLogMode())
				m_pLW->WriteLn("mul9, sn = ", sSessionName.GetBuffer());
			mutexLog.unlock();
#if LOGSESSSIONINFO
			DWORD timeHandleEnd = GetTickCount();
			if( pSessionsInfoMan != NULL)
				pSessionsInfoMan->AddSessionInfo( iSessionInfoIndex, nType, timeHandleEnd - timeHandleBegin);
#endif				
		}
		else 
			iMessageSize += iHeaderSize;

		// Удаляем сообщение из буфера
		RemoveMessage( iMessageSize);
	}
	LeaveCriticalSection( &csBuffer);
}

void CClientSession::CopyToBuffer( BYTE* aData, unsigned int aDataSize)
{
	if (dwBufferCapacity < dwBufferSize + aDataSize)
	{
		int newSize = dwBufferCapacity;
		if ((int)dwBufferCapacity > aDataSize)
			newSize += dwBufferCapacity;
		else
			newSize += aDataSize;		

		BYTE* pNewBuffer = MP_NEW_ARR(BYTE, newSize);
		if ( pBuffer != NULL)
		{
			rtl_memcpy( pNewBuffer, newSize, pBuffer, dwBufferSize);
			MP_DELETE_ARR( pBuffer);
		}
		pBuffer = pNewBuffer;
		dwBufferCapacity = newSize;
	}
	rtl_memcpy( pBuffer + dwBufferSize, dwBufferCapacity - dwBufferSize, aData, aDataSize);
	dwBufferSize += aDataSize;
}

int CClientSession::OnAlive()
{
	unsigned char btDebugMode;
#ifdef DEBUG
	btDebugMode = 1;
#else
	btDebugMode = 0;
#endif
	btDebugMode = 0;
	Send( OUT_ALIVE_MESSAGE, &btDebugMode, 1);
	return 4;
}

bool CClientSession::CheckSignature()
{
	return (uSignature == (*(USHORT*)pBuffer));
}

DWORD CClientSession::CalculateChecksum( BYTE* aData, long aDataSize)
{
	DWORD sum = 0;
	if (aData != NULL)
	{
		for (int i = 0;  i < aDataSize;  i++)
			sum += aData[ i];
	}
	return sum;
}

void CClientSession::OnSignatureError()
{
	// По-умолчанию обрываем соединение
	mutexLog.lock();
	if( IsDetailedLogMode())
		m_pLW->WriteLn("ml10, sn = ", sSessionName.GetBuffer());

	if(m_pLW!=NULL)
	{
		CLogValue log("[ERROR][SESSION] ", sSessionName.GetString(), "  Signature Error - ", pServerInfo->sIP.GetString(), ":", (int)pServerInfo->tcpPort);
		m_pLW->WriteLnLPCSTR(log.GetData());
	}
	if( IsDetailedLogMode())
		m_pLW->WriteLn("mul10, sn = ", sSessionName.GetBuffer());
	mutexLog.unlock();
	Close( true);
}

void CClientSession::OnCheckSumError( DWORD aExpectedCheckSum, DWORD aActalCheckSum, BYTE* aData, int aDataSize)
{
	// По-умолчанию обрываем соединение
	mutexLog.lock();
	if( IsDetailedLogMode())
		m_pLW->WriteLn("ml11, sn = ", sSessionName.GetBuffer());
	//mutex.lock();
	if(m_pLW!=NULL)
	{
		CLogValue log1("[ERROR][SESSION] ", sSessionName.GetString(), " - Checksum Error - Expected: ", aExpectedCheckSum, ", Actual: ", aActalCheckSum);
		CLogValue log2("  Wrong checksum data: ", aData, aDataSize);
		CLogValue log(log1, "\n", log2);
		CComString sLog = log.GetData();
		if( sLog.GetLength() > 0xFFFF)
			sLog = sLog.Left(0xFFFF) + "...more data after 0xFFFF bytes";
		m_pLW->WriteLnLPCSTR( sLog.GetBuffer());
	}
	//mutex.unlock();
	if( IsDetailedLogMode())
		m_pLW->WriteLn("mul11, sn = ", sSessionName.GetBuffer());
	mutexLog.unlock();
	Close( true);
}

void CClientSession::RemoveMessage( int aMessageSize)
{
	/*if( IsSyncServerSession())
	{
		m_pLW->WriteLn( "RemoveMessage. Before buffer size = ", dwBufferSize);
	}*/

	m_numbersOfMessage++;
	if ((int)dwBufferSize/*размер данных*/ > aMessageSize/*размер сообщений*/ && aMessageSize > 0)
	{
		dwBufferSize -= aMessageSize;
		memmove( pBuffer, pBuffer + aMessageSize, dwBufferSize);
	}
	else
	{
		dwBufferSize = 0;
	}

	/*if( IsSyncServerSession())
	{
		m_pLW->WriteLn( "RemoveMessage. After buffer size = ", dwBufferSize);
	}*/
}

void CClientSession::RemoveQueryIfAnswerMessage( WORD auMessageId)
{
	if( auMessageId == 0)
		return;

	CSendData*	apDataToDelete = NULL;

	EnterCriticalSection( &csSendQueue);
	CSendQueueMapIter iter = oSendedQueriesMap.find( auMessageId);
	if ( iter != oSendedQueriesMap.end())
	{
		apDataToDelete = (CSendData *) iter->second;
		oSendedQueriesMap.erase( iter);
	}
	LeaveCriticalSection( &csSendQueue);

	if(apDataToDelete!=NULL)
	{
#ifdef DEBUG_SENDPACKETINFO
		int n = 0;
		if(apDataToDelete->bDeleted)
		{
			n = 1;
		}
		ATLASSERT( !apDataToDelete->bDeleted);
		if( !apDataToDelete->bDeleted)
		{
			apDataToDelete->bDeleted = 4;
			mutexLog.lock();
			if(m_pLW!=NULL)
			{
				CLogValue log("[CLSESSION] del num packet=", auMessageId, "  packetID==", apDataToDelete->wPacketID);
				m_pLW->WriteLnLPCSTR(log.GetData());
			}
			mutexLog.unlock();
		}
#else	
		MP_DELETE( apDataToDelete); 
#endif
	}
}

unsigned int CClientSession::GetConnectionStatus()
{
	return (bConnected) ? STATUS_CONNECTED : STATUS_NOTCONNECTED;
}


float  CClientSession::GetMiddleReceiveSpeed()
{
	EnterCriticalSection( &csBufferStat);
	DWORD totalSeconds = (GetTickCount() - createTime) / 1000;
	float result = receivedSize / totalSeconds;
	LeaveCriticalSection( &csBufferStat);
	return result;
}

float  CClientSession::GetMiddleSendSpeed()
{
	EnterCriticalSection( &csSendQueueStat);
	DWORD totalSeconds = (GetTickCount() - createTime) / 1000;
	float result = sendedSize / totalSeconds;
	LeaveCriticalSection( &csSendQueueStat);
	return result;
}

float  CClientSession::GetReceiveSpeed()
{
	EnterCriticalSection( &csBufferStat);
	unsigned int calculateTime = 0;
	DWORD time = GetTickCount();
	float size = 0;
	std::list<packetInfo>::iterator it = receivedInfoPackets.begin();
	std::list<packetInfo>::iterator end = receivedInfoPackets.end();
	for ( ; it != end; ++it)
	{
		calculateTime = max(calculateTime, (time - it->time));
		size += it->size;
	}
	float result = 1000 * size / calculateTime;  // в байтах
	LeaveCriticalSection( &csBufferStat);
	return result;
}

float  CClientSession::GetSendSpeed()
{
	EnterCriticalSection( &csSendQueueStat);
	int calculateTime = 0;
	DWORD time = GetTickCount();
	float size = 0;
	std::list<packetInfo>::iterator it = sendedInfoPackets.begin();
	std::list<packetInfo>::iterator end = sendedInfoPackets.end();
	for ( ; it != end; ++it)
	{
		calculateTime = max(calculateTime, (int)(time - it->time));
		size += it->size;
	}
	float result = 1000 * size / calculateTime;  // в байтах
	LeaveCriticalSection( &csSendQueueStat);
	return result;
}

float  CClientSession::GetReceivedSize()
{
	EnterCriticalSection( &csBufferStat);
	float result = receivedSize;
	LeaveCriticalSection( &csBufferStat);
	return result;
}

float CClientSession::GetSendedSize()
{
	EnterCriticalSection( &csSendQueueStat);
	float result = sendedSize;
	LeaveCriticalSection( &csSendQueueStat);
	return result;
}

void CClientSession::changeLogWriteEnabled(bool isLogEnabled, info::IInfoManager * infoMan)
{
	mbWriteLogEnabled = isLogEnabled;
	m_pInfoMan = infoMan;
}

std::string CClientSession::GetSessionName()
{
	return sSessionName.GetBuffer();
}

////////////////////////////////////////////////
// CMultiPacket
////////////////////////////////////////////////
CClientSession::CMultiPacket::CMultiPacket():
MP_LIST_INIT(oSendQueue)
{
	Clear();
}

CClientSession::CMultiPacket::~CMultiPacket()
{

}

void	CClientSession::CMultiPacket::Clear()
{
	pImmediatelySendData = NULL;
	oSendQueue.clear();
	uDataSize = 0;
}

bool	CClientSession::CMultiPacket::AddImmediatelyData( CSendData* apSendData)
{
	if( pImmediatelySendData != NULL)
		uDataSize -= pImmediatelySendData->dwSize;

	pImmediatelySendData = apSendData;
	uDataSize += pImmediatelySendData->dwSize;

	return true;
}

bool	CClientSession::CMultiPacket::AddData( CSendData* apSendData)
{
	if( oSendQueue.size() != 0 && uDataSize + apSendData->dwSize > MaxMultiPacketSize)
		return false;

	oSendQueue.push_back( apSendData);
	uDataSize += apSendData->dwSize;

	return true;
}

bool	CClientSession::CMultiPacket::CreateMultiPacket(byte** appData, unsigned int* apuDataSize)
{
	if( !appData)
		return false;
	if( !apuDataSize)
		return false;

	if( uDataSize == 0)
		return false;

	// если работаем в режиме один пакет - то ничего не копируем
	if( uDataSize > MaxMultiPacketSize)
	{
		CSendData* pOverflowData = pImmediatelySendData;
		if( pOverflowData == NULL)
			pOverflowData = *oSendQueue.begin();
		if( pOverflowData != NULL)
		{
			*appData = pOverflowData->pData;
			*apuDataSize = pOverflowData->dwSize;
			return true;
		}
		else
		{
			// если тут упадет, то значит размер выставлен большой, а сообщений нет по факту. Это ошибка
			ATLASSERT( FALSE);
			return false;
		}
	}

	byte* pDest = memoryBuffer;

	if( pImmediatelySendData != NULL)
	{
		rtl_memcpy( pDest, uDataSize - (pDest - memoryBuffer), pImmediatelySendData->pData, pImmediatelySendData->dwSize);
		pDest += pImmediatelySendData->dwSize;
	}

	CSendQueue::iterator it = oSendQueue.begin(), end = oSendQueue.end();
	for( ; it!=end; it++)
	{
		rtl_memcpy( pDest, uDataSize - (pDest - memoryBuffer), (*it)->pData, (*it)->dwSize);
		pDest += (*it)->dwSize;
	}

	*appData = memoryBuffer;
	*apuDataSize = uDataSize;

	return true;
}

CClientSession::CSendData*	CClientSession::CMultiPacket::PopImmediatelyData()
{
	CClientSession::CSendData* foo = pImmediatelySendData;
	pImmediatelySendData = NULL;
	if( foo)
		uDataSize -= foo->dwSize;
	return foo;
}

unsigned int CClientSession::CMultiPacket::GetDataSize()
{
	return uDataSize;
}

bool CClientSession::CMultiPacket::IsEmpty()
{
	return (pImmediatelySendData==NULL && oSendQueue.size() == 0) ? true : false;
}

CClientSession::CSendData*	CClientSession::CMultiPacket::PopData()
{
	CClientSession::CSendData* foo = NULL;
	if( oSendQueue.size() != 0)
	{
		CSendQueue::iterator it = oSendQueue.end();
		it--;
		foo = *it;
		oSendQueue.pop_back();
		uDataSize -= foo->dwSize;
	}
	return foo;
}

void CClientSession::PreDestroy()
{
#if LOGSESSSIONINFO
	pSessionsInfoMan = NULL;
#endif
	mutexLog.lock();
	m_pLW = NULL;
	mutexLog.unlock();
	threadContextObjectLock.lock();
	if (pThreadContextObject != NULL)
	{
		pThreadContextObject->Lock();
		if (pThreadContextObject->pConnection != NULL)
			pThreadContextObject->pConnection->setILogWriter(NULL);
		pThreadContextObject->Unlock();

	}
	threadContextObjectLock.unlock();

	if (m_connectionManager != NULL)
		m_connectionManager->Unsubscribe(this);

	Close();

	m_connectionManager = NULL;
}

// разрывает соединение через неверную контрольную сумму, а потом не восстанавливается до вызова UnfreezeConnection
void CClientSession::DropAndFreeze()
{
	EmulateBreak( /*0xFFFFFFFF*/);
	m_connectionIsFreezed = true;
}

// разрешает работу механизма восстановления сессии, остановленного вызовом DropConnectionAndFreeze
void CClientSession::Unfreeze()
{
	m_connectionIsFreezed = false;
}

#if LOGSESSSIONINFO
void CClientSession::SetSessionsInfoMan( int aiSessionInfoIndex, CSessionsInfoMan *apSessionsInfoMan)
{
	iSessionInfoIndex = aiSessionInfoIndex;
	pSessionsInfoMan = apSessionsInfoMan;
}
#endif
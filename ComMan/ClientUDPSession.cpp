// ClientSession.cpp: implementation of the CClientSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <QOS.h>
//#include <Qosname.h>
//#include <Ws2spi.h>
//#include "Traffic.h"
#include "ClientSession.h"
#include "consts.h"
#include "CrashHandlerInThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "QueryPackageIn.h"

using namespace ResManProtocol;

bool CClientSession::CreateUDPSockets()
{
	sckSendUDPServer = socket(AF_INET, SOCK_DGRAM, 0);

	sockAddrSendUDP.sin_family = AF_INET;
	sockAddrSendUDP.sin_addr.s_addr = inet_addr( pServerInfo->sIP.GetBuffer());

	sockAddrSendUDP.sin_port = htons( pServerInfo->udpSendPort);

	int tos = 8;
	if(setsockopt(sckSendUDPServer, IPPROTO_IP, IP_TOS, (LPCSTR)&tos, sizeof(tos))==SOCKET_ERROR) 
	{
		int ff = GetLastError();
		ff+=0;
	}

	bool bListen = CreateListenUDPSocket();
	if (bListen)
	{
		bRunning = true;
		ResetEvent( hSendUDPEvent);

		CSimpleThreadStarter sendThreadStarter(__FUNCTION__);		
		sendThreadStarter.SetRoutine(sendUDPThread_);
		sendThreadStarter.SetParameter(this);
		hSendUDPThread = sendThreadStarter.Start();

		CSimpleThreadStarter recvThreadStarter(__FUNCTION__);		
		recvThreadStarter.SetRoutine(recvUDPThread_);
		recvThreadStarter.SetParameter(this);
		hRecvUDPThread = recvThreadStarter.Start();
	}
	else
	{
		closesocket(sckSendUDPServer);
		sckSendUDPServer = INVALID_SOCKET;
	}
	return bConnected;
}

bool CClientSession::CreateListenUDPSocket()
{
	SOCKERR serr=0;
	SOCKET sck = socket(AF_INET, SOCK_DGRAM, 0);
	serr = (sck == INVALID_SOCKET) ? WSAGetLastError(): 0;

	if (serr != 0)
		return false;

	int tos = 8;
	//int tos = (26<<2);
	if(setsockopt(sck, IPPROTO_IP, IP_TOS, (LPCSTR)&tos, sizeof(tos))==SOCKET_ERROR) 
	{
		int ff = GetLastError();
		ff+=0;
	}

	SOCKADDR_IN RecvAddr;
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(pServerInfo->udpRecvPort);
	RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sck, (SOCKADDR *) &RecvAddr, sizeof(RecvAddr))==-1)
		//if(bind(sck, (SOCKADDR *) &sockAddr, sizeof(sockAddr))==-1)
	{
		int ff = GetLastError();
		ff+=0;
	}

	/*if (connect( sck, (SOCKADDR *)&sockAddr, sizeof(sockAddr)) != 0) 
	serr = WSAGetLastError();*/

	if(serr==0)
	{
		sckRecvUDPServer = sck;
	}
	else
	{
		closesocket( sck);
		sckRecvUDPServer = INVALID_SOCKET;
		return false;
	}
	return true;
}

CClientSession::CSendData::CSendData( USHORT aSignature, unsigned long long aSessionID, WORD aPacketID, int aMessageSize, BYTE aType, const BYTE FAR *aData, int aDataSize)
{
#ifdef DEBUG_SENDPACKETINFO
	bDeleted = false;
#endif
	bType = aType;
	wPacketID = aPacketID;
	DWORD dwHeaderSize = 13;
	DWORD dwBodySize = 0;
	if (aMessageSize < 0)
	{
		dwHeaderSize += 4;
		dwBodySize = aDataSize;
	}
	else
		dwBodySize = (DWORD)aMessageSize;

	dwSize = dwHeaderSize + dwBodySize;
	//pData = new BYTE[ dwSize];
	pData = MP_NEW_ARR(BYTE, dwSize);


	int i = 0;
	*((DWORD64* )(pData + i)) = aSessionID;
	i += sizeof( aSessionID);
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

	bQuery = false;
}

// Отправляет сообщения
void CClientSession::SendUDP( BYTE aType, const BYTE FAR *aData, int aDataSize)
{
	int iMessageSize = GetOutMessageSize( aType);
	if (iMessageSize > aDataSize)
	{
		::MessageBox( NULL, "Wrong out message size.", "Error", MB_ICONERROR | MB_OK);
		return;
	}

	//CSendData* pSendUDPData = new CSendData( uSignature, ulSessionID, iMessageSize, aType, aData, aDataSize);
	MP_NEW_P7(pSendUDPData, CSendData, uSignature, ulSessionID, 0, iMessageSize, aType, aData, aDataSize);

	EnterCriticalSection( &csSendUDPQueue);
	oSendUDPQueue.push_back( pSendUDPData);
	oSendUDPQueueMap.insert( std::pair<WORD, CSendData *>(messageID, pSendUDPData));
	SetEvent( hSendUDPEvent);
	LeaveCriticalSection( &csSendUDPQueue);
}

void CClientSession::SendUDPImmediately(BYTE aType, const BYTE FAR *aData, int aDataSize)
{
	int iMessageSize = GetOutMessageSize( aType);
	if (iMessageSize > aDataSize)
	{
		::MessageBox( NULL, "Wrong out message size.", "Error", MB_ICONERROR | MB_OK);
		return;
	}

	EnterCriticalSection( &csSendUDPQueue);
	if (pImmediatelySendUDPData == NULL)
		MP_DELETE( pImmediatelySendUDPData);
	//pImmediatelySendUDPData = new CSendData( uSignature, ulSessionID, iMessageSize, aType, aData, aDataSize);
	MP_NEW_V7( pImmediatelySendUDPData, CSendData, uSignature, ulSessionID, 0, iMessageSize, aType, aData, aDataSize);
	SetEvent( hSendUDPEvent);
	LeaveCriticalSection( &csSendUDPQueue);
}

DWORD CClientSession::sendUDPThread()
{
	CoInitialize(NULL);
	DWORD resWait = 0;
	while (bRunning && (resWait = WaitForSingleObject( hSendUDPEvent, 30000)) != WAIT_FAILED)
	{
		CSendData* pSendUDPData = NULL;
		EnterCriticalSection( &csSendUDPQueue);
		if (pImmediatelySendUDPData != NULL)
			pSendUDPData = pImmediatelySendUDPData;
		else if (resWait==WAIT_TIMEOUT && !oSendUDPQueueMap.empty())
		{
			CSendQueueMap::iterator it = oSendUDPQueueMap.begin();
			pSendUDPData = it->second;
		}
		else if (!oSendUDPQueue.empty())
		{
			CSendQueue::iterator it = oSendUDPQueue.begin();
			pSendUDPData = *it;
		}
		LeaveCriticalSection( &csSendUDPQueue);

		if (pSendUDPData == NULL)
		{
			ResetEvent( hSendUDPEvent);
			continue;
		}

		if (sckSendUDPServer == INVALID_SOCKET)
		{
			ResetEvent( hSendUDPEvent);
			continue;
		}

		if ( pSendUDPData->dwSize < 7)
		{
			ATLASSERT( false);
		}
		int n = 0;
		n = ::sendto( sckSendUDPServer, (char*)pSendUDPData->pData, pSendUDPData->dwSize, 0, (SOCKADDR *)&sockAddrSendUDP, sizeof(sockAddrSendUDP));

		if (n == SOCKET_ERROR)
			n = WSAGetLastError();
		else
		{
			EnterCriticalSection( &csSendUDPQueue);
			if (pSendUDPData == pImmediatelySendUDPData)
				pImmediatelySendUDPData = NULL;
			else if (!oSendUDPQueue.empty())
				oSendUDPQueue.pop_front();
			if (oSendUDPQueue.empty())
				ResetEvent( hSendUDPEvent);
			LeaveCriticalSection( &csSendUDPQueue);
			MP_DELETE( pSendUDPData);
		}
	}

	mutexClose.lock();
	hSendUDPThread = NULL;
	mutexClose.unlock();
	CoUninitialize();
	return 0;
}

DWORD WINAPI CClientSession::sendUDPThread_( VOID* param)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	return ((CClientSession*)param)->sendUDPThread();
}

DWORD CClientSession::recvUDPThread()
{
	CoInitialize(NULL);

	BYTE btBuff[ 1024];
	int nBuffLen = 0;
	while (bRunning)
	{
		if (sckRecvUDPServer == INVALID_SOCKET)
		{
			Sleep( 1000);
			if(ulSessionID==0)
				break;
			CreateListenUDPSocket();
			continue;
		}

		struct sockaddr_in sockAddrAnswer;
		int sockAddrSize  = sizeof(sockAddrAnswer);
		nBuffLen = ::recvfrom( sckRecvUDPServer, (char*)&btBuff[0], 1024, 0, (SOCKADDR *)&sockAddrAnswer, &sockAddrSize);

		CComString s =  inet_ntoa(sockAddrAnswer.sin_addr);

		if (nBuffLen < 0)
		{
			//int ff = GetLastError();
			Sleep(1000);
			CreateListenUDPSocket();
			continue;
		}

		if (nBuffLen != 0)
		{
			handleReceiveUDPData( btBuff, nBuffLen);
		}
	}

	CoUninitialize();
	return 0;
}

DWORD WINAPI CClientSession::recvUDPThread_( VOID* param)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	return ((CClientSession*)param)->recvUDPThread();
}

void CClientSession::handleReceiveUDPData( BYTE* aData, unsigned int aDataSize)
{
	EnterCriticalSection( &csBuffer);

	// Принимаем данные - копируем данные в буфер
	CopyToBuffer( aData, aDataSize);

	while (dwBufferSize >= 3)
	{
		// Формируем сообщение
		// по сигнатуре проверяем достоверность полученного сообщения
		if (!CheckSignature())
		{
			OnSignatureError();
			RemoveMessage( dwBufferSize);
			break;
		}

		// Определяем тип сообщения
		BYTE nType = pBuffer[ 2];
//		int iHeaderSize = 3;
		int iHeaderSize = 5;
		int iMessageSize = HM_WRONG_TYPE;

		WORD recvMessageID = *(WORD*)(pBuffer + iHeaderSize - 2);
		if(recvMessageID>0 && oSendUDPQueueMap.find( recvMessageID) != oSendUDPQueueMap.end())
		{
			oSendUDPQueueMap.erase( recvMessageID);				
		}
		BYTE* pData = pBuffer + iHeaderSize;
		DWORD dwDataSize = dwBufferSize - iHeaderSize;

		if (nType == IN_ALIVE_MESSAGE)
		{
			iMessageSize = OnAlive();
		}
		else
		{
			oldType = nType;
			// Обрабатываем сообщение - вызываем соответствующий обработчик		
			if (pMessageHandler != NULL)
				iMessageSize = pMessageHandler->HandleMessage(nType, pData, dwDataSize, 0);
		}

		if (iMessageSize == HM_NOT_ENOUGH_DATA)
			break;

		if (iMessageSize == HM_WRONG_TYPE)
		{
			iMessageSize = dwBufferCapacity;
		}
		else if (iMessageSize == HM_SYNCH_DATA)
		{
			//BYTE* pData = pBufferPacket + iHeaderSize;
			//DWORD dwDataSize = dwBufferPacketSize - iHeaderSize;
			CQueryPackageIn queryIn(pData, dwDataSize);
			if(!queryIn.Analyse())
			{
				if(m_pLW!=NULL)
					m_pLW->WriteLn("[ANALYSE FALSE][SESSION] ", sSessionName.GetString(), "  CQueryPackageIn");
				ATLASSERT( FALSE);
				break;
			}
			DWORD dwSize = queryIn.GetPackageSize();
			WORD queryNum = recvMessageID;//queryIn.GetQueryNum();

			iMessageSize = dwSize + iHeaderSize + 4;
			mutex.lock();
			if( oSyncQueryMap.find( queryNum) != oSyncQueryMap.end())
			{
				CSyncQuery *pSyncQuery = (CSyncQuery *) oSyncQueryMap[ queryNum];
				pSyncQuery->replaceData(queryIn.DetachPacketData());
				pSyncQuery->signal();				
			}
			mutex.unlock();
		}
		else 
			iMessageSize += iHeaderSize;

		// Удаляем сообщение из буфера
		RemoveMessage( iMessageSize);
	}
	LeaveCriticalSection( &csBuffer);
}
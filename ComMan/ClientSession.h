// ClientSession.h: interface for the CClientSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTSOCKET_H__375D114C_CE19_4DD9_A2C4_BF3F6D5B789B__INCLUDED_)
#define AFX_CLIENTSOCKET_H__375D114C_CE19_4DD9_A2C4_BF3F6D5B789B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <map>

#include "serverInfo.h"
#include "ProxySettings.h"
#include "imessagehandler.h"
#include "iclientsession.h"
#include "serverInfo.h"
#include "DataBuffer2.h"
#include "Mutex.h"
#include "ILogWriter.h"
#include "InfoMan.h"
#include "WinSocketConnection.h"
#include "SocksConnection.h"
#include "TunnelConnection.h"
#include "ConnectionManager.h"

#if LOGSESSSIONINFO
#include "SessionsInfoMan.h"
#endif
//#define DEBUG_SENDPACKETINFO 1

using namespace cs;

class CConnectionManager;
class ThreadContextObject;

#define IN_ALIVE_MESSAGE_SIZE 13

#define IN_ALIVE_MESSAGE 255
#define OUT_ALIVE_MESSAGE 255
#define SERVICE_WORLD_SESSION_ID	0xFFFFFFFFFFFFFFFF
#define INCORRECT_SESSION_ID		0

#define SIGNATURE_RES_SOCKET	5641

#define MAX_WAIT_CONNECT_TIME	120000

enum EPacketProcessMethod
{
	EPM_FIFO, 
	EPM_LIFO
};

struct packetInfo
{
	DWORD size;
	DWORD time;
	packetInfo( DWORD aSize, DWORD aTime)
	{
		size = aSize;
		time = aTime;
	}
} ;

class CClientSession : public iclientsession
{
public:
	static unsigned int STATUS_CONNECTED;
	static unsigned int STATUS_NOTCONNECTED;
public:
	// Конструктор
	CClientSession( LPCSTR aSessionName, USHORT aSignature);

	// Деструктор
	virtual ~CClientSession();

	// Устанавливает сессию
	void CClientSession::SetSessionID( unsigned long long aSessionID);

	void SetSessionKey( CComString aSessionKey);

	// Открывает сессию
	//bool Open(LPCTSTR aSocketAddress, UINT aSocketPort, unsigned long long aSessionID, ILogWriter *m_pLW);
	bool Open( cs::CServerInfo *aServerInfo, unsigned long long aSessionID, CComString aSessionKey, ILogWriter *am_pLW, CConnectionManager* aConnectionManager, bool aIsAsynchConnect);
	// Возвращает текущий размер очереди отправки
	int GetSendQueueSize();
	// Получает статус клиентской сессии
	int GetStatus();
	// Посылает сообщение
	void Send(BYTE aType, const BYTE FAR *aData, int aDataSize);

	// Отправляет сообщения c сохранением отправленных для ожидания ответа на них
	void SendQuery( BYTE aType, const BYTE FAR *aData, int aDataSize);

	// Посылает на сервер данные, вперед всех сообщений очереди
	// Перед посылкой удаляет все сообщения с указанным типом
	void SendSignalQuery(unsigned char aType, const unsigned char FAR *aData, int aDataSize);

	// Удаляет запрос из очередей
	virtual void RemoveQueryByType(unsigned char aType);

	// Посылает сообщение и ждет ответа
	int SendAndReceive(BYTE aType, const BYTE FAR *aData, DWORD aDataSize, BYTE **aAnswerData, DWORD *aAnswerDataSize, DWORD timeout = INFINITE, int* aQueryID = NULL);

	// Закрывает сессию
	void Close(bool bError = false);

	// Возвращает true - если сессия успешно соединена с сервером (сервер ответил и соединение было установлено)
	// и эта сессия сейчас активна
	// Возвращаемые значения:
	// 0 - не установлено
	// 1 - установлено
	unsigned int GetConnectionStatus();

	void SetMessageHandler(cs::imessagehandler* pMesHandler);

	void SetSessionListener(cs::isessionlistener* aListener);

	void SetSignature(USHORT uSignature);
	USHORT GetSignature();

	// Устанавливает режим детализированного лога
	void SetDetailedLogModeEnabled( bool abEnabled);

	float GetMiddleReceiveSpeed();
	float GetMiddleSendSpeed();

	float GetReceiveSpeed();
	float GetSendSpeed();

	float GetReceivedSize();
	float GetSendedSize();

	void changeLogWriteEnabled(bool isLogEnabled, info::IInfoManager * infoMan);

	std::string GetSessionName();

	virtual void ChangeConnection();
	virtual void EmulateBreak( /*int auConnectionMask*/);

	void PreDestroy(); // вызывается перед удалением сессии

	void DropAndFreeze(); // разрывает соединение через неверную контрольную сумму, а потом не восстанавливается до вызова UnfreezeConnection
	void Unfreeze(); // разрешает работу механизма восстановления сессии, остановленного вызовом DropConnectionAndFreeze

#if LOGSESSSIONINFO
	void SetSessionsInfoMan( int aiSessionInfoIndex, CSessionsInfoMan *apSessionsInfoMan);
#endif

private:
	struct CSyncQuery
	{
		HANDLE event;
		CDataBuffer2 *pData;
		bool isAnyDataReceived;

		CSyncQuery()
		{
			event = CreateEvent( NULL, TRUE, FALSE, NULL);
			pData = MP_NEW( CDataBuffer2);
			isAnyDataReceived = false;
		}

		~CSyncQuery()
		{
			MP_DELETE( pData);
			CloseHandle( event);
		}

		void reset()
		{
			ResetEvent( event);
		}

		void signal()
		{
			SetEvent( event);
		}

		void replaceData(CDataBuffer2* apData){
			if(pData)
				MP_DELETE( pData);
			pData = apData;
		}

		// Устанавливает флаг получения данных. Сделано для того, чтобы при медленном коннекте знать, 
		// что данные поступают. При этом код ошибки TimeOut возвращаться не будет.
		void setUpdated()
		{
			isAnyDataReceived = true;
		}

		bool isUpdated()
		{
			return  isAnyDataReceived;
		}

		bool resetUpdated()
		{
			bool foo = isAnyDataReceived;
			isAnyDataReceived = false;
			return foo;
		}
	};
	typedef MP_MAP<WORD, CSyncQuery *> CSyncQueryMap;

protected:
	CComString sSessionName;
	USHORT uSignature;
	BYTE oldType;

	virtual int GetOutMessageSize( BYTE aType)
	{
		if (aType == 0)
			return 8;	// handshake without key
		else if (aType == 254)
			return 84+8;		// handshake with key
		return -1;
	}
	virtual void OnSignatureError();
	virtual void OnCheckSumError( DWORD aExpectedCheckSum, DWORD aActalCheckSum, BYTE* aData, int aDataSize);
	CSyncQuery*	GetWaitingSyncQueryWithoutLock( WORD aiQueryID);

	void SetMaxQueueSize( int aMaxQueueSize);
	int QueueSizeIsBig();
	void DecreaseQueueSize( int aCountElement);

	// Посылает сообщение
	void SendMessageImpl(BYTE aType, const BYTE FAR *aData, int aDataSize, bool abQuery = false, WORD aMessageID = 0, EPacketProcessMethod aProcessMethod = EPM_FIFO);
	void RemoveSpecifiedTypeMessages(BYTE aType);
public:
	struct CSendData
	{
		DWORD dwSize;
		BYTE* pData;
		WORD wPacketID;
		bool bQuery;
		BYTE bType;
		CSendData();
		CSendData( USHORT aSignature, unsigned long long aSessionID, WORD aPacketID, int aMessageSize, BYTE aType, const BYTE FAR *aData, int aDataSize);
		CSendData( USHORT aSignature, WORD aPacketID, int aMessageSize, BYTE aType, const BYTE FAR *aData, int aDataSize, bool aQuery = false);
		~CSendData();
#ifdef DEBUG_SENDPACKETINFO
		int bDeleted;
#endif
	};
	typedef MP_LIST< CSendData*> CSendQueue;
	typedef MP_MAP<WORD, CSendData *> CSendQueueMap;
	typedef CSendQueueMap::iterator CSendQueueMapIter;

	class CMultiPacket
	{
#define MaxMultiPacketSize 16384
	public:
		CMultiPacket();
		~CMultiPacket();

		bool		AddImmediatelyData( CSendData* apSendData);
		bool		AddData( CSendData* apSendData);
		bool		CreateMultiPacket(byte** appData, unsigned int* apuDataSize);
		void		Clear();
		bool		IsEmpty();
		unsigned int GetDataSize();

		CSendData*	PopImmediatelyData();
		CSendData*	PopData();

	protected:
		CSendQueue	oSendQueue;
		CSendData*	pImmediatelySendData;
		unsigned int uDataSize;
		byte		memoryBuffer[MaxMultiPacketSize];
	};

private:
	WORD messageID;

	WORD IncMessageID();

	// Посылает сообщение
	void SendImmediately(BYTE aType, const BYTE FAR *aData, int aDataSize);
	// Посылает сообщение, запоминаем. что должен быть ответ
	void SendImmediatelyQuery(BYTE aType, const BYTE FAR *aData, int aDataSize);
	// Освобождает все от ждущие эвенты
	void SignalAllWaitingsEvents();
	// Перепосылает все пакеты, на которые не пришел ответ
	void ResendLostQueries();
	// Нужно и можно ли писать детальный лог?
	bool IsDetailedLogMode( );
	
	bool CreateSocket(bool aIsAsynchConnect);
	bool CreateUDPSockets();
	bool CreateListenUDPSocket();

	CComString GetIPViaHostName( LPCSTR alpHost);
	bool IsIPAddress( LPCSTR alpHost);
	void ReleaseThreads();

	// Поток отправки сообщений
	static DWORD WINAPI sendThread( VOID* aParams);
	// Дискриптор потока 
	HANDLE hSendThread;
	// Очередь запросов
	CSendQueue oSendQueue;
	// Мапа запросов, на которые сервер должен вернуть ответы
	CSendQueueMap oSendedQueriesMap;
	// Мапа синхронных запросов, ответы на которые ждем в потоках
	CSyncQueryMap oSyncQueryMap;
	// Указатель на данные, которые должны быть посланы в первую очередь
	CSendData* pImmediatelySendData;

	// Событие появление нового запроса
	HANDLE hSendEvent;
	// Критическая секция на изменения oSendQueue
	CRITICAL_SECTION csSendQueue;
	CRITICAL_SECTION csSendQueueStat;

	// Поток получения сообщений
	static DWORD WINAPI recvThread( VOID* aParams);
	// Обработка полученных данных
	virtual void handleReceiveData( BYTE* aData, unsigned int aDataSize);
	// Обработка полученных UDP данных
	virtual void handleReceiveUDPData( BYTE* aData, unsigned int aDataSize);
	// Дискриптор потока 
	HANDLE hRecvThread;
	
	// Очередь отправленных UDP запросов
	CSendQueue oSendUDPQueue;
	CSendData* pImmediatelySendUDPData;
	CSendQueueMap oSendUDPQueueMap;
	// Дискрипторы потоков 
	HANDLE hSendUDPThread;
	HANDLE hRecvUDPThread;

	// Событие появление нового UDP запроса
	HANDLE hSendUDPEvent;

	// Событие окончания подключения - нужно для синхронного подключения в Open()
	HANDLE hConnectEvent;


	SOCKADDR_IN sockAddrSendUDP;
	SOCKADDR_IN sockAddrRecvUDP;
	void SendUDP( BYTE aType, const BYTE FAR *aData, int aDataSize);
	void SendUDPImmediately(BYTE aType, const BYTE FAR *aData, int aDataSize);

	// Критическая секция на изменения oSendUDPQueue
	CRITICAL_SECTION csSendUDPQueue;
	// Поток отправки сообщений
	DWORD sendUDPThread();
	static DWORD WINAPI sendUDPThread_( VOID* param);
	// Поток получения сообщений
	DWORD recvUDPThread();
	static DWORD WINAPI recvUDPThread_( VOID* param);

	int m_MaxQueueSize;

protected:
	BYTE* pBuffer;
	DWORD dwBufferCapacity;
	DWORD dwBufferSize;
	CRITICAL_SECTION csBuffer;
	CRITICAL_SECTION csBufferStat;
	DWORD m_errorCode;
	CServerInfo serverInfo;
	CServerInfo *pServerInfo;
	unsigned long long ulSessionID;
	CComString sSessionKey;
	bool bConnected;
	bool bErrorConnection;
	ThreadContextObject* pThreadContextObject;
	ThreadContextObject* pThreadContextObjectForThreadSaver; // ссылка для потоков
	CMutex	threadContextObjectLock;

protected:
	void CopyToBuffer( BYTE* aData, unsigned int aDataSize);
	int OnAlive();
	bool CheckSignature();
	DWORD CalculateChecksum( BYTE* aData, long aDataSize);
	void RemoveMessage( int aMessageSize);
	int Connect(int aConnectResult);
	void RemoveQueryIfAnswerMessage( WORD auMessageId);
	void PushPacketsToMultiPacket(bool& abImmediateSended);
	void RestorePacketsFromMultiPacket();
	void RemovePacketsFromMultiPacket();
	void HandleSentPacket( CSendData* pSendData);
	void RemovePacketFromSendedQueriesMap( CSendData* pSendData);
	bool IsSyncServerSession();
	bool DataIsAlivePacket( BYTE* aData, int aDataSize);
	void CheckLongPacketDataLost( BYTE* aData, unsigned int aDataSize);
	
private:
	//SOCKET sckServer;
	SOCKET sckRecvUDPServer;
	SOCKET sckSendUDPServer;
	volatile bool bRunning;
	int m_numbersOfMessage;
	int m_countHandleDataCalls;
	int m_sequentialAlivePacketsCount;

	BOOL IsThreadTerminated();
	DWORD GetExitCode();

protected:
	cs::imessagehandler* pMessageHandler;	
	cs::isessionlistener* pSessionListener;	

	CMutex mutex;
	CMutex mutexRestore;
	CMutex mutexClose;
	CMutex mutexLog;
	ILogWriter *m_pLW;
	CConnectionManager* m_connectionManager;

	HANDLE m_handle;

	float receiveSpeed;
	float sendSpeed;

	float instantaneousReceiveSpeed;
	float instantaneousSendSpeed;

	float receivedSize;
	float sendedSize;

	DWORD createTime;

	MP_LIST<packetInfo> sendedInfoPackets;
	MP_LIST<packetInfo> receivedInfoPackets;

	bool mbWriteLogEnabled;

	// Формируемый блок для посылки данных
	CMultiPacket	oMultiPacket;

	info::IInfoManager * m_pInfoMan;

	bool		bLogDetailedInformation;
	bool		bFirstSendAfterConnectionWasExecuted;
	bool		bFirstReceiveAfterConnectionWasExecuted;

	WORD		m_lastRecvMsgID;
	bool		m_emulateSignatureError;
	unsigned int m_stackSize;

	DWORD		m_connectionSessionID;
	bool		m_connectionIsFreezed;	// соединение не будет восстанавливаться при разрыве связи

	// номер сессии
	int iSessionInfoIndex;
#if LOGSESSSIONINFO
	CSessionsInfoMan *pSessionsInfoMan;
#endif
};

#endif 

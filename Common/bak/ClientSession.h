// ClientSession.h: interface for the CClientSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGSERVER_H__375D114C_CE19_4DD9_A2C4_BF3F6D5B789B__INCLUDED_)
#define AFX_LOGSERVER_H__375D114C_CE19_4DD9_A2C4_BF3F6D5B789B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>


class CClientSession
{
public:
	// Конструктор
	CClientSession();

	// Деструктор
	virtual ~CClientSession();

	// Открывает сессию
	bool Open( LPCTSTR aSocketAddress, UINT aSocketPort, unsigned long long aSessionID);

	// Посылает сообщение
	void Send( BYTE aType, const BYTE FAR *aData, int aDataSize);

	// Закрывает сессию
	void Close();

protected:
	USHORT uSignature;

	virtual int GetOutMessageSize( BYTE aType)
	{
		if (aType == 0)
			return 8;
		return -1;
	}
	virtual int HandleMessage( BYTE aType, BYTE* aData, int aDataSize) = 0;
	virtual void OnSignatureError( BYTE* aData, int aDataSize);
	virtual void OnError( BYTE* aData, int aDataSize)
	{
	}

private:
	struct CSendData
	{
		DWORD dwSize;
		BYTE* pData;
		CSendData() 
		{
			dwSize = 0;
			pData = NULL;
		}
	};
	typedef std::list< CSendData*> CSendQueue;
	
	bool CreateSocket( LPCSTR aSocketAddress, UINT aSocketPort);

	// Поток отправки сообщений
	DWORD sendThread();
	static DWORD WINAPI sendThread_( VOID* param);
	// Дискриптор потока 
	HANDLE hSendThread;
	// Очередь запросов
	CSendQueue oSendQueue;
	// Событие появление нового запроса
	HANDLE hSendEvent;
	// Критическая секция на изменения oSendQueue
	CRITICAL_SECTION csSendQueue;

	// Поток получения сообщений
	DWORD recvThread();
	static DWORD WINAPI recvThread_( VOID* param);
	// Обработка полученных данных
	void handleReceiveData( BYTE* aData, int aDataSize);
	// Дискриптор потока 
	HANDLE hRecvThread;

	byte* pBuffer;
	DWORD dwBufferCapacity;
	DWORD dwBufferSize;
	CRITICAL_SECTION csBuffer;

	void CopyToBuffer( BYTE* aData, int aDataSize);
	bool CheckSignature();
	void RemoveMessage( int aMessageSize);
	
private:
	SOCKET sckServer;
	bool bRunning;
};

#define HM_WRONG_TYPE -1
#define HM_NOT_ENOUGH_DATA -2

#define BEGIN_IN_MESSAGE_MAP() \
	virtual int HandleMessage( BYTE aType, BYTE* aData, int aDataSize) \
	{ \
		switch (aType) \
		{
#define IN_MESSAGE( TYPE, HANDLER, MES_SIZE) \
			case TYPE: \
			{ \
				if (aDataSize < MES_SIZE) \
					return HM_NOT_ENOUGH_DATA; \
				HANDLER( aData, MES_SIZE); \
				return MES_SIZE; \
			}
#define IN_MESSAGE2( TYPE, HANDLER) \
			case TYPE: \
			{ \
				int nSize = (*(int*) aData); \
				if (aDataSize < nSize) \
					return HM_NOT_ENOUGH_DATA; \
				HANDLER( aData + 4, nSize); \
				return nSize + 4; \
			}
#define END_IN_MESSAGE_MAP() \
		} \
		return HM_WRONG_TYPE; \
	}

#define BEGIN_OUT_MESSAGE_MAP() \
	virtual int GetOutMessageSize( BYTE aType) \
	{ \
		switch (aType) \
		{
#define IN_MESSAGE( TYPE, MES_SIZE) \
			case TYPE: return MES_SIZE;
#define END_OUT_MESSAGE_MAP() \
		} \
		return -1; \
	}

#endif // !defined(AFX_LOGSERVER_H__375D114C_CE19_4DD9_A2C4_BF3F6D5B789B__INCLUDED_)

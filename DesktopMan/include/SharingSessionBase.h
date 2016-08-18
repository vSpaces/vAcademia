#pragma once

#include "SharingConnection.h"

class CSharingConnection;

//////////////////////////////////////////////////////////////////////////
class DESKTOPMAN_API CSharingSessionBase : public ISharingConnectionEvents
{
	friend class CSharingConnectionFactory;
	friend class CSharingSessionFactory;
public:
	CSharingSessionBase( SHARING_CONTEXT* apContext);
	virtual ~CSharingSessionBase();

	void SetSessionID( unsigned short aSessionID);
	unsigned short GetSessionID();
	static unsigned short INCORRECT_SESSION_ID;

	virtual void HandleMessage( BYTE aType, BYTE* aData, int aDataSize) = 0;
	
public:
	bool m_isFirstStartFileSession;
	sharingError StartSession();
	void StopSession(bool withNotificationServer);
	void Send(unsigned char aType, const unsigned char FAR *aData, int aDataSize);
	bool SendWithWait(unsigned char aType, const unsigned char FAR *aData, int aDataSize);
	void SendQuery(unsigned char aType, const unsigned char FAR *aData, int aDataSize);
	void SetConnection( CSharingConnection*	aConnection);
	
	bool IsPaused()
	{
		return isPausedSession;
	}
	LPCSTR GetSessionName()
	{
		return sessionName.GetBuffer();
	}
	LPCSTR GetFileName()
	{
		if ( !fileName.IsEmpty())
			return fileName.GetBuffer();
		else
			return NULL;
	}
	unsigned int GetRecordID()
	{
		return recordID;
	}
	void SetSessionStarted()
	{
		sessionStarted = true;
	}
	void SetSessionNotStarted()
	{
		sessionStarted = false;
	}
	CSharingConnection* GetConnection()
	{
		return connection;
	}
	void SetFileName( LPCSTR alpcName)
	{
		if( alpcName)
			fileName = alpcName;
	}
	bool GetRemoteAccessMode()
	{
		return canRemoteAccess;
	}
	LPCSTR GetUnicSessionName();
public:
	void OnConnected();
	void OnConnectLost();
	void OnConnectRestored();
	bool IsFileSharingSession();
	void SetFileSharing();
	bool IsConnectionConnected();

	void SetIsPaused(bool anIsPausedSession)
	{
		isPausedSession = anIsPausedSession;
	}
protected:
	void SetSessionName( LPCSTR alpcName)
	{
		if( alpcName)
			sessionName = alpcName;
	}

	void SetRecordID(unsigned int recId)
	{
		recordID = recId;
	}
	
	void SetRemoteAccessMode(bool accessMode)
	{
		canRemoteAccess = accessMode;
	}
	void SetUnicSessionName(LPCSTR alpcName);
	// Must be overriden
	virtual void OnSessionStarted() = 0;
	virtual void OnSessionStopped() = 0;
	SHARING_CONTEXT*	context;
	virtual void	SendTerminateSessionRequest();

	void SetSeekFileTime(unsigned int aSeekTime);
	unsigned int GetSeekFileTime();

	void SetServerVersion(unsigned int aServerVersion);
	unsigned int GetServerVersion();
	

private:
	bool				canRemoteAccess;
	bool				isPausedSession;
	unsigned int		recordID;
	bool				isFileSharing;
	CSharingConnection*	connection;
	CComString			sessionName;
	CComString			fileName;
	unsigned short		sessionID;
	bool				sessionStarted;
	// имя сессии уникальное для сервера
	CComString			unicSessionName;

	unsigned int		seekFileTime;
	unsigned int		serverVersion;
protected:
	bool				connectionConnected;
};
#pragma once

#include "ConnectionFactory.h"
#include "ObjectMapByName.h"
#include "SharingSession.h"
#include "SharingViewSession.h"
#include "SharingContext.h"
#include "ilogwriter.h"
//////////////////////////////////////////////////////////////////////////
class CSharingSessionFactory : public ISharingConnectionMessageHandler
{
public:
	CSharingSessionFactory(SHARING_CONTEXT* aContext);
	virtual ~CSharingSessionFactory();

public:
	bool	StartCapturingSession( LPCSTR alpcTarget, IDesktopSelector* apSelector, CSharingConnection* apConnection, unsigned int quality, bool remoteAccessMode);
	bool	StartViewSession( LPCSTR alpcTarget, LPCSTR alpcFileName, CSharingConnection* apConnection);
	bool	StartFilePlay( LPCSTR alpcTarget, LPCSTR alpFileName, unsigned int recId, unsigned int auBornRealityID, bool isPaused, CSharingConnection* apConnection, unsigned int aShiftTime, unsigned int aServerVersion);
	void	SetViewSessionDestination( LPCSTR alpcTarget, IDesktopSharingDestination* apIDesktopSharingDestination);
	CObjectMapByName<CSharingSession> GetAllSharingSession();
	CObjectMapByName<CSharingViewSession> GetAllViewSession();
	CSharingSession*	GetSharingSessionByName( LPCSTR alpcGroupName);
	CSharingViewSession*	GetViewSessionByName( LPCSTR alpcGroupName);
	void TryingStartFileSession(LPCSTR alpTarget);
public:
	void	DestroyViewSession( unsigned int aSessionID, bool withNotificationServer);
	void	DestroyViewSession( LPCSTR alpTarget, bool withNotificationServer);
	void	DestroyCapturingSession( unsigned int aSessionID, bool withNotificationServer);
	void	DestroyCapturingSession( LPCSTR alpTarget, bool withNotificationServer);
	void	DestroyAllSessions();
	void	AddVASEPack( void* pack, bool abKeyFrame, CSharingViewSession* viewSession);
	void	DestroyDeletedSessions();

public:
	void	ShowSharingSessionRectangle( LPCSTR alpSessionName);
	void	HideSharingSessionRectangle( LPCSTR alpSessionName);
	void	SetSharingViewFps( LPCSTR alpTarget, unsigned int fps);
	CComString	ConvertToUnicName( LPCSTR alpTarget);
	CComString	ConvertToUnicName( LPCSTR alpTarget, int aRealityID);

	// реализация ISharingConnectionMessageHandler
public:
	void HandleMessage( CSharingConnection* aConnection, unsigned short aSessionID
						, BYTE aType, BYTE* aData, int aDataSize);



private:
	CSharingViewSession*	CreateViewSession( LPCSTR alpcGroupName, LPCSTR alpFileName);
	CSharingViewSession*	CreateViewSession( LPCSTR alpcGroupName, LPCSTR alpFileName, unsigned int aRecordID, bool isPaused, unsigned int aShiftTime, unsigned int aServerVersion);
	CSharingSession*		CreateCapturingSession( LPCSTR alpcObjectName, bool remoteAccessMode);
	CSharingSessionBase*	GetSessionByID( unsigned short aSessionID, unsigned int aSessionType);
	CComString				GetRandomName(unsigned short sessionID);

private:
	// View sessions
	CObjectMapByName<CSharingViewSession>	mapViewSessions;
	CObjectMapByName<CSharingSession>		mapSharingSessions;
	MP_VECTOR<CSharingSession*>		deletingSharingSessions;
	CCriticalSection	csFactory;


	SHARING_CONTEXT*	context;
};
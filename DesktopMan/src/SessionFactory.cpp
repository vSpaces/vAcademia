#include "StdAfx.h"
#include "..\include\SessionFactory.h"
#include "..\include\SharingConnection.h"
#include "..\include\SharingViewSession.h"
#include "..\Protocol\SessionParamsIn.h"
#include "..\Protocol\MissedNameOut.h"
#include "..\include\SharingMan.h"
#include "..\include\CSHelper.h"
#include "..\protocol\RecordIn.h"
#include "..\protocol\HideSession.h"
#include <time.h>

CSharingSessionFactory::CSharingSessionFactory(SHARING_CONTEXT* aContext) : MP_VECTOR_INIT(deletingSharingSessions)
{
	context = aContext;
	deletingSharingSessions.clear();
	csFactory.Init();
}

CSharingSessionFactory::~CSharingSessionFactory()
{
	csFactory.Term();
}

bool	CSharingSessionFactory::StartCapturingSession( LPCSTR alpcTarget, IDesktopSelector* apSelector, CSharingConnection* apConnection, unsigned int quality, bool remoteAccessMode)
{
	// почистим объекты которые можно
	DestroyDeletedSessions();

	ATLASSERT( alpcTarget);
	ATLASSERT( *alpcTarget);
	ATLASSERT( apSelector);
	ATLASSERT( apConnection);
	if( !alpcTarget)
		return false;
	if( !*alpcTarget)
		return false;
	if( !apSelector)
		return false;
	if( !apConnection)
		return false;

	DestroyCapturingSession( alpcTarget, true);

	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS

	// Create capturing session
	CSharingSession*	session = CreateCapturingSession( alpcTarget, remoteAccessMode);
	ATLASSERT( session);
	if( session == NULL)
	{
		return false;
	}
	session->SetQuality(quality);
	session->SetSelector( apSelector);
	if (context->sharingMan->IsNewCodec)
		session->SetUseNewCodec();
	if(apSelector->GetType() == VIDEOFILE_SELECTOR)
	{
		if(!apSelector->Run())
			return false;
	}
	session->SetConnection( apConnection);	
	// Начало передачи данных
	session->StartSession();

	return true;
}

bool	CSharingSessionFactory::StartViewSession( LPCSTR alpcTarget, LPCSTR alpcFileName, CSharingConnection* apConnection)
{
	// почистим объекты которые можно
	DestroyDeletedSessions();

	ATLASSERT( alpcTarget);
	ATLASSERT( *alpcTarget);
	ATLASSERT( apConnection);
	if( !alpcTarget)
		return false;
	if( !*alpcTarget)
		return false;
	if( !apConnection)
		return false;
	
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	CComString mystr = alpcTarget;
	if (context->gcontext->mpLogWriter)
context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]start StartViewSession "+mystr);
	CSharingViewSession* viewSession = GetViewSessionByName(alpcTarget);
	if (viewSession != NULL && viewSession->IsFileSharingSession())
		return false;

	if (context->gcontext->mpLogWriter)
		context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]delete VS on  StartViewSession");
	DestroyViewSession( alpcTarget, true);

	// Create view session
	CSharingViewSession*	session = CreateViewSession( alpcTarget, alpcFileName);
	ATLASSERT( session);
	if( session == NULL)
	{
		return false;
	}
	
	session->SetConnection( apConnection);
	// Начало приема данных
	session->StartSession();
	if (context->gcontext->mpLogWriter)
context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]end StartViewSession "+mystr);
	return true;
}
bool CSharingSessionFactory::StartFilePlay(LPCSTR alpcTarget, LPCSTR alpFileName, unsigned int recID, unsigned int auBornRealityID, bool isPaused, CSharingConnection* apConnection, unsigned int aShiftTime, unsigned int aServerVersion)
{
	// почистим объекты которые можно
	DestroyDeletedSessions();

	if( !alpcTarget)
		return false;
	if( !*alpcTarget)
		return false;
	if( !apConnection)
		return false;
	
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	if (context->gcontext->mpLogWriter)
	context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]begin StartFilePlay");

	CComString baseSessionName;
	baseSessionName = context->sharingMan->GetNameInZeroReality(alpcTarget);

	if (!baseSessionName.IsEmpty())
	{
		if (context->gcontext->mpLogWriter)
			context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]delete VS on  StartFilePlay (baseSessionName)");
		DestroyViewSession( baseSessionName, true);
		DestroyCapturingSession( baseSessionName, true);
	}
	CComString sessionName = context->sharingMan->ConvertNameToRecordReality(alpcTarget, auBornRealityID);
	
	CSharingViewSession* session = NULL;
	session = GetViewSessionByName(sessionName.GetString());

	if (session != NULL)
	{
		if (context->gcontext->mpLogWriter)
		context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]found session with this name");
		if (session->m_isFirstStartFileSession || !session->IsDynamicTextureLive())
		{
			if (session->IsConnectionConnected())
			{
				if (context->gcontext->mpLogWriter)
				context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]found session with this name and destroy");
				DestroyViewSession( sessionName.GetString(), true);
				session = NULL;
			}
		}
	}

	if (session == NULL)
	{
		if (context->gcontext->mpLogWriter)
		context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]begin create view session");
		// Create view session
		session = CreateViewSession( sessionName.GetString(), alpFileName, recID, isPaused, aShiftTime, aServerVersion);

		if( session == NULL)
		{
			return false;
		}
		session->SetConnection( apConnection);
		if (context->gcontext->mpLogWriter)
		context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]end create view session");
	}
	else if (!session->m_isFirstStartFileSession || !session->IsConnectionConnected())
	{
		if (context->gcontext->mpLogWriter)
		context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]changed  file in session");
		CSharingSession* sharingSession = NULL;
		sharingSession = GetSharingSessionByName(sessionName.GetString());
		if (sharingSession != NULL) 
		{
			sharingSession->SetFileName(alpFileName);
			sharingSession->SetSeekFileTime(aShiftTime);
			sharingSession->SetServerVersion(aServerVersion);
		}
		session->SetFileName(alpFileName);
		session->ResetCountsOfTrying();
		session->SetSessionNotStarted();
		session->SetIsPaused(isPaused);
		session->SetSeekFileTime(aShiftTime);
		session->SetServerVersion(aServerVersion);
	}
	// Начало приема данных
	session->StartSession();
	if (context->gcontext->mpLogWriter)
	context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]end StartFilePlay");
	return true;
}

void	CSharingSessionFactory::SetViewSessionDestination( LPCSTR alpcTarget, IDesktopSharingDestination* apIDesktopSharingDestination)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS

	CSharingViewSession*	session = mapViewSessions.GetObjectByName( alpcTarget, context);
	CComString mystr = alpcTarget;
	if (context->gcontext->mpLogWriter)
	context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]VS try set destination" + mystr);
	if( session)
	{
		if (context->gcontext->mpLogWriter)
		context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]VS setting destination" + mystr);
		session->SetDestinationPointer( apIDesktopSharingDestination);
		session->SendStartSendFrames();
		CSharingSession* sharingSession = mapSharingSessions.GetObjectByName( session->GetSessionName(), context);
		if (sharingSession != NULL)
		{
			sharingSession->SetViewSession(mapViewSessions.GetObjectByName( session->GetSessionName(), context));
			sharingSession->NeedKeyFrame();
	}
}
}

void	CSharingSessionFactory::DestroyViewSession( LPCSTR alpTarget, bool withNotificationServer)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS

	CSharingViewSession*	viewSession = mapViewSessions.GetObjectByName( alpTarget, context);
	if( viewSession != NULL)
	{
		//устанавливаем ViewSession в NULL для sharingSession
		CSharingSession* sharingSession = mapSharingSessions.GetObjectByName(viewSession->GetSessionName(), context);
		if (sharingSession != NULL)
			sharingSession->SetViewSession(NULL);
		viewSession->StopSession(withNotificationServer);
		mapViewSessions.DeleteObjectByName( alpTarget);
	}
}

void	CSharingSessionFactory::DestroyViewSession( unsigned int aSessionID, bool withNotificationServer)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS

	CSharingViewSession*	viewSession = (CSharingViewSession*)GetSessionByID( aSessionID, CSessionParamsQueryIn::PLAY_SESSION_TYPE);
	if( viewSession != NULL)
	{
		//устанавливаем ViewSession в NULL для sharingSession
		CSharingSession* sharingSession = mapSharingSessions.GetObjectByName(viewSession->GetSessionName(), context);
		if (sharingSession != NULL)
			sharingSession->SetViewSession(NULL);
		viewSession->StopSession(withNotificationServer);
		mapViewSessions.DeleteObject( viewSession);
	}
}

void	CSharingSessionFactory::DestroyCapturingSession( LPCSTR alpTarget, bool withNotificationServer)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	CSharingSession*	sharingSession = mapSharingSessions.GetObjectByName( alpTarget, context);
	if( sharingSession != NULL)
	{
		if (sharingSession->GetSelector())
		{
			if (sharingSession->GetSelector()->GetType() == INTERACTIVEBOARD_SELECTOR)
			{
				CHookMouse* hookClass = CHookMouse::Instance();
				hookClass->StopHookMouse();
				hookClass->FreeInst();
			}
		}

		sharingSession->StopSession(withNotificationServer);
		mapSharingSessions.EraseObjectByName( alpTarget);
		//сессия не удалилась, тк еще работает поток захвата кадров
		if (!sharingSession->Destroy())
			deletingSharingSessions.push_back(sharingSession);
	}

}

void	CSharingSessionFactory::DestroyCapturingSession( unsigned int aSessionID, bool withNotificationServer)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS

	CSharingSession*	sharingSession = (CSharingSession*)GetSessionByID( aSessionID, CSessionParamsQueryIn::RECORD_SESSION_TYPE);
	if( sharingSession != NULL)
	{
		sharingSession->StopSession(withNotificationServer);
		mapSharingSessions.EraseObject( sharingSession);
		//сессия не удалилась, тк еще работает поток захвата кадров
		if (!sharingSession->Destroy())
			deletingSharingSessions.push_back(sharingSession);
	}	
}
// вызывается при закрытии приложения
void	CSharingSessionFactory::DestroyAllSessions()
{
	CFactoryCSHelper mutex( &csFactory);
	mapViewSessions.DeleteAllObjects();

	CSharingSession* session = NULL;
	if( (session = mapSharingSessions.GetFirst()) != NULL)
	{
		do 
		{
			//сессия не удалилась, тк еще работает поток захвата кадров
			if (!session->Destroy())
				deletingSharingSessions.push_back(session);
		} while ( (session = mapSharingSessions.GetNext()) != NULL);
	}

	mapSharingSessions.EraseAllObjects();

	DestroyDeletedSessions();

	// будет ожидать завершения потока захвата кадров на деструкторе
	for (unsigned int i =0; i < deletingSharingSessions.size(); i++)
	{
		SAFE_DELETE(deletingSharingSessions[i]);
	}
	deletingSharingSessions.clear();
}

void CSharingSessionFactory::DestroyDeletedSessions()
{
	//должен вызываться главным потоком - тк создаются окно в CSharingSession в главном потоке
	CFactoryCSHelper mutex( &csFactory);

	std::vector<CSharingSession*>		liveSharingSessions;

	for (unsigned int i =0; i < deletingSharingSessions.size(); i++)
	{
		if (deletingSharingSessions[i] != NULL)
		{
			if (!deletingSharingSessions[i]->IsLive())
			{
				SAFE_DELETE(deletingSharingSessions[i]);
			}
			else
				liveSharingSessions.push_back(deletingSharingSessions[i]);
		}
	}
	deletingSharingSessions.clear();
	deletingSharingSessions = liveSharingSessions;
}

void	CSharingSessionFactory::HandleMessage( CSharingConnection* aConnection, unsigned short aSessionID
											, BYTE aType, BYTE* aData, int aDataSize)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS

	// sharing session started (from this computer)
	if( aType == RT_SessionStarted)
	{
		CSessionParamsQueryIn	paramsIn( aData, aDataSize);
		if( !paramsIn.Analyse())
		{
			ATLASSERT( FALSE);
			return;
		}

		CSharingSessionBase*	session = mapSharingSessions.GetObjectByName( paramsIn.GetSessionName(), context);
		if( session)
		{
			session->SetSessionID( aSessionID);
			if (paramsIn.GetSessionType() == CSessionParamsQueryIn::RECORD_SESSION_TYPE)
			{
				CComString mystr = GetRandomName(session->GetSessionID());
				session->SetFileName(mystr);
				((CSharingSession*)session)->NeedKeyFrame(); // если это по разрыву связи то принудительно капчим ключевой кадр
				if (context->gcontext->mpLogWriter)
				{
					context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]file name="+mystr);
					context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]file name for save from server received");
				}
				context->sharingMan->RaiseSessionStartedEvent( session->GetSessionName(), session->GetConnection()->GetConnectionName(), session->GetFileName());
			}
			else if (paramsIn.GetSessionType() == CSessionParamsQueryIn::PLAY_SESSION_TYPE)
			{ // проигрывается записанный шаринг
				if (context->sharingMan->RecordPlaying())
					context->sharingMan->ResumeServerSharingInternal();
			}
		}
		else
		{
			if (paramsIn.GetSessionType() == CSessionParamsQueryIn::PLAY_SESSION_TYPE)
			{// проигрывается записанный шаринг
				if (!strcmp(paramsIn.GetSessionName(), ""))
					return;
				CSharingSession*	sharingSession = CreateCapturingSession(paramsIn.GetSessionName(), false);
				sharingSession->SetSessionID( aSessionID);
				CSharingViewSession* viewSession = GetViewSessionByName(sharingSession->GetSessionName());
				if (viewSession != NULL)
				{
					sharingSession->SetFileName(viewSession->GetFileName());
					sharingSession->SetRecordID(viewSession->GetRecordID());
					sharingSession->SetSeekFileTime(viewSession->GetSeekFileTime());
					sharingSession->SetIsPaused(viewSession->IsPaused());
				}

				sharingSession->SetFileSharing();
				sharingSession->SetConnection(context->connectionsFactory->GetConnectionByLocationName(context->sharingMan->GetTargetGroupNameInRecord()));
				sharingSession->SetSessionStarted();
				if (context->sharingMan->RecordPlaying())
					context->sharingMan->ResumeServerSharingInternal();
			}
		}
	}
	// view sharing session started (from this computer)
	else	if( aType == RT_ViewSessionStarted)
	{
		CSessionParamsQueryIn	paramsIn( aData, aDataSize);
		if( !paramsIn.Analyse())
		{
			ATLASSERT( FALSE);
			return;
		}
		CComString mystr= paramsIn.GetSessionName();
		if (context->gcontext->mpLogWriter)
			context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]get message RT_ViewSessionStarted "+mystr);
		CSharingSessionBase*	session = mapViewSessions.GetObjectByName( paramsIn.GetSessionName(), context);
		if( session)
		{
			session->SetSessionID( aSessionID);
			if (session->m_isFirstStartFileSession)
			{
				bool canRemoteAccess = false;
				session->m_isFirstStartFileSession = false;
				if (paramsIn.GetSessionType() == CSessionParamsQueryIn::REMOTEACCESS_SESSION_TYPE)
					canRemoteAccess = true;
				context->sharingMan->SendDestinationRequest(session->GetSessionName());
				context->sharingMan->RaiseViewSessionStartedEvent( session->GetSessionName(), session->GetConnection()->GetConnectionName(), canRemoteAccess);			

				if (context->gcontext->mpLogWriter)
					context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]start VS on message RT_ViewSessionStarted "+mystr);
			}
			else
			{
				CSharingViewSession* viewSession = GetViewSessionByName(session->GetSessionName());
				if (context->gcontext->mpLogWriter)
					context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]resume VS on message RT_ViewSessionStarted "+mystr);
				if (viewSession != NULL)
				{
					viewSession->SendStartSendFrames();
				}
			}
		}
	}
	// sharing session was not found due to StartSession request
	else	if( aType == RT_SessionNotFound)
	{
		CSessionParamsQueryIn	paramsIn( aData, aDataSize);
		if( !paramsIn.Analyse())
		{
			ATLASSERT( FALSE);
			return;
		}
		if (strcmp(paramsIn.GetSessionName(), ""))
			context->sharingMan->SessionClosedEvent(paramsIn.GetSessionName());
		if (context->gcontext->mpLogWriter)
			context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]delete VS on  RT_SessionNotFound");
		DestroyViewSession( paramsIn.GetSessionName(), false);
	}
	// sharing session was not found due to StartSession request
	else	if( aType == RT_SessionClosed)
	{
		CSessionParamsQueryIn	paramsIn( aData, aDataSize);
		if( !paramsIn.Analyse())
		{
			ATLASSERT( FALSE);
			return;
		}
		if( paramsIn.GetSessionType() == CSessionParamsQueryIn::RECORD_SESSION_TYPE)
		{
			if (strcmp(paramsIn.GetSessionName(), ""))
				context->sharingMan->SessionClosedEvent(paramsIn.GetSessionName());
			DestroyCapturingSession( aSessionID, true);
		}
		else
		{
			CSharingSessionBase*	session = mapViewSessions.GetObjectByName( paramsIn.GetSessionName(), context);
			if (session)
			{
				if (strcmp(paramsIn.GetSessionName(), ""))
					context->sharingMan->ViewSessionClosedEvent(paramsIn.GetSessionName());
			}
			if (context->gcontext->mpLogWriter)
				context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]delete VS on  RT_SessionClosed");
			DestroyViewSession( aSessionID, false);
		}
		
	}
	// sharing session with name which is send with StartSession request already exists on server
	else	if( aType == RT_SessionAlreadyExists)
	{
		CSessionParamsQueryIn	paramsIn( aData, aDataSize);
		if( !paramsIn.Analyse())
		{
			ATLASSERT( FALSE);
			return;
		}
		context->sharingMan->RaiseSharingEventWithCode( paramsIn.GetSessionName(), SHARING_SESSION_RECREATED_ON_SERVER);
		// приходит в случае если на сервере пересоздана шаринг сессия др пользователем
		// удаляем свою сессию захвата, нам придет(или уже пришло) изменение св-в объекта и запуск просмотра чужого шаринга
		if (context->gcontext->mpLogWriter)
			context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]SS recreated by not owner destroy our CapturingSession ");
		DestroyCapturingSession( paramsIn.GetSessionName(), false);
	}
	else if (aType == RT_WaitOwner)
	{
		CSessionParamsQueryIn	paramsIn( aData, aDataSize);
		if( !paramsIn.Analyse())
		{
			ATLASSERT( FALSE);
			return;
		}
		context->sharingMan->WaitOwnerEvent(paramsIn.GetSessionName());

	}
	else	if (aType == RT_StartRecording)
	{
		RecordIn recIn( aData, aDataSize);
		if ( !recIn.Analyse())
		{
			ATLASSERT( FALSE);
			return;
		}
		if (!strcmp(recIn.GetSessionName(), "") || !strcmp(recIn.GetFileName(), ""))
			return;
		CComString mystr = recIn.GetFileName();
		if (context->gcontext->mpLogWriter)
			context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]file name for save from server received "+mystr);
		context->sharingMan->RecordStartEvent(recIn.GetSessionName(), recIn.GetFileName());
	}
	else if (aType == RT_RemoteCommand)
	{
		CSharingSessionBase* session = GetSessionByID( aSessionID, CSessionParamsQueryIn::RECORD_SESSION_TYPE);
		if (session)
			session->HandleMessage( aType, aData, aDataSize);
	}
	else if (aType == RT_AllowRemoteControl)
	{
		CSharingSessionBase* session = GetSessionByID( aSessionID, CSessionParamsQueryIn::PLAY_SESSION_TYPE);
		if (session && session->GetSessionName())
			context->sharingMan->RemoteAccessModeChangedEvent(session->GetSessionName(), true);
	}
	else if (aType == RT_DenyRemoteControl)
	{
		CSharingSessionBase* session = GetSessionByID( aSessionID, CSessionParamsQueryIn::PLAY_SESSION_TYPE);
		if (session && session->GetSessionName())
			context->sharingMan->RemoteAccessModeChangedEvent(session->GetSessionName(), false);
	}
	else
	{
		CSharingSessionBase*	session = GetSessionByID( aSessionID, CSessionParamsQueryIn::PLAY_SESSION_TYPE);
		if( !session)
		{
			if (context->gcontext->mpLogWriter)
				context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]ST_MissedSessionName ");
			SharingManProtocol::CMissedNameOut	queryOut( aSessionID);
			aConnection->Send(aSessionID, ST_MissedSessionName, queryOut.GetData(), queryOut.GetDataSize());
		}
		else
		{
			session->HandleMessage( aType, aData, aDataSize);
		}
	}
}
CObjectMapByName<CSharingSession> CSharingSessionFactory::GetAllSharingSession()
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	return mapSharingSessions;
}
CObjectMapByName<CSharingViewSession> CSharingSessionFactory::GetAllViewSession()
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	return mapViewSessions;
}

CSharingSession*	CSharingSessionFactory::GetSharingSessionByName( LPCSTR alpcGroupName)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	CSharingSession* session = NULL;
	if( (session = mapSharingSessions.GetFirst()) != NULL)
	{
		do 
		{
			CComString s = "CSharingSessionFactory::GetSharingSessionByName: alpcGroupName=="; s += alpcGroupName;
			s+= "   session->GetSessionName()=="; s += session->GetSessionName();
			context->gcontext->mpLogWriter->WriteLnLPCSTR( s.GetBuffer());
			if (!strcmp(alpcGroupName, session->GetSessionName()))
				return (CSharingSession*)session;
		} while ( (session = mapSharingSessions.GetNext()) != NULL);
	}

	return NULL;
}
CSharingViewSession*	CSharingSessionFactory::GetViewSessionByName( LPCSTR alpcGroupName)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	CSharingViewSession* viewSession = NULL;
	if( (viewSession = mapViewSessions.GetFirst()) != NULL)
	{
		do 
		{
			if( !strcmp(alpcGroupName, viewSession->GetSessionName()))
				return (CSharingViewSession*)viewSession;
		} while ( (viewSession = mapViewSessions.GetNext()) != NULL);
	}

	return NULL;
}
void	CSharingSessionFactory::AddVASEPack( void* pack, bool abKeyFrame, CSharingViewSession* viewSession)
{
	//CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	viewSession->AddVASEPack((VASEFramesDifPack*)pack, abKeyFrame);
}
/************************************************************************/
/* Private
/************************************************************************/
CSharingSessionBase*	CSharingSessionFactory::GetSessionByID( unsigned short aSessionID, unsigned int aSessionType)
{
	if( aSessionType == CSessionParamsQueryIn::RECORD_SESSION_TYPE)
	{
		CSharingSession* session = NULL;
		if( (session = mapSharingSessions.GetFirst()) != NULL)
		{
			do 
			{
				if( session->GetSessionID() == aSessionID)
					return (CSharingSessionBase*)session;
			} while ( (session = mapSharingSessions.GetNext()) != NULL);
		}
	}
	else	if( aSessionType == CSessionParamsQueryIn::PLAY_SESSION_TYPE)
	{
		CSharingViewSession* viewSession = NULL;
		if( (viewSession = mapViewSessions.GetFirst()) != NULL)
		{
			do 
			{
				if( viewSession->GetSessionID() == aSessionID)
					return (CSharingSessionBase*)viewSession;
			} while ( (viewSession = mapViewSessions.GetNext()) != NULL);
		}
	}
	else
	{
		ATLASSERT( FALSE);
	}

	return NULL;
}

CSharingViewSession*	CSharingSessionFactory::CreateViewSession( LPCSTR alpcGroupName, LPCSTR alpFileName)
{
	CSharingViewSession* session = mapViewSessions.GetObjectByName( alpcGroupName, context);
	if( !session)
	{
		session = mapViewSessions.CreateObjectByName( alpcGroupName, context);
		session->SetSessionName( alpcGroupName);
		session->SetFileName( alpFileName);
	}
	return session;
}
CSharingViewSession*	CSharingSessionFactory::CreateViewSession( LPCSTR alpcGroupName, LPCSTR alpFileName, unsigned int aRecordID, bool isPaused, unsigned int aShiftTime, unsigned int aServerVersion)
{
	CSharingViewSession* session = mapViewSessions.GetObjectByName( alpcGroupName, context);
	if( !session)
	{
		session = mapViewSessions.CreateObjectByName( alpcGroupName, context);
		session->SetSessionName( alpcGroupName);
		session->SetFileName( alpFileName);
		session->SetRecordID(aRecordID);
		session->SetFileSharing();
		session->SetIsPaused(isPaused);
		session->SetSeekFileTime(aShiftTime);
		session->SetServerVersion(aServerVersion);
	}
	return session;
}

CSharingSession*		CSharingSessionFactory::CreateCapturingSession( LPCSTR alpcObjectName, bool remoteAccessMode)
{
	CSharingSession*	session = mapSharingSessions.CreateObjectByName( alpcObjectName, context);
	session->SetSessionName( alpcObjectName);
	session->SetRemoteAccessMode(remoteAccessMode);
	return session;
}

void CSharingSessionFactory::ShowSharingSessionRectangle( LPCSTR alpSessionName)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	CSharingSession *session = mapSharingSessions.GetObjectByName(alpSessionName, context);
	if (session)
        session->ShowAwarenessWnd();
}

void CSharingSessionFactory::HideSharingSessionRectangle( LPCSTR alpSessionName)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	CSharingSession *session = mapSharingSessions.GetObjectByName(alpSessionName, context);
	if (session)
        session->HideAwarenessWnd();

}

void CSharingSessionFactory::SetSharingViewFps(LPCSTR alpTarget, unsigned int fps)
{
	CFactoryCSHelper mutex( &csFactory);	// will automatically lock/unlock CS
	CSharingViewSession*	viewSession = mapViewSessions.GetObjectByName( alpTarget, context);
	if( viewSession != NULL)
	{
	//	viewSession->SetDestinationPointer(NULL);
		viewSession->SetSessionFps(fps);
		if (fps == 0) {
			if (context->gcontext->mpLogWriter)
				context->gcontext->mpLogWriter->WriteLnLPCSTR("[SharingMAN]delete VS on hide screen ");
			//меняем размер текстуры при удалении от доски
			viewSession->ResizeImage();

			//посылаем серверу, что надо прекратить слать кадры
			SharingManProtocol::HideSession HideSession(viewSession->GetSessionID(), alpTarget, true); 
			viewSession->Send(ST_HideSession, HideSession.GetData(), HideSession.GetDataSize());
			//DestroyViewSession(viewSession->GetSessionName(), true);
		}
		else if (viewSession->IsResizeApply())
		{
			viewSession->ResetResize();
			//посылаем серверу, что надо начать слать кадры
			SharingManProtocol::HideSession HideSession(viewSession->GetSessionID(), alpTarget, false); 
			viewSession->Send(ST_HideSession, HideSession.GetData(), HideSession.GetDataSize());
		}
	}
}
void CSharingSessionFactory::TryingStartFileSession(LPCSTR alpTarget)
{
	CSharingViewSession* session = GetViewSessionByName(alpTarget);
	if (session)
	{
		if (session->GetNumbersOfTryingStartFileSession() < 5)
		{
			session->IncCountsOfTrying();
			session->SetSessionNotStarted();
			session->StartSession();
		}
	}
}

CComString CSharingSessionFactory::GetRandomName(unsigned short sessionID)
{
	time_t t;
	time(&t);
	struct tm tmstruct;
	gmtime_s(&tmstruct, &t);
	char buf[100];
	
	asctime_s(buf, 100, &tmstruct);

	CComString str = buf;
	str.Delete(str.GetLength()-1, 1);

	int randomValue;
	randomValue = rand();

	
	_itoa_s(randomValue, (char*)buf, 100, 10);
	str += ".";
	str += buf;
	randomValue = rand();
	_itoa_s(randomValue, (char*)buf, 100, 10);
	str += ".";
	str += buf;
	_itoa_s(sessionID, (char*)buf, 100, 10);
	str += "_";
	str += buf;
	str += ".vas";

	for (int i =0; i < str.GetLength(); i++)
	{
		if (str[i] == ':')
			str.SetAt(i, '_');
	}

	return str;
}
CComString CSharingSessionFactory::ConvertToUnicName( LPCSTR alpTarget)
{
	return context->sharingMan->ConvertToUnicName(alpTarget);
}

CComString	CSharingSessionFactory::ConvertToUnicName( LPCSTR alpTarget, int aRealityID)
{
	return context->sharingMan->ConvertNameToRecordReality(alpTarget, aRealityID);
}
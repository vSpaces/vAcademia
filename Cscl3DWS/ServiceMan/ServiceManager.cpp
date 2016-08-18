#include "StdAfx.h"
#include "ServiceManagerBase.h"
#include "ServiceManager.h"
#include "atltime.h"

#include "GetServiceOut.h"
#include "ExecMethodOut.h"
#include "RegisterServiceOut.h"

#include "GetServiceIn.h"
#include "ExecMethodIn.h"

#include "Protocol\PreviewServerRequestQueryOut.h"
#include "Protocol\PreviewServerQueryIn.h"
#include "Protocol\PreviewServersQueryIn.h"
#include "Protocol\SharingServersQueryIn.h"

#include "Protocol\GetVoipServerForRecordQueryOut.h"
#include "Protocol\VoipServerRequestQueryOut.h"
#include "Protocol\VoipServerQueryIn.h"
#include "Protocol\VoipServersQueryIn.h"
#include "Protocol\VoipServerForRecordQueryIn.h"
#include "Protocol\RegisterRecordQueryOut.h"
#include "Protocol\UnregisterRecordQueryOut.h"
#include "Protocol\ServiceLoadingFailedIn.h"
#include "Protocol\SetSplashSrcQueryOut.h"
#include "Protocol\SendErrorMessageToAdminOut.h"
#include "Protocol\SendIPadChangeTokenOut.h"
#include "Protocol\SendIPadObjectFullStateOut.h"
#include "Protocol\SendIPadSlideSrcChangedOut.h"
#include "Protocol\SendIPadLogoutOut.h"

#include "VoipManClient.h"
#include "IDesktopMan.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "DataBuffer.h"

using namespace ServiceManProtocol;


CServiceManager::CServiceManager( omsContext* aContext):CServiceManagerBase(aContext)
{
	if (aContext != NULL)
		aContext->mpServiceMan = this;
	MP_NEW_V(pIPadManager, CIPadManager, aContext);
}

CServiceManager::~CServiceManager()
{	
	MP_DELETE( pIPadManager);	
	Disconnect();	
	
}

void CServiceManager::Destroy()
{
	MP_DELETE_THIS;
}

// ----------------------------------------------------	
// Реадизация ф-й интерфейса IBaseServiceManager
// ----------------------------------------------------	
// Подключись к серверу сервисов
void CServiceManager::Connect()
{
	CServiceManagerBase::Connect(this, this);
}

// Отключись от сервера сервисов
void CServiceManager::Disconnect()
{
	CServiceManagerBase::Disconnect();
}

// Удалить менеджер синхронизации
void CServiceManager::Release()
{
	CServiceManagerBase::Release();
}

// Получить описание 
omsresult CServiceManager::GetJSONService(unsigned int aRmmlID, const wchar_t* apwcServiceName)
{
	return CServiceManagerBase::GetJSONService(aRmmlID, apwcServiceName);
}

// Вызвать метод
omsresult CServiceManager::ExecMethod(int requestID, unsigned int rmmlID, const wchar_t* apwcJSONMethodDescription)
{
	return CServiceManagerBase::ExecMethod(requestID, rmmlID, apwcJSONMethodDescription);
}

// Установить интерфейс, принимающий обратные вызовы от SyncManager-а
bool CServiceManager::SetCallbacks(IServiceManCallbacks* apCallbacks)
{
	return CServiceManagerBase::SetCallbacks(apCallbacks);
}
// Подтвердить регистрацию сервиса на клиенте
omsresult CServiceManager::RegisterService(int pendingID, unsigned int rmmlID)
{
	return CServiceManagerBase::RegisterService(pendingID, rmmlID);
}

/*// отослать на сервер, не отосланные сообщения сервисов
void CServiceManager::SendAllNotSendingServiceMessage()
{
	CServiceManagerBase::SendAllNotSendingServiceMessage();
}*/

omsresult CServiceManager::SendPreviewServerRequest( unsigned int aiRecordID)
{
	CPreviewServerRequestQueryOut queryOut( aiRecordID);
	bool result = SendOnMyClientSession(EST_SendQuery,  ST_PreviewServerRequest, queryOut.GetData(), queryOut.GetDataSize());
	if (!result)
		return OMS_ERROR_NOT_INITIALIZED;
	return OMS_OK;
}

// Получить адрес Voip сервера
omsresult CServiceManager::SendVoipServerRequest( const char* alpcRoomName)
{
	if( !alpcRoomName)
		return OMS_ERROR_FAILURE;

	CVoipServerRequestQueryOut	queryOut( alpcRoomName);
	bool result = SendOnMyClientSession(EST_SendQuery, ST_VoipServerRequest, queryOut.GetData(), queryOut.GetDataSize());
	if (!result)
		return OMS_ERROR_NOT_INITIALIZED;
	return OMS_OK;
}

omsresult CServiceManager::SendSharingServerRequest( const char* alpcRoomName)
{
	if( !alpcRoomName)
		return OMS_ERROR_FAILURE;

	CVoipServerRequestQueryOut	queryOut( alpcRoomName);
	bool result = SendOnMyClientSession(EST_SendQuery, ST_SharingServerRequest, queryOut.GetData(), queryOut.GetDataSize());
	if (!result)
		return OMS_ERROR_NOT_INITIALIZED;

	return OMS_OK;
}

// Зарегистировать запись голосового сервера
omsresult CServiceManager::SendVoipRecordRegisterRequest( const char* alpcServerIP, const char* alpcRoomName, unsigned int aiRecordID)
{
	CRegisterRecordQueryOut	queryOut( alpcServerIP, aiRecordID);
	bool result = SendOnMyClientSession(EST_Send, ST_RegisterRecord, queryOut.GetData(), queryOut.GetDataSize());
	if (!result)
		return OMS_ERROR_NOT_INITIALIZED;

	return OMS_OK;
}

omsresult CServiceManager::SendVoipGetPlayServerRequest( unsigned int aiPlayRealityID,
														unsigned int aiPlayingRecordID)
{
	CGetVoipServerForRecordQueryOut	queryOut( aiPlayRealityID, aiPlayingRecordID);
	bool result = SendOnMyClientSession(EST_SendQuery, ST_VoipServerForRecordRequest, queryOut.GetData(), queryOut.GetDataSize());

	if (!result)
		return OMS_ERROR_NOT_INITIALIZED;
	return OMS_OK;
}

// установить splashSrc
void CServiceManager::SetSplashSrc( unsigned int aiRecordID, const char* alpcFileName)
{
	SetSplashSrcQueryOut	queryOut( aiRecordID, alpcFileName);
	SendOnMyClientSession(EST_Send, ST_SetSplashSrc, queryOut.GetData(), queryOut.GetDataSize());
}

void CServiceManager::SendErrorMessageToAdmin( const wchar_t *apErrorText)
{
	SendErrorMessageToAdminOut	queryOut( apErrorText);
	SendOnMyClientSession(EST_Send, ST_SendErrorMessageToAdmin, queryOut.GetData(), queryOut.GetDataSize());
}

void CServiceManager::SendIPadChangeToken( unsigned int aToken)
{
	CSendIPadChangeTokenOut queryOut( aToken);
	SendOnMyClientSession(EST_Send, ST_SendIPadChangeToken, queryOut.GetData(), queryOut.GetDataSize());
}

void CServiceManager::SendIPadObjectFullState(  unsigned int aPacketID, unsigned int aToken, BYTE* aBinState, int aBinStateSize)
{
	CSendIPadObjectFullStateOut queryOut( aPacketID, aToken, aBinState, aBinStateSize);
	SendOnMyClientSession(EST_Send, ST_SendIPadObjectFullState, queryOut.GetData(), queryOut.GetDataSize());
}

void CServiceManager::SendIPadSlideSrcChangedOut( unsigned int aPacketID, unsigned int aToken, float aProportion, float aScaleX, float aScaleY, const wchar_t *aSrc)
{
	CSendIPadSlideSrcChangedOut queryOut( aPacketID, aToken, aProportion, aScaleX, aScaleY, aSrc);
	SendOnMyClientSession(EST_Send, ST_SendIPadSlideSrcChangedOut, queryOut.GetData(), queryOut.GetDataSize());
}

void CServiceManager::SendIPadLogoutOut( int aCode)
{
	CSendIPadLogoutOut queryOut( aCode);
	SendOnMyClientSession(EST_Send, ST_SendIPadLogoutOut, queryOut.GetData(), queryOut.GetDataSize());
}


void CServiceManager::OnPreviewServerRequest(BYTE* aData, int aDataSize)
{
	if( !pContext->mpPreviewRecorder)
		return;

	CPreviewServerQueryIn messageIn(aData, aDataSize);
	if( !messageIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CPreviewServersQueryIn");
		ATLASSERT( false);
		ATLASSERT( FALSE);
	}
	else
	{
		pContext->mpPreviewRecorder->SetPreviewServerAddress(messageIn.GetRecordID()
															, messageIn.GetPreviewServerIP().GetBuffer()
															, messageIn.GetTcpPort()
															, messageIn.GetPreviewHttpHost().GetBuffer()
															, messageIn.GetStoragePath().GetBuffer());
	}
}

/*void CServiceManager::OnPreviewServersRequest(BYTE* aData, int aDataSize)
{
	if( !pContext->mpPreviewRecorder)
		return;

	CPreviewServersQueryIn messageIn(aData, aDataSize);
	if( !messageIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CPreviewServerQueryIn");
		ATLASSERT( FALSE);
	}
	else
	{
	}
}*/

void CServiceManager::OnSharingServerRequest( BYTE* aData, int aDataSize)
{
	if( !pContext->mpSharingMan)
		return;

	CSharingServersQueryIn	queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CSharingServersQueryIn");
		ATLASSERT( FALSE);
		return;
	}

	USES_CONVERSION;
	pContext->mpSharingMan->SetSharingServerAddress( W2A(queryIn.GetGroupName().c_str())
													, W2A(queryIn.GetSharingServerIP().c_str())
													, queryIn.GetTcpPort());
}

void CServiceManager::OnVoipServerRequest( BYTE* aData, int aDataSize)
{
	if( !pContext->mpVoipMan)
		return;

	CVoipServerQueryIn	queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CVoipServerQueryIn");
		ATLASSERT( FALSE);
		return;
	}

	pContext->mpVoipMan->SetVoipRecordServerAddress( queryIn.GetVoipHostName()
		, queryIn.GetUdpPort()
		, queryIn.GetRecordHostName()
		, queryIn.GetRecordPort()
		, queryIn.GetRoomName());
}

/*void CServiceManager::OnVoipServersRequest( BYTE* aData, int aDataSize)
{
	if( !pContext->mpVoipMan)
		return;

	CVoipServersQueryIn	queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CVoipServersQueryIn");
		ATLASSERT( FALSE);
		return;
	}
}*/

void CServiceManager::OnVoipServerForRecordRequest( BYTE* aData, int aDataSize)
{
	if( !pContext->mpVoipMan)
		return;

	CVoipServerForRecordQueryIn	queryIn( aData, aDataSize);
	if( !queryIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CVoipServerForRecordQueryIn");
		ATLASSERT( FALSE);
		return;
	}

	pContext->mpVoipMan->SetVoipPlayServerAddress( queryIn.GetRecordID()
									, queryIn.GetRecordServerIP()
									, queryIn.GetRecordServerPort()
									, queryIn.GetPlayServerIP()
									, queryIn.GetPlayServerPort()
									);
}

void CServiceManager::OnIPadMessage( BYTE aType, BYTE* aData, int aDataSize, DWORD aCheckSum)
{
	if( pContext->mpIPadMan == NULL)
		return;
	pContext->mpIPadMan->OnIPadMessage( aType, aData, aDataSize);
}


void CServiceManager::OnConnectLost()
{
	if(pContext!=NULL && pContext->mpSM!=NULL)
		pContext->mpSM->OnServerDisconnected(rmml::ML_SERVER_MASK_SERVICE, NULL);
}

void CServiceManager::OnConnectRestored()
{
	if(pCallbacks!=NULL)
		pCallbacks->onConnectRestored();	
	if(pContext!=NULL && pContext->mpSM!=NULL) 
		pContext->mpSM->OnServerConnected(rmml::ML_SERVER_MASK_SERVICE, true);
	pIPadManager->OnConnectRestored();
}

#include "StdAfx.h"
#include "ServiceManager.h"
#include "atltime.h"

#include "GetServiceOut.h"
#include "ExecMethodOut.h"
#include "RegisterServiceOut.h"

#include "GetServiceIn.h"
#include "ExecMethodIn.h"
#include "NotifyMethodIn.h"

#include "Protocol\PreviewServerRequestQueryOut.h"
#include "Protocol\PreviewServersQueryIn.h"

#include "Protocol\GetVoipServerForRecordQueryOut.h"
#include "Protocol\VoipServerRequestQueryOut.h"
#include "Protocol\VoipServersQueryIn.h"
#include "Protocol\VoipServerForRecordQueryIn.h"
#include "Protocol\RegisterRecordQueryOut.h"
#include "Protocol\UnregisterRecordQueryOut.h"
#include "Protocol\ServiceLoadingFailedIn.h"
#include "Protocol\SetSplashSrcQueryOut.h"

#include "VoipManClient.h"
#include "imessagehandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "DataBuffer.h"


using namespace ServiceManProtocol;


CServiceManagerBase::CServiceManagerBase( omsContext* aContext)
{
	pContext = aContext;
	pClientSession = NULL;
	pCallbacks = NULL;
	disconnectRequired = false;
	connectionState = ECS_DISCONNECTED;
	csFactory.Init();
}

CServiceManagerBase::~CServiceManagerBase()
{
	Disconnect();
	csFactory.Term();
}

bool CServiceManagerBase::BeforeConnect()
{
	if (pContext == NULL || pContext->mpComMan == NULL)
		return false;

	Disconnect();

	csFactory.Lock();
	disconnectRequired = false;
	if( connectionState == ECS_CONNECTED || connectionState == ECS_CONNECTING)
	{
		csFactory.Unlock();
		return false;
	}
	connectionState = ECS_CONNECTING;
	csFactory.Unlock();

	return true;
}

void CServiceManagerBase::Connect(cs::imessagehandler*  handler, cs::isessionlistener *sessionListener)
{
	ConnectImpl( handler, sessionListener, false);
}

void CServiceManagerBase::ConnectWorldServer(cs::imessagehandler*  handler, cs::isessionlistener *sessionListener)
{
	ConnectImpl( handler, sessionListener, true);
}

void CServiceManagerBase::ConnectImpl(cs::imessagehandler*  handler, cs::isessionlistener *sessionListener, bool abIsWorldServer)
{
	if (pContext == NULL || pContext->mpComMan == NULL)
		return;

	Disconnect();

	csFactory.Lock();
	disconnectRequired = false;

	// если соединение уже есть  - просто выходим
	if( connectionState == ECS_CONNECTED)
	{
		csFactory.Unlock();
		return;
	}
	else if( connectionState == ECS_CONNECTING) 	// если соединяется в другом потоке, ждем завершения
	{
		csFactory.Unlock();

		while( true)
		{
			csFactory.Lock();
			if( (connectionState != ECS_CONNECTING) || disconnectRequired)
			{
				csFactory.Unlock();
				return;
			}
			csFactory.Unlock();
			Sleep( 100);
		}
	}
	else
	{
		connectionState = ECS_CONNECTING;
		csFactory.Unlock();
	}

	cs::iclientsession* apClientSession = NULL;
	if( abIsWorldServer)
		pContext->mpComMan->CreateServiceWorldServerSession( &apClientSession);
	else
		pContext->mpComMan->CreateServiceServerSession( &apClientSession);

	SetSessionImpl( apClientSession, handler, sessionListener);
}

void CServiceManagerBase::SetSessionImpl(cs::iclientsession* apClientSession, cs::imessagehandler*  handler, cs::isessionlistener *sessionListener)
{
	csFactory.Lock();

	pClientSession = apClientSession;
	connectionState = ECS_CONNECTED;

	if( disconnectRequired)
	{
		Disconnect();
		csFactory.Unlock();
		return;
	}

	pClientSession->SetMessageHandler( handler);
	pClientSession->SetSessionListener(sessionListener);

	csFactory.Unlock();
}

void CServiceManagerBase::Disconnect()
{
	if (pContext == NULL || pContext->mpComMan == NULL)
		return;

	csFactory.Lock();
	if( connectionState == ECS_DISCONNECTED)
	{
		csFactory.Unlock();
		return;
	}
	if( connectionState == ECS_CONNECTING)
	{
		disconnectRequired = true;
		csFactory.Unlock();
		return;
	}
	cs::iclientsession* apClientSession = pClientSession;
	pClientSession = NULL;

	pContext->mpComMan->DestroyServerSession( &apClientSession);

	connectionState = ECS_DISCONNECTED;

	csFactory.Unlock();
}

void CServiceManagerBase::Release()
{
	MP_DELETE_THIS;
}

omsresult CServiceManagerBase::GetJSONService(unsigned int aRmmlID, const wchar_t* apwcServiceName)
{
	CGetServiceOut outData(aRmmlID, apwcServiceName);
	bool result = SendOnMyClientSession(EST_SendQuery, ST_GetService, outData.GetData(), outData.GetDataSize());

	if (!result)
		return OMS_ERROR_NOT_INITIALIZED;

	return OMS_OK;
}

// Подтвердить регистрацию сервиса на клиенте
omsresult CServiceManagerBase::RegisterService(int pendingID, unsigned int rmmlID)
{
	CRegisterServiceOut outData(pendingID, rmmlID);
	bool result = SendOnMyClientSession(EST_Send, ST_RegisterService, outData.GetData(), outData.GetDataSize());
	if (!result)
		return OMS_ERROR_NOT_INITIALIZED;
	return OMS_OK;
}

omsresult CServiceManagerBase::ExecMethod(int requestID, unsigned int rmmlID, const wchar_t* apwcJSONMethodDescription)
{
	CExecMethodOut outData(requestID, rmmlID, apwcJSONMethodDescription);
	bool result = SendOnMyClientSession(EST_SendQuery, ST_ExecMethod, outData.GetData(), outData.GetDataSize());
	if (!result)
		return OMS_ERROR_NOT_INITIALIZED;

	return OMS_OK;
}


// Установить интерфейс, принимающий обратные вызовы от ServiceManager-а
bool CServiceManagerBase::SetCallbacks(service::IServiceManCallbacks* apCallbacks)
{
	pCallbacks = apCallbacks;
	return true;
}

void CServiceManagerBase::OnGetService( BYTE* aData, int aDataSize)
{
	CGetServiceIn serviceInfo(aData, aDataSize);
	if(!serviceInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CGetServiceIn");
		ATLASSERT( FALSE);
		return;
	}

	if (pCallbacks != NULL)
		pCallbacks->onGetService(serviceInfo.getRmmlID(), serviceInfo.GetServiceName(),serviceInfo.GetServiceDescription());
}

// Возвращает в рммл результат работы метода (не нотификация)
void CServiceManagerBase::OnExecMethodResults( BYTE* aData, int aDataSize)
{	CExecMethodIn serviceInfo(aData, aDataSize);
	if(!serviceInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CExecMethodIn ::OnExecMethodResults");
		ATLASSERT( FALSE);
		return;
	}

// Возвращаемся в rmml
	unsigned int iHTTPRequestID = serviceInfo.getRequestID();
	//clearServicePacket(iHTTPRequestID);
	int iError=0;
	if (pCallbacks != NULL)
		pCallbacks->onMethodComplete(iHTTPRequestID, serviceInfo.getRmmlID(), serviceInfo.GetMethodResult(),false, iError);
}

// Возвращает в рммл сервис
void CServiceManagerBase::OnGetServiceByMethod( BYTE* aData, int aDataSize)
{
	CExecMethodIn serviceInfo(aData, aDataSize);
	if(!serviceInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CExecMethodIn ::OnGetServiceByMethod");
		ATLASSERT( FALSE);
		return;
	}

	unsigned int iHTTPRequestID = serviceInfo.getRequestID();
	//clearServicePacket(iHTTPRequestID);
	// Возвращаемся в rmml
	int iError=0;
	if (pCallbacks != NULL)
		pCallbacks->onMethodComplete(iHTTPRequestID, serviceInfo.getRmmlID(), serviceInfo.GetMethodResult(),true, iError);
}

void CServiceManagerBase::OnServiceSendNotify( BYTE* aData, int aDataSize)
{
	CNotifyMethodIn serviceInfo(aData, aDataSize);
	if(!serviceInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CNotifyMethodIn");
		ATLASSERT( FALSE);
		return;
	}

	int iError=0;
	if (pCallbacks != NULL)
		pCallbacks->onServiceNotify(serviceInfo.getRmmlID(), serviceInfo.GetMethodResult(), 0);
}

void CServiceManagerBase::OnServiceLoadingFailed( BYTE* aData, int aDataSize)
{
	CServiceLoadingFailedIn serviceInfo(aData, aDataSize);
	if(!serviceInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CServiceLoadingFailedIn");
		ATLASSERT( FALSE);
		return;
	}

	int iError=1;
	if (pCallbacks != NULL)
		pCallbacks->onGetService(serviceInfo.getRmmlID(), L"",L"", &iError);
	
// Пока не дописано
// !!!!!!!!!!!!!!!!!!!!!!
}


void CServiceManagerBase::OnConnectLost()
{
}

void CServiceManagerBase::OnConnectRestored()
{
}

bool CServiceManagerBase::SendOnMyClientSession( BYTE aSendType, BYTE aType, const BYTE FAR *aData, int aDataSize)
{
	csFactory.Lock();
	if( pClientSession == NULL)
	{
		csFactory.Unlock();
		return false;
	}
	bool result = true;
	if (aSendType == EST_SendSignalQuery)
		pClientSession->SendSignalQuery( aType, aData, aDataSize);
	else if (aSendType == EST_SendQuery)
		pClientSession->SendQuery( aType, aData, aDataSize);
	else if (aSendType == EST_Send) {
		pClientSession->Send( aType, aData, aDataSize);
	}
	else {
		result = false;
	}
	
	csFactory.Unlock();
	return result;
}
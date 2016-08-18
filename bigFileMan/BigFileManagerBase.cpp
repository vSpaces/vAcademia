#include "StdAfx.h"
#include "ServiceManager.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "DataBuffer.h"


using namespace ServiceManProtocol;


CBigFileManagerBase::CBigFileManagerBase( omsContext* aContext)
{
	pContext = aContext;
	pClientSession = NULL;
	pCallbacks = NULL;
	//hReadySendEvent = CreateEvent( NULL, TRUE, FALSE, NULL);
	isConnected = false;
}

CBigFileManagerBase::~CBigFileManagerBase()
{
	Disconnect();
	//CloseHandle( hReadySendEvent);
	isConnected = false;
}

bool CBigFileManagerBase::BeforeConnect()
{
	if (pContext == NULL || pContext->mpComMan == NULL)
		return false;

	Disconnect();
	return true;
}

void CBigFileManagerBase::Connect(cs::imessagehandler*  handler, cs::isessionlistener *sessionListener)
{
	if (!BeforeConnect()) 
		return;
	pContext->mpComMan->CreateServiceServerSession( &pClientSession);
	AfterConnect(handler, sessionListener);
	isConnected = true;
}

void CBigFileManagerBase::AfterConnect(cs::imessagehandler*  handler, cs::isessionlistener *sessionListener)
{
	if (pClientSession == NULL)
		return;

	pClientSession->SetMessageHandler( handler);
	pClientSession->SetSessionListener(sessionListener);
}

void CBigFileManagerBase::ConnectWorldServer(cs::imessagehandler*  handler, cs::isessionlistener *sessionListener)
{
	if (!BeforeConnect()) 
		return;
	pContext->mpComMan->CreateServiceWorldServerSession( &pClientSession);
	AfterConnect(handler, sessionListener);
	isConnected = true;
}

void CBigFileManagerBase::Disconnect()
{
	if (pContext == NULL || pContext->mpComMan == NULL || pClientSession == NULL)
		return;
	if (!isConnected)
		return;
	pContext->mpComMan->DestroyServerSession( &pClientSession);
	pClientSession = NULL;
	mutex.lock();
	/*std::map<unsigned int, CBigFileQuery *>::iterator iter = oServiceQueryMap.begin(), iter_end = oServiceQueryMap.end();
	for ( ; iter != iter_end; ++iter)
	{
		CBigFileQuery *serviceQuery = (CBigFileQuery *) iter->second;
		if ( serviceQuery)
		{
			delete serviceQuery;
			serviceQuery = NULL;
		}
	}
	oServiceQueryMap.clear();*/
	mutex.unlock();
	isConnected = false;
}

void CBigFileManagerBase::Release()
{
	delete this;
}

omsresult CBigFileManagerBase::GetJSONService(unsigned int aRmmlID, const wchar_t* apwCBigFileName)
{
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CGetServiceOut outData(aRmmlID, apwCBigFileName);

	pClientSession->SendQuery( ST_GetService, outData.GetData(), outData.GetDataSize());

	return OMS_OK;
}

// Подтвердить регистрацию сервиса на клиенте
omsresult CBigFileManagerBase::RegisterService(int pendingID, unsigned int rmmlID)
{
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CRegisterServiceOut outData(pendingID, rmmlID);

	pClientSession->Send( ST_RegisterService, outData.GetData(), outData.GetDataSize());
	return OMS_OK;
}

omsresult CBigFileManagerBase::ExecMethod(int requestID, unsigned int rmmlID, const wchar_t* apwcJSONMethodDescription)
{
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CExecMethodOut outData(requestID, rmmlID, apwcJSONMethodDescription);
	
	//addServicePacket(ST_ExecMethod, requestID, outData.GetData(), outData.GetDataSize());

	pClientSession->SendQuery( ST_ExecMethod, outData.GetData(), outData.GetDataSize());
	return OMS_OK;
}


// Установить интерфейс, принимающий обратные вызовы от ServiceManager-а
bool CBigFileManagerBase::SetCallbacks(service::IServiceManCallbacks* apCallbacks)
{
	pCallbacks = apCallbacks;
	return true;
}

void CBigFileManagerBase::OnGetService( BYTE* aData, int aDataSize)
{
	CGetServiceIn serviceInfo(aData, aDataSize);
	if(!serviceInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CGetServiceIn");
		return;
	}

	if (pCallbacks != NULL)
		pCallbacks->onGetService(serviceInfo.getRmmlID(), serviceInfo.GetServiceName(),serviceInfo.GetServiceDescription());
}

// Возвращает в рммл результат работы метода (не нотификация)
void CBigFileManagerBase::OnExecMethodResults( BYTE* aData, int aDataSize)
{	CExecMethodIn serviceInfo(aData, aDataSize);
	if(!serviceInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CExecMethodIn ::OnExecMethodResults");
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
void CBigFileManagerBase::OnGetServiceByMethod( BYTE* aData, int aDataSize)
{
	CExecMethodIn serviceInfo(aData, aDataSize);
	if(!serviceInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CExecMethodIn ::OnGetServiceByMethod");
		return;
	}

	unsigned int iHTTPRequestID = serviceInfo.getRequestID();
	//clearServicePacket(iHTTPRequestID);
	// Возвращаемся в rmml
	int iError=0;
	if (pCallbacks != NULL)
		pCallbacks->onMethodComplete(iHTTPRequestID, serviceInfo.getRmmlID(), serviceInfo.GetMethodResult(),true, iError);
}

void CBigFileManagerBase::OnServiceSendNotify( BYTE* aData, int aDataSize)
{
	CNotifyMethodIn serviceInfo(aData, aDataSize);
	if(!serviceInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CNotifyMethodIn");
		return;
	}

	int iError=0;
	if (pCallbacks != NULL)
		pCallbacks->onServiceNotify(serviceInfo.getRmmlID(), serviceInfo.GetMethodResult(), 0);
}

void CBigFileManagerBase::OnServiceLoadingFailed( BYTE* aData, int aDataSize)
{
	CBigFileLoadingFailedIn serviceInfo(aData, aDataSize);
	if(!serviceInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CBigFileLoadingFailedIn");
		return;
	}

	int iError=1;
	if (pCallbacks != NULL)
		pCallbacks->onGetService(serviceInfo.getRmmlID(), L"",L"", &iError);
	
// Пока не дописано
// !!!!!!!!!!!!!!!!!!!!!!
}


void CBigFileManagerBase::OnConnectLost()
{
}

void CBigFileManagerBase::OnConnectRestored()
{
}

/*void CBigFileManagerBase::addServicePacket(unsigned int aTypeMessage, unsigned int aiPacketID, BYTE *aData, unsigned int dwSize)
{
	mutex.lock();
	CBigFileQuery *pServiceQuery = new CBigFileQuery(aTypeMessage, aData, dwSize);
	oServiceQueryMap.insert( std::pair<unsigned int, CBigFileQuery *>( aiPacketID, pServiceQuery));	
	//oServicesQueryVector.push_back(pServiceQuery);	
	mutex.unlock();
}

void CBigFileManagerBase::clearServicePacket(unsigned int aiPacketID)
{
	mutex.lock();
	if( oServiceQueryMap.find( aiPacketID) != oServiceQueryMap.end())
	{
		CBigFileQuery *pServiceQuery = (CBigFileQuery  *) oServiceQueryMap[ aiPacketID];		
		if ( pServiceQuery)
		{
			delete pServiceQuery;
			pServiceQuery = NULL;
		}
		oServiceQueryMap.erase(aiPacketID);
	}

	mutex.unlock();
}*/

/*DWORD WINAPI CBigFileManagerBase::SendAllNotSendingServiceMessage_( VOID* param)
{
	((CBigFileManagerBase*)param)->SendAllNotSendingServiceMessage();
	return 0;
}

void CBigFileManagerBase::SendAllNotSendingServiceMessage()
{
	while(WaitForSingleObject( hReadySendEvent, 7000) != WAIT_FAILED)
	{
		mutex.lock();
		std::map<unsigned int, CBigFileQuery *>::iterator iter = oServiceQueryMap.begin(), iter_end = oServiceQueryMap.end();
		for ( ; iter != iter_end; ++iter)
		{
			CBigFileQuery *serviceQuery = (CBigFileQuery *) iter->second;
			if ( serviceQuery)
			{
				pClientSession->Send( serviceQuery->typeMessage, serviceQuery->GetData(), serviceQuery->GetDataSize());
				delete serviceQuery;
				serviceQuery = NULL;
			}
		}
		oServiceQueryMap.clear();
		mutex.unlock();
		break;
	}
}*/
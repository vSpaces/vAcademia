#include "StdAfx.h"
#include "IPadManager.h"

//#include "DataBuffer.h"
//#include <ctime>
#include "Protocol\IPadSessionStartedIn.h"
#include "Protocol\IPadSessionStopedIn.h"
#include "Protocol\IPadSessionStatusIn.h"
#include "Protocol\IPadSessionBinStateIn.h"
#include "Protocol\IPadSessionChangeSlideIn.h"
#include <time.h>
//#include "../SyncMan/SyncMan.h"

using namespace ServiceManProtocol;
#include "../res/res.h"
using namespace res;

CIPadManager::CIPadManager( omsContext *aContext)
{
	pContext = aContext;
	if (aContext != NULL)
		aContext->mpIPadMan = this;
	bIPadConnected = false;
	packetID = 0;
	objectID = -1;
	bornRealityID = -1;
	realityID = -1;
	char pchValue[3000] = "";
	if(pContext->mpApp)
		pContext->mpApp->GetSettingFromIni("paths", "server", pchValue, 2995);
	USES_CONVERSION;
	serverIP = A2W( pchValue);
	pImgObject = NULL;

	prop = 1;
	scaleFromAspectX = 1;
	scaleFromAspectY = 1;
}

CIPadManager::~CIPadManager()
{
}

void CIPadManager::StartSession( wchar_t *aName, unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID, void *aImgObject)
{
	GenerateToken();
	SetObjectParam( aName, aObjectID, aBornRealityID, aRealityID, ( moMedia *)aImgObject);	
	//bIPadConnected = true;
}

void CIPadManager::DestroySession()
{
	bIPadConnected = false;
	SetObjectParam( NULL, 0, 0, 0, NULL);	
}

void CIPadManager::SetObjectParam( wchar_t *aName, unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID, moMedia *aImgObject)
{	
	bIPadConnected = false;
	if(aName != NULL)
		sObjName = aName;
	else
		sObjName.Empty();
	objectID = aObjectID;
	bornRealityID = aBornRealityID;
	realityID = aRealityID;
	if( aImgObject != NULL)
	{
		pImgObject = aImgObject->GetIImage();
		pImgObject->SetIPadDrawing( true);
		if( bIPadConnected)			
			SendFullState();
		else
		{
			pContext->mpServiceMan->SendIPadChangeToken( token);
		}
	}
	else
	{
		if( pImgObject != NULL)
			pImgObject->SetIPadDrawing( false);
		pImgObject = NULL;
	}
	bIPadConnected = false;
}

bool CIPadManager::IsIPadObject( wchar_t *aName, unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aRealityID)
{
	if( sObjName.IsEmpty() || objectID == 0 || objectID == -1 || bornRealityID == -1 || realityID == -1)
		return false;
	if( sObjName == aName && objectID == aObjectID && bornRealityID == aBornRealityID && realityID == aRealityID)
		return true;
	return false;
}

void CIPadManager::IncPacketID()
{
	mutex.lock();
	packetID++;
	mutex.unlock();
}

void CIPadManager::GenerateToken()
{
	srand( (unsigned int)time(NULL));
	token = rand() + 1000;
}

void CIPadManager::SendFullState()
{
	if( pImgObject != NULL)
	{
		if( pContext->mpServiceMan != NULL)
		{
			IncPacketID();			
			byte *binState = NULL;
			int binStateSize;
			pImgObject->GetBinState( binState, binStateSize);
			pContext->mpServiceMan->SendIPadSlideSrcChangedOut( packetID, token, prop, scaleFromAspectX, scaleFromAspectY, src.GetBuffer());
			pContext->mpServiceMan->SendIPadObjectFullState(packetID, token, binState, binStateSize);
		}
	}
	else
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[IPAD WARNING] SendFullState pImgObject == NULL");
	}
}

void CIPadManager::SendForceFullState()
{
	GenerateToken();
	SendFullState();
}

void CIPadManager::SetIPadImageParam( const wchar_t *apSrc, float aProportion, float aScaleX, float aScaleY)
{	
	if( apSrc != NULL && wcslen(apSrc)>0)
	{
		src = pContext->mpResM->GetFullPath( apSrc, RES_REMOTE);
		src.Replace(L"\\", L"/");	
		src.Insert(0, L"/");	
		src.Insert(0, serverIP.GetBuffer());
		src.Replace(L"//", L"/");
		src.Insert(0, L"http://");
	}
	else
		src.Empty();

	prop = aProportion;
	scaleFromAspectX = aScaleX;
	scaleFromAspectY = aScaleY;

	if(bIPadConnected && pContext->mpServiceMan != NULL)
	{		
		IncPacketID();
		pContext->mpServiceMan->SendIPadSlideSrcChangedOut( packetID, token, prop, scaleFromAspectX, scaleFromAspectY, src.GetBuffer());
	}	
}

void CIPadManager::OnConnectRestored()
{
	if( objectID == 0 || objectID == -1 || bornRealityID == -1 || realityID == -1)
		return;
	if (pContext->mpServiceMan != NULL)
		pContext->mpServiceMan->SendIPadChangeToken( token);
}

void CIPadManager::OnIPadSessionStarted( unsigned int aToken)
{
	//srand( time(NULL));
	//token = rand() + 1000;
	bIPadConnected = true;
	SendFullState();
	//pContext->mpServiceMan->SendIPadChangeToken( token);
	//token = aToken;
}

void CIPadManager::OnIPadSessionStoped()
{
	//bIPadConnected = false;
	//SetObjectParam( NULL, 0, 0, 0, NULL);
}

void CIPadManager::OnIPadSessionStatus( int aStatus)
{
	status = aStatus;
}

void CIPadManager::OnIPadSessionBinState( unsigned char type, unsigned char *aData, unsigned int aDataSize)
{	
	if(pImgObject == NULL)
		return;
	pImgObject->ChangeState( type, aData, aDataSize);
}


void CIPadManager::OnIPadMessage( BYTE aType, BYTE* aData, int aDataSize)
{
	switch(aType)
	{
	case RT_IPadSessionStarted:
		{
			CIPadSessionStartedIn	queryIn( aData, aDataSize);
			if( !queryIn.Analyse())
			{
				if(pContext->mpLogWriter!=NULL)
					pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CIPadSessionStartedIn");
				ATLASSERT( FALSE);
				return;
			}
			OnIPadSessionStarted( queryIn.GetToken());
			break;
		}
	case RT_IPadSessionStoped:
		{
			/*CIPadSessionStopedIn	queryIn( aData, aDataSize);
			if( !queryIn.Analyse())
			{
				if(pContext->mpLogWriter!=NULL)
					pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CIPadSessionStartedIn");
				ATLASSERT( FALSE);
				return;
			}*/
			OnIPadSessionStoped();
			break;
		}
	case RT_IPadSessionStatus:
		{
			CIPadSessionStatusIn	queryIn( aData, aDataSize);
			if( !queryIn.Analyse())
			{
				if(pContext->mpLogWriter!=NULL)
					pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CIPadSessionStatusIn");
				ATLASSERT( FALSE);
				return;
			}			
			OnIPadSessionStatus( queryIn.getStatus());
			break;
		}
	case RT_IPadSessionBinState:
		{
			CIPadSessionBinStateIn	queryIn( aData, aDataSize);
			if( !queryIn.Analyse())
			{
				if(pContext->mpLogWriter!=NULL)
					pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CIPadSessionBinStateIn");
				ATLASSERT( FALSE);
				break;
			}
			mutex.lock();
			packetID = queryIn.GetPacketID();
			mutex.unlock();
			if(token != queryIn.GetToken())
			{
				if(pContext->mpLogWriter!=NULL)
					pContext->mpLogWriter->WriteLnLPCSTR("CHECK TOKEN FALIED in BinState");
				break;
			}
			OnIPadSessionBinState( queryIn.GetType(), queryIn.GetBinState(), queryIn.GetBinStateSize());
			break;
		}
	case RT_IPadSessionChangeSlide:
		{
			CIPadSessionChangeSlideIn	queryIn( aData, aDataSize);
			if( !queryIn.Analyse())
			{
				if(pContext->mpLogWriter!=NULL)
					pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CIPadSessionBinStateIn");
				ATLASSERT( FALSE);
				break;
			}
			mutex.lock();
			packetID = queryIn.GetPacketID();
			mutex.unlock();
			if(token != queryIn.GetToken())
			{
				if(pContext->mpLogWriter!=NULL)
					pContext->mpLogWriter->WriteLnLPCSTR("CHECK TOKEN FALIED in ChangeSlide");
				break;
			}			
			if(pContext->mpSM!=NULL)
				pContext->mpSM->OnIPadSessionChangeSlide( queryIn.GetChangeSlide());
			break;
		}	
	}
	if( !bIPadConnected && aType > RT_IPadSessionStatus)
	{
		bIPadConnected = true;
		SendFullState();
	}
}


void CIPadManager::Logout()
{
	if(bIPadConnected && pContext->mpServiceMan)
	{
		pContext->mpServiceMan->SendIPadLogoutOut( 0);
		bIPadConnected = false;
	}
}

void CIPadManager::UpdateIPadObjects()
{
	if( pImgObject == NULL)
		return;
	pImgObject->ApplyIPadState();
}
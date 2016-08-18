
#include "StdAfx.h"
#include "RealityHandler.h"
#include "RealityDescIn.h"

CRealityHandler::CRealityHandler(CCommonPacketIn* apQueryIn):
	CHandleBase(apQueryIn)
{
}

bool CRealityHandler::Handle(CRealityInfo &oRealityInfo, ILogWriter *apLogWriter)
{
	CRealityDescIn packetIn((BYTE*)pQueryIn->GetData(), pQueryIn->GetDataSize());
	if (!packetIn.Analyse())
	{
		if(apLogWriter!=NULL)
			apLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRealityDescIn");
		//ATLASSERT( false);
		return false;
	}

	oRealityInfo.SetRealityID( packetIn.GetRealityID());
	oRealityInfo.SetParentRealityID( packetIn.GetParentRealityID());
	oRealityInfo.SetType( (cmRealityType) packetIn.GetType());
	oRealityInfo.SetTime( packetIn.GetTime());
	oRealityInfo.SetName( packetIn.GetNameData(), packetIn.GetNameLength());
	oRealityInfo.SetDepth(packetIn.GetRealtyDepth());
	oRealityInfo.SetGuardFlags(packetIn.GetGuardFlags());

	return true;
}
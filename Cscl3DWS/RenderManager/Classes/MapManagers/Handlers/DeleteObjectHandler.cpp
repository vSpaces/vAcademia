
#include "StdAfx.h"
#include "DeleteObjectHandler.h"
#include "DeleteObjectIn.h"

CDeleteObjectHandler::CDeleteObjectHandler(CCommonPacketIn* apQueryIn):
	CHandleBase(apQueryIn)
{
}

bool CDeleteObjectHandler::Handle(unsigned int *aObjectID, unsigned int *aBornRealityID, ILogWriter *apLogWriter)
{
	CDeleteObjectIn packetIn((BYTE*)pQueryIn->GetData(), pQueryIn->GetDataSize());
	if (!packetIn.Analyse())
	{
		if(apLogWriter!=NULL)
			apLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CDeleteObjectIn");
		ATLASSERT( false);
		return false;
	}

	*aObjectID = packetIn.GetObjectID();
	*aBornRealityID = packetIn.GetBornRealityID();
	return true;
}
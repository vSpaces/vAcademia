#include "stdafx.h"
#include "ObjectCreateResultQueryIn.h"

namespace SyncManProtocol
{

ObjectCreateResultQueryIn::ObjectCreateResultQueryIn( BYTE* apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

bool ObjectCreateResultQueryIn::Analyse()
{
	if(!CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	if(!memparse_object(pData, idx, uErrorCode))
		return false;

	if(!memparse_object(pData, idx, uObjectID))
		return false;

	if(!memparse_object(pData, idx, uBornRealityID))
		return false;

	if(!memparse_object(pData, idx, uRmmlHandlerID))
		return false;

	return true;
}
}
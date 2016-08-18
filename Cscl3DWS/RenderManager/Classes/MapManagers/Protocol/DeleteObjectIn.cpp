#include "stdafx.h"
#include "deleteObjectIn.h"

bool CDeleteObjectIn::Analyse()
{
	if(!CCommonPacketIn::Analyse())
		return false;
	int idx = 0;
	if (!memparse_object( pData, idx, objectID))
		return false;
	if (!memparse_object( pData, idx, bornRealityID))
		return false;
	return EndOfData( idx);
}

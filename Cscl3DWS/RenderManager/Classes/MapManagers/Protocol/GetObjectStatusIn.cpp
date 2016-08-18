#include "stdafx.h"
#include "GetObjectStatusIn.h"

using namespace MapManagerProtocol;

bool CGetObjectStatusIn::Analyse()
{
	if(!CCommonPacketIn::Analyse())
		return false;
	if(iDataSize<2)
		return false;
	int idx=0;
	if(!memparse_object<unsigned int>(pData, idx, objectID))
		return false;
	if(!memparse_object<unsigned int>(pData, idx, bornRealityID))
		return false;
	if(!memparse_object<int>(pData, idx, status))
		return false;
	if(!memparse_object<unsigned int>(pData, idx, zoneID))
		return false;
	return EndOfData( idx);
}


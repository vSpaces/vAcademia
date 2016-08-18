#include "stdafx.h"
#include "realitydescin.h"

CRealityDescIn::CRealityDescIn(BYTE *aData, int aDataSize)
				: CCommonPacketInForMapManager(aData, aDataSize)
				, nRealityDepth(1) , nRealityID(-1)
				, nParentRealityID(-1) , nGuardFlags(0)
				, nNameLength(0), pNameData(NULL)
{
	
};

bool CRealityDescIn::Analyse()
{
	if(!CCommonPacketIn::Analyse())
		return false;
	int idx = 0;
	if (!memparse_object( pData, idx, nRealityID))
		return false;
	if (!memparse_object( pData, idx, nParentRealityID))
		return false;
	if (!memparse_object( pData, idx, nType))
		return false;

	unsigned int uiTime = 0;
	if (!memparse_object( pData, idx, uiTime))
		return false;
	nTime = (time_t)uiTime;
	if (!memparse_string_unicode( pData, idx, nNameLength, pNameData))
		return false;

	if (!memparse_object( pData, idx, nRealityDepth))
		return false;

	if (!memparse_object( pData, idx, nGuardFlags))
		return false;

	return EndOfData( idx);

}

#include "stdafx.h"
#include "ReadRemoteResourceIn.h"

using namespace ResManProtocol;

CReadRemoteResourceIn::CReadRemoteResourceIn(BYTE *aData, int aDataSize) : CCommonPacketIn(aData, aDataSize){
	pResourceData = NULL;
	iResourceDataSize = 0;
	iFileSize = 0;
	btExist = 0;
}

bool CReadRemoteResourceIn::Analyse()
{
	if(!CCommonPacketIn::Analyse())
		return false;
	int idx = 0;

	btExist = 0;
	if(!memparse_object(pData, idx, btExist))
		return false;
	if (btExist == 0)
		return EndOfData( idx);

	if (!memparse_object(pData, idx, iFileSize))
		return false;

	if (!memparse_object(pData, idx, iResourceDataSize))
		return false;

	pResourceData = pData + idx;
	idx += iResourceDataSize;

	return EndOfData( idx);
}

BYTE CReadRemoteResourceIn::IsExist(){
	return btExist;
}

DWORD CReadRemoteResourceIn::GetResourceSize(){
	return iFileSize;
}


DWORD CReadRemoteResourceIn::GetResourceDataSize(){
	return iResourceDataSize;
}

BYTE* CReadRemoteResourceIn::GetResourceData(){
	return pResourceData;
}

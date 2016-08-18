#include "stdafx.h"
#include "FileInfoIn.h"

using namespace ResManProtocol;

CFileInfoIn::CFileInfoIn(BYTE *aData, int aDataSize) : CCommonPacketIn(aData, aDataSize){
}

bool CFileInfoIn::Analyse()
{
	if(!CCommonPacketIn::Analyse())
		return false;
	int idx = 0;
	btExist = 0;
	if(!memparse_object(pData, idx, btExist))
		return false;
	iFileSize = 0;
	if(!memparse_object(pData, idx, iFileSize))
		return false;
	/*if(!memparse_string_fixed_size(pData, idx, &sDateTime, 8))
		return false;*/
	if(!memparse_object(pData, idx, lDateTime))
		return false;
	return EndOfData(idx);
}

BYTE CFileInfoIn::GetbtExist(){
	return btExist;
}
DWORD CFileInfoIn::GetFileSize(){
	return iFileSize;
}

__int64 CFileInfoIn::GetDateTime(){
	return lDateTime;
}

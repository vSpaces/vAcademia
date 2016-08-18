#include "stdafx.h"
#include "../../include/RecordServer/Protocol/FileCreatedQueryIn.h"


CFileCreatedQueryIn::CFileCreatedQueryIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize)
{
}

bool CFileCreatedQueryIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	// 1. UserName
	memparse_string_unicode( pData, idx, userName);

	// 2. FileName
	memparse_string_unicode( pData, idx, fileName);

	return EndOfData( idx);
}

CComString		CFileCreatedQueryIn::GetUserName()
{
	return userName;
}

CComString		CFileCreatedQueryIn::GetFileName()
{
	return fileName;
}
#include "stdafx.h"
#include "../../include/RecordServer/Protocol/FileCreatedQueryIn.h"


CFileCreatedQueryIn::CFileCreatedQueryIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize)
{
	recordID = 0;
}

bool CFileCreatedQueryIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	// 1. UserName
	if (!memparse_string_unicode( pData, idx, userName))
		return false;

	// 2. FileName
	if (!memparse_string_unicode( pData, idx, fileName))
		return false;

	if ( !memparse_object( pData, idx, recordID))
		return false;

	return EndOfData( idx);
}

unsigned int	CFileCreatedQueryIn::GetRecordID()
{
	return recordID;
}

CComString		CFileCreatedQueryIn::GetUserName()
{
	return userName;
}

CComString		CFileCreatedQueryIn::GetFileName()
{
	return fileName;
}
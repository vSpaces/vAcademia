#include "stdafx.h"
#include "SetSplashSrcQueryIn.h"


CSetSplashSrcQueryIn::CSetSplashSrcQueryIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize),
		MP_WSTRING_INIT(fileName)
{
}

bool CSetSplashSrcQueryIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	// 1. recordID
	if ( !memparse_object( pData, idx, recordID))
		return false;

	// 2. fileName
	if (!memparse_string_unicode( pData, idx, fileName, true))
		return false;

	return EndOfData( idx);
}

unsigned int	CSetSplashSrcQueryIn::GetRecordID()
{
	return recordID;
}

CComString		CSetSplashSrcQueryIn::GetFileName()
{
	return CComString(fileName.c_str());
}
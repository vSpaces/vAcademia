#include "StdAfx.h"
#include "recordIn.h"

RecordIn::RecordIn(BYTE *aData, int aDataSize) : 
CCommonPacketIn(aData, aDataSize)
{
}

RecordIn::~RecordIn(void)
{
}
bool RecordIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;
	if ( !memparse_string_unicode( pData, idx, sessionName))
		return false;
	
	if ( !memparse_string_unicode( pData, idx, fileName))
		return false;

	return EndOfData( idx);
}
 
LPCSTR RecordIn::GetSessionName()
{
	return sessionName.GetBuffer();
}

LPCSTR	RecordIn::GetFileName()
{
	return fileName.GetBuffer();
}
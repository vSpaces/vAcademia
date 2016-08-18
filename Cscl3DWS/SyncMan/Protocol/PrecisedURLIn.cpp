#include "stdafx.h"
#include "PrecisedURLIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CPrecisedURLIn::CPrecisedURLIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize),
	MP_WSTRING_INIT(sURL),
	MP_WSTRING_INIT(sEnteredEventInfo)
{
}

CPrecisedURLIn::~CPrecisedURLIn(){
}



bool CPrecisedURLIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;

	
	if (!memparse_object(pData, idx, uErrorCode))
		return false;

	if (!memparse_string(pData, idx, sURL))
		return false;

	if (!memparse_string(pData, idx, sEnteredEventInfo))
		return false;

	return EndOfData( idx);
}

const wchar_t* CPrecisedURLIn::GetURL() const
{
	return sURL.c_str();
}

const wchar_t* CPrecisedURLIn::GetEnteredEventInfo() const
{
	return sEnteredEventInfo.c_str();
}

const unsigned int CPrecisedURLIn::GetErrorCode() const
{
	return uErrorCode;
}
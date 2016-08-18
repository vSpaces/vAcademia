#include "stdafx.h"
#include "PrepareToPlayIn.h"

using namespace SyncManProtocol;

CPrepareToPlayIn::~CPrepareToPlayIn()
{
	//delete(wsUrl);wsUrl=NULL;
}

bool CPrepareToPlayIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	BYTE* p = (BYTE*)(pData);
	int idx = 0;
	if(!memparse_string(p, idx, wsUrl)){
		if(!memparse_object(p, idx, iDataLength))
			return false;
		return true;
	}
	iDataLength = wcslen(wsUrl.c_str());
	return EndOfData( idx);
}

unsigned short CPrepareToPlayIn::GetDataLength(){
	return iDataLength;
}

const wchar_t* CPrepareToPlayIn::GetWsURL(){
	return wsUrl.c_str();
}
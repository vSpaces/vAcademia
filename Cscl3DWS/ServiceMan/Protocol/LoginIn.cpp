#include "stdafx.h"
#include "LoginIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CLoginIn::CLoginIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

CLoginIn::~CLoginIn(){
}



bool CLoginIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, rmmlID))
		return false;
	bsResult = &pData[idx];
	bsResultSize = iDataSize - idx;
	unsigned int signature = 0;
	if(!memparse_object(pData, idx, signature))
		return false;
	unsigned int session = 0;
	if(!memparse_object(pData, idx, session))
		return false;
	byte format = 0;
	if(!memparse_object(pData, idx, format))
		return false;
	byte count = 0;
	if(!memparse_object(pData, idx, count))
		return false;
	idx+=count*11;
	if(!memparse_object(pData, idx, typeSendClientLog))
		return false;
	/*if (!memparse_string(pData, idx, (BYTE **)&bsResult))
		return false;*/
	return EndOfData( idx);
}

const BYTE* CLoginIn::GetMethodResult() const{
	return bsResult;
}

unsigned int CLoginIn::GetMethodResultSize() const{
	return bsResultSize;
}


unsigned int CLoginIn::getRmmlID() const
{
	return rmmlID;
}

/*unsigned int CLoginIn::getTypeSendClientLog() const
{
	return typeSendClientLog;
}*/
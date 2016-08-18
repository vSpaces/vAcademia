#include "stdafx.h"
#include "ExecMethodIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CExecMethodIn::CExecMethodIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize),
	MP_WSTRING_INIT(wsResult)
{
}

CExecMethodIn::~CExecMethodIn(){
}



bool CExecMethodIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, requestID))
		return false;
	if(!memparse_object(pData, idx, rmmlID))
		return false;
	if(EndOfData( idx))
		return true;
	if (!memparse_string(pData, idx, wsResult))
		return false;
	return EndOfData( idx);
}

const wchar_t* CExecMethodIn::GetMethodResult() const{
	return wsResult.c_str();
}


unsigned int CExecMethodIn::getRmmlID() const
{
	return rmmlID;
}

unsigned int CExecMethodIn::getRequestID() const
{
	return requestID;
}

#include "stdafx.h"
#include "GetServiceIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CGetServiceIn::CGetServiceIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize),
	MP_WSTRING_INIT(wsName),
	MP_WSTRING_INIT(wsDescription)
{
}

CGetServiceIn::~CGetServiceIn(){
}



bool CGetServiceIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, rmmlID))
		return false;
	if (!memparse_string(pData, idx, wsName))
		return false;
	if (!memparse_string(pData, idx, wsDescription))
		return false;
	return EndOfData( idx);
}

const wchar_t* CGetServiceIn::GetServiceName() const {
	return wsName.c_str();
}


const wchar_t* CGetServiceIn::GetServiceDescription() const{
	return wsDescription.c_str();
}

unsigned int CGetServiceIn::getRmmlID() const
{
	return rmmlID;
}

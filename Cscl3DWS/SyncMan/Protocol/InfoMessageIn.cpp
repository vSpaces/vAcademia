#include "stdafx.h"
#include "InfoMessageIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CInfoMessageIn::CInfoMessageIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

CInfoMessageIn::~CInfoMessageIn(){
}



bool CInfoMessageIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if (!memparse_object(pData, idx, uiMsgCode))
		return false;
	if (!memparse_string(pData, idx, sMessage))
		return false;
	
	return EndOfData( idx);
}

const wchar_t* CInfoMessageIn::GetMessage() const
{
	return sMessage.c_str();
}


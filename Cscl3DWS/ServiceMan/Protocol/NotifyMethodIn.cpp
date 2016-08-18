#include "stdafx.h"
#include "NotifyMethodIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CNotifyMethodIn::CNotifyMethodIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

CNotifyMethodIn::~CNotifyMethodIn()
{
}

bool CNotifyMethodIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, rmmlID))
		return false;
	if (!memparse_string(pData, idx, wsResult))
		return false;
	return EndOfData( idx);
}

const wchar_t* CNotifyMethodIn::GetMethodResult() const
{
	return wsResult.c_str();
}


unsigned int CNotifyMethodIn::getRmmlID() const
{
	return rmmlID;
}


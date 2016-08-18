#include "stdafx.h"
#include "SetSessionStateResultIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CSetSessionStateResultIn::CSetSessionStateResultIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

CSetSessionStateResultIn::~CSetSessionStateResultIn()
{
}

bool CSetSessionStateResultIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx = 0;
	if (!memparse_object( pData, idx, type))
		return false;
	if (!memparse_object( pData, idx, code))
		return false;
	return EndOfData( idx);
}

unsigned short CSetSessionStateResultIn::getType()
{
	return type;
}

unsigned short CSetSessionStateResultIn::getCode()
{
	return code;
}
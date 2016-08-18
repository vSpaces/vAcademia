#include "stdafx.h"
#include "LogoutIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CLogoutIn::CLogoutIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

CLogoutIn::~CLogoutIn()
{
}

bool CLogoutIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, code))
		return false;
	return EndOfData( idx);
}

unsigned int CLogoutIn::getCode() const
{
	return code;
}
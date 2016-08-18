#include "stdafx.h"
#include "IPadSessionStatusIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CIPadSessionStatusIn::CIPadSessionStatusIn( BYTE *apData, int apDataSize) :
	CIPadSessionBaseIn( apData, apDataSize)
{
}

CIPadSessionStatusIn::~CIPadSessionStatusIn()
{
}

bool CIPadSessionStatusIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	idx=0;
	if(!memparse_object(pData, idx, status))
		return false;
	return EndOfData( idx);
}

int CIPadSessionStatusIn::getStatus() const
{
	return status;
}
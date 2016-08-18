#include "stdafx.h"
#include "IPadSessionStopedIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CIPadSessionStopedIn::CIPadSessionStopedIn( BYTE *apData, int apDataSize) :
	CIPadSessionBaseIn( apData, apDataSize)
{
}

CIPadSessionStopedIn::~CIPadSessionStopedIn()
{
}

bool CIPadSessionStopedIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	idx=0;
	return EndOfData( idx);
}

/*unsigned int In::getCode() const
{
	return code;
}*/
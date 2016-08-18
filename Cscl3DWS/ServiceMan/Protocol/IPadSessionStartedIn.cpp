#include "stdafx.h"
#include "IPadSessionStartedIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CIPadSessionStartedIn::CIPadSessionStartedIn( BYTE *apData, int apDataSize) :
	CIPadSessionBaseIn( apData, apDataSize)
{
}

CIPadSessionStartedIn::~CIPadSessionStartedIn()
{
}

bool CIPadSessionStartedIn::Analyse()
{
	if (!CIPadSessionBaseIn::Analyse())
		return false;

	return EndOfData( idx);
}

/*unsigned int CIn::getCode() const
{
	return code;
}*/
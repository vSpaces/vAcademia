#include "stdafx.h"
#include "IPadSessionBinStateIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CIPadSessionBinStateIn::CIPadSessionBinStateIn( BYTE *apData, int apDataSize) :
	CIPadSessionBaseIn( apData, apDataSize)
{
}

CIPadSessionBinStateIn::~CIPadSessionBinStateIn()
{
}

bool CIPadSessionBinStateIn::Analyse()
{
	if (!CIPadSessionBaseIn::Analyse())
		return false;	

	if(!memparse_object(pData, idx, type))
		return false;
	binState = NULL;
	binStateSize = 0;	
	if(!memparse_object(pData, idx, binStateSize))
		return false;
	/*if (binStateSize != 0)
		binState = MP_NEW_ARR( unsigned char, binStateSize);*/
	memparse_data(pData, idx, binStateSize, &binState);
	return EndOfData( idx);
}

unsigned char CIPadSessionBinStateIn::GetType() const
{
	return type;
}

unsigned char *CIPadSessionBinStateIn::GetBinState() const
{
	return binState;
}

unsigned int CIPadSessionBinStateIn::GetBinStateSize() const
{
	return binStateSize;
}
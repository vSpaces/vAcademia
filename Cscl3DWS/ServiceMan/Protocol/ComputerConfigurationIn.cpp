#include "stdafx.h"
#include "ComputerConfigurationIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

ComputerConfigurationIn::ComputerConfigurationIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

ComputerConfigurationIn::~ComputerConfigurationIn()
{
}

bool ComputerConfigurationIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, code))
		return false;
	return EndOfData( idx);
}

unsigned int ComputerConfigurationIn::getCode() const
{
	return code;
}
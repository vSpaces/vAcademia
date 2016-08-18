#include "stdafx.h"
#include "../../include/PipeProtocol/CurrentInputLevelIn.h"


CCurrentInputLevelIn::CCurrentInputLevelIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CCurrentInputLevelIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( currentInputLevel))
		return false;

	return EndOfData();
}

float CCurrentInputLevelIn::GetCurrentInputLevel()
{
	return currentInputLevel;
}
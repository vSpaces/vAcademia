#include "stdafx.h"
#include "../../include/PipeProtocol/GetDevicesIn.h"


CGetDevicesIn::CGetDevicesIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CGetDevicesIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if( !data->Read( btNeedAnswerToRmml))
		return false;
	
	if( !data->Read( btForce))
		return false;	

	return EndOfData();
}
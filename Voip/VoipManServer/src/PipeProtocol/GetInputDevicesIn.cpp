#include "stdafx.h"
#include "../../include/PipeProtocol/GetInputDevicesIn.h"


CGetInputDevicesIn::CGetInputDevicesIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CGetInputDevicesIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if( !data->Read( btNeedAnswerToRmml))
		return false;
	
	if( !data->Read( btForce))
		return false;	

	return EndOfData();
}
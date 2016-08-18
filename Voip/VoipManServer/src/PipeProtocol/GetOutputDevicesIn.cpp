#include "stdafx.h"
#include "../../include/PipeProtocol/GetOutputDevicesIn.h"


CGetOutputDevicesIn::CGetOutputDevicesIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
{
}

bool CGetOutputDevicesIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if( !data->Read( btNeedAnswerToRmml))
		return false;
	
	if( !data->Read( btForce))
		return false;	

	return EndOfData();
}
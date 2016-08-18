#include "stdafx.h"
#include "include/PipeProtocol/GetDevicesOut.h"

CGetDevicesOut::CGetDevicesOut(byte aID, bool aNeedAnswerToRmml, bool abForce) : CCommonPipePacketOut( aID)
{
	byte btNeedAnswerToRmml = (byte)aNeedAnswerToRmml;
	byte btForce = (byte)abForce;
	data->Add( btNeedAnswerToRmml);
	data->Add( btForce);
}
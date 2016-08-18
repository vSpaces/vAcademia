#include "stdafx.h"
#include "include/PipeProtocol/GetInputDevicesOut.h"

CGetInputDevicesOut::CGetInputDevicesOut(byte aID, bool aNeedAnswerToRmml, bool abForce) : CCommonPipePacketOut( aID)
{
	byte btNeedAnswerToRmml = (byte)aNeedAnswerToRmml;
	byte btForce = (byte)abForce;
	data->Add( btNeedAnswerToRmml);
	data->Add( btForce);
}
#include "stdafx.h"
#include "include/PipeProtocol/GetOutputDevicesOut.h"

CGetOutputDevicesOut::CGetOutputDevicesOut(byte aID, bool aNeedAnswerToRmml, bool abForce) : CCommonPipePacketOut( aID)
{
	byte btNeedAnswerToRmml = (byte)aNeedAnswerToRmml;
	byte btForce = (byte)abForce;
	data->Add( btNeedAnswerToRmml);
	data->Add( btForce);
}
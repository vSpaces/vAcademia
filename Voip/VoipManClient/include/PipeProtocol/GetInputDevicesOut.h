#pragma once
#include "CommonPipePacketOut.h"

class CGetInputDevicesOut : public CCommonPipePacketOut
{
public:
	CGetInputDevicesOut(byte aID, bool abNeedAnswerToRmml, bool abForce);
};
#pragma once
#include "CommonPipePacketOut.h"

class CGetOutputDevicesOut : public CCommonPipePacketOut
{
public:
	CGetOutputDevicesOut(byte aID, bool abNeedAnswerToRmml, bool abForce);
};
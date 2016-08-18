#pragma once
#include "CommonPipePacketOut.h"

class CGetDevicesOut : public CCommonPipePacketOut
{
public:
	CGetDevicesOut(byte aID, bool abNeedAnswerToRmml, bool abForce);
};
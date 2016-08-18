#pragma once
#include "CommonPipePacketOut.h"
#ifdef USE_CONAITO_SDK_LIB
#include "ConaitoSDK/EvoWrapper.h"
#endif

#ifdef USE_MUMBLE_SDK
#include "MumbleAPI.h"
using namespace api;
#endif

class CGetInputDevicesResultOut : public CCommonPipePacketOut
{
public:
	CGetInputDevicesResultOut(byte aID, vecDeviceInfo *pInputDevicesInfo, byte abtNeedAnswerToRmml);
};
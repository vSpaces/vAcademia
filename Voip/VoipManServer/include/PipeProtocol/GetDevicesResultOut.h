#pragma once
#include "CommonPipePacketOut.h"
#ifdef USE_CONAITO_SDK_LIB
#include "ConaitoSDK/EvoWrapper.h"
#endif
#ifdef USE_MUMBLE_SDK
#include "MumbleAPI.h"
using namespace api;
#endif

class CGetDevicesResultOut : public CCommonPipePacketOut
{
public:
	CGetDevicesResultOut(byte aID, vecDeviceInfo *pInputDevicesInfo, vecDeviceInfo *pOutputDevicesInfo, byte abtNeedAnswerToRmml);
};
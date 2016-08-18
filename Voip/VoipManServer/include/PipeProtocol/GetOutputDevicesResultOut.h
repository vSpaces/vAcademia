#pragma once
#include "CommonPipePacketOut.h"
#ifdef USE_CONAITO_SDK_LIB
#include "ConaitoSDK/EvoWrapper.h"
#endif

#ifdef USE_MUMBLE_SDK
#include "MumbleAPI.h"
using namespace api;
#endif


class CGetOutputDevicesResultOut : public CCommonPipePacketOut
{
public:
	CGetOutputDevicesResultOut(byte aID, vecDeviceInfo *pOutputDevicesInfo, byte abtNeedAnswerToRmml);
};
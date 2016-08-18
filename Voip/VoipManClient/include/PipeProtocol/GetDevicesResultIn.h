#pragma once
#include "CommonPipePacketIn.h"

#ifdef USE_CONAITO_SDK_LIB
#include "ConaitoSDK/EvoWrapper.h"
#endif

#ifdef USE_MUMBLE_SDK
#include "MumbleAPI.h"
using namespace api;
#endif

class CGetDevicesResultIn : public CCommonPipePacketIn
{	
	MP_VECTOR<SoundDevice> inputDevicesInfo;
	MP_VECTOR<SoundDevice> outputDevicesInfo;
	byte btNeedAnswerToRmml;
public:
	CGetDevicesResultIn(BYTE *aData, int aDataSize);
	virtual bool Analyse();
	vecDeviceInfo *GetInputDevicesInfo();
	vecDeviceInfo *GetOutputDevicesInfo();	
	bool GetNeedAnswerToRmml(){return btNeedAnswerToRmml!=0;};
};
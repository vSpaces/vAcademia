#pragma once
#include "CommonPipePacketIn.h"

#ifdef USE_CONAITO_SDK_LIB
#include "ConaitoSDK/EvoWrapper.h"
typedef std::vector<SoundDevice> vecDeviceInfo;
typedef vecDeviceInfo::iterator vecDeviceInfoIt;
#endif

#ifdef USE_MUMBLE_SDK
#include "MumbleAPI.h"
using namespace api;
#endif


class CGetOutputDevicesResultIn : public CCommonPipePacketIn
{
	MP_VECTOR<SoundDevice> outputDevicesInfo;
	byte btNeedAnswerToRmml;
public:
	CGetOutputDevicesResultIn(BYTE *aData, int aDataSize);
	virtual bool Analyse();
	vecDeviceInfo *GetOutputDevicesInfo();
	bool GetNeedAnswerToRmml(){return btNeedAnswerToRmml!=0;};
};
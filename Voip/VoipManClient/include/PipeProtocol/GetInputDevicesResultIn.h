#pragma once
#include "CommonPipePacketIn.h"

#ifdef USE_CONAITO_SDK_LIB
//#include "ConaitoSDK/EvoWrapper.h"

//typedef std::vector<SoundDevice> vecDeviceInfo;
//typedef vecDeviceInfo::iterator vecDeviceInfoIt;
#endif

#ifdef USE_MUMBLE_SDK
#include "MumbleAPI.h"
using namespace api;
#endif



class CGetInputDevicesResultIn : public CCommonPipePacketIn
{
	MP_VECTOR<SoundDevice> inputDevicesInfo;
	byte btNeedAnswerToRmml;
public:
	CGetInputDevicesResultIn(BYTE *aData, int aDataSize);
	virtual bool Analyse();
	vecDeviceInfo *GetInputDevicesInfo();
	bool GetNeedAnswerToRmml(){return btNeedAnswerToRmml!=0;};
};
#pragma once
#include "CommonPipePacketIn.h"

class CJoinChannelIn : public CCommonPipePacketIn
{
	MP_STRING sRoomName;	
public:
	CJoinChannelIn(BYTE *aData, int aDataSize);
	virtual bool Analyse();
	std::string GetRoomName();	
};
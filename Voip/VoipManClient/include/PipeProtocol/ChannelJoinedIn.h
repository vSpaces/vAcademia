#pragma once
#include "CommonPipePacketIn.h"

class CChannelJoinedIn : public CCommonPipePacketIn
{
	MP_WSTRING channelPath;
public:
	CChannelJoinedIn(BYTE *aData, int aDataSize);
	std::wstring GetChannelPath();
	virtual bool Analyse();
};
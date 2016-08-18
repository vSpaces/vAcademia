#pragma once
#include "CommonPipePacketIn.h"

class CDoLoginIn : public CCommonPipePacketIn
{
	MP_WSTRING sUserName;
	MP_WSTRING sPassword;
	MP_WSTRING sRoomName;
	int udpPort;
public:
	CDoLoginIn(BYTE *aData, int aDataSize);
	virtual bool Analyse();
	std::wstring GetUserName();
	std::wstring GetPassword();
	std::wstring GetRoomName();
};
#pragma once
#include "CommonPipePacketOut.h"

class CDoLoginOut : public CCommonPipePacketOut
{
public:
	CDoLoginOut(byte aID, const wchar_t *aUserName, const wchar_t *aUserPassword, const wchar_t *aRoomName);
};
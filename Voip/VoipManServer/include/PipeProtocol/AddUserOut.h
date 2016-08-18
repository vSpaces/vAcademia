#pragma once
#include "CommonPipePacketOut.h"

class CAddUserOut : public CCommonPipePacketOut
{
public:
	CAddUserOut(byte aID, unsigned int aUserID, wchar_t *apNickName);
};
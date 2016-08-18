#pragma once
#include "CommonPipePacketOut.h"

class CKickedOut : public CCommonPipePacketOut
{
public:
	CKickedOut(byte aID);
};
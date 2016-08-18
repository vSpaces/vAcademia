#pragma once
#include "CommonPipePacketIn.h"

class CGetMyUserIDIn : public CCommonPipePacketIn
{
	int userID;
	bool bVoipSystemInitialized;
public:
	CGetMyUserIDIn(BYTE *aData, int aDataSize);
	int GetUserID();
	virtual bool Analyse();
	bool IsVoipSystemInitialized();
};
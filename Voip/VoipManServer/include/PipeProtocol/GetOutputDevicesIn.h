#pragma once
#include "CommonPipePacketIn.h"

class CGetOutputDevicesIn : public CCommonPipePacketIn
{
	byte btNeedAnswerToRmml;
	byte btForce;
public:
	CGetOutputDevicesIn(BYTE *aData, int aDataSize);
	virtual bool Analyse();
	bool GetNeedAnswerToRmml(){return btNeedAnswerToRmml!=0;};
	bool GetForce(){return btForce!=0;};
};
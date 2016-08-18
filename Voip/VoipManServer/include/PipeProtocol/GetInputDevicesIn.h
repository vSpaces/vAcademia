#pragma once
#include "CommonPipePacketIn.h"

class CGetInputDevicesIn : public CCommonPipePacketIn
{
	byte btNeedAnswerToRmml;
	byte btForce;
public:
	CGetInputDevicesIn(BYTE *aData, int aDataSize);
	virtual bool Analyse();
	bool GetNeedAnswerToRmml(){return btNeedAnswerToRmml!=0;};
	bool GetForce(){return btForce!=0;};
};
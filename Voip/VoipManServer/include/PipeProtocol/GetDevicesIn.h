#pragma once
#include "CommonPipePacketIn.h"

class CGetDevicesIn : public CCommonPipePacketIn
{
	byte btNeedAnswerToRmml;
	byte btForce;
public:
	CGetDevicesIn(BYTE *aData, int aDataSize);
	virtual bool Analyse();
	bool GetNeedAnswerToRmml(){return btNeedAnswerToRmml!=0;};
	bool GetForce(){return btForce!=0;};
};
#pragma once
#include "CommonPacketInForMapManager.h"

using namespace Protocol;

class CDeleteObjectIn :
	public CCommonPacketInForMapManager
{
	unsigned int objectID;
	unsigned int bornRealityID;
public:
	CDeleteObjectIn(BYTE *aData, int aDataSize) :
		CCommonPacketInForMapManager(aData, aDataSize)
	{
	};
	virtual bool Analyse();


public:
	__forceinline unsigned int GetObjectID() const { return objectID;}
	__forceinline unsigned int GetBornRealityID() const { return bornRealityID;}
};

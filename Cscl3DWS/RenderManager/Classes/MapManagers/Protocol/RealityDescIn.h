#pragma once
#include "CommonPacketInForMapManager.h"

using namespace Protocol;

class CRealityDescIn :
	public CCommonPacketInForMapManager
{
	unsigned int nRealityID;
	unsigned int nParentRealityID;
	unsigned int nRealityDepth;
	unsigned int nGuardFlags;
	unsigned char nType;
	time_t nTime;
	unsigned short nNameLength;
	wchar_t* pNameData;

public:
	CRealityDescIn(BYTE *aData, int aDataSize);
	virtual bool Analyse();


public:
	__forceinline unsigned int GetRealityID() const { return nRealityID;}
	__forceinline unsigned int GetGuardFlags() const { return nGuardFlags;}
	__forceinline unsigned int GetParentRealityID() const { return nParentRealityID;}
	__forceinline unsigned int GetRealtyDepth() const { return nRealityDepth;}
	__forceinline unsigned char GetType() const { return nType;}
	__forceinline time_t GetTime() const { return nTime;}
	__forceinline unsigned short GetNameLength() const { return nNameLength;}
	__forceinline const wchar_t* GetNameData() const { return pNameData;}
};

#pragma once

#include "DataStorage.h"

class CCommonPipePacketIn
{
protected:
	CDataStorage *data;
	byte id;
public:
	CCommonPipePacketIn( BYTE *aData, int aDataSize);
	~CCommonPipePacketIn();
	virtual bool Analyse();
	//CDataStorage *GetDataStorage();	
	int	GetID();
	bool EndOfData();
};
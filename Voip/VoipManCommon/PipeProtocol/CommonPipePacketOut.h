#pragma once

#include "DataStorage.h"
#include "..\ComMan\ProxySettings.h"

class CCommonPipePacketOut
{
protected:
	CDataStorage *data;
public:
	CCommonPipePacketOut(byte aID);
	~CCommonPipePacketOut();
	CDataStorage *GetDataStorage();
};
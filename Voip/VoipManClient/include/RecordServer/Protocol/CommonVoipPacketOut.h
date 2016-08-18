#pragma once

#include "DataBuffer2.h"
class CCommonVoipPacketOut
{
public:
	inline CDataBuffer2& GetDataBuffer()
	{
		return data;
	}
	inline unsigned char* GetData()
	{
		return (unsigned char*) data.getData();
	}
	inline unsigned short GetDataSize()
	{
		return data.getSize();
	}

private: 
	CDataBuffer2 data;
};
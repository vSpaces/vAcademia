#pragma once

#include "DataBuffer2.h"
class CDesktopPacketOut
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
	inline int GetDataSize()
	{
		return data.getSize();
	}

private: 
	CDataBuffer2 data;
};
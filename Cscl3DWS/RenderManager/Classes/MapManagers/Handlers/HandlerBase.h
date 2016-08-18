
#pragma once
#include "MapManagerServer.h"
#include "MapManager.h"

namespace Protocol
{
	class CCommonPacketIn;
};
using namespace Protocol;

class CHandleBase
{
protected:
	CCommonPacketIn* pQueryIn;
public:
	CHandleBase(CCommonPacketIn* apQueryIn);
	virtual ~CHandleBase();
};
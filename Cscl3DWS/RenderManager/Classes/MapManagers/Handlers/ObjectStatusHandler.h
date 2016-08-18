#pragma once

class CObjectStatusHandler
{
public:
	static bool Handle(CCommonPacketIn* apQueryIn, cm::cmIMapManCallbacks* apCallbacks);
};
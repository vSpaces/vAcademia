#pragma once

#include "CommonPacketOut.h"

using namespace Protocol;

namespace ServiceManProtocol
{
	class ComputerStateOut : public CCommonPacketOut
	{
	public:
		ComputerStateOut( const char *fps, int quality, unsigned int currentPhysicalMemory, unsigned int currentVirtualMemory);
	};
};
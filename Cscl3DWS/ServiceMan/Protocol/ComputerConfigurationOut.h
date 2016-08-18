#pragma once

#include "CommonPacketOut.h"

using namespace Protocol;

namespace ServiceManProtocol
{
	class ComputerConfigurationOut : public CCommonPacketOut
	{
	public:
		ComputerConfigurationOut();
		void AddComputerInfo(  std::wstring &compName, std::string &processName, unsigned int &processorCount, unsigned int &realCoreCount, bool &hyperthreadingEnabled, unsigned __int64 physicalMemorySize, unsigned __int64 virtualMemorySize, std::string &osVersion, unsigned char &osBits);
		void AddGPUInfo( std::string &vendorName, std::string &deviceName, unsigned __int64 &videoMemorySize, int &major, int &minor);
	};
};
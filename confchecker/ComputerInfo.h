
#pragma once

#include <string>

class CComputerInfo
{
public:
	CComputerInfo();
	~CComputerInfo();

	bool IsLaptop();

	std::string GetCompName();
	UINT64 GetPhysicalMemorySize();
	UINT64 GetAvailablePhysicalMemorySize();
	UINT64 GetVirtualMemorySize();
	UINT64 GetAvailableVirtualMemorySize();
	unsigned int GetProcessorCount();
	std::string GetProcessorName();

	std::string GetOSVersion();
	unsigned int GetOSBits();

	int GetDisplayCount();
	int GetMouseButtonCount();

	bool IsMousePresent();
	bool IsNetworkPresent();

	unsigned int GetCPUFrequency();
};
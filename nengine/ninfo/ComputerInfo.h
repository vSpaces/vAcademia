
#pragma once

#include <string>

class CComputerInfo
{
public:
	CComputerInfo();
	~CComputerInfo();

	bool IsLaptop();

	bool IsAeroEnabled();

	std::string GetCompName();
	__int64 GetPhysicalMemorySize();
	__int64 GetAvailablePhysicalMemorySize();
	__int64 GetVirtualMemorySize();
	__int64 GetAvailableVirtualMemorySize();

	std::string GetProcessorName();
	unsigned int GetProcessorCount();
	unsigned int GetRealCoreCount();
	bool IsHyperthreadingEnabled();
	unsigned int IsMultiCoreCapable();

	void GetOSVersion(unsigned int* apuMajor,  unsigned int* apuMinor,  unsigned int* apuBuild);
	std::string GetOSVersionString();
	std::string GetOSVersionHumanString();
	unsigned int GetOSBits();

	int GetDisplayCount();
	int GetMouseButtonCount();

	bool IsMousePresent();
	bool IsNetworkPresent();

	unsigned int GetCPUFrequency();

	bool IsSecondLifeInstalled();
};
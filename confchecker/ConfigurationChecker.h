
#pragma once

#include <string>
#include <vector>
#include <Windows.h>

#define DRIVERS_NOT_INSTALLED		0
#define VENDOR_OK					1
#define VENDOR_UNSUPPORTED			2
#define DRIVER_INVALID				3
#define DRIVER_UNSUPPORTED_VERSION	4

typedef struct _SVendorInfo
{
	std::string vendorName;
	int majorVer;
	int minorVer;
	int buildVer;
	std::vector<int> leadVers;

	_SVendorInfo()
	{
		majorVer = 0;
		minorVer = 0;
		buildVer = 0;
	}

	_SVendorInfo(const _SVendorInfo& v)
	{
		vendorName = v.vendorName;
		majorVer = v.majorVer;
		minorVer = v.minorVer;
		buildVer = v.buildVer;

		std::vector<int>::const_iterator it = v.leadVers.begin();
		std::vector<int>::const_iterator itEnd = v.leadVers.end();
		for ( ; it != itEnd; it++)
		{
			leadVers.push_back(*it);
		}
	}
} SVendorInfo;
	
typedef struct _SInvalidDriverInfo
{
	int vendorID;
	int minorVer;
	int majorVer;

	_SInvalidDriverInfo()
	{
		majorVer = 0;
		minorVer = 0;
		vendorID = 0;
	}

	_SInvalidDriverInfo(const _SInvalidDriverInfo& v)
	{
		majorVer = v.majorVer;
		minorVer = v.minorVer;
		vendorID = v.vendorID;
	}
} SInvalidDriverInfo;

class CConfigurationChecker
{
public:
	CConfigurationChecker();
	~CConfigurationChecker();

	void InitFromXML(std::string path);

	void AddSupportedVendor(std::string vendor);
	void AddRequiredExtension(std::string extension);

	int IsVendorSupported();
	bool AreRequiredExtensionsExists();
	bool AreExtendedAbilitiesSupported();

	bool IsTextureRenderingSupported();

private:
	bool IsAtiBuildSupported(int& buildVer);

	std::vector<SVendorInfo> m_supportedVendors;
	std::vector<SInvalidDriverInfo> m_invalidDrivers;
	std::vector<std::string> m_requiredExtensions;
};
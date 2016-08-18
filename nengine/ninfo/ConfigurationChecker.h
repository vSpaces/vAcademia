
#pragma once

#include "CommonInfoHeader.h"

#include <string>
#include <vector>
#include <Windows.h>

#define DRIVERS_NOT_INSTALLED		0
#define VENDOR_OK					1
#define VENDOR_UNSUPPORTED			2
#define DRIVER_INVALID				3
#define DRIVER_UNSUPPORTED_VERSION	4

#define VENDOR_SUPPORT_UNCHECKED	255

typedef struct _SVendorInfo
{
	MP_STRING vendorName;
	int majorVer;
	int minorVer;
	int buildVer;
	MP_VECTOR<int> leadVers;

	_SVendorInfo();
	_SVendorInfo(const _SVendorInfo& v);
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

	void InitFromXML(std::wstring path);

	void AddSupportedVendor(std::string vendor);
	void AddRequiredExtension(std::string extension);

	int IsVendorSupported();
	bool AreRequiredExtensionsExists();
	bool AreExtendedAbilitiesSupported();

	bool IsTextureRenderingSupported();

private:
	bool IsAtiBuildSupported(int& buildVer);

	unsigned char m_isVendorSupportedResult;

	MP_VECTOR<SVendorInfo> m_supportedVendors;
	MP_VECTOR<SInvalidDriverInfo> m_invalidDrivers;
	MP_VECTOR<MP_STRING> m_requiredExtensions;
};

#include "StdAfx.h"
#include "Globals.h"
#include "XMLList.h"
#include "HelperFunctions.h"
#include "ConfigurationChecker.h"

CConfigurationChecker confChecker;

CConfigurationChecker::CConfigurationChecker()
{
}

void CConfigurationChecker::InitFromXML(std::string path)
{
	// vendors
	std::string majorVersionOptionFieldName = "majorver";
	std::string minorVersionOptionFieldName = "minorver";
	std::string leadVersionOptionFieldName = "leadver";
	std::string buildVersionOptionFieldName = "build";
	std::vector<std::string> optionalFields;
	optionalFields.push_back(majorVersionOptionFieldName);
	optionalFields.push_back(minorVersionOptionFieldName);
	optionalFields.push_back(leadVersionOptionFieldName);
	optionalFields.push_back(buildVersionOptionFieldName);

	CXMLList vendors(path + "vendors.xml", optionalFields);

	std::string vendorName;
	
	while (vendors.GetNextValue(vendorName))
	{
		SVendorInfo vendor;
		vendor.vendorName = vendorName;

		std::string majorVer, minorVer, buildVer, leadVers;
		vendors.GetOptionalFieldValue(majorVersionOptionFieldName, majorVer);
		vendors.GetOptionalFieldValue(minorVersionOptionFieldName, minorVer);
		vendors.GetOptionalFieldValue(buildVersionOptionFieldName, buildVer);
		vendors.GetOptionalFieldValue(leadVersionOptionFieldName, leadVers);

		vendor.majorVer = atoi(majorVer.c_str());
		vendor.minorVer = atoi(minorVer.c_str());
		vendor.buildVer = atoi(buildVer.c_str());

		int pos = 0;
		if (leadVers.size() > 0)
		while (pos != -1)
		{
			int leadVer = atoi(leadVers.c_str() + pos);
			vendor.leadVers.push_back(leadVer);
			pos = leadVers.find(",", pos);
			if (pos != -1)
			{
				pos++;
			}
		}

		m_supportedVendors.push_back(vendor);
	}

	// invalid drivers
	optionalFields.clear();
	std::string driverVersionOptionFieldName = "version";
	optionalFields.push_back(driverVersionOptionFieldName);

	CXMLList invalidDrivers(path + "invalid_drivers.xml", optionalFields);

	SInvalidDriverInfo invalidDriver;
	std::string version;
	while (invalidDrivers.GetNextValue(vendorName))
	{		
		invalidDrivers.GetOptionalFieldValue(driverVersionOptionFieldName, version);

#pragma warning(push)
#pragma warning(disable : 4239)
		std::string& deviceStringID = gpuInfo.GetDeviceStringID(vendorName);
#pragma warning(pop)
		invalidDriver.vendorID = gpuInfo.GetVendorID(deviceStringID);

		unsigned int pointPos = version.find(".");
		invalidDriver.majorVer = atoi(version.c_str());
		invalidDriver.minorVer = atoi(version.c_str() + pointPos + 1);
		
		m_invalidDrivers.push_back(invalidDriver);
	}


	// extensions
	CXMLList extensions(path + "extensions.xml");

	std::string extension;
	while (extensions.GetNextValue(extension))
	{
		m_requiredExtensions.push_back(extension);
	}
}

void CConfigurationChecker::AddSupportedVendor(std::string vendor)
{
	SVendorInfo newVendor;
	newVendor.vendorName = vendor;

	m_supportedVendors.push_back(newVendor);
}

void CConfigurationChecker::AddRequiredExtension(std::string extension)
{
	m_requiredExtensions.push_back(extension);
}

int CConfigurationChecker::IsVendorSupported()
{
    std::string vendor = gpuInfo.GetVendorGLName();
	vendor = StringToLower(vendor);

	// OpenGL Software Emulation?
	if (vendor.find("microsoft") != -1)
	{
		return DRIVERS_NOT_INSTALLED;
	}

	int leadVersion, majorVersion, minorVersion;
	gpuInfo.GetDriverVersion(leadVersion, majorVersion, minorVersion);
	int vendorID = gpuInfo.GetVendorID();

	// checking for invalid driver
	std::vector<SInvalidDriverInfo>::iterator it1 = m_invalidDrivers.begin(); 
	std::vector<SInvalidDriverInfo>::iterator itEnd1 = m_invalidDrivers.end();
	for ( ; it1 != itEnd1; it1++)
	{
		if (vendorID == it1->vendorID)
		{
			if ((it1->majorVer == majorVersion) && (it1->minorVer == minorVersion))
			{
				return DRIVER_INVALID;
			}
		}
	}

	
	std::vector<SVendorInfo>::iterator it2 = m_supportedVendors.begin();
	std::vector<SVendorInfo>::iterator itEnd2 = m_supportedVendors.end();
	for ( ; it2 != itEnd2; it2++)
	{
		std::string lcVendorName = StringToLower((*it2).vendorName);
		if (vendor.find(lcVendorName) != -1)
		{
			// —равниваем версию с минимально установленной
			if ((majorVersion < (*it2).majorVer) || ((majorVersion == (*it2).majorVer) && (minorVersion < (*it2).minorVer)))
			{
				if (majorVersion == 0 && minorVersion == 0 && IsAtiBuildSupported((*it2).buildVer))
					return VENDOR_OK;

				return DRIVER_UNSUPPORTED_VERSION;
			}

			if ((*it2).leadVers.size() > 0)
			{
				bool isFoundLeadVersion = false;
			
				std::vector<int>::iterator lIt = (*it2).leadVers.begin();
				std::vector<int>::iterator lItEnd = (*it2).leadVers.end();

				for ( ; lIt != lItEnd; lIt++)
				if (*lIt == leadVersion)
				{
					isFoundLeadVersion = true;
					break;
				}

				if (!isFoundLeadVersion)
				{
					return DRIVER_UNSUPPORTED_VERSION;
				}
			}
			
			return VENDOR_OK;
		}
	}
			
	return VENDOR_UNSUPPORTED;
}

bool CConfigurationChecker::AreRequiredExtensionsExists()
{
	std::string extensions = extInfo.GetExtensionsString();
	
	std::vector<std::string>::iterator it = m_requiredExtensions.begin();
	std::vector<std::string>::iterator itEnd = m_requiredExtensions.end();
	for ( ; it != itEnd; it++)
	{
		if (extensions.find(StringToLower(*it)) == -1)
		{
			return false;
		}
	}
		
	return true;
}

bool CConfigurationChecker::AreExtendedAbilitiesSupported()
{
	if (IsVendorSupported() != VENDOR_OK)
	{ 
		return false;
	}

	if (!extInfo.IsExtensionSupported(GLEXT_ext_framebuffer_object))
	{
		return false;
	}

	if (gpuInfo.GetVendorID() == VENDOR_ATI)
	{
		int leadVersion, majorVersion, minorVersion;
		gpuInfo.GetDriverVersion(leadVersion, majorVersion, minorVersion);

		if (majorVersion >= 8)
		{
			return true;
		}

		std::vector<SVendorInfo>::iterator it = m_supportedVendors.begin();
		std::vector<SVendorInfo>::iterator itEnd = m_supportedVendors.end();
		for ( ; it != itEnd; it++)
		{
			std::string lcVendorName = StringToLower((*it).vendorName);

			if (lcVendorName == "ati")
			{
				if (IsAtiBuildSupported((*it).buildVer))
				{
					return true;
				}
			}
		}
	
		return false;
	}
	else
	{
		return true;
	}
}

bool CConfigurationChecker::IsTextureRenderingSupported()
{
	return false;
}

bool CConfigurationChecker::IsAtiBuildSupported(int& buildVer)
{
	return (gpuInfo.GetAtiBuildNumber() > buildVer);	
}

CConfigurationChecker::~CConfigurationChecker()
{
}



#include "StdAfx.h"
#include "Special.h"
#include "Globals.h"
#include "XMLList.h"
#include "HelperFunctions.h"
#include "ConfigurationChecker.h"
#include "GlobalSingletonStorage.h"
#include "PlatformDependent.h"

_SVendorInfo::_SVendorInfo():
	MP_VECTOR_INIT(leadVers),
	MP_STRING_INIT(vendorName)
{
	majorVer = 0;
	minorVer = 0;
	buildVer = 0;
}

_SVendorInfo::_SVendorInfo(const _SVendorInfo& v):
	MP_VECTOR_INIT(leadVers),
	MP_STRING_INIT(vendorName)
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

CConfigurationChecker::CConfigurationChecker():
	MP_VECTOR_INIT(m_supportedVendors),
	MP_VECTOR_INIT(m_invalidDrivers),
	MP_VECTOR_INIT(m_requiredExtensions),
	m_isVendorSupportedResult(VENDOR_SUPPORT_UNCHECKED)
{

}

void CConfigurationChecker::InitFromXML(std::wstring path)
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

	CXMLList vendors(path + L"vendors.xml", optionalFields, true);

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

		vendor.majorVer = rtl_atoi(majorVer.c_str());
		vendor.minorVer = rtl_atoi(minorVer.c_str());
		vendor.buildVer = rtl_atoi(buildVer.c_str());

		int pos = 0;
		if (leadVers.size() > 0)
		while (pos != -1)
		{
			int leadVer = rtl_atoi(leadVers.c_str() + pos);
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

	CXMLList invalidDrivers(path + L"invalid_drivers.xml", optionalFields, true);

	if ( g)
	{
		SInvalidDriverInfo invalidDriver;
		std::string version;
		while (invalidDrivers.GetNextValue(vendorName))
		{		
			invalidDrivers.GetOptionalFieldValue(driverVersionOptionFieldName, version);

	#pragma warning(push)
	#pragma warning(disable : 4239)
			std::string& deviceStringID = g->gi.GetDeviceStringID(vendorName);
	#pragma warning(pop)
			invalidDriver.vendorID = g->gi.GetVendorID(deviceStringID);

			unsigned int pointPos = version.find(".");
			invalidDriver.majorVer = rtl_atoi(version.c_str());
			invalidDriver.minorVer = rtl_atoi(version.c_str() + pointPos + 1);
			
			m_invalidDrivers.push_back(invalidDriver);
		}
	}
	
	// extensions
	CXMLList extensions(path + L"extensions.xml", true);

	std::string extension;
	while (extensions.GetNextValue(extension))
	{
		m_requiredExtensions.push_back(MAKE_MP_STRING(extension));
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
	m_requiredExtensions.push_back(MAKE_MP_STRING(extension));
}

int CConfigurationChecker::IsVendorSupported()
{	
	if (m_isVendorSupportedResult != VENDOR_SUPPORT_UNCHECKED)
	{		
		return m_isVendorSupportedResult;
	}

    std::string vendor = g->gi.GetVendorGLName();
	vendor = StringToLower(vendor);

	// OpenGL Software Emulation?
	if (vendor.find("microsoft") != -1)
	{		
		m_isVendorSupportedResult = DRIVERS_NOT_INSTALLED;
		return DRIVERS_NOT_INSTALLED;
	}	

	int leadVersion, majorVersion, minorVersion;
	g->gi.GetDriverVersion(leadVersion, majorVersion, minorVersion);
	int vendorID = g->gi.GetVendorID();

	// checking for invalid driver
	MP_VECTOR<SInvalidDriverInfo>::iterator it1 = m_invalidDrivers.begin(); 
	MP_VECTOR<SInvalidDriverInfo>::iterator itEnd1 = m_invalidDrivers.end();
	for ( ; it1 != itEnd1; it1++)
	{
		if (vendorID == it1->vendorID)
		{
			if ((it1->majorVer == majorVersion) && (it1->minorVer == minorVersion))
			{
				m_isVendorSupportedResult = DRIVER_INVALID;
				return DRIVER_INVALID;
			}
		}
	}

	
	MP_VECTOR<SVendorInfo>::iterator it2 = m_supportedVendors.begin();
	MP_VECTOR<SVendorInfo>::iterator itEnd2 = m_supportedVendors.end();
	for ( ; it2 != itEnd2; it2++)
	{
		std::string lcVendorName = StringToLower((*it2).vendorName);
		if (vendor.find(lcVendorName) != -1)
		{
			// —равниваем версию с минимально установленной
			if ((majorVersion < (*it2).majorVer) || ((majorVersion == (*it2).majorVer) && (minorVersion < (*it2).minorVer)))
			{
				if (g->gi.GetVendorID() == VENDOR_ATI)
				if (majorVersion == 0 && minorVersion == 0 && IsAtiBuildSupported((*it2).buildVer))
				{
					m_isVendorSupportedResult = VENDOR_OK;
					return VENDOR_OK;
				}

				if (g->gi.GetVendorID() == VENDOR_NVIDIA)
				{
					SOpenGLVersion ver = g->gi.GetOpenGLVersionNumber();
					if (ver.major >= 2)
					{
						m_isVendorSupportedResult = VENDOR_OK;
						return VENDOR_OK;
					}
				}

				m_isVendorSupportedResult = DRIVER_UNSUPPORTED_VERSION;
				return DRIVER_UNSUPPORTED_VERSION;
			}

			if ((*it2).leadVers.size() > 0)
			{
				bool isFoundLeadVersion = false;
			
				MP_VECTOR<int>::iterator lIt = (*it2).leadVers.begin();
				MP_VECTOR<int>::iterator lItEnd = (*it2).leadVers.end();

				for ( ; lIt != lItEnd; lIt++)
				if (*lIt == leadVersion)
				{
					isFoundLeadVersion = true;
					break;
				}

				if (!isFoundLeadVersion)
				{
					m_isVendorSupportedResult = DRIVER_UNSUPPORTED_VERSION;
					return DRIVER_UNSUPPORTED_VERSION;
				}
			}
			
			m_isVendorSupportedResult = VENDOR_OK;
			return VENDOR_OK;
		}
	}
			
	m_isVendorSupportedResult = VENDOR_UNSUPPORTED;
	return VENDOR_UNSUPPORTED;
}

bool CConfigurationChecker::AreRequiredExtensionsExists()
{
	std::string extensions = g->ei.GetExtensionsString();
	
	MP_VECTOR<MP_STRING>::iterator it = m_requiredExtensions.begin();
	MP_VECTOR<MP_STRING>::iterator itEnd = m_requiredExtensions.end();
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
#ifdef ENABLE_INTEL_EMULATION
	return false;
#endif

	if (!g->ei.IsExtensionSupported(GLEXT_ext_framebuffer_object))
	{
		return false;
	}

	if (g->gi.GetVendorID() == VENDOR_ATI)
	{
		int leadVersion, majorVersion, minorVersion;
		g->gi.GetDriverVersion(leadVersion, majorVersion, minorVersion);

		if (majorVersion >= 8)
		{
			return true;
		}

		MP_VECTOR<SVendorInfo>::iterator it = m_supportedVendors.begin();
		MP_VECTOR<SVendorInfo>::iterator itEnd = m_supportedVendors.end();
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
	else if (g->gi.GetVendorID() == VENDOR_NVIDIA)
	{
		int lead, major, minor;
		g->gi.GetDriverVersion(lead, major, minor);
		return (major > 81);
	}
	else
	{
		return true;
	}
}

bool CConfigurationChecker::IsTextureRenderingSupported()
{
	bool isNotSupported = ((g->gi.GetVendorID() == VENDOR_INTEL) || (g->gi.GetVendorID() == VENDOR_SIS) || (!g->cc.AreExtendedAbilitiesSupported()) || 
		(g->rs.GetInt(TEXTURE_SIZE_DIV) != 1));	

	if (g->gi.GetVendorID() == VENDOR_ATI)
	{
		int major, minor, lead;
		g->gi.GetDriverVersion(lead, major, minor);
		if ((major < 8) || ((major == 8) && (minor <= 7)))
		{
			isNotSupported = true;
		}
	}

	if (g->gi.GetVendorID() == VENDOR_ATI)
	{
		return false;
	}

	return (!isNotSupported);
}

bool CConfigurationChecker::IsAtiBuildSupported(int& buildVer)
{
	return (g->gi.GetAtiBuildNumber() > buildVer);	
}

CConfigurationChecker::~CConfigurationChecker()
{
}


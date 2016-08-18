	
#include "StdAfx.h"
#include <glew.h>
#include "HelperFunctions.h"
#include <Windows.h>
#include <IoStream>
#include "GPUInfo.h"
#include "Globals.h"
#include <nvapi.h>
#include "NvApiDriverSettings.h"
#include <map>
#include "GlobalSingletonStorage.h"
#include "PlatformDependent.h"
#include "XMLList.h"

#define DEF_VENDOR(x, y, z) m_vendorList.insert(MP_MAP<MP_STRING, MP_STRING>::value_type(MAKE_MP_STRING(x), MAKE_MP_STRING(y))); m_vendorListID.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(x), z));
#define DEF_DEVICE(x, y) m_deviceList.insert(MP_MAP<MP_STRING, MP_STRING>::value_type(MAKE_MP_STRING(x), MAKE_MP_STRING(y)));
#define DEF_DEVICE_INV(y, x) m_deviceList.insert(MP_MAP<MP_STRING, MP_STRING>::value_type(MAKE_MP_STRING(x), MAKE_MP_STRING(y)));
#define DEF_NON_NPOT_SIGNATURE(x) m_nonNPOTDevices.push_back(MAKE_MP_STRING(x));

CGPUInfo::CGPUInfo():
	m_isAnalyzed(false),
	m_isSupportNPOT(-1),
	m_optimusStatus(-1),
	MP_MAP_INIT(m_vendorList),
	MP_MAP_INIT(m_vendorListID),
	MP_MAP_INIT(m_deviceList),
	MP_VECTOR_INIT(m_nonNPOTDevices),
	MP_MAP_INIT(m_atiDriverVersionByBuild),
	MP_STRING_INIT(m_deviceStringID),
	MP_STRING_INIT(m_vendorStringID)
{
	InitVendorList();
	InitDeviceList();
	InitAtiDriverVersionByBuildList();
}

std::string ReadRegKey(std::string path, std::string key)
{	 
	HKEY rKey;
	char str[1024];	
	DWORD strSize = sizeof(str);
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_QUERY_VALUE, &rKey) != ERROR_SUCCESS)
	{		
		return "";
	}
	if (RegQueryValueEx(rKey, (char*)key.c_str(), NULL, NULL, (LPBYTE)&str[0], &strSize) != ERROR_SUCCESS)
	{		
		return "";
	}
	if (RegCloseKey(rKey) != ERROR_SUCCESS)
	{		
		return "";
	}

	std::string s = str;

	return s;
}

bool CGPUInfo::GetDriverLink(std::string& url, std::string& videoURL, const std::string lang, unsigned char& linkType)
{
	std::string _bits = IntToStr(g->ci.GetOSBits());
	std::string _os = g->ci.GetOSVersionHumanString();
	std::string _generation = IntToStr(g->gi.GetVideocardGeneration());
	std::string _vendor = g->gi.GetVendorName();

	std::vector<std::string> optionalFields;
	optionalFields.push_back("os");
	optionalFields.push_back("bits");
	optionalFields.push_back("generation");
	optionalFields.push_back("vendor");
	optionalFields.push_back("type");
	CXMLList list(GetApplicationRootDirectory() + L"drivers_links.xml", optionalFields, true);

	linkType = LINK_TYPE_DIRECT_URL;

	std::string _url;
	while (list.GetNextValue(_url))
	{
		std::string os;
		if (!list.GetOptionalFieldValue("os", os))
		{
			continue;
		}

		if ((os != _os) && (os != "any"))
		{
			continue;
		}

		std::string bits;
		if (!list.GetOptionalFieldValue("bits", bits))
		{
			continue;
		}

		if ((bits != _bits) && (bits != "any"))
		{
			continue;
		}

		std::string generation;
		if (!list.GetOptionalFieldValue("generation", generation))
		{
			continue;
		}

		if ((generation != _generation) && (generation != "any"))
		{
			continue;
		}

		std::string vendor;
		if (!list.GetOptionalFieldValue("vendor", vendor))
		{
			continue;
		}

		if ((vendor != _vendor) && (vendor != "any"))
		{
			continue;
		}

		url = _url;

		if ((g->gi.GetVendorID() == VENDOR_NVIDIA) && (lang == "eng"))
		{
			StringReplace(url, ".ru", ".com");
			StringReplace(url, "-ru", "");
		}

		std::string linkTypeStr;
		if (list.GetOptionalFieldValue("type", linkTypeStr))
		if (linkTypeStr == "site_autodetect")
		{
			linkType = LINK_TYPE_SITE_AUTODETECT;
		}
		else if (linkTypeStr == "standalone_autodetect")
		{
			linkType = LINK_TYPE_STANDALONE_AUTODETECT;
		}
		else if (linkTypeStr == "select_on_site")
		{
			linkType = LINK_TYPE_SELECT_ON_SITE;
		}

		videoURL = GetDriverVideoLink(lang);

		return true;
	}

	return false;
}

std::string CGPUInfo::GetDriverVideoLink(const std::string _lang)
{
	std::vector<std::string> optionalFields;
	optionalFields.push_back("vendor");
	optionalFields.push_back("lang");	
	CXMLList list(GetApplicationRootDirectory() + L"driver_video.xml", optionalFields, true);

	std::string _url;
	while (list.GetNextValue(_url))
	{
		std::string lang;
		if (!list.GetOptionalFieldValue("lang", lang))
		{
			continue;
		}

		std::string vendor;
		if (!list.GetOptionalFieldValue("vendor", vendor))
		{
			continue;
		}

		if ((_lang == lang) && (vendor == GetVendorName()))
		{
			return _url;
		}
	}

	return "";
}

int CGPUInfo::GetAtiBuildNumber()
{
	std::string versionAtiGl = g->gi.GetGLVersion();
	
	int buildVersion = 0;
	unsigned int pos = versionAtiGl.find(".");
	if (pos != -1)
	{
		pos = versionAtiGl.find(".", pos + 1);
		if (pos != -1)
		{
			std::string strBuild = versionAtiGl.substr(pos + 1, versionAtiGl.length() - pos - 1);
			buildVersion = rtl_atoi(strBuild.c_str());		
		}
	}

	return buildVersion;
}

SOpenGLVersion CGPUInfo::GetOpenGLVersionNumber()
{
	SOpenGLVersion result;

	std::string version = g->gi.GetGLVersion();
	int pos = version.find_first_of(".");
	if (pos != -1)
	{
		if (pos - 1 >= 0)
		{
			result.major = version[pos - 1] - '0';
		}

		if ((unsigned int)(pos + 1) < version.size())
		{
			result.minor = version[pos + 1] - '0';
		}
	}

	return result;
}

void CGPUInfo::InitAtiDriverVersionByBuildList()
{
#define DEF_VERSION(b, v1, v2) { unsigned int a; *(unsigned short*)&a = v1; *((unsigned short*)&a + 1) = v2; m_atiDriverVersionByBuild.insert(std::map<unsigned int, unsigned int>::value_type(b, a)); }

	// 2008
	DEF_VERSION(7278, 8, 2);
	DEF_VERSION(7412, 8, 3);
	DEF_VERSION(7412, 8, 4);
	DEF_VERSION(7537, 8, 5);
	DEF_VERSION(7659, 8, 6);
	DEF_VERSION(7767, 8, 7);
	DEF_VERSION(7769, 8, 7);
	DEF_VERSION(7873, 8, 8);
	DEF_VERSION(7976, 8, 9);
	DEF_VERSION(8086, 8, 10);
	DEF_VERSION(8201, 8, 11);
	DEF_VERSION(8304, 8, 12);

	// 2009
	DEF_VERSION(8395, 9, 1);
	DEF_VERSION(8494, 9, 2);
	DEF_VERSION(8543, 9, 3);
	DEF_VERSION(8577, 9, 4);
	DEF_VERSION(8664, 9, 5);
	DEF_VERSION(8673, 9, 6);
	DEF_VERSION(8787, 9, 7);
	DEF_VERSION(8870, 9, 8);
	DEF_VERSION(8918, 9, 9);
	DEF_VERSION(9026, 9, 10);
	DEF_VERSION(9116, 9, 11);
	DEF_VERSION(9232, 9, 12);

	// 2010
	DEF_VERSION(9252, 10, 1);
	DEF_VERSION(9551, 10, 2);
	DEF_VERSION(9704, 10, 3);
	DEF_VERSION(9756, 10, 4);
	DEF_VERSION(9836, 10, 5);
	DEF_VERSION(9901, 10, 6);
	DEF_VERSION(10061, 10, 7);
	DEF_VERSION(10151, 10, 8);
	DEF_VERSION(10188, 10, 9); 
	DEF_VERSION(10243, 10, 10);
	DEF_VERSION(10317, 10, 11);
	DEF_VERSION(10362, 10, 12);

	// 2011
	DEF_VERSION(10516, 11, 1); // 11.1 hotfix jan26
	DEF_VERSION(10524, 11, 2);
	DEF_VERSION(10600, 11, 3);
	DEF_VERSION(10664, 11, 4);
	DEF_VERSION(10750, 11, 5);
	DEF_VERSION(10834, 11, 6);
	DEF_VERSION(10907, 11, 7);
	DEF_VERSION(11005, 11, 8);
	DEF_VERSION(11079, 11, 9);
	DEF_VERSION(11161, 11, 10);
	DEF_VERSION(11251, 11, 11);
	DEF_VERSION(11318, 11, 12);

	// 2012
	DEF_VERSION(11399, 12, 1);
	DEF_VERSION(11554, 12, 2);
	DEF_VERSION(11566, 12, 3);
	DEF_VERSION(11631, 12, 4);
	DEF_VERSION(11744, 12, 6);
	DEF_VERSION(11653, 12, 6);
	DEF_VERSION(11762, 12, 8);
	DEF_VERSION(11931, 12, 10);

	// 2013
	DEF_VERSION(11672, 13, 0); // legacy 13.1 whql
	DEF_VERSION(12002, 13, 1);
	DEF_VERSION(12217, 13, 4);
}

void CGPUInfo::GetDeviceAndVendorIDs(std::string& vendorStringID, std::string& deviceStringID)
{
	int i = 0;
	std::string id;
	DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);

	// locate primary display device
	while (EnumDisplayDevices(NULL, i, &dd, 0))
	{
		if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
		{
			//id = WideToChar(dd.DeviceID);
			id = dd.DeviceID;
			break;
		}
		i++;
	}

	if (id == "") 
	{
		id = ReadRegKey("SYSTEM\\CurrentControlSet\\Control\\Watchdog\\Display", "HardwareID");
		if (id == "")
		{
			id = ReadRegKey("SYSTEM\\ControlSet001\\services\\vga\\Enum", "0");
			if (id == "")
			{
				vendorStringID = "";
				deviceStringID = "";
				return;
			}
		}		
	}

	// get vendor ID
	vendorStringID = id.substr(8, 4);

	// get device ID
	deviceStringID = id.substr(17, 4);
}

bool CGPUInfo::Analyze()
{
	GetDeviceAndVendorIDs(m_vendorStringID, m_deviceStringID);

	if ((m_vendorStringID == "") && (m_deviceStringID == ""))
	{
		return false;
	}

	std::string glVendor = GetVendorGLName();
	glVendor = StringToLower(glVendor);
	
	if ((GetVendorID() == VENDOR_INTEL) && (glVendor.find("nvidia") != -1))
	{
		m_vendorStringID = "10DE";
		m_optimusStatus = OPTIMUS_NVIDIA_USED;
	}	

	if (GetVendorID() == VENDOR_NVIDIA)	
	{
		DisableNVidiaDriverMultithreading();
	}

	m_isAnalyzed = true;
	return true;
}

unsigned char CGPUInfo::GetOptimusDeviceStatus()
{
	if (!m_isAnalyzed)
	{
		Analyze();
	}

	if (-1 == m_optimusStatus)
	{
		if (GetVendorID() == VENDOR_INTEL) 
		{
			int major, minor;
			GetNVidiaDriverVersion(major, minor);

			if (major > 81) 
			{
				m_optimusStatus = OPTIMUS_INTEL_USED;
			}
			else
			{
				m_optimusStatus = OPTIMUS_NOT_FOUND;
			}
		}
		else
		{
			m_optimusStatus = OPTIMUS_NOT_FOUND;
		}
	}

	return m_optimusStatus;
}

bool CGPUInfo::IsOfficialVersion()
{
	return true;

	if (GetVendorID() != VENDOR_ATI)
	{
		return true;
	}

	if (g->ci.IsLaptop())
	{
		return true;
	}

	unsigned int buildNumber = GetAtiBuildNumber();

	// если номер билда находится в диапозоне из таблицы, но не равен к-л билду из него, то версия неоф.
	// если номер больше известного в таблице, то возможно новая версия драйвера, о кот-й еще не знаем

	unsigned int maxValue = 0;
	unsigned int minValue = 0;

	std::map<unsigned int, unsigned int>::iterator it = m_atiDriverVersionByBuild.find(buildNumber);
	std::map<unsigned int, unsigned int>::iterator itEnd = m_atiDriverVersionByBuild.end();
	if (it != itEnd)
	{
		return true;
	}
	else
	{
		it = m_atiDriverVersionByBuild.begin();
		for (;it!=itEnd; it++)
		{
			if ( (*it).first > buildNumber)
			{
				maxValue = (*it).first;
			}
			else
			{
				minValue = (*it).first;
			}
		}

		if (minValue==0 || maxValue==0)
			return true;
	}

	
	return false;
}

std::string CGPUInfo::GetVendorStringID()
{
	return m_vendorStringID;
}

std::string CGPUInfo::GetVendorName()
{
	MP_MAP<MP_STRING, MP_STRING>::iterator iter = m_vendorList.find(MAKE_MP_STRING(m_vendorStringID));

	if (iter != m_vendorList.end())
	{
		return (*iter).second;
	}
	else
	{
		return "Unknown";
	}
}

std::string CGPUInfo::GetVendorGLName()
{
	unsigned char* vendor = (unsigned char*)GLFUNCR(glGetString(GL_VENDOR));
	if (vendor)
	{
		return (char*)vendor;
	}
	else
	{
		return "";
	}
}

std::string CGPUInfo::GetRendererGLName()
{
	unsigned char* renderer = (unsigned char*)GLFUNCR(glGetString(GL_RENDERER));
	if (renderer)
	{
		return (char*)renderer;
	}
	else
	{
		return "";
	}
}

std::string CGPUInfo::GetGLVersion()
{
	unsigned char* version = (unsigned char*)GLFUNCR(glGetString(GL_VERSION));
	if (version)
	{
		return (char*)version;
	}
	else
	{
		return "";
	}
}

int CGPUInfo::GetVendorID()
{
	MP_MAP<MP_STRING, int>::iterator iter = m_vendorListID.find(MAKE_MP_STRING(m_vendorStringID));

	if (iter != m_vendorListID.end())
	{
		return (*iter).second;
	}
	else
	{
		return VENDOR_OTHER;
	}
}

int CGPUInfo::GetVendorID(std::string& vendorStringID)
{
	MP_MAP<MP_STRING, int>::iterator iter = m_vendorListID.find(MAKE_MP_STRING(vendorStringID));

	if (iter != m_vendorListID.end())
	{
		return (*iter).second;
	}
	else
	{
		return VENDOR_OTHER;
	}
}

std::string CGPUInfo::GetDeviceStringID()
{
	return m_deviceStringID;
}

std::string CGPUInfo::GetDeviceName()
{
	MP_MAP<MP_STRING, MP_STRING>::iterator iter = m_deviceList.find(m_deviceStringID);

	if (iter != m_deviceList.end())
	{
		return (*iter).second;
	}
	else
	{
		return "Unknown";
	}
}

std::string CGPUInfo::GetDeviceStringID(std::string& deviceName)
{
	MP_MAP<MP_STRING, MP_STRING>::iterator it = m_vendorList.begin();
	MP_MAP<MP_STRING, MP_STRING>::iterator itEnd = m_vendorList.end();

	for ( ; it != itEnd; ++it)
	{
		if (it->second.find(deviceName) != -1)
		{
			return it->first;
		}
	}
	return "Unknown";
}

int CGPUInfo::GetMaxAnisotropy()
{
	if (g->ei.IsExtensionSupported(GLEXT_ext_texture_filter_anisotropic))
	{
		if (g->gi.GetVendorID() == VENDOR_INTEL)
		{
			return 0;
		}

		if (g->gi.GetVendorID() == VENDOR_SIS)
		{
			return 0;
		}

		int a;
		GLFUNC(glGetIntegerv)(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &a);
		return a; 
	}
	else
	{
		return 0;
	}
}

unsigned char CGPUInfo::GetMaxAntialiasingPower()
{
	if (!g->ei.IsExtensionSupported(GLEXT_ext_framebuffer_multisample))
	{
		return 1;
	}

	if (!g->ei.IsExtensionSupported(GLEXT_ext_framebuffer_blit))
	{
		return 1;
	}

	if (g->rs.GetInt(TEXTURE_SIZE_DIV) > 1)
	{
		return 1;
	}

	GLint sampleCount = 0;
	GLFUNC(glGetIntegerv)(GL_MAX_SAMPLES_EXT, &sampleCount);

	// теперь попробуем усечь возвращаемое значение с тем, чтобы
	// 1. заведомо не тормозило
	// 2. не вылетало с ошибкой в дровах
	// 3. не добавляло новых багов
	// для этого примем условия
	// 256 мб видеопамяти - максимум 4х
	// 512 мб видеопамяти - максимум 8х
	int videoMemorySize = g->gi.GetVideoMemory()->GetTotalSize();
	if ((g->gi.GetVendorID() == VENDOR_ATI) && (g->ci.GetDisplayCount() > 1))
	{
		videoMemorySize /= 2;
	}
	if ((videoMemorySize < 400 * 1024 * 1024) && (sampleCount > 4))
	{
		sampleCount = 4;
	}
	if ((videoMemorySize < 800 * 1024 * 1024) && (sampleCount > 8))
	{
		sampleCount = 8;
	}

	if (g->gi.GetVendorID() == VENDOR_ATI)
	{
		int major, minor;
		GetAtiDriverVersion(major, minor);
		if ((major < 9) || ((major == 9) && (minor < 8)))
		{
			// ATI drivers version 9.7 and lower don't support MSAA properly
			sampleCount = 1;
		}
	}

	if (g->gi.GetVendorID() == VENDOR_NVIDIA)
	{
		int major, minor;
		GetNVidiaDriverVersion(major, minor);
		if (major < 169)
		{
			sampleCount = 1;
		}
	}

	return (unsigned char)sampleCount;
}

void CGPUInfo::GetDriverVersion(int& leadVersion, int& majorVersion, int& minorVersion)
{
	int vendorID = GetVendorID();
	leadVersion = 0;
	switch(vendorID)
	{
	case VENDOR_ATI:
		GetAtiDriverVersion(majorVersion, minorVersion);
		break;

	case VENDOR_NVIDIA:
		GetNVidiaDriverVersion(majorVersion, minorVersion);
		break;

	case VENDOR_INTEL:
		GetIntelDriverVersion(leadVersion, majorVersion, minorVersion);
		break;

	case VENDOR_VIRTUALBOX:
		GetVirtualBoxDriverVersion(majorVersion, minorVersion);
		break;

	default:
		{
			majorVersion = 0;
			minorVersion = 0;
		}
		break;
	}
}

void CGPUInfo::GetIntelDriverVersion(int& leadVersion, int& majorVersion, int& minorVersion)
{
	majorVersion = 0;
	minorVersion = 0;

	std::string ver = GetGLVersion();
	int pos = ver.find("Build ");
	if (pos != -1)
	{
		ver = ver.substr(pos + 6, ver.size() - pos - 6);
	}

	pos = ver.find_first_of(".");
	leadVersion = rtl_atoi((char *)ver.substr(0, pos).c_str());
	ver = ver.substr(pos + 1, ver.size() - pos);
	pos = ver.find_first_of(".");
	majorVersion = rtl_atoi((char *)ver.substr(0, pos).c_str());
	ver = ver.substr(pos + 1, ver.size() - pos);
	pos = ver.find_first_of(".");
	minorVersion = rtl_atoi((char *)ver.substr(0, pos).c_str());
}

void CGPUInfo::GetAtiDriverVersion(int& majorVersion, int& minorVersion)
{
	// пробуем определить версию по билду
	unsigned int buildNumber = GetAtiBuildNumber();
	std::map<unsigned int, unsigned int>::iterator it = m_atiDriverVersionByBuild.find(buildNumber);
	if (it != m_atiDriverVersionByBuild.end())
	{
		majorVersion = *(unsigned short *)(&((*it).second));
		minorVersion = *((unsigned short *)(&((*it).second)) + 1);		
		return;
	}

	// не получилось - пробуем взять из реестра
	majorVersion = 0;
	minorVersion = 0;

	HKEY hKey = NULL;

	TCHAR currentClassesPath[MAX_PATH] = "SYSTEM\\CurrentControlSet\\Control\\Class\\";

	std::string driverVersionString = "";
	// ищем ключ в реестре, соответствующий классу Display	
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, currentClassesPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		int subKeyIndex = 0;

		TCHAR classKeyNameBuf[MAX_PATH];
		DWORD classKeyNameBufLen = MAX_PATH;

		TCHAR classKeyName[MAX_PATH];
		HKEY hClassKey = NULL;

		TCHAR displayValueBuf[MAX_PATH];
		DWORD displayValueBufLen = MAX_PATH;

		while (RegEnumKeyEx(hKey, subKeyIndex, classKeyNameBuf, &classKeyNameBufLen, 0, NULL, NULL, NULL) != ERROR_NO_MORE_ITEMS)
		{		
			strcpy_s(classKeyName, currentClassesPath);
			strcat_s(classKeyName, classKeyNameBuf);

			RegOpenKeyEx(HKEY_LOCAL_MACHINE, classKeyName, 0, KEY_READ, &hClassKey);

			if (RegQueryValueEx(hClassKey, "Class", 0, NULL, (LPBYTE)displayValueBuf, &displayValueBufLen) == ERROR_SUCCESS)
			{
				if (strcmp(displayValueBuf, "Display") == 0)
				{
					// рекурсивно ищем в поддереве значение у Catalyst_Version
					FindRegistryKeyValue(classKeyName, "Catalyst_Version", driverVersionString);

					RegCloseKey(hClassKey);
					break;
				}
			}

			if (hClassKey != NULL)
			{
				RegCloseKey(hClassKey);
			}

			subKeyIndex++;
			memset(classKeyName, 0, MAX_PATH);
			memset(classKeyNameBuf, 0, MAX_PATH);
			memset(displayValueBuf, 0, MAX_PATH);
			classKeyNameBufLen = MAX_PATH;
			displayValueBufLen = MAX_PATH;
		}
	}

	if (hKey != NULL)
	{
		RegCloseKey(hKey);
	}

	if (driverVersionString != "")
	{
		std::string majorStr, minorStr;
		int versionDotPos = driverVersionString.find('.');

		majorStr = driverVersionString.substr(0, versionDotPos);
		minorStr = driverVersionString.substr(versionDotPos + 1, driverVersionString.size()-(versionDotPos + 1));

		majorVersion = rtl_atoi(majorStr.c_str());
		minorVersion = rtl_atoi(minorStr.c_str());
	}

	if ((0 == minorVersion) && (0 == majorVersion))
	{
		g->lw.WriteLn("Could not detect version of ATI videodriver. Build number is ", buildNumber);
	}
}

bool CGPUInfo::FindRegistryKeyValue(std::string path, std::string key, std::string& val)
{
	HKEY hKey;
	int subKeyIndex = 0;

	TCHAR versionBuf[MAX_PATH];
	DWORD versionBufLen = MAX_PATH;

	// открываем текущий ключ
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		// ищем в нем параметр key
		if (RegQueryValueEx(hKey, key.c_str(), 0, NULL, (LPBYTE)versionBuf, &versionBufLen) == ERROR_SUCCESS)
		{
			// нашли - присваиваем переменной val
			val = versionBuf;
			return true;
		}
		// если параметр key не найден рекурсивно запускаемся для веток
		else
		{
			TCHAR subKeyBuf[MAX_PATH];
			DWORD subKeyBufLen = MAX_PATH;
			path += "\\";

			while (RegEnumKeyEx(hKey, subKeyIndex, subKeyBuf, &subKeyBufLen, 0, NULL, NULL, NULL) != ERROR_NO_MORE_ITEMS)
			{
				if(FindRegistryKeyValue(path + subKeyBuf, key, val))
				{
					RegCloseKey(hKey);
					return true;
				}

				memset(subKeyBuf, 0, MAX_PATH);
				subKeyBufLen = MAX_PATH;

				subKeyIndex ++;
			}
		}
	}

	return false;
}	

/*GL_EXT_texture_array
GL_MAX_ARRAY_TEXTURE_LAYERS_EXT

GL_EXT_texture_buffer_object
GL_MAX_TEXTURE_BUFFER_SIZE_EXT

GL_EXT_geometry_shader4
GL_MAX_VARYING_COMPONENTS_EXT
GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_EXT
GL_MAX_GEOMETRY_VARYING_COMPONENTS_EXT
GL_MAX_VERTEX_VARYING_COMPONENTS_EXT
GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_EXT
GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT
GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_EXT

GL_EXT_fragment_lighting
GL_MAX_FRAGMENT_LIGHTS_EXT
GL_MAX_ACTIVE_LIGHTS_EXT

GL_EXT_framebuffer_multisample
GL_MAX_SAMPLES_EXT

GL_EXT_framebuffer_object
GL_MAX_RENDERBUFFER_SIZE_EXT
GL_MAX_COLOR_ATTACHMENTS_EXT

GL_MAX_TEXTURE_SIZE

GL_EXT_texture3D
GL_MAX_3D_TEXTURE_SIZE

GL_EXT_draw_range_elements
GL_MAX_ELEMENTS_VERTICES
GL_MAX_ELEMENTS_INDICES

GL_ARB_multitexture
GL_MAX_TEXTURE_UNITS

GL_ARB_texture_cube_map
GL_MAX_CUBE_MAP_TEXTURE_SIZE

GL_ARB_draw_buffers
GL_MAX_DRAW_BUFFERS_ARB

GL_MAX_VERTEX_ATTRIBS
GL_MAX_TEXTURE_COORDS
GL_MAX_TEXTURE_IMAGE_UNITS
GL_MAX_FRAGMENT_UNIFORM_COMPONENTS
GL_MAX_VERTEX_UNIFORM_COMPONENTS
GL_MAX_VARYING_FLOATS
GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS

GL_ARB_texture_rectangle
GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB

GL_ARB_vertex_blend
GL_MAX_VERTEX_UNITS_ARB

GL_ARB_fragment_program
GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB
GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB
GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB
GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB
GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB
GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB
GL_MAX_TEXTURE_COORDS_ARB
GL_MAX_TEXTURE_IMAGE_UNITS_ARB

GL_ARB_fragment_shader
GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB

GL_ARB_vertex_program
GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB
GL_MAX_PROGRAM_MATRICES_ARB
GL_MAX_VERTEX_ATTRIBS_ARB
GL_MAX_PROGRAM_INSTRUCTIONS_ARB
GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB
GL_MAX_PROGRAM_TEMPORARIES_ARB
GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB
GL_MAX_PROGRAM_PARAMETERS_ARB
GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB
GL_MAX_PROGRAM_ATTRIBS_ARB
GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB
GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB
GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB
GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB
GL_MAX_PROGRAM_ENV_PARAMETERS_ARB

GL_ARB_vertex_shader
GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB
GL_MAX_VARYING_FLOATS_ARB
GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB
GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB

GL_ATI_draw_buffers
GL_MAX_DRAW_BUFFERS_ATI

GL_ATI_pn_triangles
GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI

GL_ATI_vertex_streams
GL_MAX_VERTEX_STREAMS_ATI

GL_EXT_bindable_uniform
GL_MAX_VERTEX_BINDABLE_UNIFORMS_EXT
GL_MAX_FRAGMENT_BINDABLE_UNIFORMS_EXT
GL_MAX_GEOMETRY_BINDABLE_UNIFORMS_EXT
GL_MAX_BINDABLE_UNIFORM_SIZE_EXT*/

void CGPUInfo::InitVendorList()
{
	DEF_VENDOR("1002", "ATI", VENDOR_ATI);
	DEF_VENDOR("10DE", "NVidia", VENDOR_NVIDIA);
	DEF_VENDOR("8086", "Intel", VENDOR_INTEL);
	DEF_VENDOR("5333", "S3 Graphics", VENDOR_S3);
	DEF_VENDOR("1039", "SiS", VENDOR_SIS);
	DEF_VENDOR("80EE", "VirtualBox", VENDOR_VIRTUALBOX);
}

void CGPUInfo::InitATIDeviceList()
{
	// ATI

	DEF_DEVICE_INV("AMD 760G", "9616");

	// ATI FireStream
	DEF_DEVICE_INV("AMD FireStream 9170", "9519");
	DEF_DEVICE_INV("AMD FireStream 9250", "9452");
	DEF_DEVICE_INV("AMD FireStream 9270", "9450");
	DEF_DEVICE_INV("ATI FireStream 2U", "724E");
	DEF_DEVICE_INV("ATI FireStream 2U Secondary", "726E");

	// ATI Radeon Xpress
	DEF_DEVICE_INV("ATI RADEON E2400", "94CB");
	DEF_DEVICE_INV("ATI RADEON Xpress 1200 Series", "793F");
	DEF_DEVICE_INV("ATI RADEON Xpress 1200 Series", "7941");
	DEF_DEVICE_INV("ATI RADEON Xpress 1200 Series", "7942");
	DEF_DEVICE_INV("ATI RADEON Xpress Series", "5A61");
	DEF_DEVICE_INV("ATI RADEON Xpress Series", "5A63");
	DEF_DEVICE_INV("ATI RADEON Xpress Series", "5A62");
	DEF_DEVICE_INV("ATI RADEON Xpress Series", "5A41");
	DEF_DEVICE_INV("ATI RADEON Xpress Series", "5A43");
	DEF_DEVICE_INV("ATI RADEON Xpress Series", "5A42");
	DEF_DEVICE_INV("ATI RADEON Xpress Series", "5954");
	DEF_DEVICE_INV("ATI RADEON Xpress Series", "5854");
	DEF_DEVICE_INV("ATI RADEON Xpress Series", "5974");
	DEF_DEVICE_INV("ATI RADEON Xpress Series", "5874");
	DEF_DEVICE_INV("ATI RADEON Xpress Series", "5975");
	DEF_DEVICE_INV("ATI RADEON Xpress Series", "5955");

	// ATI Radeon 9500-9800
	DEF_DEVICE_INV("ATI RADEON 9500", "4144");
	DEF_DEVICE_INV("ATI RADEON 9500", "4149");
	DEF_DEVICE_INV("ATI RADEON 9500 PRO / 9700", "4E45");
	DEF_DEVICE_INV("ATI RADEON 9500 PRO / 9700 Secondary", "4E65");
	DEF_DEVICE_INV("ATI RADEON 9500 Secondary", "4164");
	DEF_DEVICE_INV("ATI RADEON 9500 Secondary", "4169");
	DEF_DEVICE_INV("ATI RADEON 9600 TX", "4E46");
	DEF_DEVICE_INV("ATI RADEON 9600 TX Secondary", "4E66");
	DEF_DEVICE_INV("ATI RADEON 9600TX", "4146");
	DEF_DEVICE_INV("ATI RADEON 9600TX Secondary", "4166");
	DEF_DEVICE_INV("ATI RADEON 9700 PRO", "4E44");
	DEF_DEVICE_INV("ATI RADEON 9700 PRO Secondary", "4E64");
	DEF_DEVICE_INV("ATI RADEON 9800", "4E49");
	DEF_DEVICE_INV("ATI RADEON 9800 PRO", "4E48");
	DEF_DEVICE_INV("ATI RADEON 9800 PRO Secondary", "4E68");
	DEF_DEVICE_INV("ATI RADEON 9800 SE", "4148");
	DEF_DEVICE_INV("ATI RADEON 9800 SE Secondary", "4168");
	DEF_DEVICE_INV("ATI RADEON 9800 Secondary", "4E69");
	DEF_DEVICE_INV("ATI RADEON 9800 XT", "4E4A");
	DEF_DEVICE_INV("ATI RADEON 9800 XT Secondary", "4E6A");
	DEF_DEVICE_INV("ATI RADEON 9550/X1050 Series", "4153");
	DEF_DEVICE_INV("ATI RADEON 9550/X1050 Series Secondary", "4173");
	DEF_DEVICE_INV("ATI RADEON 9600 Series", "4150");
	DEF_DEVICE_INV("ATI RADEON 9600 Series", "4E51");
	DEF_DEVICE_INV("ATI RADEON 9600 Series", "4151");
	DEF_DEVICE_INV("ATI RADEON 9600 Series", "4155");
	DEF_DEVICE_INV("ATI RADEON 9600 Series", "4152");
	DEF_DEVICE_INV("ATI RADEON 9600 Series Secondary", "4E71");
	DEF_DEVICE_INV("ATI RADEON 9600 Series Secondary", "4171");
	DEF_DEVICE_INV("ATI RADEON 9600 Series Secondary", "4170");
	DEF_DEVICE_INV("ATI RADEON 9600 Series Secondary", "4175");
	DEF_DEVICE_INV("ATI RADEON 9600 Series Secondary", "4172");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON 9500", "4E52");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON 9550", "4E56");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON 9600/9700 Series", "4E50");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON 9800", "4A4E");

	// ATI Radeon Xxxx
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X300", "5461");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X300", "5460");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X300", "3152");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X600", "3150");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X600 SE", "5462");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X700", "5652");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X700", "5653");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X700 Secondary", "5673");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X700 XL", "564F");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X800", "5D4A");
	DEF_DEVICE_INV("ATI MOBILITY/ATI RADEON X800  XT", "5D48");
	DEF_DEVICE_INV("ATI MOBILITY/ATI RADEON X700", "5653");
	DEF_DEVICE_INV("ATI RADEON X800 GT", "554E");
	DEF_DEVICE_INV("ATI RADEON X800 GT Secondary", "556E");
	DEF_DEVICE_INV("ATI RADEON X800 XL", "554D");
	DEF_DEVICE_INV("ATI RADEON X800 XL Secondary", "556D");
	DEF_DEVICE_INV("ATI RADEON X850 PRO", "4B4B");
	DEF_DEVICE_INV("ATI RADEON X850 PRO Secondary", "4B6B");
	DEF_DEVICE_INV("ATI RADEON X850 SE", "4B4A");
	DEF_DEVICE_INV("ATI RADEON X850 SE Secondary", "4B6A");
	DEF_DEVICE_INV("ATI RADEON X850 XT", "4B49");
	DEF_DEVICE_INV("ATI RADEON X850 XT Platinum Edition", "4B4C");
	DEF_DEVICE_INV("ATI RADEON X850 XT Platinum Edition Secondary", "4B6C");
	DEF_DEVICE_INV("ATI RADEON X850 XT Secondary", "4B69");
	DEF_DEVICE_INV("ATI RADEON X300/X550/X1050 Series", "5B60");
	DEF_DEVICE_INV("ATI RADEON X300/X550/X1050 Series", "5B63");
	DEF_DEVICE_INV("ATI RADEON X300/X550/X1050 Series Secondary", "5B73");
	DEF_DEVICE_INV("ATI RADEON X300/X550/X1050 Series Secondary", "5B70");
	DEF_DEVICE_INV("ATI RADEON X550/X700 Series", "5657");
	DEF_DEVICE_INV("ATI RADEON X550/X700 Series Secondary", "5677");
	DEF_DEVICE_INV("ATI RADEON X600 Series", "5B62");
	DEF_DEVICE_INV("ATI RADEON X600 Series Secondary", "5B72");
	DEF_DEVICE_INV("ATI RADEON X600/X550 Series", "3E50");
	DEF_DEVICE_INV("ATI RADEON X600/X550 Series Secondary", "3E70");
	DEF_DEVICE_INV("ATI RADEON X700", "5E4D");
	DEF_DEVICE_INV("ATI RADEON X700 PRO", "5E4B");
	DEF_DEVICE_INV("ATI RADEON X700 PRO Secondary", "5E6B");
	DEF_DEVICE_INV("ATI RADEON X700 SE", "5E4C");
	DEF_DEVICE_INV("ATI RADEON X700 SE Secondary", "5E6C");
	DEF_DEVICE_INV("ATI RADEON X700 Secondary", "5E6D");
	DEF_DEVICE_INV("ATI RADEON X700 XT", "5E4A");
	DEF_DEVICE_INV("ATI RADEON X700 XT Secondary", "5E6A");
	DEF_DEVICE_INV("ATI RADEON X700/X550 Series", "5E4F");
	DEF_DEVICE_INV("ATI RADEON X700/X550 Series Secondary", "5E6F");
	DEF_DEVICE_INV("ATI RADEON X800 GT", "554B");
	DEF_DEVICE_INV("ATI RADEON X800 GT Secondary", "556B");
	DEF_DEVICE_INV("ATI RADEON X800 GTO", "5549");
	DEF_DEVICE_INV("ATI RADEON X800 GTO", "554F");
	DEF_DEVICE_INV("ATI RADEON X800 GTO", "5D4F");
	DEF_DEVICE_INV("ATI RADEON X800 CrossFire Edition", "554D");
	DEF_DEVICE_INV("ATI RADEON X800 CrossFire Edition Secondary", "556D");
	DEF_DEVICE_INV("ATI RADEON X850 CrossFire Edition", "5D52");
	DEF_DEVICE_INV("ATI RADEON X850 CrossFire Edition Secondary", "5D72");
	DEF_DEVICE_INV("ATI RADEON X800 GTO Secondary", "5569");
	DEF_DEVICE_INV("ATI RADEON X800 GTO Secondary", "556F");
	DEF_DEVICE_INV("ATI RADEON X800 GTO Secondary", "5D6F");
	DEF_DEVICE_INV("ATI RADEON X800 PRO", "4A49");
	DEF_DEVICE_INV("ATI RADEON X800 PRO Secondary", "4A69");
	DEF_DEVICE_INV("ATI RADEON X800 SE", "4A4F");
	DEF_DEVICE_INV("ATI RADEON X800 SE Secondary", "4A6F");
	DEF_DEVICE_INV("ATI RADEON X800 Series", "4A48");
	DEF_DEVICE_INV("ATI RADEON X800 Series", "4A4A");
	DEF_DEVICE_INV("ATI RADEON X800 Series", "5548");
	DEF_DEVICE_INV("ATI RADEON X800 Series", "4A4C");
	DEF_DEVICE_INV("ATI RADEON X800 Series Secondary", "4A68");
	DEF_DEVICE_INV("ATI RADEON X800 Series Secondary", "4A6A");
	DEF_DEVICE_INV("ATI RADEON X800 Series Secondary", "4A6C");
	DEF_DEVICE_INV("ATI RADEON X800 Series Secondary", "5568");
	DEF_DEVICE_INV("ATI RADEON X800 VE", "4A54");
	DEF_DEVICE_INV("ATI RADEON X800 VE Secondary", "4A74");
	DEF_DEVICE_INV("ATI RADEON X800 XT", "4A4B");
	DEF_DEVICE_INV("ATI RADEON X800 XT", "5D57");
	DEF_DEVICE_INV("ATI RADEON X800 XT Platinum Edition", "4A50");
	DEF_DEVICE_INV("ATI RADEON X800 XT Platinum Edition", "554A");
	DEF_DEVICE_INV("ATI RADEON X800 XT Platinum Edition Secondary", "4A70");
	DEF_DEVICE_INV("ATI RADEON X800 XT Platinum Edition Secondary", "556A");
	DEF_DEVICE_INV("ATI RADEON X800 XT Secondary", "4A6B");
	DEF_DEVICE_INV("ATI RADEON X800 XT Secondary", "5D77");
	DEF_DEVICE_INV("ATI RADEON X850 XT", "5D52");
	DEF_DEVICE_INV("ATI RADEON X850 XT Platinum Edition", "5D4D");
	DEF_DEVICE_INV("ATI RADEON X850 XT Platinum Edition Secondary", "5D6D");
	DEF_DEVICE_INV("ATI RADEON X850 XT Secondary", "5D72");

	// ATI Radeon X1xxx
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1300", "714A");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1300", "7149");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1300", "714C");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1300", "714B");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1350", "718B");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1350", "718C");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1350", "7196");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1400", "7145");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1450", "7186");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1450", "718D");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1600", "71C5");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1700", "71D5");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1700", "71DE");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1700 XT", "71D6");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1800", "7102");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1800 XT", "7101");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X1900", "7284");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X2300", "718A");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON X2300", "7188");
	DEF_DEVICE_INV("ATI RADEON X1200 Series", "791E");
	DEF_DEVICE_INV("ATI RADEON X1200 Series", "791F");
	DEF_DEVICE_INV("ATI RADEON X1950 GT", "7288");
	DEF_DEVICE_INV("ATI RADEON X1950 XTX Uber - Limited Edition", "7248");
	DEF_DEVICE_INV("ATI RADEON X1950 XTX Uber - Limited Edition Secondary", "7268");
	DEF_DEVICE_INV("ATI RADEON X1950 GT Secondary", "72A8");
	DEF_DEVICE_INV("ATI RADEON X1300 / X1550 Series", "7146");
	DEF_DEVICE_INV("ATI RADEON X1300 / X1550 Series Secondary", "7166");
	DEF_DEVICE_INV("ATI RADEON X1300 Series", "714D");
	DEF_DEVICE_INV("ATI RADEON X1300 Series", "71C3");
	DEF_DEVICE_INV("ATI RADEON X1300 Series", "718F");
	DEF_DEVICE_INV("ATI RADEON X1300 Series", "714E");
	DEF_DEVICE_INV("ATI RADEON X1300 Series", "715E");
	DEF_DEVICE_INV("ATI RADEON X1300 Series Secondary", "716D");
	DEF_DEVICE_INV("ATI RADEON X1300 Series Secondary", "71E3");
	DEF_DEVICE_INV("ATI RADEON X1300 Series Secondary", "71AF");
	DEF_DEVICE_INV("ATI RADEON X1300 Series Secondary", "716E");
	DEF_DEVICE_INV("ATI RADEON X1300 Series Secondary", "717E");
	DEF_DEVICE_INV("ATI RADEON X1300/X1550 Series", "7142");
	DEF_DEVICE_INV("ATI RADEON X1300/X1550 Series", "7180");
	DEF_DEVICE_INV("ATI RADEON X1300/X1550 Series", "7183");
	DEF_DEVICE_INV("ATI RADEON X1300/X1550 Series", "7187");
	DEF_DEVICE_INV("ATI RADEON X1300/X1550 Series Secondary", "71A7");
	DEF_DEVICE_INV("ATI RADEON X1300/X1550 Series Secondary", "7162");
	DEF_DEVICE_INV("ATI RADEON X1300/X1550 Series Secondary", "71A0");
	DEF_DEVICE_INV("ATI RADEON X1300/X1550 Series Secondary", "71A3");
	DEF_DEVICE_INV("ATI RADEON X1550 64-bit", "719F");
	DEF_DEVICE_INV("ATI RADEON X1550 64-bit", "7147");
	DEF_DEVICE_INV("ATI RADEON X1550 64-bit", "715F");
	DEF_DEVICE_INV("ATI RADEON X1550 64-bit Secondary", "7167");
	DEF_DEVICE_INV("ATI RADEON X1550 64-bit Secondary", "717F");
	DEF_DEVICE_INV("ATI RADEON X1550 Series", "7193");
	DEF_DEVICE_INV("ATI RADEON X1550 Series", "7143");
	DEF_DEVICE_INV("ATI RADEON X1550 Series Secondary", "71B3");
	DEF_DEVICE_INV("ATI RADEON X1550 Series Secondary", "7163");
	DEF_DEVICE_INV("ATI RADEON X1600 Pro / ATI RADEON X1300 XT", "71CE");
	DEF_DEVICE_INV("ATI RADEON X1600 Pro / ATI RADEON X1300 XT Secondary", "71EE");
	DEF_DEVICE_INV("ATI RADEON X1600 Series", "71C0");
	DEF_DEVICE_INV("ATI RADEON X1600 Series", "71C2");
	DEF_DEVICE_INV("ATI RADEON X1600 Series", "7181");
	DEF_DEVICE_INV("ATI RADEON X1600 Series", "71CD");
	DEF_DEVICE_INV("ATI RADEON X1600 Series", "7140");
	DEF_DEVICE_INV("ATI RADEON X1600 Series Secondary", "71E2");
	DEF_DEVICE_INV("ATI RADEON X1600 Series Secondary", "71E6");
	DEF_DEVICE_INV("ATI RADEON X1600 Series Secondary", "71A1");
	DEF_DEVICE_INV("ATI RADEON X1600 Series Secondary", "71ED");
	DEF_DEVICE_INV("ATI RADEON X1600 Series Secondary", "71E0");
	DEF_DEVICE_INV("ATI RADEON X1600 Series Secondary", "7160");
	DEF_DEVICE_INV("ATI RADEON X1650 Series", "71C6");
	DEF_DEVICE_INV("ATI RADEON X1650 Series", "71C1");
	DEF_DEVICE_INV("ATI RADEON X1650 Series", "7293");
	DEF_DEVICE_INV("ATI RADEON X1650 Series", "7291");
	DEF_DEVICE_INV("ATI RADEON X1650 Series", "71C7");
	DEF_DEVICE_INV("ATI RADEON X1650 Series Secondary", "71E1");
	DEF_DEVICE_INV("ATI RADEON X1650 Series Secondary", "72B3");
	DEF_DEVICE_INV("ATI RADEON X1650 Series Secondary", "72B1");
	DEF_DEVICE_INV("ATI RADEON X1650 Series Secondary", "71E7");
	DEF_DEVICE_INV("ATI RADEON X1800 Series", "7100");
	DEF_DEVICE_INV("ATI RADEON X1800 Series", "7108");
	DEF_DEVICE_INV("ATI RADEON X1800 Series", "7109");
	DEF_DEVICE_INV("ATI RADEON X1800 Series", "710A");
	DEF_DEVICE_INV("ATI RADEON X1800 Series", "710B");
	DEF_DEVICE_INV("ATI RADEON X1800 Series", "710C");
	DEF_DEVICE_INV("ATI RADEON X1800 Series Secondary", "7120");
	DEF_DEVICE_INV("ATI RADEON X1800 Series Secondary", "7128");
	DEF_DEVICE_INV("ATI RADEON X1800 Series Secondary", "7129");
	DEF_DEVICE_INV("ATI RADEON X1800 Series Secondary", "712A");
	DEF_DEVICE_INV("ATI RADEON X1800 Series Secondary", "712C");
	DEF_DEVICE_INV("ATI RADEON X1800 Series Secondary", "712B");
	DEF_DEVICE_INV("ATI RADEON X1900 Series", "7243");
	DEF_DEVICE_INV("ATI RADEON X1900 Series", "7245");
	DEF_DEVICE_INV("ATI RADEON X1900 Series", "7246");
	DEF_DEVICE_INV("ATI RADEON X1900 Series", "7247");
	DEF_DEVICE_INV("ATI RADEON X1900 Series", "7248");
	DEF_DEVICE_INV("ATI RADEON X1900 Series", "7249");
	DEF_DEVICE_INV("ATI RADEON X1900 Series", "724A");
	DEF_DEVICE_INV("ATI RADEON X1900 Series", "724B");
	DEF_DEVICE_INV("ATI RADEON X1900 Series", "724C");
	DEF_DEVICE_INV("ATI RADEON X1900 Series", "724D");
	DEF_DEVICE_INV("ATI RADEON X1900 Series", "724F");
	DEF_DEVICE_INV("ATI RADEON X1900 Series Secondary", "7263");
	DEF_DEVICE_INV("ATI RADEON X1900 Series Secondary", "7265");
	DEF_DEVICE_INV("ATI RADEON X1900 Series Secondary", "7266");
	DEF_DEVICE_INV("ATI RADEON X1900 Series Secondary", "7267");
	DEF_DEVICE_INV("ATI RADEON X1900 Series Secondary", "7268");
	DEF_DEVICE_INV("ATI RADEON X1900 Series Secondary", "7269");
	DEF_DEVICE_INV("ATI RADEON X1900 Series Secondary", "726A");
	DEF_DEVICE_INV("ATI RADEON X1900 Series Secondary", "726B");
	DEF_DEVICE_INV("ATI RADEON X1900 Series Secondary", "726C");
	DEF_DEVICE_INV("ATI RADEON X1900 Series Secondary", "726D");
	DEF_DEVICE_INV("ATI RADEON X1900 Series Secondary", "726F");
	DEF_DEVICE_INV("ATI RADEON X1950 Series", "7280");
	DEF_DEVICE_INV("ATI RADEON X1950 Series", "7240");
	DEF_DEVICE_INV("ATI RADEON X1950 Series", "7244");
	DEF_DEVICE_INV("ATI RADEON X1950 Series Secondary", "72A0");
	DEF_DEVICE_INV("ATI RADEON X1950 Series Secondary", "7260");
	DEF_DEVICE_INV("ATI RADEON X1950 Series Secondary", "7264");

	// ATI Radeon HD 2xxx
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON HD 2300", "7210");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON HD 2300", "7211");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON HD 2400", "94C9");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON HD 2400 XT", "94C8");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON HD 2600", "9581");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON HD 2600 XT", "9583");
	DEF_DEVICE_INV("ATI Radeon HD 2350 Series/2400 PRO", "94C3");
	DEF_DEVICE_INV("ATI Radeon HD 2350", "94C7");
	DEF_DEVICE_INV("ATI Radeon HD 2400 XT", "94C1");
	DEF_DEVICE_INV("ATI Radeon HD 2400 Series", "94CC");
	DEF_DEVICE_INV("ATI Radeon HD 2600 X2 Series", "958A");
	DEF_DEVICE_INV("ATI Radeon HD 2600 PRO/XT", "9588");
	DEF_DEVICE_INV("ATI Radeon HD 2900 XT", "9400");
	DEF_DEVICE_INV("ATI RADEON 2100", "796E");
	DEF_DEVICE_INV("ATI RADEON HD 2400 LE", "94C5");
	DEF_DEVICE_INV("ATI RADEON HD 2400 PRO AGP", "94C4");
	DEF_DEVICE_INV("ATI RADEON HD 2600 LE", "958E");
	DEF_DEVICE_INV("ATI RADEON HD 2600 Pro AGP", "9587");
	DEF_DEVICE_INV("ATI RADEON HD 2600 XT AGP", "9586");
	DEF_DEVICE_INV("ATI RADEON HD 2900 PRO", "9403");
	DEF_DEVICE_INV("ATI RADEON HD 2900 XT", "9401");
	DEF_DEVICE_INV("ATI RADEON HD 2900 XT", "9402");
	DEF_DEVICE_INV("ATI MOBILITY / ATI RADEON HD 2600 XT Gemini", "958B");

	// ATI Radeon HD 3xxx
	DEF_DEVICE_INV("ATI RADEON 3100 Graphics", "9613");
	DEF_DEVICE_INV("ATI Radeon 3100 Graphics", "9611");
	DEF_DEVICE_INV("ATI Radeon HD 3200 Graphics", "9610");
	DEF_DEVICE_INV("ATI Radeon HD 3300 Graphics", "9614");
	DEF_DEVICE_INV("ATI Radeon HD 3410/2400 LE", "95C3");
	DEF_DEVICE_INV("ATI Radeon HD 3600 Series/2600 Pro", "9589");
	DEF_DEVICE_INV("ATI Radeon HD 3830", "9507");
	DEF_DEVICE_INV("ATI Radeon HD 3850 X2", "9513");
	DEF_DEVICE_INV("ATI Radeon HD 3870 X2", "950F");
	DEF_DEVICE_INV("ATI RADEON HD 3200 Graphics", "9612");
	DEF_DEVICE_INV("ATI RADEON HD 3430", "95C7");
	DEF_DEVICE_INV("ATI RADEON HD 3470", "95C0");
	DEF_DEVICE_INV("ATI RADEON HD 3600 Series", "9590");
	DEF_DEVICE_INV("ATI RADEON HD 3600 Series", "9596");
	DEF_DEVICE_INV("ATI RADEON HD 3600 Series", "9597");
	DEF_DEVICE_INV("ATI RADEON HD 3600 Series", "9599");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON HD 3400 Series", "95C4");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON HD 3430", "95C2");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON HD 3650", "9591");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON HD 3850", "9504");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON HD 3850 X2", "9506");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON HD 3870", "9508");
	DEF_DEVICE_INV("ATI MOBILITY /ATI RADEON HD 3870 X2", "9509");

	// ATI Radeon HD 4xxx
	DEF_DEVICE_INV("ATI Radeon HD 4200", "9710");
	DEF_DEVICE_INV("ATI Radeon HD 3400/3500/4200 Series", "95C5");
	DEF_DEVICE_INV("ATI Radeon HD 3400/3500/4200 Series", "95C0");
	DEF_DEVICE_INV("ATI Radeon HD 4300/4500 Series", "954F");
	DEF_DEVICE_INV("ATI Radeon HD 4570/4580/3750/3730/3600 Series/All-in-Wonder HD/2600 XT", "9598");
	DEF_DEVICE_INV("ATI Radeon HD 4590/4550", "9540");
	DEF_DEVICE_INV("ATI Radeon HD 4600/4700 Series", "9498");
	DEF_DEVICE_INV("ATI Radeon HD 4600/4700 Series", "9490");
	DEF_DEVICE_INV("ATI Radeon HD 4700 Series", "94B4");
	DEF_DEVICE_INV("ATI Radeon HD 4750/3800 Series/3690", "9501");
	DEF_DEVICE_INV("ATI Radeon HD 4730/4750/3800 Series/3690", "9505");
	DEF_DEVICE_INV("ATI Radeon HD 4770", "94B3");
	DEF_DEVICE_INV("ATI Radeon HD 4770", "94B5");
	DEF_DEVICE_INV("ATI Radeon HD 4810/4820/4700 Series", "944E");
	DEF_DEVICE_INV("ATI Radeon HD 4850", "9442");
	DEF_DEVICE_INV("ATI Radeon HD 4870", "9440");
	DEF_DEVICE_INV("ATI Radeon HD 4830", "944C");
	DEF_DEVICE_INV("ATI Radeon HD 4800 Series", "9460");
	DEF_DEVICE_INV("ATI Radeon HD 4800 Series", "9462");
	DEF_DEVICE_INV("ATI Radeon HD 4850 X2", "9443");
	DEF_DEVICE_INV("ATI Radeon HD 4870 X2", "9441");

	// ATI Radeon HD 5xxx
	DEF_DEVICE_INV("ATI Radeon HD 5400 Series", "68F9");
	DEF_DEVICE_INV("ATI Radeon HD 5500 Series", "68D9");
	DEF_DEVICE_INV("ATI Radeon HD 5600 Series", "68D8");
	DEF_DEVICE_INV("ATI Radeon HD 5700 Series", "68B8");
	DEF_DEVICE_INV("ATI Radeon HD 5700 Series", "68BE");
	DEF_DEVICE_INV("ATI Radeon HD 5800 Series", "6898");
	DEF_DEVICE_INV("ATI Radeon HD 5800 Series", "6899");
	DEF_DEVICE_INV("ATI Radeon HD 5900 Series", "689C");

	// ATI FireGL/FireMV
	DEF_DEVICE_INV("ATI FireGL T2", "4154");
	DEF_DEVICE_INV("ATI FireGL T2 Secondary", "4174");
	DEF_DEVICE_INV("ATI FireGL V3100", "5B64");
	DEF_DEVICE_INV("ATI FireGL V3100 Secondary", "5B74");
	DEF_DEVICE_INV("ATI FireGL V3200", "3E54");
	DEF_DEVICE_INV("ATI FireGL V3200 Secondary", "3E74");
	DEF_DEVICE_INV("ATI FireGL V3300", "7152");
	DEF_DEVICE_INV("ATI FireGL V3300 Secondary", "7172");
	DEF_DEVICE_INV("ATI FireGL V3350", "7153");
	DEF_DEVICE_INV("ATI FireGL V3350 Secondary", "7173");
	DEF_DEVICE_INV("ATI FireGL V3400", "71D2");
	DEF_DEVICE_INV("ATI FireGL V3400 Secondary", "71F2");
	DEF_DEVICE_INV("ATI FireGL V3600", "958D");
	DEF_DEVICE_INV("ATI FireGL V5000", "5E48");
	DEF_DEVICE_INV("ATI FireGL V5000 Secondary", "5E68");
	DEF_DEVICE_INV("ATI FireGL V5100", "5551");
	DEF_DEVICE_INV("ATI FireGL V5100 Secondary", "5571");
	DEF_DEVICE_INV("ATI FireGL V5200", "71DA");
	DEF_DEVICE_INV("ATI FireGL V5200 Secondary", "71FA");
	DEF_DEVICE_INV("ATI FireGL V5300", "7105");
	DEF_DEVICE_INV("ATI FireGL V5300 Secondary", "7125");
	DEF_DEVICE_INV("ATI FireGL V5600", "958C");
	DEF_DEVICE_INV("ATI FireGL V7100", "5550");
	DEF_DEVICE_INV("ATI FireGL V7100 Secondary", "5570");
	DEF_DEVICE_INV("ATI FireGL V7200", "5D50");
	DEF_DEVICE_INV("ATI FireGL V7200", "7104");
	DEF_DEVICE_INV("ATI FireGL V7200 Secondary", "5D70");
	DEF_DEVICE_INV("ATI FireGL V7200 Secondary", "7124");
	DEF_DEVICE_INV("ATI FireGL V7300", "710E");
	DEF_DEVICE_INV("ATI FireGL V7300 Secondary", "712E");
	DEF_DEVICE_INV("ATI FireGL V7350", "710F");
	DEF_DEVICE_INV("ATI FireGL V7350 Secondary", "712F");
	DEF_DEVICE_INV("ATI FireGL V7600", "940F");
	DEF_DEVICE_INV("ATI FireGL V7700", "9511");
	DEF_DEVICE_INV("ATI FireGL V8600", "940B");
	DEF_DEVICE_INV("ATI FireGL V8650", "940A");
	DEF_DEVICE_INV("ATI FireGL X1", "4E47");
	DEF_DEVICE_INV("ATI FireGL X1 Secondary", "4E67");
	DEF_DEVICE_INV("ATI FireGL X2-256/X2-256t", "4E4B");
	DEF_DEVICE_INV("ATI FireGL X2-256/X2-256t Secondary", "4E6B");
	DEF_DEVICE_INV("ATI FireGL X3-256", "4A4D");
	DEF_DEVICE_INV("ATI FireGL X3-256 Secondary", "4A6D");
	DEF_DEVICE_INV("ATI FireGL Z1", "4147");
	DEF_DEVICE_INV("ATI FireGL Z1 Secondary", "4167");
	DEF_DEVICE_INV("ATI FireMV 2200", "5B65");
	DEF_DEVICE_INV("ATI FireMV 2200 Secondary", "5B75");
	DEF_DEVICE_INV("ATI FireMV 2250", "719B");
	DEF_DEVICE_INV("ATI FireMV 2250 Secondary", "71BB");
	DEF_DEVICE_INV("ATI FireMV 2260", "95CE");
	DEF_DEVICE_INV("ATI FireMV 2260", "95CF");
	DEF_DEVICE_INV("ATI FireMV 2400", "3151");
	DEF_DEVICE_INV("ATI FireMV 2400 Secondary", "3171");
	DEF_DEVICE_INV("ATI FireMV 2450", "95CD");
	DEF_DEVICE_INV("ATI MOBILITY FIRE GL T2/T2e", "4E54");
	DEF_DEVICE_INV("ATI MOBILITY FireGL V3100", "5464");
	DEF_DEVICE_INV("ATI MOBILITY FireGL V3200", "3154");
	DEF_DEVICE_INV("ATI MOBILITY FireGL V5000", "564A");
	DEF_DEVICE_INV("ATI MOBILITY FireGL V5000", "564B");
	DEF_DEVICE_INV("ATI MOBILITY FireGL V5100", "5D49");
	DEF_DEVICE_INV("ATI MOBILITY FireGL V5200", "71C4");
	DEF_DEVICE_INV("ATI MOBILITY FireGL V5250", "71D4");
	DEF_DEVICE_INV("ATI MOBILITY FireGL V7100", "7106");
	DEF_DEVICE_INV("ATI MOBILITY FireGL V7200", "7103");
}

void CGPUInfo::InitIntelDeviceList()
{
	// Intel
	DEF_DEVICE("2A42", "GMA 4500MHD");
	DEF_DEVICE("2A42", "GMA 4500MHD");
	DEF_DEVICE("2E42", "GMA 4500");
	DEF_DEVICE("2E43", "GMA 4500");
	DEF_DEVICE("2E92", "GMA 4500");
	DEF_DEVICE("2E93", "GMA 4500");
	DEF_DEVICE("2E12", "GMA 4500");
	DEF_DEVICE("2E13", "GMA 4500");
	DEF_DEVICE("2E32", "GMA X4500");
	DEF_DEVICE("2E33", "GMA X4500");
	DEF_DEVICE("2E22", "GMA X4500HD");
	DEF_DEVICE("2E23", "GMA X4500HD");
	DEF_DEVICE("2A02", "GMA X3100");
	DEF_DEVICE("2A03", "GMA X3100");
	DEF_DEVICE("2A12", "GMA X3100");
	DEF_DEVICE("2A13", "GMA X3100");
	DEF_DEVICE("2982", "GMA X3500");
	DEF_DEVICE("2983", "GMA X3500");
	DEF_DEVICE("29A2", "GMA X3000");
	DEF_DEVICE("29A3", "GMA X3000");
	DEF_DEVICE("8108", "GMA 500");
	DEF_DEVICE("8109", "GMA 500");
	DEF_DEVICE("A001", "GMA 3150");
	DEF_DEVICE("A002", "GMA 3150");
	DEF_DEVICE("A011", "GMA 3150");
	DEF_DEVICE("A012", "GMA 3150");
	DEF_DEVICE("29D2", "GMA 3100");
	DEF_DEVICE("29D3", "GMA 3100");
	DEF_DEVICE("29B2", "GMA 3100");
	DEF_DEVICE("29B3", "GMA 3100");
	DEF_DEVICE("29C2", "GMA 3100");
	DEF_DEVICE("29C3", "GMA 3100");
	DEF_DEVICE("2972", "GMA 3000");
	DEF_DEVICE("2973", "GMA 3000");
	DEF_DEVICE("2992", "GMA 3000");
	DEF_DEVICE("2993", "GMA 3000");
	DEF_DEVICE("2772", "GMA 950");
	DEF_DEVICE("2776", "GMA 950");
	DEF_DEVICE("27A0", "GMA 945");
	DEF_DEVICE("27A2", "GMA 950");
	DEF_DEVICE("27A6", "GMA 950");
	DEF_DEVICE("27AС", "GMA 945");
	DEF_DEVICE("27AE", "GMA 950");
	DEF_DEVICE("2582", "GMA 900");
	DEF_DEVICE("2782", "GMA 900");
	DEF_DEVICE("2592", "GMA 900");
	DEF_DEVICE("2792", "GMA 900");
}

void CGPUInfo::InitNVidiaDeviceList()
{
	// NVidia
	DEF_DEVICE("0040", "NVIDIA GeForce 6800 Ultra");
	DEF_DEVICE("0041", "NVIDIA GeForce 6800");
	DEF_DEVICE("0042", "NVIDIA GeForce 6800 LE");
	DEF_DEVICE("0043", "NVIDIA GeForce 6800 XE");
	DEF_DEVICE("0044", "NVIDIA GeForce 6800 XT");
	DEF_DEVICE("0045", "NVIDIA GeForce 6800 GT");
	DEF_DEVICE("0046", "NVIDIA GeForce 6800 GT");
	DEF_DEVICE("0047", "NVIDIA GeForce 6800 GS");
	DEF_DEVICE("0048", "NVIDIA GeForce 6800 XT");
	DEF_DEVICE("004D", "NVIDIA Quadro FX 3400");
	DEF_DEVICE("004E", "NVIDIA Quadro FX 4000");
	DEF_DEVICE("0090", "NVIDIA GeForce 7800 GTX");
	DEF_DEVICE("0091", "NVIDIA GeForce 7800 GTX");
	DEF_DEVICE("0092", "NVIDIA GeForce 7800 GT");
	DEF_DEVICE("0093", "NVIDIA GeForce 7800 GS");
	DEF_DEVICE("0095", "NVIDIA GeForce 7800 SLI");
	DEF_DEVICE("009D", "NVIDIA Quadro FX 4500");
	DEF_DEVICE("00C0", "NVIDIA GeForce 6800 GS");
	DEF_DEVICE("00C1", "NVIDIA GeForce 6800");
	DEF_DEVICE("00C2", "NVIDIA GeForce 6800 LE");
	DEF_DEVICE("00C3", "NVIDIA GeForce 6800 XT");
	DEF_DEVICE("00CD", "NVIDIA Quadro FX 3450/4000 SDI");
	DEF_DEVICE("00CE", "NVIDIA Quadro FX 1400");
	DEF_DEVICE("00F1", "NVIDIA GeForce 6600 GT");
	DEF_DEVICE("00F2", "NVIDIA GeForce 6600");
	DEF_DEVICE("00F3", "NVIDIA GeForce 6200");
	DEF_DEVICE("00F4", "NVIDIA GeForce 6600 LE");
	DEF_DEVICE("00F5", "NVIDIA GeForce 7800 GS");
	DEF_DEVICE("00F6", "NVIDIA GeForce 6800 GS/XT");
	DEF_DEVICE("00F8", "NVIDIA Quadro FX 3400/4400");
	DEF_DEVICE("00F9", "NVIDIA GeForce 6800 Series GPU");
	DEF_DEVICE("0140", "NVIDIA GeForce 6600 GT");
	DEF_DEVICE("0141", "NVIDIA GeForce 6600");
	DEF_DEVICE("0142", "NVIDIA GeForce 6600 LE");
	DEF_DEVICE("0143", "NVIDIA GeForce 6600 VE");
	DEF_DEVICE("0145", "NVIDIA GeForce 6610 XL");
	DEF_DEVICE("0147", "NVIDIA GeForce 6700 XL");
	DEF_DEVICE("014A", "NVIDIA Quadro NVS 440");
	DEF_DEVICE("014C", "NVIDIA Quadro FX 540M");
	DEF_DEVICE("014D", "NVIDIA Quadro FX 550");
	DEF_DEVICE("014E", "NVIDIA Quadro FX 540");
	DEF_DEVICE("014F", "NVIDIA GeForce 6200");
	DEF_DEVICE("0160", "NVIDIA GeForce 6500");
	DEF_DEVICE("0161", "NVIDIA GeForce 6200 TurboCache(TM)");
	DEF_DEVICE("0162", "NVIDIA GeForce 6200SE TurboCache(TM)");
	DEF_DEVICE("0163", "NVIDIA GeForce 6200 LE");
	DEF_DEVICE("0165", "NVIDIA Quadro NVS 285");
	DEF_DEVICE("0169", "NVIDIA GeForce 6250");
	DEF_DEVICE("016A", "NVIDIA GeForce 7100 GS");
	DEF_DEVICE("0191", "NVIDIA GeForce 8800 GTX");
	DEF_DEVICE("0193", "NVIDIA GeForce 8800 GTS");
	DEF_DEVICE("0194", "NVIDIA GeForce 8800 Ultra");
	DEF_DEVICE("0197", "NVIDIA Tesla C870");
	DEF_DEVICE("019D", "NVIDIA Quadro FX 5600");
	DEF_DEVICE("019E", "NVIDIA Quadro FX 4600");
	DEF_DEVICE("01D0", "NVIDIA GeForce 7350 LE");
	DEF_DEVICE("01D1", "NVIDIA GeForce 7300 LE");
	DEF_DEVICE("01D2", "NVIDIA GeForce 7550 LE");
	DEF_DEVICE("01D3", "NVIDIA GeForce 7300 SE/7200 GS");
	DEF_DEVICE("01DB", "NVIDIA Quadro NVS 120M");
	DEF_DEVICE("01DD", "NVIDIA GeForce 7500 LE");
	DEF_DEVICE("01DE", "NVIDIA Quadro FX 350");
	DEF_DEVICE("01DF", "NVIDIA GeForce 7300 GS");
	DEF_DEVICE("0211", "NVIDIA GeForce 6800");
	DEF_DEVICE("0212", "NVIDIA GeForce 6800 LE");
	DEF_DEVICE("0215", "NVIDIA GeForce 6800 GT");
	DEF_DEVICE("0218", "NVIDIA GeForce 6800 XT");
	DEF_DEVICE("0221", "NVIDIA GeForce 6200");
	DEF_DEVICE("0222", "NVIDIA GeForce 6200 A-LE");
	DEF_DEVICE("0240", "NVIDIA GeForce 6150");
	DEF_DEVICE("0241", "NVIDIA GeForce 6150 LE");
	DEF_DEVICE("0242", "NVIDIA GeForce 6100");
	DEF_DEVICE("0245", "NVIDIA Quadro NVS 210S / NVIDIA GeForce 6150LE");
	DEF_DEVICE("0290", "NVIDIA GeForce 7900 GTX");
	DEF_DEVICE("0291", "NVIDIA GeForce 7900 GT/GTO");
	DEF_DEVICE("0292", "NVIDIA GeForce 7900 GS");
	DEF_DEVICE("0293", "NVIDIA GeForce 7950 GX2");
	DEF_DEVICE("0294", "NVIDIA GeForce 7950 GX2");
	DEF_DEVICE("0295", "NVIDIA GeForce 7950 GT");
	DEF_DEVICE("029C", "NVIDIA Quadro FX 5500");
	DEF_DEVICE("029D", "NVIDIA Quadro FX 3500");
	DEF_DEVICE("029E", "NVIDIA Quadro FX 1500");
	DEF_DEVICE("029F", "NVIDIA Quadro FX 4500 X2");
	DEF_DEVICE("02E0", "NVIDIA GeForce 7600 GT");
	DEF_DEVICE("02E1", "NVIDIA GeForce 7600 GS");
	DEF_DEVICE("02E2", "NVIDIA GeForce 7300 GT");
	DEF_DEVICE("02E3", "NVIDIA GeForce 7900 GS");
	DEF_DEVICE("02E4", "NVIDIA GeForce 7950 GT");
	DEF_DEVICE("038B", "NVIDIA GeForce 7650 GS");
	DEF_DEVICE("0390", "NVIDIA GeForce 7650 GS");
	DEF_DEVICE("0391", "NVIDIA GeForce 7600 GT");
	DEF_DEVICE("0392", "NVIDIA GeForce 7600 GS");
	DEF_DEVICE("0393", "NVIDIA GeForce 7300 GT");
	DEF_DEVICE("0394", "NVIDIA GeForce 7600 LE");
	DEF_DEVICE("0395", "NVIDIA GeForce 7300 GT");
	DEF_DEVICE("039E", "NVIDIA Quadro FX 560");
	DEF_DEVICE("03D0", "NVIDIA GeForce 6150SE nForce 430");
	DEF_DEVICE("03D1", "NVIDIA GeForce 6100 nForce 405");
	DEF_DEVICE("03D2", "NVIDIA GeForce 6100 nForce 400");
	DEF_DEVICE("03D5", "NVIDIA GeForce 6100 nForce 420");
	DEF_DEVICE("03D6", "NVIDIA GeForce 7025 / NVIDIA nForce 630a");
	DEF_DEVICE("0400", "NVIDIA GeForce 8600 GTS");
	DEF_DEVICE("0401", "NVIDIA GeForce 8600 GT");
	DEF_DEVICE("0402", "NVIDIA GeForce 8600 GT");
	DEF_DEVICE("0403", "NVIDIA GeForce 8600 GS");
	DEF_DEVICE("0404", "NVIDIA GeForce 8400 GS");
	DEF_DEVICE("0406", "NVIDIA GeForce 8300 GS");
	DEF_DEVICE("040A", "NVIDIA Quadro FX 370");
	DEF_DEVICE("040E", "NVIDIA Quadro FX 570");
	DEF_DEVICE("040F", "NVIDIA Quadro FX 1700");
	DEF_DEVICE("0410", "NVIDIA GeForce GT 330");
	DEF_DEVICE("0420", "NVIDIA GeForce 8400 SE");
	DEF_DEVICE("0421", "NVIDIA GeForce 8500 GT");
	DEF_DEVICE("0422", "NVIDIA GeForce 8400 GS");
	DEF_DEVICE("0423", "NVIDIA GeForce 8300 GS");
	DEF_DEVICE("0424", "NVIDIA GeForce 8400 GS");
	DEF_DEVICE("042C", "NVIDIA GeForce 9400 GT");
	DEF_DEVICE("042F", "NVIDIA Quadro NVS 290");
	DEF_DEVICE("053A", "NVIDIA GeForce 7050 PV / NVIDIA nForce 630a");
	DEF_DEVICE("053B", "NVIDIA GeForce 7050 PV / NVIDIA nForce 630a");
	DEF_DEVICE("053E", "NVIDIA GeForce 7025 / NVIDIA nForce 630a");
	DEF_DEVICE("05E0", "NVIDIA GeForce GTX 295");
	DEF_DEVICE("05E1", "NVIDIA GeForce GTX 280");
	DEF_DEVICE("05E2", "NVIDIA GeForce GTX 260");
	DEF_DEVICE("05E3", "NVIDIA GeForce GTX 285");
	DEF_DEVICE("05E6", "NVIDIA GeForce GTX 275");
	DEF_DEVICE("05E7", "NVIDIA Tesla C1060 / T10 / M1060");
	DEF_DEVICE("05EA", "NVIDIA GeForce GTX 260");
	DEF_DEVICE("05EB", "NVIDIA GeForce GTX 295");
	DEF_DEVICE("05ED", "NVIDIA Quadroplex 2200 D2");
	DEF_DEVICE("05F8", "NVIDIA Quadroplex 2200 S4");
	DEF_DEVICE("05F9", "NVIDIA Quadro CX");
	DEF_DEVICE("05FD", "NVIDIA Quadro FX 5800");
	DEF_DEVICE("05FE", "NVIDIA Quadro FX 4800");
	DEF_DEVICE("05FF", "NVIDIA Quadro FX 3800");
	DEF_DEVICE("0600", "NVIDIA GeForce 8800 GTS 512");
	DEF_DEVICE("0601", "NVIDIA GeForce 9800 GT");
	DEF_DEVICE("0602", "NVIDIA GeForce 8800 GT");
	DEF_DEVICE("0603", "NVIDIA GeForce GT 230");
	DEF_DEVICE("0604", "NVIDIA GeForce 9800 GX2");
	DEF_DEVICE("0605", "NVIDIA GeForce 9800 GT");
	DEF_DEVICE("0606", "NVIDIA GeForce 8800 GS");
	DEF_DEVICE("0607", "NVIDIA GeForce GTS 240");
	DEF_DEVICE("060D", "NVIDIA GeForce 8800 GS");
	DEF_DEVICE("0610", "NVIDIA GeForce 9600 GSO");
	DEF_DEVICE("0611", "NVIDIA GeForce 8800 GT");
	DEF_DEVICE("0612", "NVIDIA GeForce 9800 GTX/9800 GTX+");
	DEF_DEVICE("0613", "NVIDIA GeForce 9800 GTX+");
	DEF_DEVICE("0614", "NVIDIA GeForce 9800 GT");
	DEF_DEVICE("0615", "NVIDIA GeForce GTS 250");
	DEF_DEVICE("0619", "NVIDIA Quadro FX 4700 X2");
	DEF_DEVICE("061A", "NVIDIA Quadro FX 3700");
	DEF_DEVICE("061B", "NVIDIA Quadro VX 200");
	DEF_DEVICE("0622", "NVIDIA GeForce 9600 GT");
	DEF_DEVICE("0623", "NVIDIA GeForce 9600 GS");
	DEF_DEVICE("0625", "NVIDIA GeForce 9600 GSO 512");
	DEF_DEVICE("0626", "NVIDIA GeForce GT 130");
	DEF_DEVICE("0627", "NVIDIA GeForce GT 140");
	DEF_DEVICE("062D", "NVIDIA GeForce 9600 GT");
	DEF_DEVICE("062E", "NVIDIA GeForce 9600 GT");
	DEF_DEVICE("0635", "NVIDIA GeForce 9600 GSO");
	DEF_DEVICE("0637", "NVIDIA GeForce 9600 GT");
	DEF_DEVICE("0638", "NVIDIA Quadro FX 1800");
	DEF_DEVICE("0640", "NVIDIA GeForce 9500 GT");
	DEF_DEVICE("0641", "NVIDIA GeForce 9400 GT");
	DEF_DEVICE("0643", "NVIDIA GeForce 9500 GT");
	DEF_DEVICE("0644", "NVIDIA GeForce 9500 GS");
	DEF_DEVICE("0645", "NVIDIA GeForce 9500 GS");
	DEF_DEVICE("0646", "NVIDIA GeForce GT 120");
	DEF_DEVICE("0658", "NVIDIA Quadro FX 380");
	DEF_DEVICE("0659", "NVIDIA Quadro FX 580");
	DEF_DEVICE("065B", "NVIDIA GeForce 9400 GT");
	DEF_DEVICE("065F", "NVIDIA GeForce G210");
	DEF_DEVICE("06E0", "NVIDIA GeForce 9300 GE");
	DEF_DEVICE("06E1", "NVIDIA GeForce 9300 GS");
	DEF_DEVICE("06E2", "NVIDIA GeForce 8400");
	DEF_DEVICE("06E3", "NVIDIA GeForce 8400 SE");
	DEF_DEVICE("06E4", "NVIDIA GeForce 8400 GS");
	DEF_DEVICE("06E6", "NVIDIA GeForce G100");
	DEF_DEVICE("06E7", "NVIDIA GeForce 9300 SE");
	DEF_DEVICE("06E9", "NVIDIA GeForce 9300M GS");
	DEF_DEVICE("06F8", "NVIDIA Quadro NVS 420");
	DEF_DEVICE("06F9", "NVIDIA Quadro FX 370 LP");
	DEF_DEVICE("06FA", "NVIDIA Quadro NVS 450");
	DEF_DEVICE("06FD", "NVIDIA Quadro NVS 295");
	DEF_DEVICE("06FF", "NVIDIA HICx8/16 + Graphics");
	DEF_DEVICE("07E0", "NVIDIA GeForce 7150 / NVIDIA nForce 630i");
	DEF_DEVICE("07E1", "NVIDIA GeForce 7100 / NVIDIA nForce 630i");
	DEF_DEVICE("07E2", "NVIDIA GeForce 7050 / NVIDIA nForce 630i");
	DEF_DEVICE("07E3", "NVIDIA GeForce 7050 / NVIDIA nForce 610i");
	DEF_DEVICE("07E5", "NVIDIA GeForce 7050 / NVIDIA nForce 620i");
	DEF_DEVICE("0846", "NVIDIA GeForce 9200");
	DEF_DEVICE("0847", "NVIDIA GeForce 9100");
	DEF_DEVICE("0848", "NVIDIA GeForce 8300");
	DEF_DEVICE("0849", "NVIDIA GeForce 8200");
	DEF_DEVICE("084A", "NVIDIA nForce 730a");
	DEF_DEVICE("084B", "NVIDIA GeForce 9200");
	DEF_DEVICE("084C", "NVIDIA nForce 980a/780a SLI");
	DEF_DEVICE("084D", "NVIDIA nForce 750a SLI");
	DEF_DEVICE("084F", "NVIDIA GeForce 8100 / nForce 720a");
	DEF_DEVICE("0860", "NVIDIA GeForce 9400");
	DEF_DEVICE("0861", "NVIDIA GeForce 9400");
	DEF_DEVICE("0864", "NVIDIA GeForce 9300");
	DEF_DEVICE("0865", "NVIDIA ION");
	DEF_DEVICE("0868", "NVIDIA nForce 760i SLI");
	DEF_DEVICE("086A", "NVIDIA GeForce 9400");
	DEF_DEVICE("086C", "NVIDIA GeForce 9300 / nForce 730i");
	DEF_DEVICE("086D", "NVIDIA GeForce 9200");
	DEF_DEVICE("0871", "NVIDIA GeForce 9200");
	DEF_DEVICE("0874", "NVIDIA ION");
	DEF_DEVICE("0876", "NVIDIA ION");
	DEF_DEVICE("087A", "NVIDIA GeForce 9400");
	DEF_DEVICE("087D", "NVIDIA ION");
	DEF_DEVICE("087E", "NVIDIA ION LE");
	DEF_DEVICE("087F", "NVIDIA ION LE");
	DEF_DEVICE("0A20", "NVIDIA GeForce GT 220");
	DEF_DEVICE("0A23", "NVIDIA GeForce 210");
	DEF_DEVICE("0A60", "NVIDIA GeForce G210");
	DEF_DEVICE("0A62", "NVIDIA GeForce 205");
	DEF_DEVICE("0A64", "NVIDIA ION");
	DEF_DEVICE("0A65", "NVIDIA GeForce 210");
	DEF_DEVICE("0A78", "NVIDIA Quadro FX 380 LP");
	DEF_DEVICE("0CA0", "NVIDIA GeForce GT 330");
	DEF_DEVICE("0CA2", "NVIDIA GeForce GT 320");
	DEF_DEVICE("0CA3", "NVIDIA GeForce GT 240");
	DEF_DEVICE("0CA4", "NVIDIA GeForce GT 340");
	DEF_DEVICE("0CA7", "NVIDIA GeForce GT 330");
}

void CGPUInfo::InitDeviceList()
{
	InitATIDeviceList();
	InitNVidiaDeviceList();
	InitIntelDeviceList();

	// S3 Graphics
	DEF_DEVICE("8E04", "DeltaChromeS4");
	DEF_DEVICE("8E00", "DeltaChromeS8");
	DEF_DEVICE("8E01", "DeltaChromeX9m");
	DEF_DEVICE("8E02", "DeltaChromeX9m ULP");
	DEF_DEVICE("8E03", "DeltaChromeM");
	DEF_DEVICE("8E13", "GammaChromeXM18 ULP MPM64");
	DEF_DEVICE("8E12", "GammaChromeXM18");
	DEF_DEVICE("8E10", "GammaChromeS18");
	DEF_DEVICE("8E40", "Chrome S23");
	DEF_DEVICE("8E48", "Chrome S27/S27M/S25");
	DEF_DEVICE("8E41", "Chrome XM23");
	DEF_DEVICE("8E4F", "Chrome XM25 ULP MPM-128");
	DEF_DEVICE("8E4C", "Chrome XM25 ULP MPM-64");
	DEF_DEVICE("8E4A", "Chrome XM27");
	DEF_DEVICE("9043", "CHROME 430 GS/GT");
	DEF_DEVICE("9045", "CHROME 440 GTX");

	// SiS
	// ??

	// Non NPOT Signatures
	DEF_NON_NPOT_SIGNATURE("X300");
	DEF_NON_NPOT_SIGNATURE("X550");
	DEF_NON_NPOT_SIGNATURE("X600");
	DEF_NON_NPOT_SIGNATURE("X700");
	DEF_NON_NPOT_SIGNATURE("X800");
	DEF_NON_NPOT_SIGNATURE("X850");
	DEF_NON_NPOT_SIGNATURE("X1050");
	DEF_NON_NPOT_SIGNATURE("9500");
	DEF_NON_NPOT_SIGNATURE("9550");
	DEF_NON_NPOT_SIGNATURE("9600");
	DEF_NON_NPOT_SIGNATURE("9700");
	DEF_NON_NPOT_SIGNATURE("9800");
	DEF_NON_NPOT_SIGNATURE("Xpress");
	DEF_NON_NPOT_SIGNATURE("X1200");
	DEF_NON_NPOT_SIGNATURE("X1300");
	DEF_NON_NPOT_SIGNATURE("X1350");
	DEF_NON_NPOT_SIGNATURE("X1400");
	DEF_NON_NPOT_SIGNATURE("X1450");
	DEF_NON_NPOT_SIGNATURE("X1550");
	DEF_NON_NPOT_SIGNATURE("X1600");
	DEF_NON_NPOT_SIGNATURE("X1650");
	DEF_NON_NPOT_SIGNATURE("X1700");
	DEF_NON_NPOT_SIGNATURE("X1800");
	DEF_NON_NPOT_SIGNATURE("X1900");
	DEF_NON_NPOT_SIGNATURE("X1950");
}

void CGPUInfo::GetVirtualBoxDriverVersion(int& majorVersion, int& minorVersion)
{
	std::string renderer = GetRendererGLName();
	int pos = renderer.find("Chromium");
	if (pos != -1)
	{
		int pos1 = renderer.find_first_of(" ", pos + 1);
		int pos2 = renderer.find_first_of(".", pos + 1);
		majorVersion = rtl_atoi(renderer.c_str() + pos1 + 1);
		minorVersion = rtl_atoi(renderer.c_str() + pos2 + 1);
	}
	else
	{
		majorVersion = 0;
		minorVersion = 0;
	}
}

bool CGPUInfo::DisableNVidiaDriverMultithreading()
{
	NvAPI_Status status;
    
	status = NvAPI_Initialize();
    if (status != NVAPI_OK) 
	{
        return false;
	}
    
    NvDRSSessionHandle hSession = 0;
    status = NvAPI_DRS_CreateSession(&hSession);
    if (status != NVAPI_OK) 
	{
        return false;
	}
    
    status = NvAPI_DRS_LoadSettings(hSession);
    if (status != NVAPI_OK) 
	{
        return false;
	}
    
    NvDRSProfileHandle hProfile = 0;
    status = NvAPI_DRS_GetBaseProfile(hSession, &hProfile);
    if (status != NVAPI_OK) 
	{
        return false;
	}
    
    NVDRS_SETTING drsSetting = {0};
    drsSetting.version = NVDRS_SETTING_VER;
    drsSetting.settingId = OGL_THREAD_CONTROL_ID;
    drsSetting.settingType = NVDRS_DWORD_TYPE;
    drsSetting.u32CurrentValue = OGL_THREAD_CONTROL_DISABLE;

    status = NvAPI_DRS_SetSetting(hSession, hProfile, &drsSetting);
    if (status != NVAPI_OK) 
	{
        return false;
	}
    
    status = NvAPI_DRS_SaveSettings(hSession);
    if (status != NVAPI_OK) 
	{
        return false;
	}
    
    NvAPI_DRS_DestroySession(hSession);
    hSession = 0;
	return true;
}

void CGPUInfo::GetNVidiaDriverVersion(int& majorVersion, int& minorVersion)
{
	bool isFound = false;
	majorVersion = 81;
	minorVersion = 0;

	NV_DISPLAY_DRIVER_VERSION driverVersion;
	driverVersion.version = NV_DISPLAY_DRIVER_VERSION_VER;

	if (NvAPI_Initialize() == NVAPI_OK)
	{
		g->lw.WriteLn("NVAPI init ok");
		NvAPI_Status nvapiStatus = NVAPI_OK;
		NvDisplayHandle hDisplay_a[NVAPI_MAX_PHYSICAL_GPUS * 2] = {0};

		for (int i = 0; nvapiStatus == NVAPI_OK; i++)
		{
			nvapiStatus = NvAPI_EnumNvidiaDisplayHandle(i, &hDisplay_a[i]);

			if (nvapiStatus == NVAPI_OK)
			{
				g->lw.WriteLn("NVAPI enum display ok");
				if (NvAPI_GetDisplayDriverVersion(hDisplay_a[i], &driverVersion) == NVAPI_OK)
				{
					g->lw.WriteLn("NVAPI get driver version ok");
					majorVersion = (int)(driverVersion.drvVersion / 100);
					minorVersion = driverVersion.drvVersion%100;
					isFound = true;
					break;
				}
			}
		}
	}

	// NVidia Optimus hack
	if ((!isFound) || (majorVersion < 10))
	{
		if (NvAPI_GetDisplayDriverVersion(NULL, &driverVersion) == NVAPI_OK)
		{
			g->lw.WriteLn("NVAPI get driver version (2) ok");
			majorVersion = (int)(driverVersion.drvVersion / 100);
			minorVersion = driverVersion.drvVersion%100;
		}
		else
		{
			g->lw.WriteLn("NVAPI get driver version (2) failed");
		}
	}
}

bool CGPUInfo::IsGetTexImageBroken()
{
	if (g->gi.GetVendorID() == VENDOR_INTEL)
	{
		unsigned int majorV, minorV, build;
		g->ci.GetOSVersion(&majorV, &minorV, &build);
		if ((majorV == 6) && (minorV <= 1))
		{
			return true;
		}
	}
	
	return false;
}

bool CGPUInfo::IsNPOTSupport()
{
	if (-1 == m_isSupportNPOT)
	{
		m_isSupportNPOT = 1;

		if (!g->ei.IsExtensionSupported(GLEXT_arb_texture_non_power_of_two))
		{
			m_isSupportNPOT = 0;
		}

		if (VENDOR_ATI == GetVendorID())
		{
			std::string device = GetRendererGLName();
			device = StringToUpper(device);

			MP_VECTOR<MP_STRING>::iterator it = m_nonNPOTDevices.begin();
			MP_VECTOR<MP_STRING>::iterator itEnd = m_nonNPOTDevices.end();

			for ( ; it != itEnd; it++)
			if (device.find(*it) != -1)
			{
				m_isSupportNPOT = 0;
			}
		}
	}


	return (1 == m_isSupportNPOT);
}

unsigned int CGPUInfo::GetVideoMemorySize()
{
	if (GetVendorID() == VENDOR_NVIDIA)
	{
		unsigned int i[5] = { 0, 0, 0x27, 0, 0 };
		unsigned int o[5] = { 0, 0, 0, 0, 0 };

		HDC hdc = CreateDC("DISPLAY", 0, 0, 0);

		if (hdc == NULL) 
		{
			return 0xFFFFFFFF;
		}

		int s = ExtEscape(hdc, 0x7032, 0x14, (LPCSTR)i, 0x14, (LPSTR)o);

		DeleteDC(hdc);

		if (s <= 0) 
		{
			return 0xFFFFFFFF;
		}

		return o[3] * 1048576;
	}
	else if (GetVendorID() == VENDOR_ATI)
	{
		if (g->ei.IsExtensionSupported(GLEXT_ati_mem_info))
		{
			unsigned int m[4], m2[4], m3[4];
			GLFUNC(glGetIntegerv)(0x87FB /*VBO_FREE_MEMORY_ATI*/, (GLint*)&m[0]);
			GLFUNC(glGetIntegerv)(0x87FC /*TEXTURE_FREE_MEMORY_ATI*/, (GLint*)&m2[0]);
			GLFUNC(glGetIntegerv)(0x87FD /*RENDERBUFFER_FREE_MEMORY_ATI*/, (GLint*)&m3[0]);

			unsigned int memoryAmount = m[0];
			if (m2[0] > memoryAmount)
			{
				memoryAmount = m2[0];
			}
			if (m3[0] > memoryAmount)
			{
				memoryAmount = m3[0];
			}

			return memoryAmount;
		}
		else
		{
			return 0xFFFFFFFF;
		}
	}

	return 0xFFFFFFFF;
}

CVideoMemory* CGPUInfo::GetVideoMemory()
{
	return &m_videoMemory;
}

bool CGPUInfo::IsSaveCompressionSupported()
{
	if (g->gi.GetVendorID() == VENDOR_SIS)
	{
		return false;
	}

	if (g->gi.GetVendorID() == VENDOR_S3)
	{
		return false;
	}

	return true;
}

unsigned int CGPUInfo::GetVideocardNumber(std::string& device)
{
	unsigned int  number = 0;

	unsigned int pos = 0xFFFFFFFF;
	for (unsigned int i = 0; i < device.size(); i++)
	if ((device[i] >= '0') && (device[i] <= '9'))
	{
		pos = i;
		break;
	}

	if (pos != 0xFFFFFFFF)
	{
		number = rtl_atoi(device.c_str() + pos);
	}

	return number;
}

unsigned int CGPUInfo::GetVideocardNumber()
{
	std::string device = GetRendererGLName();	
	unsigned int number = GetVideocardNumber(device);
	if (number == 0)
	{
		device = GetDeviceName();
		number = GetVideocardNumber(device);
	}

	return number;
}

bool CGPUInfo::IsION()
{
	std::string device = GetRendererGLName();		
	device = StringToLower(device);		
	if (device.find("ion") != -1)
	{
		return true;
	}

	return false;
}

unsigned char CGPUInfo::GetVideocardGeneration()
{
	unsigned int number = GetVideocardNumber();

	if (GetVendorID() == VENDOR_INTEL)
	{
		std::string device = GetRendererGLName();
		device = StringToLower(device);

		if (device.find("hd graphics") != -1)
		{
			return 1;
		}

		if (device.find("pineview") != -1)
		{
			return 1;
		}

		if (device.find("bear lake") != -1)
		{
			return 1;
		}

		if (device.find("g41") != -1)
		{
			return 1;
		}

		if (device.find("cantiga") != -1)
		{
			return 1;
		}

		SOpenGLVersion ver = GetOpenGLVersionNumber();
		if (ver.major >= 2)
		{
			return 1;
		}

		if ((number > 700) && (number < 950))
		{
			return 0;
		}

		if (number >= 950)
		{
			return 1;
		}

		if (device.find("4 series express") != -1)
		{
			return 1;
		}

		return 0;
	}

	if (GetVendorID() == VENDOR_NVIDIA)
	{
		if ((number >= 5000) && (number < 6000))
		{
			return 1;
		}

		if ((number >= 6000) && (number < 9999))
		{
			return 2;
		}

		if ((number >= 100) && (number < 1000))
		{
			return 2;
		}

		if (IsION())
		{			
			return 2;
		}

		std::string device = GetRendererGLName();
		device = StringToLower(device);
		if ((device.find("quadro") != -1) || (device.find("nvs") != -1))
		{
			return 2;
		}

		return 0;
	}
	else if (GetVendorID() == VENDOR_ATI)
	{
		std::string device = GetRendererGLName();
		device = StringToLower(device);

		if (device.find("hd") != -1)
		{
			return 2;
		}

		if (device.find("r5") != -1)
		{
			return 2;
		}

		if (device.find("r7") != -1)
		{
			return 2;
		}

		if (device.find("r9") != -1)
		{
			return 2;
		}

		if ((number >= 8500) && (number <= 9800))
		{
			return 1;
		}
		
		if ((number >= 1) && (number < 2000))
		{
			return 1;
		}

		if ((number >= 2000) && (number < 8000))
		{
			return 2;
		}

		return 0;
	}

	return 0;
}

bool CGPUInfo::IsLowEndVideocard()
{	
	if (GetVendorID() == VENDOR_INTEL)
	{
		return true;
	}

	if (GetVendorID() == VENDOR_SIS)
	{
		return true;
	}

	unsigned int number = GetVideocardNumber();

	if (GetVendorID() == VENDOR_NVIDIA)
	{
		// GeForce FX, 6xxx
		if ((number >= 5000) && (number < 7000))
		{
			return true;
		}

		// GeForce 7xxx: <= 7400
		if ((number >= 7000) && (number <= 7400))
		{
			return true;
		}

		// GeForce 8xxx: < 8400
		if ((number >= 8000) && (number < 8400))
		{
			return true;
		}

		// GeForce 9xxx: < 9300
		if ((number >= 9000) && (number < 9300))
		{
			return true;
		}

		// GeForce GTX: <= 210, <= 310, < 410, < 510
		if (((number >= 200) && (number <= 205)) ||
			((number >= 300) && (number <= 305)) ||
			((number >= 400) && (number < 405)) ||
			((number >= 500) && (number < 505)))
		{
			return true;
		}		
	}
	else if (GetVendorID() == VENDOR_ATI)
	{
		// Radeon 8500-9800
		if ((number >= 8500) && (number <= 9800))
		{
			return true;
		}

		// Radeon X < 1800
		if ((number >= 1) && (number < 1800))
		{
			return true;
		}

		// Radeon HD 2xxx: < 2600
		if ((number >= 2000) && (number < 2600))
		{
			return true;
		}

		// Radeon HD 3xxx: < 3400
		if ((number >= 3000) && (number < 3300))
		{
			return true;
		}

		// Radeon HD 4xxx: < 4400
		if ((number >= 4000) && (number < 4300))
		{
			return true;
		}

		// Radeon HD 5xxx: < 5300
		if ((number >= 5000) && (number < 5300))
		{
			return true;
		}

		// Radeon HD 6xxx: < 6300
		if ((number >= 6000) && (number < 6200))
		{
			return true;
		}

		// Radeon HD 7xxx: < 6300
		if ((number >= 7000) && (number < 7200))
		{
			return true;
		}
	}
	else
	{
		// because of unknown vendor
		return true;
	}
	
	return false;
}

CGPUInfo::~CGPUInfo()
{
}
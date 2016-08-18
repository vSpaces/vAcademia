
#pragma once

#include <string>
#include <vector>
#include <map>

#define VENDOR_NVIDIA				1
#define VENDOR_ATI					2
#define VENDOR_INTEL				3
#define VENDOR_S3					4
#define VENDOR_SIS					5
#define VENDOR_OTHER				6

typedef struct _SOpenGLVersion
{
	unsigned char minor;
	unsigned char major;

	_SOpenGLVersion()
	{
		major = 1;
		minor = 0;
	}

	_SOpenGLVersion(unsigned char _major, unsigned char _minor)
	{
		major = _major;
		minor = _minor;
	}

	_SOpenGLVersion(const _SOpenGLVersion& other)
	{
		major = other.major;
		minor = other.minor;
	}
} SOpenGLVersion;

class CGPUInfo
{
public:
	CGPUInfo();
	~CGPUInfo();

	bool Analyze();

	SOpenGLVersion GetOpenGLVersionNumber();

	std::string GetVendorStringID();
	std::string GetVendorName();

	std::string GetVendorGLName();
	std::string GetRendererGLName();
	std::string GetGLVersion();

	std::string GetDeviceStringID();
	std::string GetDeviceName();
	std::string GetDeviceStringID(std::string& deviceName);

	int GetVendorID();
	int GetVendorID(std::string& vendorName);

	int GetMaxAnisotropy();

	bool IsNPOTSupport();
	bool IsGetTexImageBroken();
	bool IsSaveCompressionSupported();

	unsigned int GetVideoMemorySize();

	void GetDriverVersion(int& leadVersion, int& majorVersion, int& minorVersion);
	int GetAtiBuildNumber();

	bool IsLowEndVideocard();

	unsigned char GetVideocardGeneration();

	bool IsOfficialVersion();

private:
	void InitVendorList();
	void InitDeviceList();

	void InitATIDeviceList();
	void InitIntelDeviceList();
	void InitNVidiaDeviceList();
	void InitAtiDriverVersionByBuildList();

	void GetAtiDriverVersion(int& majorVersion, int& minorVersion);
	void GetNVidiaDriverVersion(int& majorVersion, int& minorVersion);
	void GetIntelDriverVersion(int& leadVersion, int& majorVersion, int& minorVersion);

	bool FindRegistryKeyValue(std::string path, std::string key, std::string& val);
	
	unsigned int GetVideocardNumber();

	bool m_isAnalyzed;
	int m_isSupportNPOT;

	std::map<std::string, std::string> m_vendorList;
	std::map<std::string, int> m_vendorListID;
	std::map<std::string, std::string> m_deviceList;
	std::vector<std::string> m_nonNPOTDevices;
	std::map<unsigned int, unsigned int> m_atiDriverVersionByBuild;

	std::string m_deviceStringID;
	std::string m_vendorStringID;
};
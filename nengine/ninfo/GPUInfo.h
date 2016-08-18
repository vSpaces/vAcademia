
#pragma once

#include "CommonInfoHeader.h"

#include <string>
#include <vector>
#include <map>
#include "VideoMemory.h"

#define LINK_TYPE_DIRECT_URL				0
#define LINK_TYPE_SITE_AUTODETECT			1
#define LINK_TYPE_STANDALONE_AUTODETECT		2
#define LINK_TYPE_SELECT_ON_SITE			3

#define VENDOR_NVIDIA				1
#define VENDOR_ATI					2
#define VENDOR_INTEL				3
#define VENDOR_S3					4
#define VENDOR_SIS					5
#define VENDOR_VIRTUALBOX			6
#define VENDOR_OTHER				7

#define OPTIMUS_NOT_FOUND			0
#define OPTIMUS_INTEL_USED			1
#define OPTIMUS_NVIDIA_USED			2

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

	void GetDeviceAndVendorIDs(std::string& vendorStringID, std::string& deviceStringID);

	std::string GetVendorStringID();
	std::string GetVendorName();	

	bool GetDriverLink(std::string& url, std::string& videoURL, const std::string lang, unsigned char& linkType);

	std::string GetVendorGLName();
	std::string GetRendererGLName();
	std::string GetGLVersion();
	SOpenGLVersion GetOpenGLVersionNumber();

	std::string GetDeviceStringID();
	std::string GetDeviceName();
	std::string GetDeviceStringID(std::string& deviceName);

	int GetVendorID();
	int GetVendorID(std::string& vendorName);

	int GetMaxAnisotropy();
	unsigned char GetMaxAntialiasingPower();

	bool IsNPOTSupport();
	bool IsGetTexImageBroken();
	bool IsSaveCompressionSupported();
	bool IsOfficialVersion();

	unsigned int GetVideoMemorySize();

	void GetDriverVersion(int& leadVersion, int& majorVersion, int& minorVersion);
	int GetAtiBuildNumber();

	CVideoMemory* GetVideoMemory();

	bool IsLowEndVideocard();

	unsigned char GetOptimusDeviceStatus();

	unsigned char GetVideocardGeneration();	

	bool IsION();

	bool DisableNVidiaDriverMultithreading();

private:
	std::string GetDriverVideoLink(const std::string lang);

	void InitVendorList();
	void InitDeviceList();

	void InitATIDeviceList();
	void InitIntelDeviceList();
	void InitNVidiaDeviceList();
	void InitAtiDriverVersionByBuildList();

	void GetAtiDriverVersion(int& majorVersion, int& minorVersion);
	void GetNVidiaDriverVersion(int& majorVersion, int& minorVersion);
	void GetIntelDriverVersion(int& leadVersion, int& majorVersion, int& minorVersion);
	void GetVirtualBoxDriverVersion(int& majorVersion, int& minorVersion);

	bool FindRegistryKeyValue(std::string path, std::string key, std::string& val);
	
	unsigned int GetVideocardNumber();

	unsigned int GetVideocardNumber(std::string& device);

	bool m_isAnalyzed;
	int m_isSupportNPOT;

	MP_MAP<MP_STRING, MP_STRING> m_vendorList;
	MP_MAP<MP_STRING, int> m_vendorListID;
	MP_MAP<MP_STRING, MP_STRING> m_deviceList;
	MP_VECTOR<MP_STRING> m_nonNPOTDevices;
	MP_MAP<unsigned int, unsigned int> m_atiDriverVersionByBuild;

	MP_STRING m_deviceStringID;
	MP_STRING m_vendorStringID;

	CVideoMemory m_videoMemory;

	char m_optimusStatus;
};
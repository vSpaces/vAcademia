
#pragma once

#define	FILTER_EXTENSION				L".ax"

enum DSDeviceType
{
	DS_DEVICE_TYPE_VIDEO = 0,
	DS_DEVICE_TYPE_AUDIO = 1,
};

bool IsDeviceRegistered(const std::string& videoDeviceName, DSDeviceType deviceType);
void TryToRegisterDirectShowFilterSilently(const std::string& filterName);
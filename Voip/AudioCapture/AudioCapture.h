#pragma once

#ifdef VOIPAUDIOCAPTURE_EXPORTS
#define VOIPAUDIOCAPTURE_API __declspec(dllexport)
#else
#define VOIPAUDIOCAPTURE_API __declspec(dllimport)
#endif

// Addition libs
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")

// Audio capture errors
#include "AudioCaptureErrors.h"

VOIPAUDIOCAPTURE_API const wchar_t*	GetVirtualAudioCableName();
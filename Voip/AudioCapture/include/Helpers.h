#pragma once

#include <mmsystem.h>
#include <xstring>
#include <vector>

/*
Заполняет WAVEFORMATEX 20-ю различными поддерживаемыми вариантами
*/
VOID GetWaveFormatFromIndex( INT nIndex, WAVEFORMATEX* pwfx );

/*
Заполняет поля nBlockAlign и nAvgBytesPerSec в WAVEFORMATEX
*/
VOID FillWaveFormatAlignAndBPS( WAVEFORMATEX* pwfx );

/*
Проверяет: входят ли переджанные параметры в поддерживаемые варианты WAVEFORMATEX. 
Проверка выполняется через вызов GetWaveFormatFromIndex
*/
bool IsAudioFormatEnumerated( unsigned int auSampleRate, unsigned int auChannelsCount,	unsigned int auBitsPerSample );

/*
Поиск GUID входного устройства
*/
struct DEVICE_ENUMERATION_PARAMS
{
	MP_WSTRING		inDeviceName;
	GUID			outDeviceGuid;

	DEVICE_ENUMERATION_PARAMS():
	MP_WSTRING_INIT(inDeviceName)
	{

	}

};
BOOL CALLBACK DirectSoundEnumerationRoutine(LPGUID guid, LPCWSTR desc, LPCWSTR driver, LPVOID user);


typedef std::vector<DEVICE_ENUMERATION_PARAMS> VecOutDevicesParams;
typedef VecOutDevicesParams::iterator VecOutDevicesParamsIt;
/*
Поиск всех входных устройства
*/
BOOL CALLBACK DirectSoundEnumerateAllRoutine(LPGUID guid, LPCWSTR desc, LPCWSTR driver, LPVOID user);
#pragma once
#include "CommonPipePacketIn.h"

class CWizardFindedDeviceIn : public CCommonPipePacketIn
{
	MP_WSTRING devGuid;
	MP_WSTRING devName;
	MP_WSTRING devLineName;
public:
	CWizardFindedDeviceIn(BYTE *aData, int aDataSize);

	std::wstring GetDeviceGuid();
	std::wstring GetDeviceName();
	std::wstring GetDeviceLineName();
	virtual bool Analyse();
};
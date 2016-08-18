#pragma once
#include "CommonPipePacketOut.h"

class CWizardFindedDeviceOut : public CCommonPipePacketOut
{
public:
	CWizardFindedDeviceOut(byte aID, wchar_t *apDevGuid, wchar_t *apDevName, wchar_t *apDevLineName);
};
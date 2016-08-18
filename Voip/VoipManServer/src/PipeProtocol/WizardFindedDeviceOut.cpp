#include "stdafx.h"
#include "include/PipeProtocol/WizardFindedDeviceOut.h"

CWizardFindedDeviceOut::CWizardFindedDeviceOut(byte aID, wchar_t *apDevGuid, wchar_t *apDevName, wchar_t *apDevLineName) : CCommonPipePacketOut( aID)
{
	data->Add( apDevGuid);
	data->Add( apDevName);
	data->Add( apDevLineName);
}
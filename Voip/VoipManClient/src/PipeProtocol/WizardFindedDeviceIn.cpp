#include "stdafx.h"
#include "../../include/PipeProtocol/WizardFindedDeviceIn.h"


CWizardFindedDeviceIn::CWizardFindedDeviceIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
		, MP_WSTRING_INIT(devGuid)
		, MP_WSTRING_INIT(devName)
		, MP_WSTRING_INIT(devLineName)
{
}

bool CWizardFindedDeviceIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read( devGuid))
		return false;

	if(!data->Read( devName))
		return false;

	if(!data->Read( devLineName))
		return false;

	return EndOfData();
}

std::wstring CWizardFindedDeviceIn::GetDeviceGuid()
{
	return devGuid;
}

std::wstring CWizardFindedDeviceIn::GetDeviceName()
{
	return devGuid;
}

std::wstring CWizardFindedDeviceIn::GetDeviceLineName()
{
	return devLineName;
}
#include "StdAfx.h"
#include "VistaAudioEngine.h"

#if _MSC_VER >= 1400
	#include <mmdeviceapi.h>

	#include <wtypes.h>
	#include <functiondiscoverykeys.h>

	#include "PolicyConfig.h"

	const PROPERTYKEY PKEY_Device_FriendlyName = { { 0xa45c254e, 0xdf1c, 0x4efd, { 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 } }, 14 };
#endif

// CPolicyConfig.cpp
HRESULT RegisterDevice(LPCWSTR devID);
HRESULT ActivateDeviceByGuid(LPCWSTR devID);

CVistaAudioEngine::CVistaAudioEngine(void):
	MP_VECTOR_INIT(vecAudioDevices)
{
	pLW = NULL;
}

CVistaAudioEngine::~CVistaAudioEngine(void)
{
}

bool CVistaAudioEngine::MakeExceptOneDeviceEnabled(ILogWriter* apLW)
{
	pLW = apLW;

	#if _MSC_VER >= 1400

	EnumerateAllInDevices();
	int inActiveDevicesCount = GetDeviceCountByState( DEVICE_STATE_ACTIVE);

	if( apLW != NULL)
	{
		CComString asLogText;
		asLogText.Format( "[VOIP] inActiveDevicesCount = %i\n", (int)inActiveDevicesCount);
		pLW->WriteLPCSTR( asLogText.GetBuffer());
	}

	if( inActiveDevicesCount == 0)
	{
		bool stereoMixerActivated = ActivateStereoMixer();

		if( apLW != NULL)
		{
			if( stereoMixerActivated)
				apLW->WriteLPCSTR("Stereo mixer activation SUCCESS\n");
			else
				apLW->WriteLPCSTR("Stereo mixer activation FAILED\n");
		}

		if( stereoMixerActivated)
		{
			EnumerateAllInDevices();
			inActiveDevicesCount = GetDeviceCountByState( DEVICE_STATE_ACTIVE);

			if( pLW != NULL)
			{
				CComString asLogText;
				asLogText.Format( "[VOIP] inActiveDevicesCount after mixer activated = %i\n", (int)inActiveDevicesCount);
				pLW->WriteLPCSTR( asLogText.GetBuffer());
			}

			if( inActiveDevicesCount == 0)
				stereoMixerActivated = false;
		}

		if( !stereoMixerActivated)
		{
			ActivateAnyDevice();
		}
	}

	#endif

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }
#endif
#if _MSC_VER >= 1400
const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
#endif

void CVistaAudioEngine::EnumerateAllInDevices()
{
	vecAudioDevices.clear();

#if _MSC_VER >= 1400
	HRESULT  hr = S_OK;
    IMMDeviceEnumerator  *pEnumerator = NULL;
    IMMDeviceCollection  *pCollection = NULL;
    IMMDevice  *pEndpoint = NULL;
    IPropertyStore  *pProps = NULL;
    LPWSTR pwszID = NULL;

    hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
           (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

    hr = pEnumerator->EnumAudioEndpoints(
                        eCapture, DEVICE_STATEMASK_ALL/*DEVICE_STATE_ACTIVE | DEVICE_STATE_DISABLED | DEVICE_STATE_NOTPRESENT | DEVICE_STATE_UNPLUGGED*/,
                        &pCollection);
    EXIT_ON_ERROR(hr)

    UINT  count;
    hr = pCollection->GetCount(&count);
    EXIT_ON_ERROR(hr)

    if (count == 0)
    {
		if( pLW != NULL)
			pLW->WriteLPCSTR( "[VOIP] No endpoints found.\n");
    }

    // Each loop prints the name of an endpoint device.
    for (ULONG i = 0; i < count; i++)
    {
        // Get pointer to endpoint number i.
        hr = pCollection->Item(i, &pEndpoint);
        EXIT_ON_ERROR(hr)

        // Get the endpoint ID string.
        hr = pEndpoint->GetId(&pwszID);
        EXIT_ON_ERROR(hr)

		DWORD dwState;
		hr = pEndpoint->GetState( &dwState);
		EXIT_ON_ERROR(hr)
        
        hr = pEndpoint->OpenPropertyStore(
                          STGM_READ, &pProps);
        EXIT_ON_ERROR(hr)

        PROPVARIANT varName;
        // Initialize container for property value.
        PropVariantInit(&varName);

        // Get the endpoint's friendly-name property.
        hr = pProps->GetValue( PKEY_Device_FriendlyName, &varName);
        EXIT_ON_ERROR(hr)

		CVistaAudioEngine::VISTA_AUDIO_DEVICE_DESC	deviceDesc;
		deviceDesc.name = varName.pwszVal;
		deviceDesc.guid = pwszID;
		deviceDesc.state = dwState;
		vecAudioDevices.push_back( deviceDesc);

        CoTaskMemFree(pwszID);
        pwszID = NULL;
        PropVariantClear(&varName);
        SAFE_RELEASE(pProps)
        SAFE_RELEASE(pEndpoint)
    }
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pCollection)
    return;

Exit:
    CoTaskMemFree(pwszID);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pCollection)
    SAFE_RELEASE(pEndpoint)
    SAFE_RELEASE(pProps)
#endif
}

std::wstring CVistaAudioEngine::GetDeviceNameByGUID( const wchar_t* alpwcGuid) const
{
	VecAudioDevicesItConst it = vecAudioDevices.begin(), end = vecAudioDevices.end();
	for(;it!=end;it++)
		if( it->guid.compare( alpwcGuid) == 0)
			return it->name;
	return L"";
}

std::wstring CVistaAudioEngine::GetDeviceGUIDByName( const wchar_t* alpwcName) const
{
	VecAudioDevicesItConst it = vecAudioDevices.begin(), end = vecAudioDevices.end();
	for(;it!=end;it++)
		if( it->name.compare( alpwcName) == 0)
			return it->guid;
	return L"";
}

int CVistaAudioEngine::GetDeviceCountByState( DWORD adwState)
{
	int count = 0;
	VecAudioDevicesItConst it = vecAudioDevices.begin(), end = vecAudioDevices.end();
	for(;it!=end;it++)
		if( it->state == adwState)
			count++;
	return count;
}

bool CVistaAudioEngine::ActivateStereoMixer()
{
#if _MSC_VER >= 1400
	VecAudioDevicesItConst it = vecAudioDevices.begin(), end = vecAudioDevices.end();
	for(;it!=end;it++)
	{
		CWComString wsName = it->name.c_str();
		wsName.MakeLower();

		if( wsName.Find(L"mixer")!=-1 || wsName.Find(L"микшер") !=-1)
		{
			return ActivateDeviceByGuid( it->guid.c_str(), it->name.c_str());
		}
	}
#endif
	return false;
}

bool CVistaAudioEngine::ActivateAnyDevice()
{
#if _MSC_VER >= 1400
	VecAudioDevices	vecAudioDevicesCopy = vecAudioDevices;

	VecAudioDevicesItConst it = vecAudioDevicesCopy.begin(), end = vecAudioDevicesCopy.end();
	for(;it!=end;it++)
	{
		if( ActivateDeviceByGuid( it->guid.c_str(), it->name.c_str()))
		{
			EnumerateAllInDevices();
			int inActiveDevicesCount = GetDeviceCountByState( DEVICE_STATE_ACTIVE);
			if( inActiveDevicesCount != 0)
				return true;
		}
	}
#endif
	return false;
}

bool CVistaAudioEngine::ActivateDeviceByGuid(const wchar_t* apwcID, const wchar_t* apwcName)
{
	HRESULT hr = ::ActivateDeviceByGuid( apwcID);

	if( pLW != NULL)
	{
		CComString asLogText;
		asLogText.Format( "[VOIP] ActivateDeviceByGuid. DeviceName = %s, Result is = %i\n", CComString( apwcName).GetBuffer(), hr);
		pLW->WriteLPCSTR( asLogText.GetBuffer());
	}

	return hr == S_OK ? true : false;
}
#include "StdAfx.h"
#include "../include/VistaAudioEngine.h"


#if _MSC_VER >= 1400
	#include <wtypes.h>
	#include <functiondiscoverykeys.h>

	#include "../include/PolicyConfig.h"

	const PROPERTYKEY PKEY_Device_FriendlyName = { { 0xa45c254e, 0xdf1c, 0x4efd, { 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 } }, 14 };
	const PROPERTYKEY PKEY_AudioEndpoint_FormFactor = { { 0x1da5d803, 0xd492, 0x4edd, { 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e } }, 0 };	

#include <ks.h>                // IKsControl
#include <ATLComCli.h>         // CComPtr
#include <strsafe.h>           // Safe string API's
#include <DeviceTopology.h>    // Endpoint API's
//#include <DeviceTopologyP.h>   // Endpoint API's
#include <propkey.h>           // PKEY_Device_FriendlyName

#endif

HRESULT RegisterDevice(LPCWSTR devID);	// CPolicyConfig.cpp

CVistaAudioEngine::CVistaAudioEngine(void):
	MP_VECTOR_INIT(vecAudioDevices)
	, MP_VECTOR_INIT(vecCaptureAudioDevices)
{
	EnumerateAllOutDevices();
	EnumerateAllInDevices();
}

CVistaAudioEngine::~CVistaAudioEngine(void)
{
}

/*VOIPAC_ERROR CVistaAudioEngine::GetCurrentOutAudioDeviceName( wchar_t* alpcwBuffer, unsigned int bufferSize)
{
#if _MSC_VER >= 1400
	return GetCurrentAudioDeviceNameImpl( alpcwBuffer, bufferSize, eRender);
#else
	return ACERROR_NOTIMPLEMENTED;
#endif
}*/

VOIPAC_ERROR CVistaAudioEngine::GetCurrentOutAudioDeviceGuid( wchar_t* alpcwBuffer, unsigned int bufferSize)
{
#if _MSC_VER >= 1400
	return GetCurrentAudioDeviceGuidImpl( alpcwBuffer, bufferSize, eRender);
#else
	return ACERROR_NOTIMPLEMENTED;
#endif
}

/*VOIPAC_ERROR CVistaAudioEngine::GetDefaultInDeviceName( wchar_t * alpcwBuffer, unsigned int bufferSize)
{
#if _MSC_VER >= 1400
	return GetCurrentAudioDeviceNameImpl( alpcwBuffer, bufferSize, eCapture);
#else
	return ACERROR_NOTIMPLEMENTED;
#endif
}*/

VOIPAC_ERROR CVistaAudioEngine::GetDefaultInDeviceGuid( wchar_t * alpcwBuffer, unsigned int bufferSize)
{
#if _MSC_VER >= 1400
	return GetCurrentAudioDeviceGuidImpl( alpcwBuffer, bufferSize, eCapture);
#else
	return ACERROR_NOTIMPLEMENTED;
#endif
}

VOIPAC_ERROR CVistaAudioEngine::GetFirstActiveMicrophone( wchar_t* alpcwBuffer, unsigned int bufferSize)
{
#if _MSC_VER >= 1400
	return GetFirstActiveDeviceByTypeImpl( alpcwBuffer, bufferSize, Microphone, false);
#else
	return ACERROR_NOTIMPLEMENTED;
#endif
}

VOIPAC_ERROR CVistaAudioEngine::GetFirstActiveNoMicrophone( wchar_t* alpcwBuffer, unsigned int bufferSize)
{
#if _MSC_VER >= 1400
	return GetFirstActiveDeviceByTypeImpl( alpcwBuffer, bufferSize, Microphone, true);
#else
	return ACERROR_NOTIMPLEMENTED;
#endif
}

VOIPAC_ERROR CVistaAudioEngine::GetActiveMicrophones(std::vector<std::wstring> &av)
{
#if _MSC_VER >= 1400
	return GetActiveDevicesByType( Microphone, vecCaptureAudioDevices, av);
#else
	return ACERROR_NOTIMPLEMENTED;
#endif
}

VOIPAC_ERROR CVistaAudioEngine::GetActiveInDevices(std::vector<std::wstring> &av)
{
	VecAudioDevicesItConst it = vecCaptureAudioDevices.begin(), end =  vecCaptureAudioDevices.end();
	for(;it!=end;it++)
		av.push_back( it->guid.c_str());
	return ACERROR_NOERROR;
}

VOIPAC_ERROR CVistaAudioEngine::GetActiveOutDevices(std::vector<std::wstring> &av)
{
	VecAudioDevicesItConst it = vecAudioDevices.begin(), end =  vecAudioDevices.end();
	for(;it!=end;it++)
		av.push_back( it->guid.c_str());
	return ACERROR_NOERROR;
}

VOIPAC_ERROR CVistaAudioEngine::GetActiveNoMicInDevices(std::vector<std::wstring> &av)
{
#if _MSC_VER >= 1400
	return GetActiveDevicesByNotType( Microphone, vecCaptureAudioDevices, av);
#else
	return ACERROR_NOTIMPLEMENTED;
#endif
}



#if _MSC_VER >= 1400

VOIPAC_ERROR CVistaAudioEngine::GetCurrentAudioDeviceGuidImpl( wchar_t* alpcwBuffer, unsigned int bufferSize, EDataFlow dataFlow)
{
	HRESULT hr;
	CComPtr<IMMDevice> pDefaultDevice;
	CComPtr<IMMDeviceEnumerator> pEnumerator;

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void **>(&pEnumerator));
	if( FAILED( hr))
		return ACERROR_COMFAILED;

	pEnumerator->GetDefaultAudioEndpoint( dataFlow, eMultimedia, &pDefaultDevice);
	if( !pDefaultDevice)
		return ACERROR_COMFAILED;

	wchar_t*	devGUID = NULL;
	hr = pDefaultDevice->GetId( &devGUID);
	if ( FAILED(hr))
		return ACERROR_ACCESSDENIED;

	std::wstring sGuid = devGUID;
	int pos = sGuid.rfind('{');
	if( pos > 0)
	{
		sGuid = sGuid.substr( pos);
	}	

	wcscpy_s( alpcwBuffer, bufferSize, sGuid.c_str());

	return ACERROR_NOERROR;
}

/*VOIPAC_ERROR CVistaAudioEngine::GetCurrentAudioDeviceNameImpl( wchar_t* alpcwBuffer, unsigned int bufferSize, EDataFlow dataFlow)
{
	HRESULT hr;
	CComPtr<IMMDevice> pDefaultDevice;
	CComPtr<IMMDeviceEnumerator> pEnumerator;

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void **>(&pEnumerator));
	if( FAILED( hr))
		return ACERROR_COMFAILED;

	pEnumerator->GetDefaultAudioEndpoint( dataFlow, eMultimedia, &pDefaultDevice);
	if( !pDefaultDevice)
		return ACERROR_COMFAILED;

	wchar_t*	devGUID = NULL;
	hr = pDefaultDevice->GetId( &devGUID);
	if ( FAILED(hr))
		return ACERROR_ACCESSDENIED;

	std::wstring deviceName = L"";
	
	if( dataFlow == eRender)
		deviceName = GetDeviceNameByGUID( devGUID, vecAudioDevices);
	else
		deviceName = GetDeviceNameByGUID( devGUID, vecCaptureAudioDevices);

	if( deviceName.length() > bufferSize - 1)
		return ACERROR_BUFFERTOOSMALL;

	wcscpy_s( alpcwBuffer, bufferSize, deviceName.c_str());

	return ACERROR_NOERROR;
}*/

VOIPAC_ERROR CVistaAudioEngine::GetFirstActiveDeviceByTypeImpl( wchar_t* alpcwBuffer, unsigned int bufferSize, EndpointFormFactor dataType, bool aNoType)
{
	std::wstring deviceName = L"";
	deviceName = GetGuidFirstActiveDeviceByType( dataType, vecCaptureAudioDevices, aNoType);

	if( deviceName.length() > bufferSize - 1)
		return ACERROR_BUFFERTOOSMALL;

	if( deviceName.size() == 0)
		return ACERROR_INDEVICENOTFOUND;
	wcscpy_s( alpcwBuffer, bufferSize, deviceName.c_str());

	return ACERROR_NOERROR;
}
#endif

VOIPAC_ERROR CVistaAudioEngine::SetCurrentOutAudioDeviceGuid( const wchar_t* alpcwDeviceName)
{
	if( FAILED( RegisterDevice( alpcwDeviceName)))
		return ACERROR_ACCESSDENIED;

	return ACERROR_NOERROR;
}

/*VOIPAC_ERROR CVistaAudioEngine::SetCurrentOutAudioDeviceName( const wchar_t* alpcwDeviceName)
{
	if( FAILED( RegisterDevice( GetDeviceGUIDByName( alpcwDeviceName, vecAudioDevices).c_str())))
		return ACERROR_ACCESSDENIED;

	return ACERROR_NOERROR;
}*/

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
#if _MSC_VER >= 1400
	vecCaptureAudioDevices.clear();
	return EnumerateAllDevicesImpl( eCapture, vecCaptureAudioDevices);
#endif
}

void CVistaAudioEngine::EnumerateAllOutDevices()
{
#if _MSC_VER >= 1400
	vecAudioDevices.clear();
	return EnumerateAllDevicesImpl( eRender, vecAudioDevices);
#endif
}

#if _MSC_VER >= 1400

void CVistaAudioEngine::EnumerateAllDevicesImpl( EDataFlow dataFlow, VecAudioDevices& aDevices)
{
	CoInitialize(NULL);
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
                        dataFlow, DEVICE_STATE_ACTIVE,
                        &pCollection);
    EXIT_ON_ERROR(hr)

    UINT  count;
    hr = pCollection->GetCount(&count);
    EXIT_ON_ERROR(hr)

    if (count == 0)
    {
        printf("No endpoints found.\n");
    }

	//AUDIO_DEVICE_INFO info = {0};	

    // Each loop prints the name of an endpoint device.
    for (ULONG i = 0; i < count; i++)
    {
		//::ZeroMemory(&info, sizeof(info));
        // Get pointer to endpoint number i.
        hr = pCollection->Item(i, &pEndpoint);
        EXIT_ON_ERROR(hr)

		/*bool isMicrophoneArray = false;
		HRESULT hResult = EndpointIsMic(pEndpoint, isMicrophoneArray);*/

        // Get the endpoint ID string.
        hr = pEndpoint->GetId(&pwszID);
		std::wstring sGuid = pwszID;
		int pos = sGuid.rfind('{');
		if( pos > 0)
		{
			sGuid = sGuid.substr( pos);
		}
        EXIT_ON_ERROR(hr)
        
        hr = pEndpoint->OpenPropertyStore(
                          STGM_READ, &pProps);
        EXIT_ON_ERROR(hr)

        PROPVARIANT varName;
        // Initialize container for property value.
        PropVariantInit(&varName);

        // Get the endpoint's friendly-name property.
        hr = pProps->GetValue(
                       PKEY_Device_FriendlyName, &varName);
        EXIT_ON_ERROR(hr)
//PKEY_Device_ContainerId	

		PROPVARIANT varFormFactor;
		// Initialize container for property value.
		PropVariantInit(&varFormFactor);
		// Get the endpoint's AudioEndpoint_FormFactor property.
		hr = pProps->GetValue(
					   PKEY_AudioEndpoint_FormFactor, &varFormFactor);
		unsigned int typeDevice = 0;
		if( hr == S_OK)
		{
			typeDevice = varFormFactor.uintVal;
		}

		VISTA_AUDIO_DEVICE_DESC	deviceDesc;
		deviceDesc.name = varName.pwszVal;
		//deviceDesc.guid = sGuid.GetBuffer();//pwszID;
		deviceDesc.guid = sGuid.c_str();//pwszID;
		deviceDesc.type = typeDevice;
		aDevices.push_back( deviceDesc);

        CoTaskMemFree(pwszID);
        pwszID = NULL;
        PropVariantClear(&varName);
		PropVariantClear(&varFormFactor);
        SAFE_RELEASE(pProps)
        SAFE_RELEASE(pEndpoint)
    }
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pCollection)
	CoUninitialize();
    return;

Exit:
    CoTaskMemFree(pwszID);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pCollection)
    SAFE_RELEASE(pEndpoint)
    SAFE_RELEASE(pProps)
	CoUninitialize();
}
#endif

std::wstring CVistaAudioEngine::GetDeviceNameByGUID( const wchar_t* alpwcGuid, const VecAudioDevices& aDevices) const
{
	VecAudioDevicesItConst it = aDevices.begin(), end = aDevices.end();
	for(;it!=end;it++)
		if( it->guid.compare( alpwcGuid) == 0)
			return it->name;
	return L"";
}

std::wstring CVistaAudioEngine::GetDeviceGUIDByName( const wchar_t* alpwcName, const VecAudioDevices& aDevices) const
{
	VecAudioDevicesItConst it = aDevices.begin(), end = aDevices.end();
	for(;it!=end;it++)
		if( it->name.compare( alpwcName) == 0)
			return it->guid;
	return L"";
}

#if _MSC_VER >= 1400
std::wstring CVistaAudioEngine::GetGuidFirstActiveDeviceByType( EndpointFormFactor aType, const VecAudioDevices& aDevices, bool aNoType) const
{
	VecAudioDevicesItConst it = aDevices.begin(), end = aDevices.end();
	for(;it!=end;it++)
		if( !aNoType && it->type == aType || aNoType && it->type != aType)
			return it->guid;
	return L"";
}

/*std::wstring CVistaAudioEngine::GetNameFirstActiveDeviceByType( EndpointFormFactor aType, const VecAudioDevices& aDevices, bool aNoType) const
{
	VecAudioDevicesItConst it = aDevices.begin(), end = aDevices.end();
	for(;it!=end;it++)
		if( !aNoType && it->type == aType || aNoType && it->type != aType)
			return it->guid;
	return L"";
}*/

VOIPAC_ERROR CVistaAudioEngine::GetActiveDevicesByType( EndpointFormFactor aType, const VecAudioDevices& aDevices, std::vector<std::wstring> &av) const
{
	VecAudioDevicesItConst it = aDevices.begin(), end = aDevices.end();
	for(;it!=end;it++)
		if( it->type == aType)
		{
			av.push_back( it->guid.c_str());
		}
	return ACERROR_NOERROR;
}

VOIPAC_ERROR CVistaAudioEngine::GetActiveDevicesByNotType( EndpointFormFactor aType, const VecAudioDevices& aDevices, std::vector<std::wstring> &av) const
{
	VecAudioDevicesItConst it = aDevices.begin(), end = aDevices.end();
	for(;it!=end;it++)
		if( it->type != aType)
		{
			av.push_back( it->guid.c_str());
		}
		return ACERROR_NOERROR;
}
#endif

int CVistaAudioEngine::GetCountInDevices()
{
	return vecCaptureAudioDevices.size();
}

int CVistaAudioEngine::GetCountOutDevices()
{
	return vecAudioDevices.size();
}

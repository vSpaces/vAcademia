#include "StdAfx.h"
#include ".\DSMixer2.h"
#include ".\Mixer.h"
#include <mmsystem.h>

#include "strmif.h"
#include "uuids.h"
#include "atlcomcli.h"
#include <vector>
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDSMixer2::VecDeviceLines	CDSMixer2::GetDeviceLinesImpl(const std::wstring& awsDeviceName, BOOL abInDevices) const
{
	CDSMixer2::VecDeviceLines		deviceLines;
	CComQIPtr<IBaseFilter>			devicePtr;

	if ( abInDevices == TRUE)
		devicePtr = GetDeviceVolumeControlImpl( awsDeviceName, abInDevices);
	else 
		devicePtr = GetOutDeviceVolumeControlImpl( awsDeviceName, abInDevices);

	if( devicePtr == NULL)
		return deviceLines;

	CComQIPtr<IBaseFilter>	filterPtr = devicePtr;

	// Now add the filter to the graph. 
	if( filterPtr != NULL)
	{
		IEnumPins*	EnumPins = NULL;
		HRESULT hr = filterPtr->EnumPins(&EnumPins);
		ATLASSERT( SUCCEEDED( hr));
		EnumPins->Reset();
		IPin*		InPin = NULL;	// renderer input
		ULONG		fetched;
		while (EnumPins->Next(1, &InPin, &fetched) == S_OK)
		{
			PIN_INFO	pinfo;
			hr = InPin->QueryPinInfo(&pinfo);
			ATLASSERT( SUCCEEDED( hr));
			pinfo.pFilter->Release();
			InPin->Release();

			if( abInDevices)
			{
				if (pinfo.achName && pinfo.dir == PINDIR_INPUT)
				{
					deviceLines.push_back( CDSMixer2::DeviceLineName(pinfo.achName));
				}
			}
			else
			{
				if (pinfo.achName && pinfo.dir == PINDIR_OUTPUT)
				{
					deviceLines.push_back( CDSMixer2::DeviceLineName(pinfo.achName));
				}
			}
		}
		EnumPins->Release();
	}
	return deviceLines;
}

CDSMixer2::VecDeviceLines	CDSMixer2::GetDevicesImpl(BOOL abInDevices) const
{
	HRESULT hr;
	CDSMixer2::VecDeviceLines	devices;
	ICreateDevEnum *pSysDevEnum = NULL;
	IFileSinkFilter *pSink= NULL;	

	CoInitialize( NULL);

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr))
	{
		return devices;
	}
	// Obtain a class enumerator for the video compressor category.
	IEnumMoniker *pEnumCat = NULL;
	CLSID enumCLSID = CLSID_AudioInputDeviceCategory;
	if( !abInDevices)
		enumCLSID = CLSID_AudioRendererCategory;
	hr = pSysDevEnum->CreateClassEnumerator(enumCLSID, &pEnumCat, 0);

	if (hr == S_OK) 
	{
		/*
		В этой функции повисло: Краш 312 http://178.63.80.136/php/show_error_statistics.php
		*/
		// Enumerate the monikers.
		int iCounter = 0;
		int iMaxCounterValue = 32;
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK && iCounter < iMaxCounterValue)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr))
				{
					IBaseFilter *pFilter;
					hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);
					// Now add the filter to the graph. 
					if( pFilter)
					{
						devices.push_back( DeviceLineName(varName.bstrVal));
					}
					pFilter->Release();
				}
				VariantClear(&varName);
				//Remember to release pFilter later.
				pPropBag->Release();
			}
			pMoniker->Release();
			iCounter++;
		}
		pEnumCat->Release();
	}
	pSysDevEnum->Release();
	CoUninitialize();
	return devices;
}

CComQIPtr<IAMAudioInputMixer>	CDSMixer2::GetDeviceVolumeControlImpl(const std::wstring& awsDeviceName, BOOL abInDevice) const
{
	HRESULT hr;
	CComQIPtr<IAMAudioInputMixer>	nullRet;
	ICreateDevEnum *pSysDevEnum = NULL;
	IFileSinkFilter *pSink= NULL;

	if( awsDeviceName.empty())
		return nullRet;

	CoInitialize( NULL);

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr))
	{
		return nullRet;
	}

	// Obtain a class enumerator for the video compressor category.
	IEnumMoniker *pEnumCat = NULL;
	GUID enumGUID = CLSID_AudioInputDeviceCategory;
	if( !abInDevice)
		enumGUID = CLSID_AudioRendererCategory;
	hr = pSysDevEnum->CreateClassEnumerator(enumGUID, &pEnumCat, 0);


	if (hr == S_OK) 
	{
		// Enumerate the monikers.
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr))
				{
					// Display the name in your UI somehow.
					if( awsDeviceName.compare(varName.bstrVal) == 0)
					//if( awsDeviceName.find(varName.bstrVal) != std::wstring::npos)
					{
						// To create an instance of the filter, do the following:
						IBaseFilter *pFilter;
						hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);
						// Now add the filter to the graph. 
						if( pFilter)
						{
							CComQIPtr<IAMAudioInputMixer>	ret = pFilter;

							VariantClear(&varName);
							pFilter->Release();
							pPropBag->Release();
							pMoniker->Release();
							pEnumCat->Release();
							pSysDevEnum->Release();
							return ret;
						}
					}
				}
				VariantClear(&varName);
				//Remember to release pFilter later.
				pPropBag->Release();
			}
			pMoniker->Release();
		}
		pEnumCat->Release();
	}
	pSysDevEnum->Release();
	CoUninitialize();
	return nullRet;
}

CComQIPtr<IAMAudioInputMixer>	CDSMixer2::GetLineVolumeControlImpl(const std::wstring& awsDeviceName, const std::wstring& awsLineName, bool abInDevice) const
{
	CComQIPtr<IAMAudioInputMixer>	nullRet;
	CComQIPtr<IAMAudioInputMixer>	devicePtr = GetDeviceVolumeControlImpl( awsDeviceName, abInDevice);
	if( devicePtr == NULL)
		return nullRet;

	CComQIPtr<IBaseFilter>	filterPtr = devicePtr;

	// Now add the filter to the graph. 
	if( filterPtr != NULL)
	{
		IEnumPins*	EnumPins = NULL;
		HRESULT hr = filterPtr->EnumPins(&EnumPins);
		ATLASSERT( SUCCEEDED( hr));
		EnumPins->Reset();
		IPin*		InPin = NULL;	// renderer input
		ULONG		fetched;
		while (EnumPins->Next(1, &InPin, &fetched) == S_OK)
		{
			PIN_INFO	pinfo;
			hr = InPin->QueryPinInfo(&pinfo);
			ATLASSERT( SUCCEEDED( hr));
			pinfo.pFilter->Release();
			InPin->Release();

			if ((abInDevice && pinfo.dir == PINDIR_INPUT)
				|| (!abInDevice && pinfo.dir == PINDIR_OUTPUT))
			{
				if( wcscmp(awsLineName.data(), pinfo.achName) == 0)
				{
					CComQIPtr<IAMAudioInputMixer> pIAMAudioInputMixer = InPin;
					if( pIAMAudioInputMixer)
					{
						EnumPins->Release();
						return pIAMAudioInputMixer;
					}
				}
			}
		}
		EnumPins->Release();
	}
	return nullRet;
}

CComQIPtr<IBaseFilter>	CDSMixer2::GetOutDeviceVolumeControlImpl(const std::wstring& awsDeviceName, BOOL abInDevice) const
{
	HRESULT hr;
	CComQIPtr<IBaseFilter>	nullRet;
	ICreateDevEnum *pSysDevEnum = NULL;
	IFileSinkFilter *pSink= NULL;

	if( awsDeviceName.empty())
		return nullRet;

	std::wstring wsPrefix = L"DirectSound: ";
	std::wstring wsDeviceName = wsPrefix + awsDeviceName;

	CoInitialize( NULL);

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr))
	{
		return nullRet;
	}

	// Obtain a class enumerator for the video compressor category.
	IEnumMoniker *pEnumCat = NULL;
	GUID enumGUID = CLSID_AudioInputDeviceCategory;
	if( !abInDevice)
		enumGUID = CLSID_AudioRendererCategory;
	hr = pSysDevEnum->CreateClassEnumerator(enumGUID, &pEnumCat, 0);


	if (hr == S_OK) 
	{
		// Enumerate the monikers.
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr))
				{					
					// Display the name in your UI somehow.
					if( awsDeviceName.compare(varName.bstrVal) == 0)
						//if( awsDeviceName.find(varName.bstrVal) != std::wstring::npos)
					{
						// To create an instance of the filter, do the following:
						IBaseFilter *pFilter;
						hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);
						// Now add the filter to the graph. 
						if( pFilter)
						{
							CComQIPtr<IBaseFilter>	ret = pFilter;

							//VariantClear(&varName);
							pFilter->Release();
							//pPropBag->Release();
							//pMoniker->Release();
							//pEnumCat->Release();
							//pSysDevEnum->Release();
							//return ret;

							//std::wstring sDeviceName = varName.bstrVal
						}
					}
				}
				VariantClear(&varName);
				//Remember to release pFilter later.
				pPropBag->Release();
			}
			pMoniker->Release();
		}

		pEnumCat->Reset();
		
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr))
				{
					// Display the name in your UI somehow.
					if( wsDeviceName.compare(varName.bstrVal) == 0)
						//if( awsDeviceName.find(varName.bstrVal) != std::wstring::npos)
					{
						// To create an instance of the filter, do the following:
						IBaseFilter *pFilter;
						hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);
						// Now add the filter to the graph. 
						if( pFilter)
						{
							CComQIPtr<IBaseFilter>	ret = pFilter;

							VariantClear(&varName);
							pFilter->Release();
							pPropBag->Release();
							pMoniker->Release();
							pEnumCat->Release();
							pSysDevEnum->Release();
							return ret;
						}
					}
				}
				VariantClear(&varName);
				//Remember to release pFilter later.
				pPropBag->Release();
			}
			pMoniker->Release();
		}

		pEnumCat->Release();
	}
	pSysDevEnum->Release();
	CoUninitialize();
	return nullRet;
}

CComQIPtr<IBaseFilter>	CDSMixer2::GetOutLineVolumeControlImpl(const std::wstring& awsDeviceName, const std::wstring& awsLineName, bool abInDevice) const
{
	CComQIPtr<IBaseFilter>	nullRet;
	CComQIPtr<IBaseFilter>	devicePtr = GetDeviceVolumeControlImpl( awsDeviceName, abInDevice);
	if( devicePtr == NULL)
		return nullRet;

	CComQIPtr<IBaseFilter>	filterPtr = devicePtr;

	// Now add the filter to the graph. 
	if( filterPtr != NULL)
	{
		IEnumPins*	EnumPins = NULL;
		HRESULT hr = filterPtr->EnumPins(&EnumPins);
		ATLASSERT( SUCCEEDED( hr));
		EnumPins->Reset();
		IPin*		InPin = NULL;	// renderer input
		ULONG		fetched;
		while (EnumPins->Next(1, &InPin, &fetched) == S_OK)
		{
			PIN_INFO	pinfo;
			hr = InPin->QueryPinInfo(&pinfo);
			ATLASSERT( SUCCEEDED( hr));
			pinfo.pFilter->Release();
			InPin->Release();

			if ((abInDevice && pinfo.dir == PINDIR_INPUT)
				|| (!abInDevice && pinfo.dir == PINDIR_OUTPUT))
			{
				if( wcscmp(awsLineName.data(), pinfo.achName) == 0)
				{
					CComQIPtr<IBaseFilter> pIBasicAudio = InPin;
					if( pIBasicAudio)
					{
						EnumPins->Release();
						return pIBasicAudio;
					}
				}
			}
		}
		EnumPins->Release();
	}
	return nullRet;
}


CComQIPtr<IAMAudioInputMixer>	CDSMixer2::GetInDeviceVolumeControl(const std::wstring& awsDeviceName) const
{
	return GetDeviceVolumeControlImpl(awsDeviceName, TRUE);
}

CComQIPtr<IAMAudioInputMixer>	CDSMixer2::GetInLineVolumeControl(const std::wstring& awsDeviceName, const std::wstring& awsLineName) const
{
	return GetLineVolumeControlImpl(awsDeviceName, awsLineName, TRUE);
}
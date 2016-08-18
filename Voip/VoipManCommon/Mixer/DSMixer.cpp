#include "StdAfx.h"
#include ".\DSMixer.h"
#include ".\Mixer.h"
#include <mmsystem.h>

#include "strmif.h"
#include "uuids.h"
#include "atlcomcli.h"
#include <vector>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// asDeviceName - имя устройства вывода. For example:
// asMicrophoneName
CDSMixer::CDSMixer(const std::wstring& awsDeviceName):
	MP_WSTRING_INIT(m_wsDeviceName)
{
	m_wsDeviceName = awsDeviceName;
	WriteLog("===CDSMixer::CDSMixer 2008-06-04===");
	WriteLog(m_wsDeviceName);
	CMixer::LogAllMicrophoneNames();
}

void	CDSMixer::WriteLog(double val)
{
	/*FILE* f = fopen("c:\\mic.log", "a+");
	if( f)
	{
		fprintf( f, "%2.1f\n", val );
		fclose(f);
	}*/
}

void	CDSMixer::WriteLogNoNewLine(const std::string& str)
{
	/*FILE* f = fopen("c:\\mic.log", "a+");
	if( f)
	{
		fwrite(str.data(), str.length(), 1, f);
		fclose(f);
	}*/
}

void	CDSMixer::WriteLog(const std::wstring& str)
{
	/*USES_CONVERSION;
	FILE* f = fopen("c:\\mic.log", "a+");
	if( f)
	{
		fwrite(W2A(str.data()), str.length(), 1, f);
		fwrite((void*)"\n", 1, 1, f);
		fclose(f);
	}*/
}

void	CDSMixer::WriteLog(const std::string& str)
{
	/*FILE* f = fopen("c:\\mic.log", "a+");
	if( f)
	{
		fwrite(str.data(), str.length(), 1, f);
		fwrite((void*)"\n", 1, 1, f);
		fclose(f);
	}*/
}

CDSMixer::~CDSMixer()
{
}

WORD	CDSMixer::GetVolume()
{
	WriteLog("===GetVolume===");
	double volumeLevel = 0;
	CComQIPtr<IAMAudioInputMixer>	deviceControl = GetMicrophoneVolumeControl();
	if( deviceControl)
	{
		WriteLog("1. deviceControl");
		if( SUCCEEDED( deviceControl->get_MixLevel(&volumeLevel)))
		{
			WriteLog("2. deviceControl");
			WriteLog(volumeLevel);
			return (WORD)(0xFFFF * volumeLevel);
		}
	}
	WriteLog("3. deviceControl");
	deviceControl = GetDeviceVolumeControl();
	if( deviceControl)
	{
		WriteLog("4. deviceControl");
		if( SUCCEEDED( deviceControl->get_MixLevel(&volumeLevel)))
		{
			WriteLog("5. deviceControl");
			WriteLog(volumeLevel);
			return (WORD)(0xFFFF * volumeLevel);
		}
	}
	return 0xFFFF;
}

BOOL	CDSMixer::SetVolume(WORD dwVol)
{
	WriteLog("===SetVolume===");
	double volumeLevel = (double)dwVol / 0xFFFF;
	CComQIPtr<IAMAudioInputMixer>	deviceControl = GetMicrophoneVolumeControl();
	if( deviceControl)
	{
		WriteLog("1. deviceControl");
		if( SUCCEEDED( deviceControl->put_MixLevel(volumeLevel)))
		{
			WriteLog("2. deviceControl");
			return TRUE;
		}
	}
	WriteLog("3. deviceControl");
	deviceControl = GetDeviceVolumeControl();
	if( deviceControl)
	{
		WriteLog("4. deviceControl");
		if( SUCCEEDED( deviceControl->put_MixLevel(volumeLevel)))
		{
			WriteLog("5. deviceControl");
			return TRUE;
		}
	}
	return FALSE;
}

BOOL	CDSMixer::EnableMicrophone(BOOL abEnable)
{
	BOOL success = FALSE;
	CComQIPtr<IAMAudioInputMixer>	deviceControl = GetMicrophoneVolumeControl();
	if( deviceControl)
		if( SUCCEEDED( deviceControl->put_Enable(abEnable)))
			success = FALSE;

	deviceControl = GetDeviceVolumeControl();
	if( deviceControl)
		if( SUCCEEDED( deviceControl->put_Enable(abEnable)))
			success = FALSE;

	return success;
}

CComQIPtr<IAMAudioInputMixer>	CDSMixer::GetDeviceVolumeControl()
{
	HRESULT hr;
	CComQIPtr<IAMAudioInputMixer>	nullRet;
	ICreateDevEnum *pSysDevEnum = NULL;
	IFileSinkFilter *pSink= NULL;

	WriteLog("===GetDeviceVolumeControl()===");
	WriteLog(m_wsDeviceName);

	if( m_wsDeviceName.empty())
		return nullRet;	

	WriteLog("1. GetDeviceVolumeControl()");

	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr))
	{
		return nullRet;
	}
	WriteLog("2. GetDeviceVolumeControl()");

	// Obtain a class enumerator for the video compressor category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory, &pEnumCat, 0);

	WriteLog("3. GetDeviceVolumeControl()");
	if (hr == S_OK) 
	{
		WriteLog("4. GetDeviceVolumeControl()");
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
				WriteLog("5. GetDeviceVolumeControl()");
				if (SUCCEEDED(hr))
				{
					WriteLog(varName.bstrVal);
					// Display the name in your UI somehow.
					if( m_wsDeviceName.compare(varName.bstrVal) == 0)
					{
						WriteLog("6. GetDeviceVolumeControl()");
						// To create an instance of the filter, do the following:
						IBaseFilter *pFilter;
						hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);
						// Now add the filter to the graph. 
						if( pFilter)
						{
							WriteLog("6. GetDeviceVolumeControl()");
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
	WriteLog("8. GetDeviceVolumeControl()");
	pSysDevEnum->Release();
	return nullRet;
}

CComQIPtr<IAMAudioInputMixer>	CDSMixer::GetMicrophoneVolumeControl()
{
	WriteLog("===GetMicrophoneVolumeControl()===");
	CComQIPtr<IAMAudioInputMixer>	nullRet;

	std::vector<std::wstring> wsMicNames = CMixer::GetMicrophoneNames(m_wsDeviceName);
	if( wsMicNames.empty())
		return nullRet;

	WriteLog("===1. Device name is: ===");
	CDSMixer::WriteLogNoNewLine(" ");
	CDSMixer::WriteLog(m_wsDeviceName);

	WriteLog("===1. Microphone names are: ===");
	for(unsigned int i=0; i<wsMicNames.size(); i++)
	{
		CDSMixer::WriteLogNoNewLine(" ");
		WriteLog(wsMicNames[i]);
	}
	WriteLog("===0. GetMicrophoneVolumeControl()===");

	CComQIPtr<IAMAudioInputMixer>	devicePtr = GetDeviceVolumeControl();
	if( devicePtr == NULL)
		return nullRet;

	WriteLog(" ");
	WriteLog("1. GetMicrophoneVolumeControl()");
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
			WriteLog("2. GetMicrophoneVolumeControl()");
			WriteLog(pinfo.achName);

			bool deviceIsMicrophone = false;
			for(unsigned int i=0; i<wsMicNames.size(); i++)
			{
				if( wcscmp(wsMicNames[i].data(), pinfo.achName) == 0)
				{
					deviceIsMicrophone = true;
					break;
				}
			}

			if (pinfo.dir == PINDIR_INPUT)
			{
				if( nullRet == NULL)
				{
					WriteLog("2. NULL RET");
					WriteLog(pinfo.achName);
					nullRet = InPin;
				}
			}

			if( deviceIsMicrophone)
			{
				if (pinfo.dir == PINDIR_INPUT)
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
	WriteLog("3. GetMicrophoneVolumeControl()");
	return nullRet;
}

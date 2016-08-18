
#include "StdAfx.h"
#include "uuids.h"
#include "strmif.h"
#include "RMContext.h"
#include "PlatformDependent.h"
#include "DirectShowFunctions.h"

bool IsDeviceRegistered(const std::string& deviceName, DSDeviceType deviceType)
{
	wchar_t* deviceNameW = CharToWide(deviceName.c_str());
	
	HRESULT hr;
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum);
	if (FAILED(hr))
	{
		MP_DELETE(deviceNameW);
		g->lw.WriteLn("[ERROR-FFMPEG] CoCreateInstance CLSID_SystemDeviceEnum error");
		return false;
	}

	CLSID clsid = CLSID_VideoInputDeviceCategory;
	if(deviceType == DS_DEVICE_TYPE_AUDIO)
		clsid = CLSID_AudioInputDeviceCategory;

	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(clsid, &pEnumCat, 0);

	if (hr == S_OK) 
	{		
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK)
		{
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
			if (SUCCEEDED(hr))
			{				
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr))
				{
					if (varName.pcVal)
					{						
						if (wcscmp((wchar_t*)varName.pcVal, deviceNameW) == 0)
						{
							if(deviceType == DS_DEVICE_TYPE_AUDIO)
								g->lw.WriteLn("Audio device was found");
							else
								g->lw.WriteLn("Video device was found");
							MP_DELETE_ARR(deviceNameW);
							return true;
						}
					}
				}
				VariantClear(&varName);
				
				pPropBag->Release();
			}
			else
			{
				g->lw.WriteLn("[ERROR-FFMPEG] BindToStorage error");
			}

			pMoniker->Release();
		}

		pEnumCat->Release();
	}
	else
	{
		g->lw.WriteLn("[ERROR-FFMPEG] CreateClassEnumerator error");
	}

	pSysDevEnum->Release();

	MP_DELETE(deviceNameW);
	return false;
}

void TryToRegisterDirectShowFilterSilently(const std::string& filterName)
{
	STARTUPINFOW startupInfo; 
	PROCESS_INFORMATION processInfo; 
	memset(&startupInfo, 0, sizeof(startupInfo)); 
	memset(&processInfo, 0, sizeof(processInfo)); 
	startupInfo.cb = sizeof(startupInfo);

	std::wstring execString = GetApplicationRootDirectory();
	execString += L"video_streaming_setup.exe /DIR=\"";
	execString += GetApplicationRootDirectory();
	execString += L"\" /LANG=\"";
	
	bool isEnglish = gRM->lang;

	execString += (isEnglish) ? L"english" : L"russian";
	execString += L"\" /silent";

	g->lw.WriteLn(L"Try to register DirectShow-filter: ", execString);

	if (CreateProcessW(NULL, (LPWSTR)execString.c_str(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, 
		GetApplicationRootDirectory().c_str(), &startupInfo, &processInfo))
	{
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}
}
#include "StdAfx.h"

#if _MSC_VER >= 1400
#include "../include/PolicyConfig.h"
#endif

// HACK
// http://social.microsoft.com/Forums/en/Offtopic/thread/9ebd7ad6-a460-4a28-9de9-2af63fd4a13e
HRESULT RegisterDevice(LPCWSTR devID)
{	
#if _MSC_VER >= 1400
	if (!devID)
	{
		return E_NOTIMPL;
	}

	LPCWSTR devIDmod;
	UINT hr;
	//WCHAR longstring[255] = L"{0.0.0.00000000}.";	
	WCHAR longstring[255] = L"";	
	wcsncat_s( longstring, devID, _countof(longstring));
	devIDmod = longstring;

	IPolicyConfig* pPolicyConfig = NULL;
//	PVOID reserved;

    hr = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL,
                 CLSCTX_ALL, __uuidof(IPolicyConfig),
                 (LPVOID *)&pPolicyConfig);

	if (!pPolicyConfig)
	{
		return E_NOTIMPL;
	}

//hr = pPolicyConfig->SetDefaultEndpoint(devIDmod, ()reserved);
	hr = pPolicyConfig->SetDefaultEndpoint(devIDmod, eMultimedia);
    SAFE_RELEASE(pPolicyConfig);
	return hr;
#else
	return E_NOTIMPL;
#endif
}


#include "StdAfx.h"
#include "InputDevice.h"
#include <DShow.h>

CInputDevice::CInputDevice( LPCTSTR sName, IMoniker* pMoniker)
{
	ATLASSERT( sName);
	if ( !sName)
		return;
	m_sName = sName;

	ATLASSERT( pMoniker);
	if ( !pMoniker)
		return;
	m_pMoniker = pMoniker;
}

CInputDevice::~CInputDevice()
{
}

//////////////////////////////////////////////////////////////////////////

IMoniker *CInputDevice::GetMoniker()
{
	return (IMoniker *) m_pMoniker;
}

LPCTSTR CInputDevice::GetName()
{
	return m_sName.GetString();
}

/*
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CPin::CPin()
{
	bOwn = FALSE;
	m_pPinMixer = NULL;
}

CPin::~CPin()
{

}

//////////////////////////////////////////////////////////////////////////

HRESULT CPin::init( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum)
{
	CComPtr< IEnumPins > pEnum;	

	if ( !pFilter)
		return E_POINTER;

	HRESULT hr = pFilter->EnumPins( &pEnum);
	if ( FAILED( hr)) 
		return hr;

	ULONG ulFound;
	IPin *pPin;
	hr = E_FAIL;

	while ( S_OK == pEnum->Next( 1, &pPin, &ulFound))
	{
		PIN_DIRECTION pindir = ( PIN_DIRECTION) 3;

		pPin->QueryDirection( &pindir);
		if ( pindir == dirrequired)
		{
			if ( iNum == 0)
			{
				m_pPin = pPin;  // Return the pin's interface
				hr = S_OK;      // Found requested pin, so clear error
				break;
			}
			iNum--;
		} 

		pPin->Release();
	} 

	return hr;
}

BOOL CPin::isPinByName( TCHAR *sPinName, TCHAR *sRequiredPinName)
{
	CComString &cmsPinName = sPinName;
	cmsPinName.MakeLower();
	if ( cmsPinName.Find( sPinName) >= 0)
	{		
		bOwn = TRUE;
		return bOwn;
	}

	return bOwn;
}

HRESULT CPin::make()
{
	HRESULT hr = m_pPin->QueryInterface( IID_IAMAudioInputMixer, (void **) &m_pPinMixer);

	if ( FAILED( hr))
		return hr;

	m_pPinMixer->put_Enable( bOwn);
	m_pPinMixer->Release();
	return hr;
}

//*/
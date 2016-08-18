// AsyncConnect.h: interface for the CAsyncConnect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASYNCCONNECT_H__AF59C3BB_62F5_4CD2_A086_1307F0AFB0E4__INCLUDED_)
#define AFX_ASYNCCONNECT_H__AF59C3BB_62F5_4CD2_A086_1307F0AFB0E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "comman.h"

using namespace cm;

class CAsyncConnect : 
	public IAsyncConnect
{
public:
	CAsyncConnect( IDataHandler* aDataHandler);
	virtual ~CAsyncConnect();

	/*virtual ULONG AddRef()
	{
		iRef++;
		return iRef;
	}
	virtual ULONG Release()
	{
		iRef--;
		return iRef;
	}
	STDMETHOD(QueryInterface)( REFIID iid, void **ppvObject)
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}*/

	// 
	long setDataHandler( IDataHandler* aDataHandler)
	{
		spDataHandler = aDataHandler;
		return S_OK;
	}
	long getConnectName( BSTR *aName)
	{
		*aName = sConnectName.AllocSysString();
		return S_OK;
	}

protected:
	int iRef;
	IDataHandler *spDataHandler;
	CComString sConnectName;

};

#endif // !defined(AFX_ASYNCCONNECT_H__AF59C3BB_62F5_4CD2_A086_1307F0AFB0E4__INCLUDED_)

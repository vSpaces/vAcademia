#pragma once

#ifndef ATLASSERT
#include <assert.h>
#define ATLASSERT(x) assert(x);
#endif

template <class T>
class _NoAddRefReleaseOnCComPtr : public T
{
private:
	STDMETHOD_(ULONG, AddRef)()=0;
	STDMETHOD_(ULONG, Release)()=0;
};

template <class T>
class CComPtrBase
{
protected:
	CComPtrBase() throw()
	{
		p = NULL;
	}
	CComPtrBase(int nNull) throw()
	{
		ATLASSERT(nNull == 0);
		(void)nNull;
		p = NULL;
	}
	CComPtrBase(T* lp) throw()
	{
		p = lp;
		if (p != NULL)
			p->AddRef();
	}
public:
	typedef T _PtrClass;
	~CComPtrBase() throw()
	{
		if (p)
			p->Release();
	}
	operator T*() const throw()
	{
		return p;
	}
	T& operator*() const throw()
	{
		ATLASSERT(p!=NULL);
		return *p;
	}
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the p member explicitly.
	T** operator&() throw()
	{
		ATLASSERT(p==NULL);
		return &p;
	}
	_NoAddRefReleaseOnCComPtr<T>* operator->() const throw()
	{
		ATLASSERT(p!=NULL);
		return (_NoAddRefReleaseOnCComPtr<T>*)p;
	}
	bool operator!() const throw()
	{
		return (p == NULL);
	}
	bool operator<(T* pT) const throw()
	{
		return p < pT;
	}
	bool operator==(T* pT) const throw()
	{
		return p == pT;
	}

	// Release the interface and set to NULL
	void Release() throw()
	{
		T* pTemp = p;
		if (pTemp)
		{
			p = NULL;
			pTemp->Release();
		}
	}
	// Compare two objects for equivalence
	bool IsEqualObject(IUnknown* pOther) throw()
	{
		if (p == NULL && pOther == NULL)
			return true;	// They are both NULL objects

		if (p == NULL || pOther == NULL)
			return false;	// One is NULL the other is not

		CComPtr<IUnknown> punk1;
		CComPtr<IUnknown> punk2;
		p->QueryInterface(__uuidof(IUnknown), (void**)&punk1);
		pOther->QueryInterface(__uuidof(IUnknown), (void**)&punk2);
		return punk1 == punk2;
	}
	// Attach to an existing interface (does not AddRef)
	void Attach(T* p2) throw()
	{
		if (p)
			p->Release();
		p = p2;
	}
	// Detach the interface (does not Release)
	T* Detach() throw()
	{
		T* pt = p;
		p = NULL;
		return pt;
	}
	HRESULT CopyTo(T** ppT) throw()
	{
		ATLASSERT(ppT != NULL);
		if (ppT == NULL)
			return E_POINTER;
		*ppT = p;
		if (p)
			p->AddRef();
		return S_OK;
	}
	HRESULT SetSite(IUnknown* punkParent) throw()
	{
		return AtlSetChildSite(p, punkParent);
	}
	HRESULT Advise(IUnknown* pUnk, const IID& iid, LPDWORD pdw) throw()
	{
		return AtlAdvise(p, pUnk, iid, pdw);
	}
	HRESULT CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL) throw()
	{
		ATLASSERT(p == NULL);
		return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&p);
	}
	HRESULT CoCreateInstance(LPCOLESTR szProgID, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL) throw()
	{
		CLSID clsid;
		HRESULT hr = CLSIDFromProgID(szProgID, &clsid);
		ATLASSERT(p == NULL);
		if (SUCCEEDED(hr))
			hr = ::CoCreateInstance(clsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&p);
		return hr;
	}
	template <class Q>
	HRESULT QueryInterface(Q** pp) const throw()
	{
		ATLASSERT(pp != NULL);
		return p->QueryInterface(__uuidof(Q), (void**)pp);
	}
	T* p;
};

template <class T>
class CComPtr : public CComPtrBase<T>
{
public:
	CComPtr() throw()
	{
	}
	CComPtr(int nNull) throw() :
		CComPtrBase<T>(nNull)
	{
	}
	/*
	template <typename Q>
	CComPtr(Q* lp)
	{
		if (lp != NULL)
			lp->QueryInterface(__uuidof(Q), (void**)&p);
		else
			p = NULL;
	}
	template <>
	*/
	CComPtr(T* lp) throw() :
		CComPtrBase<T>(lp)

	{
	}
	CComPtr(const CComPtr<T>& lp) throw() :
		CComPtrBase<T>(lp.p)
	{
	}
//	template<>
	/*
	T* operator=(void* lp)
	{
		return (T*)AtlComPtrAssign((IUnknown**)&p, (T*)lp);
	}
	*/
	/*
	template <typename Q>
	T* operator=(Q* lp)
	{
		return (T*)AtlComQIPtrAssign((IUnknown**)&p, lp, __uuidof(T));
	}
	template <>
	*/
	T* operator=(T* lp) throw()
	{
		return static_cast<T*>(AtlComPtrAssign((IUnknown**)&p, lp));
	}
	template <typename Q>
	T* operator=(const CComPtr<Q>& lp) throw()
	{
		return static_cast<T*>(AtlComQIPtrAssign((IUnknown**)&p, lp, __uuidof(T)));
	}
	template <>
	T* operator=(const CComPtr<T>& lp) throw()
	{
		return static_cast<T*>(AtlComPtrAssign((IUnknown**)&p, lp));
	}
};

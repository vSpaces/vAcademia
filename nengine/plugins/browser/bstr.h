#pragma once

#ifndef ATLASSERT
inline void AtlThrow( HRESULT hr ){
	throw;
	// ??
}

#define ATLASSERT(expr) assert(expr)
inline int ocslen(LPCOLESTR x) throw() { return lstrlenW(x); }

#ifndef _DEBUG
#define USES_CONVERSION_EX int _convert_ex; (_convert_ex); UINT _acp_ex = CP_ACP; (_acp_ex); LPCWSTR _lpw_ex; (_lpw_ex); LPCSTR _lpa_ex; (_lpa_ex); 
//USES_ATL_SAFE_ALLOCA
#else
#define USES_CONVERSION_EX int _convert_ex = 0; (_convert_ex); UINT _acp_ex = CP_ACP; (_acp_ex); LPCWSTR _lpw_ex = NULL; (_lpw_ex); LPCSTR _lpa_ex = NULL; (_lpa_ex); 
//USES_ATL_SAFE_ALLOCA
#endif

inline BSTR A2WBSTR(LPCSTR lp, int nLen = -1)
{
	if (lp == NULL || nLen == 0)
		return NULL;
	USES_CONVERSION_EX;
	BSTR str = NULL;
	int nConvertedLen = MultiByteToWideChar(_acp_ex, 0, lp,
		nLen, NULL, NULL);
	int nAllocLen = nConvertedLen;
	if (nLen == -1)
		nAllocLen -= 1;  // Don't allocate terminating '\0'
	str = ::SysAllocStringLen(NULL, nAllocLen);

	if (str != NULL)
	{
		int nResult;
		nResult = MultiByteToWideChar(_acp_ex, 0, lp, nLen, str, nConvertedLen);
		ATLASSERT(nResult == nConvertedLen);
		if(nResult != nConvertedLen)
		{
			SysFreeString(str);
			return NULL;
		}

	}
	return str;
}

#define ATLTRYALLOC(x) try{x;} catch(...){}

#define ATLTRY(x) ATLTRYALLOC(x)

#endif

class CBSTR
{
public:
	BSTR m_str;
	CBSTR() throw()
	{
		m_str = NULL;
	}
	CBSTR(int nSize)
	{
		if (nSize == 0)
			m_str = NULL;
		else
		{
			m_str = ::SysAllocStringLen(NULL, nSize);
			if (m_str == NULL)
				AtlThrow(E_OUTOFMEMORY);
		}
	}
	CBSTR(int nSize, LPCOLESTR sz)
	{
		if (nSize == 0)
			m_str = NULL;
		else
		{
			m_str = ::SysAllocStringLen(sz, nSize);
			if (m_str == NULL)
				AtlThrow(E_OUTOFMEMORY);
		}
	}
	CBSTR(LPCOLESTR pSrc)
	{
		if (pSrc == NULL)
			m_str = NULL;
		else
		{
			m_str = ::SysAllocString(pSrc);
			if (m_str == NULL)
				AtlThrow(E_OUTOFMEMORY);
		}
	}
	CBSTR(const CBSTR& src)
	{
		m_str = src.Copy();
		if (!!src && m_str == NULL)
			AtlThrow(E_OUTOFMEMORY);

	}
	CBSTR(REFGUID guid)
	{
		OLECHAR szGUID[64];
		::StringFromGUID2(guid, szGUID, 64);
		m_str = ::SysAllocString(szGUID);
		if (m_str == NULL)
			AtlThrow(E_OUTOFMEMORY);
	}

	CBSTR& operator=(const CBSTR& src)
	{
		if (m_str != src.m_str)
		{
			::SysFreeString(m_str);
			m_str = src.Copy();
			if (!!src && m_str == NULL)
				AtlThrow(E_OUTOFMEMORY);
		}
		return *this;
	}

	CBSTR& operator=(LPCOLESTR pSrc)
	{
		if (pSrc != m_str)
		{
			::SysFreeString(m_str);
			if (pSrc != NULL)
			{
				m_str = ::SysAllocString(pSrc);
				if (m_str == NULL)
					AtlThrow(E_OUTOFMEMORY);
			}
			else
				m_str = NULL;
		}
		return *this;
	}

	~CBSTR() throw()
	{
		::SysFreeString(m_str);
	}
	unsigned int Length() const throw()
	{
		return (m_str == NULL)? 0 : SysStringLen(m_str);
	}
	unsigned int ByteLength() const throw()
	{
		return (m_str == NULL)? 0 : SysStringByteLen(m_str);
	}
	operator BSTR() const throw()
	{
		return m_str;
	}
	BSTR* operator&() throw()
	{
		return &m_str;
	}
	BSTR Copy() const throw()
	{
		if (m_str == NULL)
			return NULL;
		return ::SysAllocStringByteLen((char*)m_str, ::SysStringByteLen(m_str));
	}
	HRESULT CopyTo(BSTR* pbstr) throw()
	{
		ATLASSERT(pbstr != NULL);
		if (pbstr == NULL)
			return E_POINTER;
		*pbstr = Copy();
		if ((*pbstr == NULL) && (m_str != NULL))
			return E_OUTOFMEMORY;
		return S_OK;
	}
	// copy BSTR to VARIANT
	HRESULT CopyTo(VARIANT *pvarDest) throw()
	{
		ATLASSERT(pvarDest != NULL);
		HRESULT hRes = E_POINTER;
		if (pvarDest != NULL)
		{
			pvarDest->vt = VT_BSTR;
			pvarDest->bstrVal = Copy();
			if (pvarDest->bstrVal == NULL && m_str != NULL)
				hRes = E_OUTOFMEMORY;
			else
				hRes = S_OK;
		}
		return hRes;
	}
	void Attach(BSTR src) throw()
	{
		if (m_str != src)
		{
			::SysFreeString(m_str);
			m_str = src;
		}
	}
	BSTR Detach() throw()
	{
		BSTR s = m_str;
		m_str = NULL;
		return s;
	}
	void Empty() throw()
	{
		::SysFreeString(m_str);
		m_str = NULL;
	}
	bool operator!() const throw()
	{
		return (m_str == NULL);
	}
	HRESULT Append(const CBSTR& bstrSrc) throw()
	{
		return AppendBSTR(bstrSrc.m_str);
	}
	HRESULT Append(LPCOLESTR lpsz) throw()
	{
		return Append(lpsz, UINT(ocslen(lpsz)));
	}
	// a BSTR is just a LPCOLESTR so we need a special version to signify
	// that we are appending a BSTR
	HRESULT AppendBSTR(BSTR p) throw()
	{
		if (p == NULL)
			return S_OK;
		BSTR bstrNew = NULL;
		HRESULT hr;
		hr = VarBstrCat(m_str, p, &bstrNew);
		if (SUCCEEDED(hr))
		{
			::SysFreeString(m_str);
			m_str = bstrNew;
		}
		return hr;
	}
	HRESULT Append(LPCOLESTR lpsz, int nLen) throw()
	{
		if (lpsz == NULL || (m_str != NULL && nLen == 0))
			return S_OK;
		int n1 = Length();
		BSTR b;
		b = ::SysAllocStringLen(NULL, n1+nLen);
		if (b == NULL)
			return E_OUTOFMEMORY;
		if(m_str != NULL)
			memcpy(b, m_str, n1*sizeof(OLECHAR));
		memcpy(b+n1, lpsz, nLen*sizeof(OLECHAR));
		b[n1+nLen] = NULL;
		SysFreeString(m_str);
		m_str = b;
		return S_OK;
	}
	HRESULT Append(char ch) throw()
	{
		OLECHAR chO = ch;

		return( Append( &chO, 1 ) );
	}
	HRESULT Append(wchar_t ch) throw()
	{
		return( Append( &ch, 1 ) );
	}
	HRESULT AppendBytes(const char* lpsz, int nLen) throw()
	{
		if (lpsz == NULL || nLen == 0)
			return S_OK;
		int n1 = ByteLength();
		BSTR b;
		b = ::SysAllocStringByteLen(NULL, n1+nLen);
		if (b == NULL)
			return E_OUTOFMEMORY;
		memcpy(b, m_str, n1);
		memcpy(((char*)b)+n1, lpsz, nLen);
		*((OLECHAR*)(((char*)b)+n1+nLen)) = NULL;
		SysFreeString(m_str);
		m_str = b;
		return S_OK;
	}
	HRESULT AssignBSTR(const BSTR bstrSrc) throw()
	{
		HRESULT hr = S_OK;
		if (m_str != bstrSrc)
		{
			::SysFreeString(m_str);
			if (bstrSrc != NULL)
			{
				m_str = ::SysAllocStringByteLen((char*)bstrSrc, ::SysStringByteLen(bstrSrc));
				if (m_str == NULL)
					hr = E_OUTOFMEMORY;
			}
			else
				m_str = NULL;
		}

		return hr;
	}

	CBSTR& operator+=(const CBSTR& bstrSrc)
	{
		HRESULT hr;
		hr = AppendBSTR(bstrSrc.m_str);
		if (FAILED(hr))
			AtlThrow(hr);
		return *this;
	}
	CBSTR& operator+=(LPCOLESTR pszSrc)
	{
		HRESULT hr;
		hr = Append(pszSrc);
		if (FAILED(hr))
			AtlThrow(hr);
		return *this;
	}

	bool operator<(const CBSTR& bstrSrc) const throw()
	{
		return VarBstrCmp(m_str, bstrSrc.m_str, LOCALE_USER_DEFAULT, 0) == VARCMP_LT;
	}
	bool operator<(LPCOLESTR pszSrc) const
	{
		CBSTR bstr2(pszSrc);
		return operator<(bstr2);
	}
	bool operator<(LPOLESTR pszSrc) const
	{
		return operator<((LPCOLESTR)pszSrc);
	}

	bool operator>(const CBSTR& bstrSrc) const throw()
	{
		return VarBstrCmp(m_str, bstrSrc.m_str, LOCALE_USER_DEFAULT, 0) == VARCMP_GT;
	}
	bool operator>(LPCOLESTR pszSrc) const
	{
		CBSTR bstr2(pszSrc);
		return operator>(bstr2);
	}
	bool operator>(LPOLESTR pszSrc) const
	{
		return operator>((LPCOLESTR)pszSrc);
	}
	
	bool operator!=(const CBSTR& bstrSrc) const throw()
	{
		return !operator==(bstrSrc);
	}
	bool operator!=(LPCOLESTR pszSrc) const
	{
		return !operator==(pszSrc);
	}
	bool operator!=(int nNull) const throw()
	{
		return !operator==(nNull);
	}
	bool operator!=(LPOLESTR pszSrc) const
	{
		return operator!=((LPCOLESTR)pszSrc);
	}

	bool operator==(const CBSTR& bstrSrc) const throw()
	{
		return VarBstrCmp(m_str, bstrSrc.m_str, LOCALE_USER_DEFAULT, 0) == VARCMP_EQ;
	}
	bool operator==(LPCOLESTR pszSrc) const
	{
		CBSTR bstr2(pszSrc);
		return operator==(bstr2);
	}
	bool operator==(LPOLESTR pszSrc) const
	{
		return operator==((LPCOLESTR)pszSrc);
	}
	
	bool operator==(int nNull) const throw()
	{
		ATLASSERT(nNull == NULL);
		(void)nNull;
		return (m_str == NULL);
	}
	CBSTR(LPCSTR pSrc)
	{
		if (pSrc != NULL)
		{
			m_str = A2WBSTR(pSrc);
			if (m_str == NULL)
				AtlThrow(E_OUTOFMEMORY);
		}
		else
			m_str = NULL;
	}

	CBSTR(int nSize, LPCSTR sz)
	{
		if (nSize != 0 && sz == NULL)
		{
			m_str = ::SysAllocStringLen(NULL, nSize);
			if (m_str == NULL)
				AtlThrow(E_OUTOFMEMORY);
			return;
		}

		m_str = A2WBSTR(sz, nSize);
		if (m_str == NULL && nSize != 0)
			AtlThrow(E_OUTOFMEMORY);
	}

	HRESULT Append(LPCSTR lpsz) throw()
	{
		if (lpsz == NULL)
			return S_OK;

		CBSTR bstrTemp;
		ATLTRY(bstrTemp = lpsz);
		if (bstrTemp.m_str == NULL)
			return E_OUTOFMEMORY;
		return Append(bstrTemp);
	}

	CBSTR& operator=(LPCSTR pSrc)
	{
		::SysFreeString(m_str);
		m_str = A2WBSTR(pSrc);
		if (m_str == NULL && pSrc != NULL)
			AtlThrow(E_OUTOFMEMORY);
		return *this;
	}
	bool operator<(LPCSTR pszSrc) const
	{
		CBSTR bstr2(pszSrc);
		return operator<(bstr2);
	}
	bool operator>(LPCSTR pszSrc) const
	{
		CBSTR bstr2(pszSrc);
		return operator>(bstr2);
	}
	bool operator!=(LPCSTR pszSrc) const
	{
		return !operator==(pszSrc);
	}
	bool operator==(LPCSTR pszSrc) const
	{
		CBSTR bstr2(pszSrc);
		return operator==(bstr2);
	}
	HRESULT WriteToStream(IStream* pStream) throw()
	{
		ATLASSERT(pStream != NULL);
		if(pStream == NULL)
			return E_INVALIDARG;
			
		ULONG cb;
		ULONG cbStrLen = ULONG(m_str ? SysStringByteLen(m_str)+sizeof(OLECHAR) : 0);
		HRESULT hr = pStream->Write((void*) &cbStrLen, sizeof(cbStrLen), &cb);
		if (FAILED(hr))
			return hr;
		return cbStrLen ? pStream->Write((void*) m_str, cbStrLen, &cb) : S_OK;
	}
	HRESULT ReadFromStream(IStream* pStream) throw()
	{
		ATLASSERT(pStream != NULL);
		if(pStream == NULL)
			return E_INVALIDARG;
			
		ATLASSERT(m_str == NULL); // should be empty
		Empty();
		
		ULONG cbStrLen = 0;
		HRESULT hr = pStream->Read((void*) &cbStrLen, sizeof(cbStrLen), NULL);
		if ((hr == S_OK) && (cbStrLen != 0))
		{
			//subtract size for terminating NULL which we wrote out
			//since SysAllocStringByteLen overallocates for the NULL
			m_str = SysAllocStringByteLen(NULL, cbStrLen-sizeof(OLECHAR));
			if (m_str == NULL)
				hr = E_OUTOFMEMORY;
			else
				hr = pStream->Read((void*) m_str, cbStrLen, NULL);
			// If SysAllocStringByteLen or IStream::Read failed, reset seek 
			// pointer to start of BSTR size.
			if (hr != S_OK)
			{
				LARGE_INTEGER nOffset;
				nOffset.QuadPart = -(static_cast<LONGLONG>(sizeof(cbStrLen)));
				pStream->Seek(nOffset, STREAM_SEEK_CUR, NULL);
			}
		}
		if (hr == S_FALSE)
			hr = E_FAIL;
		return hr;
	}

	// each character in BSTR is copied to each element in SAFEARRAY
	HRESULT BSTRToArray(LPSAFEARRAY *ppArray) throw()
	{
		return VectorFromBstr(m_str, ppArray);
	}

	// first character of each element in SAFEARRAY is copied to BSTR
	HRESULT ArrayToBSTR(const SAFEARRAY *pSrc) throw()
	{
		::SysFreeString(m_str);
		return BstrFromVector((LPSAFEARRAY)pSrc, &m_str);
	}
};

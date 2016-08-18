#if !defined(AF_MYSTRING_H__C514A391_FB4C_11D3_AABC_0000E215F0C2__INCLUDED_)
#define AF_MYSTRING_H__C514A391_FB4C_11D3_AABC_0000E215F0C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMyWString;

typedef const char* MYS_LPCSTR;
//typedef char* MYS_LPSTR;
typedef const wchar_t* MYS_LPWCSTR;

class CMyString  
{
public: 
    CMyString();
    CMyString(char* auStr);
    CMyString(wchar_t* auStr);
	CMyString(CMyString& strSrc);
    CMyString(CMyWString& strSrc);
	virtual ~CMyString();

	// Assignment Operations
	const CMyString& operator=(CMyString& strSrc);
	const CMyString& operator=(const char* lpsz);
	const CMyString& operator=(wchar_t* bstrStr);
	operator MYS_LPCSTR() const { return mpszString; }
	char*	GetString() const { return mpszString; }

	const CMyString& operator+=(CMyString& strSrc);
	const CMyString& operator+=(CMyWString& strSrc);
	const CMyString& operator+=(const char* lpsz);
	const CMyString& operator+=(const wchar_t* bstrStr);
//	const CMyString& operator+=(TCHAR ch);
	const CMyString& operator+=(int aiNum);
	const CMyString& operator+=(double adNum);

	// Accessors for the String as an Array
	int		GetLength() const;
	bool	IsEmpty() const;
	void	Empty();
	char	GetAt(int nIndex);
	void	SetAt(int nIndex, char ch);
	char	operator[] (int nIndex);

	// Extraction
	CMyString Mid(int nFirst) const;
	CMyString Mid(int nFirst, int nCount) const;
	CMyString Left(int nCount) const;
	CMyString Right(int nCount) const;
	CMyString SpanIncluding(const char* lpszCharSet) const;
	CMyString SpanExcluding(const char* lpszCharSet) const;

	// Replacing
	int Replace(char chOld, char chNew);
	int Replace(const char* lpszOld, const char* lpszNew);

protected:
	char* mpszString;
	void StringCopy(CMyString& str, int nLen, int nIndex, int nExtra) const;
	void StringCopy(int nSrcLen, const char* lpszSrcData);
	void ConcatCopy(const char* lpszData);
	void ConcatCopy(char ch);
	void ConcatCopy(const char* lpszData1, const char* lpszData2);
	void AllocString(int nLen);
	void ReAllocString(int nLen);
};

class CMyWString  
{
public: 
    CMyWString();
    CMyWString(wchar_t* auStr);
	virtual ~CMyWString();

	operator MYS_LPWCSTR() const { return mpwcString; }

	// Accessors for the String as an Array
	int		GetLength() const;
	bool	IsEmpty() const;
	void	Empty();
	char	GetAt(int nIndex);
	void	SetAt(int nIndex, char ch);
	char	operator[] (int nIndex);
protected:
	wchar_t* mpwcString;
	void AllocString(int nLen);
};

#endif
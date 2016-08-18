// CWComString.h : header file
#pragma once

class VA_RTL_API CWComString  
{
public: 
	void CreateString( const wchar_t* aStrData, int aDataLen);
	wchar_t* GetBuffer(int count = -1);
	CWComString();
	CWComString(const CWComString& str);
	CWComString(const wchar_t* pszString);
	CWComString(wchar_t ch, int nRepeat);
	CWComString(const wchar_t* aCharData, int aDataLen);
	CWComString(HINSTANCE aInstance, UINT aResourceID);
	virtual ~CWComString();

	bool LoadString(HINSTANCE hInst, UINT nID);

	// Assignment Operations
	const CWComString& operator=(const CWComString& strSrc);
	const CWComString& operator=(const wchar_t* lpsz);
	operator const wchar_t*() const	{ return m_pszString; }
	const wchar_t*	GetString() const	{ return m_pszString; }
	BSTR	AllocSysString() const;
	
	// Concatenation
	const CWComString& operator+=(CWComString& strSrc);
	const CWComString& operator+=(const wchar_t* lpsz);
	const CWComString& operator+=(wchar_t ch);

	friend VA_RTL_API CWComString operator+(CWComString& strSrc1, CWComString& strSrc2);
	friend VA_RTL_API CWComString operator+(const CWComString& strSrc1, const CWComString& strSrc2);

	// Insertion
	void	Insert(int nIndex, wchar_t ch);
	void	Insert(int nIndex, const wchar_t* lpszStr);
	void	Insert(int nIndex, const CWComString& strSrc);

	// Accessors for the String as an Array
	int		GetLength() const;
	bool	IsEmpty() const;
	void	Empty();
	wchar_t	GetAt(int nIndex);
	void	SetAt(int nIndex, wchar_t ch);
	wchar_t	operator[] (int nIndex);

	// Conversions
	void	MakeUpper();
	void	MakeLower();
	void	MakeReverse();
	void	TrimLeft();
	void	TrimRight();

	// Searching
	int		Find(wchar_t ch)					const;
	int		Find(wchar_t ch, int nStart)		const;
	int     FindChar(wchar_t ch)				const;
	int     ReverseFind(wchar_t ch)			const;
	int     FindChar(wchar_t ch, int nStart)	const;
	int		FindOneOf(const wchar_t* lpszCharSet)	const;
	int		Find(const wchar_t* lpszSub);
	int		Find(const wchar_t* lpszSub, int nStart);

	// Extraction
	CWComString Mid(int nFirst) const;
	CWComString Mid(int nFirst, int nCount) const;
	CWComString Left(int nCount) const;
	CWComString Right(int nCount) const;
	CWComString SpanIncluding(const wchar_t* lpszCharSet) const;
	CWComString SpanExcluding(const wchar_t* lpszCharSet) const;

	// Comparison
	int Compare(CWComString& str) const;
	int Compare(const wchar_t* lpsz) const;
	int CompareNoCase(CWComString& str) const;
	int CompareNoCase(const wchar_t* lpsz) const;
	int Collate(CWComString& str) const;
	int Collate(const wchar_t* lpsz) const;

    const BOOL operator>(CWComString& strSrc);
    const BOOL operator<(CWComString& strSrc);

	// Formatting
	void Format(const wchar_t* pszCharSet, ...);
	void FormatV(const wchar_t* pszCharSet, va_list vl);

	// Replacing
	int Replace(wchar_t chOld, wchar_t chNew);
	int Replace(const wchar_t* lpszOld, const wchar_t* lpszNew);

	// Deleting
	int Delete(int nIndex, int nCount = 1);
	
	wchar_t* GetBufferSetLength(int count);

protected:
	wchar_t*	m_pszString;
	void	StringCopy(CWComString& str, int nLen, int nIndex, int nExtra) const;
	void	StringCopy(int nSrcLen, const wchar_t* lpszSrcData);
	void	ConcatCopy(const wchar_t* lpszData);
	void	ConcatCopy(wchar_t ch);
	void	ConcatCopy(const wchar_t* lpszData1, const wchar_t* lpszData2);
	void	AllocString(int nLen);
	void	ReAllocString(int nLen);
	int		m_allocetedStringSize;
};	

// Compare operations
bool operator==(const CWComString& s1, const CWComString& s2);
bool operator==(const CWComString& s1, const wchar_t* s2);
bool operator==(const wchar_t* s1, const CWComString& s2);
bool operator!=(const CWComString& s1, const CWComString& s2);
bool operator!=(const CWComString& s1, const wchar_t* s2);
bool operator!=(const wchar_t* s1, const CWComString& s2);
bool operator<(const CWComString& s1, const CWComString& s2);
bool operator<(const CWComString& s1, const wchar_t* s2);
bool operator<(const wchar_t* s1, const CWComString& s2);
bool operator>(const CWComString& s1, const CWComString& s2);
bool operator>(const CWComString& s1, const wchar_t* s2);
bool operator>(const wchar_t* s1, const CWComString& s2);

// Compare implementations
inline bool operator==(const CWComString& s1, const CWComString& s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(const CWComString& s1, const wchar_t* s2)
	{ return s1.Compare(s2) == 0; }
inline bool operator==(const wchar_t* s1, const CWComString& s2)
	{ return s2.Compare(s1) == 0; }
inline bool operator!=(const CWComString& s1, const CWComString& s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(const CWComString& s1, const wchar_t* s2)
	{ return s1.Compare(s2) != 0; }
inline bool operator!=(const wchar_t* s1, const CWComString& s2)
	{ return s2.Compare(s1) != 0; }
inline bool operator<(const CWComString& s1, const CWComString& s2)
	{ return s1.Compare(s2) < 0; }
inline bool operator<(const CWComString& s1, const wchar_t* s2)
	{ return s1.Compare(s2) < 0; }
inline bool operator<(const wchar_t* s1, const CWComString& s2)
	{ return s2.Compare(s1) > 0; }
inline bool operator>(const CWComString& s1, const CWComString& s2)
	{ return s1.Compare(s2) > 0; }
inline bool operator>(const CWComString& s1, const wchar_t* s2)
	{ return s1.Compare(s2) > 0; }
inline bool operator>(const wchar_t* s1, const CWComString& s2)
	{ return s2.Compare(s1) < 0; }
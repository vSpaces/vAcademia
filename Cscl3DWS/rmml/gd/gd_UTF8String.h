// UTF8String.h: interface for the CUTF8String class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTF8STRING_H__E3392447_138A_446A_92E3_6E09FD04025E__INCLUDED_)
#define AFX_UTF8STRING_H__E3392447_138A_446A_92E3_6E09FD04025E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef _GD_UTF8STRING_H_
#define _GD_UTF8STRING_H_
#endif


#ifndef _TCHAR_DEFINED
	#pragma warning( disable : 4267 )
	#include <cwchar>
	#include <memory>
	#include <string>
	#include <cassert>
	#define _ASSERT assert
	typedef unsigned long       DWORD;
	typedef int                 BOOL;
	typedef unsigned char       BYTE;
	typedef unsigned short      WORD;
	typedef float               FLOAT;
	typedef void	            *LPVOID;
	typedef int                 INT;
	typedef unsigned int        UINT;
	typedef long HRESULT;
	#define S_OK                                   ((HRESULT)0x00000000L)
	#define S_FALSE                                ((HRESULT)0x00000001L)
	#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)
	#define E_FAIL                           _HRESULT_TYPEDEF_(0x80004005L)
	#define FALSE   0
	#define TRUE    1
	#define NULL    0
	#define _T(x)	L##x
#endif

namespace gd
{

/*! \brief string object for the UTF8 format

CUTF8String stores text internaly in utf8 format.
\verbatim
1 0xxxxxxx                            (7F)
2 110xxxxx 10xxxxxx                   (7FF)
3 1110xxxx 10xxxxxx 10xxxxxx          (FFFF)
4 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx (1FFFFF)
\endverbatim

\p
Working with the CUTF8String you never need to convert text to the utf8 format. The CUTF8String
manages conversion internally. it is possible to get information as utf8 text from the CUTF8String
object using methods for that.
*/
class CUTF8String  
{
public:
   struct MaskItem
   {
     unsigned chMask;
     unsigned uShift;
     unsigned uSize;
     unsigned wcMax;
   };

public:
   CUTF8String() {
      m_pData = (CUTF8String::_Data*)pdata_nil; }
   CUTF8String( const char* pbszText );
#ifdef _WCHAR_T_DEFINED
   CUTF8String( const wchar_t* pwszText );
#else
   CUTF8String( const unsigned short* pwszText );
#endif
   CUTF8String( const CUTF8String& other );
   CUTF8String& operator=( const CUTF8String& other );
	~CUTF8String();

// Operator
public:
   //! returns const pointer to the text buffer
   operator const char*() const throw() {
      return m_pData->GetText(); }
   //! assign to char string
   CUTF8String& operator=(const char* pbsz) {
      Empty(); Add( pbsz ); return *this; }
   //! assign to widechar
#ifdef _WCHAR_T_DEFINED
   CUTF8String& operator=(const wchar_t* pwsz) {
#else
   CUTF8String& operator=(const unsigned short* pwsz) {
#endif
      if( m_pData != (CUTF8String::_Data*)pdata_nil ) m_pData->SetLength( 0 );
      Add( pwsz ); return *this; }
   //! add multibyte character
   CUTF8String& operator+=(char chbCharacter) throw() {
      Add( chbCharacter ); return *this; }
   //! add widechar
#ifdef _WCHAR_T_DEFINED
   CUTF8String& operator+=(wchar_t chwCharacter) throw() {
#else
   CUTF8String& operator+=(unsigned short chwCharacter) throw() {
#endif
      Add( chwCharacter ); return *this; }
   //! add multibyte text
   CUTF8String& operator+=(const char* pbszText) throw() {
      Add( pbszText ); return *this; }
   //! add unicode text
#ifdef _WCHAR_T_DEFINED
   CUTF8String& operator+=(const wchar_t* pwszText) throw() {
#else
   CUTF8String& operator+=(const unsigned short* pwszText) throw() {
#endif
      Add( pwszText ); return *this; }
   //! is string equal to 
   bool operator==( const CUTF8String& o ) const;
   //! is string less than the string compared to
   bool operator<(const CUTF8String& other) const {
      return (strcmp( m_pData->GetText(), other.m_pData->GetText() ) < 0 ); } 
   //! is string greater than the string compared to
   bool operator>(const CUTF8String& other) const {
      return (strcmp( m_pData->GetText(), other.m_pData->GetText() ) > 0 ); } 


// Get/Set
public:
   //! Get the UTF8 length in bytes
   unsigned GetLength();
   //! Get the text in UTF8 format
   char* GetUTF8Text();
#ifdef _MFC_VER
   //! return MFC string containing text from internal buffer
   CString MFCGetString();
#endif

#ifdef _GD_STRING_H_
   //! return GD string containing text from internal buffer
   gd::CString GDGetString();
#endif

// Operation
public:
   //! set raw utf8 formated string
   void SetUTF8String( char* pbszUTF8, int iLength );

   //! Add char character to string
   void Add( char chCharacter );
   //! Add character to string (unicode version)
#ifdef _WCHAR_T_DEFINED
   void Add( wchar_t chwCharacter );
#else
   void Add( unsigned short chwCharacter );
#endif 
   //! add normal text to the CUTF8String object
   void Add( const char* pbszText );
   //! add normal text with specified length to the CUTF8String object
   void Add( const char* pbszText, int iLength );
   //! add unicode text
#ifdef _WCHAR_T_DEFINED
   void Add( const wchar_t* pwszText );
#else
   void Add( const unsigned short* pwszText );
#endif 
   //! add unicode text
#ifdef _WCHAR_T_DEFINED
   void Add( const wchar_t* pwszText, int iLength );
#else
   void Add( const unsigned short* pwszText, int iLength );
#endif 
   //! insert text into specified position
   void Insert( unsigned uPosition, const wchar_t* pszText, unsigned uRepeat = 1 );

   // * FIND
   //! Finds position where characters is located from start. returns zero-based index for the 
   //! character or -1 if not found.
   int Find( char chCharacter ) const;
   //! Finds position where characters is located beging to search from iStart. returns zero 
   //! base index for the character or -1 if not found.
   int Find( char chCharacter, unsigned uStart ) const;
   //! Find text in string and return zero-based index for first textitem that is found. If not
   //! found then -1 is returned
   int Find( const char* pbszText ) const;
   int Find( const char* pbszText, unsigned uStart ) const;
#ifdef _WCHAR_T_DEFINED
   //! Finds position where characters is located from start. returns zero-based index for the 
   //! character or -1 if not found.
   int Find( wchar_t chCharacter ) const;
   //! Finds position where characters is located beging to search from iStart. returns zero 
   //! base index for the character or -1 if not found.
   int Find( wchar_t chCharacter, unsigned uStart ) const;
   //! Find text in string and return zero-based index for first textitem that is found. If not
   //! found then -1 is returned
   int Find( const wchar_t* pbszText ) const;
   int Find( const wchar_t* pbszText, unsigned uStart ) const;
#else
   //! Finds position where characters is located from start. returns zero-based index for the 
   //! character or -1 if not found.
   int Find( unsigned short chCharacter ) const;
   //! Finds position where characters is located beging to search from iStart. returns zero 
   //! base index for the character or -1 if not found.
   int Find( unsigned short chCharacter, unsigned uStart ) const;
   //! Find text in string and return zero-based index for first textitem that is found. If not
   //! found then -1 is returned
   int Find( const unsigned short* pbszText ) const;
   int Find( const unsigned short* pbszText, unsigned uStart ) const;
#endif
   //! Find utf8 formated text, returns index to first position or -1 if not found
   int FindUTF8( const char* pbszUTF8Find, unsigned uStart ) const;

   //! return string starting at specified position and the requested length
   CUTF8String Mid( unsigned uStart, int iLength = -1 ) const;

   // * REMOVE
   //! empty string on characters
   void Empty();

#ifdef _DEBUG
   //* DEBUG
   //! 
   void HexDump();
#endif

public:
   //
   static char* GotoUTF8Char( unsigned uIndex, char* pszText );
   //! convert character to UFT8 format
   static unsigned UTF8Char( unsigned uChar, char* pszBuffer );
   //! calculate lenght for unicode string in UTF8 format
#ifdef _WCHAR_T_DEFINED
   static unsigned GetUTF8Length( const wchar_t* pwszWC, int iMax = -1 );
#else
   static unsigned GetUTF8Length( unsigned short* pwszWC, int iMax = -1 );
#endif
   //! calculate lenght for multibyte string in UTF8 format
   static unsigned GetUTF8Length( const char* pbszMB, int iMax = -1 );
   //! Calculate length for string containing UFT8 characters
   static unsigned GetTextLength( const char* pbszUTF8, int iMax );
   //! convert multibyte to UTF8
   //static void wchar_t_to_UTF8( const char* pbszMB, char* pbszUTF8, unsigned uMaxLength );
   //! convert unicode to UTF8
   static void wchar_t_to_UTF8( const wchar_t* pwszWC, char* pbszUTF8, unsigned uMaxLength, const wchar_t* pwszEnd = NULL );
   //! convert multibyte to UTF8
   static void wchar_t_to_UTF8( const char* pbszMB, char* pbszUTF8, unsigned uMaxLength, const char* pbszEnd = NULL );
   //! convert UTF8 to unicode
   static HRESULT UTF8_to_wchar_t( const char* pbszUTF8, wchar_t* pwszWC, unsigned uMaxLength, unsigned* puCount, char const** ppbszEnd );
   //! convert UTF8 to multibyte
   static HRESULT UTF8_to_wchar_t( const char* pbszUTF8, char* pbszMB, unsigned uMaxLength, unsigned* puCount, char const** ppbszEnd );
   //! check UTF-8 string if valid. If not valid then return position to character that didn't 
   //! meet the conditions for UTF-8 rules.
   static unsigned CheckUTF8( const char* pbszUTF8, unsigned uLength, bool* pbOk );

private:
   void _Safe();
   void _AllocAdd( int iSize );
   void _Delete();


public:

// nested
private:
   struct _Data
   {
      ~_Data() {
		//if( m_iRefs == 0 ) MP_DELETE_THIS; }
		  if( m_iRefs == 0 )
		  {
			  BYTE *pData = (BYTE*)this;
			  MP_DELETE_ARR( pData); 
		  }
	  }

      int GetRefs() {
         return m_iRefs; }
      int GetLength() {
         return m_iLength; }
      int GetMaxLength() {
         return m_iMaxLength; }
      char* GetText() {
         return (char*)( this + 1 ); }

      void SetLength( int iLength ) {
         m_iLength = iLength; }

      void AddLength( int iLength ) {
         m_iLength += iLength; }

      void AddRef() {
         m_iRefs++; }
      void Release() {
         _ASSERT( m_iRefs > 0 );
         m_iRefs--;
		 //if( m_iRefs == 0 ) MP_DELETE_THIS; }
		if( m_iRefs == 0 )
		{
			BYTE *pData = (BYTE*)this;
			MP_DELETE_ARR( pData); 
		}
	  }

      char* End() {
         return &((char*)(this+1))[m_iLength]; }

      _Data* Safe();

      int m_iRefs;
      int m_iLength;
      int m_iMaxLength;
   };

// Attributes
protected:
   _Data* m_pData;
#ifdef _DEBUG
   char* m_psz;
#endif

   static unsigned int pdata_nil[];
   static int string_m_iTinyAlloc;
   static int string_m_iSmallAlloc;
   static int string_m_iBigAlloc;
};

__forceinline unsigned CUTF8String::GetLength() {
   return m_pData->GetLength(); }

__forceinline char* CUTF8String::GetUTF8Text() {
   return m_pData->GetText(); }

__forceinline int CUTF8String::Find( char chCharacter ) const {
   wchar_t pszText[2]; pszText[0] = chCharacter; pszText[1] = 0;
   return Find( pszText, 0 );
}

__forceinline int CUTF8String::Find( char chCharacter, unsigned uStart ) const {
   wchar_t pszText[2]; pszText[0] = chCharacter; pszText[1] = 0;
   return Find( pszText, uStart );
}

__forceinline int CUTF8String::Find( const char* pszText ) const {
   return Find( pszText, 0 );
}

#ifdef _WCHAR_T_DEFINED
__forceinline int CUTF8String::Find( wchar_t chCharacter ) const {
   wchar_t pszText[2]; pszText[0] = chCharacter; pszText[1] = 0;
   return Find( pszText, 0 );
}

__forceinline int CUTF8String::Find( wchar_t chCharacter, unsigned uStart ) const {
   wchar_t pszText[2]; pszText[0] = chCharacter; pszText[1] = 0;
   return Find( pszText, uStart );
}

__forceinline int CUTF8String::Find( const wchar_t* pszText ) const {
   return Find( pszText, 0 );
}
#else
__forceinline int CUTF8String::Find( unsigned short chCharacter ) const {
   unsigned short pszText[2]; pszText[0] = chCharacter; pszText[1] = 0;
   return Find( pszText, 0 );
}

__forceinline int CUTF8String::Find( unsigned short chCharacter, unsigned uStart ) const {
   unsigned short pszText[2]; pszText[0] = chCharacter; pszText[1] = 0;
   return Find( pszText, uStart );
}

__forceinline int CUTF8String::Find( const unsigned short* pszText ) const {
   return Find( pszText, 0 );
}
#endif

__forceinline bool operator==( const CUTF8String& s, const char* pbszText ) {
   return s.operator==( CUTF8String( pbszText ) ); }
__forceinline bool operator==( const char* pbszText, const CUTF8String& s ) {
   return s.operator==( CUTF8String( pbszText ) ); }

__forceinline bool operator!=( const CUTF8String& s, const char* pbszText ) {
   return !(s.operator==( CUTF8String( pbszText ) )); }
__forceinline bool operator!=( const char* pbszText, const CUTF8String& s ) {
   return !(s.operator==( CUTF8String( pbszText ) )); }

#ifdef _WCHAR_T_DEFINED
__forceinline bool operator==( const CUTF8String& s, const wchar_t* pbszText ) {
   return s.operator==( CUTF8String( pbszText ) ); }
__forceinline bool operator==( const wchar_t* pbszText, const CUTF8String& s ) {
   return s.operator==( CUTF8String( pbszText ) ); }

__forceinline bool operator!=( const CUTF8String& s, const wchar_t* pbszText ) {
   return !(s.operator==( CUTF8String( pbszText ) )); }
__forceinline bool operator!=( const wchar_t* pbszText, const CUTF8String& s ) {
   return !(s.operator==( CUTF8String( pbszText ) )); }
#else
__forceinline bool operator==( const CUTF8String& s, const unsigned short* pbszText ) {
   return s.operator==( CUTF8String( pbszText ) ); }
__forceinline bool operator==( const unsigned short* pbszText, const CUTF8String& s ) {
   return s.operator==( CUTF8String( pbszText ) ); }

__forceinline bool operator!=( const CUTF8String& s, const unsigned short* pbszText ) {
   return !(s.operator==( CUTF8String( pbszText ) )); }
__forceinline bool operator!=( const unsigned short* pbszText, const CUTF8String& s ) {
   return !(s.operator==( CUTF8String( pbszText ) )); }
#endif

__forceinline CUTF8String operator+( CUTF8String& s, char* pbszText ) {
   CUTF8String sR( s ); sR.Add( pbszText ); return sR; }
#ifdef _WCHAR_T_DEFINED
__forceinline CUTF8String operator+( CUTF8String& s, wchar_t* pwszText ) {
#else
__forceinline CUTF8String operator+( CUTF8String& s, unsigned short* pwszText ) {
#endif
   CUTF8String sR( s ); sR.Add( pwszText ); return sR; }


}; // gd

#endif // !defined(AFX_UTF8STRING_H__E3392447_138A_446A_92E3_6E09FD04025E__INCLUDED_)

// www    -> www.gorep.se
// e-mail -> per@gorep.se
// 
// Author -> Per Ghosh (e-mail: per@gorep.se)
//
// This code is free for personal and commercial use, providing this 
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
//	This file is provided "as is" with no expressed or implied warranty.
//	The author accepts no liability for any damage/loss of business that
//	this product may cause.



#if !defined(AFX_STRING_H__A6EA7C25_3255_438B_B9CE_547EC2EA8630__INCLUDED_)
#define AFX_STRING_H__A6EA7C25_3255_438B_B9CE_547EC2EA8630__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _GD_STRING_H_
#define _GD_STRING_H_
#endif


#ifndef _ASSERT
   #include <assert.h>
   #define _ASSERT assert
#endif

#ifndef _TCHAR_DEFINED
	#pragma warning( disable : 4267 4244 )
	#include <cwchar>
	#include <memory>
	#include <cstdarg>
	#include <cctype>
	#include <clocale>
	typedef unsigned long       DWORD;
	typedef int                 BOOL;
	typedef unsigned char       BYTE;
	typedef unsigned short      WORD;
	typedef float               FLOAT;
	typedef void	            *LPVOID;
	typedef int                 INT;
	typedef unsigned int        UINT;

	typedef const char	*LPCSTR;
	typedef const wchar_t	*LPCWSTR;
	typedef long HRESULT;
//	#define _T(x)      L ## x
	#define FALSE   0
	#define TRUE    1
	#define NULL    0
#endif


/*! \file 
    \brief has functionality for the gd::CString class. 

  This class is very similar to the CString that exist in MFC. If you are using
  CString from MFC then you need to include all of MFC. This is not allways good.
  The gd::CString works by it self and don't need other for it's use.
*/


/*! \brief general classes 

gd or all namespaces that start with gd. That is ojbects and functionality that is created
to be flexible and could be used in many situations. All of these classes need special 
consideration when changes are done. Code depends on the code that exist in the gd area.
If one function is removed then it is very likley that there will be errors in code
that depends on the things that has been changed. If you are adding functionality then there
is no problem. Does you need some new functionality and this could be placed in the gd area
for use by other code. Then you should do so. But it has to be general and safe.
*/
namespace gd
{

/*! \class CString
\brief CString handles one string. It has functionality to work with this string in numreous ways.

%CString created to be fast and not consume more memory that needed. In order to make it fast,
memory allocations has been minimized. Each new memoryallocation takes time. String will be
copied to new location and also the allocation itself takes time.<br>
Also it uses references. Many string could refer to ont buffer. Copy and assignment will not
create new strings. They just add's reference to the string they are assigned to.
One empty string will only hold a pointer. 

FAQ
Q: How can I assign a value to a string object<br>
A: EXAMPLE: 
\code
CString s;
s = "Text";
s.Set( "Text" );
CString s1( "Text" ); 
\endcode

Q: How do I find the character "a" in string object<br>
A: EXAMPLE:  
\code
CString s("abcdefgh");
int iPos = s.Find('a');
if( iPos != -1 ) { ... } 
\endcode

Q: How do I find the text "cd" in string object<br>
A: EXAMPLE:  
\code
CString s("abcdefgh");
int iPos = s.Find("cd");
if( iPos != -1 ) { ... } 
\endcode

Q: I want to create a new string object from the character
   "x" in a existing string. How do I do that.<br>
A: EXAMPLE:  
\code
CString s("0123456xHi !");
int iPos = s.Find('x');
if( iPos != -1 )
{
   CString sPart = s.Mid( iPos );
   ....
} 
\endcode

Q: If i need to remove trailing spaces from string<br>
A: EXAMPLE: 
\code
CString s("123   ");
s.Trim( " ", gd::CString::eRight ); 
\endcode

Q: I want to remove a word from string<br>
A: EXAMPLE 
\code
CString s("One Two Three");
int i = s.Find( "Two" );
if( i != -1 )
{
   s.Cut( i, 3 );
} 
\endcode

Q: I need to print the string with the standard C function printf. <br>
A: EXAMPLE 
\code
CString s("print this");
printf("%s",(const char*)s); 
\endcode

Q: If a text should be inserted a number of times. how is that done<br>
A: EXAMPLE 
\code
CString s("0123456789");
s.Insert( 3, "< >", 2 );
// string is now "012< >< >3456789" 
\endcode

Q: I want to work with the internal buffer, how do I get it.<br>
A: If you are going to modify the internal string buffer without using 
   memberfunctions. Then you need to call ReleaseBuffer() when you are done.<br>
   EXAMPLE \code
CString s;
char* pszText = s.GetBuffer( 10 );
strcpy( pszText, "012345678" );
s.ReleaseBuffer(); \endcode

*/
class CString  
{
public:
   enum Align
   {
      eLeft = -1,
      eMiddle = 0,
      eRight = 1,
   };

public:
   CString() {
      m_pData = (CString::_Data*)pdata_nil; }
   CString( const wchar_t* pszText, int iLength ) {
      m_pData = (CString::_Data*)pdata_nil;
      Set( pszText, iLength ); }
   CString( const char* pbszText ) {
      m_pData = (CString::_Data*)pdata_nil;
      Set( pbszText ); }
#ifdef _WCHAR_T_DEFINED
   CString( const wchar_t* pwszText ) {
#else
   CString( const unsigned short* pwszText ) {
#endif
      m_pData = (CString::_Data*)pdata_nil;
      Set( pwszText ); }
   CString( char ch ) {
      m_pData = (CString::_Data*)pdata_nil;
      Add( ch ); }
   CString( unsigned short ch ) {
      m_pData = (CString::_Data*)pdata_nil;
      Add( ch ); }

   CString( const CString& other );
   CString& operator=( const CString& other );
	~CString();

// Operator
public:
   //! returns const pointer to the text buffer
   operator LPCWSTR() const throw() {
      return m_pData->GetText(); }
   wchar_t& operator[](int iIndex) const throw() {
      _ASSERT( m_pData->GetLength() > iIndex );
      return m_pData->GetText()[iIndex]; }
   wchar_t& operator[](unsigned uIndex) const throw() {
      _ASSERT( (unsigned)m_pData->GetLength() > uIndex );
      return m_pData->GetText()[uIndex]; }
   //! assign to widechar
#ifdef _WCHAR_T_DEFINED
   CString& operator=(wchar_t* pwsz) {
#else
   CString& operator=(unsigned short* pwsz) {
#endif
      Set( pwsz ); return *this; }
   //! add multibyte character
   CString& operator+=(char chbCharacter) throw() {
      Add( chbCharacter ); return *this; }
   //! add widechar
#ifdef _WCHAR_T_DEFINED
   CString& operator+=(wchar_t chwCharacter) throw() {
#else
   CString& operator+=(unsigned short chwCharacter) throw() {
#endif
      Add( chwCharacter ); return *this; }
   //! add multibyte text
   CString& operator+=(const char* pbszText) throw() {
      Add( pbszText ); return *this; }
   //! add unicode text
#ifdef _WCHAR_T_DEFINED
   CString& operator+=(const wchar_t* pwszText) throw() {
#else
   CString& operator+=(const unsigned short* pwszText) throw() {
#endif
      Add( pwszText ); return *this; }
   //! Add string object
   CString& operator+=(const CString& s) throw() {
      Add( s ); return *this; }

   CString& operator<<( char chbCharacter ) throw() {
      Add( chbCharacter ); return *this; }
#ifdef _WCHAR_T_DEFINED
   CString& operator<<( wchar_t wchCharacter ) throw() {
#else
   CString& operator<<( unsigned short wchCharacter ) throw() {
#endif
      Add( wchCharacter ); return *this; }
   CString& operator<<( const char* pbszText ) throw() {
      Add( pbszText ); return *this; }
#ifdef _WCHAR_T_DEFINED
   CString& operator<<( const wchar_t* pwszText ) throw() {
#else
   CString& operator<<( const unsigned short* pwszText ) throw() {
#endif
      Add( pwszText ); return *this; }
   CString& operator<<( const CString& s ) throw() {
      Add( s ); return *this; }
   CString& operator<<( int iNumber ) throw() {
      Add( iNumber ); return *this; }
   CString& operator<<( double dNumber ) throw() {
      Add( dNumber ); return *this; }

   //! is string less than the string compared to
   bool operator<(const CString& other) const {
      return (wcscmp( m_pData->GetText(), other.m_pData->GetText() ) < 0 ); } 
   //! is string greater than the string compared to
   bool operator>(const CString& other) const {
      return (wcscmp( m_pData->GetText(), other.m_pData->GetText() ) > 0 ); } 

// get/set
public:
   //! return length for string
   int GetLength() const {
      return m_pData->GetLength(); }
   //! set length for string (make sure that you don't set the length to more than internal buffer 
   //! length)
   void SetLength( unsigned uLength );
   //! return length for internal buffer
   int GetBufferLength() const {
      return m_pData->GetMaxLength(); }
   wchar_t* GetBuffer() const {
      return m_pData->GetText(); }
   // WARNIG! To set these allocationbuffers you change all stringobjects.
   // These members that handle when string is realocated are static 
   static void SetTinyReallocSize( int iSize );
   static void SetSmallReallocSize( int iSize );
   static void SetBigReallocSize( int iSize );

// operation
public:
   // * ASK
   bool IsLocked();
   bool IsEmpty() const; //!< return true if string is empty or false if it contains characters

   // * GET
   //! return pointer to offset in internal textbuffer. Remember to call ReleaseBuffer() if you 
   //! change it.
   const wchar_t* GetText( int Index ) const throw();  
   wchar_t* GetBufferSetLength( int iLength );
   int GetRefs() const throw(); //!< Return number of references to text
   //! return integer value for string
   int GetInt() const throw();

   // * SET
   void Set( const char* pbszText );
#ifdef _WCHAR_T_DEFINED
   void Set( const wchar_t* pwszText );
#else
   void Set( const unsigned short* pwszText );
#endif 
   void Set( const char* pbszText, int iLength );
   //! Set string to specified lenght from wide character buffer
#ifdef _WCHAR_T_DEFINED
   void Set( const wchar_t* pwszText, int iLength );
#else
   void Set( const unsigned short* pwszText, int iLength );
#endif 
   void Format( const wchar_t* pszFormat, ... );
   CString& FormatAdd( const wchar_t* pszFormat, ... );

   // * ADD
   //! Add integer value to string, the integer value is converted to text before it is added
   void Add( int iValue );
   //! Add double value to string, the integer double is converted to text before it is added
   void Add( double dValue );
   //! Add character to string (multibyte version)
   void Add( char chbCharacter );
   //! Add character to string (unicode version)
#ifdef _WCHAR_T_DEFINED
   void Add( wchar_t chwCharacter );
#else
   void Add( unsigned short chwCharacter );
#endif 
   //! Add multibyte text to string
   void Add( const char* pbszText );
   //! Add unicode text to string
#ifdef _WCHAR_T_DEFINED
   void Add( const wchar_t* pwszText );
#else
   void Add( const unsigned short* pwszText );
#endif 
   void Add( const char* pbszText, int iLenght );
   //! Add specified lenght of characters from wide character buffer
#ifdef _WCHAR_T_DEFINED
   void Add( const wchar_t* pwszText, int iLenght );
#else
   void Add( const unsigned short* pwszText, int iLenght );
#endif 
   void Add( const CString& s );
   //! This adds one character to string. It works fast because no checking is done if the string
   //! has more than one reference. In order to use this function you need to make sure that string 
   //! only hase one reference and this is done by the member function Single() .
   void FastAdd( wchar_t chCharacter ); 
   void FastAdd( const wchar_t* pszText ); 
   void FastAdd( const CString* pstring ); 
   void RawAdd( wchar_t chCharacter );
   //! add string (multibyte) with specified lenght and return string object
   CString& StreamAdd( const char* pbszText, int iLenght );
   //! add string (unicode) with specified lenght and return string object
#ifdef _WCHAR_T_DEFINED
   CString& StreamAdd( const wchar_t* pwszText, int iLenght );
#else
   CString& StreamAdd( const unsigned short* pwszText, int iLenght );
#endif 
   //! add string return string object
   CString& StreamAdd( const CString& s );
   //!
   void Insert( int iPosition, const wchar_t* pszText, int iRepeat = 1 );

   // * COMPARE
   //! compare text if it is equal to internal string
   bool CompareNoCase( const wchar_t* pszText );
   //! Compare from start of string. If string start is equal to text than this returns true.
   //! No lenght checking is done, which menas that the shortest string decide lenght of text
   //! that is compared. <b>Check for empty strings !! these will allways return true</b>
   bool CompareStart( const wchar_t* pszText );               // compare text from start 
   //! compare string from start (ignore case)
   bool CompareStartNoCase( const wchar_t* pszText );         
   bool CompareEnd( const wchar_t* pszText );                 // compare text from end
   //! compare string from end (ignore case)
   bool CompareEndNoCase( const wchar_t* pszText );         

   // * MODIFY
   void Trim( const wchar_t* pszCharacters, Align align = eRight );
   void Cut( int iStart, int iEnd );
   void MakeLower();    //!< convert characters that are uppercase to lowercase
   void MakeUpper();    //!< convert characters  that are lowercase to uppercase
   //! Replace specified text in buffer wiht new one (all that is found is replaced)
   void Replace( const wchar_t* pszOld, const wchar_t* pszNew, int iCount = -1 );
   void StripNonAlnumCharacter();   //!< Remove all non characters, number or underscore
   void Strip( const wchar_t* psz );  //!< Remove all characters in buffer sent to funtion

   // implement these
   void AlignText( const wchar_t* pszCharacters, Align align = eLeft ) ;

   // * EXTRACT
   CString Left( int iCount ) const;   //!< return text with specified length starting from left
   CString Right( int iCount ) const;  //!< return text with specified length starting from right
   //! Return a portion of the string. Start from index iStart and the length iLength. If iLength 
   //! is not specified or the length of is longer that string, then the string that is returned
   //! stops at internal end. 
   CString Mid( int iStart, int iLength = -1 ) const; 
   //! Same as Mid( iStart, iLenght ) but this version stops when stopstring has been found or
   //! string has ended.
   CString Mid( int iStart, const wchar_t* pszStop ) const; 
   //! Same type as Mid( iStart, iLenght ) but this finds the pszStart string and copies characters 
   //! after to pszStop is found or string is ended.
   CString Mid( const wchar_t* pszStart, const wchar_t* pszStop ) const; 

   // * FIND
   //! Finds position where characters is located from start. returns zero-based index for the 
   //! character or -1 if not found.
   int Find( wchar_t chCharacter ) const;
   //! Finds position where characters is located beging to search from iStart. returns zero 
   //! base index for the character or -1 if not found.
   int Find( wchar_t chCharacter, int iStart ) const;
   //! Find text in string and return zero-based index for first textitem that is found. If not
   //! found then -1 is returned
   int Find( const wchar_t* pszText ) const;
   int Find( const wchar_t* pszText, int iStart ) const;
   //! Find number of text sections in string and return index to last section found
   int FindCount( const wchar_t* pszText, unsigned uCount ) const;
   //! Search character from end. If found then return zerobased index for the charactersposition
   //! from start of string buffer. If not found then -1 is returned.
   int ReverseFind( wchar_t chCharacter ) const;
   int ReverseFind( const wchar_t* pszText ) const;

   // * COUNT
   //! count how many specified text items that is found in string
   unsigned Count( const wchar_t* pszText ) const;            

   // * BUFFER
   //! Return pointer to internal string buffer and set the minimal buffer size. If buffer is 
   //! modified, remember to call ReleaseBuffer()
   wchar_t* GetBuffer( int iMinLength );
   //! Return pointer to end of the internal string buffer. It also makes sure that buffer string
   //! could add specified number of characters more that current. use this function if text is
   //! added to buffer and not by a CString member function. remember to call ReleaseBuffer()
   wchar_t* GetBufferEnd( int iAddLength );
   //! return character at specified position
   wchar_t  GetAt( unsigned uIndex );

   // * UPDATE
   //! Recalculate the internal length
   void ReleaseBuffer();
   //! calculate the length from end 
   void ReleaseBufferFromEnd();                             

   // * STRIP

   // * REMOVE
   void Empty();
   void FreeExtra();
   void Kill();

   // * PROTECT
   void Single();    //!< Make sure text only has one reference to it
   void LockBuffer();
   void UnlockBuffer();

#ifdef WIN32
   // * WIN32
//   BSTR AllocSysString() const;  //!< allocate BSTR and fill it with stringvalue (remember to  MP_DELETE( BSTR)
   //! Load string from resource
//   void LoadString( HINSTANCE hInstance, unsigned uId );
   //! Load string from resource and format
//   void LoadStringFormat( HINSTANCE hInstance, unsigned uId, ... );
#endif

   // WARNING !
   // If you are going to use these functions make sure you know what is happening
   void SetExternalBuffer( wchar_t* pszBuffer, int iLength );

private:
   void _F( const wchar_t* pszFormat, va_list arguments );
   void _Safe();
   void _AllocAdd( int iSize );
   void _Delete();
   

// nested
private:
   struct _Data
   {
      ~_Data() 
	  {
         if( m_iRefs == 0 )
		 {
			 BYTE *pData = (BYTE*)this;
			 MP_DELETE_ARR( pData); 
		 }
	  }
		 //if( m_iRefs == 0 ) MP_DELETE_THIS; }

      int GetRefs() {
         return m_iRefs; }
      int GetLength() {
         return m_iLength; }
      int GetMaxLength() {
         return m_iMaxLength; }
      wchar_t* GetText() {
         return (wchar_t*)( this + 1 ); }

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

      wchar_t* End() {
         return &((wchar_t*)(this+1))[m_iLength]; }

      _Data* Safe();

      int m_iRefs;
      int m_iLength;
      int m_iMaxLength;
   };

// attributes
private:
   _Data* m_pData;
#ifdef _DEBUG
   wchar_t* m_psz;
#endif

   static unsigned int pdata_nil[];
   static int string_m_iTinyAlloc;
   static int string_m_iSmallAlloc;
   static int string_m_iBigAlloc;
};

__forceinline void CString::SetLength( unsigned uLength ) {
   _ASSERT( (unsigned)m_pData->GetMaxLength() > uLength );
   m_pData->SetLength( uLength );
   m_pData->GetText()[uLength] = L'\0';
}

__forceinline bool CString::IsLocked() {
   return (m_pData->m_iRefs == -2); }

__forceinline bool CString::IsEmpty() const {
   return (m_pData->GetLength() == 0); }

__forceinline int CString::Find( wchar_t chCharacter ) const {
   wchar_t pszText[2]; pszText[0] = chCharacter; pszText[1] = 0;
   return Find( pszText, 0 );
}

__forceinline int CString::Find( wchar_t chCharacter, int iStart ) const {
   wchar_t pszText[2]; pszText[0] = chCharacter; pszText[1] = 0;
   return Find( pszText, iStart );
}

__forceinline int CString::Find( const wchar_t* pszText ) const {
   return Find( pszText, 0 );
}

__forceinline bool operator==( const CString& s, const wchar_t* pszText ) {
   return wcscmp( s, pszText ) == 0; }
__forceinline bool operator==( const wchar_t* pszText, const CString& s ) {
   return wcscmp( s, pszText ) == 0; }
__forceinline bool operator==( const CString& s1, const CString& s2 ) {
   return wcscmp( s1, s2 ) == 0; }
__forceinline bool operator!=( const CString& s, const wchar_t* pszText ) {
   return wcscmp( s, pszText ) != 0; }
__forceinline bool operator!=( const wchar_t* pszText, const CString& s ) {
   return wcscmp( s, pszText ) != 0; }
__forceinline bool operator!=( const CString& s1, const CString& s2 ) {
   return wcscmp( s1, s2 ) != 0; }

__forceinline CString operator+( CString& s, char* pbszText ) {
   CString sR( s ); sR.Add( pbszText ); return sR; }
#ifdef _WCHAR_T_DEFINED
__forceinline CString operator+( CString& s, wchar_t* pwszText ) {
#else
__forceinline CString operator+( CString& s, unsigned short* pwszText ) {
#endif
   CString sR( s ); sR.Add( pwszText ); return sR; }

__forceinline CString operator+( const char* pbszText, CString& s ) {
   CString sNew( pbszText ); sNew += s; return sNew; }

__forceinline const wchar_t* CString::GetText( int iIndex ) const {
   _ASSERT( iIndex < m_pData->GetLength() );
   return &m_pData->GetText()[iIndex]; }

__forceinline int CString::GetRefs() const {
   return m_pData->GetRefs(); }

__forceinline int CString::GetInt() const {
   return _wtoi( m_pData->GetText() ); 
}

__forceinline void CString::FastAdd( wchar_t chCharacter ) {
   _AllocAdd( 1 );
   m_pData->End()[0] = (wchar_t)chCharacter;
   m_pData->End()[1] = 0;
   m_pData->m_iLength++;
}

__forceinline void CString::FastAdd( const wchar_t* pszText ) {
   int iLength = wcslen( pszText );
   _AllocAdd( iLength );
   memcpy( m_pData->End(), pszText, (iLength + 1) * sizeof(wchar_t) );
   m_pData->m_iLength += iLength;
}

__forceinline void CString::FastAdd( const CString* pstring ) {
   unsigned uLength = (unsigned)pstring->GetLength() + 1;
   if( (unsigned)m_pData->GetMaxLength() < (m_pData->GetLength() + uLength) ) {
      _AllocAdd( pstring->GetLength() );
   }
   memcpy( m_pData->End(), pstring->GetBuffer(), uLength * sizeof(wchar_t) );
   m_pData->m_iLength += (uLength - 1);
}


__forceinline void CString::RawAdd( wchar_t chCharacter ) {
   _ASSERT( m_pData->GetRefs() == 1 );
   wchar_t* psz = m_pData->End(); *psz = chCharacter; psz[1] = 0; m_pData->AddLength( 1 ); }

/*! compare if string is equal to internal string */
__forceinline bool CString::CompareNoCase( const wchar_t* pszText ) {
   return (_wcsicmp( m_pData->GetText(), pszText ) == 0); 
}

__forceinline bool CString::CompareStart( const wchar_t* pszText ) {
   unsigned uLength = wcslen( pszText );
   if( m_pData->GetLength() < (int)uLength ) uLength = m_pData->GetLength();
   return (memcmp(m_pData->GetText(), pszText, uLength * sizeof(wchar_t) ) == 0 ); }

//! get character at specified position (you are responsible to make sure that it isn't reading outside internal buffer)
__forceinline wchar_t CString::GetAt( unsigned uIndex ) {
   _ASSERT( uIndex <= (unsigned)m_pData->GetLength() );
   return m_pData->GetText()[uIndex];
}

//! set tiny allocation buffer. see CString::SetBigReallocSize
__forceinline void CString::SetTinyReallocSize( int iSize ) {
   _ASSERT( iSize < CString::string_m_iSmallAlloc ); // have to be smaller then big reallocation
   CString::string_m_iTinyAlloc = iSize;
}

//! set small allocation buffer. see CString::SetBigReallocSize
__forceinline void CString::SetSmallReallocSize( int iSize ) {
   _ASSERT( iSize < CString::string_m_iBigAlloc ); // have to be smaller then big reallocation
   CString::string_m_iSmallAlloc = iSize;
}
//! set big allocation buffer. 
/*! %SetTinyReallocSize, %SetSmallReallocSize and %SetBigReallocSize sets the internal allocation
    logic for CString. They are static which means that they work on all string objects for that 
    application. By modifying those you can optimize the string in order to match you application.
*/
__forceinline void CString::SetBigReallocSize( int iSize ) {
   _ASSERT( iSize > CString::string_m_iSmallAlloc ); // have to be larger than small allocation size
   CString::string_m_iBigAlloc = iSize;
}

__forceinline void CString::Single() {
   _Safe();
}

/*
inline CString operator+( CString& s, const unsigned short* pwszText ) {
   s.Add( pwszText ); return s; }
   */
}; // namespace gd

#endif // !defined(AFX_STRING_H__A6EA7C25_3255_438B_B9CE_547EC2EA8630__INCLUDED_)

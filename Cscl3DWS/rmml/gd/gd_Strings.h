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

#if !defined(AFX_GD_STRINGS_H__3DA104AB_3F3E_42B7_A787_C6A41C61EF35__INCLUDED_)
#define AFX_GD_STRINGS_H__3DA104AB_3F3E_42B7_A787_C6A41C61EF35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _GD_STRINGS_H_
#define _GD_STRINGS_H_


#ifndef _ASSERT
   #include <assert.h>
   #define _ASSERT assert
#endif


#ifndef _TCHAR_DEFINED
	#include <cwchar>
	#include <cstdarg>
	#include <string>
	typedef unsigned long       DWORD;
	typedef int                 BOOL;
	typedef unsigned char       BYTE;
	typedef unsigned short      WORD;
	typedef float               FLOAT;
	typedef void	            *LPVOID;
	typedef int                 INT;
	typedef unsigned int        UINT;
	typedef long HRESULT;
	#define FALSE   0
	#define TRUE    1
	#define NULL    0
#endif

namespace gd
{

/** —троки, размещаемые в одном длинном массиве wchar_t-ов 
 * 
 */
class CStrings  
{

public:
   CStrings() {
      m_pData = (_Data*)pdata_nil; }
   CStrings( const wchar_t* pszString ) {
      m_pData = (CStrings::_Data*)pdata_nil;
      Add( pszString ); }
   CStrings( const CStrings& other );
   CStrings& operator=( const CStrings& other );
	~CStrings();

// get/set
public:
   // return number of strings in strings object
   UINT GetCount() const;

// Operations
public:
   //! Add text to strings object
   void  Add( const wchar_t* pszText );
   //! Add text to strings object that has specified length
   void  Add( const wchar_t* pszText, UINT uLength );
   //! return string for specified index
   const wchar_t* GetString( UINT uIndex ) const;
   //! Find index for specified string
   int   Find( const wchar_t* pszText ) const;
   //! Find index for specified string
   int   Find( const wchar_t* pszText, UINT uLength ) const;
   //! return pointer to next string
   const wchar_t* Next( const wchar_t* pszString ) const;
   //! Add string 
   void  Push( const wchar_t* pszText );
   //! Get the last string and remove it
   const wchar_t* Pop();
   //
   void Empty();

private:
   BYTE* _Next( BYTE* pszData ) const;
   wchar_t* _Find( BYTE* pszData, int iCount ) const;
   void _Safe();
   void _AllocAdd( int iSize );
   void _Delete();


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
      int GetCount() {
         return m_iCount; }
      wchar_t* GetText() {
         return (wchar_t*)( this + 1 ) + sizeof(long); }
      BYTE* GetData() {
         return (BYTE*)( this + 1 ); }


      void SetLength( int iLength ) {
         m_iLength = iLength; }
      void SetCount( int iCount ) {
         m_iCount = iCount; }

      void AddLength( int iLength ) {
         m_iLength += iLength; }

      void AddTextLength( long lLength ) {
         *(long*)End() = lLength;
         m_iLength += sizeof(long); }

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

      BYTE* End() {
         return &((BYTE*)(this+1))[m_iLength]; }

      _Data* Safe();

      int m_iRefs;
      int m_iLength;
      int m_iMaxLength;
      int m_iCount;
   };

// attributes
private:
   _Data* m_pData;

   static unsigned int pdata_nil[];
   static int strings_m_iTinyAlloc;
   static int strings_m_iSmallAlloc;
   static int strings_m_iBigAlloc;
};

inline UINT CStrings::GetCount() const {
   return m_pData->GetCount(); }

inline BYTE* CStrings::_Next( BYTE* pbPosition ) const {
   _ASSERT( pbPosition < m_pData->End() );
   return pbPosition + (*(long*)pbPosition * sizeof(wchar_t)) + sizeof(long) + sizeof(wchar_t); }

inline void CStrings::Push( const wchar_t* pszText ) {
   Add( pszText ); }


}; // namespace gd
   
#endif // _GD_STRINGS_H_

#endif // !defined(AFX_GD_STRINGS_H__3DA104AB_3F3E_42B7_A787_C6A41C61EF35__INCLUDED_)

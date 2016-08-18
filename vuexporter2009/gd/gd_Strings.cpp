// Copyright (C) Gorep Development
// www    -> www.gorep.se
// e-mail -> info@gorep.se
// 
// Author -> Per Ghosh (e-mail: per@gorep.se)
//
// This code is free to use without modification

#include "stdafx.h"
#include "gd_Strings.h"

#ifdef _DEBUG
   #undef THIS_FILE
   static char THIS_FILE[]=__FILE__;
   #ifdef DEBUG_NEW
      #define new DEBUG_NEW
   #endif
#endif

#define _NOTHING (CStrings::_Data*)pdata_nil
#define _SINGLE _Safe()


namespace gd
{

unsigned int CStrings::pdata_nil[] = { -1, 0, 0, 0, NULL };
int CStrings::strings_m_iTinyAlloc = 128;
int CStrings::strings_m_iSmallAlloc = 1024;
int CStrings::strings_m_iBigAlloc = 4096;

//*************************************************************************************************
CStrings::CStrings( const CStrings& other )
{
   _ASSERT( other.m_pData->GetRefs() != -2 );
   
   if( other.m_pData == _NOTHING ) m_pData = _NOTHING;
   else
   {
      m_pData = other.m_pData;
      m_pData->AddRef();
   }
}

//*************************************************************************************************
CStrings& CStrings::operator=( const CStrings& other )
{
   _ASSERT( other.m_pData->GetRefs() != -2 );

   if( this == &other ) return *this;
   
   if( other.m_pData == _NOTHING ) m_pData = _NOTHING;
   else
   {
      if( m_pData != _NOTHING ) m_pData->Release();
      m_pData = other.m_pData;
      m_pData->AddRef();
   }

   return *this;
}

CStrings::~CStrings()
{
   _Delete();
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Add text to strings object */
void CStrings::Add(
   const TCHAR* pszText //
   )
{
   _SINGLE;

   long lLength;

   lLength = lstrlen( pszText );
   _AllocAdd( lLength );
   m_pData->AddTextLength( lLength );

   lLength++;
   lLength *= sizeof(TCHAR);
   memcpy( m_pData->End(), pszText, lLength );
   m_pData->AddLength( lLength );
   m_pData->m_iCount++;
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Add text to strings object */
void CStrings::Add(
   const TCHAR* pszText, // text that is going to be added to strings object
   UINT uLength // length for string that is added
   )
{
   _SINGLE;

   long lLength;

   _AllocAdd( uLength );

   lLength = uLength;

   m_pData->AddTextLength( lLength );
   lLength++;
   lLength *= sizeof(TCHAR);
   memcpy( m_pData->End(), pszText, lLength );
   m_pData->End()[uLength * sizeof(TCHAR)] = 0;
   m_pData->AddLength( lLength );
   m_pData->m_iCount++;
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Return string for specified index
RETURN:  Pointer to string for specified index*/
const TCHAR* CStrings::GetString(
   UINT uIndex //
   ) const
{
   _ASSERT( uIndex < (UINT)m_pData->GetCount() );
   return _Find( m_pData->GetData(), uIndex );
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Find string in strings object
RETURN:  index for string or -1 if string was not found */
int CStrings::Find(
   const TCHAR* pszText //
   ) const
{
   int iIndex;
   BYTE* pbCompare;

   pbCompare = m_pData->GetData();
   for( iIndex = 0; iIndex < m_pData->GetCount(); iIndex++ )
   {
      if( lstrcmp( (TCHAR*)(pbCompare + sizeof(long)), pszText ) == 0 )
      {
         return iIndex;
      }
      pbCompare = _Next( pbCompare );
   }

   return -1;
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Find string in strings object
RETURN:  index for string or -1 if string was not found */
int CStrings::Find(
   const TCHAR* pszText, // Text that is searched for
   UINT uLength // length for text
   ) const
{
   int iIndex;
   BYTE* pbCompare;

   pbCompare = m_pData->GetData();
   for( iIndex = 0; iIndex < m_pData->GetCount(); iIndex++ )
   {
      if( (*(long*)pbCompare == (long)uLength) && (_tcsncmp( (TCHAR*)(pbCompare + sizeof(long)), pszText, uLength ) == 0) )
      {
         return iIndex;
      }
      pbCompare = _Next( pbCompare );
   }

   return -1;
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Get string after current string that is sent to function. If pointer sent to function is
         NULL then return the first string. If pointer sent to function points at the last string
         in strings object then return NULL.
RETURN:  Pointer to next string i line, NULL if no more strings*/
const TCHAR* CStrings::Next(
   const TCHAR* pszString //
   ) const
{
   _ASSERT( pszString == NULL || pszString >= (TCHAR*)(m_pData->GetData() + sizeof(long)) );
   _ASSERT( pszString < (TCHAR*)m_pData->End() );

   BYTE* pbPosition;

   if( pszString != NULL )
   {
      pbPosition = _Next( (BYTE*)pszString - sizeof(long) );
   }
   else
   {
      if( m_pData->GetCount() == 0 ) return NULL;

      pbPosition = m_pData->GetData();
   }

   if( pbPosition >= m_pData->End() - sizeof(TCHAR) ) return NULL;
   
   return (TCHAR*)(pbPosition + sizeof(long));
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Return the last string and remove it from strings object
RETURN:  pointer to the last string in strings object*/
const TCHAR* CStrings::Pop()
{
   BYTE* pbLast;
   BYTE* pbPosition;
   BYTE* pbEnd;

   pbLast = pbPosition = m_pData->GetData();
   pbEnd = m_pData->End();
   while( (pbPosition = _Next( pbPosition )) < pbEnd ) pbLast = pbPosition;

   m_pData->SetLength( m_pData->GetLength() - (*(long*)pbLast * sizeof(TCHAR)) - sizeof(TCHAR) - sizeof(long) );
   m_pData->m_iCount--;
   return (TCHAR*)(pbLast + sizeof(long));
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Remove all strings from string object  */
void CStrings::Empty()
{
   _SINGLE;

   m_pData->SetLength( 0 );
   m_pData->SetCount( 0 );
}

//*************************************************************************************************
/*TYPE:  
PURPOSE: 
RETURN:  */
TCHAR* CStrings::_Find(
   BYTE* pszData, //
   int iCount //
   ) const
{

   while( iCount != 0 )
   {
      iCount--;
      pszData = _Next( pszData );
      _ASSERT( pszData < m_pData->End() ); 
   }

   pszData += sizeof(long);
   return (TCHAR*)pszData;
}

//*************************************************************************************************
/*TYPE:  PRIVATE
PURPOSE: Make sure that string has only one reference */
void CStrings::_Safe()
{
   if( m_pData->GetRefs() != 1 )
   {
      _ASSERT( m_pData->GetRefs() != -2 );
      m_pData = m_pData->Safe();
   }
}

//*************************************************************************************************
/*TYPE:  PRIVATE
PURPOSE: Make sure string buffer is specified size */
void CStrings::_AllocAdd(
   int iSize //
   )
{
   _ASSERT( m_pData->GetRefs() == 1 );

   _Data* pData;

   iSize++;
   iSize *= sizeof(TCHAR);
   iSize += sizeof(long);
   if( m_pData->GetMaxLength() < (m_pData->GetLength() + iSize) )
   {
      iSize += sizeof(_Data) + m_pData->GetLength();
      if( iSize < strings_m_iSmallAlloc )
      {
         iSize += strings_m_iTinyAlloc - iSize % strings_m_iTinyAlloc;
      }
      else if( iSize < strings_m_iBigAlloc )
      {
         iSize += strings_m_iSmallAlloc - iSize % strings_m_iSmallAlloc;
      }
      else
      {
         iSize += strings_m_iBigAlloc - iSize % strings_m_iBigAlloc;
      }

      pData = (_Data*) new BYTE[iSize];
      memcpy( pData, m_pData, sizeof( _Data ) + m_pData->GetLength() );
      pData->m_iMaxLength = iSize - sizeof(_Data);

      delete (BYTE*)m_pData;
      m_pData = pData;
   }
}

//*************************************************************************************************
/*TYPE:  PRIVATE
PURPOSE: Delete string object  */
void CStrings::_Delete()
{
   if( m_pData != _NOTHING )
   {
      m_pData->Release();
      m_pData = _NOTHING;
   }
}

CStrings::_Data* CStrings::_Data::Safe() 
{
   _Data* pData;
   int iLength;
   if( m_iRefs != 1 ) 
   {
      iLength = sizeof(_Data) + m_iLength;
      iLength += 64 - iLength % 64;
      pData = (_Data*) new BYTE[iLength];
      pData->m_iMaxLength = iLength - sizeof( _Data );
      memcpy( pData->GetData(), GetData(), GetLength() );
      pData->m_iLength = GetLength();
      pData->m_iCount = GetCount();
      pData->m_iRefs = 1;
      if( m_iRefs > 1 ) m_iRefs--;
      return pData;
   }
   return this; 
}

}; // namespace gd
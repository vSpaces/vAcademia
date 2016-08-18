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

/*! \file
    \brief file containing a very similar class to MFC's CString
*/

#include "stdafx.h"

#include <malloc.h>

#include "gd_String.h"

#ifdef _DEBUG
   #undef THIS_FILE
   static char THIS_FILE[]=__FILE__;
   #ifdef DEBUG_NEW
      #define new DEBUG_NEW
   #endif
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*! Define for the pdata_nil object, this is the object string will point at if it is empty */
#define _NOTHING (CString::_Data*)pdata_nil
/*! Make sure that string could be changed (no other references to it */
#define _SINGLE _Safe()

#ifdef _DEBUG
   //! in debug mode there is a member in string that points to start of text.
   //! this is for making it more easy to debug.
   #define _SETPSZ() m_psz = (TCHAR*)(m_pData+1)
#else
   #define _SETPSZ()
#endif


#ifdef WIN32
#ifdef _DEBUG
   #define CHECK_STRING( string ) _ASSERT( ::IsBadStringPtr( string, -1 ) == FALSE )
#endif

//! convert text to unicode
#ifdef _WCHAR_T_DEFINED
#define TO_UNICODE( pbsz, pwsz ) \
{ \
   unsigned uLength = strlen( pbsz ) + 1; \
   pwsz = (wchar_t*)_alloca( uLength * sizeof(wchar_t) ); \
   ::MultiByteToWideChar( CP_ACP, 0, pbsz,  -1, pwsz, uLength ); \
}
#else
#define TO_UNICODE( pbsz, pwsz ) \
{ \
   unsigned uLength = strlen( pbsz ) + 1; \
   pwsz = (unsigned short*)_alloca( uLength * sizeof(unsigned short) ); \
   ::MultiByteToWideChar( CP_ACP, 0, pbsz,  -1, pwsz, uLength ); \
}
#endif
      
//! convert text to multibyte
#define TO_MULTIBYTE( pwsz, pbsz ) \
{ \
   unsigned uLength = wcslen( pwsz) + 1; \
   pbsz = (char*)_alloca( uLength ); \
   ::WideCharToMultiByte( CP_ACP, 0, pwsz, -1, pbsz, uLength, NULL, NULL ); \
}
#endif // WIN32

#ifndef CHECK_STRING
   #define CHECK_STRING( string )
#endif

namespace gd
{

unsigned int CString::pdata_nil[] = { -1, 0, 0, NULL };
int CString::string_m_iTinyAlloc = 64;
int CString::string_m_iSmallAlloc = 128;
int CString::string_m_iBigAlloc = 512;

//*************************************************************************************************
CString::CString( const CString& other )
{
   _ASSERT( other.m_pData->GetRefs() != -2 );
   
   if( other.m_pData == _NOTHING ) m_pData = _NOTHING;
   else
   {
      m_pData = other.m_pData;
      m_pData->AddRef();
   }
   _SETPSZ();
}

//*************************************************************************************************
CString& CString::operator=( const CString& other )
{
   _ASSERT( other.m_pData->GetRefs() != -2 );
   _ASSERT( !(this == &other) );

   if( other.m_pData == _NOTHING )
   {
      if( m_pData != _NOTHING ) m_pData->Release();
      m_pData = _NOTHING;
   }
   else
   {
      if( m_pData != _NOTHING ) m_pData->Release();
      m_pData = other.m_pData;
      m_pData->AddRef();
   }
   _SETPSZ();

   return *this;
}

//*************************************************************************************************
CString::~CString()
{
   _Delete();
}


//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Make sure that internal buffer is specified length and also set string to end at specified
         length. if the string is shorter the end will not modify it <BR>
INFO:    If string contains more characters than \a iLength it will be shortened. <br>
RETURN:  pointer to buffer */
TCHAR* CString::GetBufferSetLength(
   int iLength //!< length that string will be set to.
   )
{
   _ASSERT( iLength >= 0 );

   _SINGLE;

   _AllocAdd( iLength - m_pData->GetLength() );

   m_pData->SetLength( iLength );
   m_pData->End()[0] = 0;

   return m_pData->GetText();
}

//*************************************************************************************************
/*!
PURPOSE: 
RETURN:  */
void CString::Set(
   const char* pbszText //
   )
{
   _Delete();
   Add( pbszText );
}
//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Set string text */
#ifdef _WCHAR_T_DEFINED
void CString::Set(
   const wchar_t* pwszText /*!< text that string will copy */
   )
{
   _Delete();
   Add( pwszText );
}
#else
void CString::Set(
   const unsigned short* pwszText /*!< text that string will copy */
   )
{
   _Delete();
   Add( pwszText );
}
#endif
//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Set text to string
RETURN:  */
void CString::Set(
   const char* pbszText, /*!< multibyte string characters will be extracted from */
   int iLength /*!< number of characters that should be inserterd */
   )
{
   _Delete();
   Add( pbszText, iLength );
}

//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Set string text (unicode).*/
#ifdef _WCHAR_T_DEFINED
void CString::Set(
   const wchar_t* pwszText, /*!< unicode string characters will be extracted from */
   int iLength /*!< number of characters that should be inserterd */
   )
{
   _Delete();
   Add( pwszText, iLength );
}
#else
void CString::Set(
   const unsigned short* pwszText, /*!< unicode string characters will be extracted from */
   int iLength /*!< number of characters that should be inserterd */
   )
{
   _Delete();
   Add( pwszText, iLength );
}
#endif

//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Format string with some of the printf flags. In order to get a feeling how the function 
         works, look for documentation about printf.<br>
\code
CString s;
int i = 10; i1 = 20;
s.Format("i = %d and i1 = %d. i + i1 = %d", i, i1, i + i1 );
// s = "i = 10 and i1 = 20. i + i1 = 30"
\endcode
*/
void CString::Format( const TCHAR* pszFormat, ... )
{
   Empty();
   Single();

   va_list arguments;
   va_start(arguments, pszFormat);

   _F( pszFormat, arguments );
   va_end(arguments);
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: 
RETURN:  */
CString& CString::FormatAdd( const TCHAR* pszFormat, ... )
{
   _SINGLE;

   va_list arguments;
   va_start(arguments, pszFormat);

   _F( pszFormat, arguments );
   va_end(arguments);

   return *this;
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: 
RETURN:  */
void CString::_F( const TCHAR* pszFormat, va_list arguments )
{
   unsigned uLength;
   const TCHAR* psz;
   TCHAR pszBuffer[64];
   

   while( *pszFormat != _T('\0') )
   {
      if( *pszFormat != _T('%') )
      {
         FastAdd( *pszFormat );
         pszFormat++;
         continue;
      }
      else
      {
         psz = pszBuffer;
         uLength = 0;
         pszFormat++;
         switch( *pszFormat )
         {
         case _T('%') :
            *pszBuffer = _T('%');
            pszBuffer[1] = _T('\0');
            uLength = 2;
            break;
         case _T('d') :
            _itot( va_arg( arguments, int ), pszBuffer, 10 );
            if( psz != NULL )
            {
               uLength = _tcslen( psz ) + 1;
            }
            break;
         case _T('g') :
#ifdef _UNICODE
            {
               char pbszBuffer[40];
               _gcvt( va_arg( arguments, double ), 35, pbszBuffer ); 
               ::MultiByteToWideChar( 0, 0, pbszBuffer, -1, pszBuffer, 40 );
               uLength = _tcslen( psz ) + 1;
            }
#else
            _gcvt( va_arg( arguments, double ), 35, pszBuffer ); 
            uLength = _tcslen( psz ) + 1;
#endif
            break;
         case _T('I') :
            if( _tcsncmp( pszFormat, _T("I64d"), 4 ) == 0 )
            {
               _i64tot( va_arg( arguments, __int64 ), pszBuffer, 10 );
               if( psz != NULL )
               {
                  uLength = _tcslen( psz ) + 1;
               }
            }
            break;
         case _T('s') :
            psz = va_arg( arguments, const TCHAR* );
            if( psz != NULL )
            {
               uLength = _tcslen( psz ) + 1;
            }
            break;
         default:
            continue;
         };

         pszFormat++;
         if( uLength > 0 )
         {
            _AllocAdd( uLength );
            memcpy( m_pData->End(), psz, uLength * sizeof(TCHAR) );
            m_pData->AddLength( uLength - 1 ); // don't add ending null character
         }
      }
   }
}


//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: add int value to string*/
void CString::Add( int iValue )
{
   TCHAR pszLong[25];
   
   _itot( iValue, pszLong, 10 );
   Add( pszLong );
}


//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: add double value to string*/
void CString::Add(
   double dValue //!< double value that is converted to string 
   )
{
   TCHAR pszDouble[40];
#ifdef _UNICODE
   {
      char pbszBuffer[40];
      _gcvt( dValue, 35, pbszBuffer ); 
      ::MultiByteToWideChar( 0, 0, pbszBuffer, -1, pszDouble, 40 );
   }
#else
   _gcvt( dValue, 35, pszDouble ); 
#endif

   Add( pszDouble );
}

void CString::Add( char chbCharacter ) 
{
   _SINGLE;

   _AllocAdd( 1 );

   m_pData->End()[0] = (TCHAR)chbCharacter;
   m_pData->End()[1] = 0;
   m_pData->m_iLength++;
}


/*---------------------------------------------------------------------------------*/ /**
 * Add wide character to string
 * \param wchCharacter character that is added to string
 */
#ifdef _WCHAR_T_DEFINED
void CString::Add( wchar_t wchCharacter )
{
   _SINGLE;

   _AllocAdd( 1 );

   m_pData->End()[0] = (TCHAR)wchCharacter;
   m_pData->End()[1] = 0;
   m_pData->m_iLength++;
}
#else
void CString::Add( unsigned short wchCharacter )
{
   _SINGLE;

   _AllocAdd( 1 );

   m_pData->End()[0] = (TCHAR)wchCharacter;
   m_pData->End()[1] = 0;
   m_pData->m_iLength++;
}
#endif

/*---------------------------------------------------------------------------------*/ /**
 * Add text to string object
 * \param pbszText text that is added to string
 */
#ifdef _WCHAR_T_DEFINED
void CString::Add( const char* pbszText )
{
   _SINGLE;

   int iLength;
   iLength = strlen( pbszText );
   _AllocAdd( iLength );
   
#ifdef _UNICODE
   wchar_t* pwsz;
   TO_UNICODE( pbszText, pwsz ); 
   memcpy( m_pData->End(), pwsz, (iLength + 1) * sizeof( wchar_t ) );
#else
   memcpy( m_pData->End(), pbszText, iLength + 1 );
#endif
   m_pData->SetLength( m_pData->GetLength() + iLength );
}
#else
void CString::Add( const char* pbszText )
{
   _SINGLE;

   int iLength;
   iLength = strlen( pbszText );
   _AllocAdd( iLength );
   
#ifdef _UNICODE
   unsigned short* pwsz;
   TO_UNICODE( pbszText, pwsz ); 
   memcpy( m_pData->End(), pwsz, (iLength + 1) * sizeof( unsigned short ) );
#else
   memcpy( m_pData->End(), pbszText, iLength + 1 );
#endif
   m_pData->SetLength( m_pData->GetLength() + iLength );
}
#endif                                                      

/*---------------------------------------------------------------------------------*/ /**
 * Add text to string object
 * \param pwszText text that is added to string (unicode)
 */
#ifdef _WCHAR_T_DEFINED
void CString::Add( const wchar_t* pwszText )
{
   _SINGLE;

   int iLength;
   iLength = wcslen( pwszText );
   _AllocAdd( iLength );
   
#ifdef _UNICODE
   memcpy( m_pData->End(), pwszText, (iLength + 1) * sizeof( wchar_t ) );
#else
   char* pbsz;
//{ 
//	unsigned uLength = strlen( pbszText ) + 1; 
//	pwsz = (wchar_t*)_alloca( uLength * sizeof(wchar_t) ); 
////	::MultiByteToWideChar( CP_ACP, 0, pbszText,  -1, pwsz, uLength ); 
//	std::use_facet<codecvt<char, wchar_t, mbstate_t> > 
//	(gds_loc,0,true).out( gds_state, 
//	pbszText, &pbszText[-1], gds_pszNext, 
//	pwsz, &pwsz[uLength], gds_pwszNext);
//}
   TO_MULTIBYTE( pwszText, pbsz );
   memcpy( m_pData->End(), pbsz, iLength + 1 );
#endif
   m_pData->SetLength( m_pData->GetLength() + iLength );
}
#else
void CString::Add( const unsigned short* pwszText )
{
   _SINGLE;

   int iLength;
   iLength = wcslen( pwszText );
   _AllocAdd( iLength );
   
#ifdef _UNICODE
   memcpy( m_pData->End(), pwszText, (iLength + 1) * sizeof( unsigned short ) );
#else
   char* pbsz;
   TO_MULTIBYTE( pwszText, pbsz );
   memcpy( m_pData->End(), pbsz, iLength + 1 );
#endif
   m_pData->SetLength( m_pData->GetLength() + iLength );
}
#endif

/*---------------------------------------------------------------------------------*/ /**
 * Add a specified numbers of characters from the string sent to function
 * \param pbszText  text that will be added to string (multibyte)
 * \param iLength   lenght of text that is added
 */
#ifdef _WCHAR_T_DEFINED
void CString::Add( const char* pbszText, int iLength )
{
   _SINGLE;

   int iTemp;
   const char* pbszAdd;

   _AllocAdd( iLength );
   pbszAdd = pbszText;
   iTemp = iLength;
   // Calculate if string actualy is the length that has been set.
   while( *pbszText != 0 && iTemp )
   {
      iTemp--;
      pbszText++;
   }

   iTemp = pbszText - pbszAdd;
   iLength = ( iLength <= iTemp ) ? iLength : iTemp;
#ifdef _UNICODE
   wchar_t* pwsz;
   pwsz = (wchar_t*)_alloca( iLength * sizeof(unsigned short) ); 
   ::MultiByteToWideChar( CP_ACP, 0, pbszAdd, iLength, pwsz, iLength ); 
   memcpy( m_pData->End(), pwsz, iLength * sizeof(unsigned short) );
#else
   memcpy( m_pData->End(), pbszAdd, iLength );
#endif
   m_pData->End()[iLength] = 0;
   m_pData->AddLength( iLength );
}
#else
void CString::Add( const char* pbszText, int iLength )
{
   _SINGLE;

   int iTemp;
   const char* pbszAdd;

   _AllocAdd( iLength );
   pbszAdd = pbszText;
   iTemp = iLength;
   // Calculate if string actualy is the length that has been set.
   while( *pbszText != 0 && iTemp )
   {
      iTemp--;
      pbszText++;
   }

   iTemp = pbszText - pbszAdd;
   iLength = ( iLength <= iTemp ) ? iLength : iTemp;
#ifdef _UNICODE
   WCHAR* pwsz;
   pwsz = (unsigned short*)_alloca( iLength * sizeof(unsigned short) ); 
   ::MultiByteToWideChar( CP_ACP, 0, pbszAdd, iLength, pwsz, iLength ); 
   memcpy( m_pData->End(), pwsz, iLength * sizeof(unsigned short) );
#else
   memcpy( m_pData->End(), pbszAdd, iLength );
#endif
   m_pData->End()[iLength] = 0;
   m_pData->AddLength( iLength );
}
#endif
/*---------------------------------------------------------------------------------*/ /**
 * Add a specified numbers of characters from the string sent to function
 * \param pwszText  add text to string (unicode)
 * \param iLength   lenght of text that is added to string

\code
CString s = "123456789";
CString sAdd;
sAdd.Add( s, 5 );

// string = "12345"
\endcode
 */
#ifdef _WCHAR_T_DEFINED
void CString::Add( const wchar_t* pwszText, int iLength )
{
   _SINGLE;

   int iTemp;
   const wchar_t* pwszAdd;

   _AllocAdd( iLength );
   pwszAdd = pwszText;
   iTemp = iLength;
   while( *pwszText != 0 && iTemp ) 
   {
      iTemp--;
      pwszText++;
   }

   iTemp = pwszText - pwszAdd;
   iLength = ( iLength <= iTemp ) ? iLength : iTemp;
#ifdef _UNICODE
   memcpy( m_pData->End(), pwszAdd, iLength * sizeof(TCHAR) );
#else
   char* pbsz = (char*)_alloca( iLength ); 
   ::WideCharToMultiByte( CP_ACP, 0, pwszAdd, iLength, pbsz, iLength, NULL, NULL ); 
   memcpy( m_pData->End(), pbsz, iLength );
#endif
   m_pData->End()[iLength] = 0;
   m_pData->AddLength( iLength );
}
#else
void CString::Add( const unsigned short* pwszText, int iLength )
{
   _SINGLE;

   int iTemp;
   const unsigned short* pwszAdd;

   _AllocAdd( iLength );
   pwszAdd = pwszText;
   iTemp = iLength;
   while( *pwszText != 0 && iTemp ) 
   {
      iTemp--;
      pwszText++;
   }

   iTemp = pwszText - pwszAdd;
   iLength = ( iLength <= iTemp ) ? iLength : iTemp;
#ifdef _UNICODE
   memcpy( m_pData->End(), pwszAdd, iLength * sizeof(TCHAR) );
#else
   char* pbsz = (char*)_alloca( iLength ); 
   ::WideCharToMultiByte( CP_ACP, 0, pwszAdd, iLength, pbsz, iLength, NULL, NULL ); 
   memcpy( m_pData->End(), pbsz, iLength );
#endif
   m_pData->End()[iLength] = 0;
   m_pData->AddLength( iLength );
}
#endif

/*---------------------------------------------------------------------------------*/ /**
 * add text in string object
 * \param s string object that is added
 */
void CString::Add( const CString& s )
{
   _SINGLE;

   _AllocAdd( s.GetLength() );

   memcpy( m_pData->End(), (LPCTSTR)s, s.GetLength() * sizeof(TCHAR) );
   m_pData->AddLength( s.GetLength() );
   m_pData->End()[0] = 0;
}

/*---------------------------------------------------------------------------------*/ /**
 * add text from multibyte character buffer
 * \param pbszText multibyte character string text are added from
 * \param iLenght number of characters added
 * \return reference to string (*this)
 */
CString& CString::StreamAdd( const char* pbszText, int iLenght )
{
   Add( pbszText, iLenght );

   return *this;
}

/*---------------------------------------------------------------------------------*/ /** 
 * Adds specified lenght of widecharacters from buffer
 * \param pwszText pointer to text buffer text are added from
 * \param iLenght number of characters that are added
 * \return returns reference to string object
 \code
 gd::CString s;
 s.StreamAdd( "1234" ).StreamAdd( "56789", 2 );
 \endcode
 */
#ifdef _WCHAR_T_DEFINED
CString& CString::StreamAdd(
   const wchar_t* pwszText, //
   int iLenght //
   )
{
   Add( pwszText, iLenght );

   return *this;
}
#else
CString& CString::StreamAdd( const unsigned short* pwszText, int iLenght  )
{
   Add( pwszText, iLenght );

   return *this;
}
#endif

/*---------------------------------------------------------------------------------*/ /**
 * add string and return reference to string object
 * \param s string containing text that is added
 * \return reference to string object (*this)
 */
CString& CString::StreamAdd( const CString& s )
{
   Add( s );

   return *this;
}


/*---------------------------------------------------------------------------------*/ /**
 * Inserts a text specified number of times to string
 * \param iPosition where text should be inserted
 * \param pszText text that is going to be inserted
 * \param iRepeat how many times text is going to be inserted

\code
CString s("####");
s.Insert( 1, "()", 2 );
// s now contains "#()()###"
\endcode
 */
void CString::Insert( int iPosition, const TCHAR* pszText, int iRepeat )
{
   CHECK_STRING( pszText );
   _ASSERT( iRepeat >= 0 );
   
   _SINGLE;

   int iLength;
   int iSumLength;
   TCHAR* pszPosition;


   // calculate length
   iLength = lstrlen( pszText );
   iSumLength = iLength * iRepeat;
   _AllocAdd( iSumLength );

   if( (unsigned)iPosition > (unsigned)m_pData->GetLength() )
   {
      pszPosition = m_pData->End();
   }
   else
   {
      pszPosition = &m_pData->GetText()[iPosition];
      memmove( &pszPosition[iSumLength], pszPosition, (m_pData->GetLength() - iPosition) * sizeof(TCHAR) );
   }

   while( iRepeat != 0 )
   {
      iRepeat--;
      memcpy( pszPosition, pszText, iLength * sizeof(TCHAR) );
      pszPosition += iLength;
   }

   m_pData->AddLength( iSumLength );
   m_pData->End()[0] = 0;
}

/*---------------------------------------------------------------------------------*/ /**
 * Compare strings from start to one of the strings ends
 * \param pszText string compared with internal string
 * \return true if start of strings match otherwise false
 */
bool CString::CompareStartNoCase( const TCHAR* pszText )
{
   unsigned uLength = m_pData->GetLength();
   unsigned u;
   for(u = 0; u < uLength && pszText[u] != _T('\0'); u++ );
   if( u < uLength ) uLength = u;

   return (_tcsnicmp( pszText, m_pData->GetText(), uLength) == 0);
}

/*---------------------------------------------------------------------------------*/ /**
 * Compare the end part of text. No full matching is needed. Just that text that i is 
   smallest is compared. If pszText is bigger then internal text then a full match i done.
 * \param pszText text that is compared with
 * \return true if same or false otherwise
 */
bool CString::CompareEnd( const TCHAR* pszText ) 
{
   unsigned uLength = m_pData->GetLength();
   unsigned u;
   for(u = 0; u < uLength && pszText[u] != _T('\0'); u++ );
   if( u < uLength ) uLength = u;
   return (memcmp(m_pData->End() - uLength, pszText, uLength * sizeof(TCHAR) ) == 0 ); 
}

//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Compare the end part of text. Ignores case. No full matching is needed. Just that text 
         that i is smallest is compared. If pszText is bigger then internal text then a full match 
         i done.
RETURN:  true if same or false otherwise*/
bool CString::CompareEndNoCase(
   const TCHAR* pszText //!< text that is compared with
   )
{
   unsigned uLength = m_pData->GetLength();
   unsigned u;
   for(u = 0; u < uLength && pszText[u] != _T('\0'); u++ );
   if( u < uLength ) uLength = u;
   return (_tcsnicmp( pszText, m_pData->End() - uLength, uLength) == 0);
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Trim characters from string */
void CString::Trim(
   const TCHAR* pszCharacters, // characters that is going to be removed
   Align align // if characters is removed from left or right
   )
{
   CHECK_STRING( pszCharacters );

   if( m_pData->GetLength() < 1 ) return;

   _SINGLE;

   int iNext;
   int iFind;
   TCHAR* pszPosition;
   TCHAR* pszEnd;

   if( align == eRight )
   {
      iNext = -1;
      pszEnd = m_pData->GetText() - 1;
      pszPosition = m_pData->End() - 1;
   }
   else
   {
      iNext = 1;
      pszEnd = m_pData->End();
      pszPosition = m_pData->GetText();
   }

   while( pszPosition != pszEnd )
   {
      iFind = 0;
      while( pszCharacters[iFind] != 0 )
      {
         if( pszCharacters[iFind] == *pszPosition ) break;
         iFind++;
      }

      if( pszCharacters[iFind] == 0 )
      {
         break;
      }

      pszPosition += iNext;
   }

   if( align == eRight )
   {
      if( pszPosition != pszEnd ) pszPosition -= iNext;
      m_pData->SetLength( m_pData->GetLength() - ((m_pData->End() - 1) - pszPosition) );
      m_pData->End()[0] = 0;
   }
   else
   {
      Cut( 0, pszPosition - m_pData->GetText() );
   }
}
                                              
/*---------------------------------------------------------------------------------*/ /** 
 * Remove characters from string
 * \param iStart start position for section that is removed
 * \param iLength length for section that is removed

\code
gd::CString s("abcdefghijklmn");
s.Cut( 1, 3 );
// s is now "aefghijklmn"
\endcode
 */
void CString::Cut( int iStart, int iLength )
{
   _ASSERT( iStart >= 0 );

   _SINGLE;

   //* If start is beyond the end of string then don't do any cutting.
   if( iStart >= m_pData->GetLength() ) return;

   //* if the lenght that should be cut is less than total lenght for string starting from "iStart"
   //* then remove text part.
   if( iLength < m_pData->GetLength() - iStart )
   {
      memmove( &m_pData->GetText()[iStart], &m_pData->GetText()[iStart + iLength], (m_pData->GetLength() - iLength) * sizeof(TCHAR) );
   }
   else
   {
      iLength = m_pData->GetLength() - iStart;
   }
   m_pData->SetLength( m_pData->GetLength() - iLength );
   m_pData->End()[0] = 0;
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: convert string to lowercase*/
void CString::MakeLower()
{
   _SINGLE;

#ifdef _UNICODE
   _wcslwr( m_pData->GetText() );
#else
   _strlwr( m_pData->GetText() );
#endif
}

/**
 * Convert string to uppercase
 */
void CString::MakeUpper()
{
   _SINGLE;

#ifdef _UNICODE
   _wcsupr( m_pData->GetText() );
#else
   _strupr( m_pData->GetText() );
#endif
}                                             

/*---------------------------------------------------------------------------------*/ /**
 * Replace specified string with replace string
 * \param pszOld string that is goingt to be replaced
 * \param pszNew string that is inserted
 * \param iCount number of replacements that should be done
 */
void CString::Replace( const TCHAR* pszOld, const TCHAR* pszNew, int iCount )
{
   CHECK_STRING( pszOld );

   if( iCount == 0 ) return;

   _SINGLE;

   int iTemp, iOldLength, iNewLength, iDifference;
   TCHAR* pszText;

   iOldLength = _tcslen( pszOld );
   if( pszNew != NULL ) iNewLength = _tcslen( pszNew );
   else iNewLength = 0;

   iDifference = iNewLength - iOldLength;
   pszText = m_pData->GetText();
   while( *pszText != 0 )
   {
      if( *pszText == *pszOld )
      {
         if( _tcsncmp( pszText, pszOld, iOldLength ) == 0 )
         {
            if( iDifference != 0 )
            {
               if( iDifference > 0 )
               {
                  iTemp = pszText - m_pData->GetText();
                  _AllocAdd( iDifference );
                  pszText = &m_pData->GetText()[iTemp];
               }

               memmove( &pszText[iOldLength + iDifference], 
                        &pszText[iOldLength], 
                        (m_pData->GetLength() - (&pszText[iOldLength] - m_pData->GetText())) * sizeof(TCHAR) );
               m_pData->SetLength( m_pData->GetLength() + iDifference );
               m_pData->End()[0] = 0;
            }// end if

            if( pszNew != NULL ) memcpy( pszText, pszNew, iNewLength * sizeof(TCHAR) );
            pszText += iNewLength - 1;

            iCount--;
            if( iCount == 0 ) return;
         }// end if
      }// end if

      pszText++;
   }// end while
}



//*************************************************************************************************
/*!TYPE:  PUBLIC<br>
PURPOSE: Get string object with specified numbers of characters from left<br>
\return  A CString object */
CString CString::Left(
   int iCount //!< number of characters that should be extracted from start
   ) const
{
   CString string;

   string.Add( m_pData->GetText(), iCount );

   return string;
}

//*************************************************************************************************
/*!TYPE:  PUBLIC<br>
PURPOSE: Get string object with specified number of characters from right<br>
\return  A CString object*/
CString CString::Right(
   int iCount //!< number of characters that should be extracted from end
   ) const
{
   CString string; 
   
   if( iCount >= m_pData->GetLength() )
   {
      string = *this;
   }
   else
   {
      string.Add( &m_pData->GetText()[m_pData->GetLength() - iCount], iCount );
   }

   return string;
}


/*---------------------------------------------------------------------------------*/ /**
 * Return string object that starts at start index and has the given length.
 * \param iStart start position where extracted string begins
 * \param iLength length for extracted string.
 * \return String object containing text from start position at specified index with the length
   from there

\code
CString s("0123456789");
CString sInner = s.Mid( 2, 3 );
// sInner = "234"
CString sLeft = s.Left( 4 );
// sLeft = "0123"
\endcode
 */
CString CString::Mid( int iStart, int iLength ) const
{
   CString string;

   if( iStart < m_pData->GetLength() )
   {
      if( iLength < 0 )
      {
         string.Set( &m_pData->GetText()[iStart] );
      }
      else
      {
         if( iLength > (m_pData->GetLength() - iStart) )
         {
            iLength = m_pData->GetLength() - iStart;
         }
         string.Set( &m_pData->GetText()[iStart], iLength );
      }
   }

   return string;
}

/*---------------------------------------------------------------------------------*/ /**
 * Return string object that starts at start index and stops when text is found or
   the complete string if stop text isn't found
 * \param iStart start position where extracted string begins
 * \param pszStop text that is searched for after the start index.
 * \return String object containing text from start position at specified index with the length
   from there
 */
CString CString::Mid( int iStart, const TCHAR* pszStop ) const
{
   CString string;

   if( iStart < m_pData->GetLength() )
   {
      int iPos = Find( pszStop, iStart );
      if( iPos == -1 )
      {
         string.Set( &m_pData->GetText()[iStart] );
      }
      else
      {
         string.Set( &m_pData->GetText()[iStart], iPos - iStart );
      }
   }

   return string;
}

/*---------------------------------------------------------------------------------*/ /**
 * Return string object that starts after start text and stops when text is found or
   the complete string if stop text isn't found
 * \param pszStart start position where extracted is extracted after
 * \param pszStop text that is searched for after the start index.
 * \return String object containing text from start position at specified index with the length
   from there
 */
CString CString::Mid( const TCHAR* pszStart, const TCHAR* pszStop ) const
{
   int iStart;
   if( pszStart == NULL ) iStart = 0;
   else
   {
      iStart = Find( pszStart );
      if( iStart == -1 ) return CString();

      iStart += _tcslen( pszStart );
   }

   if( pszStop == NULL )
   {
      return Mid( iStart );
   }

   return Mid( iStart, pszStop );
}


//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Find text in string object
RETURN:  index for the first character that was found in string or -1 if text wasn't found*/
int CString::Find(
   const TCHAR* pszFind, // text to search for
   int iStart // where to start looking in string
   ) const
{
   _ASSERT( iStart >= 0 );
   _ASSERT( pszFind != NULL );
   CHECK_STRING( pszFind );

   int iFind;
   const TCHAR* pszTest;
   const TCHAR* pszText;

   if( iStart > m_pData->GetLength() ) return -1;


   pszText = m_pData->GetText();

   while( pszText[iStart] != 0 )
   {
      if( pszText[iStart] == *pszFind )
      {
         iFind = 0;
         pszTest = &pszText[iStart];
         while( (pszTest[iFind] == pszFind[iFind]) && (pszFind[iFind] != 0) ) iFind++;

         if( pszFind[iFind] == 0 ) return iStart;
      }
      iStart++;
   }

   return -1;
}

//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Find specified text in text section in string specified number of times.<br>
RETURN:  index to the last text position if found otherwise -1<br>
\code
gd::CString s("1,2,3,4,5,6,7,8,9");
int iPos = s.FindCount( ",", 3 );
// IPos = 5
*/
int CString::FindCount(
   const TCHAR* pszText, //!<
   unsigned uCount //!<
   ) const
{
   _ASSERT( uCount > 0 );

   int iPosition = 0;
   unsigned uLength = _tcslen( pszText );

   while( uCount )
   {
      uCount--;
      iPosition = Find( pszText, iPosition );
      if( iPosition == -1 || uCount == 0 ) break;
      iPosition += uLength;
   }

   return iPosition;
}

//*************************************************************************************************
/*TYPE:  
PURPOSE: 
RETURN:  */
int CString::ReverseFind(
   TCHAR chCharacter //
   ) const
{
   UINT uPosition;
   const TCHAR* pszText;

   uPosition = m_pData->GetLength();

   pszText = m_pData->GetText();
   while( uPosition != 0 )
   {
      uPosition--;
      if( pszText[uPosition] == chCharacter )
      {
         return uPosition; 
      }
   }

   return -1;
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Find text in string, and start searching from end.
RETURN:  zero based index of the found text or -1 if not found*/
int CString::ReverseFind(
   const TCHAR* pszFind //
   ) const
{
   CHECK_STRING( pszFind );

   const TCHAR* pszText;
   unsigned uPosition;
   unsigned uLength = _tcslen( pszFind );
   if( uLength > (unsigned)m_pData->GetLength() ) return -1;

   pszText = m_pData->GetText();
   uPosition = 1 + (unsigned)m_pData->GetLength() - uLength;
   while( uPosition != 0 )
   {
      uPosition--;
      if( *pszFind == pszText[uPosition] )
      {
         if( memcmp( pszFind, &pszText[uPosition], uLength * sizeof( TCHAR ) ) == 0 ) return uPosition;
      }
   }

   return -1;
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Count how many times the sent text exist in string
RETURN:  Number of times text was found in string. */
unsigned CString::Count(
   const TCHAR* pszText //
   ) const
{
   _ASSERT( pszText != NULL );
   CHECK_STRING( pszText );

   unsigned uLength, uCount;
   const TCHAR* pszPosition;

   uCount = 0;
   uLength = _tcslen( pszText );

   pszPosition = m_pData->GetText();
   while( *pszPosition != _T('\0') )
   {
      if( *pszPosition == *pszText )
      {
         if( _tcsncmp( pszPosition, pszText, uLength ) == 0 )
         {
            uCount++;
            pszPosition += uLength - 1;
         }
      }
      pszPosition++;
   }

   return uCount;
}

//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Return internal buffer for string.<br>
INFO:    When there is some need to change the buffer without memberfunctions in string class. This
         function can be called to get the internal buffer. If the buffer has been changed, don't 
         forget to call ReleaseBuffer().
\code
CString s;
const char* psz = s.GetBuffer( 1000 );
strcpy( psz, "You need to call ReleaseBuffer" );
// If you don't call ReleaseBuffer CString have no knowledge about that the length has bee changed
s.ReleaseBuffer();
\endcode
\return  Pointer to string buffer */
TCHAR* CString::GetBuffer(
   int iMinLength //!< Make sure the internal stringbuffer can store iMinLength number of characters
   ) 
{
   _ASSERT( iMinLength >= 0 );

   _SINGLE;

   _AllocAdd( iMinLength - m_pData->GetLength() );
   return m_pData->GetText();
}

//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Return last position in string buffer and make sure buffer can hold specified length<br>
\code
CString s( "xxx" );
strcpy( s.GetBufferEnd( 4 ), "yyy" );
// String has been extended with characters so there is no need to calculate from start. 
s.ReleaseBufferFromEnd();
\endcode
\return  Pointer to string buffer */
TCHAR* CString::GetBufferEnd(
   int iAddLength //!< Make sure that the internal stringbuffer can add iAddLength number of characters
   )
{
   _ASSERT( iAddLength >= 0 );

   _SINGLE;

   _AllocAdd( iAddLength );
   return m_pData->End();
}


//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Recalculate length for string<br>
INFO:    When you modify string buffer "outside" CString this needs to be called in order to
         update the internal length. Otherwise CString don't know where it ends.*/
void CString::ReleaseBuffer()
{
   int iLength;
   iLength = lstrlen( m_pData->GetText() );
   m_pData->SetLength( iLength );
}

//*************************************************************************************************
/*!TYPE:  PUBLIC <br>
PURPOSE: Calculate the lenght from end of string. <br>
INFO:    If buffer has been modified from outside and there has text has been added. Then this
         function can be used because it only adds from the end of previous text. It will
         because of that do a faster jobb then start calculating from the beginging. */
void CString::ReleaseBufferFromEnd()
{
   int iLength;
   iLength = lstrlen( m_pData->End() );
   m_pData->AddLength( iLength );
}

//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Remove all signs that isn't characters, numbers or underscore<br>
\code
CString s( "xxx gh {( _" );
s.StripNonAlnumCharacter()
// s is now "xxxgh_"
\endcode
*/
void CString::StripNonAlnumCharacter()
{
   _SINGLE;
   TCHAR* pszPosition, * pszInsert;

   pszPosition = pszInsert = m_pData->GetText();
   while( *pszPosition != 0 ) 
   {
      if( iscsym( *pszPosition ) != 0 ) 
      {
         *pszInsert = *pszPosition;
         pszInsert++;
      }
      pszPosition++;
   }

   // Set end to the new text
   m_pData->SetLength( pszInsert - m_pData->GetText() );
   m_pData->End()[0] = 0;
}

//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Remove characters from string. Finds characters that exist in buffer en removes them from
         stringvalue. <br>
\code
CString s( "abcdefghijkabcd" );
s.Strip("acd")
// s is now "befghijkb"
\endcode
*/
void CString::Strip( 
   const TCHAR* psz //!< pointer to buffer containing characters that should be removed
   )
{
   _SINGLE;

   unsigned uDecrease = 0;
   unsigned uLength = _tcslen( psz );
   TCHAR* pszPosition, *pszStore;

   pszStore = pszPosition = m_pData->GetText();
   while( *pszPosition != 0 ) 
   {
      for( unsigned u = 0; u < uLength; u++ )
      {
         if( psz[u] != *pszPosition ) continue;
         uDecrease++;
         goto skipstore;
      }
      *pszStore = *pszPosition;
      pszStore++;
skipstore:
      pszPosition++;
   }

   if( uDecrease == 0 ) return;

   m_pData->SetLength( m_pData->GetLength() - uDecrease );
   m_pData->End()[0] = 0;
}

#ifdef WIN32
//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Alloctate a BSTR string from string<br>
RETURN:  BSTR */
BSTR CString::AllocSysString() const
{
	ATLASSERT( FALSE);
   BSTR bstr;
/*#ifdef _UNICODE
   bstr = ::SysAllocString( m_pData->GetText() );
#else
   #ifdef _WCHAR_T_DEFINED
      wchar_t* pwsz;
      TO_UNICODE( m_pData->GetText(), pwsz ); 
      bstr = ::SysAllocString( pwsz );
   #else
      unsigned short* pwsz;
      TO_UNICODE( m_pData->GetText(), pwsz ); 
      bstr = ::SysAllocString( pwsz );
   #endif
#endif*/

   return bstr;
}

//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Load stringresource from specified recource instance<br>
RETURN:  <br>*/
void CString::LoadString(
   HINSTANCE hInstance, //!<
   unsigned uId //!<
   )
{
   int iLength = ::LoadString( hInstance, uId, GetBuffer( 512 ), 512 );
   m_pData->SetLength( iLength );
   m_pData->End()[0] = 0;
}

//*************************************************************************************************
/*!TYPE: PUBLIC<br>
PURPOSE: Load stringresource from specified recource instance<br>
RETURN:  <br>*/
void CString::LoadStringFormat(
   HINSTANCE hInstance, //!<
   unsigned uId, //!<
   ...
   )
{
   Empty();
   Single();

   TCHAR pszFormat[512];

   va_list arguments;
   va_start(arguments, uId);


   int iLength = ::LoadString( hInstance, uId, pszFormat, 512 );
   _F( pszFormat, arguments );
}

#endif

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: 
RETURN:  */
void CString::SetExternalBuffer(
   TCHAR* pszBuffer, //
   int iLength //
   )
{
   _ASSERT( iLength > sizeof(_Data) );

   _Data* pData;

   pData = (_Data*)pszBuffer;
   pData->m_iRefs = 2;
   pData->m_iLength = 0;
   pData->m_iMaxLength = iLength - sizeof(_Data);

   _Delete();
   m_pData = pData;
}

//*************************************************************************************************
/*! Clear stringdata (internal buffer is not destroyed) */
void CString::Empty()
{
   if( m_pData->GetRefs() > 1 )
   {
      m_pData->Release();
      m_pData = (CString::_Data*)pdata_nil;
   }
   else if( m_pData->GetLength() > 0 )
   {
      m_pData->SetLength( 0 );
      m_pData->End()[0] = 0;
   }
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Dealocate space that is unused */
void CString::FreeExtra()
{
   _SINGLE;

   int iSize;
   _Data* pData;

   if( m_pData->GetLength() < m_pData->GetMaxLength() )
   {
      iSize = sizeof(_Data) + (1 + m_pData->GetLength());
      iSize += string_m_iTinyAlloc - iSize % string_m_iTinyAlloc;
#ifdef _UNICODE
      pData = (_Data*) new BYTE[iSize * sizeof(TCHAR)];
      iSize += sizeof(_Data) / 2;
#else
      pData = (_Data*) new BYTE[iSize];
#endif
      memcpy( pData, m_pData, sizeof( _Data ) + (m_pData->GetLength() + 1) * sizeof(TCHAR) );
      pData->m_iMaxLength = iSize - sizeof(_Data);

      delete (BYTE*)m_pData;
      m_pData = pData;
      _SETPSZ();
   }
}

//*************************************************************************************************
/*TYPE:  
PURPOSE: */
void CString::Kill()
{
   _Delete();
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Protect the buffer from beeing modified. When string is locked no functions can be used
         that may change the internal buffer. */
void CString::LockBuffer()
{
   _SINGLE;

   m_pData->m_iRefs = -2;
}

//*************************************************************************************************
/*TYPE:  PUBLIC
PURPOSE: Remove the lock from string and it is possible to modifiy it again. */
void CString::UnlockBuffer()
{
   _ASSERT( m_pData->GetRefs() != -1 );
   m_pData->m_iRefs = 1;
}


//*************************************************************************************************
/*TYPE:  PRIVATE
PURPOSE: Make sure that string has only one reference */
void CString::_Safe()
{
   if( m_pData->GetRefs() != 1 )
   {
      _ASSERT( m_pData->GetRefs() != -2 );
      m_pData = m_pData->Safe();
      _SETPSZ();
   }
}

/*---------------------------------------------------------------------------------*/ /**
 * Method used to allocate more space to internal buffer.<br>
 * Before any operations is done on the internal string that could add characters, this method
 * is called to make sure that there is enough room. If string needs more space to fit then
 * the internal buffer increases in size.<br>
 * The growth of the internal buffer is done in blocks that aligns well with interal memory
 * management. Also the growth is done in a way then new allocations should'n be done to
 * often. This is a slow process and by optimizing new allocations you can improve the overall
 * speed for the application. It is possible to change then parameters used to increase the
 * internal buffer to optimize even more.
 * \param iSize how much more characters that should fit in the internal buffer
 */
void CString::_AllocAdd( int iSize )
{
   _ASSERT( m_pData->GetRefs() == 1 );

   _Data* pData;

   if( m_pData->GetMaxLength() < m_pData->GetLength() + iSize + 1 )
   {
      iSize += sizeof(_Data) + (1 + m_pData->GetLength());
      if( iSize < string_m_iSmallAlloc )
      {
         iSize += string_m_iTinyAlloc - iSize % string_m_iTinyAlloc;
      }
      else if( iSize < string_m_iBigAlloc )
      {
         iSize += string_m_iSmallAlloc - iSize % string_m_iSmallAlloc;
      }
      else
      {
         iSize += string_m_iBigAlloc - iSize % string_m_iBigAlloc;
      }
#ifdef _UNICODE
      pData = (_Data*) new BYTE[iSize * sizeof(TCHAR)];
      iSize += sizeof(_Data) / 2;
#else
      pData = (_Data*) new BYTE[iSize];
#endif
      memcpy( pData, m_pData, sizeof( _Data ) + (m_pData->GetLength() + 1) * sizeof(TCHAR) );
      pData->m_iMaxLength = iSize - sizeof(_Data);

      delete (BYTE*)m_pData;
      m_pData = pData;
      _SETPSZ();
   }
}

//*************************************************************************************************
/*TYPE:  PRIVATE
PURPOSE: Delete string object  */
void CString::_Delete()
{
   if( m_pData != _NOTHING )
   {
      m_pData->Release();
      m_pData = _NOTHING;
      _SETPSZ();
   }
}


CString::_Data* CString::_Data::Safe() 
{
   _Data* pData;
   int iLength;
   if( m_iRefs != 1 ) 
   {
      iLength = sizeof(_Data) + (1 + m_iLength);
      iLength += 32 - iLength % 32;
#ifdef _UNICODE
      pData = (_Data*) new BYTE[iLength * sizeof(TCHAR)];
      iLength += sizeof(_Data) / 2;
#else
      pData = (_Data*) new BYTE[iLength];
#endif
      pData->m_iMaxLength = iLength - sizeof( _Data );
      memcpy( pData->GetText(), GetText(), (GetLength() + 1) * sizeof(TCHAR) );
      pData->m_iLength = GetLength();
      pData->m_iRefs = 1;
      if( m_iRefs > 1 ) m_iRefs--;
      return pData;
   }
   return this; 
}


}; // namespace gd


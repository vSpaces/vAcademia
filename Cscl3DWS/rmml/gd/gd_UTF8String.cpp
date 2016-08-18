// gd_UTF8String.cpp: implementation of the CUTF8String class.
//
//////////////////////////////////////////////////////////////////////

#include "mlRMML.h" // for precompile heades only
#include "gd\gd_UTF8String.h"
#include <malloc.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define _NOTHING (CUTF8String::_Data*)pdata_nil
#define _SINGLE _Safe()

#ifdef _DEBUG
   #define _SETPSZ() m_psz = (char*)(m_pData+1)
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
   pwsz = MP_NEW_ARR( wchar_t, uLength ); \
   ::MultiByteToWideChar( CP_ACP, 0, pbsz, -1, pwsz, uLength ); \
}
#else
#define TO_UNICODE( pbsz, pwsz ) \
{ \
   unsigned uLength = strlen( pbsz ) + 1; \
   pwsz = MP_NEW_ARR( unsigned short, uLength); \
   ::MultiByteToWideChar( CP_ACP, 0, pbsz, -1, pwsz, uLength ); \
}
#endif

#define TO_MULTIBYTE( pwsz, pbsz ) \
{ \
   int iLength = wcslen( pwsz) + 1; \
   pbsz = MP_NEW_ARR( char, iLength ); \
   ::WideCharToMultiByte( CP_ACP, 0, pwsz, -1, pbsz, iLength, NULL, NULL ); \
}
#endif // WIN32

#ifndef CHECK_STRING
   #define CHECK_STRING( string )
#endif


namespace gd
{

unsigned int CUTF8String::pdata_nil[] = { -1, 0, 0, NULL };
int CUTF8String::string_m_iTinyAlloc = 64;
int CUTF8String::string_m_iSmallAlloc = 128;
int CUTF8String::string_m_iBigAlloc = 512;

//*************************************************************************************************
CUTF8String::CUTF8String( const char* pbszText )
{
   m_pData = (CUTF8String::_Data*)pdata_nil;
   Add( pbszText );
}

//*************************************************************************************************
#ifdef _WCHAR_T_DEFINED
CUTF8String::CUTF8String( const wchar_t* pwszText )
{
   m_pData = (CUTF8String::_Data*)pdata_nil;
   Add( pwszText );
}
#else
CUTF8String::CUTF8String( const unsigned short* pwszText )
{
   m_pData = (CUTF8String::_Data*)pdata_nil;
   Add( pwszText );
}
#endif

//*************************************************************************************************
CUTF8String::CUTF8String( const CUTF8String& other )
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
CUTF8String& CUTF8String::operator=( const CUTF8String& other )
{
   _ASSERT( other.m_pData->GetRefs() != -2 );

   if( this == &other ) return *this;
   
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
CUTF8String::~CUTF8String()
{
   _Delete();
}

/*---------------------------------------------------------------------------------*/ /**
 * equal operator for comparing two CUTF8String objects
 * \param o reference to CUTF8String that should be compared to
 * \return true if strings are equal or false if not
 */
bool CUTF8String::operator==( const CUTF8String& o ) const
{
   if( m_pData->GetLength() == o.m_pData->GetLength() && 
       memcmp( m_pData->GetText(), o.m_pData->GetText(), m_pData->GetLength() ) == 0 )
   {
      return true;
   }
   return false;
}

#ifdef _MFC_VER
/*---------------------------------------------------------------------------------*/ /**
 * Return text as CString for MFC
 * \return CString object containg text from internal UF8 buffer
 */
CString CUTF8String::MFCGetString()
{
   CString stringMFC;

   //* calculate length
   unsigned uTextLength = CUTF8String::GetTextLength( m_pData->GetText(), m_pData->GetLength() ) + 1;
   CUTF8String::UTF8_to_wchar_t( m_pData->GetText(), stringMFC.GetBufferSetLength( uTextLength ), uTextLength, NULL, NULL );

   return stringMFC;
}
#endif

#ifdef _GD_STRING_H_
/*---------------------------------------------------------------------------------*/ /**
 * Return text as CString for GD
 * \return CString object containg text from internal UF8 buffer
 */
gd::CString CUTF8String::GDGetString()
{
   gd::CString stringGD;

   //* calculate length
   unsigned uTextLength = CUTF8String::GetTextLength( m_pData->GetText(), m_pData->GetLength() ) + 1;
   CUTF8String::UTF8_to_wchar_t( m_pData->GetText(), stringGD.GetBuffer( uTextLength ), uTextLength, NULL, NULL );
   stringGD.SetLength( uTextLength );

   return stringGD;
}
#endif

/*---------------------------------------------------------------------------------*/ /**
 * Set raw utf8 formated text
 * \param pbszUTF8 text that is inserterd to string object
 * \param iLength the length of inserter text, if less than 0 the string is inserted to zero 
 * termination.
 */
void CUTF8String::SetUTF8String( char* pbszUTF8, int iLength )
{
   _SINGLE;
   
   if( iLength < 0 )
   {
      iLength = (int)strlen( pbszUTF8 );
   }

   _AllocAdd( iLength );

   memcpy( m_pData->GetText(), pbszUTF8, iLength );
   m_pData->SetLength( iLength );
   m_pData->End()[0] = 0;
}

/*---------------------------------------------------------------------------------*/ /**
 * Add char character to string
 * \param chCharacter character that is added to string
 */
void CUTF8String::Add( char chCharacter )
{
   _SINGLE;

   char pbsz[2];
   unsigned uLength = UTF8Char( chCharacter, pbsz );
   _AllocAdd( uLength );
   m_pData->End()[uLength+1] = 0;
   m_pData->m_iLength += uLength;
   while( uLength )
   {
      uLength--;
      m_pData->End()[uLength] = pbsz[uLength];
   } // while( uLength ){
}


/*---------------------------------------------------------------------------------*/ /**
 * Add wide character to string
 * \param wchCharacter character that is added to string
 */
#ifdef _WCHAR_T_DEFINED
void CUTF8String::Add( wchar_t wchCharacter )
{
   _SINGLE;

   char pbsz[5];
   unsigned uLength = UTF8Char( wchCharacter, pbsz );
   _AllocAdd( uLength );
   m_pData->End()[uLength+1] = 0;
   m_pData->m_iLength += uLength;
   while( uLength )
   {
      uLength--;
      m_pData->End()[uLength] = pbsz[uLength];
   } // while( uLength ){
}
#else
void CUTF8String::Add( unsigned short wchCharacter )
{
   _SINGLE;

   char pbsz[5];
   unsigned uLength = UTF8Char( wchCharacter, pbsz );
   _AllocAdd( uLength );
   m_pData->End()[uLength+1] = 0;
   m_pData->m_iLength += uLength;
   while( uLength )
   {
      uLength--;
      m_pData->End()[uLength] = pbsz[uLength];
   } // while( uLength ){
}
#endif


/*---------------------------------------------------------------------------------*/ /**
 * Add text from char buffer
 * \param pbszText pointer to buffer where text is added from
 */
void CUTF8String::Add( const char* pbszText )
{
   _SINGLE;

   unsigned uLength = GetUTF8Length( pbszText );

   _AllocAdd( uLength );

   wchar_t_to_UTF8( pbszText, m_pData->End(), uLength, NULL );   
   m_pData->SetLength( m_pData->GetLength() + uLength );
}

/*---------------------------------------------------------------------------------*/ /**
 * Add text from char buffer
 * \param pbszText pointer to buffer where text is added from
 * \param iLength number of characters that is added from buffer
 */
void CUTF8String::Add( const char* pbszText, int iLength )
{
   _SINGLE;

   if( iLength < 0 ) iLength = strlen( pbszText );

   unsigned uLength = GetUTF8Length( pbszText, iLength );

   _AllocAdd( uLength );

   wchar_t_to_UTF8( pbszText, m_pData->End(), uLength, NULL );   
   m_pData->SetLength( m_pData->GetLength() + uLength );
}

/*---------------------------------------------------------------------------------*/ /**
 * Add text from char buffer
 * \param pbszText pointer to buffer where text is added from
 */
#ifdef _WCHAR_T_DEFINED
void CUTF8String::Add( const wchar_t* pwszText )
{
   _SINGLE;

   unsigned uLength = GetUTF8Length( pwszText );

   _AllocAdd( uLength );

   wchar_t_to_UTF8( pwszText, m_pData->End(), uLength, NULL );   
   m_pData->SetLength( m_pData->GetLength() + uLength );
}
#else
void CUTF8String::Add( const unsigned short* pwszText )
{
   _SINGLE;

   unsigned uLength = GetUTF8Length( pwszText );

   _AllocAdd( uLength );

   wchar_t_to_UTF8( pwszText, m_pData->End(), uLength, NULL );   
   m_pData->SetLength( m_pData->GetLength() + uLength );
}
#endif

/*---------------------------------------------------------------------------------*/ /**
 * Add text from char buffer
 * \param pwszText pointer to buffer where text is added from
 * \param iLength number of characters that is added from buffer
 */
#ifdef _WCHAR_T_DEFINED
void CUTF8String::Add( const wchar_t* pwszText, int iLength )
{
   _SINGLE;

   if( iLength < 0 ) iLength = wcslen( pwszText );

   unsigned uLength = GetUTF8Length( pwszText, iLength );

   _AllocAdd( uLength );

   wchar_t_to_UTF8( pwszText, m_pData->End(), uLength, NULL );   
   m_pData->SetLength( m_pData->GetLength() + uLength );
}
#else
void CUTF8String::Add( const unsigned short* pwszText, int iLength )
{
   _SINGLE;

   if( iLength < 0 ) iLength = wcslen( pwszText );

   unsigned uLength = GetUTF8Length( pwszText, iLength );

   _AllocAdd( uLength );

   wchar_t_to_UTF8( pwszText, m_pData->End(), uLength, NULL );   
   m_pData->SetLength( m_pData->GetLength() + uLength );
}
#endif


/*---------------------------------------------------------------------------------*/ /**
 * Insert normal text into CUTF8String object
 * \param uPosition position in utf8 string where text should be inserted
 * \param pszText text that is inserted to utf8 string
 * \param uRepeat how many times text should be inserted
 */
void CUTF8String::Insert( unsigned uPosition, const wchar_t* pszText, unsigned uRepeat )
{
   _SINGLE;

   //* find position in UTF8 text
   char* pszPosition = GotoUTF8Char( uPosition, m_pData->GetText() );
   
   uPosition = (unsigned)(pszPosition - m_pData->GetText());

   //* convert text that should be inserterd to utf8
   unsigned uLength = GetUTF8Length( pszText );            // get utf8 length
   unsigned uSumLength = uLength * uRepeat;                // calculate total length
   char* pszBuffer = MP_NEW_ARR( char, uLength + 1 );       // allocate buffer for utf8 text

   _AllocAdd( uSumLength );

   wchar_t_to_UTF8( pszText, pszBuffer, uLength );            // convert text

   if( *pszPosition == 0 )
   {
      pszPosition = m_pData->End();
   }
   else
   {
      pszPosition = &m_pData->GetText()[uPosition];
      memmove( &pszPosition[uSumLength], pszPosition, (m_pData->GetLength() - uPosition) );
   }

   while( uRepeat != 0 )
   {
      uRepeat--;
      memcpy( pszPosition, pszBuffer, uLength );
      pszPosition += uLength;
   }

   m_pData->AddLength( uSumLength );
   m_pData->End()[0] = 0;

}

/*---------------------------------------------------------------------------------*/ /**
 * Find text in internal buffer
 * \param pbszFindText text that should be found
 * \param uStart where to start search for text
 * \return index for start if found text or -1 if not found
 */
int CUTF8String::Find( const char* pbszFindText, unsigned uStart ) const
{
   unsigned uLength = GetUTF8Length( pbszFindText ) + 1;

   char* pszFind = MP_NEW_ARR( char, uLength );               // allocate buffer for utf8 text
   wchar_t_to_UTF8( pbszFindText, pszFind, uLength );          // convert text

   return FindUTF8( pszFind, uStart );
}

/*---------------------------------------------------------------------------------*/ /**
 * Find text in internal buffer
 * \param pwszFindText text that should be found
 * \param uStart where to start search for text
 * \return index for start if found text or -1 if not found
 */
#ifdef _WCHAR_T_DEFINED
int CUTF8String::Find( const wchar_t* pwszFindText, unsigned uStart ) const
{
   unsigned uLength = GetUTF8Length( pwszFindText ) + 1;

   char* pszFind = MP_NEW_ARR( char, uLength );               // allocate buffer for utf8 text
   wchar_t_to_UTF8( pwszFindText, pszFind, uLength );          // convert text

   return FindUTF8( pszFind, uStart );
}
#else
int CUTF8String::Find( const unsigned short* pwszFindText, unsigned uStart ) const
{
   unsigned uLength = GetUTF8Length( pwszFindText ) + 1;

   char* pszFind = MP_NEW_ARR(char, uLength );               // allocate buffer for utf8 text
   wchar_t_to_UTF8( pwszFindText, pszFind, uLength );          // convert text

   return FindUTF8( pszFind, uStart );
}
#endif

/*---------------------------------------------------------------------------------*/ /**
 * find utf8 text in string
 * \param pbszUTF8Find utf8 formated text that is searched for
 * \param uStart uStart where to start search for text
 * \return index for start if found text or -1 if not found
 */
int CUTF8String::FindUTF8( const char* pbszUTF8Find, unsigned uStart ) const
{
   char* pszText;
   if( uStart == 0 )
   {
      pszText = m_pData->GetText();
   }
   else
   {
      pszText = GotoUTF8Char( uStart, m_pData->GetText() );
      if( *pszText == 0 ) return -1;
   }

   pszText = m_pData->GetText();

   while( *pszText != 0 )
   {
      if( *pszText == *pbszUTF8Find )
      {
         unsigned uFind = 0;
         char* pszTest = pszText;
         while( (pszTest[uFind] == pbszUTF8Find[uFind]) && (pbszUTF8Find[uFind] != 0) ) uFind++;

         if( pbszUTF8Find[uFind] == 0 )
         {
            return GetTextLength( m_pData->GetText(), (unsigned)(pszText - m_pData->GetText()) );
         }
      }
      pszText++;
   }

   return -1;
}


/*---------------------------------------------------------------------------------*/ /**
 * Return string object that starts at start index and has the given length.
 * \param iStart start position where extracted string begins
 * \param iLength length for extracted string.
 * \return String object containing text from start position at specified index with the length
   from there
\code
CUTF8String s("0123456789");
CUTF8String sInner = s.Mid( 2, 3 );
// sInner = "234"
CUTF8String sLeft = s.Left( 4 );
// sLeft = "0123"
\endcode
 */
CUTF8String CUTF8String::Mid( unsigned uStart, int iLength ) const
{
   CUTF8String string;

   //* find position in UTF8 text
   char* pbszPosition = GotoUTF8Char( uStart, m_pData->GetText() );

   if( pbszPosition < m_pData->End() )
   {
      if( iLength < 0 )
      {
         string.SetUTF8String( pbszPosition, (int)(m_pData->End() - pbszPosition) );
      }
      else
      {
         char* pbszEnd = GotoUTF8Char( iLength, pbszPosition );
         string.SetUTF8String( pbszPosition, (int)(pbszEnd - pbszPosition) );
      }
   }

   return string;
}

/*---------------------------------------------------------------------------------*/ /**
 * Empty string
 */
void CUTF8String::Empty()
{
   if( m_pData->GetRefs() > 1 )
   {
      m_pData->Release();
      m_pData = (CUTF8String::_Data*)pdata_nil;
   }
   else if( m_pData->GetLength() > 0 )
   {
      m_pData->SetLength( 0 );
      m_pData->End()[0] = 0;
   }
}



static const CUTF8String::MaskItem _utf8string_pMaskItem[] = 
{
  { 0x80,  0*6, 1, 0x7F,   },   // 1 byte sequence
  { 0xC0,  1*6, 2, 0x7FF,  },   // 2 byte sequence
  { 0xE0,  2*6, 3, 0xFFFF, },   // 3 byte sequence
  { 0xF0,  3*6, 4, 0x1FFFFF, }, // 4 byte sequence
};

#define UTF_TRAIL( value, n ) ( (char)(0x80 | (value >> (n * 6))) )
/*---------------------------------------------------------------------------------*/ /**
 * Goto to specified position in UTF8 text
 * \param uIndex  index in UTF8 text
 * \param pszText UTF8 text
 * \return pointer to position in text
 */
char* CUTF8String::GotoUTF8Char( unsigned uIndex, char* pszText )
{
   while( *pszText && uIndex )
   {
      if( !(*pszText & 0x80) ) 
      {
         pszText++;
      } // if( !(*pszText & 0x80) ){
      else if( *pszText & 0xC0 ) 
      {
         pszText += 2;
      } // if( *pszText & 0xC0 ){
      else if( *pszText & 0xE0 ) 
      {
         pszText += 3;
      } // if( *pszText & 0xE0 ){
      else
      {
         pszText += 4;
      }
      uIndex--;
   } // while( *pszText && uIndex ){

   return pszText;
}

/*---------------------------------------------------------------------------------*/ /**
 * Convert character to UTF-8 format <br>
 * \param uChar charcter that is converterd
 * \param pszBuffer buffer that gets UTF-8 characters
 */
unsigned CUTF8String::UTF8Char( unsigned uChar, char* pszBuffer )
{
   if( uChar <= 0x7F ) 
   {
      *pszBuffer = (char)uChar;
      return 1;
   }
   else if( uChar <= 0x7FF )
   {
      *pszBuffer = (char)(0xC0 | (uChar >> 6));
      pszBuffer++;
      *pszBuffer = (char)(uChar & 0xBF);
      return 2;
   } // if uChar <= 0x7FF ){
   else if( uChar <= 0xFFFF )
   {
      *pszBuffer = (char)(0xE0 | (uChar >> 12));
      pszBuffer++;
      *pszBuffer = UTF_TRAIL( uChar, 1 );
      pszBuffer++;
      *pszBuffer = UTF_TRAIL( uChar, 0 );
      return 3;
   } // if(  uChar <= 0xFFFF ){
   else if( uChar <= 0x1FFFFF )
   {
      *pszBuffer = (char)(0xF0 | (uChar >> 18));
      pszBuffer++;
      *pszBuffer = UTF_TRAIL( uChar, 2 );
      pszBuffer++;
      *pszBuffer = UTF_TRAIL( uChar, 1 );
      pszBuffer++;
      *pszBuffer = UTF_TRAIL( uChar, 0 );
      return 4;
   } // if( uChar <= 0x1FFFFF ){
   
   _ASSERT( FALSE );
   return 0;                                                                                 
}                                                                                            

/*---------------------------------------------------------------------------------*/ /**
 * calculate how many characters that is needed for wide character string
 * \param pwszWC  pointer to string that that UTF8 length is calculated for
 * \param iMax    max number of characters in string that should be checked for UTF8 length
 * \return number of characters in UTF8 format 
 */
#ifdef _WCHAR_T_DEFINED
unsigned CUTF8String::GetUTF8Length( const wchar_t* pwszWC, int iMax )
{
   const wchar_t* pwszEnd;
   unsigned uLength;

   if( iMax > 0 ) pwszEnd = pwszWC + iMax;
   else pwszEnd = pwszWC + wcslen( pwszWC );

   uLength = 0;
   while( (pwszWC < pwszEnd) )
   {
      if( *pwszWC <= 0x7F ) 
      {
         uLength++;
      } // if( *pwszWC <= 0x7F ){
      else if( *pwszWC <= 0x7FF )  
      {
         uLength += 2;
      } // if( *pwszWC <= 0x7FF ){
      else if( *pwszWC <= 0xFFFF )
      {
         uLength += 3;
      } // if( *pwszWC <= 0xFFFF ){
      else
      {
         uLength += 4;
      }
      pwszWC++;
   } // while( (*pwszWC != L'\0') && (pwszWC != pwszEnd) ){

   return uLength;
}
#else
unsigned CUTF8String::GetUTF8Length( unsigned short* pwszWC, int iMax )
{
   const unsigned short* pwszEnd;
   unsigned uLength;

   if( iMax > 0 ) pwszEnd = pwszWC + iMax;
   else pwszEnd = pwszWC + wcslen( pwszWC ) + 1;

   uLength = 0;
   while( (pwszWC < pwszEnd) )
   {
      if( *pwszWC <= 0x7F ) 
      {
         uLength++;
      } // if( *pwszWC <= 0x7F ){
      else if( *pwszWC <= 0x7FF )  
      {
         uLength += 2;
      } // if( *pwszWC <= 0x7FF ){
      else if( *pwszWC <= 0xFFFF )
      {
         uLength += 3;
      } // if( *pwszWC <= 0xFFFF ){
      else
      {
         uLength += 4;
      }
      pwszWC++;
   } // while( (*pwszWC != L'\0') && (pwszWC != pwszEnd) ){

   return uLength;
}
#endif

//*************************************************************************************************
/*! calculate how many characters that is needed for multibyte character string
    \return number of characters in UTF8 format */
unsigned CUTF8String::GetUTF8Length( 
   const char* pbszMB, //!< pointer to string that that UTF8 length is calculated for
   int iMax //!< max number of characters in string that should be checked for UTF8 length
   )
{
   const char* pbszEnd;
   unsigned uLength;

   uLength = 0;
   if( iMax >= 0 )
   {
      pbszEnd = pbszMB + iMax;
      while( pbszMB < pbszEnd )
      {
         if( (unsigned char)*pbszMB < 0x80 ) 
         {
            pbszMB++;
            uLength++;
            continue;
         }
         else
         {
            uLength += 2;
            pbszMB++;
         }
      } 
   } // if( iMax > 0 ){
   else
   {
      while( *pbszMB != L'\0' )
      {
         if( (unsigned char)*pbszMB < 0x80 ) 
         {
            pbszMB++;
            uLength++;
            continue;
         }
         else
         {
            uLength += 2;
            pbszMB++;
         }
      } 
   }

   return uLength;
}

/*---------------------------------------------------------------------------------*/ /**
 * Calculate the length in normal characters for the utf8 text
 * \param pbszUTF8 text that is calculated to how long it is in normal characters
 * \param iMax where to stop calculating
 * \return how many characters that existed in specified utf8 buffer
 */
unsigned CUTF8String::GetTextLength( const char* pbszUTF8, int iMax )
{
   unsigned uLength = 0;
   const char* pbszEnd = NULL;
   if( iMax >= 0 ) pbszEnd = pbszUTF8 + iMax;

   while( *pbszUTF8 && pbszUTF8 != pbszEnd )
   {
      // check first and second bit. could never be 10
      _ASSERT( !((*pbszUTF8 & 0xC0) == 0x80) ); // 10111111 ERROR!

      uLength++;
      if( (unsigned char)*pbszUTF8 <= 0x7F ) 
      {
         pbszUTF8++;
      } // if( *pbszUTF8 <= 0x7F ){
      else if( (*pbszUTF8 & 0xF0) == 0xC0 )
      {
         pbszUTF8+=2;
      }
      else if( (*pbszUTF8 & 0xF0) == 0xE0 )
      {
         pbszUTF8+=3;
      }
      else
      {
         pbszUTF8+=4;
      }
   } // while( *pbszUTF8 && pbszUTF8 != pbszEnd ){

   return uLength;
}

//*************************************************************************************************
/*! Convert string from unicode to UTF-8 */
void CUTF8String::wchar_t_to_UTF8(
   const wchar_t* pwszWC, //!< pointer to unicode string that is converterd
   char* pbszUTF8, //!< pointer to UTF8 buffer string is converterd to
   unsigned uMaxLength, //!< max length of UTF8 buffer that can receivie characters
   const wchar_t* pwszEnd //!< pointer to end of unicode string 
   )
{
   char* pbszMax;
   unsigned uShift;
   const MaskItem* pMaskItem;

   pbszMax = pbszUTF8 + uMaxLength;

   while( (*pwszWC != L'\0') && (pwszWC != pwszEnd) && (pbszUTF8 < pbszMax) )
   {
      if( *pwszWC < 0x80 ) 
      {
         *pbszUTF8 = (char)*pwszWC;
         pbszUTF8++;
      }
      else
      {
         for( pMaskItem = &_utf8string_pMaskItem[1]; pMaskItem->chMask; pMaskItem++ )
         {
            if( *pwszWC < pMaskItem->wcMax ) break;
         }

         // Check size for buffer
         if( pbszUTF8 > (pbszMax - pMaskItem->uSize) ) break;

         uShift = pMaskItem->uShift;
         *pbszUTF8 = (char)(pMaskItem->chMask | (*pwszWC >> uShift));
         pbszUTF8++;
         while( uShift > 0 )
         {
            uShift -= 6;
            *pbszUTF8 = (char)(0x80 | (*pwszWC >> uShift)) & 0xBF;
            pbszUTF8++;
         }
      }
      pwszWC++;
   }

   *pbszUTF8 = '\0';
}

//*************************************************************************************************
/*! Convert string from multibyte to UTF-8 */
void CUTF8String::wchar_t_to_UTF8(
   const char* pbszMB, //!< pointer to multibyte string that is converterd
   char* pbszUTF8, //!< pointer to UTF8 buffer string is converterd to
   unsigned uMaxLength, //!< max length of UTF8 buffer that can receivie characters
   const char* pbszEnd //!< pointer to end of multibyte string 
   )
{
   char* pbszMax;

   pbszMax = pbszUTF8 + uMaxLength;

   while( (*pbszMB != L'\0') && (pbszMB != pbszEnd) && (pbszUTF8 < pbszMax) )
   {
      if( (unsigned char)*pbszMB < 0x80 ) 
      {
         *pbszUTF8 = (char)*pbszMB;
         pbszUTF8++;
      }
      else
      {
         // Check size for buffer
         if( pbszUTF8 > (pbszMax - 1) ) break;

         *pbszUTF8 = (char)(0xC0 | (*(unsigned char*)pbszMB >> 6));
         pbszUTF8++;
         *pbszUTF8 = (char)(0x80 | ((unsigned char)*pbszMB) & 0x3F);
         pbszUTF8++;
      }
      pbszMB++;
   }

   *pbszUTF8 = '\0';
}


//*************************************************************************************************
/*TYPE:  
PURPOSE: 
RETURN:  */
HRESULT CUTF8String::UTF8_to_wchar_t(
   const char* pbszUTF8, // string that is converterd
   wchar_t* pwszWC, // buffer string is converterd to
   unsigned uMaxLength, // maximum lenght of buffer that gets converterd UTF-8 text
   unsigned* puCount,  // number of characters that are converterd
   char const ** ppbszEnd // will point to last character that was converterd in UTF-8 buffer
   )
{
   unsigned uLength;
   wchar_t* pwszEnd;
   wchar_t* pwszStart;
   char chTemp;

   pwszStart = pwszWC;
   pwszEnd = pwszWC + uMaxLength - 1;

   // * go through the characters. end if UTF-8 string is null or if
   // * max number of characters has been reached.
   while( (*pbszUTF8 != '\0') && (pwszWC != pwszEnd) )
   {
      if( (*pbszUTF8 & 0x80) == 0 )
      {
         *pwszWC = *pbszUTF8;
         pbszUTF8++;
      }
      else
      {
         // Calculate bits
         //chTemp = (char)0xF0;
         if( (*pbszUTF8 & 0xF0) == 0xF0 ) { uLength = 4; chTemp = (char)0xF0; }
         else if( (*pbszUTF8 & 0xE0) == 0xE0 ) { uLength = 3; chTemp = (char)0xE0; }
         else if( (*pbszUTF8 & 0xC0) == 0xC0 ) { uLength = 2; chTemp = (char)0xC0; }
         else return E_FAIL;

         // Check if UTF8 string is complete
         if( (pbszUTF8 + uLength) > *ppbszEnd )
         {
            *ppbszEnd = pbszUTF8;
            goto exit;
         }

         // Build character
         // The first byte needs to clear part that has count of bytes for the UTF8 character.
         // This is done by clearing the number bits for each byte + 1 from right that the character
         // are taken.
         // Then we shift the final character one octal bitlenght for each byte that is added.
         uLength--;
         *pwszWC = (*pbszUTF8 & ~chTemp) << (uLength * 6);

         pbszUTF8++; uLength--;
         *pwszWC |= (*pbszUTF8 & 0x3F) << (uLength * 6);
         if( uLength )
         {
            pbszUTF8++; uLength--;
            *pwszWC |= (*pbszUTF8 & 0x3F) << (uLength * 6);
            if( uLength )
            {
               pbszUTF8++; uLength--;
               *pwszWC |= (*pbszUTF8 & 0x3F);
            }
         }
         pbszUTF8++;
      }
      pwszWC++;
   }

exit:
   *pwszWC = L'\0';

   if( ppbszEnd != NULL ) *ppbszEnd = pbszUTF8;
   if( puCount != NULL ) *puCount = (unsigned)(pwszWC - pwszStart);

   return S_OK;
}

//*************************************************************************************************
/*! Convert UTF8 string to normal char* string  */
HRESULT CUTF8String::UTF8_to_wchar_t(
   const char* pbszUTF8, //!< string that is converterd
   char* pbszMB, //!< buffer string is converterd to
   unsigned uMaxLength, //!< maximum lenght of buffer that gets converterd UTF-8 text
   unsigned* puCount,  //!< number of characters that are converterd
   char const ** ppbszEnd //!< will point to last character in UTF-8 buffer
   )
{
   unsigned uLength;
   char* pbszEnd;
   char* pbszStart;
   const char* pbszUTF8End = (char*)-1;
   unsigned char chMask;

   pbszStart = pbszMB;
   pbszEnd = pbszMB + uMaxLength - 1;

   if( ppbszEnd != NULL ) pbszUTF8End = *ppbszEnd;


   // * go through the characters. end if UTF-8 string is null or if
   // * max number of characters has been reached.
   while( (*pbszUTF8 != '\0') && (pbszMB != pbszEnd) )
   {
      if( ((unsigned char)*pbszUTF8 & 0x80) == 0 )
      {
         *pbszMB = *pbszUTF8;
         pbszUTF8++;
      }
      else
      {
         // Calculate bits
         //chTemp = (char)0xF0;
         if( ((unsigned char)*pbszUTF8 & 0xe0) == 0xc0 )
         {
            uLength = 1;
            chMask = 0x1f;
            //uValue |= (*pbszUTF8 & 0x03) << 6;
         }
         else if( ((unsigned char)*pbszUTF8 & 0xf0) == 0xe0 )
         {
            uLength = 2;
            chMask = 0x0f;
         }
         else if( ((unsigned char)*pbszUTF8 & 0xf8) == 0xf0 )
         {
            uLength = 3;
            chMask = 0x07;
         }
         else
         {
            return E_FAIL;
         }
         unsigned uValue = ((unsigned char)*pbszUTF8 & chMask) << (uLength * 6);
         pbszUTF8++;

         // Check if UTF8 string is complete
         if( (pbszUTF8 + uLength) > pbszUTF8End )
         {
            pbszUTF8--;
            *ppbszEnd = pbszUTF8;
            goto exit;
         }

         // Build character
         // The first byte needs to clear part that has count of bytes for the UTF8 character.
         // This is done by clearing the number bits for each byte + 1 from right that the character
         // are taken.
         // Then we shift the final character one octal bitlenght for each byte that is added.
         while( uLength )
         {
            uLength--;
            uValue |= (*pbszUTF8 & 0x3f) << (uLength * 6);
            pbszUTF8++;
         }
         *pbszMB =  (unsigned char)uValue;
      }
      pbszMB++;
   }

exit:
   *pbszMB = L'\0';

   if( ppbszEnd != NULL ) *ppbszEnd = pbszUTF8;
   if( puCount != NULL ) *puCount = pbszMB - pbszStart;

   return S_OK;
}


/*---------------------------------------------------------------------------------*/ /**
 * Check string for valid UTF-8 format
 * \param pbszUTF8 pinter to UTF-8 text
 * \param uLength Length for string that is checked
 * \param pbOk Pointer to bool value that returns if text is ok(true)  or not(false).
 * \return 0 if string was ok, otherwise position where the text had character error
 */
unsigned CUTF8String::CheckUTF8( const char* pbszUTF8, unsigned uLength, bool* pbOk )
{
   const char* pbszEnd;

   unsigned uCheck;
   if( pbOk != NULL ) *pbOk = true;
   pbszEnd = pbszUTF8 + uLength;

   while( (*pbszUTF8 != '\0') && (pbszUTF8 < pbszEnd) )
   {
      if( (unsigned char)*pbszUTF8 & 0x80 )
      {
         if( ((unsigned char)*pbszUTF8 & 0xF0) == 0xF0 ) uCheck = 3;
         else if( ((unsigned char)*pbszUTF8 & 0xE0) == 0xE0 ) uCheck = 2;
         else if( ((unsigned char)*pbszUTF8 & 0xC0) == 0xC0 ) uCheck = 1;
         else goto error;
         pbszUTF8++;
         while( uCheck > 0 )
         {
            uCheck--;
            if( *pbszUTF8 == '\0' )
            {
               return 0;
            }

            if( ((unsigned char)*pbszUTF8 & 0xC0) != 0x80 )
            {
               goto error;
            }
            pbszUTF8++;
         }
      }
      else
      {
         pbszUTF8++;
      }
   }

   return 0;

error:
   _ASSERT( FALSE );
   if( pbOk != NULL ) *pbOk = false;
   return (unsigned)(pbszUTF8 - (pbszEnd - uLength));
}


//*************************************************************************************************
/*TYPE:  PRIVATE
PURPOSE: Make sure that string has only one reference */
void CUTF8String::_Safe()
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
void CUTF8String::_AllocAdd( int iSize )
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
      pData = (_Data*) MP_NEW_ARR( BYTE, iSize);
      memcpy( pData, m_pData, sizeof( _Data ) + (m_pData->GetLength() + 1) * sizeof(wchar_t) );
      pData->m_iMaxLength = iSize - sizeof(_Data);

      BYTE* pDataDel = (BYTE*)m_pData;
	  MP_DELETE_ARR( pDataDel);
      m_pData = pData;
      _SETPSZ();
   }
}

//*************************************************************************************************
/*TYPE:  PRIVATE
PURPOSE:  delete string object  */
void CUTF8String::_Delete()
{
   if( m_pData != _NOTHING )
   {
      m_pData->Release();
      m_pData = _NOTHING;
      _SETPSZ();
   }
}


CUTF8String::_Data* CUTF8String::_Data::Safe() 
{
   _Data* pData;
   int iLength;
   if( m_iRefs != 1 ) 
   {
      iLength = sizeof(_Data) + (1 + m_iLength);
      iLength += 32 - iLength % 32;
      pData = (_Data*) MP_NEW_ARR( BYTE, iLength);
      pData->m_iMaxLength = iLength - sizeof( _Data );
      memcpy( pData->GetText(), GetText(), (GetLength() + 1) );
      pData->m_iLength = GetLength();
      pData->m_iRefs = 1;
      if( m_iRefs > 1 ) m_iRefs--;
      return pData;
   }
   return this; 
}

#ifdef _DEBUG
/*---------------------------------------------------------------------------------*/ /**
 * Dump internal uft8 text as hex codes.<br>
 * If internal buffer is over 100 bytes long then only the first 100 are dumped. Make sure TRACE 
 * is defined.
 */
void CUTF8String::HexDump()
{
	static const wchar_t pszHex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

   unsigned uLength = m_pData->GetLength();
   // allocate buffer for hex string
   if( uLength > 100 ) uLength = 100;
   wchar_t* pszDump = MP_NEW_ARR( wchar_t, (uLength * 3 + 1));
   
   wchar_t* pszPos = pszDump;
   unsigned char uc;
   for( unsigned u = 0; u < uLength; u++ )
   {
      uc = (unsigned char)m_pData->GetText()[u];
		*pszPos++ = pszHex[(uc >> 4) & 0x0F];
		*pszPos++ = pszHex[uc & 0x0F];

      *pszPos++ = L' ';
   } // for( unsigned u = 0; u < uLength; u++ ){

   *pszPos = L'\0';

#ifdef TRACE
   if( pszPos != pszDump )
   {
      TRACE( _T("%s"), pszDump );
      if( m_pData->GetLength() > 100 )
      {
         TRACE( _T(" ...") );
      }
      TRACE( _T("\n") );
   } // if( pszPos != pszDump ){
   else
   {
      TRACE( _T("The CUTF8String object is empty.\n") );
   }
#endif
}

#endif // _DEBUG


}; // namespace gd
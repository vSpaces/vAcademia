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

#include "mlRMML.h" // for precompile heades only
#include "gd\gd_XMLArchive.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#pragma warning( disable:  4701 )

#define _UNICODE
#undef _T 
#define _T(x) L##x

#define _XML_SPACE  0x01
#define _XML_WWW    0x20
#define _XML_ENTITY 0x10
#define _XML_MAX_WATCH 63

namespace gd_xml_stream
{

unsigned char pszWatchFor[] = { 
   0x00, // 0
   0x00, // 1
   0x00, // 2
   0x00, // 3
   0x00, // 4
   0x00, // 5
   0x00, // 6
   0x00, // 7
   0x00, // 8
   0x01, // 9 (space)(tab)
   0x01, // 10 (space) 
   0x00, // 11
   0x00, // 12
   0x01, // 13 (space)
   0x00, // 14
   0x00, // 15
   0x00, // 16
   0x00, // 17
   0x00, // 18
   0x00, // 19
   0x00, // 20
   0x00, // 21
   0x00, // 22
   0x00, // 23
   0x00, // 24
   0x00, // 25
   0x00, // 26
   0x00, // 27
   0x00, // 28
   0x00, // 29
   0x00, // 30
   0x00, // 31
   0x01, // 32 (space) change to 0x11 if space is a entity
   0x40, // 33 !
   0x10, // 34 "
   0x00, // 35
   0x00, // 36
   0x00, // 37
   0x10, // 38 &
   0x00, // 39 '
   0x00, // 40
   0x00, // 41
   0x00, // 42
   0x00, // 43
   0x00, // 44
   0x00, // 45
   0x00, // 46
   0x80, // 47 /
   0x00, // 48
   0x00, // 49
   0x00, // 50
   0x00, // 51
   0x00, // 52
   0x00, // 53
   0x00, // 54
   0x00, // 55
   0x00, // 56
   0x00, // 57
   0x00, // 58
   0x00, // 59
   0x10, // 60 <
   0x00, // 61
   0x10, // 62 >
   0x00, // 63
   0x00, // 64
   0x00, // 65
   0x00, // 66
   0x00, // 67
   0x00, // 68
   0x00, // 69
   0x00, // 70
   0x00, // 71
   0x20, // 72 H
   0x00, // 73
   0x00, // 74
   0x00, // 75
   0x00, // 76
   0x00, // 77
   0x00, // 78
   0x00, // 79
   0x00, // 80
   0x00, // 81
   0x00, // 82
   0x00, // 83
   0x00, // 84
   0x00, // 85
   0x00, // 86
   0x20, // 87 W
   0x00, // 88
   0x00, // 89
   0x00, // 90
   0x00, // 91
   0x00, // 92
   0x00, // 93
   0x00, // 94
   0x00, // 95
   0x00, // 96
   0x00, // 97
   0x00, // 98
   0x00, // 99
   0x00, // 100
   0x00, // 101
   0x00, // 102
   0x00, // 103
   0x20, // 104 h
   0x00, // 105
   0x00, // 106
   0x00, // 107
   0x00, // 108
   0x00, // 109
   0x00, // 110
   0x00, // 111
   0x00, // 112
   0x00, // 113
   0x00, // 114
   0x00, // 115
   0x00, // 116
   0x00, // 117
   0x00, // 118
   0x20, // 119 w
};


__forceinline int _stringstrncmp( const wchar_t* psz1, const wchar_t* psz2, size_t count )
{
   while( *psz1 == *psz2 && count )
   {
      psz1++;
      psz2++;
      count--;
   }

   return !(*psz1 == *psz2);
}

int CopyXMLText( const wchar_t* pszIn, wchar_t* pszTo, UINT uMaxLength, wchar_t chEnd )
{
   wchar_t ch;
   UINT uAdd;
   UINT uIndex;

   uIndex = 0;
   uMaxLength--;
   while( *pszIn != chEnd && uIndex < uMaxLength )
   {
      if( *pszIn != L'&' )
      {
         if( pszTo != NULL ) pszTo[uIndex] = *pszIn;
         pszIn++;
         uIndex++;
         continue;
      }
      else
      {
         pszIn++;
         uAdd = 0;
         /*
         if( _stringstrncmp( pszIn, L"nbsp;", 4 ) == 0 )
         {
            ch = L' ';
            uAdd = 4;
         }
         else 
         */if( _stringstrncmp( pszIn, L"gt;", 2 ) == 0 )
         {
            ch = L'>';
            uAdd = 2;
         }
         else if( _stringstrncmp( pszIn, L"lt;", 2 ) == 0 )
         {
            ch = L'<';
            uAdd = 2;
         }
         else if( _stringstrncmp( pszIn, L"amp;", 3 ) == 0 )
         {
            ch = L'&';
            uAdd = 3;
         }
         else if( _stringstrncmp( pszIn, L"quot;", 4 ) == 0 )
         {
            ch = L'"';
            uAdd = 4;
         }
         else
         {
            _ASSERT(FALSE);
         }

         if( uAdd != 0 )
         {
            if( pszTo != NULL ) pszTo[uIndex] = ch;
            uIndex++;
            pszIn += uAdd;
         }
         else
         {
            pszIn--;
            pszTo[uIndex] = *pszIn;
            uIndex++;
         }
      }
      pszIn++;
   }

   pszTo[uIndex] = 0;

   return uIndex;
}

void CopyXMLText( const wchar_t* pszIn, gd::CString* pstringText, wchar_t chEnd )
{
   wchar_t ch;
   UINT uAdd;

   pstringText->Single();

   while( *pszIn != chEnd )
   {
      if( *pszIn != L'&' )
      {
         pstringText->FastAdd( *pszIn );
         pszIn++;
         continue;
      }
      else
      {
         pszIn++;
         uAdd = 0;
         if( _stringstrncmp( pszIn, L"gt;", 2 ) == 0 )
         {
            ch = L'>';
            uAdd = 2;
         }
         else if( _stringstrncmp( pszIn, L"lt;", 2 ) == 0 )
         {
            ch = L'<';
            uAdd = 2;
         }
         else if( _stringstrncmp( pszIn, L"amp;", 3 ) == 0 )
         {
            ch = L'&';
            uAdd = 3;
         }
         else if( _stringstrncmp( pszIn, L"quot;", 4 ) == 0 )
         {
            ch = L'"';
            uAdd = 4;
         }
         else
         {
            _ASSERT(FALSE);
         }

         if( uAdd != 0 )
         {
            pstringText->FastAdd( ch );
            pszIn += uAdd;
         }
         else
         {
            pszIn--;
            pstringText->FastAdd( *pszIn );
         }
      }
      pszIn++;
   }
}

UINT XMLToTextLength( const wchar_t* pszIn, wchar_t chEnd )
{
   UINT uLength = 0;

   while( *pszIn != chEnd )
   {
      if( *pszIn != L'&' )
      {
         uLength++;
         pszIn++;
         continue;
      }
      else
      {
         pszIn++;
         if( _stringstrncmp( pszIn, L"gt;", 2 ) == 0 )
         {
            uLength++;
            pszIn += 3;
         }
         else if( _stringstrncmp( pszIn, L"lt;", 2 ) == 0 )
         {
            uLength++;
            pszIn += 3;
         }
         else if( _stringstrncmp( pszIn, L"amp;", 3 ) == 0 )
         {
            uLength++;
            pszIn += 4;
         }
         else if( _stringstrncmp( pszIn, L"quot;", 4 ) == 0 )
         {
            uLength++;
            pszIn += 5;
         }
         else
         {
            _ASSERT(FALSE);
         }
      }
   }

   return uLength;
}

bool HasXML( const wchar_t* pszXML )
{
   while( *pszXML != 0 )
   {
      if( *pszXML < _XML_MAX_WATCH && pszWatchFor[*pszXML] & _XML_ENTITY ) return true;

      pszXML++;
   }

   return false;
}

bool HasHTML( const wchar_t* pszText )
{
   while( *pszText != 0 )
   {
      if( *pszText < _XML_MAX_WATCH && pszWatchFor[*pszText] & _XML_ENTITY ) return true;
      if( *pszText > L'z' ) return true;
      pszText++;
   }

   return false;
}


void TextToXML( const wchar_t* pszIn, gd::CString* pstringXML )
{
   pstringXML->Single();

   while( *pszIn != 0 )
   {
      if( (unsigned)*pszIn >= _XML_MAX_WATCH )
      {
         pstringXML->FastAdd( *pszIn );
         pszIn++;
         continue;
      }
      else
      {
         if( pszWatchFor[*pszIn] & _XML_ENTITY )
         {
            switch( *pszIn )
            {
            //case L' ' : *pstringXML += L"&nbsp;"; break;
            case L'<' : pstringXML->FastAdd( L"&lt;" ); break;
            case L'>' : pstringXML->FastAdd( L"&gt;" ); break;
            case L'&' : pstringXML->FastAdd( L"&amp;" ); break;
            //case L'\'' : *pstringXML += L"&apos;"; break;
            case L'"' : pstringXML->FastAdd( L"&quot;" ); break;
            }
         }
         else
         {
            *pstringXML += *pszIn;
         }
      }

      pszIn++;
   } // end while
}

gd::CString TextToXML( const wchar_t* pszIn )
{
   gd::CString string;
   string.Single();

   while( *pszIn != 0 )
   {
      if( (unsigned)*pszIn >= _XML_MAX_WATCH )
      {
         string.FastAdd( *pszIn );
         pszIn++;
         continue;
      }
      else
      {
         if( pszWatchFor[*pszIn] & _XML_ENTITY )
         {
            switch( *pszIn )
            {
            //case L' ' : *pstringXML += L"&nbsp;"); break;
            case L'<' : string.FastAdd( L"&lt;" ); break;
            case L'>' : string.FastAdd( L"&gt;" ); break;
            case L'&' : string.FastAdd( L"&amp;" ); break;
            //case L'\'' : *pstringXML += L"&apos;"); break;
            case L'"' : string.FastAdd( L"&quot;" ); break;
            }
         }
         else
         {
            string += *pszIn;
         }
      }

      pszIn++;
   } // end while

   return string;
}

UINT TextToXMLLength( const wchar_t* pszIn )
{
   UINT uLength = 0;

   while( *pszIn != 0 )
   {
      if( (unsigned)*pszIn >= _XML_MAX_WATCH )
      {
         uLength++;
         pszIn++;
         continue;
      }
      else
      {
         if( pszWatchFor[*pszIn] & _XML_ENTITY )
         {
            switch( *pszIn )
            {
            //case L' ' : *pstringXML += L"&nbsp;"; break;
            case L'<' : uLength += 4; break;
            case L'>' : uLength += 4; break;
            case L'&' : uLength += 5; break;
            //case L'\'' : *pstringXML += L"&apos;"; break;
            case L'"' : uLength += 6; break;
            }
         }
         else
         {
            uLength++;
         }
      }

      pszIn++;
   } // end while

   return uLength;
}

void TextToHTML( const wchar_t* pszIn, gd::CString* pstringXML )
{
   wchar_t chCharacter;                            
   wchar_t chConvert;

   pstringXML->Single();

   while( *pszIn != 0 )
   {
      if( ((unsigned)*pszIn > L' ') && ((unsigned)*pszIn <= _T('z')) && !(pszWatchFor[*pszIn] & _XML_ENTITY) )
      {
         pstringXML->FastAdd( *pszIn );
         pszIn++;
         continue;
      }
      else
      {
         if( ((unsigned)*pszIn < _XML_MAX_WATCH) && (pszWatchFor[*pszIn] & _XML_ENTITY) )
         {
            switch( *pszIn )
            {
            //case L' ' : *pstringXML += L"&nbsp;"); break;
            case L'<' : pstringXML->FastAdd( L"&lt;" ); break;
            case L'>' : pstringXML->FastAdd( L"&gt;" ); break;
            case L'&' : pstringXML->FastAdd( L"&amp;" ); break;
            //case L'\'' : *pstringXML += L"&apos;"); break;
            case L'"' : pstringXML->FastAdd( L"&quot;" ); break;
            default: _ASSERT(FALSE);
            }
         }
         else
         {
            pstringXML->FastAdd( L"&#x" );

            chConvert = (unsigned)*pszIn;
#ifdef _UNICODE
            if( chConvert > 0xff )
            {
               chCharacter = (wchar_t)(chConvert >> 12);                             
               chCharacter += (chCharacter > 9) ? L'7' : L'0';
               pstringXML->FastAdd( chCharacter );
               chCharacter = (wchar_t)(chConvert >> 8) & 0x000f;                             
               chCharacter += (chCharacter > 9) ? L'7' : L'0';
               pstringXML->FastAdd( chCharacter );
               chConvert &= 0x00ff;
            }
#endif
            chCharacter = (wchar_t)((unsigned char)chConvert >> 4);                             
            chCharacter += (chCharacter > 9) ? _T('7') : _T('0');
            pstringXML->FastAdd( chCharacter );
            chCharacter = (wchar_t)(chConvert & 0x0f);
            chCharacter += (chCharacter > 9) ? _T('7') : _T('0');
            pstringXML->FastAdd( chCharacter );
            pstringXML->FastAdd( _T(';') );
         }
      }

      pszIn++;
   } // end while
}

//*************************************************************************************************
/*!TYPE: GLOBAL<br>
PURPOSE: Replace characters in string that is reserved as xml characters*/
void ReplaceXML( wchar_t* pszXML, wchar_t chReplace )
{
   while( *pszXML != NULL )
   {
      if( (unsigned)*pszXML < _XML_MAX_WATCH )
      {
         if( pszWatchFor[*pszXML] & _XML_ENTITY )
         {
            *pszXML = chReplace;
         }
      }
      pszXML++;
   }
}

void TextToXMLEx( const wchar_t* pszIn, Texts* pTexts, gd::CString* pstringXML, UINT uFlags )
{
   UINT uCount, uCounter;
   TextText* pTextText;
   const wchar_t* pszText;
   bool bFound;

   uCount = 0;
   if( pTexts != NULL )
   {
      pTextText = pTexts->pTextText;
      uCount = pTexts->uCount;
   }

   pstringXML->Single();

   while( *pszIn != 0 )
   {
      bFound = false;
      
      if( uFlags & eSpace )
      {
         if( *pszIn == _T(' ') )
         {
            uCounter = 1;
            while( pszIn[uCounter] == _T(' ') ) uCounter++;
            if( uCounter > 1 )
            {
               pszIn += uCounter;
               pstringXML->Insert( -1, _T("&nbsp"), uCounter );
            }
         }
      }

      if( uFlags & eWWW )
      {
         bFound = false;

         if( (*pszIn <= 'w') && (pszWatchFor[*pszIn] & _XML_WWW) )
         {
            if( _wcsnicmp( pszIn, _T("www."), 4 ) == 0 )
            {
               if( pszIn[4] > ' ' ) bFound = true;
            }
            else if( _wcsnicmp( pszIn, _T("http://"), 7 ) == 0 )
            {
               if( pszIn[7] > ' ' ) bFound = true;
            }

            if( bFound == true )
            {
               if( (*pszIn == _T('w')) || (*pszIn == _T('W')) )
               {
                  pstringXML->Add( _T("<A HREF=\"http://") );
               }
               else
               {
                  pstringXML->Add( _T("<A HREF=\"") );
               }
               uCounter = 4;
               while( pszIn[uCounter] > ' ' ) uCounter++;
               pstringXML->Add( pszIn, uCounter );
               pstringXML->Add( _T("\">") );
               pstringXML->Add( pszIn, uCounter );
               pstringXML->Add( _T("</A>") );
               pszIn += uCounter;
               continue;
            }
         }
      }

      if( uCount && ((unsigned)*pszIn <= pTexts->uMaxCode) )
      {
         for( uCounter = 0; uCounter < uCount; uCounter++ )
         {
            if( pTextText[uCounter].pszFrom[0] == *pszIn )
            {
               pszText = pTextText[uCounter].pszFrom;
               if( wcsncmp( pszText, pszIn, wcslen( pszText ) ) == 0 )
               {
                  pszIn += wcslen( pszText );
                  *pstringXML += pTextText[uCounter].pszTo;
                  bFound = true;
                  continue;
               }
            }
         }
      }

      if( uFlags & eXML )
      {
         if( (unsigned)*pszIn >= _XML_MAX_WATCH )
         {
            pstringXML->FastAdd( *pszIn );
            pszIn++;
            continue;
         }
         else
         {
            if( pszWatchFor[*pszIn] & _XML_ENTITY )
            {
               bFound = true;
               switch( *pszIn )
               {
               //case _T(' ') : *pstringXML += _T("&nbsp;"); break;
               case _T('<') : pstringXML->FastAdd( _T("&lt;") ); break;
               case _T('>') : pstringXML->FastAdd( _T("&gt;") ); break;
               case _T('&') : pstringXML->FastAdd( _T("&amp;") ); break;
               case _T('"') : pstringXML->FastAdd( _T("&quot;") ); break;
               }
               pszIn++;
            }
         }
      }

      if( bFound == false )
      {
         pstringXML->FastAdd( *pszIn );
         pszIn++;
      }
   } // end while

}

//*************************************************************************************************
/*!TYPE: GLOBAL FUNCTION<br>
PURPOSE: Convert xml signs in text to normal signs and return the result string<br>
RETURN:  string that has normal characters<br>*/
gd::CString XMLToText( const wchar_t* pszIn /*!< string that should be converted */)
{
   struct Convert
   {
      wchar_t* pszLookFor;
      long   lLength;
      wchar_t  chReplace;
   };

   static Convert pConvert[] =
   {
      { _T("lt;"), 3, _T('<') },
      { _T("gt;"), 3, _T('>') },
      { _T("amp;"), 4, _T('&') },
      { _T("quot;"), 4, _T('\"') },
      { NULL, NULL }
   };

   Convert* pC;
   gd::CString string;

   while( *pszIn != 0 )
   {
      if( *pszIn != _T('&') )
      {
         string.FastAdd( *pszIn );
         pszIn++;
         continue;
      }
      else
      {
         for( pC = pConvert; pC->pszLookFor != NULL; pC++ )
         {
            if( wcsncmp( pC->pszLookFor, &pszIn[1], pC->lLength ) == 0 )
            {
               string.FastAdd( pC->chReplace );
               pszIn += pC->lLength;
               break;
            }
         }

         if( pC->pszLookFor == NULL )
         {
            string.FastAdd( *pszIn );
         }
      }

      pszIn++;
   } // end while

   return string;
}

void XMLToText( const wchar_t* pszIn, gd::CString* pstringXML )
{
   struct Convert
   {
      wchar_t* pszLookFor;
      long   lLength;
      wchar_t  chReplace;
   };

   static Convert pConvert[] =
   {
      { _T("lt;"), 3, _T('<') },
      { _T("gt;"), 3, _T('>') },
      { _T("amp;"), 4, _T('&') },
      { _T("quot;"), 4, _T('\"') },
      { NULL, NULL }
   };

   Convert* pC;

   while( *pszIn != 0 )
   {
      if( *pszIn != _T('&') )
      {
         *pstringXML += *pszIn;
         pszIn++;
         continue;
      }
      else
      {
         for( pC = pConvert; pC->pszLookFor != NULL; pC++ )
         {
            if( wcsncmp( pC->pszLookFor, &pszIn[1], pC->lLength ) == 0 )
            {
               *pstringXML += pC->chReplace;
               pszIn += pC->lLength;
               break;
            }
         }

         if( pC->pszLookFor == NULL )
         {
            *pstringXML += *pszIn;
         }
      }

      pszIn++;
   } // end while
}



#pragma warning( disable:  4244 )
void BinToHex( const BYTE* pbValue, wchar_t* pszHex, UINT uLength )
{                                                  
   BYTE    bTemp;                                  
   wchar_t   chCharacter;                            
   UINT    uCounter;

   for( uCounter = 0; uCounter < uLength; uCounter++ )
   {
      bTemp = pbValue[uCounter];
      chCharacter = (wchar_t)(bTemp >> 4);                             
      chCharacter += (chCharacter > 9) ? _T('7') : _T('0');
      *pszHex = chCharacter;
      pszHex++;

      bTemp = pbValue[uCounter];
      chCharacter = (wchar_t)(bTemp & 0x0f);
      chCharacter += (chCharacter > 9) ? _T('7') : _T('0');
      *pszHex = chCharacter;
      pszHex++;
   }
}

int HexToBin( const wchar_t* pszHex, BYTE* pbValue, UINT uLength, wchar_t chEnd )
{                                                  
   BYTE    bTemp;//, bTemp2;       
   wchar_t   chCharacter;                            
   UINT    uCounter;

   for( uCounter = 0; uCounter < uLength; uCounter++ )
   {
      chCharacter = *pszHex;
      if( chEnd == chCharacter ) break; 
      _CHECK( (chCharacter >= _T('0') && chCharacter <= _T('9')) || (chCharacter >= _T('A') && chCharacter <= _T('F')) );
      bTemp = ((BYTE)(chCharacter - ((chCharacter > _T('9')) ? _T('7') : _T('0')))) << 4;
      pszHex++;

      chCharacter = *pszHex;
      if( chEnd == chCharacter ) break; 
      _CHECK( (chCharacter >= _T('0') && chCharacter <= _T('9')) || (chCharacter >= _T('A') && chCharacter <= _T('F')) );
      bTemp += (BYTE)(chCharacter - ((chCharacter > _T('9')) ? _T('7') : _T('0')));
      pszHex++;
      *pbValue = bTemp;

      pbValue++;
   }

   return uCounter;
}

UINT HexToBinLength( const wchar_t* pszHex, wchar_t chEnd )
{                                                  
   UINT uCounter = 0;

   while( *pszHex && *pszHex != chEnd )
   {
      uCounter++;
      pszHex++;
      if( *pszHex == 0 || *pszHex == chEnd ) break;
      pszHex++;
   }

   return uCounter;
}

};
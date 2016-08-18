#include "StdAfx.h"
#include ".\rtlStrings.h"
#include <strsafe.h>

namespace rtl
{
	namespace strings
	{
		VA_RTL_API int rtl_strcmp( const char *stringCompare1, const char *stringCompare2)
		{
			ATLASSERT( stringCompare1);
			ATLASSERT( stringCompare2);
			return strcmp( stringCompare1, stringCompare2);
		}

		VA_RTL_API int rtl_stricmp( const char *stringCompare1, const char *stringCompare2)
		{
			ATLASSERT( stringCompare1);
			ATLASSERT( stringCompare2);
			return _stricmp( stringCompare1, stringCompare2);
		}

		VA_RTL_API int rtl_wcscmp( const wchar_t *stringCompare1, const wchar_t *stringCompare2)
		{
			ATLASSERT( stringCompare1);
			ATLASSERT( stringCompare2);
			return wcscmp( stringCompare1, stringCompare2);
		}

		VA_RTL_API int rtl_wcsicmp( const wchar_t *stringCompare1, const wchar_t *stringCompare2)
		{
			ATLASSERT( stringCompare1);
			ATLASSERT( stringCompare2);
			return _wcsicmp( stringCompare1, stringCompare2);
		}

		VA_RTL_API RtlError rtl_vswprintf( wchar_t * pDest, int bufferSize, int startIndex, const wchar_t * format, va_list arg )
		{
			int result = 0;
			if( bufferSize > startIndex)
			{

				long hr = StringCchVPrintfW( pDest + startIndex, bufferSize - startIndex, format, arg);
				switch ( hr)
				{
				case STRSAFE_E_INVALID_PARAMETER:
					result = 1;
					break;
				case STRSAFE_E_INSUFFICIENT_BUFFER:
					result = 2;
					break;
				}
			}
			else
				result = 3;

			if(result != 0)
			{
				// TODO write to log and create dump
				ATLASSERT( false);
				return RTLERROR_CONVERTERROR;
			}
			return RTLERROR_NOERROR;
		}

		VA_RTL_API RtlError rtl_vsprintf( char * pDest, int bufferSize, int startIndex, const char * format, va_list arg )
		{
			int result = 0;
			if( bufferSize > startIndex)
			{

				long hr = StringCchVPrintfA( pDest + startIndex, bufferSize - startIndex, format, arg);
				switch ( hr)
				{
				case STRSAFE_E_INVALID_PARAMETER:
					result = 1;
					break;
				case STRSAFE_E_INSUFFICIENT_BUFFER:
					result = 2;
					break;
				}
			}
			else
				result = 3;

			if(result != 0)
			{
				// TODO write to log and create dump
				ATLASSERT( false);
				return RTLERROR_CONVERTERROR;
			}
			return RTLERROR_NOERROR;
		}

		VA_RTL_API RtlError rtl_sprintf( char* pDest, int bufferSize, int startIndex, const char* format, ...)
		{
			va_list args;
			va_start(args, format);
			int result = rtl_vsprintf(pDest + startIndex, bufferSize, startIndex, format, args);
			va_end(args);
			return result;
		}

		VA_RTL_API RtlError rtl_sprintf( char* pDest, int bufferSize, const char* format, ...)
		{
			va_list args;
			va_start(args, format);
			int result = rtl_vsprintf(pDest + 0, bufferSize, 0, format, args);
			va_end(args);
			return result;
		}

		VA_RTL_API RtlError rtl_swprintf( wchar_t* pDest, int bufferSize, int startIndex, const wchar_t* format, ...)
		{
			va_list args;
			va_start(args, format);
			int result = rtl_vswprintf(pDest + startIndex, bufferSize, startIndex, format, args);
			va_end(args);
			return result;
		}

		VA_RTL_API RtlError rtl_swprintf( wchar_t* pDest, int bufferSize, const wchar_t* format, ...)
		{
			va_list args;
			va_start(args, format);
			int result = rtl_vswprintf(pDest + 0, bufferSize, 0, format, args);
			va_end(args);
			return result;
		}

		// Wrapper for strcat
		VA_RTL_API RtlError rtl_strcat( char *strDestination, size_t numberOfElements, const char *strSource)
		{
			ATLASSERT( strDestination);
			ATLASSERT( strSource);
			ATLASSERT( numberOfElements > 0 && numberOfElements<0xFFFF);

#if _MSC_VER >= 1500 //vs2008

			errno_t errorCode = strcat_s( strDestination, numberOfElements, strSource);
			if( errorCode == 0)
			{
				return RTLERROR_NOERROR;
			}
			ATLASSERT( FALSE);
			return RTLERROR_CONVERTERROR;

#else
			//ATLASSERT( numberOfElements > strlen(strSource));
			strcat(strDestination, strSource);
			return RTLERROR_NOERROR;

#endif
		}

		// Wrapper for wcscat
		VA_RTL_API RtlError rtl_wcscat( wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource)
		{
			ATLASSERT( strDestination);
			ATLASSERT( strSource);
			ATLASSERT( numberOfElements > 0 && numberOfElements<0xFFFF);

#if _MSC_VER >= 1500 //vs2008

			errno_t errorCode = wcscat_s( strDestination, numberOfElements, strSource);
			if( errorCode == 0)
			{
				return RTLERROR_NOERROR;
			}
			ATLASSERT( FALSE);
			return RTLERROR_CONVERTERROR;

#else
			//ATLASSERT( numberOfElements > strlen(strSource));
			wcscat(strDestination, strSource);
			return RTLERROR_NOERROR;

#endif
		}


		// Wrapper for strcpy
		VA_RTL_API RtlError rtl_strcpy( char *strDestination, size_t numberOfElements, const char *strSource)
		{
			ATLASSERT( strDestination);
			ATLASSERT( strSource);
			ATLASSERT( numberOfElements > 0 && numberOfElements<0xFFFF);

			#if _MSC_VER >= 1500 //vs2008

						errno_t errorCode = strcpy_s( strDestination, numberOfElements, strSource);
						if( errorCode == 0)
						{
							return RTLERROR_NOERROR;
						}
							ATLASSERT( FALSE);
						return RTLERROR_CONVERTERROR;

			#else
						//ATLASSERT( numberOfElements > strlen(strSource));
						strcpy(strDestination, strSource);
						return RTLERROR_NOERROR;

			#endif
		}

		// Wrapper for strcpy
		VA_RTL_API RtlError rtl_wcscpy( wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource)
		{
			ATLASSERT( strDestination);
			ATLASSERT( strSource);
			ATLASSERT( numberOfElements > 0 && numberOfElements<0xFFFF);

#if _MSC_VER >= 1500 //vs2008

			errno_t errorCode = wcscpy_s( strDestination, numberOfElements, strSource);
			if( errorCode == 0)
			{
				return RTLERROR_NOERROR;
			}
			ATLASSERT( FALSE);
			return RTLERROR_CONVERTERROR;

#else
			ATLASSERT( numberOfElements > wcslen(strSource));
			wcscpy(strDestination, strSource);
			return RTLERROR_NOERROR;

#endif
		}

		// Wrapper for strncpy
		VA_RTL_API RtlError rtl_strncpy( char *strDestination, size_t numberOfElements, const char *strSource, size_t count)
		{
			ATLASSERT( strDestination);
			ATLASSERT( strSource);
			ATLASSERT( count > 0 && count<0xFFFF);

#if _MSC_VER >= 1500 //vs2008

			errno_t errorCode = strncpy_s( strDestination, numberOfElements, strSource, count);
			if( errorCode == 0)
			{
				return RTLERROR_NOERROR;
			}
			ATLASSERT( FALSE);
			return RTLERROR_CONVERTERROR;

#else
			ATLASSERT( numberOfElements > count);
			strncpy(strDestination, strSource, count);
			return RTLERROR_NOERROR;

#endif
		}

		// Wrapper for wcsncpy
		VA_RTL_API RtlError rtl_wcsncpy( wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource, size_t count)
		{
			ATLASSERT( strDestination);
			ATLASSERT( strSource);
			ATLASSERT( numberOfElements > 0 && numberOfElements<0xFFFF);

			#if _MSC_VER >= 1500 //vs2008

				errno_t errorCode = wcsncpy_s( strDestination, numberOfElements, strSource, count);
				if( errorCode == 0)
				{
					return RTLERROR_NOERROR;
				}
				ATLASSERT( FALSE);
				return RTLERROR_CONVERTERROR;

			#else
				ATLASSERT( numberOfElements > count);
				wcsncpy(strDestination, strSource, count);
				return RTLERROR_NOERROR;

			#endif
		}

		VA_RTL_API RtlError rtl_mbstowcs( wchar_t *strDestination, size_t numberOfElements, const char *strSource, size_t count)
		{
			ATLASSERT( strDestination);
			ATLASSERT( strSource);
			ATLASSERT( numberOfElements > 0 && numberOfElements<0xFFFF);

			#if _MSC_VER >= 1500 //vs2008

					size_t convertedChars = 0;
					mbstowcs_s( &convertedChars, strDestination, numberOfElements, strSource, count);
					if( convertedChars != count)
					{
						ATLASSERT( FALSE);
						return RTLERROR_CONVERTERROR;
					}
					return RTLERROR_NOERROR;

			#else

					ATLASSERT( numberOfElements > strlen(strSource));
					mbstowcs(strDestination, strSource, count);
					return RTLERROR_NOERROR;

			#endif
		}

		VA_RTL_API RtlError rtl_wcstombs( char *strDestination, size_t numberOfElements, const wchar_t *strSource, size_t count)
		{
			ATLASSERT( strDestination);
			ATLASSERT( strSource);
			ATLASSERT( numberOfElements > 0 && numberOfElements<0xFFFF);

#if _MSC_VER >= 1500 //vs2008

			size_t convertedChars = 0;
			wcstombs_s( &convertedChars, strDestination, numberOfElements, strSource, count);
			if( convertedChars != count)
			{
				ATLASSERT( FALSE);
				return RTLERROR_CONVERTERROR;
			}
			return RTLERROR_NOERROR;

#else

			ATLASSERT( numberOfElements > wcslen(strSource));
			wcstombs(strDestination, strSource, count);
			return RTLERROR_NOERROR;

#endif
		}
	}
}
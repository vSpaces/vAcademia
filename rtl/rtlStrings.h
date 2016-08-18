#pragma once

namespace rtl
{
	namespace strings
	{
		VA_RTL_API int rtl_strcmp( const char *stringCompare1, const char *stringCompare2);
		VA_RTL_API int rtl_stricmp( const char *stringCompare1, const char *stringCompare2);
		VA_RTL_API int rtl_wcscmp( const wchar_t *stringCompare1, const wchar_t *stringCompare2);
		VA_RTL_API int rtl_wcsicmp( const wchar_t *stringCompare1, const wchar_t *stringCompare2);
		VA_RTL_API RtlError rtl_strcat( char *strDestination, size_t numberOfElements, const char *strSource);
		VA_RTL_API RtlError rtl_wcscat( wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource);
		VA_RTL_API RtlError rtl_strcpy( char *strDestination, size_t numberOfElements, const char *strSource);
		VA_RTL_API RtlError rtl_wcscpy( wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource);
		VA_RTL_API RtlError rtl_strncpy( char *strDestination, size_t numberOfElements, const char *strSource, size_t count);
		VA_RTL_API RtlError rtl_wcsncpy( wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource, size_t count);
		VA_RTL_API RtlError rtl_mbstowcs( wchar_t *strDestination, size_t numberOfElements, const char *strSource, size_t count);
		VA_RTL_API RtlError rtl_wcstombs( char *strDestination, size_t numberOfElements, const wchar_t *strSource, size_t count);
		VA_RTL_API RtlError rtl_lstrcpy( wchar_t *strDestination, size_t numberOfElements, const char *strSource, size_t count);
		VA_RTL_API RtlError rtl_vsprintf( char * pDest	// input buffer
										, int bufferSize
										, int startIndex
										, const char * format
										, va_list arg );
		VA_RTL_API RtlError rtl_vswprintf( wchar_t * pDest	// input buffer
										, int bufferSize
										, int startIndex
										, const wchar_t * format
										, va_list arg );
		VA_RTL_API RtlError rtl_sprintf( char* pDest
										, int bufferSize
										, int startIndex
										, const char* format
										, ...);
		VA_RTL_API RtlError rtl_sprintf( char* pDest
										, int bufferSize
										, const char* format
										, ...);
		VA_RTL_API RtlError rtl_swprintf( wchar_t* pDest
			, int bufferSize
			, int startIndex
			, const wchar_t* format
			, ...);

		VA_RTL_API RtlError rtl_swprintf( wchar_t* pDest
			, int bufferSize
			, const wchar_t* format
			, ...);

	}
}
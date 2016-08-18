#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wrapper for strcpy
#if _MSC_VER >= 1500 //vs2008
#define va_strcpy( outBuffer, inBuffer, bufLength)	\
	strcpy_s( outBuffer, bufLength, inBuffer)
#else
#define va_strcpy( outBuffer, inBuffer, bufLength)	\
	strcpy(outBuffer, inBuffer)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wrapper for wcsncpy
#if _MSC_VER >= 1500 //vs2008
#define va_wcsncpy( outResBuffer, inBuffer, bufLength)	\
	wcsncpy_s( outResBuffer, bufLength, inBuffer, bufLength)
#else
#define va_wcsncpy( outResBuffer, inBuffer, bufLength)	\
	wcsncpy(outResBuffer, inBuffer, bufLength)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wrapper for mbstowcs
#if _MSC_VER >= 1500 //vs2008
#define va_mbstowcs( outResBuffer, inBuffer, bufLength)	\
						size_t convertedChars = 0;	\
						mbstowcs_s( &convertedChars, outResBuffer, bufLength, inBuffer, bufLength)
#else
#define va_mbstowcs( outResBuffer, inBuffer, bufLength)	\
						mbstowcs(outResBuffer, inBuffer, bufLength)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wrapper for _itow
#if _MSC_VER >= 1500 //vs2008
#define va_itow( invalue, dest, radix, sizeInChars)	\
	_itow_s( invalue, dest, sizeInChars, radix)
#else
#define va_itow( invalue, dest, radix)	\
	_itow(invalue, dest, radix)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wrapper for _ui64tow
#if _MSC_VER >= 1500 //vs2008
#define va_ui64tow( invalue, dest, radix, sizeInChars)	\
	_ui64tow_s( invalue, dest, sizeInChars, radix)
#else
#define va_ui64tow( invalue, dest, radix)	\
	_ui64tow(invalue, dest, radix)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wrapper for _ultow
#if _MSC_VER >= 1500 //vs2008
#define va_ultow( invalue, dest, radix, sizeInChars)	\
	_ultow_s( invalue, dest, sizeInChars, radix)
#else
#define va_ultow( invalue, dest, radix)	\
	_ultow(invalue, dest, radix)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wrapper for sizeof(a)/sizeof(a[0])
#define va_arrsize_in_bytes(a)  sizeof(a)/sizeof(a[0])
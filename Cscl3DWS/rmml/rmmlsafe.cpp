#include "mlRMML.h"

#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers
#include <tchar.h>
#include <strsafe.h>
#include <iostream>

#include "rmmlsafe.h"

int rmmlsafe_vsprintf(char * pDest	// input buffer
					  , int bufferSize
					  , int startIndex
					  , const char * format
					  , va_list arg )
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
		assert( false);
	}
	return result;
}

int rmmlsafe_strcpy(char* pDest
					, int bufferSize
					, int startIndex
					, const char* pszSrc)
{
	int result = 0;

	if( bufferSize > startIndex)
	{
		long hr = StringCchCopyA( pDest + startIndex
									, bufferSize - startIndex
									, pszSrc);
		switch( hr)
		{
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
		assert( false);
	}
	return result;
}

int rmmlsafe_wcscpy(wchar_t* pDest
					, int bufferSize
					, int startIndex
					, const wchar_t* pszSrc)
{
	int result = 0;
	if( bufferSize > startIndex)
	{
		long hr = StringCchCopyW( pDest + startIndex
			, bufferSize - startIndex
			, pszSrc);
		switch( hr)
		{
		case STRSAFE_E_INSUFFICIENT_BUFFER:
			result = 2;
		}
	}
	else
		result = 3;

	if(result != 0)
	{
		// TODO write to log and create dump
		assert( false);
	}
	return result;
}

int rmmlsafe_sprintf(char* pDest
					 , int bufferSize
					 , int startIndex
					 , const char* format
					 , ...)
{
	va_list args;
	va_start(args, format);
	int result = rmmlsafe_vsprintf(pDest + startIndex, bufferSize, startIndex, format, args);
	va_end(args);
	return result;
}
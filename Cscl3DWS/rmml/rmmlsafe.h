#pragma once

/** 
\return int
	0 - OK
	1 - STRSAFE_E_INVALID_PARAMETER
	2 - STRSAFE_E_INSUFFICIENT_BUFFER
	3 - BUFFER SIZE IS SMALLER THAN START INDEX
*/
int rmmlsafe_vsprintf(char * pDest	// input buffer
					  , int bufferSize
					  , int startIndex
					  , const char * format
					  , va_list arg );

/** 
\return int
0 - OK
2 - STRSAFE_E_INSUFFICIENT_BUFFER
3 - BUFFER SIZE IS SMALLER THAN START INDEX
*/
int rmmlsafe_strcpy(char* pDest
					, int bufferSize
					, int startIndex
					, const char* pszSrc);

/** 
\return int
0 - OK
2 - STRSAFE_E_INSUFFICIENT_BUFFER
3 - BUFFER SIZE IS SMALLER THAN START INDEX
*/
int rmmlsafe_wcscpy(wchar_t* pDest
					, int bufferSize
					, int startIndex
					, const wchar_t* pszSrc);


int rmmlsafe_sprintf(char* pDest
					 , int bufferSize
					 , int startIndex
					 , const char* format
					 , ...);
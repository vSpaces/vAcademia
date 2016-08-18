#include "StdAfx.h"
#include <stdio.h>
#include "func.h"
#include <shlobj.h>
#include <io.h>

void getApplicationDataDirectory( wchar_t *lpFilePath)
{
	if ( lpFilePath != NULL)
		SHGetSpecialFolderPathW(NULL, lpFilePath, CSIDL_APPDATA, true);	
}

BOOL getFileNameByProjName(  const wchar_t *aFileName, const wchar_t *ext, wchar_t *lpFileName)
{
	WCHAR tmp[MAX_PATH_4];
	SHGetSpecialFolderPathW( NULL, tmp, CSIDL_APPDATA, true);

	if ( wcscmp( ext, L"/q") == 0)
		swprintf( lpFileName, L"%s\\%s", tmp, aFileName);
	else
		swprintf( lpFileName, L"%s\\%s.%s", tmp, aFileName, ext);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

// lstrrchr (avoid the C Runtime )
TCHAR * lstrrchr(LPCTSTR string, int ch)
{
	char *start = (char *)string;

	while (*string++)                       /* find end of string */
		;
	/* search towards front */
	while (--string != start && *string != (TCHAR) ch)
		;

	if (*string == (TCHAR) ch)                /* char found ? */
		return (TCHAR *)string;

	return NULL;
}

// lstrrchr (avoid the C Runtime )
WCHAR * lstrrchrW(LPCWSTR string, wchar_t ch)
{
	wchar_t *start = (wchar_t *)string;

	while ( (*string) != '\0')                       /* find end of string */
	{
		string++;
	}
	/* search towards front */
	while (--string != start && *string != ch)
		;

	if (*string == ch)                /* char found ? */
		return (WCHAR *)string;

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// FormatTime
//
// Format the specified FILETIME to output in a human readable format,
// without using the C run time.
void FormatTime(LPTSTR output, FILETIME TimeToPrint)
{
	output[0] = _T('\0');
	WORD Date, Time;
	if (FileTimeToLocalFileTime(&TimeToPrint, &TimeToPrint) &&
		FileTimeToDosDateTime(&TimeToPrint, &Date, &Time))
	{
		wsprintf(output, _T("%d/%d/%d %02d:%02d:%02d"),
			(Date / 32) & 15, Date & 31, (Date / 512) + 1980,
			(Time >> 11), (Time >> 5) & 0x3F, (Time & 0x1F) * 2);
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetFilePart
TCHAR * GetFilePart(LPCTSTR source)
{
	TCHAR *result = lstrrchr(source, _T('\\'));
	if (result)
		result++;
	else
		result = (TCHAR *)source;
	return result;
}

WCHAR * GetFilePart(LPCWSTR source)
{
	WCHAR *result = lstrrchrW(source, L'\\');
	if (result)
		result++;
	else
		result = (WCHAR *)source;
	return result;
}

//////////////////////////////////////////////////////////////////////////

FILE *fp__ = NULL;

BOOL OpenLog__( WCHAR *sFileName, BOOL bCreateBakup, BOOL bAppend)
{
	BOOL BAlloc = FALSE;
	if ( sFileName == NULL)
	{		
		sFileName = new wchar_t[_MAX_PATH];
		BAlloc = TRUE;
		if ( !getFileNameByProjName( L"CrashReport", L"txt", sFileName))
		{
			delete[] sFileName;
			return FALSE;
		}
	}

	if ( bCreateBakup)
	{
		FILE *fp_r = _wfopen( sFileName, L"r");
		if ( fp_r != NULL)
		{
			int size = _filelength( fp_r->_file);
			char *bytes = new char[ size + 1];
			memset( bytes, 32, size + 1);
			int readed = fread( bytes, 1, size, fp_r);
			fclose( fp_r);
			WCHAR sFileNameInf[MAX_PATH_4];
			rtl_wcscpy( sFileNameInf, MAX_PATH_4, sFileName);
			rtl_wcscat( sFileNameInf, MAX_PATH_4 - wcslen(sFileNameInf), L".inf");
			bytes[ readed < size ? readed : size] = '\0';
			fp_r = _wfopen( sFileNameInf, L"w");
			if ( fp_r != NULL)
			{
				fwrite( bytes, 1, readed < size ? readed : size, fp_r);
				fclose( fp_r);
				fp_r = NULL;
			}
			delete[] bytes;
		}
	}

	if ( fp__ == NULL)
	{
		if ( bAppend)
		{
			fp__ = _wfopen( sFileName, L"a");
		}
		else
		{
			fp__ = _wfopen( sFileName, L"w");
		}
	}
	if ( BAlloc)
		delete[] sFileName;
	return fp__ != NULL;
}

BOOL WriteLog__( TCHAR *sInfo)
{
	if ( fp__ != NULL)
	{
		fwrite( sInfo, 1, strlen( sInfo), fp__) > 0;
		fflush( fp__);
		return TRUE;
	}
	return TRUE;
}

BOOL WriteLog__( WCHAR *sInfo)
{
	if ( fp__ != NULL)
	{
		USES_CONVERSION;
		fwrite( W2A(sInfo), 1, wcslen( sInfo), fp__) > 0;
		fflush( fp__);
		return TRUE;
	}
	return TRUE;
}

BOOL WriteOnceLog__( FILE *fp__, TCHAR *sKey, TCHAR *sInfo)
{
	BOOL bOpened = FALSE;
	if ( fp__ == NULL)
		bOpened = OpenLog__( NULL, FALSE, TRUE);	
	WriteLog__( sKey);	
	WriteLog__( sInfo);	
	WriteLog__( "\n");
	if ( bOpened)
		CloseLog__();	
	return TRUE;
}

void CloseLog__()
{
	if ( fp__ != NULL)
	{
		fclose( fp__);
		fp__ = NULL;
	}
}

BOOL RestoreLog()
{
	CloseLog__();
	WCHAR *sFileName = new wchar_t[_MAX_PATH];		
	if ( !getFileNameByProjName( L"CrashReport", L"txt", sFileName))
	{
		delete[] sFileName;
		return FALSE;
	}

	FILE *fp_r = _wfopen( sFileName, L"r");
	if ( fp_r != NULL)
	{
		int size = _filelength( fp_r->_file);
		char *bytes = new char[ size + 1];
		memset( bytes, 32, size + 1);
		int readed = fread( bytes, 1, size, fp_r);
		fclose( fp_r);
		WCHAR sFileNameInf[MAX_PATH_4];
		rtl_wcscpy( sFileNameInf, MAX_PATH_4, sFileName);
		rtl_wcscat( sFileNameInf, MAX_PATH_4 - wcslen(sFileNameInf), L".inf.check");
		bytes[ readed < size ? readed : size] = '\0';
		fp_r = _wfopen( sFileNameInf, L"w");
		if ( fp_r != NULL)
		{
			fwrite( bytes, 1, readed < size ? readed : size, fp_r);
			fclose( fp_r);
			fp_r = NULL;
		}		
		delete[] bytes;
	}
	
	WCHAR sFileNameInf[MAX_PATH_4];
	rtl_wcscpy( sFileNameInf, MAX_PATH_4, sFileName);
	rtl_wcscat( sFileNameInf, MAX_PATH_4 - wcslen(sFileNameInf), L".inf");

	fp_r = _wfopen( sFileNameInf, L"r");
	if ( fp_r != NULL)
	{
		int size = _filelength( fp_r->_file);
		char *bytes = new char[ size + 1];
		memset( bytes, 32, size + 1);
		int readed = fread( bytes, 1, size, fp_r);
		fclose( fp_r);

		bytes[ readed < size ? readed : size] = '\0';

		fp_r = _wfopen( sFileName, L"w");
		if ( fp_r != NULL)
		{
			fwrite( bytes, 1, readed < size ? readed : size, fp_r);
			fclose( fp_r);
			fp_r = NULL;
		}
		delete[] bytes;
	}
	return TRUE;
}
#include "func.h"
#include <shlobj.h>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <atlconv.h>

BOOL getUUID( char **psUUID)
{
	UUID uuid;
	if ( UuidCreate( &uuid) != RPC_S_OK)
	{					
		return FALSE;
	}

	unsigned char *sUUID;
	if ( UuidToString( &uuid, &sUUID) != RPC_S_OK)
	{					
		return FALSE;
	}

	strcpy( *psUUID, (const char*) sUUID);
	if ( RpcStringFree( &sUUID) != RPC_S_OK)
	{

	}
	return TRUE;
}

FILE *fpGlobal__ = NULL;

BOOL GetValueStrFromIni( WCHAR *fileName, TCHAR *lpSection, TCHAR *lpKey, TCHAR **sValue, FILE *fp, int count /*= MAX_PATH*/)
{
	wchar_t wValue[MAX_PATH];
	USES_CONVERSION;
	if ( GetPrivateProfileStringW( A2W( lpSection), A2W( lpKey), L"", wValue, count, fileName))
	{
		if ( wcslen( wValue) < 1 || wcsicmp( wValue, L"") == 0)
		{
			TCHAR *s = new TCHAR[ MAX_PATH_4];
			sprintf( s, "ERROR (GetValueStrFromIni): sValue of section '%s' key '%s' of file '%s' is equal ""\n", lpSection, lpKey, W2A( fileName));
			WriteLog__( fp, s);
			delete[] s;
			return FALSE;
		}
		strcpy( *sValue, W2A(wValue));
		TCHAR *s = new TCHAR[ MAX_PATH_4];
		sprintf( s, "key '%s' = '%s'\n", lpKey, *sValue);
		WriteLog__( fp, s);
		delete[] s;		
		return TRUE;
	}
	int errorCode = GetLastError();
	TCHAR *s = new TCHAR[ MAX_PATH_4];
	sprintf( s, "ERROR (GetValueStrFromIni): section '%s' key '%s' of file '%s' is failed, errorCode='%d'\n", lpSection, lpKey, W2A( fileName), errorCode);
	WriteLog__( fp, s);
	delete[] s;
	return FALSE;
}

BOOL GetValueStrFromIni( WCHAR *fileName, TCHAR *lpSection, TCHAR *lpKey, WCHAR **sValue, FILE *fp, int count /*= MAX_PATH*/)
{
	USES_CONVERSION;
	if ( GetPrivateProfileStringW( A2W( lpSection), A2W( lpKey), L"", *sValue, count, fileName))
	{
		if ( wcslen( *sValue) < 1 || wcsicmp( *sValue, L"") == 0)
		{
			TCHAR *s = new TCHAR[ MAX_PATH_4];
			sprintf( s, "ERROR (GetValueStrFromIni): sValue of section '%s' key '%s' of file '%s' is equal ""\n", lpSection, lpKey, W2A( fileName));
			WriteLog__( fp, s);
			delete[] s;
			return FALSE;
		}		
		TCHAR *s = new TCHAR[ MAX_PATH_4];
		sprintf( s, "key '%s' = '%s'\n", lpKey, W2A(*sValue));
		WriteLog__( fp, s);
		delete[] s;		
		return TRUE;
	}
	int errorCode = GetLastError();
	TCHAR *s = new TCHAR[ MAX_PATH_4];
	sprintf( s, "ERROR (GetValueStrFromIni): section '%s' key '%s' of file '%s' is failed, errorCode='%d'\n", lpSection, lpKey, W2A( fileName), errorCode);
	WriteLog__( fp, s);
	delete[] s;
	return FALSE;
}

void getApplicationDataDirectory( wchar_t *lpFilePath)
{
	if ( lpFilePath != NULL)
		SHGetSpecialFolderPathW(NULL, lpFilePath, CSIDL_APPDATA, true);	
}

BOOL getFileNameByProjName( const char *aFileName, const wchar_t *ext, wchar_t *lpFileName)
{	
	/*HMODULE hModule = GetModuleHandle( sProjectName);

	if ( GetModuleFileName( hModule, lpFileName, MAX_PATH) == 0)
	{
		int codeerror = GetLastError();
		return FALSE;
	}
	LPTSTR lpName = NULL;
	for (lpName = lpFileName + lstrlen( lpFileName); lpName > lpFileName; lpName--)
	{
		if(*lpName == _T('\\'))
		{
			*lpName = '\0';
			break;
		}
	}*/
	WCHAR tmp[MAX_PATH_4];
	SHGetSpecialFolderPathW( NULL, tmp, CSIDL_APPDATA, true);
	USES_CONVERSION;
	swprintf( lpFileName, L"%s\\%s.%s", tmp, A2W(aFileName), ext);
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

// lstrrchrW (avoid the C Runtime )
WCHAR * lstrrchrW(LPCWSTR string, WCHAR ch)
{
	wchar_t *start = (wchar_t *)string;

	while ((*string) != '\0')                       /* find end of string */
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

int lstrpos(LPCTSTR string, LPCTSTR strfind)
{
	char *pdest;
	pdest = (char *) _tcsstr( string, strfind);
	if ( pdest == NULL)
		return -1;
	return pdest - string;
}

int lstrposW(LPCWSTR string, LPCWSTR strfind)
{
	wchar_t *pdest;
	pdest = (wchar_t *) wcsstr( string, strfind);
	if ( pdest == NULL)
		return -1;
	return pdest - string;
}

TCHAR * substr(LPCTSTR string, int pos, int len)
{
	int ipos = 0;
	while ( *string && ipos < pos)                       /* find ipos == pos*/
	{
		*string++;
		ipos++;
	}

	ipos = 0;
	char *start = (char *)string;
	while ( *start && ipos < len)                       /* find ipos == pos*/
	{
		*start++;
		ipos++;
	}
	*start = '\0';
		
	return (TCHAR *) string;
}

WCHAR * substr(LPCWSTR string, int pos, int len)
{
	int ipos = 0;
	while ( *string && ipos < pos)                       /* find ipos == pos*/
	{
		*string++;
		ipos++;
	}

	ipos = 0;
	wchar_t *start = (wchar_t *)string;
	while ( *start && ipos < len)                       /* find ipos == pos*/
	{
		*start++;
		ipos++;
	}
	*start = L'\0';

	return (WCHAR *) string;
}

//////////////////////////////////////////////////////////////////////////

void DeleteLog__( TCHAR *sFileName, TCHAR *m_szAppName)
{	
	if ( sFileName == NULL)
	{
		WCHAR _sFileName[_MAX_PATH];
		if ( !getFileNameByProjName( m_szAppName, L"txt", _sFileName))
			return;		
		//DeleteFileW( _sFileName);
		return;
	}
	//DeleteFile( sFileName);	
}

BOOL OpenLog__( FILE **fp__, WCHAR *sFileName, TCHAR *m_szAppName)
{		
	if ( sFileName == NULL)
	{
		WCHAR _sFileName[_MAX_PATH];		
		if ( !getFileNameByProjName( m_szAppName, L"txt", _sFileName))		
			return FALSE;
		*fp__ = _wfopen( _sFileName, L"a");	
		return *fp__ != NULL;
	}
	*fp__ = _wfopen( sFileName, L"a");	
	fpGlobal__ = *fp__;
	return *fp__ != NULL;
}

BOOL WriteLog__( FILE *fp__, TCHAR *sInfo)
{
	if ( fp__ != NULL)
	{
		fwrite( sInfo, 1, strlen( sInfo), fp__) > 0;
		try
		{
			fflush( fp__);
		}		
		catch (...)
		{
		}
		return TRUE;
	}
	return FALSE;
}

BOOL WriteLog__( FILE *fp__, WCHAR *sInfo)
{
	if ( fp__ != NULL)
	{
		USES_CONVERSION;
		fwrite( W2A(sInfo), 1, wcslen( sInfo), fp__) > 0;
		try
		{
			fflush( fp__);
		}		
		catch (...)
		{
		}
		return TRUE;
	}
	return FALSE;
}

BOOL WriteLog__( TCHAR *sInfo)
{
	if ( fpGlobal__ != NULL)
	{
		fwrite( sInfo, 1, strlen( sInfo), fpGlobal__) > 0;
		try
		{
			fflush( fpGlobal__);
		}		
		catch (...)
		{
		}
		return TRUE;
	}
	return FALSE;
}

BOOL WriteLog__( WCHAR *sInfo)
{
	if ( fpGlobal__ != NULL)
	{
		USES_CONVERSION;
		fwrite( W2A(sInfo), 1, wcslen( sInfo), fpGlobal__) > 0;
		try
		{
			fflush( fpGlobal__);
		}		
		catch (...)
		{
		}
		return TRUE;
	}
	return FALSE;
}

BOOL WriteOnceLog__( FILE *fp__, TCHAR *sInfo)
{
	BOOL bOpened = FALSE;
	if ( fp__ == NULL)
		bOpened = OpenLog__( &fp__, NULL, "mdump");	
	BOOL bSucc = WriteLog__( fp__, sInfo);
	if ( bSucc)
		bSucc = WriteLog__( fp__, "\n");
	if ( bOpened)
		CloseLog__( &fp__);	
	return bSucc;
}

void CloseLog__( FILE **fp__)
{
	fpGlobal__ = NULL;
	if ( *fp__ != NULL)
	{
		fclose( *fp__);
		*fp__ = NULL;
	}
}

#include "XZip.h"
#include "mdump.h"
#include <io.h>

int GetDumpIndex( TCHAR *szProjectName, WCHAR *sDir)
{
	USES_CONVERSION;
	WIN32_FIND_DATAW fdData;
	HANDLE hFind = FindFirstFileW( sDir, &fdData);
	int dumpIndexMax = 0;
	if (hFind != INVALID_HANDLE_VALUE)
	{
		WCHAR sFileName[ MAX_PATH];
		WCHAR sFileNameSrc[ MAX_PATH];
		wcscpy( sFileNameSrc, A2W( szProjectName));
		int dumpIndex = 0;
		do 
		{
			if ((fdData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{				
				wcscpy( sFileName, fdData.cFileName);
				int pos = lstrposW( sFileName, sFileNameSrc);
				int pos2 = lstrposW( sFileName, L".");
				int pos3 = lstrposW( sFileName, L".zip");
				if ( pos > -1 && pos < pos2 - wcslen( sFileNameSrc)
					&& pos3 == wcslen( sFileName) - 4)
				{
					pos += wcslen( sFileNameSrc);
					WCHAR *sDumpIndex = substr( sFileName, pos + 1, pos2 - pos - 1);
					dumpIndex = _wtoi( sDumpIndex);
					if ( dumpIndex > dumpIndexMax)
						dumpIndexMax = dumpIndex;
				}
			}
		} while ( FindNextFileW( hFind, &fdData));
	}
	return dumpIndexMax + 1;
}

struct TFileName
{
	WCHAR s[ MAX_PATH_4];
};

// ZipFiles
int ZipFiles( int iCrashType, TCHAR *szProjectName, WCHAR *szDumpPath, FILE *fp__, TCHAR *sMumpLog)
{	
	WriteLog__( fp__, "ZipFiles start\n");	
	TFileName fileNameForDelete[ 2];

	int nFiles = 0;
	WCHAR lpFileName[ MAX_PATH_4];	

	WCHAR *lastperiod = lstrrchrW( szDumpPath, L'\\');
	if (lastperiod)
		lastperiod[0] = 0;	

	wcscpy( lpFileName, szDumpPath);

	WriteLog__( fp__, "zipFileName = ");
	WriteLog__( fp__, lpFileName);
	WriteLog__( fp__, "\n");

	WCHAR dir[ MAX_PATH_4];
	wcscpy( dir, szDumpPath);
	wcscat( dir, L"\\*.*");
	int dumpIndex = GetDumpIndex( szProjectName, dir);

	TCHAR szDumpName[ MAX_PATH];
	strcpy( szDumpName, szProjectName);
	
	wcscat( lpFileName, L"\\");
	USES_CONVERSION;
	wcscat( lpFileName, A2W( szProjectName));
	wcscat( lpFileName, L"_");
	WCHAR sDumpIndex[ MAX_PATH];
	_itow( dumpIndex, sDumpIndex, 10);
	wcscat( lpFileName, sDumpIndex);	
	wcscat( lpFileName, L".zip");

	HZIP hZip = NULL;
	hZip = CreateZip((LPVOID) lpFileName, 0, ZIP_FILENAME);
	int errorCode = GetLastError();

	WriteLog__( fp__, "Create zip '");
	WriteLog__( fp__, lpFileName);
	WriteLog__( fp__, "'\n");

	TCHAR szLog[_MAX_PATH];

	if ( hZip == NULL)
	{
		sprintf( szLog, "ERROR: create zip (error %d)\n", errorCode);
		WriteLog__( fp__, szLog);
		return FALSE;
	}	
		
	if ( iCrashType == APP_HANG_ID)
	{
		_tcscat( szDumpName, "_hang");
	}
	else if ( iCrashType == ASSERT_ERROR_ID)
	{
		_tcscat( szDumpName, "_assert");
	}
	_tcscat( szDumpName, ".dmp");
	
	WCHAR *pszFilePart = GetFilePart( lpFileName);
	lstrcpyW(pszFilePart, A2W(szDumpName));

	WriteLog__( fp__, "szDumpName = ");
	WriteLog__( fp__, szDumpName);
	WriteLog__( fp__, "\n");

	WriteLog__( fp__, "lpFileName = ");
	WriteLog__( fp__, lpFileName);
	WriteLog__( fp__, "\n");

	WriteLog__( fp__, "Add zip");
	WriteLog__( fp__, "\n");
	ZRESULT zr = ZipAdd(hZip, szDumpName, lpFileName, 0, ZIP_FILENAME);
	if (zr == ZR_OK)
	{
		wcscpy( fileNameForDelete[ 0].s, lpFileName);
		nFiles++;
	}
	else
	{	
		errorCode = GetLastError();
		sprintf( szLog, "ERROR: dmp-file is can't add to zip! (error %d)\n", errorCode);
		WriteLog__( fp__, szLog);
		if (hZip)
			CloseZip(hZip);
		return FALSE;
	}
	
	lastperiod = lstrrchrW( lpFileName, L'\\');
	if (lastperiod)
		lastperiod[0] = 0;

	TCHAR sInfo[ MAX_PATH];
	_tcscpy( sInfo, "INFO");
	wcscat( lpFileName, L"\\");
	if ( iCrashType == APP_HANG_ID)
	{
		_tcscat( sInfo, "_HANG");
	}
	else if ( iCrashType == ASSERT_ERROR_ID)
	{
		_tcscat( sInfo, "_ASSERT");
	}

	_tcscat( sInfo, ".TXT");
	wcscat( lpFileName, A2W(sInfo));

	zr = ZipAdd(hZip, sInfo, lpFileName, 0, ZIP_FILENAME);
	if (zr == ZR_OK)
	{
		wcscpy( fileNameForDelete[ 1].s, lpFileName);
		nFiles++;
	}
	else
	{		
		errorCode = GetLastError();
		sprintf( szLog, "ERROR: info.txt is can't add to zip! (error %d)\n", errorCode);
		WriteLog__( fp__, szLog);		
		if (hZip)
			CloseZip(hZip);
		return FALSE;
	}
	
	DeleteFileW( fileNameForDelete[ 0].s);
	DeleteFileW( fileNameForDelete[ 1].s);

	lastperiod = lstrrchrW( lpFileName, L'\\');
	if (lastperiod)
		lastperiod[0] = 0;

	wcscpy( dir, lpFileName);
	wcscat( dir, L"\\Vacademia\\Log\\*.*");
	WIN32_FIND_DATAW fdData;
	HANDLE hFind = FindFirstFileW( dir, &fdData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		TCHAR sFileName[ MAX_PATH];
		FILETIME lastTime;
		lastTime.dwHighDateTime = 0;
		lastTime.dwLowDateTime = 0;
		do 
		{
			if ((fdData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				FILETIME time_ = fdData.ftLastWriteTime;
				if ( lastTime.dwHighDateTime < time_.dwHighDateTime)
				{
					lastTime = time_;
					_tcscpy( sFileName, W2A(fdData.cFileName));
				}
				if ( lastTime.dwHighDateTime == time_.dwHighDateTime && lastTime.dwLowDateTime < time_.dwLowDateTime)
				{
					lastTime = time_;
					_tcscpy( sFileName, W2A(fdData.cFileName));
				}
			}
		} while ( FindNextFileW( hFind, &fdData));

		if ( _tcslen( sFileName) > 0)
		{
			pszFilePart = GetFilePart( lpFileName);
			lstrcatW( pszFilePart, L"\\Vacademia\\Log\\");
			lstrcatW( pszFilePart, A2W(sFileName));

			WriteLog__( fp__, "Open log '");
			WriteLog__( fp__, lpFileName);
			WriteLog__( fp__, "'\n");

			FILE *fp = _wfopen( lpFileName, L"r");
			if ( fp == NULL)
			{				
				errorCode = GetLastError();
				sprintf( szLog, "ERROR: log file is can't open. Zipping without vacademia logs (error %d)\n", errorCode);
				WriteLog__( fp__, szLog);
				if (hZip)
					CloseZip(hZip);
				return nFiles;
			}
			WriteLog__( fp__, "Read log...\n");
			int size = _filelength( fp->_file);
			char *sbuffer = new char[ size];
			int readed = fread( sbuffer, 1, size, fp);
			if ( readed != size)
			{
				errorCode = GetLastError();
				sprintf( szLog, "ERROR: log file is can't read. Zipping without vacademia logs (error %d)\n", errorCode);
				WriteLog__( fp__, szLog);
				delete[] sbuffer;
				fclose( fp);
				//delete[] lpFileName;
				if (hZip)
					CloseZip(hZip);
				return nFiles;
			}

			WriteLog__( fp__, "Zip log...\n");
			ZRESULT zr = ZipAdd(hZip, sFileName, sbuffer, readed, ZIP_MEMORY);
			if (zr == ZR_OK)
			{
				nFiles++;
			}
			else
			{
				errorCode = GetLastError();
				sprintf( szLog, "ERROR: vacademia logs is can't add to zip! (error %d)\n", errorCode);
				WriteLog__( fp__, szLog);
			}
			delete[] sbuffer;
			fclose( fp);
		}
	}
	else
	{
		errorCode = GetLastError();
		sprintf( szLog, "ERROR: hFind == INVALID_HANDLE_VALUE (error %d)\n", errorCode);
		WriteLog__( fp__, szLog);
	}
	
	lastperiod = lstrrchrW( lpFileName, L'\\');
	if (lastperiod)
		lastperiod[0] = '\0';
	lastperiod = lstrrchrW( lpFileName, L'\\');
	if (lastperiod)
		lastperiod[0] = '\0';

	wcscpy( dir, lpFileName);
	wcscat( dir, L"\\dump_params.ini");	
	WCHAR *scriptLogFileName = new WCHAR[ MAX_PATH_4];
	WriteLog__( fp__, "Getting name of script log\n");

	if ( !GetValueStrFromIni( dir, "system", "scriptLog", &scriptLogFileName, fp__, MAX_PATH_4))
	{
		WriteLog__( fp__, "ERROR: key 'scriptLog' is can't get\n");
		delete[] scriptLogFileName;
		if (hZip)
			CloseZip(hZip);
		return nFiles;
	}

	WriteLog__( fp__, "Open script log '");
	WriteLog__( fp__, W2A(scriptLogFileName));
	WriteLog__( fp__, "'\n");

	FILE *fp = _wfopen( scriptLogFileName, L"r");
	if ( fp == NULL)
	{	
		errorCode = GetLastError();
		sprintf( szLog, "ERROR: script log is can't open (error %d)\n", errorCode);
		WriteLog__( fp__, szLog);
		if (hZip)
			CloseZip(hZip);
		return nFiles;
	}
	WriteLog__( fp__, "Read script log...\n");
	int size = _filelength( fp->_file);
	char *sbuffer = new char[ size];
	int readed = fread( sbuffer, 1, size, fp);
	if ( readed != size)
	{
		errorCode = GetLastError();
		sprintf( szLog, "ERROR: script log is can't read (error %d)\n", errorCode);
		WriteLog__( fp__, szLog);
		delete[] sbuffer;
		fclose( fp);
		if (hZip)
			CloseZip(hZip);
		return FALSE;
	}

	WriteLog__( fp__, "Zip script log...\n");	
	WCHAR *s1 = GetFilePart( scriptLogFileName);
	zr = ZipAdd(hZip, W2A(s1), sbuffer, readed, ZIP_MEMORY);
	if (zr == ZR_OK)
	{
		nFiles++;
	}
	else
	{
		errorCode = GetLastError();
		sprintf( szLog, "ERROR: script logs is can't add to zip! (error %d)\n", errorCode);
		WriteLog__( fp__, szLog);
	}
	delete[] sbuffer;
	fclose( fp);

	lastperiod = lstrrchrW( lpFileName, L'\\');
	if (lastperiod)
		lastperiod[0] = '\0';
	
	wcscat( lpFileName, L"\\");
	wcscat( lpFileName, L"mdump.txt");

	WriteLog__( fp__, "Zip mdump.txt: ");
	WriteLog__( fp__, W2A(lpFileName));
	WriteLog__( fp__, "\n");
	CloseLog__( &fp__);
	delete[] scriptLogFileName;

	zr = ZipAdd(hZip, "mdump.txt", lpFileName, 0, ZIP_FILENAME);
	if (zr == ZR_OK)
	{
		nFiles++;
	}
	else
	{
		OpenLog__( &fp__, NULL, sMumpLog);
		errorCode = GetLastError();
		sprintf( szLog, "ERROR: mdump.txt is can't add to zip! (error %d)\n", errorCode);
		WriteLog__( fp__, szLog);		
		if (hZip)
			CloseZip(hZip);
		WriteLog__( fp__, "ZipFiles end\n");
		return nFiles;
	}

	if (hZip)
		CloseZip(hZip);

	OpenLog__( &fp__, NULL, sMumpLog);
	WriteLog__( fp__, "ZipFiles end\n");

	return nFiles;
}
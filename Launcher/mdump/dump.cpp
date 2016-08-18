// dump.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <shlobj.h>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>

#include "mdump.h"
#include "Winsock2.h"
#include "psapi.h"
#include "io.h"

#include "MiniVersion.h"
#include "GetWinVer.h"
#include "func.h"
#include "dump.h"
#include <DbgHelp.h>


//#define XCRASHREPORT_ERROR_LOG_FILE				_T("INFO.TXT")
//#define XCRASHREPORT_ERROR_HANG_LOG_FILE		_T("INFO_HANG.TXT")
//#define XCRASHREPORT_ERROR_ASSERT_LOG_FILE		_T("INFO_ASSERT.TXT")

int g_iLanguage = ENG;
TCHAR login[] = "unknown"; 
int sessionID = 0;

///////////////////////////////////////////////////////////////////////////////
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

void getApplicationDataDirectory( wchar_t *lpFilePath)
{
	if ( lpFilePath != NULL)
		SHGetSpecialFolderPathW(NULL, lpFilePath, CSIDL_APPDATA, true);	
}

bool getAppLogin()
{	
	WCHAR szModuleName[MAX_PATH*2] = {L'\0'};
	//ZeroMemory(szModuleName, sizeof(szModuleName));
	if (GetModuleFileNameW(0, szModuleName, _countof(szModuleName)-2) <= 0)
		lstrcpyW(szModuleName, L"Unknown");

	WCHAR *fileName = new WCHAR[ MAX_PATH];		

	wcscpy( fileName, szModuleName);
	WCHAR *pszFilePart = GetFilePart( fileName);
	lstrcpyW(pszFilePart, CONFIG_FILE);	

	FILE *fp = _wfopen( fileName, L"r");

	if ( fp != NULL)
	{
		WCHAR *loginName = new WCHAR[ MAX_PATH];
		WCHAR *password = new WCHAR[ MAX_PATH];
		char *str = new char[ BUFFER_SIZE];
		int size = _filelength( fp->_file);
		if ( fread( str, 1, size, fp) <= 0)
		{
			TCHAR *s = new TCHAR[ MAX_PATH];
			sprintf( s, "Size of config file '%s' is equal 0\n", fileName);
			//WriteLog__( s);
			delete[] s;
			delete[] fileName;
			delete[] loginName;
			return false;
		}
		fclose( fp);
		
		GetPrivateProfileStringW( L"user", L"login", L"unknown", loginName, 256, fileName);
		GetPrivateProfileStringW( L"user", L"password", L"", password, 256, fileName);

		USES_CONVERSION;
		strcpy( login, W2A(loginName));
		SetUserMDump( login,  W2A(password), 1);
		delete[] password;
		delete[] loginName;
		return true;
	}	
	delete[] fileName;
	strcpy( login, "unknown");
	return true;
}

void setSessionID( int aSessionID)
{
	sessionID = aSessionID;
	SetSessionID( sessionID);
}

//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// GetMiniDumpString
// 
// This function reads a string from the minidump
// 

std::wstring GetMiniDumpStr( PVOID pMiniDump, RVA Rva )
{
	std::wstring rv( L"" ); 

	if( pMiniDump == 0 ) 
	{
		_ASSERTE( !_T("Minidump pointer is null.") ); 
		return rv; 
	}

	if( Rva == 0 ) 
	{
		_ASSERTE( !_T("RVA is null.") ); 
		return rv; 
	}

	MINIDUMP_STRING* pStrBuf = (MINIDUMP_STRING*)((BYTE*)pMiniDump + Rva); 

	if( IsBadReadPtr( pStrBuf, sizeof(MINIDUMP_STRING) ) ) 
	{
		_ASSERTE( !_T("Invalid structure pointer.") ); 
		return rv; 
	}

	if( IsBadStringPtrW( pStrBuf->Buffer, pStrBuf->Length / sizeof(WCHAR) ) ) 
	{
		_ASSERTE( !_T("Invalid data in MINIDUMP_STRING structure.") ); 
		return rv; 
	}

	rv = pStrBuf->Buffer; 

	return rv; 

}

bool ReadThreadInfo( MINIDUMPREADDUMPSTREAM pDump, PVOID pMiniDump);

bool ReadMemoryInfo( MINIDUMPREADDUMPSTREAM pDump, PVOID pMiniDump);


bool ReadDumpStream()
{
	//	MiniDumpReadDumpStream

	HMODULE hDll = NULL;
	if ( hDll == NULL)
	{
		// load any version we can
		hDll = ::LoadLibrary( "DBGHELP.DLL" );	
	}

	LPCTSTR szResult = NULL;

	if ( hDll)
	{		
		MINIDUMPREADDUMPSTREAM pDump = (MINIDUMPREADDUMPSTREAM)::GetProcAddress( hDll, "MiniDumpReadDumpStream" );
		if ( pDump)
		{
			HANDLE pViewOfFile = NULL;

			WCHAR path[ MAX_PATH];
			getApplicationDataDirectory( path);
			wcscat( path, L"\\player_hang.dmp");

			HANDLE	hFile = CreateFileW( path, GENERIC_READ, 0, NULL, OPEN_EXISTING, NULL, NULL);

			if( ( hFile == NULL ) || ( hFile == INVALID_HANDLE_VALUE ) ) 
			{
				return false;
			}

			HANDLE hMapFile = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, 0, NULL ); 
			//HANDLE hFile = OpenFileMapping( FILE_MAP_READ, FALSE, path);				

			if ( hMapFile == NULL)
				return false;

			pViewOfFile = MapViewOfFile( hMapFile, FILE_MAP_READ, 0, 0, 0);
			if ( pViewOfFile == NULL)
			{
				if( hMapFile != NULL ) 				
					CloseHandle( hMapFile);
				return false;
			}

			ReadThreadInfo( pDump, pViewOfFile);

			ReadMemoryInfo( pDump, pViewOfFile);

			if( pViewOfFile != NULL )
			{
				if( !UnmapViewOfFile( pViewOfFile ) ) 
				{
					DWORD ec = GetLastError();
					_ASSERTE( !_T("UnmapViewOfFile() failed.") );
				}
			}

			if( hMapFile != NULL ) 
			{
				if( !CloseHandle( hMapFile ) ) 
				{
					//DWORD ec = GetLastError();
					//_ASSERTE( !_T("CloseHandle() failed.") );
				}
			}
		}
	}
	return true;
}

bool ReadThreadInfo( MINIDUMPREADDUMPSTREAM pDump, PVOID pMiniDump)
{
	PMINIDUMP_DIRECTORY   pMiniDumpDir  = 0; 
	PVOID                 pStream       = 0; 
	ULONG                 StreamSize    = 0; 

	PMINIDUMP_DIRECTORY Dir = NULL;
	PMINIDUMP_THREAD_EX_LIST  StreamPointer = NULL;
	
	if ( pDump( pMiniDump, ThreadListStream, &pMiniDumpDir, &pStream, &StreamSize))
	{
		if ( ( pStream == 0 ) || ( StreamSize == 0 ))
		{
			//_ASSERTE( !_T("Invalid minidump format.") );					
			return false; 				
		}

		// Process the stream data
		PMINIDUMP_THREAD_LIST pInfo = (PMINIDUMP_THREAD_LIST) pStream;
		for ( int i = 0; i < pInfo->NumberOfThreads; i++)
		{
			std::wstring sInfo( L"n/a" );
			MINIDUMP_THREAD pThreadInfo = pInfo->Threads[ i];
			//pThreadInfo->ThreadId
			MINIDUMP_MEMORY_DESCRIPTOR* pStackInfo = &pThreadInfo.Stack;
			MINIDUMP_LOCATION_DESCRIPTOR *pThreadContext = &pThreadInfo.ThreadContext;
			//MINIDUMP_MEMORY_DESCRIPTOR *pBackingStore = &pThreadInfo.BackingStore;
			int threadID = pThreadInfo.ThreadId;
			if( pThreadContext->Rva != 0 )
				sInfo = GetMiniDumpStr( (PVOID) pMiniDump, pThreadContext->Rva);

			MINIDUMP_LOCATION_DESCRIPTOR *pThreadContextNext = &pStackInfo->Memory;
			//pStackInfo->StartOfMemoryRange;
			if( pThreadContextNext->Rva != 0 )
				sInfo = GetMiniDumpStr( (PVOID) pMiniDump, pThreadContextNext->Rva);
		}
	}
}

bool ReadMemoryInfo( MINIDUMPREADDUMPSTREAM pDump, PVOID pMiniDump)
{	
	PMINIDUMP_DIRECTORY   pMiniDumpDir  = 0; 
	PVOID                 pStream       = 0; 
	ULONG                 StreamSize    = 0; 

	PMINIDUMP_DIRECTORY Dir = NULL;
	PMINIDUMP_MEMORY_INFO_LIST StreamPointer = NULL;	

	if ( pDump( pMiniDump, MemoryInfoListStream, &pMiniDumpDir, &pStream, &StreamSize))
	{
		if ( ( pStream == 0 ) || ( StreamSize == 0 ))
		{
			return false;
		}

		// Process the stream data

		const MINIDUMP_MEMORY_INFO_LIST* pInfo = (const MINIDUMP_MEMORY_INFO_LIST*)pStream; 

		// Check the contents of MINIDUMP_HANDLE_DATA_STREAM structure 

		if( pInfo->SizeOfHeader != sizeof(MINIDUMP_MEMORY_INFO_LIST) ) 
		{
			_ASSERTE( !_T("Incorrect MINIDUMP_MEMORY_INFO_LIST.SizeOfHeader") ); 		
			return false; 
		}

		if( pInfo->SizeOfEntry != sizeof(MINIDUMP_MEMORY_INFO) ) 
		{
			_ASSERTE( !_T("Incorrect MINIDUMP_MEMORY_INFO.SizeOfDescriptor") ); 
			return false; 
		}

		// Obtain and save the handle information 

		const MINIDUMP_MEMORY_INFO_LIST* pTemp = ( pInfo + 1 ); 

		const MINIDUMP_MEMORY_INFO* pMemoryInfo = (const MINIDUMP_MEMORY_INFO*)pTemp; 

		for( unsigned int i = 0; i < pInfo->NumberOfEntries; i++, pMemoryInfo++ ) 
		{
			//std::wstring TypeName( L"n/a" );
			//std::wstring ObjName( L"n/a" );

			WCHAR szCrashModulePathName[MAX_PATH*2] = {L'\0'};
			//ZeroMemory(szCrashModulePathName, sizeof(szCrashModulePathName)*sizeof(szCrashModulePathName[0]));						

			WCHAR *pszCrashModuleFileName = L"Unknown";

			if (GetModuleFileNameW((HINSTANCE)pMemoryInfo->AllocationBase,
				szCrashModulePathName,
				sizeof(szCrashModulePathName)-2) > 0)
			{
				pszCrashModuleFileName = GetFilePart(szCrashModulePathName);
			}
		
			int x = 0;

		}
	}

	int errorID = ::GetLastError();

	return true;
}


#include <tlhelp32.h>

// Tool Help functions.
typedef	HANDLE (WINAPI * CREATE_TOOL_HELP32_SNAPSHOT)(DWORD dwFlags, DWORD th32ProcessID);
typedef	BOOL (WINAPI * MODULE32_FIRST)(HANDLE hSnapshot, LPMODULEENTRY32W lpme);
typedef	BOOL (WINAPI * MODULE32_NEST)(HANDLE hSnapshot, LPMODULEENTRY32W lpme);

CREATE_TOOL_HELP32_SNAPSHOT	CreateToolhelp32Snapshot_;
MODULE32_FIRST	Module32First_;
MODULE32_NEST	Module32Next_;

#define	DUMP_SIZE_MAX	8000	//max size of our dump
#define	CALL_TRACE_MAX	((DUMP_SIZE_MAX - 2000) / (MAX_PATH + 40))	//max number of traced calls
#define	NL				"\r\n"	//new line

//****************************************************************************************
bool Get_Module_By_Ret_Addr(int iProcessId, PBYTE Ret_Addr, PWCHAR Module_Name, BYTE * Module_Addr)
//****************************************************************************************
// Find module by Ret_Addr (address in the module).
// Return Module_Name (full path) and Module_Addr (start address).
// Return TRUE if found.
{
	HMODULE hKernel32 = GetModuleHandle("KERNEL32");
	CreateToolhelp32Snapshot_ = (CREATE_TOOL_HELP32_SNAPSHOT)GetProcAddress(hKernel32, "CreateToolhelp32Snapshot");
	Module32First_ = (MODULE32_FIRST)GetProcAddress(hKernel32, "Module32FirstW");
	Module32Next_ = (MODULE32_NEST)GetProcAddress(hKernel32, "Module32NextW");

	MODULEENTRY32W	M = {sizeof(M)};
	HANDLE	hSnapshot;

	Module_Name[0] = L'\0';

	if (CreateToolhelp32Snapshot_)
	{
		hSnapshot = CreateToolhelp32Snapshot_(TH32CS_SNAPMODULE, iProcessId);

		if ((hSnapshot != INVALID_HANDLE_VALUE) &&
			Module32First_(hSnapshot, &M))
		{
			do
			{
				if (DWORD(Ret_Addr - M.modBaseAddr) < M.modBaseSize)
				{
					lstrcpynW(Module_Name, M.szExePath, MAX_PATH);
					Module_Addr = M.modBaseAddr;
					break;
				}
			} while (Module32Next_(hSnapshot, &M));
		}

		CloseHandle(hSnapshot);
	}

	return Module_Name[0] != '\0';
} //Get_Module_By_Ret_Addr

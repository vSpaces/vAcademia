// mdump.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "mdump.h"
#include "Winsock2.h"
#include "psapi.h"

//#include "DialogDebugError.h"
#include <malloc.h>
#include <io.h>

#if MDUMP_STATIC_LIB
#else

HINSTANCE hInst = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			hInst = (HINSTANCE) hModule;
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

// This is an example of an exported variable
MDUMP_API int nmdump=0;

// This is an example of an exported function.
MDUMP_API int fnmdump(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see mdump.h for the class definition
Cmdump::Cmdump()
{ 
	return; 
}

#endif

#include "MiniVersion.h"
#include "GetWinVer.h"
#include "func.h"

//LPCTSTR lpModules[] = {"EvoVoIP.dll", "vu.exe", "ntdll.dll", "Unknown"};
LPCWSTR lpSkipExceptionOnTernimationModules[] = {L"EvoVoIP.dll"};
LPCWSTR lpSkipExceptionRuntimeModules[] = {L"EvoVoIP.dll"};

long g_iSessionID = 0;
TCHAR g_userName[MAX_PATH] = "\0";
TCHAR g_password[MAX_PATH] = "\0";
int g_iEncodingPassword = 0;
TCHAR g_appVersion[MAX_PATH] = "\0";
WCHAR g_root[2*MAX_PATH_4] = L"\0";
TCHAR g_launcherName[MAX_PATH] = "\0";
int g_iLanguage = ENG;
BOOL g_bAppFinishing = FALSE;
BOOL g_bAllowRestartApp = TRUE;
BOOL g_bSendCrashShow = FALSE;


//#include "CrashHandler.h"
//CCrashHandler ch;

LPTOP_LEVEL_EXCEPTION_FILTER StartExceptionHandler(LPTOP_LEVEL_EXCEPTION_FILTER apFilterFunction)
{	
	::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);	
	return ::SetUnhandledExceptionFilter( apFilterFunction );
}

void StartExceptionHandlers()
{
//	ch.SetProcessExceptionHandlers();
//	ch.SetThreadExceptionHandlers();	
}

bool skipExceptionForRuntimeModule( wchar_t* lpModuleName)
{
	for( int i=0; i<sizeof(lpSkipExceptionRuntimeModules)/sizeof(lpSkipExceptionRuntimeModules[0]); i++)
		if( wcscmp( lpModuleName, lpSkipExceptionRuntimeModules[i]) == 0)
			return true;
	return false;
}

bool skipExceptionForModule( wchar_t* lpModuleName)
{
	if ( !g_bAppFinishing)
		return false;

	for( int i=0; i<sizeof(lpSkipExceptionOnTernimationModules)/sizeof(lpSkipExceptionOnTernimationModules[0]); i++)
		if( wcscmp( lpModuleName, lpSkipExceptionOnTernimationModules[i]) == 0)
			return true;
	return false;
}

void SetSessionID( long aSessionID)
{
	g_iSessionID = aSessionID;
}

void SetUserMDump( const wchar_t *aUserName, const wchar_t *aPassword, int aiEncodingPassword)
{
	USES_CONVERSION;
	char *userName_ = W2A( aUserName);
	char *password_ = W2A( aPassword);
	_tcscpy( g_userName, userName_);
	_tcscpy( g_password, password_);
	g_iEncodingPassword = aiEncodingPassword;
}

void SetUserMDump( const char *aUserName, const char *aPassword, int aiEncodingPassword)
{	
	_tcscpy( g_userName, aUserName);
	_tcscpy( g_password, aPassword);	
	g_iEncodingPassword = aiEncodingPassword;
}

void SetAppVersionMDump( const TCHAR *aAppVersion)
{
	_tcscpy( g_appVersion, aAppVersion);
}

void SetRootMDump( const WCHAR *aRoot)
{
	wcscpy( g_root, aRoot);	
}

void SetLauncherMDump( const TCHAR *aLauncherName)
{
	_tcscpy( g_launcherName, aLauncherName);
}

void SetAppFinishing()
{
	g_bAppFinishing = TRUE;
	g_bAllowRestartApp = FALSE;
}

CWS3D *gpWS3D = NULL;

void SetWS3D( CWS3D *pWS3D)
{
	gpWS3D = pWS3D;
}

// скрывает картинку запуска приложени€
void HideLauncher(const TCHAR * szEventName)
{
	HANDLE hEvent = OpenEvent( 
		EVENT_ALL_ACCESS,      // request full access
		FALSE,                 // handle not inheritable
		szEventName);  // object name

	if (hEvent == NULL)
		return;

	SetEvent(hEvent);
}

BOOL getAppParams( WCHAR *sRoot, TCHAR **serverName, TCHAR **appVersion, int *iLanguage, FILE *fpLog)
{
	if ( PRIMARYLANGID( GetSystemDefaultLangID()) == LANG_RUSSIAN 
		&& PRIMARYLANGID( GetUserDefaultLangID()) == LANG_RUSSIAN)
	{
		*iLanguage = RUS;		
	}
	else
		*iLanguage = ENG;

	WCHAR szModuleName[MAX_PATH_4*2] = {L'\0'};
	//ZeroMemory(szModuleName, sizeof(szModuleName));
	if (GetModuleFileNameW(0, szModuleName, _countof(szModuleName)-2) <= 0)
		lstrcpyW(szModuleName, L"Unknown");

	WCHAR *fileName = new WCHAR[ MAX_PATH_4];	

	if ( wcslen( sRoot) > 2 && wcsstr( sRoot, L":") != NULL)
	{
		wcscpy( fileName, sRoot);
		wcscat( fileName, L"\\");
		wcscat( fileName, CONFIG_FILE);
	}
	else
	{
		wcscpy( fileName, szModuleName);
		WCHAR *pszFilePart = GetFilePart( fileName);
		lstrcpyW(pszFilePart, CONFIG_FILE);
	}	

	FILE *fp = _wfopen( fileName, L"r");
	int errorCode = GetLastError();
	USES_CONVERSION;
	if ( fp == NULL)
	{
		TCHAR *s = new TCHAR[ MAX_PATH_4];		
		sprintf( s, "WARNING: config file '%s' is can't opened, errorCode='%d'\n", W2A( fileName), errorCode);
		WriteLog__( fpLog, s);
		delete[] s;

		wcscpy( fileName, szModuleName);
		WCHAR *pszFilePart = GetFilePart( fileName);
		lstrcpyW(pszFilePart, CONFIG_FILE);
		fp = _wfopen( fileName, L"r");
	}

	if ( fp != NULL)
	{
		int size = _filelength( fp->_file);
		char *str = new char[ size];
		if ( fread( str, 1, size, fp) <= 0)
		{
			TCHAR *s = new TCHAR[ MAX_PATH_4];
			sprintf( s, "ERROR: size of config file '%s' is equal 0\n",  W2A( fileName));
			WriteLog__( fpLog, s);
			delete[] s;
			strcpy( *appVersion, g_appVersion);
			delete[] str;
			delete[] fileName;
			return TRUE;
		}
		delete[] str;
		fclose( fp);

		if ( !GetValueStrFromIni( fileName, "settings", "dumpserver", serverName, fpLog))
		{
			strcpy( *appVersion, g_appVersion);
			strcpy( *serverName, SERVER_NAME);
			delete[] fileName;
			return TRUE;
		}		

		if ( !GetValueStrFromIni( fileName, "settings", "version", appVersion, fpLog))
		{
			strcpy( *appVersion, g_appVersion);	
			delete[] fileName;
			return TRUE;
		}

		TCHAR *sLang = new TCHAR[ MAX_PATH];
		if ( !GetValueStrFromIni( fileName, "settings", "language", &sLang, fpLog))
		{
			delete[] sLang;
			delete[] fileName;
			return TRUE;
		}

		
		//GetPrivateProfileStringW( "settings", "dumpserver", SERVER_NAME, *serverName, 128, fileName);
		//GetPrivateProfileStringW( "settings", "version", "0.0.0.0", *appVersion, 128, fileName);
		//GetPrivateProfileStringW( "settings", "language", "eng", sLang, 128, fileName);

		if ( stricmp( sLang, "rus") == 0)
		{
			*iLanguage = RUS;
		}
		else if ( stricmp( sLang, "kzh") == 0)
		{
			*iLanguage = KZH;
		}
		else
			*iLanguage = ENG;
		delete[] sLang;
		delete[] fileName;

		return TRUE;
	}
	errorCode = GetLastError();
	TCHAR *s = new TCHAR[ MAX_PATH_4];
	sprintf( s, "ERROR:config file '%s' is can't opened, errorCode='%d'\n", W2A(fileName), errorCode);
	WriteLog__( fpLog, s);
	delete[] s;
	strcpy( *appVersion, g_appVersion);
	strcpy( *serverName, SERVER_NAME);
	delete[] fileName;
	return TRUE;
}

LONG WINAPI MDumpFilterFunction( EXCEPTION_POINTERS *pExceptionInfo, int iCrashType /*= APP_ERROR_ID*/)
{	
	FILE *fp2__ = NULL;
	OpenLog__( &fp2__, NULL, "mdump2");
	TCHAR sTh[MAX_PATH];
	sprintf( sTh, "TopLevelFilter::Start - GetCurrentThreadId = %d, GetCurrentProcessID = %d\n", GetCurrentThreadId(), GetCurrentProcessId());
	WriteLog__( fp2__, sTh);

	static bool bEnding = false;
	static bool bFirstTime = true;
	static bool bFirstDumpErrTime = true;
	static int currentID = ::GetCurrentThreadId();

	sprintf( sTh, "currentID = %d, GetCurrentThreadId = %d, iCrashType = %d\n", currentID, ::GetCurrentThreadId(), iCrashType);
	WriteLog__( fp2__, sTh);
	CloseLog__( &fp2__);

	if ( !bFirstTime)	// Going recursive! That must mean this routine crashed!
	{
		TCHAR s[MAX_PATH];
		sprintf( s, "mdump_%d", ::GetCurrentThreadId());
		OpenLog__( &fp2__, NULL, s);
		sprintf( s, "currentID = %d, GetCurrentThreadId = %d\n", currentID, ::GetCurrentThreadId());
		WriteLog__( fp2__, s);		

		if ( !bEnding && currentID == ::GetCurrentThreadId())
		{
			WriteLog__( fp2__, "Error creating exception report.\r\n");
			CloseLog__( &fp2__);
			// упало в дампе?
			// подождем 5 секунд. ћожет быть bEnding установитс€ в значение true
			Sleep( 5000);
			if ( bEnding)
				return EXCEPTION_EXECUTE_HANDLER;
			if ( bFirstDumpErrTime)
			{
				bFirstDumpErrTime = false;
				// создаем дамп как assert
				BOOL bSucc = _make_dump( g_root, ::GetCurrentProcess(), TRUE, ASSERT_ERROR_ID);
				bEnding = true;
				return bSucc ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
			}
			// второй раз упало в дампе?
			// отправл€ем на повисание
		}
		CloseLog__( &fp2__);

		while( !bEnding)
			Sleep( 1000);
		return EXCEPTION_EXECUTE_HANDLER;
	}

	bFirstTime = false;


	PEXCEPTION_RECORD Exception = pExceptionInfo->ExceptionRecord;
	PCONTEXT          Context   = pExceptionInfo->ContextRecord;

	WCHAR szCrashModulePathName[MAX_PATH_4*2] = {L'\0'};
	//ZeroMemory(szCrashModulePathName, sizeof(szCrashModulePathName));

	WCHAR *pszCrashModuleFileName = L"Unknown";

	MEMORY_BASIC_INFORMATION MemInfo;

	// VirtualQuery can be used to get the allocation base associated with a
	// code address, which is the same as the ModuleHandle. This can be used
	// to get the filename of the module that the crash happened in.
	if (VirtualQuery((void*)Context->Eip, &MemInfo, sizeof(MemInfo)) &&
		(GetModuleFileNameW((HINSTANCE)MemInfo.AllocationBase,
		szCrashModulePathName,
		sizeof(szCrashModulePathName)-2) > 0))
	{
		pszCrashModuleFileName = GetFilePart(szCrashModulePathName);
	}

	if( skipExceptionForRuntimeModule( pszCrashModuleFileName))
	{
		return EXCEPTION_VA_RUNTIME_MODULE;
	}
	

	DeleteLog__( NULL, "mdump");
	TBuffer buffer;
	FILE *fp__ = NULL;
	OpenLog__( &fp__, NULL, "mdump");
	WriteLog__( fp__, "Start\n");
	SYSTEMTIME time;
	TCHAR sTime[ MAX_PATH];
	GetSystemTime( &time);	
	sprintf( sTime, "system time = %d.%d.%d-%d:%d:%d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);	
	WriteLog__( fp__, sTime);
	GetLocalTime( &time);
	sprintf( sTime, "local time = %d.%d.%d-%d:%d:%d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);	
	WriteLog__( fp__, sTime);

	// Create a filename to record the error information to.
	// Storing it in the executable directory works well.
	WriteLog__( fp__, "Start2\n");
	
	WCHAR szModuleName[MAX_PATH_4*2] = {L'\0'};
	//ZeroMemory(szModuleName, sizeof(szModuleName));
	if (GetModuleFileNameW(0, szModuleName, _countof(szModuleName)-2) <= 0)
		lstrcpyW(szModuleName, L"Unknown");
	WriteLog__( fp__, szModuleName);
	WriteLog__( fp__, "\n");
	//CloseLog__( &fp__);
	TCHAR appName[ MAX_PATH];

	WCHAR *pszFilePart = GetFilePart(szModuleName);
	USES_CONVERSION;
	strcpy( appName, W2A(pszFilePart));
	TCHAR m_szAppName[ MAX_PATH];
	strcpy( m_szAppName, appName);
	TCHAR *lastperiod = lstrrchr(m_szAppName, _T('.'));
	if (lastperiod)
		lastperiod[0] = 0;

	WriteLog__( fp__, m_szAppName);
	WriteLog__( fp__, "\n");

	if ( stricmp( m_szAppName, "VoipMan") == 0)
	{
		g_bSendCrashShow = FALSE;
		g_bAllowRestartApp = FALSE;
	}

	// Extract the file name portion and remove it's file extension
	TCHAR szFileName[MAX_PATH_4*2];
	lstrcpy(szFileName, W2A(pszFilePart));
	

	// Replace the executable filename with our error log file name
	lstrcpyW(pszFilePart, iCrashType == APP_LAUNCHER_ERROR_ID ? XCRASHREPORT_LAUCNHER_ERROR_LOG_FILE : XCRASHREPORT_ERROR_LOG_FILE);

	WriteLog__( fp__, "CreateFile Start\n");

	WCHAR path[ MAX_PATH_4];
	getApplicationDataDirectory( path);
	WCHAR szModuleInfoName[ MAX_PATH_4];
	wcscpy( szModuleInfoName, path);
	wcscat( szModuleInfoName, L"\\");
	wcscat( szModuleInfoName, XCRASHREPORT_ERROR_LOG_FILE);

	HANDLE hLogFile = CreateFileW( szModuleInfoName, GENERIC_WRITE, 0, 0,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 0);

	if (hLogFile == INVALID_HANDLE_VALUE)
	{
		//OutputDebugString(_T("Error creating exception report\r\n"));		
		WriteLog__( fp__, "MDumpFilterFunction::Error creating exception report\r\n");
		CloseLog__( &fp__);
		bEnding = true;
		return EXCEPTION_CONTINUE_SEARCH;
	}

	// Append to the error log
	SetFilePointer(hLogFile, 0, 0, FILE_END);

	// Print out a blank line to separate this error log from any previous ones
	//hprintf(hLogFile, _T("\r\n"));

//////////////////////////////////////////////////////////////////////////
//
// Exception Address
//
//////////////////////////////////////////////////////////////////////////
	WriteLog__( fp__, "Exception Address Start\n");

	/*PEXCEPTION_RECORD Exception = pExceptionInfo->ExceptionRecord;
	PCONTEXT          Context   = pExceptionInfo->ContextRecord;

	TCHAR szCrashModulePathName[MAX_PATH_4*2];
	ZeroMemory(szCrashModulePathName, sizeof(szCrashModulePathName));*/

	WriteLog__( fp__, "Exception Address\n");

	/*TCHAR *pszCrashModuleFileName = _T("Unknown");

	MEMORY_BASIC_INFORMATION MemInfo;

	// VirtualQuery can be used to get the allocation base associated with a
	// code address, which is the same as the ModuleHandle. This can be used
	// to get the filename of the module that the crash happened in.
	if (VirtualQuery((void*)Context->Eip, &MemInfo, sizeof(MemInfo)) &&
		(GetModuleFileName((HINSTANCE)MemInfo.AllocationBase,
		szCrashModulePathName,
		sizeof(szCrashModulePathName)-2) > 0))
	{
		pszCrashModuleFileName = GetFilePart(szCrashModulePathName);
	}*/
	CloseLog__( &fp__);
	OpenLog__( &fp__, NULL, "mdump");
	WriteLog__( fp__, "Exception Address VirtualQuery\n");

	// Print out the beginning of the error log in a Win95 error window
	// compatible format.	

	if ( iCrashType == APP_LAUNCHER_ERROR_ID)
	{
		hprintf(hLogFile, &buffer, _T("%s caused an launcher %s (0x%08x) \r\nin module %s at %04x:%08x.\r\n"),
			szFileName, GetExceptionDescription(Exception->ExceptionCode),
			Exception->ExceptionCode,
			W2A(pszCrashModuleFileName), Context->SegCs, Context->Eip);
	}
	else
	{
		hprintf(hLogFile, &buffer, _T("%s caused %s (0x%08x) \r\nin module %s at %04x:%08x.\r\n"),
			szFileName, GetExceptionDescription(Exception->ExceptionCode),
			Exception->ExceptionCode,
			W2A(pszCrashModuleFileName), Context->SegCs, Context->Eip);
	}

	TCHAR *sparams = new TCHAR[ 4 * MAX_PATH];
	
	TCHAR *sUUID = new TCHAR[ MAX_PATH];
	if ( getUUID( &sUUID))
	{
		hprintf(hLogFile, &buffer, _T("UserNameInAcademia=\"%s\" SessionID=\"%ld\" UUID=\"%s\".\r\n"), g_userName, g_iSessionID, sUUID);
		sprintf( sparams, "UserNameInAcademia=\"%s\" SessionID=\"%ld\" UUID=\"%s\".\r\n", g_userName, g_iSessionID, sUUID);
		WriteLog__( fp__, sparams);	
	}
	else
	{
		hprintf(hLogFile, &buffer, _T("UserNameInAcademia=\"%s\" SessionID=\"%ld\".\r\n"), g_userName, g_iSessionID);	
		sprintf( sparams, "UserNameInAcademia=\"%s\" SessionID=\"%ld\".\r\n", g_userName, g_iSessionID);
		WriteLog__( fp__, sparams);	
	}

	hprintf(hLogFile, &buffer, _T("g_appVersion=\"%s\"\r\n"), g_appVersion);
	
	sprintf( sparams, "g_appVersion = '%s'\n", g_appVersion);
	WriteLog__( fp__, sparams);	

	TCHAR *serverName = new TCHAR[MAX_PATH];
	TCHAR *appVersion = new TCHAR[MAX_PATH];
	serverName[0] = 0;
	appVersion[0] = 0;
	int iLanguage = ENG;
	getAppParams( g_root, &serverName, &appVersion, &iLanguage, fp__);	
	hprintf(hLogFile, &buffer, _T("AppVersion=\"%s\" ServerName=\"%s\" Language=\"%s\".\r\n\r\n"), appVersion, serverName, iLanguage == RUS ? "rus" : "eng");
	TCHAR *sCrashType = ( iCrashType == APP_HANG_ID) ? APP_HANG : ( ( iCrashType == ASSERT_ERROR_ID) ? ASSERT_ERROR : (( iCrashType == APP_LAUNCHER_ERROR_ID) ? APP_LAUNCHER_ERROR : APP_ERROR));	
	sprintf( sparams, "szCrashType=\"%s\" AppVersion=\"%s\" ServerName=\"%s\" Language=\"%s\".", sCrashType, appVersion, serverName, (iLanguage == RUS ? "rus" : "eng"));
	WriteLog__( fp__, sparams);	
	
	delete[] sparams;
	delete[] appVersion;
	delete[] serverName;
	delete[] sUUID;	

	if ( skipExceptionForModule( pszCrashModuleFileName))
	{
		// перезапуск
		//restartModule();
		TCHAR s[ MAX_PATH_4];
		sprintf( s, "skipExceptionForModule::pszCrashModuleFileName = %s\n", W2A(pszCrashModuleFileName));
		WriteLog__( fp__, s);
		CloseLog__( &fp__);
		bEnding = true;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	WriteLog__( fp__, "Exception Address hprintf\n");
	//hprintf(hLogFile, _T("Exception handler called in %s.\r\n"), lpszMessage);

//////////////////////////////////////////////////////////////////////////
//
// System Information
//
//////////////////////////////////////////////////////////////////////////
	
	WriteLog__( fp__, "System Info\n");
	CloseLog__( &fp__);
	OpenLog__( &fp__, NULL, "mdump");

	HANDLE hProcess = GetCurrentProcess();
	WriteLog__( fp__, "DumpSystemInformation -----\n");
	DumpSystemInformation(hLogFile, hProcess, &buffer);

	// If the exception was an access violation, print out some additional
	// information, to the error log and the debugger.
	if (Exception->ExceptionCode == STATUS_ACCESS_VIOLATION &&
		Exception->NumberParameters >= 2)
	{
		TCHAR szDebugMessage[1000];
		const TCHAR* readwrite = _T("Read from");
		if (Exception->ExceptionInformation[0])
			readwrite = _T("Write to");
		wsprintf(szDebugMessage, _T("%s location %08x caused an access violation.\r\n"),
			readwrite, Exception->ExceptionInformation[1]);

#ifdef	_DEBUG
		// The Visual C++ debugger doesn't actually tell you whether a read
		// or a write caused the access violation, nor does it tell what
		// address was being read or written. So I fixed that.
		OutputDebugString(_T("Exception handler: "));
		OutputDebugString(szDebugMessage);
#endif

		hprintf(hLogFile, &buffer, _T("%s"), szDebugMessage);
	}

	WriteLog__( fp__, "System Info 2\n");
	CloseLog__( &fp__);
	OpenLog__( &fp__, NULL, "mdump");

	int flag = 0; 
	PCHAR Str = Get_Exception_Info( pExceptionInfo, fp__, flag);
	if ( Str)
		hprintf(hLogFile, &buffer, _T("%s"), Str);
	if ( flag == 1)
	{
		// перезапуск системы
		//restartModule();
		WriteLog__( fp__, "flag == 1\n");
		CloseLog__( &fp__);
		bEnding = true;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	

	hprintf(hLogFile, &buffer, _T("\r\n"));	

//////////////////////////////////////////////////////////////////////////
//
// Registers
//
//////////////////////////////////////////////////////////////////////////
	
	CloseLog__( &fp__);
	OpenLog__( &fp__, NULL, "mdump");
	WriteLog__( fp__, "Register Info\n");		

	DumpRegisters(hLogFile, Context, &buffer);

	// Print out the bytes of code at the instruction pointer. Since the
	// crash may have been caused by an instruction pointer that was bad,
	// this code needs to be wrapped in an exception handler, in case there
	// is no memory to read. If the dereferencing of code[] fails, the
	// exception handler will print '??'.
	hprintf(hLogFile, &buffer, _T("\r\nBytes at CS:EIP:\r\n"));
	BYTE * code = (BYTE *)Context->Eip;
	for (int codebyte = 0; codebyte < buffer.NumCodeBytes; codebyte++)
	{
		__try
		{
			hprintf(hLogFile, &buffer, _T("%02x "), code[codebyte]);

		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			hprintf(hLogFile, &buffer, _T("?? "));
		}
	}
	hprintf(hLogFile, &buffer, _T("\r\n"));

//////////////////////////////////////////////////////////////////////////
//
// Stack
//
//////////////////////////////////////////////////////////////////////////

	// Time to print part or all of the stack to the error log. This allows
	// us to figure out the call stack, parameters, local variables, etc.

	// Esp contains the bottom of the stack, or at least the bottom of
	// the currently used area
	//DWORD* pStack = (DWORD *)Context->Esp;

	//DumpStack(hLogFile, pStack, &buffer);

//////////////////////////////////////////////////////////////////////////
//
// Modules
//
//////////////////////////////////////////////////////////////////////////

	CloseLog__( &fp__);
	OpenLog__( &fp__, NULL, "mdump");
	WriteLog__( fp__, "Modules Info\n");	

	hprintf(hLogFile, &buffer, _T("\r\n---------------\r\n"));

	DumpModuleList(hLogFile, &buffer);

	hflush(hLogFile, &buffer);

	hprintf(hLogFile, &buffer, _T("\r\n===== [end of %s] =====\r\n"), 
		XCRASHREPORT_ERROR_LOG_FILE);	
	CloseHandle(hLogFile);

///////////////////////////////////////////////////////////////////////////
//
// write minidump
//
///////////////////////////////////////////////////////////////////////////		
		
	HWND hParent = NULL;						// find a better value for your app
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
    
	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	HMODULE hDll = NULL;
	char szDbgHelpPath[ _MAX_PATH];

	/*if (GetModuleFileName( NULL, szDbgHelpPath, _MAX_PATH ))
	{
		char *pSlash = _tcsrchr( szDbgHelpPath, '\\' );
		if ( pSlash)
		{
			_tcscpy( pSlash+1, "DBGHELP.DLL" );
			hDll = ::LoadLibrary( szDbgHelpPath );
		}
	}
	//*/

	CloseLog__( &fp__);
	OpenLog__( &fp__, NULL, "mdump");
	WriteLog__( fp__, "Loading 'DBGHELP.DLL'...");
	WriteLog__( fp__, "\n");

	if ( hDll == NULL)
	{
	// load any version we can
		hDll = ::LoadLibrary( "DBGHELP.DLL" );
	}

	LPCTSTR szResult = NULL;
	char szScratch[_MAX_PATH];

	if ( hDll)
	{
		WriteLog__( fp__, "Dll 'DBGHELP.DLL' is loaded");
		WriteLog__( fp__, "\n");
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if ( pDump)
		{			
			wchar_t szDumpPath[MAX_PATH_4];
			//WriteLog__( "pDump is valid\n");
			if ( !getFileNameByProjName( m_szAppName, L"dmp", szDumpPath))
			{
				WriteLog__( fp__, "getFileNameByProjName return error");
				bEnding = true;
				return EXCEPTION_CONTINUE_SEARCH;
			}
			
			// create the file
			HANDLE hFile = ::CreateFileW( szDumpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL );

			if ( hFile != INVALID_HANDLE_VALUE)
			{
				_MINIDUMP_EXCEPTION_INFORMATION ExInfo;					
				ExInfo.ThreadId = ::GetCurrentThreadId();
				ExInfo.ExceptionPointers = pExceptionInfo;
				ExInfo.ClientPointers = FALSE;					
				
				// write the dump //MiniDumpNormal MiniDumpWithHandleData MiniDumpWithIndirectlyReferencedMemory  
				// MiniDumpWithDataSegs MiniDumpWithFullMemory  
				BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, static_cast<MINIDUMP_TYPE>(MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithProcessThreadData | MiniDumpWithHandleData), &ExInfo, NULL, NULL);
				//BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithIndirectlyReferencedMemory, &ExInfo, NULL, NULL);
				if (bOK)
				{
					sprintf( szScratch, "MDumpFilterFunction::Saved dump file to '%s'\n", W2A(szDumpPath) );
					WriteLog__( fp__, szScratch);
					WriteLog__( fp__, "\n");
					szResult = szScratch;

					retval = EXCEPTION_EXECUTE_HANDLER;
				}
				else
				{
					int error = HRESULT_FROM_WIN32(GetLastError());
					sprintf( szScratch, "MDumpFilterFunction::Failed to save dump file to '%s' (error %d)\n", W2A(szDumpPath), error);
					WriteLog__( fp__, szScratch);
					WriteLog__( fp__, "\n");
					szResult = szScratch;
				}
				::CloseHandle(hFile);				
				if ( ZipFiles( APP_ERROR_ID, m_szAppName, szDumpPath, fp__, "mdump") < 2)
				{
					WriteLog__( fp__, "ZipFiles is failed\n");				
				}
			}
			else
			{
				sprintf( szScratch, "Failed to create dump file '%s' (error %u)\n", W2A(szDumpPath), GetLastError() );
				WriteLog__( fp__, szScratch);
				WriteLog__( fp__, "\n");
				CloseLog__( &fp__);
				szResult = szScratch;
				bEnding = true;
				return EXCEPTION_CONTINUE_SEARCH;
			}			
		}
		else
		{
			szResult = "DBGHELP.DLL too old";
			sprintf( szScratch, "DBGHELP.DLL too old (error %u)\n", GetLastError());
			WriteLog__( fp__, szScratch);
			WriteLog__( fp__, "\n");
			CloseLog__( &fp__);
			bEnding = true;
			return EXCEPTION_CONTINUE_SEARCH;
		}
	}
	else
	{
		//szResult = "DBGHELP.DLL not found";
		sprintf( szScratch, "DBGHELP.DLL not found (error %u)\n", GetLastError());
		WriteLog__( fp__, szScratch);
		WriteLog__( fp__, "\n");
		CloseLog__( &fp__);
		bEnding = true;
		return EXCEPTION_CONTINUE_SEARCH;
	}	

	GetSystemTime( &time);	
	sprintf( sTime, "creating dump is finished \n  system time = %d.%d.%d-%d:%d:%d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);	
	WriteLog__( fp__, sTime);
	GetLocalTime( &time);
	sprintf( sTime, "  local time = %d.%d.%d-%d:%d:%d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);	
	WriteLog__( fp__, sTime);
	CloseLog__( &fp__);	
	bEnding = true;
//////////////////////////////////////////////////////////////////////////
//
// pop up our crash report app
//
//////////////////////////////////////////////////////////////////////////	
	retval = SendReport( appName, pszFilePart, szModuleName, fp__, sCrashType);	

	return retval;
}

///////////////////////////////////////////////////////////////////////////////

// hflush
void hflush(HANDLE LogFile, TBuffer *buffer)
{
	if (buffer->hprintf_index > 0)
	{
		DWORD NumBytes;
		WriteFile(LogFile, buffer->hprintf_buffer, lstrlen(buffer->hprintf_buffer), &NumBytes, 0);
		buffer->hprintf_index = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
// hprintf
void hprintf(HANDLE LogFile, TBuffer *buffer, LPCSTR Format, ...)
{
	if (buffer->hprintf_index > (HPRINTF_BUFFER_SIZE-1024))
	{
		DWORD NumBytes;
		WriteFile(LogFile, buffer->hprintf_buffer, lstrlen( buffer->hprintf_buffer), &NumBytes, 0);
		buffer->hprintf_index = 0;
	}

	va_list arglist;
	va_start( arglist, Format);
	buffer->write( Format, arglist);
	//buffer->hprintf_index += wvsprintf( &buffer->hprintf_buffer[ buffer->hprintf_index], Format, arglist);
	va_end( arglist);

/*	//////////////////////////////////////////////////////////////////////////
	DWORD NumBytes;
	WriteFile(LogFile, buffer->hprintf_buffer, lstrlen( buffer->hprintf_buffer), &NumBytes, 0);
	buffer->hprintf_index = 0;
	//////////////////////////////////////////////////////////////////////////*/
}

///////////////////////////////////////////////////////////////////////////////
// DumpModuleInfo
//
// Print information about a code module (DLL or EXE) such as its size,
// location, time stamp, etc.
bool DumpModuleInfo(HANDLE LogFile, HINSTANCE ModuleHandle, int nModuleNo, TBuffer *buffer)
{
	bool rc = false;
	WCHAR szModName[MAX_PATH_4*2] = {'\0'};
	//ZeroMemory(szModName, sizeof(szModName)*sizeof(szModName[0]));

	__try
	{
		if (GetModuleFileNameW(ModuleHandle, szModName, sizeof(szModName)-2) > 0)
		{
			// If GetModuleFileName returns greater than zero then this must
			// be a valid code module address. Therefore we can try to walk
			// our way through its structures to find the link time stamp.
			IMAGE_DOS_HEADER *DosHeader = (IMAGE_DOS_HEADER*)ModuleHandle;
			if (IMAGE_DOS_SIGNATURE != DosHeader->e_magic)
				return false;

			IMAGE_NT_HEADERS *NTHeader = (IMAGE_NT_HEADERS*)((TCHAR *)DosHeader
				+ DosHeader->e_lfanew);
			if (IMAGE_NT_SIGNATURE != NTHeader->Signature)
				return false;

			// open the code module file so that we can get its file date and size
			HANDLE ModuleFile = CreateFileW(szModName, GENERIC_READ,
				FILE_SHARE_READ, 0, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, 0);

			TCHAR TimeBuffer[100];
			TimeBuffer[0] = _T('\0');

			DWORD FileSize = 0;
			if (ModuleFile != INVALID_HANDLE_VALUE)
			{
				FileSize = GetFileSize(ModuleFile, 0);
				FILETIME LastWriteTime;
				if (GetFileTime(ModuleFile, 0, 0, &LastWriteTime))
				{
					FormatTime(TimeBuffer, LastWriteTime);
				}
				CloseHandle(ModuleFile);
			}
			hprintf(LogFile, buffer, _T("Module %d\r\n"), nModuleNo);
			USES_CONVERSION;
			hprintf(LogFile, buffer, _T("Name: %s\r\n"), W2A(szModName));
			hprintf(LogFile, buffer, _T("Image Base: 0x%08x  Image Size: 0x%08x\r\n"), 
				NTHeader->OptionalHeader.ImageBase, 
				NTHeader->OptionalHeader.SizeOfImage), 

				hprintf(LogFile, buffer, _T("Checksum:   0x%08x  Time Stamp: 0x%08x\r\n"), 
				NTHeader->OptionalHeader.CheckSum,
				NTHeader->FileHeader.TimeDateStamp);

			hprintf(LogFile, buffer, _T("File Size:  %-10d  File Time:  %s\r\n"),
				FileSize, TimeBuffer);

			hprintf(LogFile, buffer, _T("Version Information:\r\n"));

			CMiniVersion ver(szModName);
			TCHAR szBuf[200];
			WORD dwBuf[4];

			ver.GetCompanyName(szBuf, sizeof(szBuf)-1);
			hprintf(LogFile, buffer, _T("   Company:    %s \r\n"), szBuf);

			ver.GetProductName(szBuf, sizeof(szBuf)-1);
			hprintf(LogFile, buffer, _T("   Product:    %s \r\n"), szBuf);

			ver.GetFileDescription(szBuf, sizeof(szBuf)-1);
			hprintf(LogFile, buffer, _T("   FileDesc:   %s \r\n"), szBuf);

			ver.GetFileVersion(dwBuf);
			hprintf(LogFile, buffer, _T("   FileVer:    %d.%d.%d.%d \r\n"), 
				dwBuf[0], dwBuf[1], dwBuf[2], dwBuf[3]);

			ver.GetProductVersion(dwBuf);
			hprintf(LogFile, buffer, _T("   ProdVer:    %d.%d.%d.%d \r\n"), 
				dwBuf[0], dwBuf[1], dwBuf[2], dwBuf[3]);

			ver.Release();

			//hprintf(LogFile, buffer, _T("   Image Version: %u.%u \r\n"), NTHeader->OptionalHeader.MajorImageVersion,
			//	NTHeader->OptionalHeader.MinorImageVersion);
			//hprintf(LogFile, buffer, _T("   Subsystem Version: %u.%u \r\n"), NTHeader->OptionalHeader.MajorSubsystemVersion,
			//	NTHeader->OptionalHeader.MinorSubsystemVersion);
			//hprintf(LogFile, buffer, _T("   Linker Version: %u.%u \r\n"), NTHeader->OptionalHeader.MajorLinkerVersion,
			//													NTHeader->OptionalHeader.MinorLinkerVersion);

			hprintf(LogFile, buffer, _T("---------------\r\n"));

			rc = true;
		}
	}
	// Handle any exceptions by continuing from this point.
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
	return rc;
}

///////////////////////////////////////////////////////////////////////////////
// DumpModuleList
//
// Scan memory looking for code modules (DLLs or EXEs). VirtualQuery is used
// to find all the blocks of address space that were reserved or committed,
// and ShowModuleInfo will display module information if they are code
// modules.
void DumpModuleList(HANDLE LogFile, TBuffer *buffer)
{
	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);

	const size_t PageSize = SystemInfo.dwPageSize;

	// Set NumPages to the number of pages in the 4GByte address space,
	// while being careful to avoid overflowing ints
	const size_t NumPages = 4 * size_t(ONEG / PageSize);
	size_t pageNum = 0;
	void *LastAllocationBase = 0;

	int nModuleNo = 1;

	while (pageNum < NumPages)
	{
		MEMORY_BASIC_INFORMATION MemInfo;
		if (VirtualQuery((void *)(pageNum * PageSize), &MemInfo,
			sizeof(MemInfo)))
		{
			if (MemInfo.RegionSize > 0)
			{
				// Adjust the page number to skip over this block of memory
				pageNum += MemInfo.RegionSize / PageSize;
				if (MemInfo.State == MEM_COMMIT && MemInfo.AllocationBase >
					LastAllocationBase)
				{
					// Look for new blocks of committed memory, and try
					// recording their module names - this will fail
					// gracefully if they aren't code modules
					LastAllocationBase = MemInfo.AllocationBase;
					if (DumpModuleInfo(LogFile, 
						(HINSTANCE)LastAllocationBase, 
						nModuleNo, buffer))
					{
						nModuleNo++;
					}
				}
			}
			else
				pageNum += SIXTYFOURK / PageSize;
		}
		else
			pageNum += SIXTYFOURK / PageSize;

		// If VirtualQuery fails we advance by 64K because that is the
		// granularity of address space doled out by VirtualAlloc()
	}
}


///////////////////////////////////////////////////////////////////////////////
// GetLocalIPAddress
//

#define DESIRED_WINSOCK_VERSION 0x0101
#define MINIMUM_WINSOCK_VERSION 0x0001  // ...but we'll take ver 1.0

struct TIPAddr
{
	TIPAddr()
	{
		ZeroMemory(szIPAddress, sizeof(szIPAddress));
	}
	TCHAR szIPAddress[ 500];
};

BOOL GetLocalIPAddress( TCHAR *sComputerName, TIPAddr *pIPAddress)
{
	int serr;
	WSADATA wsadata;
	serr = WSAStartup(DESIRED_WINSOCK_VERSION, &wsadata);

	if (serr != 0) 
	{
		//::MessageBox(NULL, "Socket initialization error.", "Error", MB_ICONERROR | MB_OK);
		return FALSE; 
	}

	if (wsadata.wVersion < MINIMUM_WINSOCK_VERSION) 
	{
		//::MessageBox(NULL,"The minimum WinSock version - 1.0", "Error", MB_ICONERROR | MB_OK);
		return FALSE; 
	}

	WORD wVersionRequested;
	WSADATA wsaData;
	PHOSTENT hostinfo;	
	wVersionRequested = MAKEWORD( 2, 0 );
	if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{
		TCHAR pIPAddr[50] = {'\0'};// = pIPAddress->szIPAddress;
		hostinfo = gethostbyname(sComputerName);
		if( hostinfo == NULL)
			hostinfo = gethostbyname("");
		if ( hostinfo != NULL)
		{
			int pos = 0;
			for( int i = 0;hostinfo->h_addr_list[i] != NULL;i++)
			{
				strcpy(pIPAddr, inet_ntoa (*(struct in_addr *)hostinfo->h_addr_list[i]));
				WriteLog__("myip==");				
				WriteLog__(pIPAddr);
				WriteLog__("\n");
				if( (hostinfo->h_addr_list[i + 1] != NULL || i > 0) && !strcmp(pIPAddr, "127.0.0.1"))
				{
					continue;
				}
				if( pos > 0)
				{
					memcpy( &pIPAddress->szIPAddress[pos], ", ", 2);
					pos+=2;
				}
				memcpy( &pIPAddress->szIPAddress[pos], pIPAddr, strlen( pIPAddr));
				pos += strlen( pIPAddr);
			}
		}		
		WSACleanup();
		return TRUE;
	}
	//::MessageBox(NULL, "WSAStartup.", "Error", MB_ICONERROR | MB_OK);
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// DumpSystemInformation
//
// Record information about the user's system, such as processor type, amount
// of memory, etc.
void DumpSystemInformation(HANDLE LogFile, HANDLE hProcess, TBuffer *buffer)
{
	FILETIME CurrentTime;
	GetSystemTimeAsFileTime(&CurrentTime);
	TCHAR szTimeBuffer[100];
	FormatTime(szTimeBuffer, CurrentTime);

	hprintf( LogFile, buffer, _T("Error occurred at %s.\r\n"), szTimeBuffer);

	WCHAR szModuleName[MAX_PATH_4*2]={L'\0'};
	//ZeroMemory(szModuleName, sizeof(szModuleName)*sizeof(szModuleName[0]));
	if ( GetModuleFileNameW(0, szModuleName, _countof(szModuleName)-2) <= 0)
		lstrcpyW(szModuleName, L"Unknown");

	TCHAR szUserName[200];
	TCHAR szComputerName[200];
	TIPAddr IPAddress;
	ZeroMemory(szUserName, sizeof(szUserName));
	ZeroMemory(szComputerName, sizeof(szComputerName));	
	DWORD UserNameSize = _countof(szUserName)-2;
	if ( !GetUserName(szUserName, &UserNameSize))
		lstrcpy(szUserName,  "Unknown");

	DWORD ComputerNameSize = _countof(szComputerName)-2;
	if ( !GetComputerName( szComputerName, &ComputerNameSize))
		lstrcpy(szComputerName,  "Unknown");	

	USES_CONVERSION;
	hprintf( LogFile, buffer, _T("%s, run by \"%s\" on computer %s.\r\n"), W2A(szModuleName), szUserName, szComputerName);

	if ( GetLocalIPAddress( szComputerName, &IPAddress))
	{
		hprintf( LogFile, buffer, _T("IP address of computer %s;\r\n"), IPAddress.szIPAddress);
	}

	//BOOL b = GetLocalIPAddress( szComputerName, szIPAddress);
	//hprintf( LogFile, buffer, _T("IP address of computer %s %i.\r\n"), szIPAddress, b);
	

	// print out operating system
	TCHAR szWinVer[ 50], szMajorMinorBuild[ 50], szMajorMinorServicePack[ 50];
	int nWinVer;
	GetWinVer(szWinVer, &nWinVer, szMajorMinorBuild, szMajorMinorServicePack);
	hprintf(LogFile, buffer, _T("Operating system:  %s SP=%s (%s).\r\n"), 
		szWinVer, szMajorMinorServicePack, szMajorMinorBuild);

	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);
	hprintf(LogFile, buffer, _T("%d processor(s), type %d.\r\n"),
		SystemInfo.dwNumberOfProcessors, SystemInfo.dwProcessorType);

	MEMORYSTATUS MemInfo;
	MemInfo.dwLength = sizeof(MemInfo);
	GlobalMemoryStatus(&MemInfo);

	// Print out info on memory, rounded up.
	hprintf(LogFile, buffer, _T("%d%% memory in use.\r\n"), MemInfo.dwMemoryLoad);
	hprintf(LogFile, buffer, _T("%d MBytes physical memory.\r\n"), (MemInfo.dwTotalPhys +
		ONEM - 1) / ONEM);
	hprintf(LogFile, buffer, _T("%d MBytes physical memory free.\r\n"), 
		(MemInfo.dwAvailPhys + ONEM - 1) / ONEM);
	hprintf(LogFile, buffer, _T("%d MBytes paging file.\r\n"), (MemInfo.dwTotalPageFile +
		ONEM - 1) / ONEM);
	hprintf(LogFile, buffer, _T("%d MBytes paging file free.\r\n"), 
		(MemInfo.dwAvailPageFile + ONEM - 1) / ONEM);
	hprintf(LogFile, buffer, _T("%d MBytes user address space.\r\n"), 
		(MemInfo.dwTotalVirtual + ONEM - 1) / ONEM);
	hprintf(LogFile, buffer, _T("%d MBytes user address space free.\r\n"), 
		(MemInfo.dwAvailVirtual + ONEM - 1) / ONEM);
	hprintf(LogFile, buffer, _T(" \r\n"));

	PROCESS_MEMORY_COUNTERS memCounters;
	if( hProcess != NULL && GetProcessMemoryInfo( hProcess, &memCounters, sizeof(memCounters)))
	{
		hprintf( LogFile, buffer, "Memory of vAcademia: \r\n");
		hprintf( LogFile, buffer, "\tPageFaultCount: %d\r\n", memCounters.PageFaultCount );
		hprintf( LogFile, buffer, "\tPeakWorkingSetSize: %d\r\n", memCounters.PeakWorkingSetSize );
		hprintf( LogFile, buffer, "\tWorkingSetSize: %d\r\n", memCounters.WorkingSetSize );
		hprintf( LogFile, buffer, "\tQuotaPeakPagedPoolUsage: %d\r\n", memCounters.QuotaPeakPagedPoolUsage );
		hprintf( LogFile, buffer, "\tQuotaPagedPoolUsage: %d\r\n", memCounters.QuotaPagedPoolUsage );
		hprintf( LogFile, buffer, "\tQuotaPeakNonPagedPoolUsage: %d\r\n", memCounters.QuotaPeakNonPagedPoolUsage );
		hprintf( LogFile, buffer, "\tQuotaNonPagedPoolUsage: %d\r\n", memCounters.QuotaNonPagedPoolUsage );
		hprintf( LogFile, buffer, "\tPagefileUsage: %d\r\n", memCounters.PagefileUsage ); 
		hprintf( LogFile, buffer, "\tPeakPagefileUsage: %d\r\n", memCounters.PeakPagefileUsage );
		hprintf( LogFile, buffer, "\tTotal memory consuming: %d\r\n", (memCounters.WorkingSetSize+memCounters.QuotaPagedPoolUsage+memCounters.QuotaNonPagedPoolUsage));
	}	
}

///////////////////////////////////////////////////////////////////////////////
// GetExceptionDescription
//
// Translate the exception code into something human readable
const TCHAR *GetExceptionDescription(DWORD ExceptionCode)
{
	struct ExceptionNames
	{
		DWORD	ExceptionCode;
		TCHAR *	ExceptionName;
	};

#if 0  // from winnt.h
#define STATUS_WAIT_0                    ((DWORD   )0x00000000L)    
#define STATUS_ABANDONED_WAIT_0          ((DWORD   )0x00000080L)    
#define STATUS_USER_APC                  ((DWORD   )0x000000C0L)    
#define STATUS_TIMEOUT                   ((DWORD   )0x00000102L)    
#define STATUS_PENDING                   ((DWORD   )0x00000103L)    
#define STATUS_SEGMENT_NOTIFICATION      ((DWORD   )0x40000005L)    
#define STATUS_GUARD_PAGE_VIOLATION      ((DWORD   )0x80000001L)    
#define STATUS_DATATYPE_MISALIGNMENT     ((DWORD   )0x80000002L)    
#define STATUS_BREAKPOINT                ((DWORD   )0x80000003L)    
#define STATUS_SINGLE_STEP               ((DWORD   )0x80000004L)    
#define STATUS_ACCESS_VIOLATION          ((DWORD   )0xC0000005L)    
#define STATUS_IN_PAGE_ERROR             ((DWORD   )0xC0000006L)    
#define STATUS_INVALID_HANDLE            ((DWORD   )0xC0000008L)    
#define STATUS_NO_MEMORY                 ((DWORD   )0xC0000017L)    
#define STATUS_ILLEGAL_INSTRUCTION       ((DWORD   )0xC000001DL)    
#define STATUS_NONCONTINUABLE_EXCEPTION  ((DWORD   )0xC0000025L)    
#define STATUS_INVALID_DISPOSITION       ((DWORD   )0xC0000026L)    
#define STATUS_ARRAY_BOUNDS_EXCEEDED     ((DWORD   )0xC000008CL)    
#define STATUS_FLOAT_DENORMAL_OPERAND    ((DWORD   )0xC000008DL)    
#define STATUS_FLOAT_DIVIDE_BY_ZERO      ((DWORD   )0xC000008EL)    
#define STATUS_FLOAT_INEXACT_RESULT      ((DWORD   )0xC000008FL)    
#define STATUS_FLOAT_INVALID_OPERATION   ((DWORD   )0xC0000090L)    
#define STATUS_FLOAT_OVERFLOW            ((DWORD   )0xC0000091L)    
#define STATUS_FLOAT_STACK_CHECK         ((DWORD   )0xC0000092L)    
#define STATUS_FLOAT_UNDERFLOW           ((DWORD   )0xC0000093L)    
#define STATUS_INTEGER_DIVIDE_BY_ZERO    ((DWORD   )0xC0000094L)    
#define STATUS_INTEGER_OVERFLOW          ((DWORD   )0xC0000095L)    
#define STATUS_PRIVILEGED_INSTRUCTION    ((DWORD   )0xC0000096L)    
#define STATUS_STACK_OVERFLOW            ((DWORD   )0xC00000FDL)    
#define STATUS_CONTROL_C_EXIT            ((DWORD   )0xC000013AL)    
#define STATUS_FLOAT_MULTIPLE_FAULTS     ((DWORD   )0xC00002B4L)    
#define STATUS_FLOAT_MULTIPLE_TRAPS      ((DWORD   )0xC00002B5L)    
#define STATUS_ILLEGAL_VLM_REFERENCE     ((DWORD   )0xC00002C0L)     
#endif

	ExceptionNames ExceptionMap[] =
	{
		{0x40010005, _T("a Control-C")},
		{0x40010008, _T("a Control-Break")},
		{0x80000002, _T("a Datatype Misalignment")},
		{0x80000003, _T("a Breakpoint")},
		{0xc0000005, _T("an Access Violation")},
		{0xc0000006, _T("an In Page Error")},
		{0xc0000017, _T("a No Memory")},
		{0xc000001d, _T("an Illegal Instruction")},
		{0xc0000025, _T("a Noncontinuable Exception")},
		{0xc0000026, _T("an Invalid Disposition")},
		{0xc000008c, _T("a Array Bounds Exceeded")},
		{0xc000008d, _T("a Float Denormal Operand")},
		{0xc000008e, _T("a Float Divide by Zero")},
		{0xc000008f, _T("a Float Inexact Result")},
		{0xc0000090, _T("a Float Invalid Operation")},
		{0xc0000091, _T("a Float Overflow")},
		{0xc0000092, _T("a Float Stack Check")},
		{0xc0000093, _T("a Float Underflow")},
		{0xc0000094, _T("an Integer Divide by Zero")},
		{0xc0000095, _T("an Integer Overflow")},
		{0xc0000096, _T("a Privileged Instruction")},
		{0xc00000fD, _T("a Stack Overflow")},
		{0xc0000142, _T("a DLL Initialization Failed")},
		{0xe06d7363, _T("a Microsoft C++ Exception")},
	};

	for (int i = 0; i < sizeof(ExceptionMap) / sizeof(ExceptionMap[0]); i++)
		if (ExceptionCode == ExceptionMap[i].ExceptionCode)
			return ExceptionMap[i].ExceptionName;

	return _T("an Unknown exception type");
}

///////////////////////////////////////////////////////////////////////////////
// DumpStack
void DumpStack(HANDLE LogFile, DWORD *pStack, TBuffer *buffer)
{
	hprintf(LogFile, buffer, _T("\r\nStack:\r\n"));

	__try
	{
		// Esp contains the bottom of the stack, or at least the bottom of
		// the currently used area.
		DWORD* pStackTop;

		__asm
		{
			// Load the top (highest address) of the stack from the
			// thread information block. It will be found there in
			// Win9x and Windows NT.
			mov	eax, fs:[4]
			mov pStackTop, eax
		}

		if (pStackTop > pStack + buffer->MaxStackDump)
			pStackTop = pStack + buffer->MaxStackDump;

		int Count = 0;

		DWORD* pStackStart = pStack;

		int nDwordsPrinted = 0;

		while (pStack + 1 <= pStackTop)
		{
			if ((Count % buffer->StackColumns) == 0)
			{
				pStackStart = pStack;
				nDwordsPrinted = 0;
				hprintf(LogFile, buffer, _T("0x%08x: "), pStack);
			}

			if ((++Count % buffer->StackColumns) == 0 || pStack + 2 > pStackTop)
			{
				hprintf(LogFile, buffer, _T("%08x "), *pStack);
				nDwordsPrinted++;

				int n = nDwordsPrinted;
				while (n < 4)
				{
					hprintf(LogFile, buffer, _T("         "));
					n++;
				}

				for (int i = 0; i < nDwordsPrinted; i++)
				{
					DWORD dwStack = *pStackStart;
					for (int j = 0; j < 4; j++)
					{
						char c = (char)(dwStack & 0xFF);
						if (c < 0x20 || c > 0x7E)
							c = '.';
#ifdef _UNICODE
						WCHAR w = (WCHAR)c;
						hprintf(LogFile, buffer, _T("%c"), w);
#else
						hprintf(LogFile, buffer, _T("%c"), c);
#endif
						dwStack = dwStack >> 8;
					}
					pStackStart++;
				}

				hprintf(LogFile, buffer, _T("---------------\r\n"));
			}
			else
			{
				hprintf(LogFile, buffer, _T("%08x "), *pStack);
				nDwordsPrinted++;
			}
			pStack++;
		}
		hprintf(LogFile, buffer, _T("---------------\r\n"));
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		hprintf(LogFile, buffer, _T("Exception encountered during stack dump.\r\n"));
	}
}

///////////////////////////////////////////////////////////////////////////////
// DumpRegisters
void DumpRegisters(HANDLE LogFile, PCONTEXT Context, TBuffer *buffer)
{
	// Print out the register values in an XP error window compatible format.
	//hprintf(LogFile, buffer, _T("---------------\r\n"));
	hprintf(LogFile, buffer, _T("Context:\r\n"));
	hprintf(LogFile, buffer, _T("EDI:    0x%08x  ESI: 0x%08x  EAX:   0x%08x\r\n"),
		Context->Edi, Context->Esi, Context->Eax);
	hprintf(LogFile, buffer, _T("EBX:    0x%08x  ECX: 0x%08x  EDX:   0x%08x\r\n"),
		Context->Ebx, Context->Ecx, Context->Edx);
	hprintf(LogFile, buffer, _T("EIP:    0x%08x  EBP: 0x%08x  SegCs: 0x%08x\r\n"),
		Context->Eip, Context->Ebp, Context->SegCs);
	hprintf(LogFile, buffer, _T("EFlags: 0x%08x  ESP: 0x%08x  SegSs: 0x%08x\r\n"),
		Context->EFlags, Context->Esp, Context->SegSs);
}

LONG SendReport( TCHAR *appName, WCHAR *pszFilePart, WCHAR *szModuleName, FILE *fp__, TCHAR *pCrashType)
{
	///////////////////////////////////////////////////////////////////////
	//
	//  pop up our crash report app
	//
	///////////////////////////////////////////////////////////////////////
	OpenLog__( &fp__, NULL, "mdump");
	if ( pszFilePart == NULL)
		pszFilePart = GetFilePart( szModuleName);
	
	// Replace the filename with our crash report exe file name
	lstrcpyW(pszFilePart, XCRASHREPORT_CRASH_REPORT_APP);

	WCHAR szArgsLine[MAX_PATH_4];
	WCHAR szCommandLine[MAX_PATH_4 * 2];
	lstrcpyW(szCommandLine, szModuleName);

	lstrcpyW(szArgsLine, L" -app_name ");
	lstrcatW(szArgsLine, L"\"");	
	USES_CONVERSION;
	lstrcatW(szArgsLine, A2W(appName));
	lstrcatW(szArgsLine, L"\"");

	lstrcatW(szArgsLine, L" -crash_type ");	
	lstrcatW(szArgsLine, L"\"");
	lstrcatW(szArgsLine, A2W(pCrashType));
	lstrcatW(szArgsLine, L"\"");

	if ( g_bAppFinishing || !g_bAllowRestartApp)
	{
		lstrcatW(szArgsLine, L" -allow_restart_app 0");		
	}
	else
	{
		if ( strlen( g_userName) > 2 && strlen( g_password) > 2)
		{
			lstrcatW(szArgsLine, L" -user ");			
			lstrcatW(szArgsLine, A2W(g_userName));			

			lstrcatW(szArgsLine, L" -password ");
			lstrcatW(szArgsLine, L"\"");
			lstrcatW(szArgsLine, A2W(g_password));
			lstrcatW(szArgsLine, L"\"");

			if ( g_iEncodingPassword > 0)
			{
				lstrcatW(szArgsLine, L" -encoding ");				
				TCHAR sz[MAX_PATH];
				sprintf( sz, "%d", g_iEncodingPassword);
				lstrcatW(szArgsLine, A2W(sz));
			}
		}
	}

	if ( wcslen( g_root) > 2)
	{
		lstrcatW(szArgsLine, L" -root ");
		lstrcatW(szArgsLine, L"\"");
		lstrcatW(szArgsLine, g_root);
		lstrcatW(szArgsLine, L"\"");
	}

	lstrcatW(szArgsLine, L" -send_crash_show ");
	TCHAR sz[MAX_PATH];
	//sprintf( sz, "%d", g_bSendCrashShow ? 1 : 0);	
	//lstrcatW(szArgsLine, A2W(sz));
	lstrcatW(szArgsLine, L"0");

	lstrcatW(szCommandLine, 	szArgsLine);

	WriteLog__( fp__, "appName = ");
	WriteLog__( fp__, appName);
	WriteLog__( fp__, "\n");

	WriteLog__( fp__, "szCommandLine = ");
	WriteLog__( fp__, W2A(szCommandLine));
	WriteLog__( fp__, "\n");
	CloseLog__( &fp__);
	OpenLog__( &fp__, NULL, "mdump");

	STARTUPINFOW si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;
	si.wShowWindow = g_bSendCrashShow ? SW_SHOW : SW_HIDE;

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	HANDLE	hCrashReport;
	if (CreateProcessW(
		NULL,					// name of executable module
		szCommandLine,			// command line string
		NULL,					// process attributes
		NULL,					// thread attributes
		FALSE,					// handle inheritance option
		0,						// creation flags CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS
		NULL,					// new environment block
		NULL,					// current directory name
		&si,					// startup information
		&pi))					// process information
	{
		if ( strcmpi(  pCrashType, APP_ERROR) == 0 && strlen( g_launcherName) > 2)
		{
			// уведомл€ем приложение vacademia.exe, что приложение player.exe упало
			HideLauncher( g_launcherName);
		}
		// VacademiaReport.exe was successfully started, so
		// suppress the standard crash dialog
		WaitForSingleObject(pi.hProcess, INFINITE);
		WriteLog__( fp__, "VacademiaReport is started!\n");
		CloseLog__( &fp__);
		return EXCEPTION_EXECUTE_HANDLER;
	}
	else
	{
		// CrashReport.exe was not started - let
		// the standard crash dialog appear			
		WriteLog__( fp__, "g_root==");	
		WriteLog__( fp__, g_root);
		WriteLog__( fp__, "g_root.length > 6 ==");
		WriteLog__( fp__,  (wcslen( g_root) > 6 ? "true" : "false"));
		if ( wcslen( g_root) > 6)
		{			
			lstrcpyW(szCommandLine, g_root);
			lstrcatW(szCommandLine, L"\\");
			lstrcatW(szCommandLine, XCRASHREPORT_CRASH_REPORT_APP);			
			lstrcatW(szCommandLine, szArgsLine);

			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_SHOW;
			ZeroMemory(&pi, sizeof(pi));

			if (CreateProcessW(
				NULL,					// name of executable module
				szCommandLine,			// command line string
				NULL,					// process attributes
				NULL,					// thread attributes
				FALSE,					// handle inheritance option
				0,						// creation flags CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS
				NULL,					// new environment block
				NULL,					// current directory name
				&si,					// startup information
				&pi))					// process information
			{
				if ( strcmpi(  pCrashType, APP_ERROR) == 0 && strlen( g_launcherName) > 2)
				{
					// уведомл€ем приложение vacademia.exe, что приложение player.exe упало
					HideLauncher( g_launcherName);
				}
				WriteLog__( fp__, "VacademiaReport is started!\n");
				CloseLog__( &fp__);
				// CrashReport.exe was successfully started, so
				// suppress the standard crash dialog
				WaitForSingleObject(pi.hProcess, INFINITE);				
				return EXCEPTION_EXECUTE_HANDLER;
			}
			WriteLog__( fp__, "VacademiaReport is not started!\n");
			CloseLog__( &fp__);
			return EXCEPTION_CONTINUE_SEARCH;
		}

		WriteLog__( fp__, "VacademiaReport is not started!\n");
		CloseLog__( &fp__);
		return EXCEPTION_CONTINUE_SEARCH;
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

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
BOOL WINAPI Get_Module_By_Ret_Addr(PBYTE Ret_Addr, PWCHAR Module_Name, BYTE * Module_Addr)
//****************************************************************************************
// Find module by Ret_Addr (address in the module).
// Return Module_Name (full path) and Module_Addr (start address).
// Return TRUE if found.
{
	HMODULE hKernel32 = GetModuleHandle("KERNEL32");
	CreateToolhelp32Snapshot_ = (CREATE_TOOL_HELP32_SNAPSHOT)GetProcAddress(hKernel32, "CreateToolhelp32SnapshotW");
	Module32First_ = (MODULE32_FIRST)GetProcAddress(hKernel32, "Module32FirstW");
	Module32Next_ = (MODULE32_NEST)GetProcAddress(hKernel32, "Module32NextW");

	MODULEENTRY32W	M = {sizeof(M)};
	HANDLE	hSnapshot;

	Module_Name[0] = L'\0';

	if (CreateToolhelp32Snapshot_)
	{
		hSnapshot = CreateToolhelp32Snapshot_(TH32CS_SNAPMODULE, 0);

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

//******************************************************************
int WINAPI Get_Call_Stack(PEXCEPTION_POINTERS pException, PCHAR Str, FILE *fp__)
//******************************************************************
// Fill Str with call stack info.
// pException can be either GetExceptionInformation() or NULL.
// If pException = NULL - get current call stack.
{
	WCHAR	Module_Name[MAX_PATH];
	PBYTE	Module_Addr = NULL;
	BYTE	Module_Addr_1[MAX_PATH];
	int		Str_Len;

	typedef struct STACK
	{
		STACK *	Ebp;
		PBYTE	Ret_Addr;
		DWORD	Param[0];
	} STACK, * PSTACK;

	STACK	Stack = {0, 0};
	PSTACK	Ebp;

	WriteLog__( fp__, "Get_Call_Stack Start\n");
	CloseLog__( &fp__);
	OpenLog__( &fp__, NULL, "mdump");

	if (pException)		//fake frame for exception address
	{
		Stack.Ebp = (PSTACK)pException->ContextRecord->Ebp;
		Stack.Ret_Addr = (PBYTE)pException->ExceptionRecord->ExceptionAddress;
		Ebp = &Stack;
	}
	else
	{
		Ebp = (PSTACK)&pException - 1;	//frame addr of Get_Call_Stack()

		// Skip frame of Get_Call_Stack().
		if (!IsBadReadPtr(Ebp, sizeof(PSTACK)))
			Ebp = Ebp->Ebp;		//caller ebp
	}

	WriteLog__( fp__, "Get_Call_Stack\n");
	CloseLog__( &fp__);
	OpenLog__( &fp__, NULL, "mdump");

	Str[0] = '\0';
	Str_Len = 0;

	// Trace CALL_TRACE_MAX calls maximum - not to exceed DUMP_SIZE_MAX.
	// Break trace on wrong stack frame.
	for (int Ret_Addr_I = 0;
		(Ret_Addr_I < CALL_TRACE_MAX) && !IsBadReadPtr(Ebp, sizeof(PSTACK)) && !IsBadCodePtr(FARPROC(Ebp->Ret_Addr));
		Ret_Addr_I++, Ebp = Ebp->Ebp)
	{
		// If module with Ebp->Ret_Addr found.
		if (Get_Module_By_Ret_Addr(Ebp->Ret_Addr, Module_Name, Module_Addr_1))
		{
			if (Module_Addr_1 != Module_Addr)	//new module
			{
				// Save module's address and full path.
				Module_Addr = Module_Addr_1;
				USES_CONVERSION;
				Str_Len += wsprintf(Str + Str_Len, NL "%08X  %s", Module_Addr, W2A(Module_Name));
			}

			// Save call offset.
			Str_Len += wsprintf(Str + Str_Len,
				NL "  +%08X", Ebp->Ret_Addr - Module_Addr);

			// Save 5 params of the call. We don't know the real number of params.
			if (pException && !Ret_Addr_I)	//fake frame for exception address
				Str_Len += wsprintf(Str + Str_Len, "  Exception Offset");
			else if (!IsBadReadPtr(Ebp, sizeof(PSTACK) + 5 * sizeof(DWORD)))
			{
				Str_Len += wsprintf(Str + Str_Len, "  (%X, %X, %X, %X, %X)",
					Ebp->Param[0], Ebp->Param[1], Ebp->Param[2], Ebp->Param[3], Ebp->Param[4]);
			}
		}
		else
			Str_Len += wsprintf(Str + Str_Len, NL "%08X", Ebp->Ret_Addr);
	}

	return Str_Len;
} //Get_Call_Stack

//***********************************
int WINAPI Get_Version_Str(PCHAR Str)
//***********************************
// Fill Str with Windows version.
{
	OSVERSIONINFOEX	V = {sizeof(OSVERSIONINFOEX)};	//EX for NT 5.0 and later

	if (!GetVersionEx((POSVERSIONINFO)&V))
	{
		ZeroMemory(&V, sizeof(V));
		V.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((POSVERSIONINFO)&V);
	}

	if (V.dwPlatformId != VER_PLATFORM_WIN32_NT)
		V.dwBuildNumber = LOWORD(V.dwBuildNumber);	//for 9x HIWORD(dwBuildNumber) = 0x04xx

	//	return wsprintf(Str,
	//		NL "Windows:  %d.%d.%d, SP %d.%d, Product Type %d",	//SP - service pack, Product Type - VER_NT_WORKSTATION,...
	//		V.dwMajorVersion, V.dwMinorVersion, V.dwBuildNumber, V.wServicePackMajor, V.wServicePackMinor, V.wProductType);
	return wsprintf(Str,
		NL "Windows:  %d.%d.%d, SP %d.%d",	//SP - service pack, Product Type - VER_NT_WORKSTATION,...
		V.dwMajorVersion, V.dwMinorVersion, V.dwBuildNumber, V.wServicePackMajor, V.wServicePackMinor);
} //Get_Version_Str

//*************************************************************
PCHAR WINAPI Get_Exception_Info(PEXCEPTION_POINTERS pException, FILE *fp__, int &flag)
//*************************************************************
// Allocate Str[DUMP_SIZE_MAX] and return Str with dump, if !pException - just return call stack in Str.
{
	CHAR		Str[ 2 * DUMP_SIZE_MAX];
	int			Str_Len;
	int			i;
	WCHAR		Module_Name[MAX_PATH];
	BYTE		Module_Addr[MAX_PATH];
	HANDLE		hFile;
	FILETIME	Last_Write_Time;
	FILETIME	Local_File_Time;
	SYSTEMTIME	T;	

	Str_Len = 0;
	//Str_Len += Get_Version_Str(Str + Str_Len);

	Str_Len += wsprintf(Str + Str_Len, NL "Process:  ");
	GetModuleFileName(NULL, Str + Str_Len, MAX_PATH);
	Str_Len = lstrlen(Str);
	Str_Len += wsprintf(Str + Str_Len, ";");	
	WriteLog__( fp__, "Get_Exception_Info start\n");

	// If exception occurred.
	if (pException)
	{
		EXCEPTION_RECORD &	E = *pException->ExceptionRecord;
		CONTEXT &			C = *pException->ContextRecord;

		// If module with E.ExceptionAddress found - save its path and date.
		if (Get_Module_By_Ret_Addr((PBYTE)E.ExceptionAddress, Module_Name, Module_Addr))
		{
			USES_CONVERSION;
			Str_Len += wsprintf(Str + Str_Len,
				NL "Module:  %s;", W2A(Module_Name));

			if ( skipExceptionForModule( Module_Name))
			{
				flag == 1;
				return Str;
			}

			if ((hFile = CreateFileW(Module_Name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE)
			{
				if (GetFileTime(hFile, NULL, NULL, &Last_Write_Time))
				{
					FileTimeToLocalFileTime(&Last_Write_Time, &Local_File_Time);
					FileTimeToSystemTime(&Local_File_Time, &T);

					Str_Len += wsprintf(Str + Str_Len,
						NL "Date Modified:  %02d/%02d/%d",
						T.wMonth, T.wDay, T.wYear);
				}
				CloseHandle(hFile);
			}
		}
		else
		{
			Str_Len += wsprintf(Str + Str_Len,
				NL "Exception Addr:  %08X", E.ExceptionAddress);
		}		

		Str_Len += wsprintf(Str + Str_Len,
			NL "Exception Code:  %08X", E.ExceptionCode);

		if (E.ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
		{
			// Access violation type - Write/Read.
			Str_Len += wsprintf(Str + Str_Len,
				NL "%s Address:  %08X",
				(E.ExceptionInformation[0]) ? "Write" : "Read", E.ExceptionInformation[1]);
		}

		WriteLog__( fp__, "Get_Exception_Info\n");
		CloseLog__( &fp__);
		OpenLog__( &fp__, NULL, "mdump");

		// Save instruction that caused exception.
		//Str_Len += wsprintf(Str + Str_Len, NL "Instruction: ");
		//for (i = 0; i < 16; i++)
		//	Str_Len += wsprintf(Str + Str_Len, " %02X", PBYTE(E.ExceptionAddress)[i]);

		/*// Save registers at exception.
		Str_Len += wsprintf(Str + Str_Len, NL "Registers:");
		Str_Len += wsprintf(Str + Str_Len, NL "EAX: %08X  EBX: %08X  ECX: %08X  EDX: %08X", C.Eax, C.Ebx, C.Ecx, C.Edx);
		Str_Len += wsprintf(Str + Str_Len, NL "ESI: %08X  EDI: %08X  ESP: %08X  EBP: %08X", C.Esi, C.Edi, C.Esp, C.Ebp);
		Str_Len += wsprintf(Str + Str_Len, NL "EIP: %08X  EFlags: %08X", C.Eip, C.EFlags);*/
	} //if (pException)

	//WriteLog__( fp__, "Get_Exception_Info 2\n");
	//CloseLog__( &fp__);
	//OpenLog__( &fp__, NULL, "mdump");

	// Save call stack info.
	Str_Len += wsprintf(Str + Str_Len, NL "Call Stack:");	
	Get_Call_Stack(pException, Str + Str_Len, fp__);

	if (Str[0] == NL[0])
		lstrcpy(Str, Str + sizeof(NL) - 1);	

	return Str;
} //Get_Exception_Info

//#pragma optimize("", on)

void restartModule()
{
	/*if ( gpWS3D != NULL)
	{
		gpWS3D->DestroyVoipModule();
		gpWS3D->CreateVoipModule();
	}*/
}

//////////////////////////////////////////////////////////////////////////

TCHAR appName[ MAX_PATH];
WCHAR szModuleName[MAX_PATH_4 * 2] = {L'\0'};
FILE *g_fp = NULL;

bool _make_dump( WCHAR *sRoot, HANDLE hProcessHandle, BOOL bSendReport, int iCrashType, BOOL bRepeat /* = FALSE */)
{
	WCHAR *pszFilePart = NULL;	
	FILE *fp__ = g_fp;

	bool retval = false;
	TBuffer buffer;

	HMODULE hDll = NULL;
	char szDbgHelpPath[ _MAX_PATH];

	WCHAR szLauncher[ _MAX_PATH];

	TCHAR sMumpLog[ MAX_PATH];

	DeleteLog__( NULL, "mdump");

	if ( bRepeat)
		_tcscpy( sMumpLog, "mdump_repeat");
	else
		_tcscpy( sMumpLog, "mdump");
	
	OpenLog__( &fp__, NULL, sMumpLog);
	WriteLog__( fp__, "Start\n");
	SYSTEMTIME time;
	TCHAR sTime[ MAX_PATH];
	GetSystemTime( &time);	
	sprintf( sTime, "system time = %d.%d.%d-%d:%d:%d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);	
	WriteLog__( fp__, sTime);
	GetLocalTime( &time);
	sprintf( sTime, "local time = %d.%d.%d-%d:%d:%d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);	
	WriteLog__( fp__, sTime);
	// Create a filename to record the error information to.
	// Storing it in the executable directory works well.
	WriteLog__( fp__, "Start2\n");	

	ZeroMemory(appName, sizeof(appName));
	//ZeroMemory(szModuleName, sizeof(szModuleName));
	if (GetModuleFileNameW(0, szModuleName, _countof(szModuleName)-2) <= 0)
		lstrcpyW(szModuleName, L"Unknown");		

	pszFilePart = GetFilePart(szModuleName);
	wcscpy( szLauncher, pszFilePart);

	HMODULE hMod;
	DWORD cbNeeded;

	wchar_t szProcessName[MAX_PATH] = L"unknown";

	if ( EnumProcessModules( hProcessHandle, &hMod, sizeof(hMod), 
		&cbNeeded) )
	{
		GetModuleBaseNameW( hProcessHandle, hMod, szProcessName, 
			sizeof(szProcessName) );
		lstrcpyW(pszFilePart, szProcessName);
	}
	else
	{
		lstrcpyW(pszFilePart, L"player.exe");
	}
	USES_CONVERSION;
	strcpy( appName, W2A(pszFilePart));
	TCHAR m_szAppName[ MAX_PATH];
	strcpy( m_szAppName, appName);
	TCHAR *lastperiod = lstrrchr(m_szAppName, _T('.'));
	if (lastperiod)
		lastperiod[0] = 0;

	WriteLog__( fp__, m_szAppName);
	WriteLog__( fp__, "\n");

	// Extract the file name portion and remove it's file extension
	TCHAR szFileName[MAX_PATH_4*2];
	lstrcpy(szFileName, W2A(pszFilePart));
	//strcpy(szFileName, "player.exe");

	WCHAR path[ MAX_PATH_4];
	getApplicationDataDirectory( path);
	WCHAR szModuleInfoName[ MAX_PATH_4];
	wcscpy( szModuleInfoName, path);
	wcscat( szModuleInfoName, L"\\");
	WCHAR *szFileInfoName = new WCHAR[ MAX_PATH_4];
	wcscpy( szFileInfoName,	(iCrashType == ASSERT_ERROR_ID || iCrashType == ASSERT_MAKE_USER_ID) ? XCRASHREPORT_ERROR_ASSERT_LOG_FILE : ( iCrashType == APP_HANG_ID ? XCRASHREPORT_ERROR_HANG_LOG_FILE : XCRASHREPORT_ERROR_LOG_FILE));
	wcscat( szModuleInfoName, szFileInfoName);

	WriteLog__( fp__, "CreateFile Start\n");

	HANDLE hLogFile = CreateFileW( szModuleInfoName, GENERIC_WRITE, 0, 0,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, 0);

	if (hLogFile == INVALID_HANDLE_VALUE)
	{		
		WriteLog__( fp__, "_make_dump::Error creating exception report\r\n");
		CloseLog__( &fp__);
		return false;
	}

	// Append to the error log
	SetFilePointer(hLogFile, 0, 0, FILE_END);

	WCHAR szCrashModulePathName[MAX_PATH_4*2] = {L'\0'};
	//ZeroMemory(szCrashModulePathName, sizeof(szCrashModulePathName));
	WCHAR *pszCrashModuleFileName = L"Unknown";

	lstrcpyW(szCrashModulePathName, szModuleName);

	pszCrashModuleFileName = GetFilePart(szCrashModulePathName);

	// Print out the beginning of the error log in a Win95 error window
	// compatible format.

	TCHAR *errorType = new TCHAR[ MAX_PATH];
	TCHAR sCrashType[ MAX_PATH];
	if ( iCrashType == APP_HANG_ID)
	{
		strcpy( sCrashType, APP_HANG);
		strcpy( errorType, "noerror");
	}
	else if ( iCrashType == ASSERT_ERROR_ID)
	{
		strcpy( sCrashType, ASSERT_ERROR);
		strcpy( errorType, "assert");
	}
	else if ( iCrashType == ASSERT_MAKE_USER_ID)
	{
		strcpy( sCrashType, ASSERT_ERROR);
		strcpy( errorType, ASSERT_MAKE_USER);
		iCrashType = ASSERT_ERROR_ID;
	}
	else if ( iCrashType == APP_LAUNCHER_ERROR_ID)
	{
		strcpy( sCrashType, APP_LAUNCHER_ERROR);
		strcpy( errorType, "Access Violation");
	}
	else 
	{
		strcpy( sCrashType, APP_ERROR);
		strcpy( errorType, "Access Violation");
	}

	hprintf(hLogFile, &buffer, _T("%s caused an %s (0x%08x) \r\nin module %s at %04x:%08x.\r\n"),
		szFileName, errorType,//GetExceptionDescription(Exception->ExceptionCode),
		0x00000000,
		W2A(szLauncher), 0, 0);

	CloseLog__( &fp__);	
	OpenLog__( &fp__, NULL, sMumpLog);

	TCHAR *sUUID = new TCHAR[ MAX_PATH];
	TCHAR *sparams = new TCHAR[ 3 * MAX_PATH];
	if ( getUUID( &sUUID))
	{
		hprintf(hLogFile, &buffer, _T("UserNameInAcademia=\"%s\" SessionID=\"%ld\" UUID=\"%s\".\r\n"), g_userName, g_iSessionID, sUUID);
		sprintf( sparams, "UserNameInAcademia=\"%s\" SessionID=\"%ld\" UUID=\"%s\".\r\n", g_userName, g_iSessionID, sUUID);
		WriteLog__( fp__, sparams);	
	}
	else
	{
		hprintf(hLogFile, &buffer, _T("UserNameInAcademia=\"%s\" SessionID=\"%ld\".\r\n"), g_userName, g_iSessionID);
		sprintf( sparams, "UserNameInAcademia=\"%s\" SessionID=\"%ld\".\r\n", g_userName, g_iSessionID);
		WriteLog__( fp__, sparams);	
	}
	
	sprintf( sparams, "g_appVersion = '%s'\n", g_appVersion);
	WriteLog__( fp__, sparams);	

	TCHAR *serverName = new TCHAR[MAX_PATH];
	TCHAR *appVersion = new TCHAR[MAX_PATH];	

	int iLanguage = ENG;
	if ( sRoot == NULL)
		sRoot = g_root;
	getAppParams( sRoot, &serverName, &appVersion, &iLanguage, fp__);
	hprintf(hLogFile, &buffer, _T("AppVersion=\"%s\" ServerName=\"%s\" Language=\"%s\".\r\n\r\n"), appVersion, serverName, iLanguage == RUS ? "rus" : "eng");	
	sprintf( sparams, "szCrashType=\"%s\" AppVersion=\"%s\" ServerName=\"%s\" Language=\"%s\".", sCrashType, appVersion, serverName, (iLanguage == RUS ? "rus" : "eng"));
	WriteLog__( fp__, sparams);	

	WriteLog__( fp__, "\nSystem Info\n");
	CloseLog__( &fp__);
	OpenLog__( &fp__, NULL, sMumpLog);
	
	DumpSystemInformation(hLogFile, hProcessHandle, &buffer);

	/*
	вместо исключени€
	*/

	hprintf(hLogFile, &buffer, _T("Read from location 1d69400c caused an access violation. \r\nProcess:  %s;"),
		W2A(szModuleName));

	/*
	//исключение
	int flag = 0; 
	PCHAR Str = Get_Exception_Info( pExceptionInfo, fp__, flag);
	if ( Str)
	hprintf(hLogFile, &buffer, _T("%s"), Str);
	*/

	hprintf(hLogFile, &buffer, _T("\r\n"));

	WriteLog__( fp__, "Register Info\n");

	/*
	//исключение
	DumpRegisters(hLogFile, Context, &buffer);
	*/

	// Print out the bytes of code at the instruction pointer. Since the
	// crash may have been caused by an instruction pointer that was bad,
	// this code needs to be wrapped in an exception handler, in case there
	// is no memory to read. If the dereferencing of code[] fails, the
	// exception handler will print '??'.
	hprintf(hLogFile, &buffer, _T("\r\nBytes at CS:EIP:\r\n"));

	hprintf(hLogFile, &buffer, _T("\r\n"));

	//////////////////////////////////////////////////////////////////////////
	//
	// Stack
	//
	//////////////////////////////////////////////////////////////////////////

	// Time to print part or all of the stack to the error log. This allows
	// us to figure out the call stack, parameters, local variables, etc.

	// Esp contains the bottom of the stack, or at least the bottom of
	// the currently used area
	//DWORD* pStack = (DWORD *)Context->Esp;

	//DumpStack(hLogFile, pStack, &buffer);

	//////////////////////////////////////////////////////////////////////////
	//
	// Modules
	//
	//////////////////////////////////////////////////////////////////////////	
	hprintf(hLogFile, &buffer, _T("\r\n---------------\r\n"));

	CloseLog__( &fp__);
	OpenLog__( &fp__, NULL, sMumpLog);
	WriteLog__( fp__, "Modules Info\n");	

	DumpModuleList(hLogFile, &buffer);

	hflush(hLogFile, &buffer);

	hprintf(hLogFile, &buffer, _T("\r\n===== [end of %s] =====\r\n"), 
		szFileInfoName);	

	delete[] sparams;
	delete[] appVersion;
	delete[] serverName;
	delete[] sUUID;
	delete[] errorType;

	CloseHandle(hLogFile);

	CloseLog__( &fp__);
	OpenLog__( &fp__, NULL, sMumpLog);
	WriteLog__( fp__, "Loading 'DBGHELP.DLL'...");
	WriteLog__( fp__, "\n");

	if ( hDll == NULL)
	{
		// load any version we can
		hDll = ::LoadLibrary( "DBGHELP.DLL" );	
	}

	LPCTSTR szResult = NULL;

	if ( hDll)
	{		
		WriteLog__( fp__, "Dll 'DBGHELP.DLL' is loaded");
		WriteLog__( fp__, "\n");

		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if ( pDump)
		{
			char szScratch[_MAX_PATH];
			wchar_t szDumpPath[ MAX_PATH_4];	
			char szDumpName[ MAX_PATH_4];
			_tcscpy( szDumpName, m_szAppName);
			if ( iCrashType == APP_HANG_ID)
				_tcscat( szDumpName, "_hang");
			else if (iCrashType == ASSERT_ERROR_ID)
				_tcscat( szDumpName, "_assert");
			
			if ( !getFileNameByProjName( szDumpName, L"dmp", szDumpPath))
			{
				//bEnding = true;
				//return EXCEPTION_CONTINUE_SEARCH;
			}


			// create the file
			HANDLE hFile = ::CreateFileW( szDumpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL, NULL );

			if ( hFile != INVALID_HANDLE_VALUE)
			{				
				int processID = GetProcessId( hProcessHandle);
				sprintf( szScratch, "processID = %d\n", processID);
				WriteLog__( fp__, szScratch);
				BOOL bOK = pDump( hProcessHandle, processID, hFile, static_cast<MINIDUMP_TYPE>(MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithProcessThreadData | MiniDumpWithHandleData), NULL, NULL, NULL);
				//BOOL bOK = pDump( hProcessHandle, processID, hFile, MiniDumpWithIndirectlyReferencedMemory, NULL, NULL, NULL);
				if (bOK)
				{				
					sprintf( szScratch, "_make_dump::Saved dump file to '%s'",  W2A(szDumpPath) );
					WriteLog__( fp__, szScratch);
					WriteLog__( fp__, "\n");
					szResult = szScratch;					
					retval = true;
				}
				else
				{
					unsigned int error = HRESULT_FROM_WIN32(GetLastError());
					sprintf( szScratch, "_make_dump::Failed to save dump file to '%s' (error %d)", W2A(szDumpPath), error);
					WriteLog__( fp__, szScratch);
					WriteLog__( fp__, "\n");
					szResult = szScratch;
					retval = false;
				}
				::CloseHandle(hFile);
				//if ( retval)
				//{
					if ( ZipFiles( iCrashType, m_szAppName, szDumpPath, fp__, sMumpLog) < 2)
						retval = false;
				//}
			}
			else
			{
				sprintf( szScratch, "Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError());
				WriteLog__( fp__, szScratch);
				WriteLog__( fp__, "\n");
				szResult = szScratch;
				retval = false;
			}
		}
		else
		{
			szResult = "DBGHELP.DLL too old";
			WriteLog__( fp__, "DBGHELP.DLL too old");
			WriteLog__( fp__, "\n");
		}
	}
	else
	{
		szResult = "DBGHELP.DLL not found";
		WriteLog__( fp__, "DBGHELP.DLL not found");
		WriteLog__( fp__, "\n");
	}

	delete[] szFileInfoName;	
	GetSystemTime( &time);	
	sprintf( sTime, "creating dump is finished \n  system time = %d.%d.%d-%d:%d:%d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);	
	WriteLog__( fp__, sTime);
	GetLocalTime( &time);
	sprintf( sTime, "  local time = %d.%d.%d-%d:%d:%d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);	
	WriteLog__( fp__, sTime);	
	CloseLog__( &fp__);	
	if ( !retval)
	{		
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// pop up our crash report app
	//
	//////////////////////////////////////////////////////////////////////////
	if ( bSendReport)
	{
		TCHAR *sCrashType = ( iCrashType == APP_HANG_ID) ? APP_HANG : ( ( iCrashType == ASSERT_ERROR_ID) ? ASSERT_ERROR : (( iCrashType == APP_LAUNCHER_ERROR_ID) ? APP_LAUNCHER_ERROR : APP_ERROR));
		retval = SendReport( appName, pszFilePart, szModuleName, fp__, sCrashType);
	}

	

	return retval;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Code is written by Vladimir (2011.03.01)

HWND ghwndProcessWindowMDump = NULL;
TCHAR wndClassName[ MAX_PATH];


BOOL CALLBACK EnumWindowsProcMDump(HWND hwnd, LPARAM lParam){
	DWORD dwWinProcID = 0;
	GetWindowThreadProcessId(hwnd, &dwWinProcID);
	if(dwWinProcID == (DWORD)lParam){
		TCHAR   className[MAX_PATH+1];
		if( GetClassName( hwnd, className, MAX_PATH) > 0)
		{
			if( _tcscmp( className, wndClassName) == 0)
			{
				ghwndProcessWindowMDump = hwnd;
				return FALSE; // нашли нужное окно. ћожно не продолжать поиск
			}
		}
	}
	return TRUE;
}

HWND MDUMP_API GetWindowHandle( int processId, TCHAR *aWndClassName /* = NULL */)
{
	if ( aWndClassName != NULL)
		strcpy( wndClassName, aWndClassName);
	else
		strcpy( wndClassName, "VSpaces virtual worlds player");
	EnumWindows(EnumWindowsProcMDump, (LPARAM)processId);
	return ghwndProcessWindowMDump;
}

// end Vladimir
//////////////////////////////////////////////////////////////////////////

BOOL bCreatingDumpAndSending = TRUE;

BOOL MDUMP_API make_dump(int iCrashType)
{	
	if ( !bCreatingDumpAndSending)
		return FALSE;

	//CDialogDebugError dialogDebugPreview;
	DWORD processID = GetCurrentProcessId();
	HANDLE hProcessHandle = GetCurrentProcess();
	HWND hWnd = GetWindowHandle( processID);

	TCHAR *serverName = new TCHAR[MAX_PATH];
	TCHAR *appVersion = new TCHAR[MAX_PATH];	
	getAppParams( g_root, &serverName, &appVersion, &g_iLanguage, NULL);
	
	_make_dump( NULL, hProcessHandle, TRUE, iCrashType);
	
	delete[] appVersion;
	delete[] serverName;
	
	return TRUE;
}

BOOL MDUMP_API make_dump( bool bNoError)
{
	if ( !bNoError)
		return make_dump();
}

BOOL MDUMP_API make_dump( WCHAR *sRoot, HANDLE hProcessHandle, BOOL bSendReport, int iCrashType)
{	
	return _make_dump( sRoot, hProcessHandle, bSendReport, iCrashType);
}

BOOL MDUMP_API send_report()
{	
	return SendReport( appName, NULL, szModuleName, NULL, APP_HANG);	
}
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MDUMP_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MDUMP_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#if MDUMP_STATIC_LIB

#define MDUMP_API

#else

#ifdef MDUMP_EXPORTS
#define MDUMP_API __declspec(dllexport)
#else
#define MDUMP_API __declspec(dllimport)
#endif

// This class is exported from the mdump.dll
class MDUMP_API Cmdump {
public:
	Cmdump(void);
	// TODO: add your methods here.
};

extern MDUMP_API int nmdump;

MDUMP_API int fnmdump(void);

#endif

#if _MSC_VER < 1300
#define DECLSPEC_DEPRECATED
// VC6: change this path to your Platform SDK headers
#include "M:\\dev7\\vs\\devtools\\common\\win32sdk\\include\\dbghelp.h"			// must be XP version of file
#else
// VC7: ships with updated headers
#include "dbgeng.h"
#include "dbghelp.h"
#endif

class CWS3D;

//#define FUNCTION_STACK_TRACE(x) \
//	char * function_stack_trace_buffer = (char *) _alloca(8); \
//	CopyMemory(function_stack_trace_buffer, x, 8);

// based on dbghelp.h

#define	ONEK			1024
#define	SIXTYFOURK		(64*ONEK)
#define	ONEM			(ONEK*ONEK)
#define	ONEG			(ONEK*ONEK*ONEK)

#include "VacademiaDump.h"

//#define EXCEPTION_EXECUTE_HANDLER       1
//#define EXCEPTION_CONTINUE_SEARCH       0
//#define EXCEPTION_CONTINUE_EXECUTION    -1
#define EXCEPTION_VA_RUNTIME_MODULE			101


typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
										 CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
										 CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
										 CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
										 );

typedef BOOL (WINAPI *MINIDUMPREADDUMPSTREAM)(PVOID BaseOfDump,
										 ULONG StreamNumber,
										 PMINIDUMP_DIRECTORY* Dir,
										 PVOID* StreamPointer,
										 ULONG* StreamSize
										 );

extern MDUMP_API LPTOP_LEVEL_EXCEPTION_FILTER StartExceptionHandler(LPTOP_LEVEL_EXCEPTION_FILTER apFilterFunction);

extern MDUMP_API void StartExceptionHandlers();

extern MDUMP_API void SetSessionID( long aSessionID);

extern MDUMP_API void SetUserMDump( const wchar_t *aUserName, const wchar_t *aPassword, int aiEncodingPassword);
extern MDUMP_API void SetUserMDump( const char *aUserName, const char *aPassword, int aiEncodingPassword);

extern MDUMP_API void SetAppVersionMDump( const TCHAR *aAppVersion);

extern MDUMP_API void SetRootMDump( const WCHAR *aRoot);

extern MDUMP_API void SetLauncherMDump( const TCHAR *aLauncherName);

extern MDUMP_API void SetAppFinishing();

extern MDUMP_API void SetWS3D( CWS3D *pWS3D);

extern MDUMP_API void HideLauncher(const TCHAR * szEventName);

extern MDUMP_API BOOL getAppParams( WCHAR *sRoot, TCHAR **serverName, TCHAR **appVersion, int *iLanguage, FILE *fpLog);

extern MDUMP_API LONG WINAPI MDumpFilterFunction( EXCEPTION_POINTERS *pExceptionInfo, int iCrashType = APP_ERROR_ID);

extern MDUMP_API HWND GetWindowHandle( int processId, TCHAR *aWndClassName = NULL);

extern MDUMP_API BOOL make_dump(int iCrashType = ASSERT_ERROR_ID);

extern MDUMP_API BOOL make_dump( bool bNoError);

extern MDUMP_API BOOL make_dump( WCHAR *sRoot, HANDLE hProcessHandle, BOOL bSendReport, int iCrashType);

extern MDUMP_API BOOL send_report();

#define HPRINTF_BUFFER_SIZE 1024 * 8

#ifndef _countof
	#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

//////////////////////////////////////////////////////////////////////////

struct TBuffer
{	
	TCHAR hprintf_buffer[ HPRINTF_BUFFER_SIZE];	// wvsprintf never prints more than one K.
	int hprintf_index;
	int NumCodeBytes;			// Number of code bytes to record.
	int MaxStackDump;			// Maximum number of DWORDS in stack dumps.
	int StackColumns;			// Number of columns in stack dump.

	void write( LPCTSTR Format, va_list arglist)
	{
		hprintf_index += wvsprintf( &hprintf_buffer[ hprintf_index], Format, arglist);
	}
	TBuffer()
	{
		NumCodeBytes = 16;
		MaxStackDump = 3072;
		StackColumns = 4;
		hprintf_index = 0;
		ZeroMemory(hprintf_buffer, HPRINTF_BUFFER_SIZE);
	}
};

extern void hflush(HANDLE LogFile, TBuffer *buffer);
extern void hprintf(HANDLE LogFile, TBuffer *buffer, LPCSTR Format, ...);
extern void DumpModuleList(HANDLE LogFile, TBuffer *buffer);
extern void DumpSystemInformation(HANDLE LogFile, HANDLE hProcess, TBuffer *buffer);
extern const TCHAR *GetExceptionDescription(DWORD ExceptionCode);
extern void DumpStack(HANDLE LogFile, DWORD *pStack, TBuffer *buffer);
extern void DumpRegisters(HANDLE LogFile, PCONTEXT Context, TBuffer *buffer);
extern LONG SendReport( TCHAR *appName, WCHAR *pszFilePart, WCHAR *szModuleName, FILE *fp__, TCHAR *pCrashType);
extern PCHAR WINAPI Get_Exception_Info(PEXCEPTION_POINTERS pException, FILE *fp__, int &flag);
extern bool _make_dump( WCHAR *sRoot, HANDLE hProcessHandle, BOOL bSendReport, int iCrashType, BOOL bRepeat = FALSE);

extern void restartModule();
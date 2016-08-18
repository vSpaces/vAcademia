
#include "StdAfx.h"
#include "MainWindow.h"
#include "resource.h"
#include "Common.h"
#include "ComString.h"
#include "CommandLineParams.h"
#include "Win7ApplicationIDFix.h"
#include "Filesys.h"
#include "ILogWriter.h"
#include "rm\rm.h"
#include "rm\rmext.h"

#include <atlbase.h>
CComModule _Module;
#include <atlcom.h>
#include <objbase.h>

#include <d3d.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <shellapi.h>
#include "mdump.h"

#include "Cscl3DWS.h"
#include "res.h"
#include "rmml.h"

#include "Tlhelp32.h"
#include "Psapi.h"

#include "ServiceMan.h"
#include "SyncMan.h"
#include "InfoMan.h"

#include "UrlParser.h"
#include "notifies.h"

#include "ReportMan.h"
#include "VacademiaReport.h"

#include "../nengine/ncommon/Thread.h"

#include "StringFunctions.h"

#include "windows.h"
#include "Winuser.h"
#include "CrashHandler.h"

#include "Wtsapi32.h"

#include "WindowPrintSupport.h"

#include "..\DesktopMan\include\IDesktopMan.h"

using namespace res;
using namespace ws3d;

bool	bExit = false;
bool	bUserExit = false;

int g_sleep = 0;

#define MAX_LOADSTRING 100
#define IDT_FPS			0
#define MAX_PATH2		3120

HANDLE g_hangMemoryObject = NULL;

// Global Variables:
HINSTANCE hInstance;								// current instance
HACCEL hAccelTable;
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void				SetupIWS3DParams();
void				DeleteRunLogFileForLauncher();
LONG WINAPI			OMSPlayerFilterFunction( struct _EXCEPTION_POINTERS *pExceptionInfo );
bool				IsRemoveEvopVOIPTrialExists();

CCommandLineParams*	gcmParams = NULL;
IConnectionPoint* gpCP=NULL;
HWND ghWnd=NULL;
HWND gh3DWSWnd=NULL;
bool gbFullScreen = false;
bool gbTopWindow = false;
bool gbUseLog = 0;
bool gbUseSyncLog = 0;
bool gbUseOculus = false;
int giSendLog = 0;
bool gbUseTracer = 0;
int  giMaxFPS = 30;
bool gbTitleIsSet = false;
wchar_t glpcWindowTitle[3000]=L"";
BOOL	gbClientSizeInitialized = FALSE;
DWORD	dwClientWidth = 1024;
DWORD	dwClientHeight = 768;
HMODULE ghmComMan = NULL;
HMODULE ghmSoundMan = NULL;
HMODULE ghmLogMan = NULL;
HMODULE ghmCscl3DWS = NULL;
HANDLE  hOMSPlayerProcessHandle = 0;
int giEmulateNetworkLags = 0;
char* glpCmdLine = NULL;

bool g_intel = false;
bool g_nvidia = false;

LPDIRECTDRAW7        gpDD;

#define REPOSITORY_PATH L"root\\files\\"
#define USER_PATH L"user\\"
#define USER_WND_CLOSE APP_FINISHING

wchar_t glpcRootPath[MAX_PATH*2+2]= L"";
wchar_t glpcRepositoryPath[MAX_PATH*2+2]=REPOSITORY_PATH;
wchar_t glpcUserPath[MAX_PATH*2+2]=USER_PATH;
TCHAR glpcUIPath[MAX_PATH*2+2]=_T("");
wchar_t glpcDownloadManager[MAX_PATH*2+2]= L"";
TCHAR glpcComponentsPath[MAX_PATH*2+2]=_T("\1");
// Путь, устанавливаемый в ComMan.QueryBase
TCHAR glpcQueryBase[MAX_PATH*2+2]=_T("");
// Путь, устанавливаемый в ComMan.RemoteBase
TCHAR glpcRemoteBase[MAX_PATH*2+2]=_T("");
// Путь в реестре для получения параметров
TCHAR glpcRegPath[MAX_PATH*2+2]=_T("");
// Тип соединения менеджера ресурсов 
TCHAR glpcResServerConnectType[MAX_PATH]=_T("");
// Тип соединения лог сервера
TCHAR glpcLogServerConnectType[MAX_PATH]=_T("");

wchar_t lpcIniFullPathName[MAX_PATH*2+2]=L"";

oms::omsContext *pOMSContext = NULL;
CReportMan g_reportMan;

CWindowPrintSupport* g_printWindowSupport = NULL;

volatile bool bCheckDesktopValid = true;

void SendPercent(int percent)
{
	SendSignal2Launcher( launchProcessID, APP_PERCENT, percent);
	CLogValue l("Sended percent: ", percent);
	if ((pOMSContext) && (pOMSContext->mpLogWriter))
	{
		pOMSContext->mpLogWriter->WriteLn(l);
	}
}

bool InitWS3DContinue();

typedef HRESULT (*TRegisterProc)();

unsigned int g_screenSaverEnabled = 0;

void DisableScreensaver()
{
	SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, (void*)&g_screenSaverEnabled, 0);
	if (g_screenSaverEnabled)
	{
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, 0, 0);
	}
}

void SetSharedMemoryValue(unsigned int value)
{
	if (!g_hangMemoryObject)
	{
		g_hangMemoryObject = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4, L"vAcademiaHangCheck");

		if (!g_hangMemoryObject)
		{
			return;
		}
	}
	
	unsigned int* mem = (unsigned int *)MapViewOfFile(g_hangMemoryObject, FILE_MAP_ALL_ACCESS, 0, 0, 4);							
	if (mem == NULL) 
	{			
		return;
	}

	*mem = value;

	UnmapViewOfFile(mem);	
}

void OnFrameEnd()
{
	SetSharedMemoryValue(GetTickCount());
}

void EnableScreensaver()
{
	if (g_screenSaverEnabled)
	{
		SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 1, 0, 0);
	}
}

bool IsFullPath(LPCSTR alpcStr)
{
	int iLen = strlen( alpcStr);
	// example ..
	if( iLen < 3)
		return false;
	// example \\host\share
	if( alpcStr[0] == '\\' && alpcStr[0] == '\\')
		return true;
	
	// example c:/
	if( alpcStr[1] == ':' &&
		(alpcStr[2] == '\\' || alpcStr[2] == '/'))
		return true;
	return false;
}

bool IsFullPath(LPCWSTR alpcStr)
{
	int iLen = wcslen( alpcStr);
	// example ..
	if( iLen < 3)
		return false;
	// example \\host\share
	if( alpcStr[0] == L'\\' && alpcStr[0] == L'\\')
		return true;

	// example c:/
	if( alpcStr[1] == L':' &&
		(alpcStr[2] == L'\\' || alpcStr[2] == L'/'))
		return true;
	return false;
}

BOOL GetProcessModule (DWORD dwPID, DWORD dwModuleID, 
					   LPMODULEENTRY32 lpMe32, DWORD cbMe32) 
{ 
	BOOL          bRet        = FALSE; 
	BOOL          bFound      = FALSE; 
	HANDLE        hModuleSnap = NULL; 
	MODULEENTRY32 me32        = {0}; 

	// Take a snapshot of all modules in the specified process. 

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); 
	if (hModuleSnap == INVALID_HANDLE_VALUE) 
		return (FALSE); 

	// Fill the size of the structure before using it. 

	me32.dwSize = sizeof(MODULEENTRY32); 

	// Walk the module list of the process, and find the module of 
	// interest. Then copy the information to the buffer pointed 
	// to by lpMe32 so that it can be returned to the caller. 

	if (Module32First(hModuleSnap, &me32)) 
	{ 
		do 
		{ 
			if (me32.th32ModuleID == dwModuleID) 
			{ 
				CopyMemory (lpMe32, &me32, cbMe32); 
				bFound = TRUE; 
			} 
		} 
		while (!bFound && Module32Next(hModuleSnap, &me32)); 

		bRet = bFound;   // if this sets bRet to FALSE, dwModuleID 
		// no longer exists in specified process 
	} 
	else 
		bRet = FALSE;           // could not walk module list 

	// Do not forget to clean up the snapshot object. 

	CloseHandle (hModuleSnap); 

	return (bRet); 
}

HWND ghwndProcessWindow = NULL;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam){
	DWORD dwWinProcID = 0;
	GetWindowThreadProcessId(hwnd, &dwWinProcID);
	if(dwWinProcID == (DWORD)lParam){
        TCHAR   className[MAX_PATH+1];
        if( GetClassName( hwnd, className, MAX_PATH) > 0)
        {
            if( _tcscmp( className, szWindowClass) == 0)
            {
		        ghwndProcessWindow = hwnd;
		        return FALSE; // нашли нужное окно. Можно не продолжать поиск
            }
        }
	}
	return TRUE;
}

void KillOtherPlayerProcesses(HINSTANCE ahInstance)
{
	// получаем путь к exe-файлу
	WCHAR lpcStrBuffer[MAX_PATH*2+2];
	::GetModuleFileNameW( ahInstance, lpcStrBuffer, MAX_PATH);
	WCHAR lpcExeFullPathName[MAX_PATH*2+2];
	LPWSTR pTemp;
	::GetFullPathNameW( lpcStrBuffer,
		MAX_PATH*2,
		lpcExeFullPathName,
		&pTemp);
	WCHAR lpcExeName[MAX_PATH];
	wcscpy(lpcExeName,pTemp);
	DWORD dwMyProcessID = GetCurrentProcessId();

	HANDLE         hProcessSnap = NULL; 
	BOOL           bRet      = FALSE; 
	PROCESSENTRY32W pe32      = {0}; 

	//  Take a snapshot of all processes in the system. 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return; 

	//  Fill in the size of the structure before using it. 
	pe32.dwSize = sizeof(PROCESSENTRY32); 

	//  Walk the snapshot of the processes, and for each process, 
	//  display information. 
	if (Process32FirstW(hProcessSnap, &pe32)) { 
		DWORD         dwPriorityClass; 
		BOOL          bGotModule = FALSE; 
		MODULEENTRY32 me32       = {0}; 

		do{ 
			// если это мой процесс, то пропусаем
			if(pe32.th32ProcessID == dwMyProcessID)
				continue;

			// убиваем ffmpeg.exe, который может висеть в памяти
			if ((wcscmp(pe32.szExeFile, L"ffmpeg.exe") == 0) || (wcscmp(pe32.szExeFile, L"pipe_forward.exe") == 0)
				|| (wcscmp(pe32.szExeFile, L"vjs_code_editor.exe") == 0))
			{
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE,FALSE, pe32.th32ProcessID);
				if (hProcess != NULL)
				{
					rtl_TerminateOtherProcess(hProcess, (DWORD)-1);
					CloseHandle(hProcess);
				}
				continue;
			}

			// если название EXE-фала другое, то тоже пропускаем
			if(wcscmp(lpcExeName, pe32.szExeFile) != 0)
				continue;
			// открываем процесс, чтобы узнать побольше о его состоянии
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,
				FALSE, pe32.th32ProcessID);
			// если процесс открыть не смогли, то пропускаем его
			if (hProcess == NULL)
				continue;
			DWORD cbNeeded;
			WCHAR szModulePath[MAX_PATH2];
			LPWSTR pszProcessName = NULL;
			// получаем полный путь к EXE-файлу процесса
			if (GetModuleFileNameExW(hProcess, NULL, szModulePath, MAX_PATH2)){
				// если путь к exe-файлу разный, то пропускаем процесс
				if(_wcsicmp(lpcExeFullPathName, szModulePath) != 0){
					CloseHandle(hProcess);
					continue;
				}
			}
			CloseHandle(hProcess);
			// находим главное окно процесса
			ghwndProcessWindow = NULL;
			EnumWindows(EnumWindowsProc, (LPARAM)pe32.th32ProcessID);
			// если главное окно нашли
			if(ghwndProcessWindow != NULL){
				// и с ним все в порядке
				// (пока будем считать, что с окном все в порядке, если можно взять инфу о нем)
				if( IsWindowVisible( ghwndProcessWindow))
					continue;
			}

			// если главное окно процесса не найдено, значит с ним что-то не так. Можно убивать
			
			// получаем описатель процесса
			hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
			if (hProcess == NULL)
				continue;

			DWORD dwError = ERROR_SUCCESS;

			// пытаемся завершить процесс
			if (!rtl_TerminateOtherProcess(hProcess, (DWORD)-1))
				dwError = GetLastError();

			// закрываем описатель процесса
			CloseHandle(hProcess);
		} 
		while (Process32NextW(hProcessSnap, &pe32)); 
	} 

	// Do not forget to clean up the snapshot object. 
	CloseHandle (hProcessSnap); 
}

#include "func.h"

void checkMDumpExisted( TCHAR *appName)
{
	std::string szProjectName = appName;
	int pos = szProjectName.rfind( ".exe");
	if ( pos > -1)
		szProjectName = szProjectName.substr( 0, pos);
	WCHAR fullAppName[ MAX_PATH_4];
	getApplicationDataDirectory( fullAppName);
	std::wstring sDir = fullAppName;
	sDir += L"\\*.*";
	int dumpIndexMax = 0;

	pOMSContext->mpLogWriter->WriteLn( "Find dump-files and zip-files is starting....");

	WIN32_FIND_DATAW fdData;
	USES_CONVERSION;
	HANDLE hFind = FindFirstFileW( sDir.c_str(), &fdData);	
	if (hFind != INVALID_HANDLE_VALUE)
	{
		std::string sFileName;
		int dumpIndex = 0;
		do 
		{
			if ((fdData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				sFileName = W2A( fdData.cFileName);
				int pos = sFileName.find( szProjectName);
				int pos2 = sFileName.find( ".");
				int pos3 = sFileName.find( ".zip");
				if ( pos > -1 && pos < pos2 - szProjectName.length()
					&& pos3 == sFileName.length() - 4)
				{
					pOMSContext->mpLogWriter->WriteLn("\tZip-file: ", sFileName);
					continue;
				}

				pos3 = sFileName.find( ".dmp");
				if ( pos > -1 && pos < pos2 - szProjectName.length()
					&& pos3 == sFileName.length() - 4)
				{
					pOMSContext->mpLogWriter->WriteLn("\tDump-file: ", sFileName);
					continue;
				}
				
				pos = sFileName.find( "INFO");
				if ( pos == -1)
					pos = sFileName.find( "Info");
				if ( pos == -1)
					pos = sFileName.find( "info");
				pos2 = sFileName.find( ".");
				pos3 = sFileName.find( ".txt");
				if ( pos > -1 && pos < pos2 - szProjectName.length()
					&& pos3 == sFileName.length() - 4)
				{
					pOMSContext->mpLogWriter->WriteLn("\tINFO-file: ", sFileName);
					continue;
				}
			}
		} while ( FindNextFileW( hFind, &fdData));
	}
	pOMSContext->mpLogWriter->WriteLn("Find dump-files and zip-files is finished");
}

#include "CrashHandlerInThread.h"

DWORD WINAPI WorldConnectThread_( VOID* param)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	((service::IServiceWorldManager*)param)->Connect();
	checkMDumpExisted( "player.exe");
	g_reportMan.init( "player.exe", APP_PREV_CRASH, (WCHAR*)pOMSContext->mp3DWS->GetApplicationRoot());
	//g_reportMan.setProtocol( CReportMan::SOCKET_QUERY);
	g_reportMan.runAsynchIn( (service::IServiceWorldManager*)param);
	return 0;
}

bool InitWS3D(HWND ahWnd)
{
	SendPercent(3);
	RECT rect;
	rect.left=0; rect.right=1024;
	rect.top=0; rect.bottom=768;

	rect.right=1;
	rect.bottom=1;
	
	/*
	Vladimir
	*/
	//MessageBox(0, "InitWS3D 1","", MB_OK);
	if ( gcmParams->LauncherProcessIdIsSet())
	{		
		//MessageBox(0, "InitWS3D 2","", MB_OK);
		launchProcessID = gcmParams->GetLauncherProcessID();
		/*
		Vladimir
		регистрация сообщений для общения между двумя сотрудничающими приложений
		*/
		std::string eventName = gcmParams->GetLauncherEventName();
		TCHAR wndMsgName[ MAX_PATH];	wndMsgName[0] = 0;
		strcpy( wndMsgName, eventName.c_str());
		strcat( wndMsgName, "_player");

		//MessageBox(0, "InitWS3D 3","", MB_OK);
		RegisterWindowMessage( wndMsgName);		
		/*
		end Vladimir
		*/

		//MessageBox(0, "InitWS3D 4","", MB_OK);
		SendPercent(4);
	}

	/*
	end Vladimir
	*/
	//MessageBox(0, "InitWS3D 5","", MB_OK);
	pOMSContext = Create3DWSManager(ahWnd, &rect);

	//MessageBox(0, "InitWS3D 6","", MB_OK);
	SetupIWS3DParams();
	SendPercent( 5);

	if (pOMSContext->mp3DWS->initialize(ghWnd) == S_OK)
	{
		//MessageBox(0, "InitWS3D 8","", MB_OK);
		std::wstring logPath = pOMSContext->mpLogWriter->GetLogFilePath();
		COPYDATASTRUCT CopyData;
		CopyData.dwData=0;
		CopyData.cbData=sizeof(wchar_t)*(logPath.size()+1);
		CopyData.lpData= (PVOID)logPath.c_str();
		SendData2Launcher( launchProcessID, APP_PERCENT, &CopyData);

		if (!InitWS3DContinue())
			return false;

		if (pOMSContext->mpServiceWorldMan != NULL)
		{
			pOMSContext->mpServiceWorldMan->SetWndHandle(ahWnd);

			CSimpleThreadStarter connectThreadStarter(__FUNCTION__);
			connectThreadStarter.SetRoutine(WorldConnectThread_);
			connectThreadStarter.SetParameter(pOMSContext->mpServiceWorldMan);
			HANDLE hWorldConnectThread = connectThreadStarter.Start();			
		}
		//MessageBox(0, "InitWS3D 10","", MB_OK);
		if( pOMSContext->mpLogWriter != NULL)
		{
			if( IsRemoveEvopVOIPTrialExists())
				pOMSContext->mpLogWriter->WriteLn("[VOIP] Failed to remove EvoVOIP trial");
			else
				pOMSContext->mpLogWriter->WriteLn("[VOIP] EvoVOIP trial removed successfully");
		}
		//MessageBox(0, "InitWS3D 11","", MB_OK);
		return true;
	}

	return false;
}

typedef HRESULT (__stdcall *TGetClassObjectProc)(REFCLSID rclsid, REFIID riid, LPVOID * ppv);

TCHAR	gpcRegKeyPath[MAX_PATH];

void RemoveEvopVOIPTrial()
{
	CComString sKeyPath = "Software\\Conaito";
	RegDeleteKey( HKEY_CURRENT_USER, sKeyPath.GetBuffer());
}

bool IsRemoveEvopVOIPTrialExists()
{
	CComString sKeyPath = "Software\\Conaito";
	HKEY hKey = 0;
	if( RegOpenKey( HKEY_CURRENT_USER, sKeyPath.GetBuffer(), &hKey) == ERROR_SUCCESS)
	{
		RegCloseKey( hKey);
		return true;
	}
	return false;
}

HKEY OpenProgramRegKey()
{
	HKEY	hKey = NULL;
	ZeroMemory(&gpcRegKeyPath, MAX_PATH*sizeof(TCHAR));
	_tcscat( gpcRegKeyPath, "SOFTWARE\\VCitiesLabs\\");	_tcscat( gpcRegKeyPath, glpcRegPath);
	if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, gpcRegKeyPath, 0, KEY_READ, &hKey)==ERROR_SUCCESS && hKey!=0){
	}else{
		_tcscpy( gpcRegKeyPath, "SOFTWARE\\VSpaces\\");	_tcscat( gpcRegKeyPath, glpcRegPath);
		if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, gpcRegKeyPath, 0, KEY_READ, &hKey)==ERROR_SUCCESS && hKey!=0){
		}/*else{
			//MessageBox(0, _T("Некорректная установка программы.\nНеобходима регистрация параметров сервера."), _T("Error!"), MB_OK);
			ATLASSERT( FALSE);
			return NULL;
		}*/
	}
	return hKey;
}

void SetupIWS3DParams()
{
	ATLASSERT( pOMSContext);
	if( !pOMSContext)	
		return;

	//MessageBox(0, "SetupIWS3DParams 1","", MB_OK);

	USES_CONVERSION;
	//MessageBox(0, "SetupIWS3DParams 2","", MB_OK);
	pOMSContext->mp3DWS->put_PathIni( lpcIniFullPathName);
//MessageBox(0, "SetupIWS3DParams 3","", MB_OK);
	pOMSContext->mp3DWS->SetApplicationRoot( glpcRootPath);
//MessageBox(0, "SetupIWS3DParams 4","", MB_OK);
	/*
	//Vladimir - передача командной строки в Cscl3DWS
	*/	
	/*
	if ( gcmParams->UrlIsSet())
		pOMSContext->mpApp->SetArguments( A2W( "-url " + gcmParams->GetUrl()));
	if ( gcmParams->RootIsSet())
		pOMSContext->mpApp->SetArguments( A2W( "-root " + gcmParams->GetRoot()));
	if ( gcmParams->ModuleIsSet())
		pOMSContext->mpApp->SetArguments( A2W( "-module " + gcmParams->GetModule()));
	if ( gcmParams->SceneIsSet())
		pOMSContext->mpApp->SetArguments( A2W( "-scene " + gcmParams->GetScene()));
	if ( gcmParams->UserNameIsSet())
		pOMSContext->mpApp->SetArguments( A2W( "-user " + gcmParams->GetUserName()));
	if ( gcmParams->PasswordIsSet())
		pOMSContext->mpApp->SetArguments( A2W( "-password " + gcmParams->GetPassword()));
	*/
//MessageBox(0, "SetupIWS3DParams 5","", MB_OK);	


	if ( gcmParams->RootIsSet())
	{
		SetRootMDump( gcmParams->GetRoot().c_str());
	}

	wchar_t *wcsVersion = new wchar_t[MAX_PATH];
	pOMSContext->mpApp->GetVersion((wchar_t*)wcsVersion, 99);	
	
	TCHAR *csVersion = W2A( wcsVersion);
	SetAppVersionMDump( csVersion);

	delete[] wcsVersion; 
	/*
	//end Vladimir
	*/
}

bool getAutoProxyKey(const char *apName, BSTR* apwcValue)
{
	HKEY hKey;
	if( RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_READ, &hKey)==ERROR_SUCCESS && hKey!=0)
	{
		DWORD dwType = REG_SZ;
		DWORD dwTypeEnable = REG_DWORD;
		DWORD dwSize;
		DWORD dwSizeEnable = 8;
		char pEnable[4];
		LONG lResEnable = RegQueryValueEx( hKey, "ProxyEnable", 0, &dwTypeEnable, (BYTE*)pEnable, &dwSizeEnable);
		if(lResEnable != ERROR_SUCCESS)
			return false;
		int iEnable = rtl_atoi(pEnable);
		if(iEnable==0)
			return false;
		// получить длину значения
		LONG lRes = RegQueryValueEx( hKey, apName, 0, &dwType, NULL, &dwSize);
		if( dwSize > 0)
		{
			DWORD dwSize = 1024;
			char p[1024];
			lRes = RegQueryValueEx( hKey, apName, 0, &dwType, (BYTE*)p, &dwSize);
			RegCloseKey(hKey);
			if (lRes == ERROR_SUCCESS)
			{
				int iLen = strlen(p);
				int nLen = MultiByteToWideChar(CP_ACP, 0, p, iLen, NULL, NULL);
				*apwcValue = ::SysAllocStringLen(NULL, nLen);
				if (*apwcValue == NULL)
				{
					ATLASSERT(0);
					return NULL;
				}
				MultiByteToWideChar(CP_ACP, 0, p, iLen, *apwcValue, nLen);
				return true;
			}
		}
		RegCloseKey( hKey);
	}
	return false;
}

bool SetupResManParams()
{
	/*ATLASSERT( gpDispComMan);
	if( !gpDispComMan)		return false;*/
	//if( *glpcRegPath == 0)	return false;

	if (pOMSContext->mpComMan == NULL)
	{
		MessageBox(0, "SetupResManParams mpComMan = NULL", "Ошибка!", MB_OK);
	}

	if (wcscmp((const wchar_t*)glpcResServerConnectType,(const wchar_t*)("socket"))==0)
	{
		pOMSContext->mpComMan->SetConnectType(RES_CONNECT_SOCKET);
	}
	else
	{
		pOMSContext->mpComMan->SetConnectType(RES_CONNECT_HTTP);
	}

	USES_CONVERSION;
	int res = pOMSContext->mpComMan->PutPathIni( lpcIniFullPathName);
	if(res==E_FAIL)
	{
		SendSignal2Launcher( launchProcessID, APP_NEED_HIDE, 1);	
		MessageBox(0, _T("Некорректная установка программы.\nНеобходима регистрация параметров сервера."), _T("Error!"), MB_OK);
		//ATLASSERT( FALSE);
		return false;
	}

	// загружаем UI
	if(*glpcUIPath){
		CComBSTR bstrUIPath(glpcUIPath);
		pOMSContext->mpResM->SetUIBase(bstrUIPath);
		//gpWS3D->put_UIBase(bstrUIPath);
	}else{
		pOMSContext->mpResM->SetUIBase(L"UI");
		//gpWS3D->put_UIBase(L"UI");
	}

	if(*glpcRootPath)
	{
		pOMSContext->mpComMan->SetLocalBase(glpcRootPath);
	}
	else
	{
		wchar_t lpcModuleFileName[MAX_PATH+200];
		::GetModuleFileNameW( hInstance, lpcModuleFileName, MAX_PATH+150);
		wchar_t lpcExeFullPathName[MAX_PATH+200];
		LPWSTR lpFileName=NULL;
		::GetFullPathNameW(lpcModuleFileName,MAX_PATH+150,lpcExeFullPathName,&lpFileName);
		if(lpFileName!=NULL){
			*lpFileName=L'\0';
			pOMSContext->mpComMan->SetLocalBase(lpcExeFullPathName);
		}
	}	


	//MessageBox(0, "InitWS3DContinue 3", "Ошибка!", MB_OK);

	//pOMSContext->mp3DWS->initResManager();	перенесено в WS3D::createManagers
	if( pOMSContext->mpRM)
	{
		pOMSContext->mpRM->GetExtendedRM()->StartLoadingResource();
	}

	if (*glpcRepositoryPath == 0)
		wcscpy( glpcRepositoryPath, REPOSITORY_PATH);
	pOMSContext->mpComMan->SetRepositoryBase( glpcRepositoryPath);

	return true;
}

void SetScreenParams()
{
	ATLASSERT( pOMSContext->mp3DWS);
	pOMSContext->mp3DWS->SetLogAndTracerMode(gbUseLog, giSendLog, gbUseTracer, gbUseSyncLog);
	//ATLASSERT( SUCCEEDED( gpWS3D->SetScreenMode( dwClientWidth, dwClientHeight, 32, gbFullScreen)));
}

// Устанавливает размеры и положение окна плеера
void AdjustWindow( HWND ahWnd)
{
	ATLASSERT( ahWnd);
	CAxWindow	_window;
	_window.Attach( ahWnd);

	// если окно минимизировано, то перед изменением размеров окна, надо его сначала восстановить
	WINDOWPLACEMENT wndpl; wndpl.length = sizeof(WINDOWPLACEMENT);
	if(_window.GetWindowPlacement(&wndpl) && wndpl.showCmd == SW_SHOWMINIMIZED){
		//WINDOWPLACEMENT wndpl2; wndpl2.length = sizeof(WINDOWPLACEMENT);
		wndpl.showCmd = SW_RESTORE;
		_window.SetWindowPlacement(&wndpl);
	}

	RECT rcWnd = {0,0,dwClientWidth,dwClientHeight};
	HWND hDesktop = ::GetDesktopWindow();
	if( !gbFullScreen && hDesktop)
	{
		RECT rcDesktop;
		::GetWindowRect(hDesktop, &rcDesktop);
		rcDesktop.right = rcDesktop.right - rcDesktop.left;
		rcDesktop.bottom = rcDesktop.bottom - rcDesktop.top;
		rcDesktop.left = rcDesktop.top = 0;
		_window.ModifyStyle(0, WS_CAPTION | WS_BORDER | WS_SYSMENU);
	}

	::AdjustWindowRectEx(&rcWnd, _window.GetStyle(),
		(!(_window.GetStyle() & WS_CHILD) && (_window.GetMenu() != NULL)),
		_window.GetExStyle());
	_window.SetWindowPos(((!gbFullScreen) && (!gbTopWindow)) ? 0 : HWND_TOPMOST, 0, 0,
		rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, 
		SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_FRAMECHANGED);

	_window.MoveWindow(0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top);

	_window.CenterWindow();
	_window.RedrawWindow();

	_window.Detach();
}

void HideLauncherNow()
{
	std::string eventName = gcmParams->GetLauncherEventName();	

	HideLauncher(eventName.c_str());
	SetLauncherMDump(eventName.c_str());
}

CCrashHandler ch;

// после OnInitialized
bool InitWS3DContinue()
{
	// Переустановим обработчик событий после загрузки всех модулей. Это нужно чтобы краши отправлялись стабильнее	
	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();	

	SendPercent(65);

	//MessageBox(0, "InitWS3DContinue 1", "Ошибка!", MB_OK);
	
	HRESULT hr;
	CLSID clsid;
	CHAR lpcModuleFileName[MAX_PATH+200];
	CHAR lpcExeFullPathName[MAX_PATH+200];
	LPSTR lpFileName=NULL;
	BOOL bByPathOnly = FALSE;
	
	if(!SetupResManParams())
		return false;

	//MessageBox(0, "InitWS3DContinue 2", "Ошибка!", MB_OK);
	

	// Установка параметров экрана
	SetScreenParams();

	/*
	//Vladimir - передача командной строки в Cscl3DWS
	*/

	//MessageBox(0, "InitWS3DContinue 4", "Ошибка!", MB_OK);

	std::wstring wLoginVal;

	if( gcmParams->UserNameIsSet())
	{
		// Разбор параметров
		BSTR loginVal = CharToWide(gcmParams->GetUserName().c_str());
		//pOMSContext->mpSM->SetLogin( loginVal);
		wLoginVal = loginVal;
		MP_DELETE_ARR( loginVal);
	}	

	//MessageBox(0, "InitWS3DContinue 5", "Ошибка!", MB_OK);

	if( gcmParams->UrlIsSet())
	{
		// Разбор параметров
		BSTR urlVal = CharToWide(gcmParams->GetUrl().c_str());
		CComString sUrl = urlVal;		
		pOMSContext->mpSM->SetLocationUrl( urlVal);
		CUrlParser urlParser( urlVal);
		const wchar_t *pLoginVal = urlParser.GetParam(L"login", NULL);
		if( pLoginVal != NULL && wcslen(pLoginVal)>0)
			wLoginVal = pLoginVal;
		const wchar_t *pLoginKeyVal = urlParser.GetParam(L"loginKey", NULL);
		if( pLoginKeyVal != NULL && wcslen(pLoginKeyVal)>0)
			pOMSContext->mpSM->SetLoginKey( pLoginKeyVal);
		CComString sStartupParams = "[STARTUP PARAMETERS] vacademia with url = '";
		sStartupParams += gcmParams->GetUrl().c_str(); sStartupParams += "'";
		if( pOMSContext->mpLogWriter != NULL)
			pOMSContext->mpLogWriter->WriteLnLPCSTR( sStartupParams.GetBuffer());
		if( pLoginKeyVal != NULL && pOMSContext->mpLogWriter != NULL)
			pOMSContext->mpLogWriter->WriteLnLPCSTR( "[STARTUP PARAMETERS] vacademia with loginKey");
		if( pOMSContext->mpLogWriter != NULL)
			
		MP_DELETE_ARR( urlVal);
	}
	
	if( gcmParams->UpdaterKeyIsSet())
	{
		std::wstring updaterKeyVal = CharToWide(gcmParams->GetUpdaterKey().c_str());
		if( updaterKeyVal.size()>0)
			pOMSContext->mpSM->SetUpdaterKey( updaterKeyVal.c_str());
	}

	if( wLoginVal.size()>0)
		pOMSContext->mpSM->SetLogin( wLoginVal.c_str());

	if( gcmParams->PasswordIsSet())
	{
		// Разбор параметров
		BSTR pswdVal = CharToWide(gcmParams->GetPassword().c_str());
		pOMSContext->mpSM->SetPassword( pswdVal);
		MP_DELETE_ARR( pswdVal);
	}

	if( gcmParams->PasswordEncodingModeIsSet())
	{
		// Разбор параметров
		int iPasswordEncodingMode = gcmParams->GetPasswordEncodingMode();
		pOMSContext->mpSM->SetPasswordEncoding( iPasswordEncodingMode);		
	}


	/*
	//end Vladimir
	*/

	// если тестовый режим передаем в InfoMan 
	if( gcmParams->TestModeIsSet())
	{
		// Разбор параметров
		int testMode = gcmParams->GetTestMode();
		if( /*testMode == 7531*/testMode > 0 && pOMSContext->mpInfoMan != NULL)
		{
			pOMSContext->mpInfoMan->CreateWnd( (unsigned long)ghWnd);
			pOMSContext->mpInfoMan->SetAutoTestMode( testMode);
		}
		if( gcmParams->TestResDirIsSet())
		{
			BSTR pswdVal = CharToWide(gcmParams->GetTestResDir().c_str());
			pOMSContext->mpSM->SetTestResDir( pswdVal);
			MP_DELETE_ARR( pswdVal);
		}
	}
	/////////////////
	SendPercent(73);
	pOMSContext->mp3DWS->loadUI(L"");

	SendPercent(99);
	
	if (*glpcUserPath == 0)
		wcscpy( glpcUserPath, USER_PATH);
	pOMSContext->mpResM->SetUserBase( glpcUserPath);
	
	HideLauncherNow();

	Sleep( 1000);

	if( !gbTitleIsSet)
		SetTimer( ghWnd, IDT_FPS, 1000, NULL);

	// загружаем модуль и сцену (если указаны в командной строке)
	if(gcmParams->ModuleIsSet()){
		pOMSContext->mp3DWS->loadModule(CharToWide(gcmParams->GetModule().c_str()), 
										CharToWide(gcmParams->GetScene().c_str()));
	}else if(gcmParams->SceneIsSet()){
		pOMSContext->mp3DWS->loadScene(CharToWide(gcmParams->GetScene().c_str()));
		// ??
	}	
	/*
	end Vladimir
	*/
	// ??

	WTSRegisterSessionNotification(ghWnd, NOTIFY_FOR_ALL_SESSIONS);

	return true;
}

void DestroyWS3D(){
	if (pOMSContext->mp3DWS)
	{
		pOMSContext->mp3DWS->PrepareForDestroy();
	}

	if( pOMSContext->mpServiceWorldMan != NULL)
	{
		// wait 5 seconds to receive code
		int k = 0;
		while( !pOMSContext->mpServiceWorldMan->IsExitCodeReceived())
		{
			Sleep(1000);
			if( (k++) == 5)
				break;
		}
	}

	g_reportMan.terminate();
	g_reportMan.setServiceWorldManager( NULL);
	SendSignal2Launcher( launchProcessID, APP_NOTIFY, 1);
	SendSignal2Launcher( launchProcessID, APP_END, 1);	

	if (g_hangMemoryObject)
	{
		CloseHandle(g_hangMemoryObject);
	}
	
	Sleep(5000);

	Destroy3DWSManager(pOMSContext);
}

bool FileExists(const wchar_t* alpcFileName){
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = FindFirstFileW(alpcFileName, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hFind);
	return true;
}

bool ParseINI(HINSTANCE ahInstance)
{
	lpcIniFullPathName[0] = 0;

	// Getting execute full name
    wchar_t lpcStrBuffer[MAX_PATH*2+2];	lpcStrBuffer[0] = 0;

	::GetModuleFileNameW( ahInstance, lpcStrBuffer, MAX_PATH);


	if( lpcStrBuffer[0] == 0)
		return false;


	SECFileSystem fs;
	CWComString sExePath = fs.GetPath( CWComString( lpcStrBuffer), TRUE);
	CWComString sExecutableFileName = fs.GetFileName( CWComString( lpcStrBuffer));
 	CWComString sIniFilePath;

	sIniFilePath = fs.AppendWildcard( sExePath, L"player.ini");
	if( FileExists(sIniFilePath))
	{
		wcscpy(lpcIniFullPathName,sIniFilePath.GetBuffer());
	}
	else
	{
		if( gcmParams->RootIsSet())
		{
			USES_CONVERSION;
			if( IsFullPath( gcmParams->GetRoot().c_str()))
				sIniFilePath = fs.AppendWildcard(gcmParams->GetRoot().c_str(), L"player.ini");
			else
				sIniFilePath = fs.AppendWildcard( fs.AppendPaths(sExePath, gcmParams->GetRoot().c_str()), L"player.ini");
			if( FileExists( sIniFilePath))
			{
				wcscpy(lpcIniFullPathName,sIniFilePath.GetBuffer());
			}
		}
	}

	if( lpcIniFullPathName[0] == 0)
	{
		MessageBox(0, "Не найден файл player.ini", "Ошибка!", MB_OK);
		return false;
	}

	if (GetPrivateProfileStringW( L"settings", L"sleep", L"0", lpcStrBuffer, MAX_PATH, lpcIniFullPathName))
	{
		g_sleep = _wtoi(lpcStrBuffer);
	}

	USES_CONVERSION;
	// If param < -root=... > exists
	if( gcmParams->RootIsSet())
	{
		wcscpy(glpcRootPath, gcmParams->GetRoot().c_str());
	}
	// else get paths from INI-file
	else if (GetPrivateProfileStringW( L"paths", L"root", L"", lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) 
	{		
		wcscpy(glpcRootPath, lpcStrBuffer);
	}

	int ii = 0;

	if( glpcRootPath[0] != 0)
	{
		// добавим в конец СЛЕШ
		for(ii=0; glpcRootPath[ii]; ii++);
		if(glpcRootPath[ii-1]!=L'\\' && glpcRootPath[ii-1]!=L'/'){
			glpcRootPath[ii]=L'\\'; glpcRootPath[ii+1]=0;
		}

		// если это не полный путь, т.е. путь относительно exe-шника
		if(*glpcRootPath != L'\0' && 
			((glpcRootPath[0] != L'\\' || glpcRootPath[1] != L'\\') 
			&& glpcRootPath[1] != L':'))
		{
			LPWSTR pTemp = NULL;
			wchar_t lpcRootPath[MAX_PATH*2+2];
			wcscpy(lpcRootPath, sExePath.GetBuffer());
			wcscat(lpcRootPath, glpcRootPath);
			::GetFullPathNameW( lpcRootPath, MAX_PATH*2, glpcRootPath, &pTemp);
			for(ii=0; glpcRootPath[ii]; ii++);
			if(glpcRootPath[ii-1]!=L'\\' && glpcRootPath[ii-1]!=L'/'){
				glpcRootPath[ii]=L'\\'; glpcRootPath[ii+1]=0;
			}
		}
	}

	if (GetPrivateProfileStringW( L"paths", L"repository", REPOSITORY_PATH, lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) {
		wcscpy(glpcRepositoryPath,lpcStrBuffer);
		for(ii=0; glpcRepositoryPath[ii]; ii++);
		if(glpcRepositoryPath[ii-1]!=_T('\\') && glpcRepositoryPath[ii-1]!=_T('/')){
			glpcRepositoryPath[ii]=_T('\\');
			glpcRepositoryPath[ii+1]=0;
		}
	}

	if (GetPrivateProfileStringW( L"paths", L"regkey", L"", lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) {
		_tcscpy(glpcRegPath, W2A(lpcStrBuffer));
		for(ii=0; glpcRegPath[ii]; ii++)
		{
			if(glpcRegPath[ii]==_T('\\') && glpcRegPath[ii]==_T('/'))
			{
				ATLASSERT( FALSE);	// пока можно задавать только имя папки
			}
		}
	}
	if (GetPrivateProfileStringW( L"paths", L"user", USER_PATH, lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) {
		wcscpy(glpcUserPath,lpcStrBuffer);
		for(ii=0; glpcUserPath[ii]; ii++);
		if(glpcUserPath[ii-1]!=_T('\\') && glpcUserPath[ii-1]!=_T('/')){
			glpcUserPath[ii]=_T('\\'); glpcUserPath[ii+1]=0;
		}
	}
	if (GetPrivateProfileStringW( L"paths", L"ui", L"", lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) {
		_tcscpy(glpcUIPath, W2A(lpcStrBuffer));
		for(ii=0; glpcUIPath[ii]; ii++);
		if(glpcUIPath[ii-1]!=_T('\\') && glpcUIPath[ii-1]!=_T('/')){
			glpcUIPath[ii]=_T('\\'); glpcUIPath[ii+1]=0;
		}
	}
	if (GetPrivateProfileStringW( L"paths", L"components", L"", lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) {
		_tcscpy(glpcComponentsPath, W2A(lpcStrBuffer));
		for(ii=0; glpcComponentsPath[ii]; ii++);
		if(glpcComponentsPath[ii-1]!=_T('\\') && glpcComponentsPath[ii-1]!=_T('/')){
			glpcComponentsPath[ii]=_T('\\'); glpcComponentsPath[ii+1]=0;
		}
	}

	gbUseLog = (GetPrivateProfileIntW( L"settings", L"use_log", 0, lpcIniFullPathName) != 0);
	giSendLog = GetPrivateProfileIntW( L"settings", L"send_log", 0, lpcIniFullPathName);
	gbUseTracer = (GetPrivateProfileIntW( L"settings", L"use_tracer", 0, lpcIniFullPathName) != 0);
	gbUseSyncLog = (GetPrivateProfileIntW( L"settings", L"use_sync_log", 0, lpcIniFullPathName) != 0);
	giEmulateNetworkLags = GetPrivateProfileIntW( L"settings", L"network_breaks_delay", 0, lpcIniFullPathName);

	gbFullScreen = (GetPrivateProfileIntW( L"screen", L"fullScreen", 0, lpcIniFullPathName) != 0);
	dwClientWidth = GetPrivateProfileIntW( L"screen", L"width", dwClientWidth, lpcIniFullPathName);
	dwClientHeight = GetPrivateProfileIntW( L"screen", L"height", dwClientHeight, lpcIniFullPathName);
	giMaxFPS = GetPrivateProfileIntW( L"screen", L"maxfps", 30, lpcIniFullPathName);
	giMaxFPS = giMaxFPS==0?30:giMaxFPS;
	if( GetPrivateProfileStringW( L"window", L"title", glpcWindowTitle, glpcWindowTitle, 2000, lpcIniFullPathName) != 0)
	{
		gbTitleIsSet = true;
	}

	if (GetPrivateProfileStringW( L"DownloadManager", L"path", L"", lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) {
		glpcDownloadManager[ 0] = 0;
		if (lpcStrBuffer[ 1] != L':')
			wcscpy( glpcDownloadManager, sExePath.GetBuffer());
		wcscat( glpcDownloadManager, lpcStrBuffer);
	}

	if (GetPrivateProfileStringW( L"settings", L"resserver", L"", lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) 
	{
		_tcscpy(glpcResServerConnectType, W2A(lpcStrBuffer));
	}
	if (GetPrivateProfileStringW( L"settings", L"logserver", L"", lpcStrBuffer, MAX_PATH, lpcIniFullPathName)) 
	{
		_tcscpy(glpcLogServerConnectType, W2A(lpcStrBuffer));
	}

	return true;
}

void ParseCmdLine(LPSTR alpCmdLine)
{	
	// разбираем все аргументы командной строки
	gcmParams = MP_NEW(CCommandLineParams);
	if (alpCmdLine)
	{
		gcmParams->SetCommandLine( alpCmdLine);
	}
}

void SetCommandLine(char* cmdLine)
{
	glpCmdLine = cmdLine;
}

LONG WINAPI OMSPlayerFilterFunction( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	CoInitialize(NULL);

	LONG result = MDumpFilterFunction( pExceptionInfo);
    if( result == EXCEPTION_VA_RUNTIME_MODULE)
	{
		//if( pOMSContext && pOMSContext->mpVoipMan)
		result = EXCEPTION_CONTINUE_SEARCH;
	}
	else
	{
		rtl_TerminateMyProcess( 1);
		CoUninitialize();
	}

	return result;
}

void SetResourceInstance(HINSTANCE hInst)
{
	hInstance = hInst;
}

static DWORD WINAPI checkDesktopValidThread(LPVOID param)
{
	while (bCheckDesktopValid)
	{
		HDESK hdesk = OpenInputDesktop( NULL, FALSE, GENERIC_READ);
		if( hdesk == NULL)
		{
			int gg = GetLastError();
			if (gg == ERROR_INVALID_PARAMETER || gg == ERROR_ACCESS_DENIED)
			{
				pOMSContext->mpLogWriter->WriteLn("Checking desktop - minimize window");
				ShowWindow(ghWnd, SW_MINIMIZE);				
			}
		}
		else
		{
			CloseHandle(hdesk);
		}

		Sleep(100);
	}
	return 0;
}

void DoInitializationAndLoading(HWND ahWnd)
{
	SendPercent(1);
	RemoveEvopVOIPTrial();

	// Установим обработчик событий после загрузки всех модулей. Это нужно чтобы краши отправлялись стабильнее.
	// В дальнейшем после загрузки основных модулей поставим его еще раз.
	//StartExceptionHandlers();//( &OMSPlayerFilterFunction);
	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();

	CoInitialize(NULL);

 	hOMSPlayerProcessHandle = GetCurrentProcess();

	SetAppUserModelId();
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OMSPLAYER, szWindowClass, MAX_LOADSTRING);

	TCHAR title[30] = _T("VSpaces virtual worlds player"); 
	wcscpy((wchar_t*)&szWindowClass, (wchar_t*)title);
	MyRegisterClass(hInstance);

	ParseCmdLine(glpCmdLine);	

	if( !ParseINI(hInstance))
	{
		CoUninitialize();
		return;
	}

	// Perform application initialization:
	if( ghWnd == 0)
	{
		SendPercent(2);
		if (!InitInstance (hInstance, SW_SHOW)) 
		{
			CoUninitialize();
			return;
		}	
	}
	else
	{
		//MessageBox(0, "1","", MB_OK);
		if (!InitWS3D(ghWnd))
		{
			MessageBox(0, "2","", MB_OK);
			bUserExit = true;
			PostQuitMessage(0);
			CoUninitialize();
			return;
		}

		//MessageBox(0, "3","", MB_OK);

		//AdjustWindow(ghWnd);
		DragAcceptFiles(ghWnd, TRUE);

		SendSignal2Launcher( launchProcessID, APP_PERCENT, 100);	
		SetForegroundWindow(ghWnd);
	}

	KillOtherPlayerProcesses(hInstance);	
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_OMSPLAYER);	

	if (pOMSContext)
	{
		if (pOMSContext->mpLogWriter)
		{
			pOMSContext->mpLogWriter->WriteLn("Command line: ", glpCmdLine);
		}
	}
}

void StartCheckDesktop()
{
	CSimpleThreadStarter recvThreadStarter(__FUNCTION__);	
	recvThreadStarter.SetRoutine(checkDesktopValidThread);
	recvThreadStarter.SetParameter(NULL);
	HANDLE hCheckDesktopValidThread = recvThreadStarter.Start();

	if (pOMSContext)
	{
		if (pOMSContext->mpLogWriter)
		{
			pOMSContext->mpLogWriter->WriteLn("Start checking desktop");
		}
	}
}

bool DoFrame()
{
	if( !pOMSContext)
		return false;

	if( !pOMSContext->mp3DWS)
		return false;	

	return pOMSContext->mp3DWS->Update(0);
}

void DoEvent(int e, int p1, int p2)
{
	//MessageBox(0,"DoEvent","DoEvent",0);
	if (ghWnd)
		WndProc(ghWnd, e, p1, p2);
}

void DoMainLoop()
{
	MSG msg;
	
	_SYSTEM_INFO nfo;
	GetSystemInfo(&nfo);
	int coreCount = nfo.dwNumberOfProcessors;

	const int bigReduceSpeedTime = 15000;
	const int reduceSpeedTime = 25000;
	int lastExecuteTime = GetTickCount() - reduceSpeedTime - 1;

	// Main message loop:
	DWORD	dwMaxSleepTime = 1000/giMaxFPS;
	DWORD	dwTime = timeGetTime();
	DWORD	dwFrameTime = 0;
	DWORD	count___ = 0;

	DisableScreensaver();

	while (true) 
	{
		dwTime = timeGetTime();
		while( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{				
				TranslateMessage(&msg);
				DispatchMessage(&msg);						
			}
			if( (msg.message == WM_QUIT || msg.message == WM_CLOSE) && (msg.hwnd == ghWnd || msg.hwnd == 0))
			{
				SendSignal2Launcher( launchProcessID, APP_CLOSED_BY_OTHER_APP, 1);	
				pOMSContext->mpLogWriter->WriteLn("Correct exit (WM_QUIT / WM_CLOSE)!");
				bExit = true;
				break;
			}	
		}

		if( bUserExit && !pOMSContext->mp3DWS->IsExecAnySysCommand())	
		{
			break;
		}

		//SetFocus(ghWnd);

		SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);

		bExit = DoFrame();
		OnFrameEnd();

		if (coreCount == 1)
		{
			int delta = GetTickCount() - lastExecuteTime;
			if (pOMSContext->mp3DWS->IsExecAnySysCommand())			
			{
				Sleep(50);
				lastExecuteTime = GetTickCount();	
			}
			else if (delta < reduceSpeedTime)
			{
				if (delta < bigReduceSpeedTime)
				{
					Sleep(45);
				}
				else
				{
					Sleep(25);
				}
			}
		}

		if( gcmParams->DebugIsSet())
			Sleep( 50);

		if( bExit)
			bUserExit = true;

		if( bExit && !pOMSContext->mp3DWS->IsExecAnySysCommand())	
		{
			if (pOMSContext)
			{
				pOMSContext->mpLogWriter->WriteLn("Correct exit (user exit)");
			}
			break;
		}		

		if (g_sleep != 0)
		{
			Sleep(g_sleep);
		}
	}

	EnableScreensaver();
}

void DoQuit()
{	
	while (pOMSContext->mp3DWS->IsExecAnySysCommand())
	{
		Sleep(1);
	}
#ifdef CHECK_CUR_DESKTOP_VALID
	bCheckDesktopValid = false;
#endif

	SetAppFinishing();
	SendSignal2Launcher( launchProcessID, APP_FINISHING, 1);	

	ShowWindow(ghWnd, SW_HIDE);
	DeleteRunLogFileForLauncher();
	UpdateWindow(ghWnd);

	DestroyWS3D();
	ghWnd=NULL;

	std::string eventName = gcmParams->GetLauncherEventName();
	HideLauncher(eventName.c_str());	
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= /*CS_HREDRAW | CS_VREDRAW*/0 | CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_OMSPLAYER);
	//wcex.hIcon			= (HICON)LoadImage(hInstance, (LPCTSTR)IDI_OMSPLAYER, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; // (LPCSTR)IDC_OMSPLAYER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	//wcex.hIconSm		= (HICON)LoadImage(hInstance, (LPCTSTR)IDI_SMALL, IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);

	return RegisterClassEx(&wcex);
}

char tmp[100];
		char tmp2[100];

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{	
	ghWnd = NULL;
	USES_CONVERSION;
	if (gbFullScreen)
	{		
		ghWnd = CreateWindowW( A2W(szWindowClass), glpcWindowTitle, WS_POPUP, 0, 0, 1024, 768, NULL, NULL, hInstance, NULL);
	}
	else
	{
		int iCap=::GetSystemMetrics(SM_CYCAPTION);

		int iXF=::GetSystemMetrics(SM_CXFIXEDFRAME);
		int iYF=::GetSystemMetrics(SM_CYFIXEDFRAME);
		int iXScr=::GetSystemMetrics(SM_CXFULLSCREEN);
		int iYScr=::GetSystemMetrics(SM_CYFULLSCREEN);
/*		ghWnd = CreateWindow(szWindowClass, glpcWindowTitle, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
				CW_USEDEFAULT, CW_USEDEFAULT, 1024+iXF*2, 768+iCap+iYF*2, NULL, NULL, hInstance, NULL);*/
		ghWnd = CreateWindowExW(0, A2W(szWindowClass), glpcWindowTitle, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_EX_APPWINDOW, 0, 0, 
			1024, 768, NULL, NULL, hInstance, NULL);
	}
	if (!ghWnd)
		return FALSE;
	ShowWindow(ghWnd, nCmdShow);
	UpdateWindow(ghWnd);
	::SetFocus(ghWnd);

	return TRUE;
}

HANDLE hMapObject = NULL;

HWND	GetRenderHWND()
{
	//CComQIPtr<IOleWindow>	pOle = gpWS3D;
//	ATLASSERT( pOle);
	//HWND	hwndControl;
	/*if( pOle && SUCCEEDED( pOle->GetWindow( &hwndControl)))
		return hwndControl;
	return 0;*/
	return ghWnd;
}

static bool bWindowMoving = false;
static bool bFullScreenActivated = false;
bool bSessionIDSendNeed = false;

void SendSessionIDToLauncher(long sessionID)
{
	short* a = (short*)&sessionID;
	SendSignal2Launcher( launchProcessID, APP_SESSION, a[0]);
	SendSignal2Launcher( launchProcessID, APP_SESSION_2, a[1]);

	pOMSContext->mpLogWriter->WriteLn("(SESSION LONG ) ", sessionID);
	pOMSContext->mpLogWriter->WriteLn("(APP_SESSION ) " ,a[0]);
	pOMSContext->mpLogWriter->WriteLn("(APP_SESSION_2 )" , a[1]);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#define WM_WTSSESSION_CHANGE 0x02B1 
#define WTS_SESSION_LOCK                0x7
#define WTS_SESSION_UNLOCK              0x8

	int wmId, wmEvent;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInstance, IDS_HELLO, szHello, MAX_LOADSTRING);
	BOOL handled = TRUE;		

	switch (message) 
	{
		case WM_LAUNCHER_WM_MESSAGE:
		{
			int lId = LOWORD(lParam);
			int wIdH = HIWORD(wParam); 
			int wIdL = LOWORD(wParam);
			int wId = (wIdH << 16) | wIdL;	
			if ( lId == APP_NOTIFY)
			{
				launchProcessID = wId;
				SendSignal2Launcher( launchProcessID, APP_NOTIFY, lId);
				if ( bSessionIDSendNeed)
				{
					long sessionID = pOMSContext->mpComMan->GetSessionID();//pOMSContext->mp3DWS->GetSessionID();
					SendSessionIDToLauncher(sessionID);
					bSessionIDSendNeed = false;
				}
			}
			else	if ( lId == APP_SESSION && wId == 1)
			{
				long sessionID = pOMSContext->mpComMan->GetSessionID();//pOMSContext->mp3DWS->GetSessionID();
				if ( launchProcessID > 0 && sessionID > 0)
				{
					SendSessionIDToLauncher(sessionID);
					//SendSignal2Launcher( launchProcessID, APP_SESSION, sessionID);					
					bSessionIDSendNeed = false;
				}
				else if ( sessionID > 0)
					bSessionIDSendNeed = true;
			}
			else	if ( lId == APP_PERCENT)// && wId == 2)
			{
				long percent = wId;//pOMSContext->mp3DWS->GetSessionID();
				if ( launchProcessID > 0)
				{
					SendPercent(percent);					
					bSessionIDSendNeed = false;
				}
			}
		}break;

		case WM_POWERBROADCAST:
			switch (wParam)
			{
				case (PBT_APMBATTERYLOW):
					if ((pOMSContext) && (pOMSContext->mpLogWriter))
					{
						pOMSContext->mpLogWriter->WriteLn("[CRITICAL-WARNING] low battery");
					}					
					break;

				case (PBT_APMQUERYSUSPEND):
					if ((pOMSContext) && (pOMSContext->mpLogWriter))
					{
						pOMSContext->mpLogWriter->WriteLn("[CRITICAL-WARNING] Request for permission to suspend - vAcademia answered BROADCAST_QUERY_DENY");
					}					
					return BROADCAST_QUERY_DENY;
					break;

				case (PBT_APMRESUMECRITICAL):
					if ((pOMSContext) && (pOMSContext->mpLogWriter))
					{
						pOMSContext->mpLogWriter->WriteLn("[CRITICAL-WARNING] Resuming from critial suspend due to low battery");						
					}
					break;

				case (PBT_APMSUSPEND):
					if ((pOMSContext) && (pOMSContext->mpLogWriter))
					{
						pOMSContext->mpLogWriter->WriteLn("[CRITICAL-WARNING] System is suspending operation");
					}
					break;
			}

		case WM_NCRBUTTONDOWN:
			SetSharedMemoryValue(0x77777777);
			break;

		case WM_NCRBUTTONUP:
			OnFrameEnd();
			break;

		case WM_LBUTTONDBLCLK:
			pOMSContext->mp3DWS->OnLMouseDblClk(message, wParam, lParam, &handled);
			break;

		case WM_GETICON:
			OnFrameEnd();
			break;

		case WM_MOVE:
			if (pOMSContext)
			{				
				pOMSContext->mp3DWS->OnWindowMoving();
			}
			break;

		case WM_RBUTTONDBLCLK:
			pOMSContext->mp3DWS->OnRMouseDblClk(message, wParam, lParam, &handled);
			break;

		case WM_LBUTTONDOWN:
			::SetCapture(ghWnd);
			pOMSContext->mp3DWS->OnLButtonDown(message, wParam, lParam, &handled);
			break; 

		case WM_LBUTTONUP:
			::ReleaseCapture();
			pOMSContext->mp3DWS->OnLButtonUp(message, wParam, lParam, &handled);
			break;

		case WM_MOUSEMOVE:
			pOMSContext->mp3DWS->OnMouseMove(message, wParam, lParam, &handled);
			break;

		case WM_WTSSESSION_CHANGE:
			if (WTS_SESSION_LOCK == wParam)
			{
				if (pOMSContext)
				{
					if (pOMSContext->mpLogWriter)
					{
						pOMSContext->mpLogWriter->WriteLn("Session locked");
						ShowWindow(ghWnd, SW_MINIMIZE);
					}
				}
			}
			else if (WTS_SESSION_UNLOCK == wParam)
			{
				if (pOMSContext)
				{
					if (pOMSContext->mpLogWriter)
					{
						pOMSContext->mpLogWriter->WriteLn("Session unlocked");
						ShowWindow(ghWnd, SW_RESTORE);
					}
				}
			}
			else
			{
				if (pOMSContext)
				{
					if (pOMSContext->mpLogWriter)
					{
						pOMSContext->mpLogWriter->WriteLn("Session unknown action: ", wParam);
						ShowWindow(ghWnd, SW_RESTORE);
					}
				}
			}
			break;

		case WM_RBUTTONDOWN:
			::SetCapture(ghWnd);
			pOMSContext->mp3DWS->OnRButtonDown(message, wParam, lParam, &handled);
			break;

		case WM_RBUTTONUP:
			::ReleaseCapture();
			pOMSContext->mp3DWS->OnRButtonUp(message, wParam, lParam, &handled);
			break;

		case WM_MBUTTONDOWN:
			pOMSContext->mp3DWS->OnMButtonDown(message, wParam, lParam, &handled);
			break;

		case WM_MBUTTONUP:
			pOMSContext->mp3DWS->OnMButtonUp(message, wParam, lParam, &handled);
			break;

		case WM_KEYDOWN:
#ifdef TraceModulesHeaps
			if( (GetKeyState(VK_CONTROL) && (wParam=='J' || wParam == 'j')))
			{
				yoyo = true;
			}
#endif
			pOMSContext->mp3DWS->OnKeyDown(WM_KEYDOWN, wParam, lParam, &handled);
			break;

		case WM_INTEL_VIDEO_FOUND:
			g_intel = true;
			StartCheckDesktop();
			break;

		case WM_NVIDIA_VIDEO_FOUND:
			g_nvidia = true;			
			break;

		case WM_HIDE_LAUNCHER_NOW:
			HideLauncherNow();
			break;

		case WM_SYSKEYDOWN:
			pOMSContext->mp3DWS->OnKeyDown(WM_KEYDOWN, wParam, lParam, &handled);
			handled = FALSE;
			break;

		case WM_KEYUP:
			pOMSContext->mp3DWS->OnKeyUp(WM_KEYUP, wParam, lParam, &handled);
			break;

		case WM_SYSKEYUP:
			pOMSContext->mp3DWS->OnKeyUp(WM_KEYUP, wParam, lParam, &handled);
			handled = FALSE;
			break;

		case WM_SYSCOMMAND:
			handled = FALSE;
			/*if (g_nvidia)
			{*/
				switch (wParam)
				{
					case SC_SCREENSAVE:  						
						return 0;
					case SC_MONITORPOWER:						
						return 0;      
				}
			//}
			if ((wParam == SC_MINIMIZE) && (g_intel))
			{
				handled = TRUE;
			}
			break;

		case WM_SETCURSOR:
			pOMSContext->mp3DWS->OnSetCursor(message, wParam, lParam, &handled);
			break;

		case 0x020A:
			pOMSContext->mp3DWS->OnMouseWheel(message, wParam, lParam, &handled);
			break;

		case WM_KILLFOCUS:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;

		case WM_SETFOCUS: {
			if (pOMSContext && pOMSContext->mpRM)
			if (pOMSContext->mpRM->GetCodeEditorSupport())
			{
				pOMSContext->mpRM->GetCodeEditorSupport()->SetNewProgramText();
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
			} break;

		case WM_CREATE:{
			ghWnd = hWnd;
			LPCREATESTRUCT pCrtStr=(LPCREATESTRUCT)lParam;
			LRESULT lRes = DefWindowProc(hWnd, message, wParam, lParam);
			if (!InitWS3D(hWnd))
			{
				bUserExit = true;
				PostQuitMessage(0);
				break;
			}
			AdjustWindow(hWnd);

			DragAcceptFiles(hWnd, TRUE);

			SendPercent(100);

			SetForegroundWindow(ghWnd);

			return lRes;
			}break;
		case WM_DROPFILES:
			{
				HDROP fDrop = (HDROP)wParam;
				wchar_t fName[2000];
				fName[0] = 0;
				DragQueryFileW(fDrop, 0, (WCHAR*)&fName[0], 2000);
				////MessageBox(NULL,fName,TEXT("Dragged fileName"),MB_OK);	
				DragFinish(fDrop);
				pOMSContext->mp3DWS->OnFileDrop(fName);
			}break;
		case WM_ACTIVATEAPP:{
			bool	activated = (bool)wParam;
			if ( activated)
			{
				//SetWindowPos(ghWnd, !gbFullScreen ? 0 : HWND_TOPMOST, 0, 0,
				//	0, 0, 
				//	SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_FRAMECHANGED);
				
				//SetForegroundWindow(ghWnd);

				RECT rcWnd = {0,0,dwClientWidth,dwClientHeight};
				HWND hDesktop = ::GetDesktopWindow();

				RECT rct, rctAll;
				GetClientRect(hWnd, &rct);
				GetWindowRect(hWnd, &rctAll);

				// определяем высоту окна с заголовком
				DWORD adwHeight = dwClientHeight + ((rctAll.bottom - rctAll.top) - (rct.bottom - rct.top));

				if ( hDesktop)
				{
					RECT rcDesktop;
					::GetWindowRect(hDesktop, &rcDesktop);
					rcDesktop.right = rcDesktop.right - rcDesktop.left;
					rcDesktop.bottom = rcDesktop.bottom - rcDesktop.top;
					rcDesktop.left = rcDesktop.top = 0;

					int toolbarOffsetX, toolbarOffsetY, toolbarWidth, toolbarHeight;
					pOMSContext->mpApp->GetToolbarParams(toolbarOffsetX, toolbarOffsetY, toolbarWidth, toolbarHeight);
					
					// проверяем: окно имеет максимальное разрешение и активация была?
					if ( !bFullScreenActivated && ((dwClientWidth + toolbarWidth) >= rcDesktop.right) || ((adwHeight + toolbarHeight) >= rcDesktop.bottom))
					{
						if ( !ghWnd)	return 0;
						CAxWindow	axParent;	
						axParent.Attach(ghWnd);						

						bFullScreenActivated = true;

						// Поставим заголовок окна
						axParent.ModifyStyle(0, WS_CAPTION | WS_BORDER | WS_SYSMENU);

						// если окно минимизировано, то перед изменением размеров окна, надо его сначала восстановить
						WINDOWPLACEMENT wndpl; wndpl.length = sizeof(WINDOWPLACEMENT);
						if ( axParent.GetWindowPlacement(&wndpl) && wndpl.showCmd == SW_SHOWMINIMIZED)
						{
							wndpl.showCmd = SW_RESTORE;							
						}

						::AdjustWindowRectEx(&rcWnd, axParent.GetStyle(),
										(!(axParent.GetStyle() & WS_CHILD) && (axParent.GetMenu() != NULL)),
										axParent.GetExStyle());

						axParent.SetWindowPos(((!gbFullScreen) && (!gbTopWindow)) ? 0 : HWND_TOPMOST, 0, 0,
									rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, 
									SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_FRAMECHANGED);

						if (pOMSContext->mpRM)
						if ((pOMSContext->mpRM->IsCaveMode()) && (rcWnd.right - rcWnd.left > 512))
						{
							int offsetX, width, height;
							pOMSContext->mpApp->GetWideDesktopParams(width, height, offsetX);
							BOOL bRet = axParent.MoveWindow(offsetX, 0, width, height);
						}
						else
						{
							int offsetX = 0, displayWidth = 0, displayHeight = 0;
							pOMSContext->mpApp->GetDesktopSize(offsetX, displayWidth, displayHeight);
							int windowWidth = rcWnd.right - rcWnd.left;
							int windowHeight = rcWnd.bottom - rcWnd.top;						
							int windowOffsetX = (int)((displayWidth - toolbarWidth - windowWidth) / 2) + toolbarOffsetX;
							int windowOffsetY = (int)((displayHeight - toolbarHeight - windowHeight) / 2) + toolbarOffsetY;						
							BOOL bRet = axParent.MoveWindow(0 + offsetX + windowOffsetX, 0 + windowOffsetY, windowWidth, windowHeight);
						}

						axParent.RedrawWindow();

						axParent.Detach();
					}
				}

				SetFocus(ghWnd);				
				SetForegroundWindow(ghWnd);
			}
			else
				bFullScreenActivated = false;
			if( pOMSContext->mp3DWS)
				pOMSContext->mp3DWS->OnActivateApp(activated);
			return 1;
		} break;

		case WM_QUERYENDSESSION:
			if (pOMSContext)
			{
				pOMSContext->mpLogWriter->WriteLn("Incorrect exit (WM_QUERYENDSESSION)");
				pOMSContext->mpServiceWorldMan->SetSessionStateValue( service::CSS_EXIT_CODE, 11);
			}
			SendSignal2Launcher( launchProcessID, APP_WORK, 11);
			DeleteRunLogFileForLauncher();
			Sleep(15000);
			return 1;
			break;

		case WM_DESTROY:
			//gpActiveXHostWnd->Detach();
			if (pOMSContext)
			{
				pOMSContext->mpLogWriter->WriteLn("Correct exit (WM_DESTROY)");
			}
			PostQuitMessage(0);
			break;

		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:					
				   //DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   SendSignal2Launcher( launchProcessID, USER_WND_CLOSE, 1);
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_WINDOWPOSCHANGED:
			if( ghWnd && GetRenderHWND()!=0 && !gbFullScreen)
			{
				RECT	rcClient;
				GetClientRect(ghWnd, &rcClient);
				//MoveWindow( GetRenderHWND(), 0, 0, rcClient.right, rcClient.bottom, TRUE);
			}
			handled = false;
			break;
		case WM_OCULUS_WANTS_TO_CHANGE_THE_RESOLUTION:
			gbUseOculus = true;
			break;
		case WM_SIZE:{
			if (gbUseOculus) { 
				//просто не мешаем поставить любое разрешение
				gbUseOculus = false;
			} 
			else {
				if (SIZE_MINIMIZED == wParam)
				{
					if ((pOMSContext) && (pOMSContext->mpLogWriter))
					{
						pOMSContext->mpLogWriter->WriteLn("Window is minimized");
					}
					handled = true;
					break;
				}

				// если окно было минимизировано или максимизировано, а теперь восстановлено до нормального размера
				if(wParam == SIZE_RESTORED) {

					if ((pOMSContext) && (pOMSContext->mpLogWriter))
					{
						pOMSContext->mpLogWriter->WriteLn("Window is restored");
					}

					// проверяем, того ли размера окно
					RECT	rcClient;
					GetClientRect(ghWnd, &rcClient);
					// и оказалось, что за время, пока окно было минимизировано, пытались изменить его размер
					if( (rcClient.bottom - rcClient.top) != dwClientHeight ||
						(rcClient.right - rcClient.left) != dwClientWidth
					){
						// то изменить размер окна на новый
						if( !ghWnd)	return 0;
						CAxWindow	axParent;	axParent.Attach(ghWnd);
						if(!bWindowMoving){
							bWindowMoving = true;
							//axParent.MoveWindow(0, 0, dwClientWidth, dwClientHeight);
							DWORD size = (DWORD)dwClientWidth + (((DWORD)dwClientHeight)<<16);
							dwClientWidth = dwClientHeight = 0; 
							axParent.SendMessage(WM_ADJUST_CONTROL_WND, N3D_MESSAGE_CODE, size);
							bWindowMoving = false;
							GetClientRect(ghWnd, &rcClient);
							dwClientWidth = rcClient.right - rcClient.left;
							dwClientHeight = rcClient.bottom - rcClient.top;
							if(pOMSContext && pOMSContext->mp3DWS)
								pOMSContext->mp3DWS->OnWindowSizeChanged(dwClientWidth, dwClientHeight);
						}
					}
				}
			}
			handled = false;
			}break;
		case WM_COPYDATA:
			if( wParam == 2)
			{
				COPYDATASTRUCT* pCopyData=(COPYDATASTRUCT*)lParam;
				if( pCopyData->cbData < 4096)
				{
					CComString asUrl;
					LPTSTR alpBuffer = asUrl.GetBufferSetLength(pCopyData->cbData);
					if( alpBuffer != NULL)
						memcpy( alpBuffer, pCopyData->lpData, pCopyData->cbData * sizeof(alpBuffer[0]));

					CCommandLineParams commandLineParams;
					commandLineParams.SetCommandLine( alpBuffer);
					if (commandLineParams.UrlIsSet())
					{
						USES_CONVERSION;
						CUrlParser urlParser( A2W(alpBuffer));
						const wchar_t *pLoginVal = urlParser.GetParam(L"login", NULL);
						if( pLoginVal != NULL && wcslen(pLoginVal)>0)
							pOMSContext->mpSM->SetLogin( pLoginVal);
						const wchar_t *pLoginKeyVal = urlParser.GetParam(L"loginKey", NULL);
						if( pLoginKeyVal != NULL && wcslen(pLoginKeyVal)>0)
							pOMSContext->mpSM->SetLoginKey( pLoginKeyVal);
						mlString sURL = MB2WC(commandLineParams.GetUrl().c_str());
						pOMSContext->mpSM->SetLocationUrlAndTeleport( sURL.c_str());
						if( IsIconic( ghWnd))
							ShowWindow(ghWnd, SW_RESTORE);
						SetForegroundWindow( ghWnd);
						/*if( pOMSContext->mpSyncMan->PreciseURL(sURL.c_str()))
						{
							pOMSContext->mpSM->SetLocationUrl( sURL.c_str());
							if( IsIconic( ghWnd))
								ShowWindow(ghWnd, SW_RESTORE);
							SetForegroundWindow( ghWnd);
						}*/
					}
				}
			}
			else if (wParam == COPY_DATA_CODE_PASSWORD)
			{
				COPYDATASTRUCT* pCopyData=(COPYDATASTRUCT*)lParam;

				SendData2Launcher(launchProcessID, APP_WORK, pCopyData);
			}
			else if (wParam == COPY_DATA_CODE_LOGIN)
			{
				COPYDATASTRUCT* pCopyData=(COPYDATASTRUCT*)lParam;
				SendData2Launcher(launchProcessID, APP_STATUS, pCopyData);
			}
			else if (wParam == COPY_DATA_CODE_COMMENT)
			{
				COPYDATASTRUCT* pCopyData=(COPYDATASTRUCT*)lParam;
				SendData2Launcher(launchProcessID, APP_NOTIFY, pCopyData);
			}
			else if (wParam == COPY_DATA_CODE_VERSION)
			{
				COPYDATASTRUCT* pCopyData=(COPYDATASTRUCT*)lParam;
				USES_CONVERSION;
				pOMSContext->mpSM->SetVersionInfo( A2W((char*)pCopyData->lpData));
			}
			else if (wParam == COPY_DATA_EXIT)
			{
				if (pOMSContext != NULL)
				{
					pOMSContext->mpServiceWorldMan->SetSessionStateValue( service::CSS_EXIT_CODE, 1, NULL);
					pOMSContext->mpLogWriter->WriteLn("Correct exit (COPY_DATA_EXIT)");
					Sleep(50);
					pOMSContext->mpRM->OnExit();
					pOMSContext->mpApp->exit();		
				}
			}
			else
			{
				pOMSContext->mp3DWS->OnCopyData(message, wParam, lParam, &handled);
			}
			break;
		case WM_ERASEBKGND:
			{
				handled = true;
			}
			break;
		case WM_PRINT:
			{
				if (!g_printWindowSupport)
				{
					MP_NEW_V(g_printWindowSupport, CWindowPrintSupport, pOMSContext);
				}
				g_printWindowSupport->HandlePrintRequest(wParam);
			} break;
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC	hdc = BeginPaint(hWnd, &ps);
				if( hdc)
					EndPaint(hWnd, &ps);
				return 0;
			}	break;
		case WM_IME_NOTIFY:
			/*if ((wParam == IMN_CLOSESTATUSWINDOW) && (g_intel))
			{
				ShowWindow(hWnd, SW_MINIMIZE);
				pOMSContext->mpLogWriter->WriteLn("MINIMIZE CLOSESTATUS");
			}*/
			break;
		case WM_ADJUST_CONTROL_WND:
			{
			if( wParam == N3D_MESSAGE_CODE)
				{
					CLogValue logValue1( "WM_ADJUST_CONTROL_WND wParam = N3D_MESSAGE_CODE");
					pOMSContext->mpLogWriter->WriteLn(logValue1);
					if( gbClientSizeInitialized &&
						dwClientWidth == LOWORD(lParam) &&
						dwClientHeight == HIWORD(lParam))
					{
						CLogValue logValue2("WM_ADJUST_CONTROL_WND ghWnd = null OR parameters null");
						pOMSContext->mpLogWriter->WriteLn(logValue2);
						break;
					}
					gbClientSizeInitialized = TRUE;
					dwClientWidth = LOWORD(lParam);
					dwClientHeight = HIWORD(lParam);
					std::string s = "jhgfwkerj";
					CLogValue logValue321341(s.c_str());
					CLogValue logValue31("WM_ADJUST_CONTROL_WND width=");
					CLogValue logValue32((int)dwClientWidth);
					CLogValue logValue33(" height=");
					CLogValue logValue34((int)dwClientHeight);
					pOMSContext->mpLogWriter->WriteLn(logValue31, logValue32, logValue33, logValue34);
					if( !ghWnd)
					{
						CLogValue logValue4("WM_ADJUST_CONTROL_WND ghWnd = null");
						pOMSContext->mpLogWriter->WriteLn(logValue4);
						return 1;
					}
					//CComQIPtr<IOleWindow>	pOle = gpWS3D;
					//ATLASSERT( pOle);
					HWND	hwndControl;
					if( true/*pOle && SUCCEEDED( pOle->GetWindow( &hwndControl))*/)
					{
						if( !ghWnd)	return 0;

						CAxWindow	axParent;	axParent.Attach(ghWnd);
						
						RECT rcWnd = {0,0,LOWORD(lParam),HIWORD(lParam)};
						HWND hDesktop = ::GetDesktopWindow();
						if( hDesktop)
						{
							RECT rcDesktop;
							::GetWindowRect(hDesktop, &rcDesktop);
							rcDesktop.right = rcDesktop.right - rcDesktop.left;
							rcDesktop.bottom = rcDesktop.bottom - rcDesktop.top;
							rcDesktop.left = rcDesktop.top = 0;
							/*if( rcWnd.right >= rcDesktop.right || rcWnd.bottom >= rcDesktop.bottom)
							{
								// Уберем заголовок окна
								DWORD ws = axParent.GetStyle();
								axParent.ModifyStyle(WS_CAPTION | WS_BORDER | WS_SYSMENU, 0);
							}
							else
							{*/
								// Поставим заголовок окна
								axParent.ModifyStyle(0, WS_CAPTION | WS_BORDER | WS_SYSMENU);
							//}
						}

											
						// Двигаем парентовое окно

							// если окно минимизировано, то перед изменением размеров окна, надо его сначала восстановить
						WINDOWPLACEMENT wndpl; wndpl.length = sizeof(WINDOWPLACEMENT);
						if(axParent.GetWindowPlacement(&wndpl) && wndpl.showCmd == SW_SHOWMINIMIZED){
							//WINDOWPLACEMENT wndpl2; wndpl2.length = sizeof(WINDOWPLACEMENT);
							wndpl.showCmd = SW_RESTORE;
							// axParent.SetWindowPlacement(&wndpl);
						}

						::AdjustWindowRectEx(&rcWnd, axParent.GetStyle(),
							(!(axParent.GetStyle() & WS_CHILD) && (axParent.GetMenu() != NULL)),
							axParent.GetExStyle());

						axParent.SetWindowPos(((!gbFullScreen) && (!gbTopWindow)) ? 0 : HWND_TOPMOST, 0, 0,
							rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, 
							SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_FRAMECHANGED);

						if (pOMSContext->mpRM)
						if ((pOMSContext->mpRM->IsCaveMode()) && (rcWnd.right - rcWnd.left > 512))
						{
							int offsetX, width, height;
							pOMSContext->mpApp->GetWideDesktopParams(width, height, offsetX);
							BOOL bRet = axParent.MoveWindow(offsetX, 0, width, height);
						}
						else
						{
							int toolbarOffsetX, toolbarOffsetY, toolbarWidth, toolbarHeight;
							pOMSContext->mpApp->GetToolbarParams(toolbarOffsetX, toolbarOffsetY, toolbarWidth, toolbarHeight);
							int offsetX = 0, displayWidth = 0, displayHeight = 0;
							pOMSContext->mpApp->GetDesktopSize(offsetX, displayWidth, displayHeight);
							int windowWidth = rcWnd.right - rcWnd.left;
							int windowHeight = rcWnd.bottom - rcWnd.top;
							int windowOffsetX = (int)((displayWidth - toolbarWidth - windowWidth) / 2) + toolbarOffsetX;
							int windowOffsetY = (int)((displayHeight - toolbarHeight - windowHeight) / 2) + toolbarOffsetY;
							BOOL bRet = axParent.MoveWindow(0 + offsetX + windowOffsetX, 0 + windowOffsetY, windowWidth, windowHeight);
						}

						axParent.RedrawWindow();

						axParent.Detach();

						/*if (gbCave)
						{
							RECT rcClient;
							GetClientRect(ghWnd, &rcClient);
							dwClientWidth = rcClient.right - rcClient.left;
							dwClientHeight = rcClient.bottom - rcClient.top;
							if(pOMSContext && pOMSContext->mp3DWS)
								pOMSContext->mp3DWS->OnWindowSizeChanged(dwClientWidth, dwClientHeight);
						}*/

						return 1;
					}
				}
			}	break;
		case WM_TIMER:
			if( wParam == IDT_FPS)
			{
				if( pOMSContext != NULL && pOMSContext->mpRM != NULL)
				{
					unsigned int auFPS = pOMSContext->mpRM->GetCurrentFPS();
					char	buffer[255];
					itoa( auFPS, buffer, 10);
					SetWindowText( ghWnd, buffer);
				}
			}
			break;
		case WM_CLOSE:
			 if (pOMSContext)
				pOMSContext->mpLogWriter->WriteLn("Correct exit (WM_CLOSE)!");
			 bUserExit = pOMSContext->mp3DWS->OnClose();
			 bUserExit = false;			
			break;
		case WM_RELOAD_SETTINGS:
			{
				pOMSContext->mpRM->ReloadUserSetting();
				pOMSContext->mpComMan->ReloadUserSettings();
			}
			break;
		case WM_RELOAD_INTERACTIVE_BOARD_SETTINGS:
			{
				pOMSContext->mpRM->ReloadUserSetting();
				if (pOMSContext->mpSharingMan)
					pOMSContext->mpSharingMan->SetupInteractiveBoardSettings();
			}
			break;
		case WM_CREATE_LOGIN_WND:
			SendSignal2Launcher(launchProcessID, APP_WORK, 14);
			break;
		case WM_SUCCES_LOGIN:
			SendSignal2Launcher(launchProcessID, APP_WORK, 15);
			break;
		case WM_EXIT_CODE:
			SendSignal2Launcher(launchProcessID, APP_STATUS, wParam);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   if(!handled)
	   return DefWindowProc(hWnd, message, wParam, lParam);
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
		case WM_INITDIALOG:
				return TRUE;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL){
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

void DeleteRunLogFileForLauncher()
{
	DWORD dwProcessId = GetCurrentProcessId();
	wchar_t ff[30];
	_itow(dwProcessId, ff, 10);
	std::wstring path = pOMSContext->mpLogWriter->GetApplicationDataDirectory() + L"\\Vacademia\\LauncherLog\\run_";
	path +=ff;
	path +=L".log";

	int error = _wremove(path.c_str());

	if (error !=0 )
	{
		for (int i = 0; i < 10; i++)
		{
				Sleep(1000);
				error = _wremove(path.c_str());
				if (error == 0)
					break;
		}
		
	}
}

std::wstring	GetApplicationRootDirectory()
{
	return glpcRootPath;
}
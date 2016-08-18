
#include "ProcessFinder.h"
#include "PsApi.h"
#include "TlHelp32.h"
#include <string.h>
#include <wchar.h>
#include <mbstring.h>

CProcessWindowFinder::CProcessWindowFinder()
{
	pCallbackUserData = NULL;
}

CProcessWindowFinder::~CProcessWindowFinder()
{
}

HWND ghwndProcessWindow = NULL;
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD dwWinProcID = 0;
	GetWindowThreadProcessId(hwnd, &dwWinProcID);
	if(dwWinProcID == (DWORD)lParam)
	{
		char   className[MAX_PATH+1];
		if( GetClassNameA( hwnd, className, MAX_PATH) > 0)
		{
			if( strcmp( className, "VSpaces virtual worlds player") == 0)
			{
				ghwndProcessWindow = hwnd;
				return FALSE; // нашли нужное окно. Можно не продолжать поиск
			}
		}
	}
	return TRUE;
}

HWND	CProcessWindowFinder::GetPlayerWindow( int auProcessID)
{
	ghwndProcessWindow = 0;
	EnumWindows(EnumWindowsProc, (LPARAM)auProcessID);
	return ghwndProcessWindow;
}

bool	CProcessWindowFinder::EnumPlayersWindow( LPCSTR asExePath, LPCSTR asExeName, LPPLAYERWINDOWSENUM lpCallback, void* apUserData)
{
	pCallbackUserData = apUserData;	
	GetPlayersWindow( asExePath, asExeName, lpCallback);
	return true;
}

SFindedProcess	CProcessWindowFinder::GetPlayerWindow( LPCSTR asExePath, LPCSTR asExeName)
{
	return GetPlayersWindow( asExePath, asExeName, NULL);
}

SFindedProcess	CProcessWindowFinder::GetPlayersWindow( LPCSTR asExePath, LPCSTR asExeName, LPPLAYERWINDOWSENUM lpCallback)
{
	HANDLE         hProcessSnap = NULL; 
	BOOL           bRet      = FALSE; 
	PROCESSENTRY32 pe32      = {0}; 

	ghwndProcessWindow = 0;
	//  Take a snapshot of all processes in the system. 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return SFindedProcess(NULL, 0);

	//  Fill in the size of the structure before using it. 
	pe32.dwSize = sizeof(PROCESSENTRY32); 

	//  Walk the snapshot of the processes, and for each process, 
	//  display information. 
	if (Process32First(hProcessSnap, &pe32))
	{ 
		BOOL          bGotModule = FALSE; 
		MODULEENTRY32 me32       = {0}; 

		do
		{
			// если название EXE-фала другое, то тоже пропускаем
			if( wcscmp( L"player.exe", pe32.szExeFile) != 0)
				continue;
			// открываем процесс, чтобы узнать побольше о его состоянии
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,
				FALSE, pe32.th32ProcessID);
			// если процесс открыть не смогли, то пропускаем его
			if (hProcess == NULL)
				continue;
			HMODULE hExeModule = NULL;
			DWORD cbNeeded;
			TCHAR szModulePath[MAX_PATH];
			LPTSTR pszProcessName = NULL;
			/*
			// получаем полный путь к EXE-файлу процесса
			if (EnumProcessModules(hProcess, &hExeModule, sizeof(HMODULE), &cbNeeded))
			{
				if (GetModuleFileNameEx(hProcess, hExeModule, szModulePath, MAX_PATH))
				{
					PathRemoveFileSpec(szModulePath);
					// если путь к exe-файлу разный, то пропускаем процесс
					if( stricmp( asExePath, szModulePath) != 0)
					{
						CloseHandle(hProcess);
						continue;
					}
				}
			}*/
			// находим главное окно процесса
			ghwndProcessWindow = NULL;
			EnumWindows(EnumWindowsProc, (LPARAM)pe32.th32ProcessID);
			if (ghwndProcessWindow)
			{				
				HWND hWnd = ghwndProcessWindow;
				unsigned int parentProcessID = pe32.th32ParentProcessID;
				WINDOWPLACEMENT wndpl; wndpl.length = sizeof(WINDOWPLACEMENT);

				if ( IsWindowVisible(hWnd) && !IsHungAppWindow(hWnd) && GetWindowPlacement(hWnd, &wndpl) && wndpl.showCmd != SW_HIDE)
				{
					SFindedProcess tmp(ghwndProcessWindow, pe32.th32ParentProcessID);				
					return tmp;
				}				
			}

			if( lpCallback)
			{
				if( ghwndProcessWindow)
				{
					lpCallback( (HANDLE)pe32.th32ProcessID, ghwndProcessWindow, pCallbackUserData);
				}
			}

			ghwndProcessWindow = NULL;

			// закрываем описатель процесса
			CloseHandle(hProcess);
		} 
		while (Process32Next(hProcessSnap, &pe32)); 
	} 

	// Do not forget to clean up the snapshot object. 
	CloseHandle (hProcessSnap); 
	return SFindedProcess(NULL, 0);
}


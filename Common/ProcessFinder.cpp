#include "stdafx.h"
#include "ProcessFinder.h"
#include "PsApi.h"
#include "TlHelp32.h"

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
		TCHAR   className[MAX_PATH+1];
		if( GetClassName( hwnd, className, MAX_PATH) > 0)
		{
			if( _tcscmp( className, "VSpaces virtual worlds player") == 0)
			{
				ghwndProcessWindow = hwnd;
				return FALSE; // нашли нужное окно. ћожно не продолжать поиск
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

bool	CProcessWindowFinder::EnumPlayersWindow( LPCTSTR asExeName, LPPLAYERWINDOWSENUM lpCallback, void* apUserData)
{
	pCallbackUserData = apUserData;	
	GetPlayersWindow( asExeName, lpCallback);
	return true;
}

std::vector<SFindedProcess>	CProcessWindowFinder::GetPlayerWindow( LPCTSTR asExeName)
{
	return GetPlayersWindow( asExeName, NULL);
}

std::vector<SFindedProcess>	CProcessWindowFinder::GetPlayersWindow( LPCTSTR asExeName, LPPLAYERWINDOWSENUM lpCallback)
{
	std::vector<SFindedProcess> res;

	HANDLE         hProcessSnap = NULL; 
	BOOL           bRet      = FALSE; 
	PROCESSENTRY32 pe32      = {0}; 

	ghwndProcessWindow = 0;
	//  Take a snapshot of all processes in the system. 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return res;

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
			if( _stricmp( asExeName, pe32.szExeFile) != 0)
				continue;
			// открываем процесс, чтобы узнать побольше о его состо€нии
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,
				FALSE, pe32.th32ProcessID);
			// если процесс открыть не смогли, то пропускаем его
			if (hProcess == NULL)
				continue;
			HMODULE hExeModule = NULL;


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
				res.push_back(SFindedProcess(ghwndProcessWindow, pe32.th32ParentProcessID));				
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
	return res;
}

void CProcessWindowFinder::GetPlayerProcesses( LPCTSTR asExeName, std::vector<unsigned int>& processIDs)
{
	HANDLE         hProcessSnap = NULL; 
	BOOL           bRet      = FALSE; 
	PROCESSENTRY32 pe32      = {0}; 

	//  Take a snapshot of all processes in the system. 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return;

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
			if( _stricmp( asExeName, pe32.szExeFile) != 0)
				continue;

			processIDs.push_back(pe32.th32ProcessID);
			processIDs.push_back(pe32.th32ParentProcessID);
		} 
		while (Process32Next(hProcessSnap, &pe32)); 
	}

	// Do not forget to clean up the snapshot object. 
	CloseHandle (hProcessSnap); 
}
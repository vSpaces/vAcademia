#include "stdafx.h"
#include "ProcessFinder.h"
#include "PsApi.h"
#include "TlHelp32.h"

CProcessWindowFinder::CProcessWindowFinder()
{
}

CProcessWindowFinder::~CProcessWindowFinder()
{
}

HWND ghwndProcessWindow = NULL;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam){
	DWORD dwWinProcID = 0;
	GetWindowThreadProcessId(hwnd, &dwWinProcID);
	if(dwWinProcID == (DWORD)lParam){
		TCHAR   className[MAX_PATH+1];
		if( GetClassName( hwnd, className, MAX_PATH) > 0)
		{
			if( _tcscmp( className, "VSpaces virtual worlds player") == 0)
			{
				ghwndProcessWindow = hwnd;
				return FALSE; // нашли нужное окно. Можно не продолжать поиск
			}
		}
	}
	return TRUE;
}
HWND	CProcessWindowFinder::get_process_hwnd( const CComString& asExePath)
{
	HANDLE         hProcessSnap = NULL; 
	BOOL           bRet      = FALSE; 
	PROCESSENTRY32 pe32      = {0}; 

	CComString	sExeName;
	CComString	sExePath = asExePath;	sExePath.MakeLower();
	int idx = max( sExePath.ReverseFind('/'), sExePath.ReverseFind('\\'));
	if( idx != -1)
		sExeName = sExePath.Right( sExePath.GetLength() - idx - 1);
	else
		sExeName = sExePath;

	//  Take a snapshot of all processes in the system. 
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

	if (hProcessSnap == INVALID_HANDLE_VALUE) 
		return 0;

	//  Fill in the size of the structure before using it. 
	pe32.dwSize = sizeof(PROCESSENTRY32); 

	//  Walk the snapshot of the processes, and for each process, 
	//  display information. 
	if (Process32First(hProcessSnap, &pe32)) { 
		DWORD         dwPriorityClass; 
		BOOL          bGotModule = FALSE; 
		MODULEENTRY32 me32       = {0}; 

		do{ 
			// если название EXE-фала другое, то тоже пропускаем
			CComString tempExe( pe32.szExeFile);	tempExe.MakeLower();
			if( sExeName != tempExe)
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
			// получаем полный путь к EXE-файлу процесса
			if (EnumProcessModules(hProcess, &hExeModule, sizeof(HMODULE), &cbNeeded))
			{
				if (GetModuleFileNameEx(hProcess, hExeModule, szModulePath, MAX_PATH))
				{
					// если путь к exe-файлу разный, то пропускаем процесс
					CComString tempPath( szModulePath); tempPath.MakeLower();
					if( sExePath != tempPath)
					{
						CloseHandle(hProcess);
						continue;
					}
				}
			}
			// находим главное окно процесса
			ghwndProcessWindow = NULL;
			EnumWindows(EnumWindowsProc, (LPARAM)pe32.th32ProcessID);
			// закрываем описатель процесса
			CloseHandle(hProcess);
		} 
		while (!ghwndProcessWindow && Process32Next(hProcessSnap, &pe32)); 
	} 

	// Do not forget to clean up the snapshot object. 
	CloseHandle (hProcessSnap); 
	return ghwndProcessWindow;
}
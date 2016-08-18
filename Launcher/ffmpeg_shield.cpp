
#include "StdAfx.h"
#include "FFMpeg_shield.h"
#include "Tlhelp32.h"
#include <Psapi.h>

void KillFFMpeg()
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
	if (Process32First(hProcessSnap, &pe32)) { 
		DWORD         dwPriorityClass; 
		BOOL          bGotModule = FALSE; 
		MODULEENTRY32 me32       = {0}; 

		do
		{ 
			if(_tcscmp(pe32.szExeFile, "ffmpeg.exe") == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE,FALSE, pe32.th32ProcessID);
				if (hProcess != NULL)
				{
					rtl_TerminateOtherProcess(hProcess, (DWORD)-1);
					CloseHandle(hProcess);
				}
				continue;
			}
		} 
		while (Process32Next(hProcessSnap, &pe32)); 
	} 

	// Do not forget to clean up the snapshot object. 
	CloseHandle (hProcessSnap); 
}
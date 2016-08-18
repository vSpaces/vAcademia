#pragma once

#define RUS 1
#define ENG 2
#define KZH 3
#define CONFIG_FILE L"player.ini"
#define BUFFER_SIZE 65536
#define SERVER_NAME "176.9.41.35"

#define APP_HANG "application is hanged"

//extern bool getAppParams( WCHAR *sRoot, TCHAR **serverName, TCHAR **appVersion, int *iLanguage);

extern bool getAppLogin();
extern void setSessionID( int aSessionID);

//extern bool make_dump( TCHAR *sRoot, HANDLE hProcessHandle, BOOL bSendReport = FALSE);

//extern bool send_report();

extern bool ReadDumpStream();

extern bool Get_Module_By_Ret_Addr(int iProcessId, PBYTE Ret_Addr, PWCHAR Module_Name, BYTE * Module_Addr);
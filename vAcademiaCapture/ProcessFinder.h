#pragma once

#include <windows.h>

typedef void (*LPPLAYERWINDOWSENUM)(HANDLE ahProcess, HWND ahWnd, void* apUserData);

typedef struct _SFindedProcess
{
	HWND window;
	unsigned int parentProcessID;

	_SFindedProcess(HWND _window, unsigned int _parentProcessID)
	{
		window = _window;
		parentProcessID = _parentProcessID;
	}

	_SFindedProcess(const _SFindedProcess& other)
	{
		window = other.window;
		parentProcessID = other.parentProcessID;
	}
} SFindedProcess;

class CProcessWindowFinder
{
public:
	CProcessWindowFinder();
	~CProcessWindowFinder();

	bool	EnumPlayersWindow( LPCSTR asExePath, LPCSTR asExeName, LPPLAYERWINDOWSENUM lpCallback, void* apUserData);
	SFindedProcess	GetPlayerWindow( LPCSTR asExePath, LPCSTR asExeName);
	HWND	GetPlayerWindow( int auProcessID);
	
private:
	SFindedProcess	GetPlayersWindow( LPCSTR asExePath, LPCSTR asExeName, LPPLAYERWINDOWSENUM lpCallback);
	void*	pCallbackUserData;
};

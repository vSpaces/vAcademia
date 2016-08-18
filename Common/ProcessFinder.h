#pragma once

#include <vector>

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

	bool	EnumPlayersWindow( LPCTSTR asExeName, LPPLAYERWINDOWSENUM lpCallback, void* apUserData);
	std::vector<SFindedProcess>	GetPlayerWindow( LPCTSTR asExeName);
	HWND	GetPlayerWindow( int auProcessID);
	void	GetPlayerProcesses( LPCTSTR asExeName, std::vector<unsigned int>& processIDs);

private:
	std::vector<SFindedProcess>	GetPlayersWindow( LPCTSTR asExeName, LPPLAYERWINDOWSENUM lpCallback);
	void*	pCallbackUserData;
};

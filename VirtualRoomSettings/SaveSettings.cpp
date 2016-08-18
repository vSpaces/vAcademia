#include "StdAfx.h"
#include ".\savesettings.h"
#include "ProcessFinder.h"
#include "../PlayerLib/notifies.h"

CUserSettings* CSaveSettings::GetUserSettings()
{
	return &m_userSettings;
}

void CSaveSettings::NotifyAboutChangeSettings()
{
	InformExternalPlayer();
}

void PlayerEnums(HANDLE ahProcess, HWND ahWnd, void* apUserData)
{
	if( apUserData)
	{
		((CSaveSettings*)apUserData)->NotifyPlayer( ahProcess, ahWnd);
	}
}

void CSaveSettings::NotifyPlayer(HANDLE ahProcess, HWND ahWnd)
{
	::PostMessage( ahWnd, WM_RELOAD_INTERACTIVE_BOARD_SETTINGS, 0, 0);	
}

void CSaveSettings::InformExternalPlayer()
{
	// get folder of the current process
	TCHAR szPlayerDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPlayerDir, MAX_PATH);
	PathRemoveFileSpec(szPlayerDir);

	CProcessWindowFinder	finder;
	finder.EnumPlayersWindow(szPlayerDir, "player.exe", &PlayerEnums, this);
}
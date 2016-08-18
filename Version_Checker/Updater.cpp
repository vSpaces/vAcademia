#include "stdafx.h"
#include "Updater.h"
#include <shlobj.h>
#include <Shlwapi.h>
#include <Shellapi.h>
#include "AssistantModule.h"

CUpdater::CUpdater(const HWND &hWnd)
{
	m_handle = hWnd;	
}

CUpdater::~CUpdater(){};

wstring CUpdater:: GetApplicationDataDirectory()
{
	wchar_t tmp[MAX_PATH];
	SHGetSpecialFolderPathW(NULL, tmp, CSIDL_APPDATA, true);
	wstring str(tmp);
	return str;
}

HWND CUpdater:: GetHandle()const
{
	return m_handle;
}

void CUpdater:: Update()const
{
	std::wstring rootDir = GetRootDir();
	rootDir += L"\\";

	std::wstring appDir = GetApplicationDataDirectory();

	std::wstring sCommandWS;

	// добавим параметр - путь к клиенту
	std::wstring clientPathWS = rootDir;
	sCommandWS += L" -clientPath \"";
	sCommandWS += clientPathWS;
	sCommandWS += L"\"";

	CStartUpdater* stUpdate = new CStartUpdater(); 

	unsigned int errorCode = 0;
	unsigned int systemErrorCode = 0;
	std::wstring updaterPath = stUpdate->prepareUpdate(rootDir, appDir, errorCode, systemErrorCode);
	
	CDictionary* pDictionary = CDictionary::GetInstance();
	if (errorCode != 0)
	{	
		wstring msg = pDictionary->GetText(CANT_GET_PARAMS);
		msg += L".";
		msg += pDictionary->GetText(UPDATE_FROM_SITE);
		MessageBoxW(NULL, msg.c_str(), pDictionary->GetText(ERROR_HAS_OCCURED).c_str(), 0);
	}else
	{
		std::wstring updaterFullFilePath = L"\"" + updaterPath + L"updater.exe\"";
		HINSTANCE errShell = ShellExecuteW(NULL, L"open", updaterFullFilePath.c_str(), sCommandWS.c_str(), rootDir.c_str(), SW_SHOW);
		if(int(errShell) <= 32)
		{
			wstring msg = pDictionary->GetText(FAILED_UPDATER);
			msg += L".";
			msg += pDictionary->GetText(UPDATE_FROM_SITE);
			MessageBoxW(NULL, msg.c_str(), pDictionary->GetText(ERROR_HAS_OCCURED).c_str(), 0);
		}
	}
	DestroyWindow(GetHandle());
}
// MonitorIdentify.cpp : Implementation of CMonitorIdentify

#include "stdafx.h"
#include "MonitorIdentify.h"
#include "screenDraw.h"
#include ".\monitoridentify.h"

#include "showImageCodes.h"
#include "SaveSettings.h"
#include "Common.h"
#include "fileSys.h"
#include ".\monitordrawer.h"

#include <GdiPlus.h>
using namespace Gdiplus;

#define RADIO_BUTTON_INDEX 7000

CMonitorIdentify::CMonitorIdentify()
{
	m_currentRadioBtnIndex = RADIO_BUTTON_INDEX;

	CSaveSettings* sharedSaveSettings =  CSaveSettings::Instance();
	CUserSettings* settings = sharedSaveSettings->GetUserSettings();
	settings->LoadGlobal();
	std::string sMonitorID;
	sMonitorID = settings->GetSetting("monitor_for_interactive_board");
	m_selectedRadioBtnIndex = atoi(sMonitorID.c_str());
	
	sharedSaveSettings->FreeInst();
}

std::string GetApplicationDataDirectory()
{
	CHAR tmp[MAX_PATH];
	SHGetSpecialFolderPath(NULL, tmp, CSIDL_APPDATA, true);
	return tmp;
}

std::string	GetApplicationRootDirectory()
{
	TCHAR lpcIniFullPathName[MAX_PATH*2+2]=_T("");

	TCHAR lpcStrBuffer[MAX_PATH*2+2];	lpcStrBuffer[0] = 0;
	::GetModuleFileName( _AppModule.m_hInst, lpcStrBuffer, MAX_PATH);

	if( lpcStrBuffer[0] == 0)
		return "";

	SECFileSystem fs;
	CComString sExePath = fs.GetPath( CComString( lpcStrBuffer), TRUE);
	return std::string( sExePath.GetBuffer());
}

BOOL CALLBACK MonitorEnumProc( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	ATLASSERT( dwData);
	vecMonitorsInfo* monitors = (vecMonitorsInfo*)dwData;
	MONITORINFOEX info;
	info.cbSize = sizeof( info);
	if( GetMonitorInfo( hMonitor, &info))
		monitors->push_back( info);
	return TRUE;
}

vecMonitorsInfo CMonitorIdentify::GetMonitors()
{
	vecMonitorsInfo monitorsInfo;
	EnumDisplayMonitors( 0, NULL, MonitorEnumProc, (LPARAM)&monitorsInfo);
	
	return monitorsInfo;
	
}

void CMonitorIdentify::CreateRadioButtonsForMonitors()
{
	vecMonitorsInfo monitorsInfo = GetMonitors();

	RemoveRadioBtnIfNeeded();
	char buf[100];
	for (unsigned int i=0; i<monitorsInfo.size(); i++)
	{
		RECT buttonRect;
		buttonRect.left = 50 + 50*i;
		buttonRect.top = 50;
		buttonRect.bottom = buttonRect.top + 50;
		buttonRect.right = buttonRect.left + 50;

		itoa(i+1,buf,10);
		CreateRadioButtonsWithText(buf, buttonRect, m_currentRadioBtnIndex++);
	}
	if (m_selectedRadioBtnIndex-1 < m_currRadioBtns.size())
	{
		m_currRadioBtns[m_selectedRadioBtnIndex-1].SetCheck(BST_CHECKED);
	}
	
}

void CMonitorIdentify::CreateRadioButtonsWithText(char* aText, RECT& aRect, UINT anID)
{
	CButton btn;

	if (btn.Create(m_hWnd, &aRect, NULL, WS_CHILD, 0, anID)){
		btn.SetButtonStyle(BS_AUTORADIOBUTTON);
		btn.SetWindowText(aText);
		btn.ShowWindow(SW_SHOW);
	}

	m_currRadioBtns.push_back(btn);
}

void CMonitorIdentify::RemoveRadioBtnIfNeeded()
{
	for (unsigned int i=0; i<m_currRadioBtns.size(); i++)
	{
		m_currRadioBtns[i].ShowWindow(SW_HIDE);
		m_currRadioBtns[i].DestroyWindow();
	}
	m_currRadioBtns.clear();
}

LRESULT CMonitorIdentify::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CAxDialogImpl<CMonitorIdentify>::OnInitDialog(uMsg, wParam, lParam, bHandled);
	CreateRadioButtonsForMonitors();

	return 1;  // Let the system set the focus
}

LRESULT CMonitorIdentify::OnBnClickedIdentify(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CMonitorDrawer* drawer = CMonitorDrawer::Instance();
	drawer->ShowMonitorImageByCode(SHOW_IDENTIFY_MONITOR, 0); // timeStep == 0
	drawer->FreeInst();
	return 0;
}

LRESULT CMonitorIdentify::OnClickedSave(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);

	CSaveSettings* sharedSaveSettings =  CSaveSettings::Instance();
	CUserSettings* settings = sharedSaveSettings->GetUserSettings();

	int monitorID = 0;
	for (unsigned int i=0; i<m_currRadioBtns.size(); i++)
	{
		if (m_currRadioBtns[i].GetCheck() == BST_CHECKED)
			monitorID = i+1;
	}
	char buf[50];
	itoa(monitorID, buf, 10);
	std::string sMonitorID = buf;

	if (monitorID != 0)
	{
		vecMonitorsInfo monitorsInfo = GetMonitors();
		std::string name = monitorsInfo[monitorID - 1].szDevice;
		do
		{
			if (!settings->SaveSetting("monitor_for_interactive_board", sMonitorID))
				break;			

			char buf[50];
			itoa(name[name.size() - 1] - '0', buf, 10);
			if (!settings->SaveSetting("displayid_for_interactive_board", buf))
				break;			
		}while (false);
	}

	sharedSaveSettings->NotifyAboutChangeSettings();
	sharedSaveSettings->FreeInst();

	return 0;
}
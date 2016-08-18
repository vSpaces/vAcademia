// VirtualRoomSettingsMain.cpp : Implementation of CVirtualRoomSettingsMain

#include "stdafx.h"
#include "VirtualRoomSettingsMain.h"
#include ".\virtualroomsettingsmain.h"
#include "MonitorIdentify.h"
#include "KinectWizard.h"
#include "SaveSettings.h"
#include "Color.h"


CVirtualRoomSettingsMain::CVirtualRoomSettingsMain()
{
	m_refForMonitorDrawer = CMonitorDrawer::Instance();
	m_refForSaveSettings = CSaveSettings::Instance();
}

CVirtualRoomSettingsMain::~CVirtualRoomSettingsMain()
{
	m_refForMonitorDrawer->FreeInst();
	m_refForSaveSettings->FreeInst();
}

LRESULT CVirtualRoomSettingsMain::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hdcStatic = (HDC) wParam;   

	if ( (HWND)lParam == GetDlgItem( IDC_MONITOR_STATUS ) )
	{
		if ( IsRightMonitorSettings() )
		{

			::SetTextColor( hdcStatic, GREEN );
		}
		else
			::SetTextColor( hdcStatic, RED );
	}
	else if ( (HWND)lParam == GetDlgItem( IDC_KINECT_STATUS ) )
	{
		if ( IsRightKinectSettings() )
			::SetTextColor( hdcStatic, GREEN );
		else
			::SetTextColor( hdcStatic, RED );
	}
	else
	{
		return 0;
	}

	::SetBkMode   ( hdcStatic, TRANSPARENT );
	::SelectObject( hdcStatic, ::GetStockObject( NULL_BRUSH) );
	return (HRESULT)::GetCurrentObject( hdcStatic, OBJ_BRUSH);
}

LRESULT CVirtualRoomSettingsMain::UpdateFormData()
{
	// проверка настроек монитора
	if (IsRightMonitorSettings())
	{

		SetDlgItemText(IDC_MONITOR_STATUS, "настроен");
	}
	else
	{
		SetDlgItemText(IDC_MONITOR_STATUS, "не настроен");		
	}

	// проверка настроек Kinect
	if (IsRightKinectSettings())
	{
		SetDlgItemText(IDC_KINECT_STATUS, "настроен");
	}
	else
	{
		SetDlgItemText(IDC_KINECT_STATUS, "не настроен");
	}

	RedrawWindow(); // обновление значений лейблов

	return 0;
}

LRESULT CVirtualRoomSettingsMain::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CAxDialogImpl<CVirtualRoomSettingsMain>::OnInitDialog(uMsg, wParam, lParam, bHandled);

	UpdateFormData();

	return 1;  // Let the system set the focus
}

LRESULT CVirtualRoomSettingsMain::OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	PostQuitMessage( 0 );
	return 0;
}

LRESULT CVirtualRoomSettingsMain::OnBnClickedMonitorButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CMonitorIdentify monnitorDlg;	
	if (monnitorDlg.DoModal() == IDOK)
		UpdateFormData();

	return 0;
}

LRESULT CVirtualRoomSettingsMain::OnBnClickedKinectButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CKinectWizard kinectWizardDlg;
	int nRet = kinectWizardDlg.DoModal();
	if ( nRet == IDOK || nRet == 0 )
		UpdateFormData();

	return 0;
}

bool CVirtualRoomSettingsMain::IsRightKinectSettings()
{
	std::string settingsStr1, settingsStr2, settingsStr3, settingsStr4, settingsStr5, settingsStr6;

	CSaveSettings* sharedSaveSettings = CSaveSettings::Instance();
	CUserSettings* settings = sharedSaveSettings->GetUserSettings();

	settings->LoadGlobal();

	settingsStr1 = settings->GetSetting(USER_SETTING_KINECT_LEFT_CORNER_X);
	settingsStr2 = settings->GetSetting(USER_SETTING_KINECT_LEFT_CORNER_Y);
	settingsStr3 = settings->GetSetting(USER_SETTING_KINECT_LEFT_CORNER_Z);
	settingsStr4 = settings->GetSetting(USER_SETTING_KINECT_RIGHT_CORNER_X);
	settingsStr5 = settings->GetSetting(USER_SETTING_KINECT_RIGHT_CORNER_Y);
	settingsStr6 = settings->GetSetting(USER_SETTING_KINECT_RIGHT_CORNER_Z);

	sharedSaveSettings->FreeInst();

	if ( settingsStr1.empty() || settingsStr2.empty() || settingsStr3.empty() || settingsStr4.empty() || settingsStr5.empty() || settingsStr6.empty() )
		return false;

	double n1, n2, n3, n4, n5, n6;
	n1 = ::atof(settingsStr1.c_str());
	n2 = ::atof(settingsStr2.c_str());
	n3 = ::atof(settingsStr3.c_str());
	n4 = ::atof(settingsStr4.c_str());
	n5 = ::atof(settingsStr5.c_str());
	n6 = ::atof(settingsStr6.c_str());

	if ( n1 == 0 && n2 == 0 && n3 == 0 && n4 == 0 && n5 == 0 && n6 == 0 )
		return false;

	return true;
}

bool CVirtualRoomSettingsMain::IsRightMonitorSettings()
{
	CSaveSettings* sharedSaveSettings = CSaveSettings::Instance();
	CUserSettings* settings = sharedSaveSettings->GetUserSettings();

	settings->LoadGlobal();

	std::string settingsStr;
	settingsStr = settings->GetSetting("monitor_for_interactive_board");

	sharedSaveSettings->FreeInst();

	if ( settingsStr.empty() )
		return false;

	return true;
}

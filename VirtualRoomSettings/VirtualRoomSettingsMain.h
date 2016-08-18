// VirtualRoomSettingsMain.h : Declaration of the CVirtualRoomSettingsMain

#pragma once

#include "resource.h"       // main symbols
#include <atlhost.h>
#include "SaveSettings.h"
#include "MonitorDrawer.h"
// CVirtualRoomSettingsMain

class CVirtualRoomSettingsMain : 
	public CAxDialogImpl<CVirtualRoomSettingsMain>
{
public:
	CVirtualRoomSettingsMain();

	~CVirtualRoomSettingsMain();

	enum { IDD = IDD_VIRTUALROOMSETTINGSMAIN };

BEGIN_MSG_MAP(CVirtualRoomSettingsMain)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	COMMAND_HANDLER(IDC_BUTTON2, BN_CLICKED, OnBnClickedMonitorButton)
	COMMAND_HANDLER(IDC_BUTTON1, BN_CLICKED, OnBnClickedKinectButton)
	MESSAGE_HANDLER(WM_CTLCOLORSTATIC , OnCtlColor)
	CHAIN_MSG_MAP(CAxDialogImpl<CVirtualRoomSettingsMain>)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedMonitorButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedKinectButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT UpdateFormData();

private:
	bool IsRightKinectSettings();
	bool IsRightMonitorSettings();

private: 
	CMonitorDrawer* m_refForMonitorDrawer;
	CSaveSettings* m_refForSaveSettings;	
};



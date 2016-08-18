// MonitorIdentify.h : Declaration of the CMonitorIdentify

#pragma once

#include "resource.h"       // main symbols
#include <atlhost.h>


// CMonitorIdentify
typedef std::vector<MONITORINFOEX>		vecMonitorsInfo;

BOOL CALLBACK MonitorEnumProc( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

class CMonitorIdentify : 
	public CAxDialogImpl<CMonitorIdentify>
{
public:
	CMonitorIdentify();

	~CMonitorIdentify()
	{
	}

	enum { IDD = IDD_MONITORIDENTIFY };

BEGIN_MSG_MAP(CMonitorIdentify)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedSave)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
/*for ()
	if(uMsg == WM_COMMAND && id == LOWORD(wParam) && code == HIWORD(wParam))
	{ 
	bHandled = TRUE;
	lResult = func(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled);
	if(bHandled)
		return TRUE;
	}
	*/
	COMMAND_HANDLER(IDC_BUTTON1, BN_CLICKED, OnBnClickedIdentify)
	CHAIN_MSG_MAP(CAxDialogImpl<CMonitorIdentify>)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnClickedSave(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}
private:
	vecMonitorsInfo GetMonitors();
	void CreateRadioButtonsForMonitors();
	void CreateRadioButtonsWithText(char* aText, RECT& aRect, UINT anID);
	void RemoveRadioBtnIfNeeded();


	std::vector<CButton> m_currRadioBtns;
	int m_currentRadioBtnIndex;
	int m_selectedRadioBtnIndex;

public:
	LRESULT OnBnClickedIdentify(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};



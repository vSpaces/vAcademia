#pragma once

#include "VacademiaReportLib.h"

class VACADEMIAREPORT_API CDialogError :
	public CDialogImpl<CDialogError>
	//public CUpdateUI<CDialogError>
{
public:
	CDialogError();
	virtual ~CDialogError();

	enum { IDD = IDD_FORMVIEW};

	//const _AtlUpdateUIMap* GetUpdateUIMap(){return NULL;};

	BEGIN_MSG_MAP(CDialogError)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_BUTTON_YES, BN_CLICKED, OnBnClickedButtonYes)
		COMMAND_HANDLER(IDC_BUTTON_NO, BN_CLICKED, OnBnClickedButtonNo)
		COMMAND_HANDLER(IDC_CHECK_HIDEWND, BN_CLICKED, OnBnClickedCheckHideWnd)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_HANDLER(IDC_CHECK_RESTART, BN_CLICKED, OnBnClickedCheckRestart)
	END_MSG_MAP()

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBnClickedButtonYes(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonNo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCheckHideWnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedCheckRestart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
	BOOL m_bCreatingDumpAndSending;
	BOOL m_bAppRestart;
	int m_iLanguage;

public:
	void SetAppLanguage( int iLanguage);
	BOOL IsCreatingDumpAndSending();
	BOOL IsAppRestart();
	
};
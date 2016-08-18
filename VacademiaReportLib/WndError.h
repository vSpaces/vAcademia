#pragma once

#include "VacademiaReportLib.h"

class CReportMan;

class VACADEMIAREPORT_API CWndError :
	public CDialogImpl<CWndError>
	//public CWindowImpl<CWndError>
	//public CUpdateUI<CWndError>
{
public:
	CWndError();
	virtual ~CWndError();

	enum { IDD = IDD_FORMVIEW_NOTIFY};

	//const _AtlUpdateUIMap* GetUpdateUIMap(){return NULL;};

	BEGIN_MSG_MAP(CWndError)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_HANDLER(IDC_BUTTON_OK, BN_CLICKED, OnBnClickedButtonOk)
		COMMAND_HANDLER(IDC_BUTTON_CANCEL, BN_CLICKED, OnBnClickedButtonCancel)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)				
	END_MSG_MAP()

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:	
	int m_iLanguage;
	int m_nTimerID;
	float fadeOutDuration;
	HWND hSplashWindow;
	HBITMAP hBitmapsDot[3];
	int m_iProgress;
	int m_iProgressStep;
	BOOL m_bProgress;
	int m_iStep;
	BOOL bEnded;
	BOOL m_bAppRestart;
	CReportMan *m_pReportMan;
	BOOL m_bAppRestartCheckVisible;

public:
	enum{ ZIPPING = 1, QUERY_GET, QUERY_POST};

	void Create(HWND hWndParent, CReportMan *pReportMan, LPARAM dwInitParam = NULL);
	void SetAppLanguage( int iLanguage);
	void SetAppRestartCheckVisible( BOOL bAppRestartCheckVisible);
	void SetProgress( int iProgress);
	void SetStep( int iStep);
	void Start();
	void Stop();
	void End();
	void Back( int iStep);
	BOOL IsAppRestart();
	
	LRESULT OnBnClickedButtonOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
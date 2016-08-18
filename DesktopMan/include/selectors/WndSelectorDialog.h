// D:\Users\Pasa\Programming\vu\Release_FG_To_Head\DesktopMan\include\WndSelectorDialog.h : Declaration of the CWndSelectorDialog

#pragma once

#include <atlhost.h>
#include <atlcrack.h>
#include "../UI/ColoredControls.h"
#include "../UI/coloredVAControls.h"
#include "../UI/WTButton.h"
#include "../UI/wtGroupBox.h"
#include "../SharingContext.h"
#include <string>
#include "Include/ProcessListDlg.h"

class CWndSelector;

// CWndSelectorDialog
class CEditImpl : public CWindowImpl<CEditImpl, CEdit>
{
	BEGIN_MSG_MAP(CEditImpl)
		MSG_WM_CONTEXTMENU(OnContextMenu)
	END_MSG_MAP()

	void OnContextMenu ( HWND hwndCtrl, CPoint ptClick )
	{
		MessageBox("Edit control handled WM_CONTEXTMENU");
	}
};


struct ISelectorWindowEvents
{
	virtual	void OnSelectorWindowOK() = 0;
	virtual	void OnSelectorWindowCancel() = 0;
};

class CWndSelectorDialog : 
	public CAxDialogImpl<CWndSelectorDialog>,
	public CWinDataExchange<CWndSelectorDialog>,
	public ViskoeThread::CThreadImpl<CWndSelectorDialog>,
	public CColoredDialog<CWndSelectorDialog>,
	public CEventSource<ISelectorWindowEvents>
{
public:
	enum { IDD = IDD_WNDSELECTOR_DIALOG };
	CWndSelectorDialog (CWndSelector *wndsel, oms::omsContext* apContext, bool NP);
	~CWndSelectorDialog();
	DWORD	Run();
	void	StartSelection();
	void	ReleaseWindow();

BEGIN_MSG_MAP(CWndSelectorDialog)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	COMMAND_HANDLER(IDC_STATIC_FINDER_TOOL, STN_CLICKED, OnStnClickedStaticFinderTool)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	COMMAND_HANDLER(IDC_HIDE_CHECK, BN_CLICKED, OnBnClickedHideCheck)
	//CHAIN_MSG_MAP(CAxDialogImpl<CWndSelectorDialog>)
	COMMAND_HANDLER(IDC_STATIC_HIDECLIENT, STN_CLICKED, OnStnClickedStaticHideclient)
	COMMAND_HANDLER(IDC_BTN_SELECT_FROM_LIST, BN_CLICKED, OnBnClickedBtnSelectFromList)
	CHAIN_MSG_MAP(CColoredDialog<CWndSelectorDialog>)
	REFLECT_NOTIFICATIONS()
END_MSG_MAP()

BEGIN_DDX_MAP(CWndSelectorDialog)
	DDX_CONTROL(IDC_NAME_EDIT, m_nameEdit)
	DDX_TEXT(IDC_NAME_EDIT, m_nameEditStr)
	DDX_TEXT(IDC_SIZE_STATIC, m_sizeStaticStr)
	DDX_CHECK(IDC_HIDE_CHECK, m_bHide)
END_DDX_MAP()

	
private:
	HCURSOR m_hCursor, m_hPrevCursor;
	//HICON hIconFinderToolFilled;
	//HICON hIconFinderToolEmpty;
	HBITMAP hBitmapFinderToolFilled;
	HBITMAP hBitmapFinderToolEmpty;
	CWndSelector *wndSelector;
	HWND m_selWindow;
	CEditImpl m_nameEdit;
	CString m_nameEditStr;
	CString m_sizeStaticStr;
	bool m_bHide;
	RECT m_mainWndRec;


	CWTButtonCtrl			buttonSet;
	CWTButtonCtrl			buttonCancel;
	CWTButtonCtrl			buttonSelectFromList;

 	CVAColoredStaticCtrl    staticCaptioSelectAplication;
	CVAColoredStaticCtrl    staticCaptionSlct;
	CVAColoredStaticCtrl	staticCaption;
	CVAColoredStaticCtrl	staticTitle;
	CVAColoredStaticCtrl	staticSize;
	CVAColoredCheckBoxStaticCtrl	staticHideClient;
	
	CWTGroupBoxCtrl			staticBack;
	CVAColoredStaticCtrl	editSize;
	CColoredEditCtrl		editTitle;
	CVAColoredCheckboxCtrl	checkHideMainWnd;

	MP_STRING staticCaptionEngText;
	MP_STRING staticHideClientEngText;
	MP_STRING staticTitleEngText;
	MP_STRING staticSizeEngText;
	MP_STRING windowCaptionText;
	MP_STRING staticTitleEngText_Or;
	MP_STRING btn_CaptionText;
	MP_STRING btnCaptionCancel;
	MP_STRING staticCaptionSelectApp;



	oms::omsContext*	context;
	bool m_englishVersion;
	CProcessListDlg* m_WndListAplication;

private:
	void SetFinderToolImage (bool bSet);
	void SetNameEdit(HWND hwnd);
	void SetSizeEdit(HWND hwnd);
	bool GetWindowSize(RECT &rec, int &width, int &height);
	bool HideMainWindow();
	bool ShowMainWindow();
	void ClearResources();
private:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		//this->DestroyWindow();
		ShowMainWindow();
		Invoke<&ISelectorWindowEvents::OnSelectorWindowOK>();
		return 0;
	}

	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		//this->DestroyWindow();
		ShowMainWindow();
		Invoke<&ISelectorWindowEvents::OnSelectorWindowCancel>();
		return 0;
	}

	
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnStnClickedStaticFinderTool(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedHideCheck(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	void OnFinalMessage(HWND /*hWnd*/);

	HWND FindParentWindow(HWND hChild);

public:
	bool notepad;
	void OnProcessListCancelled();
	void OnProcessWindowSelected(HWND ahWnd);
	LRESULT OnStnClickedStaticHideclient(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBtnSelectFromList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};



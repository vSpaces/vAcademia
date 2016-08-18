#pragma once

#include "MainWindow.h"
#include "Dictionary.h"
#include "Icon.h"
#include "IniValueFinder.h"
#include "URLCheckerVersion.h"
#include "Updater.h"

#define TIMER_DELAY 3600000 //1 hour
#define TIMER_ID -10
#define SEND_CHECKBOX_ID		(WM_USER + 5)
#define SEND_BUTTONYES_ID		(WM_USER + 4)
#define SEND_BUTTONNO_ID		(WM_USER + 3)
#define ICON_ID	(WM_USER + 2)

class CUpdateDialog: public CMainWindow
{
public:
	CUpdateDialog(const HINSTANCE &hInstance, const int &nCmdShow);
	virtual ~CUpdateDialog();
	BOOL Initialize();
	LRESULT Processor(const HWND &hWnd, const UINT &message, const WPARAM &wParam, const LPARAM &lParam); //перегрузка
	void Destroyer();
private:
	HWND CreateButton(const wstring &lpWindowName, _In_ DWORD dwStyle, const int x, const int y,
		const int nWidth, const int nHeight, _In_opt_  HMENU hMenu)const;
	void OnPressButtonYes();
	void OnPressButtonNo();
	bool CreateDictionary();
	void CreateInterface();
	void CreateIcon();
	bool CheckIgnoreBtn();
	bool CheckUpdate();

	CDictionary* m_pDictionary;
	CIcon* m_pIcon;
	HWND m_edit, m_checkbox, m_buttonYes, m_buttonNo, m_editTypeUpdate/*,m_editQuestion*/;
	HBITMAP m_hBitmap;
	HBRUSH m_hbrBkgnd;
	wstring m_urlVersion;
	string m_language;
	unsigned int m_updateType;
	bool m_bCheckUpdate;
	wstring m_ignoredVersion;
};
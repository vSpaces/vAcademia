#include "stdafx.h"
#include "UpdateDialog.h"
#include "AssistantModule.h"

#define WM_SETFONT 0x0030
#define INIFILE L"player.ini"
#define UPDATE_NOT_REQUIRED 0 //обновление не требуется
#define CAN_UPDATE 1 //желательно обновиться и есть возможность автоматического обновления через updater.exe
#define CAN_UPDATE_NOT_AUTO 2 // желательно обновиться, но не возможности сделать это автоматически через updater.exe
#define NEED_UPDATE 3 //необходимо обновиться и есть возможность автоматического обновления через updater.exe
#define NEED_FULL_UPDATE 4 //требуется полное обновление через загрузку и установку дистрибутива
#define NEED_FULL_UPDATE_AND_NOT_LOGIN 5//требуется полное обновление через загрузку и установку дистрибутива, 
//причем протокол авторизации изменился, поэтому текущая версия клиента не сможет авторизироваться

CUpdateDialog:: CUpdateDialog(const HINSTANCE &hInstance, const int &nCmdShow):
	m_edit(NULL), m_checkbox(NULL), m_buttonYes(NULL), m_buttonNo(NULL),
	m_hbrBkgnd(NULL),
	m_updateType(NULL),
	m_editTypeUpdate(NULL),
	//m_editQuestion(NULL),
	m_hBitmap(NULL),
	m_bCheckUpdate(true),
	CMainWindow(hInstance, nCmdShow)
{
	m_ignoredVersion = L"";
	wstring cfgFilePath (CUpdater::GetApplicationDataDirectory() + L"\\Vacademia\\VersionCheckerIgnore.cfg");
	FILE* cfgFile = NULL;
	_wfopen_s(&cfgFile, cfgFilePath.c_str(), L"rb");
	if(cfgFile)
	{
		WCHAR p[3124];
		fwscanf_s(cfgFile, L"%s", p, _countof(p));
		m_ignoredVersion = p;
		fclose(cfgFile);
	}
}

CUpdateDialog:: ~CUpdateDialog() {}

bool CUpdateDialog:: CreateDictionary()
{
	CIniValueFinder iniFinder(INIFILE);
	m_language = iniFinder.GetValue(L"settings", L"language");
	if(m_language == "incorrect value")
	{
		MessageBoxW(NULL, L"Can not load player.ini", L"Error", 0);
		return false;
	}

	m_pDictionary = new CDictionary();
	m_pDictionary->SetLanguage(wstring(m_language.begin(), m_language.end()));
	if(!(m_pDictionary->FillDictionary()))
	{
		MessageBoxW(NULL, L"Can not load the dictionary", L"Error", 0);
		return false;
	}else
	{
		return true;
	}
}
void CUpdateDialog:: CreateInterface()
{
	m_edit = CreateWindowExW(NULL, L"EDIT",   // predefined class 
		L"",       // button text 
		WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE,  // styles 
		100,         // starting x position 
		20,         // starting y position 
		320,        // button width 
		45,        // button height 
		GetHandle(),       // parent window 
		(HMENU)NULL,       
		(HINSTANCE) GetWindowLong(GetHandle(), GWL_HINSTANCE), 
		NULL);      // pointer not needed
	HFONT hFont=CreateFont (13, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Tahoma");
	SendMessage (m_edit, WM_SETFONT, WPARAM (hFont), TRUE);
	
	HFONT hFontBold=CreateFont (14, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Tahoma");

	m_editTypeUpdate = CreateWindowExW(NULL, L"EDIT",   // predefined class 
		L"",       // button text 
		WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE,  // styles 
		15,         // starting x position 
		76,         // starting y position 
		405,        // button width 
		40,        // button height 
		GetHandle(),       // parent window 
		(HMENU)NULL,       
		(HINSTANCE) GetWindowLong(GetHandle(), GWL_HINSTANCE), 
		NULL);      // pointer not needed
	SendMessage (m_editTypeUpdate, WM_SETFONT, WPARAM (hFontBold), TRUE);

	/*m_editQuestion = CreateWindowExW(NULL, L"EDIT",   // predefined class 
		L"",       // button text 
		WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE,  // styles 
		25,         // starting x position 
		120,         // starting y position 
		450,        // button width 
		20,        // button height 
		GetHandle(),       // parent window 
		(HMENU)NULL,       
		(HINSTANCE) GetWindowLong(GetHandle(), GWL_HINSTANCE), 
		NULL);      // pointer not needed	
	SendMessage (m_editQuestion, WM_SETFONT, WPARAM (hFontBold), TRUE);*/

	m_checkbox = CreateButton(m_pDictionary->GetText(DONT_ASK_ME), WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,
		19, 112, 400, 20, (HMENU)SEND_CHECKBOX_ID);
	SendMessage (m_checkbox, WM_SETFONT, WPARAM (hFont), TRUE);
	SendMessage(m_checkbox, BM_SETCHECK, BST_UNCHECKED, 0);

	m_buttonYes = CreateButton(m_pDictionary->GetText(UPDATE), WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		219, 140, 80, 25, (HMENU)SEND_BUTTONYES_ID);
	SendMessage (m_buttonYes, WM_SETFONT, WPARAM (hFont), TRUE);
	
	m_buttonNo = CreateButton(m_pDictionary->GetText(NO), WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		316, 140, 80, 25, (HMENU)SEND_BUTTONNO_ID);
	SendMessage (m_buttonNo, WM_SETFONT, WPARAM (hFont), TRUE);

	wstring tmpWstr = GetRootDir();
	//string rootDir(tmpWstr.begin(), tmpWstr.end());
	tmpWstr += L"\\logo1.bmp";
	m_hBitmap = (HBITMAP)LoadImageW(GetHinst(), tmpWstr.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

HWND CUpdateDialog:: CreateButton(const wstring &lpWindowName, _In_ DWORD dwStyle, const int x, const int y,
								  const int nWidth, const int nHeight, _In_opt_  HMENU hMenu)const
{
	HWND button = CreateWindowExW(NULL, L"BUTTON", lpWindowName.c_str(), dwStyle,
		x, y, nWidth, nHeight, GetHandle(),	hMenu,
		(HINSTANCE) GetWindowLong(GetHandle(), GWL_HINSTANCE), NULL);
	return button;
}
void CUpdateDialog:: CreateIcon()
{
	HICON hIcon = ::LoadIcon(GetHinst(), MAKEINTRESOURCE(IDI_ICON1));
	m_pIcon = new CIcon(GetHandle(), hIcon, 0, m_pDictionary->GetText(NO_NEW_UPDATES));
}
BOOL CUpdateDialog:: Initialize()
{
	if (!IsCorrectInstance())
	{
		return FALSE;
	}

	if(!CreateDictionary())
	{
		return FALSE;
	}
	CreateInterface();
	CreateIcon();
	SetTitle(m_pDictionary->GetText(TITLE));
	
	SetTimer(GetHandle(), TIMER_ID, 300000, NULL);
	//SetTimer(GetHandle(), TIMER_ID, TIMER_DELAY, NULL);
	DoMessageLoop();
	return TRUE;
}
void CUpdateDialog:: OnPressButtonYes()
{
	if(m_updateType == CAN_UPDATE_NOT_AUTO || m_updateType == NEED_FULL_UPDATE || m_updateType == NEED_FULL_UPDATE_AND_NOT_LOGIN)
	{
		CIniValueFinder iniFinder(INIFILE);
		string domain = iniFinder.GetValue(L"settings", L"domain");
		ShellExecute(GetHandle(), "open", (string("http://www." + domain + "/site/downloadclient")).c_str(),
			NULL, NULL, SW_SHOWDEFAULT);
		DestroyWindow(GetHandle());
	}else
	{
		CUpdater updater(GetHandle());
		updater.Update();
		Show(SW_HIDE);
	}
}
void CUpdateDialog:: OnPressButtonNo()
{
	CheckIgnoreBtn();
	Show(SW_HIDE);
}
void CUpdateDialog:: Destroyer()
{
	KillTimer(GetHandle(), TIMER_ID);
	if(m_pIcon)
	{
		delete m_pIcon;
	}
	if(m_pDictionary)
	{
		delete m_pDictionary;
	}
}
LRESULT CUpdateDialog:: Processor(const HWND &hWnd, const UINT &message, const WPARAM &wParam, const LPARAM &lParam)
{
	int wmId, wmEvent;
	switch(message)
	{
	case WM_PAINT:
		PAINTSTRUCT 	ps;
		HDC 			hdc;
		BITMAP 			bitmap;
		HDC 			hdcMem;
		HGDIOBJ 		oldBitmap;

		hdc = BeginPaint(hWnd, &ps);
		//SetBkColor(hdc, RGB(16, 85, 188));
		hdcMem = CreateCompatibleDC(hdc);
		oldBitmap = SelectObject(hdcMem, m_hBitmap);

		GetObject(m_hBitmap, sizeof(bitmap), &bitmap);
		//BitBlt(hdc, 30, 25, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
		BitBlt(hdc, 13, 10, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, oldBitmap);
		DeleteDC(hdcMem);

		EndPaint(hWnd, &ps);
		break;
	case WM_TIMER:
		SetTimer(GetHandle(), TIMER_ID, TIMER_DELAY, NULL);
		CheckUpdate();
		break;
	case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC) wParam;
			HWND hElem = (HWND) lParam;
			if( m_editTypeUpdate == hElem)
			{
				SetTextColor(hdcStatic, RGB(254,255,157));
			}
			else
				SetTextColor(hdcStatic, RGB(255,255,255));
			SetBkColor(hdcStatic, RGB(16, 85, 188));

			if (m_hbrBkgnd == NULL)
			{
				m_hbrBkgnd = CreateSolidBrush(RGB(16, 85, 188));
			}
			return (INT_PTR)m_hbrBkgnd;
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case SEND_BUTTONYES_ID:
			{
				OnPressButtonYes();
			}			
			break;
		case SEND_BUTTONNO_ID:
			{
				OnPressButtonNo();
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		KillTimer(GetHandle(), TIMER_ID);
		delete m_pIcon;
		delete m_pDictionary;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

bool CUpdateDialog::CheckUpdate()
{
	CIniValueFinder iniFinder(INIFILE);
	string iniBuildtype = iniFinder.GetValue(L"settings", L"buildtype");

	if (iniBuildtype == "base")
	{
		string ip = GetIP("http://mmlab.ru/get_vacademia_ip.php");
		if (ip != "")
		{			
			wstring ipAddress = wstring(ip.begin(), ip.end());
			iniFinder.SetValue(L"paths", L"server", ipAddress.c_str());

			std::wstring path = CUpdater::GetApplicationDataDirectory();
			path += L"\\new_va_ip";

			FILE* fl = _wfopen(path.c_str(), L"w");
			if (fl != NULL)
			{
				fputs(ip.c_str(), fl);
				fclose(fl);
			}
		}
	}
	
	if(!m_bCheckUpdate)
		return false;

	string serverForUpdate;
	string iniVersion;
	
	bool isServerNotAvailable;

	
	serverForUpdate = iniFinder.GetValue(L"paths", L"server");
	iniVersion = iniFinder.GetValue(L"settings", L"version");	

	string str;
	str.assign(serverForUpdate.begin(), serverForUpdate.end());
	CURLCheckerVersion URLcheckerVersion;
	if(!(URLcheckerVersion.Check("http://" + str + "/vu/php_0_1/getVersion.php", iniVersion, iniBuildtype)))
	{
		return false;
	}
	string urlVersion = URLcheckerVersion.GetVersion();
	m_urlVersion = wstring(urlVersion.begin(), urlVersion.end());
	m_updateType = URLcheckerVersion.GetUpdateType();
	isServerNotAvailable = URLcheckerVersion.IsServerNotAvailable();

	if(!isServerNotAvailable && m_updateType != UPDATE_NOT_REQUIRED)
	{
		if(m_urlVersion == L"" || m_urlVersion == m_ignoredVersion)
		{
			return false;
		}
		wstring text = m_pDictionary->GetText(NEW_UPDATE);
		wstring textTypeUpdate;
		wstring textQuestion;		
		text += L".\r\n";
		text += m_pDictionary->GetText(CURR_VER);
		text += L": ";
		text += wstring(iniVersion.begin(), iniVersion.end());
		text += L"\r\n";
		text += m_pDictionary->GetText(NEW_VER);
		text += L": ";
		text += m_urlVersion;
		if(m_updateType == CAN_UPDATE)
		{
			textTypeUpdate += m_pDictionary->GetText(DESIRED_UPDATE);
		}else if(m_updateType == CAN_UPDATE_NOT_AUTO)
		{
			textTypeUpdate += m_pDictionary->GetText(DESIRED_UPDATE);
			textTypeUpdate += L". " + m_pDictionary->GetText(UPDATE_FROM_SITE);
			//textTypeUpdate += L".\r\n";
		}else if(m_updateType == NEED_UPDATE)
		{
			textTypeUpdate += m_pDictionary->GetText(REQUIRED_UPDATE);
		}else if(m_updateType == NEED_FULL_UPDATE)
		{
			textTypeUpdate += m_pDictionary->GetText(REQ_FULL_UPDATE);
			textTypeUpdate += L". " + m_pDictionary->GetText(UPDATE_FROM_SITE);
			//textTypeUpdate += L".\r\n";
		}else
		{
			textTypeUpdate += m_pDictionary->GetText(REQ_FULL_UPDATE);
			textTypeUpdate += L". " + m_pDictionary->GetText(CURR_VER_CANT_LOGIN);
			//textTypeUpdate += L".\r\n";
			textTypeUpdate += L". " + m_pDictionary->GetText(UPDATE_FROM_SITE);
			//textTypeUpdate += L".\r\n";			
		}
		/*textQuestion*/
		textTypeUpdate += L". ";
		textTypeUpdate += m_pDictionary->GetText(Q_UPDATE);
		SetWindowTextW(m_edit, text.c_str());
		SetWindowTextW(m_editTypeUpdate, textTypeUpdate.c_str());
		//SetWindowTextW(m_editQuestion, textQuestion.c_str());

		m_pIcon->SetMessage(m_pDictionary->GetText(NEW_UPDATE).c_str());
		Show(SW_SHOW);
		return true;
	}else
	{
		return false;
	}
}

bool CUpdateDialog::CheckIgnoreBtn()
{
	m_bCheckUpdate = false;	
	LRESULT res = SendMessage(m_checkbox, BM_GETCHECK, 0, 0);
	if(res == BST_CHECKED)
	{
		m_ignoredVersion = m_urlVersion;
		wstring cfgFilePath (CUpdater::GetApplicationDataDirectory() + L"\\Vacademia\\VersionCheckerIgnore.cfg");
		FILE* cfgFile = NULL;
		_wfopen_s(&cfgFile, cfgFilePath.c_str(), L"wb");
		if(cfgFile)
		{
			fwprintf_s(cfgFile, L"%s", wstring(m_ignoredVersion.begin(), m_ignoredVersion.end()).c_str());
		}else
		{
			return false;
		}
		fclose(cfgFile);
		m_pIcon->SetMessage(m_pDictionary->GetText(NO_NEW_UPDATES));
	}
	KillTimer(GetHandle(), TIMER_ID);
	return true;
}

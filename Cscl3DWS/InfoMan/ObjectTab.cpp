#include "stdafx.h"
#include "ObjectTab.h"
#include ".\objecttab.h"
#include "shellapi.h"

CObjectTab::CObjectTab(){
	mBuilding = false;
	objectServerConnectionIsFreezed = false;
	resServerConnectionIsFreezed = false;
	voipServerConnectionIsFreezed = false;
	serviceServerConnectionIsFreezed = false;
};

CObjectTab::~CObjectTab(){
	//m_Tmpl.SetResourceInstance( GetModuleHandle(NULL));
	if( IsWindow())
	{
		DestroyWindow();
		m_hWnd = 0;
	}
};

LRESULT CObjectTab::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	return 0;
}


LRESULT CObjectTab::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SetWindowPos( 0, &rcDefault, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE |SWP_HIDEWINDOW);
	CButton checkFiltr( GetDlgItem( IDC_CHECK_SYNCH));
	objectTree.Attach( GetDlgItem( IDC_TREE_OBJECT));
	updatePropBitton.Attach( GetDlgItem( IDC_PROP_UPDATE_BUTTON));
	openSrcBitton.Attach( GetDlgItem( IDC_BUTTON_OPEN_SRC));
	updateBinStatePropBitton.Attach( GetDlgItem( IDC_UPDATE_BINSTATE));
	enumateNetworksBreaks.Attach( GetDlgItem( IDC_EMULATE_NETWORK_BREAKS));
	updateBinStatePropBitton.EnableWindow(false);
	//compInfoToBufferButton.Attach( GetDlgItem( IDC_BUTTON_COMPINFO_TO_BUFFER));
	checkFiltr.SetCheck( true);
	CButton checkFiltr2( GetDlgItem( IDC_CHECK_CHARACTER));
	checkFiltr2.SetCheck( true);
	mBuilding = true;
	objectTree.SetFilter( 3);
	mBuilding = false;

	mProperties.Attach( GetDlgItem( IDC_EDIT_PROPERTIES));
	mProperties.SetReadOnly( TRUE);	

	bHandled = FALSE;
	return 0;
}

LRESULT CObjectTab::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

void CObjectTab::AddObject()
{
	//objectTree.AddObject();
}

char* WC2MB2(const wchar_t* apwc){
	int iSLen=wcslen(apwc);
	char* pch = MP_NEW_ARR( char, iSLen*MB_CUR_MAX+1);
	wcstombs(pch, apwc, iSLen);
	pch[iSLen]=0;
	return pch;
}

void CObjectTab::SetSynchProperties(const std::wstring& info)
{
	if (info.find(L"binState:")!= std::wstring::npos)
	{
		updateBinStatePropBitton.EnableWindow(true);
	}
	else
	{
		updateBinStatePropBitton.EnableWindow(false);
	}

	char* sProperties = WC2MB2(info.c_str());
	mProperties.SetWindowText( sProperties);
	MP_DELETE_ARR( sProperties);
}

LRESULT CObjectTab::OnTvnSelchangedTreeObject(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if (mBuilding)
		return 0;
	mpObjectInfo = (mlObjectInfo *) objectTree.GetItemData(pNMTreeView->itemNew.hItem);
	
	if(mpObjectInfo != NULL)
	{
		SetSynchProperties( pContext->mpSM->GetJsonFormatedProps(mpObjectInfo->src.c_str()));
	}
	else
	{
		SetSynchProperties( L"");
	}
	return 0;
}

LRESULT CObjectTab::OnBnClickedButtonUpdate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	mBuilding = true;
	objectTree.Build();
	mBuilding = false;
	mpObjectInfo = NULL;
	return 0;
}

LRESULT CObjectTab::OnBnClickedCheckCharacter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CButton checkFiltr( GetDlgItem( IDC_CHECK_CHARACTER));
	int check = checkFiltr.GetCheck() == true ? 2 : ~2;
	if( check == 2)
		check |= objectTree.GetFilter();
	else
		check &= objectTree.GetFilter();
	mBuilding = true;
	objectTree.SetFilter( check);
	mBuilding = false;
	return 0;
}

LRESULT CObjectTab::OnBnClickedCheckSynch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CButton checkFiltr( GetDlgItem( IDC_CHECK_SYNCH));
	int check = checkFiltr.GetCheck() == true ? 1 : ~1;
	if( check == 1)
		check |= objectTree.GetFilter();
	else
		check &= objectTree.GetFilter();
	mBuilding = true;
	objectTree.SetFilter( check);
	mBuilding = false;
	return 0;
}

LRESULT CObjectTab::OnBnClickedButtonFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CEdit findFiltr( GetDlgItem( IDC_EDIT_FIND));
	int len = findFiltr.GetWindowTextLength();
	char *lpszStringBuf = MP_NEW_ARR( char, len+2);
	findFiltr.GetWindowText(lpszStringBuf, len+1);
	objectTree.Find( lpszStringBuf);
	MP_DELETE_ARR( lpszStringBuf);
	return 0;
}

LRESULT CObjectTab::OnBnClickedPropUpdateButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(mpObjectInfo != NULL)
	{
		SetSynchProperties( pContext->mpSM->GetJsonFormatedProps(mpObjectInfo->src.c_str()));
	}
	return 0;
}

LRESULT CObjectTab::OnBnClickedPropUpdateButton2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here

	return 0;
}

LRESULT CObjectTab::OnBnClickedRichEditLog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here

	return 0;
}

LRESULT CObjectTab::OnBnClickedUpdateBinState(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(mpObjectInfo != NULL)
	{
		pContext->mpSM->TransposeBinState(mpObjectInfo->src.c_str());
	}

	return 0;
}

LRESULT CObjectTab::OnBnClickedFreezeVoipServerConnection(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(pContext->mpComMan)
	{
		voipServerConnectionIsFreezed = !voipServerConnectionIsFreezed;

		if( voipServerConnectionIsFreezed)
		{
			pContext->mpComMan->DropAndFreezeConnection(0x1619);
			::SetWindowText( GetDlgItem( IDC_BREAK_VOIP_SERVER_CONNECTION), "Rest Voip");
		}
		else
		{
			pContext->mpComMan->UnfreezeConnection(0x1619);
			::SetWindowText( GetDlgItem( IDC_BREAK_VOIP_SERVER_CONNECTION), "Drop Voip");
		}
	}
	return 0;
}

LRESULT CObjectTab::OnBnClickedFreezeServiceServerConnection(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(pContext->mpComMan)
	{
		serviceServerConnectionIsFreezed = !serviceServerConnectionIsFreezed;

		if( serviceServerConnectionIsFreezed)
		{
			pContext->mpComMan->DropAndFreezeConnection(0x4554);
			pContext->mpComMan->DropAndFreezeConnection(0x4774);
			::SetWindowText( GetDlgItem( IDC_BREAK_SERVICE_SERVER_CONNECTION), "Rest Service");
		}
		else
		{
			pContext->mpComMan->UnfreezeConnection(0x4554);
			pContext->mpComMan->UnfreezeConnection(0x4774);
			::SetWindowText( GetDlgItem( IDC_BREAK_SERVICE_SERVER_CONNECTION), "Drop Service");
		}
	}
	return 0;
}

LRESULT CObjectTab::OnBnClickedFreezeResServerConnection(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(pContext->mpComMan)
	{
		resServerConnectionIsFreezed = !resServerConnectionIsFreezed;

		if( resServerConnectionIsFreezed)
		{
			pContext->mpComMan->DropAndFreezeConnection(0x1609);
			pContext->mpComMan->DropAndFreezeConnection(0x9753);
			pContext->mpComMan->DropAndFreezeConnection(0x1111);
			::SetWindowText( GetDlgItem( IDC_BREAK_RES_SERVER_CONNECTION), "Rest Res");
		}
		else
		{
			pContext->mpComMan->UnfreezeConnection(0x1609);
			pContext->mpComMan->UnfreezeConnection(0x9753);
			pContext->mpComMan->UnfreezeConnection(0x1111);
			::SetWindowText( GetDlgItem( IDC_BREAK_RES_SERVER_CONNECTION), "Drop Res");
		}
	}
	return 0;
}

LRESULT CObjectTab::OnBnClickedFreezeObjectServerConnection(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(pContext->mpComMan)
	{
		objectServerConnectionIsFreezed = !objectServerConnectionIsFreezed;

		if( objectServerConnectionIsFreezed)
		{
			pContext->mpComMan->DropAndFreezeConnection(0x4554);
			::SetWindowText( GetDlgItem( IDC_BREAK_OBJECT_SERVER_CONNECTION), "Drop Obj");
		}
		else
		{
			pContext->mpComMan->UnfreezeConnection(0x4554);
			::SetWindowText( GetDlgItem( IDC_BREAK_OBJECT_SERVER_CONNECTION), "Drop Obj");
		}
	}
	return 0;
}

LRESULT CObjectTab::OnBnClickedEnumateNWBs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	wchar_t pwcValue[3000] = L"";
	if(pContext->mpApp)
	{
		if( pContext->mpApp->GetSetting(L"network_breaks_delay", pwcValue, countof(pwcValue), L"settings"))
		{
			int delay = _wtoi( pwcValue);
			pContext->mpComMan->SetNetworkBreaksEmulationDelay( 1000 * delay, 0);
		}
	}
	return 0;
}


// lstrrchr (avoid the C Runtime )
TCHAR * lstrrchr(LPCTSTR string, int ch)
{
	char *start = (char *)string;

	while (*string++)                       /* find end of string */
		;
	/* search towards front */
	while (--string != start && *string != (TCHAR) ch)
		;

	if (*string == (TCHAR) ch)                /* char found ? */
		return (TCHAR *)string;

	return NULL;
}

WCHAR * lstrrchrW(LPCWSTR string, WCHAR ch)
{
	wchar_t *start = (wchar_t *)string;

	while ((*string) != '\0')                       /* find end of string */
	{
		string++;
	}
	/* search towards front */
	while (--string != start && *string != ch)
		;

	if (*string == ch)                /* char found ? */
		return (WCHAR *)string;

	return NULL;
}

LRESULT CObjectTab::OnBnClickedButtonOpenSrc(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	CWComString sysCommand;

	WCHAR	moduleFileName[ MAX_PATH] = {L'\0'};
	GetModuleFileNameW( NULL, moduleFileName, MAX_PATH);

	LPWSTR lpFileName = NULL;
	WCHAR	modulePath[ 2*MAX_PATH] = {L'\0'};
	::GetFullPathNameW(moduleFileName,sizeof(modulePath)/sizeof(modulePath[0]),modulePath,&lpFileName);
	//if( lpFileName != NULL)
	//{
		//_tcscpy(lpFileName, cLPCSTR(apwcSysCommand));
	//	*lpFileName = 0;
	//}
	WCHAR *lastperiod = lstrrchrW(modulePath, L'.');
	if (lastperiod)
		lastperiod[0] = 0;
	wcscat( modulePath, L".ini");

	sysCommand.Format( L"vsdumps:///?file=%s", modulePath);
	int iError = 0;
	HINSTANCE  res = ShellExecuteW( 0, L"open", sysCommand.GetBuffer(), L"", modulePath, SW_SHOWNORMAL);
	if( (int)res <= 32)
	{
		res = ShellExecuteW( 0, L"open", L"iexplore" , sysCommand.GetBuffer(), modulePath, SW_SHOWNORMAL);
		if( (int)res <= 32)
		{
			//GetError();
			iError = GetLastError();
			//m_pWS3D->GetError();
		}	
	}

	return 0;
}

LRESULT CObjectTab::OnBnClickedButtonCompInfoToBuffer(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CWComString sCompInfo = L"1. Описание:							\r\n";
	sCompInfo += L"2. Процедура повторения:		\r\n		\r\n		\r\n";
	CWComString s;
	CWComString sData;
	if(pContext->mpApp)
		pContext->mpApp->GetVersion( sData.GetBufferSetLength(256), 256);
	long sessionID = 0;
	if(pContext->mpComMan)
		sessionID = pContext->mpComMan->GetSessionID();
	sCompInfo += s.GetBuffer();
	char pchValue[3000] = "";
	if(pContext->mpApp)
		pContext->mpApp->GetSettingFromIni("paths", "server", pchValue, 2995);
	USES_CONVERSION;
	CComString sServerIP = pchValue;
	s.Format(L"3. Билд: vacademia_%s\r\n4. Сессия %d\r\n5. Сервер %s\r\n", sData.GetBuffer(), sessionID, A2W(pchValue));
	sCompInfo += s.GetBuffer();
	s.Empty();

	char computerName[501];
	unsigned long size = 500;
	GetComputerName((LPTSTR)&computerName, &size);

	std::wstring wIP = L"";
	if(pContext->mpComMan)
		wIP = pContext->mpComMan->GetMyIP();
	s.Format(L"6. Компьютер:%s,  IP: %s\r\n", A2W(computerName), wIP.c_str());
	sCompInfo += s.GetBuffer();
	s.Empty();
	sCompInfo += L"7. Комп ауд:		,№   \r\n";		

	if(pContext->mpLogWriter)
	{	
		CComString s1;
		CComString sLogPath;
		if( sServerIP.Find("192.168.")==-1)
		{
			sLogPath.Format("http://%s/vu/php_0_1/LoadLog.php?sessionID=%d", sServerIP.GetBuffer(), sessionID);
		}
		else
		{
			sLogPath = pContext->mpLogWriter->GetLogFilePath().c_str();
		}
		s1.Format("8. Лог %s\r\n", sLogPath.GetBuffer());
		sCompInfo += A2W(s1.GetBuffer());
	}
	s.Empty();
	if(pContext->mpServiceWorldMan != NULL)
		sData = pContext->mpSM->GetClientStringID();
	s.Format(L"9. Логин: %s\r\n", sData.GetBuffer());
	sCompInfo += s.GetBuffer();	
	s.Empty();
	sCompInfo += L"10. Процедура тестирования:		\r\n";
		
	int err = 0;
	if(!::OpenClipboard( m_hWnd))
	{
		err = GetLastError();
	}
	if(!::EmptyClipboard())
	{
		err = GetLastError();
	}
	
	//CComString sBuffer = sCompInfo.GetBuffer();
	HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (sCompInfo.GetLength() + 1) * sizeof(wchar_t));
	if (hglbCopy == NULL)
	{
		::CloseClipboard();
		return FALSE;
	}
	wchar_t *lptstrCopy = (wchar_t*)GlobalLock(hglbCopy);
	memcpy(lptstrCopy, sCompInfo.GetBuffer(), sCompInfo.GetLength() * sizeof(wchar_t));
	lptstrCopy[sCompInfo.GetLength()] = (wchar_t) 0;    // null character 
	GlobalUnlock(hglbCopy);

	// Помещаем дескриптор в буфер обмена.
	if( ::SetClipboardData(CF_UNICODETEXT, hglbCopy) == NULL)
	{
		err = GetLastError();
	}
	if(!::CloseClipboard())
	{
		err = GetLastError();
	}
	return 0;
}

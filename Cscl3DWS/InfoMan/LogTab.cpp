#include "stdafx.h"
#include "LogTab.h"
#include ".\objecttab.h"
#include ".\logtab.h"
#include <fstream>

CLogTab::CLogTab():
MP_MAP_INIT(mFiltersMap)
{
};

CLogTab::~CLogTab(){
	if( IsWindow())
	{
		DestroyWindow();
		m_hWnd = NULL;
	}
};

LRESULT CLogTab::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	SetWindowPos( 0, &rcDefault, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);

	m_EditLog.Attach( GetDlgItem( IDC_RICHEDIT_LOG));

	HWND hwndCheck = GetDlgItem(IDC_CHECK_DEFAULT);
	DWORD dCheck=::SendMessage(hwndCheck, BM_SETCHECK, BST_CHECKED, 0);
	mFiltersMap[ltDefault] = true;
	LoadLogFromFile();
	if (mpOmsContext != NULL)
	{
		DWORD dCheck=::SendMessage( GetDlgItem(IDC_CHECK_AUTOUPDATE), BM_SETCHECK, BST_CHECKED, 0);
		mpOmsContext->mpLogWriter->StartSendingToInfoMan( mpInfoManager);
	}

	::SendMessage( m_EditLog.m_hWnd, EM_SETSEL, 0, 0);

	m_EditSearch.Attach( GetDlgItem( IDC_EDIT_FIND));
	n_ButtonSearch.Attach( GetDlgItem( IDC_BUTTON_FIND));
	bHandled = FALSE;
	return 0;
}

LRESULT CLogTab::OnBnClickedButtonFind(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	char buf[1024];

	m_EditSearch.GetLine(0, &buf[0], 1024);
	std::string findSubString = buf;

	LRESULT lLength=::SendMessage( m_EditLog.m_hWnd, WM_GETTEXTLENGTH, 0, 0);

	DWORD dwStartPos=0;
	DWORD dwCarretPos=0;
	::SendMessage( m_EditLog.m_hWnd, EM_GETSEL,(WPARAM)&dwStartPos,(LPARAM)&dwCarretPos);
	mLastFindPos = dwStartPos;

	FINDTEXT ft;
	ft.lpstrText = buf;
//	ft.chrg.cpMin = mLastFindPos;
	ft.chrg.cpMin = dwStartPos;
	ft.chrg.cpMax = lLength;

	LRESULT lPos = ::SendMessage(m_EditLog.m_hWnd, EM_FINDTEXT, (WPARAM)FR_DOWN, (LPARAM)&ft);
	if (lPos != -1)
	{
		m_EditLog.SetFocus();
		::SendMessage( m_EditLog.m_hWnd, EM_SETSEL, lPos, lPos + findSubString.length());
		mLastFindPos = lPos + 1;
	}
	else
	{
		mLastFindPos = 0;
	}

	ScrollToSelected();

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckAutoUpdate(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	if (IsDlgButtonChecked(IDC_CHECK_AUTOUPDATE) == BST_CHECKED)
		mpOmsContext->mpLogWriter->StartSendingToInfoMan( mpInfoManager);
	else
		mpOmsContext->mpLogWriter->StopSendingToInfoMan();

	return 0;
}

void CLogTab::Trace(const std::string& text, LOG_TYPE logType)
{
	/*
	Закоментарил - так как НИРАЗУ не читал лог в этом инструменте. Даже не знал что он тут есть.

	Также..
	Закоментарил чтение лога с файла. Причина - при большом файле долго читает лог. 
	3 мб файл не успевает прочитаться за время определения повисания. См. краш 5564
	*/

	//TraceImpl( text, logType);
}

void CLogTab::TraceImpl(const std::string& text, LOG_TYPE logType)
{
	if (mFiltersMap.find(logType) == mFiltersMap.end())
		return;
	if (!mFiltersMap[logType])
		return;

	HWND hwnd = m_EditLog.m_hWnd;

	LRESULT lLength=::SendMessage( hwnd, WM_GETTEXTLENGTH, 0, 0);

	//set selection for insertion
	::SendMessage( hwnd, EM_SETSEL, lLength, lLength);

	// insert text
	const char * begin = text.c_str();
	::SendMessage( hwnd, EM_REPLACESEL, 0, (LPARAM)begin);


	lLength=::SendMessage(hwnd,WM_GETTEXTLENGTH,0,0);
	::SendMessage(hwnd,EM_SETSEL,(WPARAM)lLength,(LPARAM)lLength);
	::SendMessage(hwnd,EM_SCROLLCARET,0,0);

}

LRESULT CLogTab::OnBnClickedGroupFilters(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckSoundMan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_SOUND_MAN) == BST_CHECKED)
		mFiltersMap[ltSoundMan] = true;
	else
		mFiltersMap[ltSoundMan] = false;
    
	return 0;
}

LRESULT CLogTab::OnBnClickedCheckRenderMan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_RENDER_MAN) == BST_CHECKED)
		mFiltersMap[ltRenderManager] = true;
	else
		mFiltersMap[ltRenderManager] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckComMan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_COM_MAN) == BST_CHECKED)
		mFiltersMap[ltComMan] = true;
	else
		mFiltersMap[ltComMan] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckAvatarEditor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_AVATAR_EDITOR) == BST_CHECKED)
		mFiltersMap[ltAvatarEditor] = true;
	else
		mFiltersMap[ltAvatarEditor] = false;

	return 0;
}

void CLogTab::UpdateEditFilters()
{
		
}

void CLogTab::ScrollToSelected()
{
	DWORD dwStartPos=0;
	DWORD dwCarretPos=0;
	::SendMessage( m_EditLog.m_hWnd, EM_GETSEL,(WPARAM)&dwStartPos,(LPARAM)&dwCarretPos);
	LRESULT lLength = ::SendMessage( m_EditLog.m_hWnd,WM_GETTEXTLENGTH,0,0);
	::SendMessage( m_EditLog.m_hWnd, EM_SCROLLCARET, 0, 0);
}

void CLogTab::LoadLogFromFile()
{
	char buf[101024];
//	int logType;
	std::string text;

	mpOmsContext->mpLogWriter->StopWriteLog();
	/*std::string filePath = 	mpOmsContext->mpLogWriter->GetLogFilePath();

	std::ifstream flog(filePath.c_str());
	while (!flog.eof())
	{
		flog.getline(buf, 101024);
//		sscanf(&buf[0], "%d", &logType);
		text = buf;
		text +=	"\r\n";
//		int pos = text.find("\t");
//		text = text.substr(pos + 1, text.length() - pos - 1) + "\n";
		Trace( text, ltDefault);
	}
	flog.close();*/

	/*
	Закоментарил чтение лога с файла. Причина - при большом файле долго читает лог. 
	3 мб файл не успевает прочитаться за время определения повисания. См. краш 5564
	*/
	TraceImpl( "Здесь лога нет. См. комментарий в коде CLogTab::LoadLogFromFile()", ltDefault);

	mpOmsContext->mpLogWriter->ContinueWriteLog();
}

LRESULT CLogTab::OnBnClickedCheckDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_DEFAULT) == BST_CHECKED)
		mFiltersMap[ltDefault] = true;
	else
		mFiltersMap[ltDefault] = false;

	return 0;
}



LRESULT CLogTab::OnBnClickedCheckInfoMan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_INFO_MAN) == BST_CHECKED)
		mFiltersMap[ltInfoMan] = true;
	else
		mFiltersMap[ltInfoMan] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckMdump(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_MDUMP) == BST_CHECKED)
		mFiltersMap[ltMdump] = true;
	else
		mFiltersMap[ltMdump] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckNtexture(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_NTEXTURE) == BST_CHECKED)
		mFiltersMap[ltNtexture] = true;
	else
		mFiltersMap[ltNtexture] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckOmsplayer(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_OMSPLAYER) == BST_CHECKED)
		mFiltersMap[ltOMSPlayer] = true;
	else
		mFiltersMap[ltOMSPlayer] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckPathRefiner(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_PATH_REFINER) == BST_CHECKED)
		mFiltersMap[ltPathRefiner] = true;
	else
		mFiltersMap[ltPathRefiner] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckResloader(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_RESLOADER) == BST_CHECKED)
		mFiltersMap[ltResLoader] = true;
	else
		mFiltersMap[ltResLoader] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckRmml(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_RMML) == BST_CHECKED)
		mFiltersMap[ltRmml] = true;
	else
		mFiltersMap[ltRmml] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckServiceMan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_SERVICE_MAN) == BST_CHECKED)
		mFiltersMap[ltServiceMan] = true;
	else
		mFiltersMap[ltServiceMan] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckSyncMan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_SYNC_MAN) == BST_CHECKED)
		mFiltersMap[ltSyncMan] = true;
	else
		mFiltersMap[ltSyncMan] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckVasencoder(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_VASENCODER) == BST_CHECKED)
		mFiltersMap[ltVASEncoder] = true;
	else
		mFiltersMap[ltVASEncoder] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckVideo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_VIDEO) == BST_CHECKED)
		mFiltersMap[ltVideo] = true;
	else
		mFiltersMap[ltVideo] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckVoipMan(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_VOIP_MAN) == BST_CHECKED)
		mFiltersMap[ltVoipMan] = true;
	else
		mFiltersMap[ltVoipMan] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckNphysics(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_NPHYSICS) == BST_CHECKED)
		mFiltersMap[ltNphysics] = true;
	else
		mFiltersMap[ltNphysics] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckSomeLogType1(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_SOME_LOG_TYPE1) == BST_CHECKED)
		mFiltersMap[ltReserved1] = true;
	else
		mFiltersMap[ltReserved1] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckSomeLogType2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_SOME_LOG_TYPE2) == BST_CHECKED)
		mFiltersMap[ltReserved2] = true;
	else
		mFiltersMap[ltReserved2] = false;

	return 0;
}

LRESULT CLogTab::OnBnClickedCheckSomeLogType3(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (IsDlgButtonChecked(IDC_CHECK_SOME_LOG_TYPE3) == BST_CHECKED)
		mFiltersMap[ltReserved3] = true;
	else
		mFiltersMap[ltReserved3] = false;

	return 0;
}

#include "stdafx.h"
#include "DialogError.h"
#include "ReportMan.h"

CDialogError::CDialogError()
{
	m_bCreatingDumpAndSending = TRUE;
	m_bAppRestart = FALSE;
	m_iLanguage = RUS;
}

CDialogError::~CDialogError()
{
	if ( IsWindow())
	{
		DestroyWindow();
		m_hWnd = 0;
	}
}

LRESULT CDialogError::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	ShowWindow( SW_HIDE);
	return 0;
}

LRESULT CDialogError::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// TODO: Add your message handler code here and/or call default
	m_bCreatingDumpAndSending = TRUE;

	if ( m_iLanguage == RUS)
	{
		SetWindowTextW( m_hWnd, L"Обнаружена ошибка");
		CWindow wndLabel1 = GetDlgItem( IDC_STATIC_1);
		SetWindowTextW( wndLabel1.m_hWnd, L"Произошла ошибка в приложении 'Виртуальная академия'. Хотите помочь разработчикам? Нажмите кнопку 'Да'. В этом случае программа произведет сбор необходимой информации по ошибке и отправит ее разработчикам.");
		CWindow wndLabel2 = GetDlgItem( IDC_STATIC_2);
		SetWindowTextW( wndLabel2.m_hWnd, L"Никакая конфиденциальная информация не будет передана.");
		CWindow wndLabel3 = GetDlgItem( IDC_STATIC_3);
		SetWindowTextW( wndLabel3.m_hWnd, L"Если вы нажмете кнопку «Нет», то никакая информация отправлена не будет.");
		CWindow wndCheck = GetDlgItem(IDC_CHECK_HIDEWND);		
		SetWindowTextW( wndCheck.m_hWnd, L"Не показывать больше данное окно при повторении ошибки");
	}
	else 
	{
		SetWindowTextW( m_hWnd, L"An error was detected");
		CWindow wndLabel1 = GetDlgItem( IDC_STATIC_1);
		SetWindowTextW( wndLabel1.m_hWnd, L"An error has occurred in \"Virtual Academia\". Click \"yes\" and the system will collect all the necessary information about the error and send it to programmers. We really appreciate your assistance.");
		CWindow wndLabel2 = GetDlgItem( IDC_STATIC_2);
		SetWindowTextW( wndLabel2.m_hWnd, L"No confidential information will be revealed.");
		CWindow wndLabel3 = GetDlgItem( IDC_STATIC_3);
		SetWindowTextW( wndLabel3.m_hWnd, L"If you click the \"No\" the information won’t be sent.");
		CWindow wndCheck = GetDlgItem(IDC_CHECK_HIDEWND);		
		SetWindowTextW( wndCheck.m_hWnd, L"Don’t show this window again when the same error occurs");
	}

	CWindow wndAppRestartCheck = GetDlgItem(IDC_CHECK_RESTART);
	wndAppRestartCheck.SendMessage( BM_SETCHECK, 1, 1);
	if ( m_iLanguage == RUS)
		SetWindowTextW( wndAppRestartCheck.m_hWnd, L"Автоматически перезапустить приложение виртуальная академия");
	else
		SetWindowTextW( wndAppRestartCheck.m_hWnd, L"Restart \"Virtual Academia\"");

	RECT rect;
	this->GetClientRect( &rect);
	CWindow wndBtnOk = GetDlgItem(IDC_BUTTON_YES);
	RECT rectBtnOk;	
	wndBtnOk.GetClientRect( &rectBtnOk);
	rectBtnOk.left = (rect.right - rect.left) / 2 - ( rectBtnOk.right - rectBtnOk.left);
	wndBtnOk.SetWindowPos( m_hWnd, &rectBtnOk, SWP_SHOWWINDOW);
	if ( m_iLanguage == RUS)
		SetWindowTextW( wndBtnOk.m_hWnd, L"Да");
	else
		SetWindowTextW( wndBtnOk.m_hWnd, L"Yes");
	//wndBtnOk.Set

	CWindow wndBtnCancel = GetDlgItem(IDC_BUTTON_NO);
	RECT rectBtnCancel;	
	wndBtnCancel.GetClientRect( &rectBtnCancel);
	rectBtnCancel.left = (rect.right - rect.left) / 2 + 20;
	wndBtnCancel.SetWindowPos( m_hWnd, &rectBtnCancel, SWP_SHOWWINDOW);
	if ( m_iLanguage == RUS)
		SetWindowTextW( wndBtnCancel, L"Нет");
	else
		SetWindowTextW( wndBtnCancel, L"No");	

	return TRUE;
}

LRESULT CDialogError::OnBnClickedButtonYes(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	CWindow wnd = GetDlgItem(IDC_CHECK_HIDEWND);
	if ( (int)wnd.SendMessage( BM_GETCHECK, 0, 0) == 1 )
		m_bCreatingDumpAndSending = FALSE;
	else
		m_bCreatingDumpAndSending = TRUE;

	CWindow wnd2 = GetDlgItem(IDC_CHECK_RESTART);
	if ( (int)wnd2.SendMessage( BM_GETCHECK, 0, 0) == 1 )
		m_bAppRestart = TRUE;
	else
		m_bAppRestart = FALSE;

	EndDialog( IDOK);

	return 0;
}

LRESULT CDialogError::OnBnClickedButtonNo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	CWindow wnd = GetDlgItem(IDC_CHECK_HIDEWND);
	if ( (int)wnd.SendMessage( BM_GETCHECK, 0, 0) == 1 )
		m_bCreatingDumpAndSending = FALSE;
	else
		m_bCreatingDumpAndSending = TRUE;

	CWindow wnd2 = GetDlgItem(IDC_CHECK_RESTART);
	if ( (int)wnd2.SendMessage( BM_GETCHECK, 0, 0) == 1 )
		m_bAppRestart = TRUE;
	else
		m_bAppRestart = FALSE;
	
	EndDialog( IDCANCEL);
	return 0;
}

LRESULT CDialogError::OnBnClickedCheckHideWnd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO: Add your control notification handler code here	
	return 0;
}

LRESULT CDialogError::OnBnClickedCheckRestart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here

	return 0;
}

LRESULT CDialogError::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default

	return 0;
}

BOOL CDialogError::IsCreatingDumpAndSending()
{
	return m_bCreatingDumpAndSending;
}

BOOL CDialogError::IsAppRestart()
{
	return m_bAppRestart;
}

void CDialogError::SetAppLanguage( int iLanguage)
{
	m_iLanguage = iLanguage;
}
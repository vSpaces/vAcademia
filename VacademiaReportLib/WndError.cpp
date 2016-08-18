#include "stdafx.h"
#include "WndError.h"
#include "ReportMan.h"
#include <objidl.h>
#include <GdiPlus.h>
#include <GdiPlusFlat.h>

#define IDT_TIMER1 1

CWndError::CWndError()
{
	m_iLanguage = ENG;
	fadeOutDuration = 1000.0f;
	m_iProgress = 0;
	m_iProgressStep = 5;
	m_bProgress = FALSE;
	m_iStep = QUERY_GET;
	bEnded = FALSE;
	m_bAppRestart = TRUE;
	m_bAppRestartCheckVisible = TRUE;
}

CWndError::~CWndError()
{
	if ( IsWindow())
	{
		KillTimer( m_nTimerID);
		DestroyWindow();
		m_hWnd = 0;
	}
}

void CWndError::Create(HWND hWndParent, CReportMan *pReportMan, LPARAM dwInitParam /* = NULL */)
{
	CDialogImpl<CWndError>::Create( hWndParent, dwInitParam);
	int err = 0;
	if ( m_hWnd == NULL)
	{
		err = GetLastError();
	}
	hSplashWindow = m_hWnd;
	m_pReportMan = pReportMan;
}

LRESULT CWndError::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default
	ShowWindow( SW_HIDE);
	return 0;
}

LRESULT CWndError::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// TODO: Add your message handler code here and/or call default	
	if ( m_iLanguage == RUS)
	{
		SetWindowTextW( m_hWnd, L"Обнаружена ошибка");
		CWindow wndLabel1 = GetDlgItem( IDC_STATIC_1);
		SetWindowTextW( wndLabel1.m_hWnd, L"Произошла ошибка в приложении vAcademia. Сейчас программа произведет сбор необходимой информации по ошибке и отправит ее разработчикам.");
		CWindow wndLabel2 = GetDlgItem( IDC_STATIC_2);
		SetWindowTextW( wndLabel2.m_hWnd, L"Никакая конфиденциальная информация не будет передана.");
		
	}
	else
	{
		SetWindowTextW( m_hWnd, L"An error was detected");
		CWindow wndLabel1 = GetDlgItem( IDC_STATIC_1);
		SetWindowTextW( wndLabel1.m_hWnd, L"An error has occurred in vAcademia. The program will collect all the necessary information about error and send it to the maintenance team.");
		CWindow wndLabel2 = GetDlgItem( IDC_STATIC_2);
		SetWindowTextW( wndLabel2.m_hWnd, L"No confidential information will be collected.");
	}	

	CWindow wndBtnOk = GetDlgItem( IDC_BUTTON_OK);	
	CWindow wndBtnCancel = GetDlgItem( IDC_BUTTON_CANCEL);
	
	if ( m_iLanguage == RUS)
	{
		SetWindowTextW( wndBtnOk.m_hWnd, L"Ок");
		SetWindowTextW( wndBtnCancel.m_hWnd, L"Отмена");
	}
	else
	{
		SetWindowTextW( wndBtnOk.m_hWnd, L"Ok");
		SetWindowTextW( wndBtnCancel.m_hWnd, L"Cancel");
	}

	RECT rect;
	this->GetClientRect( &rect);

	//rect.right = rect.left + 440;
	//rect.bottom = rect.top + 285;
	//this->SetWindowPos( m_hWnd, &rect, SWP_SHOWWINDOW);

	//RECT rect;
	//this->GetClientRect( &rect);

	RECT rectBtnOk;	
	wndBtnOk.GetClientRect( &rectBtnOk);
	rectBtnOk.left = (rect.right - rect.left) / 2 - ( rectBtnOk.right + 10);
	wndBtnOk.SetWindowPos( m_hWnd, &rectBtnOk, SWP_SHOWWINDOW);

	RECT rectBtnCancel;	
	wndBtnCancel.GetClientRect( &rectBtnCancel);
	rectBtnCancel.left = (rect.right - rect.left) / 2 + 20;//( rectBtnCancel.right - rectBtnCancel.left);
	wndBtnCancel.SetWindowPos( m_hWnd, &rectBtnCancel, SWP_SHOWWINDOW);
	
	
	CWindow wndAppRestartCheck = GetDlgItem(IDC_CHECK_RESTART);

	if ( m_bAppRestartCheckVisible)
	{
		wndAppRestartCheck.SendMessage( BM_SETCHECK, 1, 1);
		if ( m_iLanguage == RUS)
			SetWindowTextW( wndAppRestartCheck.m_hWnd, L"Автоматически перезапустить vAcademia");
		else
			SetWindowTextW( wndAppRestartCheck.m_hWnd, L"Restart vAcademia");
	}
	else
	{
		wndAppRestartCheck.ShowWindow( SW_HIDE);
		m_bAppRestart = FALSE;
	}

	m_nTimerID = this->SetTimer(IDT_TIMER1, 500, NULL);	

	SendDlgItemMessage(IDC_PROGRESS_1, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); 
	SendDlgItemMessage(IDC_PROGRESS_1, PBM_SETPOS, (WPARAM)m_iProgress, 0);
	SendDlgItemMessage(IDC_PROGRESS_1, PBM_SETSTEP, (WPARAM)5, 0);	

	Start();
	
	return TRUE;
}

LRESULT CWndError::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default

	return 0;
}

void CWndError::SetAppRestartCheckVisible( BOOL bAppRestartCheckVisible)
{
	m_bAppRestartCheckVisible = bAppRestartCheckVisible;
}

void CWndError::SetAppLanguage( int iLanguage)
{
	m_iLanguage = iLanguage;
}

void CWndError::SetProgress( int iProgress)
{
	m_iProgress = iProgress;
	SendDlgItemMessage(IDC_PROGRESS_1, PBM_SETPOS, (WPARAM)m_iProgress, 0);
	UpdateWindow();
}

void CWndError::SetStep( int iStep)
{
	m_iStep = iStep;
}

void CWndError::Start()
{
	m_bProgress = TRUE;
}

void CWndError::Stop()
{
	m_bProgress = FALSE;
}

void CWndError::End()
{
	SetProgress( 100);	
	bEnded = TRUE;
}

LRESULT CWndError::OnBnClickedButtonOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	CWindow wnd2 = GetDlgItem(IDC_CHECK_RESTART);
	if ( (int)wnd2.SendMessage( BM_GETCHECK, 0, 0) == 1 )
		m_bAppRestart = TRUE;
	else
		m_bAppRestart = FALSE;	
	this->ShowWindow( SW_HIDE);	
	return 0;
}

LRESULT CWndError::OnBnClickedButtonCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	m_bAppRestart = FALSE;
	this->ShowWindow( SW_HIDE);
	return 0;
}

LRESULT CWndError::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{	
	// TODO: Add your message handler code here and/or call default
	if ( !m_bProgress)
		return 0;

	if ( m_iStep == ZIPPING)
	{
		if ( m_iProgress > 25)
			return 0;		
		if ( m_iProgress > 20)
			m_iProgressStep = 2;
		else if ( m_iProgress >= 10)
			m_iProgressStep = 3;
		else
			m_iProgressStep = 5;
	}

	if ( m_iStep == QUERY_GET)
	{
		if ( m_iProgress > 55)
			return 0;		
		if ( m_iProgress > 50)
			m_iProgressStep = 2;
		else if ( m_iProgress >= 40)
			m_iProgressStep = 3;
		else
			m_iProgressStep = 5;
	}
	else if ( m_iStep == QUERY_POST)
	{
		if ( m_iProgress > 95)
			return 0;
		if ( m_iProgress > 90)
			m_iProgressStep = 1;
		else if ( m_iProgress > 80)
			m_iProgressStep = 2;
		else if ( m_iProgress >= 40)
			m_iProgressStep = 3;
		else
			m_iProgressStep = 5;
	}
	
	m_iProgress += m_iProgressStep;
	SendDlgItemMessage(IDC_PROGRESS_1, PBM_SETPOS, (WPARAM)m_iProgress, 0);
	UpdateWindow();	

	return 0;
}

void CWndError::Back( int iStep)
{
	if ( m_iStep == ZIPPING)
		m_iProgress = 1;
	else if ( m_iStep == QUERY_GET)
		m_iProgress = 25;
	else if ( m_iStep == QUERY_POST)
		m_iProgress = 55;
}

BOOL CWndError::IsAppRestart()
{
	return m_bAppRestart;
}
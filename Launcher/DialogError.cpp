#include "stdafx.h"
#include "mdump/dump.h"
#include "DialogError.h"

CDialogError::CDialogError()
{
	m_bCreatingDumpAndSending = TRUE;
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
		SetWindowTextW( m_hWnd, L"Виртуальная академия");
		CWindow wndLabel1 = GetDlgItem( IDC_STATIC_1);
		SetWindowTextW( wndLabel1.m_hWnd, L"Кажется приложении 'Виртуальная академия' зависла. Хотите помочь разработчикам? Нажмите кнопку 'Да'. В этом случае программа произведет сбор необходимой информации по ошибке и отправит ее разработчикам.");
		CWindow wndLabel2 = GetDlgItem( IDC_STATIC_2);
		SetWindowTextW( wndLabel2.m_hWnd, L"Никакая конфиденциальная информация не будет передана.");
		CWindow wndLabel3 = GetDlgItem( IDC_STATIC_3);
		SetWindowTextW( wndLabel3.m_hWnd, L"Если вы нажмете кнопку «Нет», то никакая информация отправлена не будет. В любом случае программа постарается продолжить работу.");
		CWindow wndCheck = GetDlgItem(IDC_CHECK_HIDEWND);		
		SetWindowTextW( wndCheck.m_hWnd, L"Не показывать больше данное окно при повторении ошибки");
	}
	else
	{
		SetWindowTextW( m_hWnd, L"Virtual Academia");
		CWindow wndLabel1 = GetDlgItem( IDC_STATIC_1);
		SetWindowTextW( wndLabel1.m_hWnd, L"The \"Virtual Academia\" application seems to have hung up. Click \"yes\" and the system will collect all the necessary information about the error and send it to programmers. We really appreciate your assistance.");
		CWindow wndLabel2 = GetDlgItem( IDC_STATIC_2);
		SetWindowTextW( wndLabel2.m_hWnd, L"No confidential information will be revealed.");
		CWindow wndLabel3 = GetDlgItem( IDC_STATIC_3);
		SetWindowTextW( wndLabel3.m_hWnd, L"If you click the \"No\" the information won’t be sent. The program will try to continue running.");
		CWindow wndCheck = GetDlgItem(IDC_CHECK_HIDEWND);		
		SetWindowTextW( wndCheck.m_hWnd, L"Don’t show this window again when the same error occurs");
	}

	RECT rect;
	this->GetClientRect( &rect);
	CWindow wndBtnOk = GetDlgItem(IDC_BTN_OK);
	RECT rectBtnOk;	
	wndBtnOk.GetClientRect( &rectBtnOk);
	rectBtnOk.left = (rect.right - rect.left) / 2 - ( rectBtnOk.right - rectBtnOk.left);
	wndBtnOk.SetWindowPos( m_hWnd, &rectBtnOk, SWP_SHOWWINDOW);
	if ( m_iLanguage == RUS)
		SetWindowTextW( wndBtnOk.m_hWnd, L"Да");
	else
		SetWindowTextW( wndBtnOk.m_hWnd, L"Yes");

	CWindow wndBtnCancel = GetDlgItem(IDC_BTN_NO);
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

LRESULT CDialogError::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: Add your message handler code here and/or call default

	return 0;
}

LRESULT CDialogError::OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	CWindow wnd = GetDlgItem(IDC_CHECK_HIDEWND);
	if ( (int)wnd.SendMessage( BM_GETCHECK, 0, 0) == 1 )
		m_bCreatingDumpAndSending = FALSE;
	else
		m_bCreatingDumpAndSending = TRUE;
	EndDialog( IDOK);
	return 0;
}

LRESULT CDialogError::OnBnClickedNo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here
	CWindow wnd = GetDlgItem(IDC_CHECK_HIDEWND);
	if ( (int)wnd.SendMessage( BM_GETCHECK, 0, 0) == 1 )
		m_bCreatingDumpAndSending = FALSE;
	else
		m_bCreatingDumpAndSending = TRUE;
	EndDialog( IDCANCEL);
	return 0;
}

LRESULT CDialogError::OnBnClickedCheckHidewnd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add your control notification handler code here

	return 0;
}

void CDialogError::CloseDialog()
{
#ifdef _DEBUG
	if ( m_bModal)
#endif //_DEBUG
		EndDialog( IDCANCEL);
}

void CDialogError::SetAppLanguage( int iLanguage)
{
	m_iLanguage = iLanguage;
}

BOOL CDialogError::IsCreatingDumpAndSending()
{
	return m_bCreatingDumpAndSending;
}
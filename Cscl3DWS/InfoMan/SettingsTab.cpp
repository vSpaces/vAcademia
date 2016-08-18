#include "stdafx.h"
#include "LogTab.h"
#include ".\SettingsTab.h"
#include ".\logtab.h"
#include <fstream>
#include "rm\rm.h"

CSettingsTab::CSettingsTab(){
};

CSettingsTab::~CSettingsTab(){
	if( IsWindow())
	{
		DestroyWindow();
		m_hWnd = NULL;
	}
};

LRESULT CSettingsTab::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	SetWindowPos( 0, &rcDefault, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);

	m_showHidden3DO.Attach(GetDlgItem( IDC_SHOW_HIDDEN_3DO));

	bHandled = FALSE;
	return 0;
}

LRESULT CSettingsTab::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CSettingsTab::OnBnClickedShowHidden3DO(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//CButton showHidden3DO( GetDlgItem( IDC_SHOW_HIDDEN_3DO));
	bool bChecked = (m_showHidden3DO.GetCheck() == 1); //(showHidden3DO.GetCheck() == 1);
	if(mpOmsContext->mpRM){
		mpOmsContext->mpRM->ShowHidden3DObjects(bChecked);
	}
	return 0;
}

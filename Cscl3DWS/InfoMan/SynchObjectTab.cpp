#include "stdafx.h"
#include "LogTab.h"
#include ".\SynchObjectTab.h"
#include ".\logtab.h"
#include <fstream>

CSynchObjectPropTab::CSynchObjectPropTab(){
};

CSynchObjectPropTab::~CSynchObjectPropTab(){
	if( IsWindow())
	{
		DestroyWindow();
		m_hWnd = NULL;
	}
};

LRESULT CSynchObjectPropTab::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	SetWindowPos( 0, &rcDefault, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);
	RECT rect;
	GetClientRect(&rect);
	rect.top += 30;

	m_EditProp.Attach(GetDlgItem( IDC_EDIT_PROP));

	bHandled = FALSE;
	return 0;
}

LRESULT CSynchObjectPropTab::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

void CSynchObjectPropTab::SetTextProp( const std::wstring& info)
{
	LRESULT length = ::SendMessage( m_EditProp.m_hWnd, WM_GETTEXTLENGTH, 0, 0);
	::SendMessage( m_EditProp.m_hWnd, EM_SETSEL, 0, length - 1);

	// insert text
	const wchar_t * begin = info.c_str();
	::SendMessageW( m_EditProp.m_hWnd, EM_REPLACESEL, 0, (LPARAM)begin);

}





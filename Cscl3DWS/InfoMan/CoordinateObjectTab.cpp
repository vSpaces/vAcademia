#include "stdafx.h"
#include "LogTab.h"
#include ".\CoordinateObjectTab.h"
#include ".\logtab.h"
#include <fstream>

CCoordinateObjectPropTab::CCoordinateObjectPropTab(){
};

CCoordinateObjectPropTab::~CCoordinateObjectPropTab(){
	if( IsWindow())
	{
		DestroyWindow();
		m_hWnd = NULL;
	}
};

LRESULT CCoordinateObjectPropTab::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ModifyStyleEx(0, WS_EX_CLIENTEDGE);
//	rcDefault.top = rcDefault.top - 100;
	RECT rect;
	GetClientRect(&rect);
	rect.top += 30;
	SetWindowPos( 0, &rect, SWP_NOACTIVATE);

	mX.Attach( GetDlgItem( IDC_EDIT_COORD_X));
	mY.Attach( GetDlgItem( IDC_EDIT_COORD_Y));
	mZ.Attach( GetDlgItem( IDC_EDIT_COORD_Z));

	bHandled = FALSE;
	return 0;
}

void CCoordinateObjectPropTab::SetTextProp( const ml3DPosition& info)
{
	char buf[30];

	itoa(info.x, buf, 10);
	LRESULT length = ::SendMessage( mX.m_hWnd, WM_GETTEXTLENGTH, 0, 0);
	::SendMessage( mX.m_hWnd, EM_SETSEL, 0, length);
	::SendMessage( mX.m_hWnd, EM_REPLACESEL, 0, (LPARAM)&buf[0]);

	itoa(info.y, buf, 10);
	length = ::SendMessage( mY.m_hWnd, WM_GETTEXTLENGTH, 0, 0);
	::SendMessage( mY.m_hWnd, EM_SETSEL, 0, length);
	::SendMessage( mY.m_hWnd, EM_REPLACESEL, 0, (LPARAM)&buf[0]);

	itoa(info.z, buf, 10);
	length = ::SendMessage( mZ.m_hWnd, WM_GETTEXTLENGTH, 0, 0);
	::SendMessage( mZ.m_hWnd, EM_SETSEL, 0, length);
	::SendMessage( mZ.m_hWnd, EM_REPLACESEL, 0, (LPARAM)&buf[0]);
}





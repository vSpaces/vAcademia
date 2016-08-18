#include "stdafx.h"
#include "LogTab.h"
#include ".\ScriptExecuteTab.h"
#include ".\logtab.h"
#include <fstream>

CScriptExecuteTab::CScriptExecuteTab(){
};

CScriptExecuteTab::~CScriptExecuteTab(){
	if( IsWindow())
	{
		DestroyWindow();
		m_hWnd = NULL;
	}
};

LRESULT CScriptExecuteTab::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	SetWindowPos( 0, &rcDefault, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);

	m_EditScript.Attach(GetDlgItem( IDC_EDIT_SCRIPT));
	m_ButtonExecute.Attach(GetDlgItem( IDC_BUTTON_EXECUTE));

	bHandled = FALSE;
	return 0;
}

LRESULT CScriptExecuteTab::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CScriptExecuteTab::OnBnClickedButtonExecute(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	LRESULT lineCount = ::SendMessage( m_EditScript.m_hWnd, EM_GETLINECOUNT, 0, 0);
	LRESULT lLength = ::SendMessage( m_EditScript.m_hWnd, WM_GETTEXTLENGTH, 0, 0);

	TCHAR* lpData = MP_NEW_ARR( TCHAR, lLength+1);
	TCHAR* pData = lpData;
	unsigned int lastPos = 0;
	for (unsigned int i = 0; i < lineCount; ++i)
	{
		lastPos += m_EditScript.GetLine(i, (LPTSTR)&pData[lastPos], lLength + 1 - lastPos);
	}
	pData[lastPos] = '\0';

	wchar_t * lpw = MP_NEW_ARR( wchar_t, lLength+1);
	ZeroMemory(lpw, sizeof(lpw));
	::MultiByteToWideChar(CP_ACP, 0, pData, -1, lpw, strlen(pData)+1);

	omsresult result;
	if (mpOmsContext != NULL)
		result = mpOmsContext->mpSM->ExecScript(lpw, strlen(pData));

	MP_DELETE_ARR( lpData);
	MP_DELETE_ARR( lpw);

	return 0;
}

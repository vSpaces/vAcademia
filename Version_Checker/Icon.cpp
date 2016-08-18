#include "stdafx.h"
#include "Icon.h"

#define MYWM_NOTIFYICON (WM_USER + 1)

CIcon:: CIcon(const HWND &hwn, const HICON &hIcon, const UINT &uID, const wstring &text) 
{ 
	m_nid.cbSize = sizeof(NOTIFYICONDATAW); 
	m_nid.hWnd = hwn; 
	m_nid.uID = uID; 
	m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_nid.uCallbackMessage = MYWM_NOTIFYICON;
	m_nid.hIcon = hIcon;
	lstrcpynW(m_nid.szTip, text.c_str(), sizeof(m_nid.szTip)/sizeof(m_nid.szTip[0]));
	Shell_NotifyIconW(NIM_ADD, &m_nid); 
}
CIcon:: ~CIcon()
{
	Shell_NotifyIconW(NIM_DELETE, &m_nid);
}

void CIcon:: SetMessage(const wstring &text)
{
	lstrcpynW(m_nid.szTip, text.c_str(), sizeof(m_nid.szTip)/sizeof(m_nid.szTip[0]));
	Shell_NotifyIconW(NIM_MODIFY, &m_nid);
}
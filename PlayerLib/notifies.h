// notifies.h: interface for the C2DScene class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NOTIFIES_H__780D64C2_7C39_44F9_9407_C230DE2896CA__INCLUDED_)
#define AFX_NOTIFIES_H__780D64C2_7C39_44F9_9407_C230DE2896CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

inline LRESULT	n3d_send_message( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{	return ::SendMessage(hWnd, Msg, wParam, lParam);
}

#define	N3D_NOTIFY_PARAM(x)	(((DWORD)x << 16) + N3D_NOTIFY_CODE)

// Window message for natura3d library notifications
#define	N3D_MESSAGE_CODE	0x12345678
#define	N3D_NOTIFY_CODE		0x1234

#define WM_VACADEMIA_WM_BASE					WM_APP

#define WM_N3DEVENT								(WM_VACADEMIA_WM_BASE + 0x5000)

#define WM_CHILD_WND							(WM_VACADEMIA_WM_BASE + 100)
#define WM_CHILD_CURSOR							(WM_VACADEMIA_WM_BASE + 101)
#define WM_SUBCLASS_WND							(WM_VACADEMIA_WM_BASE + 102)
#define WM_UNSUBCLASS_WND						(WM_VACADEMIA_WM_BASE + 103)
// Сообщение о необходимости изменения размеров окна
#define WM_ADJUST_WND							(WM_VACADEMIA_WM_BASE + 104)
// Сообщение о необходимости изменения размеров окна
#define WM_ADJUST_CONTROL_WND					(WM_VACADEMIA_WM_BASE + 105)
// Сообщение о необходимости перезагрузки настроек
#define WM_RELOAD_SETTINGS						(WM_VACADEMIA_WM_BASE + 106)

#define WM_INTEL_VIDEO_FOUND					(WM_VACADEMIA_WM_BASE + 107)

#define WM_HIDE_LAUNCHER_NOW					(WM_VACADEMIA_WM_BASE + 108)

// Сообщение об успешном создании окна логинации
#define WM_CREATE_LOGIN_WND						(WM_VACADEMIA_WM_BASE + 109)

#define WM_SUCCES_LOGIN							(WM_VACADEMIA_WM_BASE + 110)

#define WM_EXIT_CODE							(WM_VACADEMIA_WM_BASE + 111)

#define WM_NEW_SESSION_AFTER_LOGOUT				(WM_VACADEMIA_WM_BASE + 112)

#define WM_NVIDIA_VIDEO_FOUND					(WM_VACADEMIA_WM_BASE + 113)

#define WM_RELOAD_INTERACTIVE_BOARD_SETTINGS	(WM_VACADEMIA_WM_BASE + 114)

#define WM_LAUNCHER_WM_MESSAGE					(WM_VACADEMIA_WM_BASE + 115)

#define WM_I_WANT_TO_KILL_VACADEMIA				(WM_VACADEMIA_WM_BASE + 116)

#define WM_OCULUS_WANTS_TO_CHANGE_THE_RESOLUTION (WM_VACADEMIA_WM_BASE + 117)

// Сообщение о том что необходимо измененить размеры окна
#define EM_ADJUST_WND		0x0001

// Notifications codes
#define NC_LEAVEFULLSCREEN	1

#endif // !defined(AFX_2DSCENE_H__780D64C2_7C39_44F9_9407_C230DE2896CA__INCLUDED_)

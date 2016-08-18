// BasePopupDialog.h: interface for the CBasePopupDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_3DWS_MIN)
#define AFX_3DWS_MIN

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../resource.h"




class C3DWSPopupDialog :  public CDialogImpl<C3DWSPopupDialog>
							
{
public:
	C3DWSPopupDialog();
	virtual ~C3DWSPopupDialog();

	enum Idd{IDD = IDD_DIALOGBAR1};

	BEGIN_MSG_MAP(C3DWSPopupDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()


protected:

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return TRUE;
	};

public:

	HWND GetWnd()
	{
		return m_hWnd;
	}
};

#endif // !defined(AFX_3DWS_MIN)

#pragma once

#include "ColoredControls.h"

/////////////////////////////////////////////////////////////////////////////
// CColoredStaticCtrl - A Static control with new colours

template< class T, class TBase = CStatic, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CVAColoredStaticImpl : 
   public CColoredStaticImpl< T, TBase, TWinTraits >
{
public:
	BOOL SubclassWindow(HWND hWnd)
	{
		BOOL  bRet = CColoredStaticImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);

		CBitmap	backBitmap;
		backBitmap.LoadBitmap( MAKEINTRESOURCE( IDB_BACKTEXTURE));
		CColoredStaticImpl< T, TBase, TWinTraits >::_Init();
		m_hbrNormalBk.DeleteObject();
		m_clrNormalText = RGB(200,227,255);
		m_hbrNormalBk.CreatePatternBrush( backBitmap.m_hBitmap);

		return bRet;
	}
};

class CVAColoredStaticCtrl : public CVAColoredStaticImpl<CColoredStaticCtrl>
{
};

class CVAColoredCheckboxCtrl : public CVAColoredStaticImpl<CColoredCheckboxCtrl, CButton>
{
};

class CVAColoredGroupboxCtrl : public CVAColoredStaticImpl<CVAColoredGroupboxCtrl, CButton>
{
};

class CVAColoredCheckBoxStaticCtrl : public CVAColoredStaticImpl<CColoredStaticCtrl>
{
	BEGIN_MSG_MAP( CVAColoredCheckBoxStaticCtrl)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		CHAIN_MSG_MAP( CVAColoredStaticImpl<CColoredStaticCtrl> )
	END_MSG_MAP()

	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}
};
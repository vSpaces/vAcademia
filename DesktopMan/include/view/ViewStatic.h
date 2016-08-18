#ifndef WTWORDSTATIC_INCLUDED
#define WTWORDSTATIC_INCLUDED

#include <atlmisc.h>

#undef SelectBitmap
#undef SelectBrush
#include "atlgdix.h"


template< class T, class TBase = CWindow, class TWinTraits = CFrameWinTraits >
class CViewStaticImpl
	: public CScrollWindowImpl< T, TBase, TWinTraits >
	  , public COffscreenDrawRect< T >
{
	typedef CViewStaticImpl _thisClass;
	typedef CScrollWindowImpl< T, TBase, TWinTraits > _baseClass;
	typedef COffscreenDrawRect< T> _baseClass2;

public:
	DECLARE_WND_CLASS_EX(_T("AWTL_RolloutView"),0,COLOR_BTNFACE);

	/*BOOL PreTranslateMessage(PMSG pMsg)
	{
		//return ::IsDialogMessage(m_hWnd, pMsg);
		return true;
	}*/

	BEGIN_MSG_MAP(_thisClass)
		MESSAGE_HANDLER(WM_VSCROLL, OnScroll)
		MESSAGE_HANDLER(WM_HSCROLL, OnScroll)
		CHAIN_MSG_MAP(_baseClass2)
		CHAIN_MSG_MAP(_baseClass)
	END_MSG_MAP()

	void SetBitmap(HBITMAP hBitmap) 
	{
		//if(!m_bmp.IsNull()) m_bmp.DeleteObject();
		m_bmp = hBitmap;
		if(!m_bmp.IsNull()) m_bmp.GetSize(m_size);
		else m_size.cx = m_size.cy = 1;
		SetScrollOffset(0, 0, FALSE);
		SetScrollSize(m_size);
	}

	void DoPaint(CDCHandle dc)
	{
		ATLASSERT(FALSE);
	}

	void DoPaint(CDCHandle dc, RECT rc)
	{
		POINT ptViewportOrg = { 0, 0 };
		dc.SetViewportOrg(-m_ptOffset.x, -m_ptOffset.y, &ptViewportOrg);

		if( !m_bmp.IsNull())
		{
			CDC dcMem;
			dcMem.CreateCompatibleDC(dc);

			CBrush brush = CreateHatchBrush( HS_DIAGCROSS, RGB(0,204,255));
			CBrushHandle oldBrush = dc.SelectBrush(brush);

			::CRect rc;
			GetClientRect(rc);
			rc.right = max( rc.right, m_size.cx);
			rc.bottom = max( rc.bottom, m_size.cy);
			dc.FillRect(&rc, brush);

			HBITMAP hBmpOld = dcMem.SelectBitmap(m_bmp);
			dc.BitBlt(0, 0, m_size.cx, m_size.cy, dcMem, 0, 0, SRCCOPY);
			dcMem.SelectBitmap(hBmpOld);

			dc.SelectBrush(oldBrush);

			if (!m_str.IsEmpty())
				dc.TextOut(20,150, m_str);
		}
		dc.SetViewportOrg(ptViewportOrg);
	}

	void SetText(CComString str)
	{
		m_str = str;
		SetScrollOffset(0, 0, FALSE);
		SetScrollSize(m_size);
	}

	LRESULT OnScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		InvalidateRect( NULL);
		return 0;
	}

	CBitmap	m_bmp;
	CSize	m_size;
	CComString m_str;
};


class CViewStaticCtrl : public CViewStaticImpl<CViewStaticCtrl>
{
public:

};

#endif // WTWORDSTATIC_INCLUDED
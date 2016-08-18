#pragma once

#include <vector>
#include "atldib.h"
#include "../atlgdix.h"
#include "atldibfilters.h"

template< class T, class TBase = CButton, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CWTGroupBoxImpl :
    public CWindowImpl< T, TBase, TWinTraits >,
    public COwnerDraw< T >,
    public CMouseHover< T >
{
private: 
    HFONT m_hFont;
    COLORREF m_colorText;
    COLORREF m_colorBg;

public:
    DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

    CWTGroupBoxImpl(void):
        m_colorText(0x303030),
        m_colorBg(0xCCCCCC),
        m_hFont(NULL)
    {
    }

    ~CWTGroupBoxImpl(void)
    {
    }

    // Operations
    BOOL SubclassWindow(HWND hWnd)
    {
        BOOL bRet = CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);
        if( bRet ) _Init();
        return bRet;
    }

    COLORREF GetTextColor() const
    {
        return m_colorText;
    }

    void SetTextColor(COLORREF _colText)
    {
        m_colorText = _colText;
    }

    void GetBorderColor(COLORREF& _colBg) const
    {
        _colBg = m_colorBg;
    }

    void SetBorderColor(COLORREF _colBg)
    {
        m_colorBg = _colBg;
    }

    void DrawItem(LPDRAWITEMSTRUCT lpDIS)
    {
        const INT roundFactor = 3;

        CRect rcItem;
        CRect rcClient = lpDIS->rcItem;
		CMemDC dc(lpDIS->hDC, &rcClient);
		UINT state = lpDIS->itemState;

        rcItem = rcClient;

        // Clear background
        {
            HWND hwndParent;
            HBRUSH brushBg;
            hwndParent = GetParent();
            brushBg = (HBRUSH) ::SendMessage(hwndParent, WM_CTLCOLORDLG, (WPARAM) dc.m_hDC, (LPARAM) hwndParent);
            dc.FillRect(rcItem, brushBg);
        }

		// Draw gradient background
		rcItem.DeflateRect( CSize(5, 5));

        // Draw boundary rectangle
        {
			HPEN hPen = CreatePen(PS_SOLID, 1, m_colorBg);
			HPEN hPenOld = dc.SelectPen(hPen);
            HBRUSH brushOld;
            brushOld = dc.SelectStockBrush(HOLLOW_BRUSH);
            dc.RoundRect(rcItem, CPoint(roundFactor, roundFactor));
            dc.SelectBrush(brushOld);
			dc.SelectPen(hPenOld);
        }

        // Draw gradient background
        rcItem.DeflateRect( CSize(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE)));

        {
            // Create gradient brush if not exist            
			//if( m_fMouseOver)
			//{
			//	if (m_brushGradSelected.IsNull())
			//	{
			//		_MakeGradientBgBrush(dc, rcItem.Height(), m_colorSelectedBgUp, m_colorSelectedBgDn, m_brushGradSelected);
			//	}
			//}
			//else
			//{
			//	if (m_brushGrad.IsNull())
			//	{
			//		_MakeGradientBgBrush(dc, rcItem.Height(), m_colorBgUp, m_colorBgDn, m_brushGrad);
			//	}
			//}

   //         // Fill pattern
   //         HBRUSH brushOld;
   //         CPoint ptOrgOld;
			//brushOld = dc.SelectBrush(m_fMouseOver ? m_brushGradSelected : m_brushGrad);
   //         ptOrgOld = dc.SetBrushOrg(0, rcItem.top);
   //         dc.PatBlt(rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), PATCOPY);
   //         dc.SetBrushOrg(ptOrgOld);
   //         dc.SelectBrush(brushOld);

			//if (state & ODS_FOCUS)
			//{
			//	dc.DrawFocusRect(&rcItem);
			//}
        }

        // Draw icon
		COLORREF colorText = m_colorText;

		if (state & ODS_DISABLED)
		{
			colorText = 0x999999;
		}

#if(WINVER >= 0x0500)
		if (state & ODS_HOTLIGHT)
		{
			colorText = 0xFFFFFF;
		}
#endif
		rcItem.OffsetRect(10, 0);

		// Draw text
        {
            static const int textOffset = 0;

            int maxTextBuf;
            std::vector<TCHAR> textBuf;
            maxTextBuf = GetWindowTextLength() + 1;
            textBuf.resize(maxTextBuf);
            GetWindowText(&(textBuf.front()), maxTextBuf);

            HFONT hOldFont;
            int oldBkMode;
            COLORREF oldTextColor;
            hOldFont = dc.SelectFont(m_hFont);
            //oldBkMode = dc.SetBkMode(TRANSPARENT);
			oldBkMode = dc.SetBkMode(OPAQUE);
            oldTextColor = dc.SetTextColor(colorText);

            rcItem.left += textOffset;
            dc.DrawText(&(textBuf.front()), -1, rcItem, DT_LEFT | DT_END_ELLIPSIS | DT_TOP | DT_SINGLELINE);

            dc.SetTextColor(oldTextColor);
            dc.SetBkMode(oldBkMode);
            dc.SelectFont(hOldFont);
        }
    }

private:
    void _Init()
    {
        // Adjust window style 
//        ModifyStyle(BS_TYPEMASK, BS_PUSHBUTTON | BS_OWNERDRAW);
		ModifyStyle(0, BS_OWNERDRAW);

        // Init objects
        if (m_hFont == NULL)
        {
            m_hFont = GetFont();
        }

    }

    void _MakeGradientBgBrush(HDC _dc, LONG _height, COLORREF topColor, COLORREF botColor, CBrush& brushGrad)
    {
        std::vector<COLORREF> gradBuff;
        CDCHandle dc(_dc);

        int i;
        LONG r, g, b;
        LONG dr, dg, db;

        // Create gradient bitmap data
        r = (GetRValue(topColor) << 16);
        g = (GetGValue(topColor) << 16);
        b = (GetBValue(topColor) << 16);

        dr = ((GetRValue(botColor) << 16) - r) / (_height - 1);
        dg = ((GetGValue(botColor) << 16) - g) / (_height - 1);
        db = ((GetBValue(botColor) << 16) - b) / (_height - 1);

        gradBuff.resize(_height);
        for (i = 0; i < _height; i++)
        {
            gradBuff[i] = RGB((r >> 16), (g >> 16), (b >> 16));
            r += dr;
            g += dg;
            b += db;
        }

        CDib24 dib;

        // Create bitmap GDI object
        BITMAPINFO bmi = 
        {
            { 
                sizeof(BITMAPINFOHEADER),   // size
                1, -_height,    // width, height
                1,  // planes
                24, // bit count
                BI_RGB, // format
                0,  // size image (0 for BI_RGB)
                96, 96, // dpi
                0, 0    // 
            }
        };
        CBitmap imageGrad;
        imageGrad.CreateCompatibleBitmap(dc, 1, _height);
        imageGrad.SetDIBits(dc, 0, _height, &(gradBuff.front()), &bmi, DIB_RGB_COLORS);
        brushGrad.CreatePatternBrush(imageGrad);
    }

    void _FreeGradientBgBrush(CBrush& brushGrad)
    {
        if (!brushGrad.IsNull())
        {
            brushGrad.DeleteObject();
        }
    }

public:
    BEGIN_MSG_MAP(CWTGroupBoxImpl)
        CHAIN_MSG_MAP( CMouseHover< T > )
        CHAIN_MSG_MAP_ALT( COwnerDraw< T >, 1 )
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClick)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {      
        LRESULT lRes = DefWindowProc();
        _Init();
        return lRes;
    }

    LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        return 1;
    }

    LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {      
        return 1;
    }

    LRESULT OnLButtonDblClick(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        // HACK: Ideally we would remove the CS_DBLCLCKS class style, but since we're
        //       subclassing the control we're forced to make it act like a regular click.
        //       Otherwise double-clicks will not show the button as pushed.
        return DefWindowProc(WM_LBUTTONDOWN, wParam, lParam);
    }   
};


class CWTGroupBoxCtrl : public CWTGroupBoxImpl<CWTGroupBoxCtrl>
{
public:
    DECLARE_WND_SUPERCLASS(_T("WTL_WTGroupBox"), GetWndClassName())
};

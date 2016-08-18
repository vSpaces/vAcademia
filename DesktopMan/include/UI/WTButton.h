#pragma once

#include <vector>
#include "atldib.h"
#include "../atlgdix.h"
#include "atldibfilters.h"

template< class T, class TBase = CButton, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CWTButtonImpl :
    public CWindowImpl< T, TBase, TWinTraits >,
    public COwnerDraw< T >,
    public CMouseHover< T >
{
private: 
    COLORREF m_colorText;
    HFONT m_hFont;
    COLORREF m_colorBgUp;
    COLORREF m_colorBgDn;
	COLORREF m_colorSelectedBgUp;
	COLORREF m_colorSelectedBgDn;
    CImageList m_imageList;
    INT m_imageIdNormal;
    INT m_imageIdHover;

    // Cached
    CBrush m_brushGrad;
	CBrush m_brushGradSelected;

public:
    DECLARE_WND_SUPERCLASS(NULL, TBase::GetWndClassName())

    CWTButtonImpl(void):
        m_colorText(0x303030),
        m_colorBgUp(0xF9F9F9),
        m_colorBgDn(0xCBD7F1),
		m_colorSelectedBgUp(0xF9F9F9),
		m_colorSelectedBgDn(0xF1E9CB),
        m_imageIdNormal(-1),
        m_imageIdHover(-1),
        m_hFont(NULL)
    {
    }

    ~CWTButtonImpl(void)
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

    void GetBgColors(COLORREF& _colBgUp, COLORREF& _colBgDn) const
    {
        _colBgUp = m_colorBgUp;
        _colBgDn = m_colorBgDn;
    }

    void SetBgColors(COLORREF _colBgUp, COLORREF _colBgDn)
    {
        m_colorBgUp = _colBgUp;
        m_colorBgDn = _colBgDn;
        _FreeGradientBgBrush( m_brushGrad);
    }

    int GetNormalImageId() const
    {
        return m_imageIdNormal;
    }

    void SetNormalImageId(int _imageIdNormal)
    {
        m_imageIdNormal = _imageIdNormal;
    }

    int GetHoverImageId() const
    {
        return m_imageIdNormal;
    }

    void SetHoverImageId(int _imageIdNormal)
    {
        m_imageIdNormal = _imageIdNormal;
    }

    CImageList& GetImageList()
    {
        return m_imageList;
    }

    void SetImageList(CImageList& _imageList)
    {
        m_imageList = _imageList;
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

        // Draw boundary rectangle
        {
            HBRUSH brushOld;
            brushOld = dc.SelectStockBrush(HOLLOW_BRUSH);
            dc.RoundRect(rcItem, CPoint(roundFactor, roundFactor));
            dc.SelectBrush(brushOld);
        }

        // Draw gradient background
        rcItem.DeflateRect( CSize(GetSystemMetrics(SM_CXEDGE), GetSystemMetrics(SM_CYEDGE)));

        {
            // Create gradient brush if not exist            
			if( m_fMouseOver)
			{
				if (m_brushGradSelected.IsNull())
				{
					_MakeGradientBgBrush(dc, rcItem.Height(), m_colorSelectedBgUp, m_colorSelectedBgDn, m_brushGradSelected);
				}
			}
			else
			{
				if (m_brushGrad.IsNull())
				{
					_MakeGradientBgBrush(dc, rcItem.Height(), m_colorBgUp, m_colorBgDn, m_brushGrad);
				}
			}

            // Fill pattern
            HBRUSH brushOld;
            CPoint ptOrgOld;
			brushOld = dc.SelectBrush(m_fMouseOver ? m_brushGradSelected : m_brushGrad);
            ptOrgOld = dc.SetBrushOrg(0, rcItem.top);
            dc.PatBlt(rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), PATCOPY);
            dc.SetBrushOrg(ptOrgOld);
            dc.SelectBrush(brushOld);

			if (state & ODS_FOCUS)
			{
				dc.DrawFocusRect(&rcItem);
			}
        }

        if (state & ODS_SELECTED)
        {
            rcItem.OffsetRect(1, 1);
        }

        // Draw icon
        if (m_imageList != NULL)
        {
            CSize szImage;
            CRect rcImage;

            // Calculate image rect
            LONG offset;
            m_imageList.GetIconSize(szImage);
            offset = (rcItem.Height() - szImage.cy) / 2;
            offset = (offset < 0) ? 0 : offset;
            rcImage.left = rcItem.left + 3;
            rcImage.top = rcItem.top + offset;
            rcImage.right = rcImage.left + szImage.cx;
            rcImage.bottom = rcImage.top + szImage.cy;
            rcImage.IntersectRect(rcImage, rcItem);

            // Draw image
            int nImage = m_imageIdNormal;
            if (state & ODS_CHECKED)
            {
//                nImage = m_imageIdPressed;
            }
/*
            else if (state & ODS_HOTLIGHT)
            {
                nImage = m_imageIdHover;
            }
*/
            else
            {
                nImage = m_imageIdNormal;
            }
            ATLASSERT(nImage < m_imageList.GetImageCount());
           
            m_imageList.DrawEx(
                nImage, dc,
                rcImage.left, rcImage.top, rcImage.Width(), rcImage.Height(), 
                CLR_NONE, CLR_NONE, ILD_TRANSPARENT);

            // Offset remaining item rectangle
            rcItem.left = rcImage.right;
        }

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
            oldBkMode = dc.SetBkMode(TRANSPARENT);
            oldTextColor = dc.SetTextColor(colorText);

            rcItem.left += textOffset;
            dc.DrawText(&(textBuf.front()), -1, rcItem, DT_VCENTER | DT_END_ELLIPSIS | DT_CENTER | DT_SINGLELINE);

            dc.SetTextColor(oldTextColor);
            dc.SetBkMode(oldBkMode);
            dc.SelectFont(hOldFont);
        }

		if (state & ODS_DISABLED)
		{
			CDib dib;
			if (dib.Create(dc.GetCurrentBitmap())){
				CGreyScaleImageFilter::Process(&dib);
				dib.Draw(dc, 0, 0);
			}
		}

    }

private:
    void _Init()
    {
        // Adjust window style 
//        ModifyStyle(BS_TYPEMASK, BS_PUSHBUTTON | BS_OWNERDRAW);
		ModifyStyle(0, BS_PUSHBUTTON | BS_OWNERDRAW);

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
    BEGIN_MSG_MAP(CWTButtonImpl)
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
        _FreeGradientBgBrush( m_brushGrad);
		_FreeGradientBgBrush( m_brushGradSelected);
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


class CWTButtonCtrl : public CWTButtonImpl<CWTButtonCtrl>
{
public:
    DECLARE_WND_SUPERCLASS(_T("WTL_WTButton"), GetWndClassName())
};

#include <atlcrack.h>
#include <atlmisc.h>

#undef SelectBitmap
#undef SelectBrush
#include "../atlgdix.h"

#include "../SharingContext.h"
#include "../IDesktopMan.h"

template< class T, class TBase = CWindow, class TWinTraits = CFrameWinTraits >
class CRemoteWindow
	: public CScrollWindowImpl< T, TBase, TWinTraits >
	  , public COffscreenDrawRect< T >
{
	typedef CRemoteWindow _thisClass;
	typedef CScrollWindowImpl< T, TBase, TWinTraits > _baseClass;
	typedef COffscreenDrawRect< T> _baseClass2;

public:
	BEGIN_MSG_MAP(_thisClass)
		MESSAGE_HANDLER(WM_VSCROLL, OnScroll)
		MESSAGE_HANDLER(WM_HSCROLL, OnScroll)
		MSG_WM_LBUTTONDBLCLK(OnLMouseDblClk)
		MSG_WM_RBUTTONDBLCLK(OnRMouseDblClk)
		MSG_WM_RBUTTONDOWN(OnRMouseDown)
		MSG_WM_RBUTTONUP(OnRMouseUp)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONDOWN(OnLMouseDown)
		MSG_WM_LBUTTONUP(OnLMouseUp)
// 		MSG_WM_KEYUP(OnKeyUp)
// 		MSG_WM_KEYDOWN(OnKeyDown)
	
		CHAIN_MSG_MAP(_baseClass2)
		CHAIN_MSG_MAP(_baseClass)
	END_MSG_MAP()

	void SetBitmap(HBITMAP hBitmap) 
	{
		m_bmp = hBitmap;
		if(!m_bmp.IsNull()) m_bmp.GetSize(m_size);
		else m_size.cx = m_size.cy = 1;
// 		SetScrollOffset(0, 0, FALSE);
// 		SetScrollSize(m_size);
		Invalidate(FALSE);
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

			WaitForSingleObject(hMutex, INFINITE);
			HBITMAP hBmpOld = dcMem.SelectBitmap(m_bmp);
			dc.BitBlt(0, 0, m_size.cx, m_size.cy, dcMem, 0, 0, SRCCOPY);
			dcMem.SelectBitmap(hBmpOld);
			ReleaseMutex(hMutex);

			dc.SelectBrush(oldBrush);
		}
		dc.SetViewportOrg(ptViewportOrg);
	}

	LRESULT OnScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		InvalidateRect( NULL);
		return 0;
	}
	// мышь 
	// тип 0 - мышь
	//код 0 
	void	OnLMouseDown(UINT t, CPoint p)
	{
		desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
		if (pHandler)
			pHandler->OnLeftMouseDown(p.x, p.y);
	}
	//код 1
	void	OnLMouseUp(UINT t, CPoint p)
	{
		desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
		if (pHandler)
			pHandler->OnRightMouseUp(p.x, p.y);
	}
	//код 2
	void	OnLMouseDblClk(UINT t, CPoint p)
	{
		desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
		if (pHandler)
			pHandler->OnLeftMouseDblClk(p.x, p.y);
	}
	//код 3
	void	OnRMouseDown(UINT t, CPoint p)
	{
		desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
		if (pHandler)
			pHandler->OnRightMouseDown(p.x, p.y);
	}
	//код 4
	void	OnRMouseUp(UINT t, CPoint p)
	{
		desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
		if (pHandler)
			pHandler->OnRightMouseUp(p.x, p.y);
	}
	//код 5
	void	OnRMouseDblClk(UINT t, CPoint p)
	{
		desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
		if (pHandler)
			pHandler->OnRightMouseDblClk(p.x, p.y);
	}
	//код 6
	void	OnMouseMove(UINT t, CPoint p)
	{
		desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
		if (pHandler)
			pHandler->OnMouseMove(p.x, p.y);
	}
	// тип 1 - клавиатура
	// код 0
	void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
		if (pHandler)
			pHandler->OnKeyUp(nChar, nFlags);
	}
	// код 1
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
		if (pHandler)
			pHandler->OnKeyDown(nChar, nFlags);
	}
	void SetSessionName(LPCSTR alpTarget)
	{
		alpSessionName = alpTarget;
	}
	void SetContext(SHARING_CONTEXT* apContext)
	{
		context = apContext;
	}

	void Lock()
	{
		if (isActiveMutex)
			WaitForSingleObject(hMutex, INFINITE);
	}

	void UnLock()
	{
		if (isActiveMutex)
			ReleaseMutex(hMutex);	
	}
	
	void InitMutex()
	{
		isActiveMutex = true;
		hMutex = CreateMutex(NULL, false, "remoteWindowMutex");
	}

	void DeleteMutex()
	{
		isActiveMutex = false;
		CloseHandle(hMutex);
	}

	SHARING_CONTEXT* context;
	HANDLE hMutex;
	CBitmap	m_bmp;
	CSize	m_size;
	LPCSTR alpSessionName;
private:
	bool isActiveMutex;
	desktop::IMouseKeyboardHandler* GetIMouseKeyboardHandler()
	{
		if (!context)
			return NULL;
		desktop::IDesktopSharingManager* pSharingMan = context->gcontext->mpSharingMan;
		if (!pSharingMan)
			return NULL;
		desktop::IMouseKeyboardHandler* pHandler = (desktop::IMouseKeyboardHandler*)pSharingMan->GetIMouseKeyboardHandler();
		if (!pHandler)
			return NULL;
		return pHandler;
	}
};


class CRemoteWindowCtrl : public CRemoteWindow<CRemoteWindowCtrl>
{
public:

};
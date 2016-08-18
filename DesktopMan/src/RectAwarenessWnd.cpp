#include "stdafx.h"
#include "..\include\rectawarenesswnd.h"

#define BORDER_WIDTH	5
#define IDT_TIMER1		1
#define TIMEOUT			100

CRectAwarenessWnd::CRectAwarenessWnd(IDesktopSelector *pSelector)
	: m_pSelector(pSelector)
{
	m_region = NULL;
	m_capturingRect = pSelector->GetCurrentSelectingRect();
	
	UpdateAwarenessWndRectByCapRect(m_capturingRect);

	Create(NULL, m_awarenessRect, NULL, WS_POPUP | WS_VISIBLE, 0, 0U, NULL);

	UpdateLayout();

	StartTimer();
}

CRectAwarenessWnd::~CRectAwarenessWnd(void)
{

}

void CRectAwarenessWnd::UpdateLayout()
{
	RECT newCapturingRect = m_pSelector->GetCurrentSelectingRect();

	if(!EqualRect(&newCapturingRect, &m_capturingRect) || !m_region)
	{
		CopyRect(&m_capturingRect, &newCapturingRect);

		UpdateAwarenessWndRectByCapRect(m_capturingRect);
		
		if (m_region)
		{
			m_region.DeleteObject();
			m_region = NULL;
		}
		
		if (!m_region.CreateRectRgnIndirect(&m_awarenessRect))
		{
			ATLASSERT(FALSE);
			SetWindowRgn(NULL, TRUE);
			return;
		}
		GenerateRegion();
		
	//	if (this->IsWindowVisible())
	//		this->SetWindowPos(NULL, &m_awarenessRect, SWP_SHOWWINDOW);
		SetRegion();
	}
}

void CRectAwarenessWnd::UpdateAwarenessWndRectByCapRect(const RECT &capturingRect)
{
	m_awarenessRect.left = m_capturingRect.left - BORDER_WIDTH;
	m_awarenessRect.top = m_capturingRect.top - BORDER_WIDTH;
	m_awarenessRect.right = m_capturingRect.right + BORDER_WIDTH;
	m_awarenessRect.bottom = m_capturingRect.bottom + BORDER_WIDTH;	
}

bool CRectAwarenessWnd::StartTimer()
{
	m_nTimerID = this->SetTimer(IDT_TIMER1, (UINT)(TIMEOUT), NULL);
	if (m_nTimerID == 0)
	{
		ATLTRACE("Can't set AwarenessWnd timer\n");
		return false;
	}
	return true;
}

bool CRectAwarenessWnd::NormalizeRect(RECT &rec)
{
	if (rec.right >= rec.left && rec.bottom >= rec.top)
	{
		rec.right -= rec.left;
		rec.bottom -= rec.top;
		rec.top = rec.left = 0;
		return true;
	}
	return false;
}

bool CRectAwarenessWnd::GenerateRegion()
{
	RECT rec1, rec2;
	rec1 = m_awarenessRect;
	NormalizeRect(rec1);

	rec2.bottom = rec1.bottom - BORDER_WIDTH;
	rec2.top = rec1.top + BORDER_WIDTH;
	rec2.left = rec1.left + BORDER_WIDTH;
	rec2.right = rec1.right - BORDER_WIDTH;	

	CRgn rgn1, rgn2;
	rgn1.CreateRectRgnIndirect(&rec1);
	rgn2.CreateRectRgnIndirect(&rec2);

	int nCombineResult = m_region.CombineRgn((HRGN)rgn1, (HRGN)rgn2, RGN_DIFF);
	ATLASSERT(nCombineResult != ERROR && nCombineResult != NULLREGION );
		
	return true;
}

void CRectAwarenessWnd::SetRegion()
{
	this->SetWindowRgn((HRGN)m_region, TRUE);
}

LRESULT CRectAwarenessWnd::OnEraseBck(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return 1;
}

LRESULT CRectAwarenessWnd::OnPaint(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps;
	HDC hDC  = this->BeginPaint(&ps);

    CDC dc = hDC;
	CBrush br;
	br.CreateSolidBrush(RGB(255, 0, 0)); 
	RECT rec;
	GetClientRect(&rec);
	dc.FillRect(&rec, br);
	
	this->EndPaint(&ps);

	return 0;
}

bool CRectAwarenessWnd::ShowAwarenessWindow()
{
	this->SetWindowPos(HWND_TOPMOST, &m_awarenessRect, SWP_SHOWWINDOW);
	return true;
}

bool CRectAwarenessWnd::HideAwarenessWindow()
{
	this->SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_HIDEWINDOW);
	return true;
}

LRESULT CRectAwarenessWnd::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	UpdateLayout();
	this->Invalidate();
	return 0;
}

/*void Test2()
{
	// Создаем главное окно 
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	RECT rec;
	rec.left = 200;
	rec.top = 200;
	rec.bottom = 500;
	rec.right = 500;

	//CRectAwarenessWnd awareneesWnd(rec);
	//awareneesWnd.SetRegion(); 

	int nRet = theLoop.Run();
	_Module.RemoveMessageLoop();
}*/
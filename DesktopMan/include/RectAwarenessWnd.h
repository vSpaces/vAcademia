#pragma once

#include <atlhost.h>
#include <atlcrack.h>
#include "selectors/IDesktopSelector.h"


class CRectAwarenessWnd : 	public CWindowImpl<CRectAwarenessWnd>
{

public:
	CRectAwarenessWnd(IDesktopSelector *pSelector);
	~CRectAwarenessWnd(void);
	// Optionally specify name of the new Windows class
	DECLARE_WND_CLASS("RectAwarenessWnd")

	BEGIN_MSG_MAP(CRectAwarenessWnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBck)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()

	LRESULT OnPaint(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBck(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	bool ShowAwarenessWindow();
	bool HideAwarenessWindow();

private:
	bool StartTimer();
	void UpdateAwarenessWndRectByCapRect(const RECT &capturingRect);
	bool GenerateRegion();
	void SetRegion();
	bool NormalizeRect(RECT &rec);
	void UpdateLayout();

private:
	UINT m_nTimerID;
	IDesktopSelector *m_pSelector;
	RECT m_capturingRect;
	RECT m_awarenessRect;
	CRgn m_region;
};

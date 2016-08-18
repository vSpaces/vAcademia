#include "StdAfx.h"
#include "..\..\include\selectors\cursorcapture.h"

CCursorCapture::CCursorCapture(void)
{
	m_canShow = false;
	m_needScale = false;
}

CCursorCapture::~CCursorCapture(void)
{
}
void CCursorCapture::DrawCursorIfNeeded(HDC hdcTo, RECT capturingRect)
{
	if (!m_canShow)
		return;
	POINT ptCursor;
	HCURSOR hCursor = GetCurrentCursorHandle(ptCursor);
	// курсор в зоне шаринга
	if (!(ptCursor.x >= capturingRect.left && ptCursor.x <= capturingRect.right
		&& ptCursor.y >= capturingRect.top && ptCursor.y <= capturingRect.bottom))
		return;
	if(hCursor != NULL)
	{
		ICONINFO info;

		if(GetIconInfo(hCursor, &info))
		{
			if (m_needScale)
				DrawIconEx(	hdcTo,
					ptCursor.x - info.xHotspot - capturingRect.left,
					ptCursor.y - info.yHotspot - capturingRect.top,
					hCursor,
					36,
					42,
					0,
					NULL,
					DI_NORMAL);
			else
				DrawIconEx(	hdcTo,
					ptCursor.x - info.xHotspot - capturingRect.left,
					ptCursor.y - info.yHotspot - capturingRect.top,
					hCursor,
					0,
					0,
					0,
					NULL,
					DI_DEFAULTSIZE | DI_NORMAL);
		}
	}
}

HCURSOR CCursorCapture::GetCurrentCursorHandle(POINT &pt)
{
	CURSORINFO curInfo;

	curInfo.cbSize = sizeof(CURSORINFO);

	if(GetCursorInfo(&curInfo))
	{
		pt = curInfo.ptScreenPos;
		return curInfo.hCursor;
	}

	return NULL;
}

void CCursorCapture::SetCursorState(int state)
{
	if (state == 0)
		m_canShow = false;
	else if (state == 1)
	{
		m_canShow = true;
		m_needScale = false;
	}
	else if (state == 2)
	{
		m_canShow = true;
		m_needScale = true;
	}
}
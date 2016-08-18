#pragma once

class DESKTOPMAN_API CCursorCapture
{
public:
	CCursorCapture(void);
	~CCursorCapture(void);

	void	DrawCursorIfNeeded(HDC hdcTo, RECT capturingRect);
	HCURSOR GetCurrentCursorHandle(POINT &pt);

public:
	//установить режим отображения курсора (0-скрыт, 1-отображается, 2-отображается увеличенным)
	void SetCursorState(int state);

private: 
	bool m_canShow;
	bool m_needScale;
};


#pragma once

#include "CommonRenderManagerHeader.h"

#define INITIAL_PLAYER_WIDTH	512

class CLastScreen
{
public:
	CLastScreen();
	~CLastScreen();

	std::wstring GetLastScreenPath();
	std::wstring GetLastScreenPathForLoading();

	void CopyLastScreen();
	void TakeLastScreen();

	bool IsActivated();

	void Hide();
	bool IsVisible();

	void Enable(const bool isEnabled);

private:
	bool m_isVisible;
	bool m_isEnabled;
	bool m_isActivated;
	bool m_isCheckedActivation;

	MP_WSTRING m_lastScreenPath;

	unsigned int m_lastScreenTextureID;
	unsigned int m_lastScreenTextureWidth;
	unsigned int m_lastScreenTextureHeight;
};
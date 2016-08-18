#pragma once

#include "Renderer.h"

class CMainRenderer
{
friend class CGlobalSingletonStorage;

public:
	__forceinline void SetMode(int mode)
	{
		m_mode = mode;
	
		switch (m_mode)
		{
		case MODE_RENDER_SOLID:
			m_modeIndex = MODE_INDEX_SOLID;
			break;

		case MODE_RENDER_TRANSPARENT:
			m_modeIndex = MODE_INDEX_TRANSPARENT;
			break;

		case MODE_RENDER_ALPHATEST:
			m_modeIndex = MODE_INDEX_ALPHATEST;
			break;

		case MODE_RENDER_ALL:
			m_modeIndex = MODE_INDEX_ALL;
			break;

		case MODE_RENDER_SOLID + MODE_RENDER_TRANSPARENT:
			m_modeIndex = MODE_INDEX_SOLID_TRANSPARENT;
			break;

		case MODE_RENDER_TRANSPARENT + MODE_RENDER_ALPHATEST:
			m_modeIndex = MODE_INDEX_TRANSPARENT_ALPHATEST;
			break;

		case MODE_RENDER_SOLID + MODE_RENDER_ALPHATEST:
			m_modeIndex = MODE_INDEX_SOLID_ALPHATEST;
			break;
		}
	}	

	__forceinline int GetMode()const
	{
		return m_mode;
	}

	__forceinline int GetModeIndex()const
	{
		return m_modeIndex;
	}

private:	
	int m_mode;
	int m_modeIndex;

	CMainRenderer();
	CMainRenderer(const CMainRenderer&);
	CMainRenderer& operator=(const CMainRenderer&);
	~CMainRenderer();
};

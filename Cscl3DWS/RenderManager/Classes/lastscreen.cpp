
#include "StdAfx.h"
#include "LastScreen.h"
#include "PlatformDependent.h"
#include "NRManager.h"
#include "IniFile.h"
#include <atlconv.h>
#include "GlobalSingletonStorage.h"
#include "RMContext.h"

CLastScreen::CLastScreen():
	m_lastScreenTextureID(0xFFFFFFFF),
	MP_WSTRING_INIT(m_lastScreenPath),
	m_lastScreenTextureHeight(0),
	m_isCheckedActivation(false),
	m_lastScreenTextureWidth(0),
	m_isActivated(false),
	m_isVisible(true),
	m_isEnabled(true)
{
	gRM->SetLastScreen(this);
}

bool CLastScreen::IsVisible()
{
	return ((m_isVisible) && (IsActivated()));
}

void CLastScreen::Hide()
{
	m_isVisible = false;
}

bool CLastScreen::IsActivated()
{
	if (!m_isCheckedActivation)
	{
		std::wstring iniFN = GetApplicationRootDirectory();
		iniFN += L"\\player.ini";
		CIniFile ini(iniFN);
		m_isActivated = (ini.GetString("run", "fast") != "true");
		m_isCheckedActivation = true;
	}

	return m_isActivated;
}

std::wstring CLastScreen::GetLastScreenPath()
{
	if (m_lastScreenPath.empty())
	{
		m_lastScreenPath = GetApplicationDataDirectory();
		m_lastScreenPath += L"\\Vacademia";
		CreateDirectory(m_lastScreenPath);
		m_lastScreenPath += L"\\new_va_last_screen_";
		m_lastScreenPath += gRM->nrMan->GetContext()->mpSM->GetClientStringID();
		m_lastScreenPath += L".png";
	}
		
	return m_lastScreenPath;
}

std::wstring CLastScreen::GetLastScreenPathForLoading()
{
	std::wstring path = gRM->lastScreen->GetLastScreenPath();
	
	if (!IsActivated()) 
	{
		path = L"1";
	}

	return path;
}

void CLastScreen::Enable(const bool isEnabled)
{
	/*if (!isEnabled)
	{
		m_isEnabled = true;
		TakeLastScreen();
	}*/

	m_isEnabled = isEnabled;
}

void CLastScreen::TakeLastScreen()
{
	if (!m_isEnabled)
	{
		return;
	}

	std::wstring fileName = GetLastScreenPath();


	if (g->scm.GetActiveScene())
	{
		SCropRect rect;

		rect.x = 0;
		rect.y = 0;
		rect.width =  g->stp.GetCurrentWidth();
		rect.height =  g->stp.GetCurrentHeight();		

		if ((gRM->nrMan->GetQualityMode() != LOW_QUALITY_MODE_ID) && (g->gi.GetVendorID() != VENDOR_INTEL))
		{
			int cnt = 0;
			while (((rect.width - rect.x) % 4) != 0)
			{
				rect.width--;			
			}

			if (g->scm.GetActiveScene()->GetPostEffect())
			{
				g->scm.GetActiveScene()->GetPostEffect()->SaveScreen(fileName, &rect);
			}
		}
		else
		{	
			if (m_lastScreenTextureID != 0xFFFFFFFF)
			{				
				if (!g->gi.IsNPOTSupport())
				{
					rect.y = GetMinBigger2st(m_lastScreenTextureHeight) - g->stp.GetCurrentHeight();
				}
				else
				{
					rect.y = 0;
				}				

				CTexture* texture = g->tm.GetObjectPointer(m_lastScreenTextureID);
				texture->SetTransparentStatus(true);

				g->tm.SaveTexture(m_lastScreenTextureID, fileName, &rect);
			}
		}
	}
	
}

void CLastScreen::CopyLastScreen()
{
	if (g->stp.GetCurrentWidth() == INITIAL_PLAYER_WIDTH)
	{
		return;
	}

	if (g->ne.isIconicAndIntel)
	{
		return;
	}

	if ((m_lastScreenTextureWidth != g->stp.GetCurrentWidth()) ||
		(m_lastScreenTextureHeight != g->stp.GetCurrentHeight())) 
	{
		if (m_lastScreenTextureID != 0xFFFFFFFF)
		{
			g->tm.DeleteObject(m_lastScreenTextureID);
			m_lastScreenTextureID = 0xFFFFFFFF;
		}
	}

	if (m_lastScreenTextureID == 0xFFFFFFFF)
	{
		glGenTextures(1, &m_lastScreenTextureID);
		glBindTexture(GL_TEXTURE_2D, m_lastScreenTextureID);
		m_lastScreenTextureWidth = g->stp.GetCurrentWidth();
		m_lastScreenTextureHeight = g->stp.GetCurrentHeight();

		if (!g->gi.IsNPOTSupport())
		{			
			int realWidth = GetMinBigger2st(m_lastScreenTextureWidth);
			int realHeight = GetMinBigger2st(m_lastScreenTextureHeight);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, realWidth, realHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			m_lastScreenTextureID = g->tm.ManageGLTexture("last_screen", m_lastScreenTextureID, m_lastScreenTextureWidth, m_lastScreenTextureHeight);
			CTexture* texture = g->tm.GetObjectPointer(m_lastScreenTextureID);
			texture->SetTextureRealWidth(realWidth);
			texture->SetTextureRealHeight(realHeight);
		}
		else
		{		
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_lastScreenTextureWidth, m_lastScreenTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			m_lastScreenTextureID = g->tm.ManageGLTexture("last_screen", m_lastScreenTextureID, m_lastScreenTextureWidth, m_lastScreenTextureHeight);
		}
	}

	CTexture* texture = g->tm.GetObjectPointer(m_lastScreenTextureID);
	glBindTexture(GL_TEXTURE_2D, texture->GetNumber());

	if (!g->gi.IsNPOTSupport())
	{
		int offsetY = GetMinBigger2st(g->stp.GetCurrentHeight()) - g->stp.GetCurrentHeight();
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, offsetY, 0, 0, g->stp.GetCurrentWidth(), g->stp.GetCurrentHeight());	
	}
	else
	{
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, g->stp.GetCurrentWidth(), g->stp.GetCurrentHeight(), 0);
	}

	g->tm.RefreshTextureOptimization();
}

CLastScreen::~CLastScreen()
{
}
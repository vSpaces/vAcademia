
#include "StdAfx.h"
#include "Special.h"
#include "IniFile.h"
#include "UserData.h"
#include "PlatformDependent.h"
#include "WhiteBoardManager.h"
#include "SimpleDrawingTarget.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

#define MAX_RESERVE_WB_IMAGES	100

CWhiteBoardManager::CWhiteBoardManager():
	MP_VECTOR_INIT(m_wbImages),
	m_isInited(false),
	m_scaleKoef(1),
	m_count(0)
{
	gRM->SetWhiteBoardManager(this);

	m_wbImages.reserve(MAX_RESERVE_WB_IMAGES);
}

void CWhiteBoardManager::PrintDebugInfo()
{
	g->lw.WriteLn("========");
	g->lw.WriteLn("BINARY STATES");
	g->lw.WriteLn("========");

	int emptyCount = 0;

	std::vector<nrmImage *>::iterator it = m_wbImages.begin();
	std::vector<nrmImage *>::iterator itEnd = m_wbImages.end();

	for ( ; it != itEnd; it++)
	{
		wchar_t* name = (*it)->GetMLMedia()->GetParentElement()->GetParentElement()->GetStringProp("name");
		USES_CONVERSION;
		std::string objName = W2A(name);

		g->lw.WriteLn("Binary state for ", objName, " has size ", (*it)->GetBinarySize(), " bytes");
		if (!(*it)->isPaintToolUsed())
		{
			emptyCount++;
		}
	}

	g->lw.WriteLn("Empty wb count: ", emptyCount);
}

void CWhiteBoardManager::OnDeleteWhiteboard(nrmImage* img)
{
	m_count--;
	DeleteWBImage(img);
}

bool CWhiteBoardManager::CreateWhiteBoard(C3DObject* obj, nrmImage* img, int& textureID, int auID)
{
	std::wstring fn = GetApplicationRootDirectory();
	fn += L"\\player.ini";
	CIniFile ini(fn);
	if (ini.GetString("player", "nowb") == "true")
	{
		return false;
	}
	/*if (name.find("sfera_ekran02") != -1)
	{
		return false;
	}*/
	
	/*if (g->cm.GetActiveCamera())
	{
		CVector3D cam = g->cm.GetActiveCamera()->GetEyePosition();
		CVector3D pos = obj->GetCoords();
		int len = (cam - pos).GetLengthSq();
		if (len > 13000 * 13000)
		{
			return false;
		}
	}*/

	
	m_count++;

	DetectWhiteBoardSizeIfNeeded();

	CSimpleDrawingTarget* target = MP_NEW(CSimpleDrawingTarget);
	USES_CONVERSION;
	target->SetName( W2A(obj->GetName()));
	int textureWidth = 128;
	int textureHeight = 128;
	if (-1 == textureID)
	{
		USES_CONVERSION;		
		if (img->GetMLMedia()->GetIntProp("width") != 0)
		{
			textureWidth = img->GetMLMedia()->GetIntProp("width");
		}		
		if (img->GetMLMedia()->GetIntProp("height") != 0)
		{
			textureHeight = img->GetMLMedia()->GetIntProp("height");
		}

		textureWidth /= m_scaleKoef;
		textureHeight /= m_scaleKoef;
		
		target->SetWidth(textureWidth);
		target->SetHeight(textureHeight);
		target->SetFormat(true, CFrameBuffer::depth16);
		int _textureID = target->CreateEmptySmallTexture();/*Temp*/

		textureID = g->tm.ManageGLTexture("whiteboard", _textureID, /*textureWidth, textureHeight*/4, 4);
		obj->GetLODGroup()->SetTexture(auID, textureID, true, 1);
		
		std::wstring wbSpriteName = L"whiteboard";
		img->CreateSpriteIfNeeded(wbSpriteName);
		img->m_sprite->SetTextureID(textureID);
		img->OnChanged(0);
		img->m_sprite->SetFullWorkRect();
		CTexture* texture = g->tm.GetObjectPointer(textureID);
		texture->UpdateCurrentFiltration(GL_NEAREST, GL_NEAREST);
		texture->SetYInvertStatus(false);
		texture->SetTransparentStatus(true);
	}

	CTexture* texture = g->tm.GetObjectPointer(textureID);
	target->AttachTexture(texture);
	target->SetClearColor(255, 255, 255, 1);
	target->SetFormat(true, CFrameBuffer::depth16);
	target->SetInvScale((unsigned char)m_scaleKoef);
	target->SetWidth(/*texture->GetTextureRealWidth()*/textureWidth * m_scaleKoef);
	target->SetHeight(/*texture->GetTextureRealHeight()*/textureHeight * m_scaleKoef);

	obj->GetLODGroup()->OnChanged(0);

	CToolPalette* palette = MP_NEW(CToolPalette);
	palette->SetDrawingTarget(target);
	palette->SetName(obj->GetName());
	img->SetPalette(palette);
	obj->SetUserData(USER_DATA_RENDER_TARGET, (void*)palette);		

	AddWBImage(img);

	return true;
}

void CWhiteBoardManager::DeleteWBImage(nrmImage* img)
{
	std::vector<nrmImage *>::iterator it = m_wbImages.begin();
	std::vector<nrmImage *>::iterator itEnd = m_wbImages.end();

	for ( ; it != itEnd; it++)
	if (*it == img)
	{
		m_wbImages.erase(it);
		return;
	}
}

void CWhiteBoardManager::AddWBImage(nrmImage* img)
{
	std::vector<nrmImage *>::iterator it = m_wbImages.begin();
	std::vector<nrmImage *>::iterator itEnd = m_wbImages.end();

	for ( ; it != itEnd; it++)
	if (*it == img)
	{
		return;
	}

	m_wbImages.push_back(img);
}

void CWhiteBoardManager::DetectWhiteBoardSizeIfNeeded()
{
	if (m_isInited)
	{
		return;
	}

	m_isInited = true;
	m_scaleKoef = 1;

	if (g->gi.GetVideoMemory()->GetTotalSize() < 129 * 1024 * 1024)
	{
		m_scaleKoef = 2;
	}

	if ((g->gi.GetVendorID() == VENDOR_INTEL) || (g->gi.GetVendorID() == VENDOR_SIS))
	{
		m_scaleKoef = 2;
	}

#ifdef ENABLE_INTEL_EMULATION
	m_scaleKoef = 2;
#endif
}

void CWhiteBoardManager::SetMapManager(CMapManager* mapManager)
{
	m_hideDistance = 10000;

	if ((g->gi.GetVendorID() == VENDOR_INTEL) || (g->gi.GetVendorID() == VENDOR_SIS))
	{
		std::string js = "scene.checkDistance = 4500;";
		mapManager->ExecJS(js.c_str());
		m_hideDistance = 4500;
	}
}

unsigned int CWhiteBoardManager::GetHideDistance()const
{
	return m_hideDistance;
}

CWhiteBoardManager::~CWhiteBoardManager()
{
}
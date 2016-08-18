#include "stdafx.h"
#include ".\sharingmousecontroller.h"


CSharingMouseController::CSharingMouseController(oms::omsContext* aContext, float aspectX, float aspectY, char* target):
	MP_STRING_INIT(alpTarget),
	m_ctrl(NULL)
{
	context = aContext;
	m_needEvents = false;
	m_dynamictexture = NULL;
	m_drawingTarget = NULL;
	m_aspectX = aspectX;
	m_aspectY = aspectY;
	alpTarget = "scene.objects.";
	alpTarget += target;
	alpTarget += ".pscreenObj.sharingCtrl.sharingImg";

	m_updateCnt = 0;

	//scene.objects.sqvu_screen_senter.pscreenObj.sharingCtrl.sharingImg
}

void CSharingMouseController::SetMouseController(IMouseController* ctrl)
{
	m_ctrl = ctrl;
}

unsigned int CSharingMouseController::GetTextureX(float x)
{	
	x = x - (0.5f - 0.5f / m_aspectX);
	return (unsigned int)(m_dynamictexture->GetSourceWidth() * x * m_aspectX);
}

bool CSharingMouseController::CheckArea(float x, float y)
{
	if (!m_dynamictexture)
	{
		return false;
	}

	if (x < (0.5f - 0.5f / m_aspectX) || x > (0.5f + 0.5f / m_aspectX))
	{
		return false;
	}

	if (y < (0.5f - 0.5f / m_aspectY) || y > (0.5f + 0.5f / m_aspectY))
	{
		return false;
	}
	return true;
}

unsigned int CSharingMouseController::GetTextureY(float y)
{
	y = y - (0.5f - 0.5f / m_aspectY);
	return (unsigned int)(m_dynamictexture->GetSourceHeight() * y * m_aspectY);
}

void CSharingMouseController::OnLMouseDown(float x, float y)
{
	if (!CheckArea(x,y))
		return;
	UpdateActiveTarget();
	desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
	if (pHandler)
		pHandler->OnLeftMouseDown(GetTextureX(x), GetTextureY(y));
}

void CSharingMouseController::OnLMouseUp(float x, float y)
{
	if (!CheckArea(x,y))
		return;
	UpdateActiveTarget();
	desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
	if (pHandler)
		pHandler->OnLeftMouseUp(GetTextureX(x), GetTextureY(y));
}

void CSharingMouseController::OnRMouseDown(float x, float y)
{
	if (!CheckArea(x,y))
		return;
	UpdateActiveTarget();
	desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
	if (pHandler)
		pHandler->OnRightMouseDown(GetTextureX(x), GetTextureY(y));
}

void CSharingMouseController::OnRMouseUp(float x, float y)
{
	if (!CheckArea(x,y))
		return;
	UpdateActiveTarget();
	desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
	if (pHandler)
		pHandler->OnRightMouseUp(GetTextureX(x), GetTextureY(y));
}

void CSharingMouseController::OnMouseMove(float x, float y)
{
	if (!CheckArea(x,y))
		return;
	UpdateActiveTarget();
	desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
	if (pHandler)
		pHandler->OnMouseMove(GetTextureX(x), GetTextureY(y));
}

void CSharingMouseController::OnChar(int key, int scancode)
{
	UpdateActiveTarget();
	desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
	if (pHandler)
		pHandler->OnKeyDown(key, scancode);
}

void CSharingMouseController::OnKeyUp(int key)
{
	UpdateActiveTarget();
	desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
	if (pHandler)
		pHandler->OnKeyUp(0, key);
}

void CSharingMouseController::OnLMouseDblClick(float x, float y)
{
	if (!CheckArea(x,y))
		return;
	UpdateActiveTarget();
	desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
	if (pHandler)
		pHandler->OnLeftMouseDblClk(GetTextureX(x), GetTextureY(y));
}

void CSharingMouseController::OnRMouseDblClick(float x, float y)
{
	if (!CheckArea(x,y))
		return;
	UpdateActiveTarget();
	desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
	if (pHandler)
		pHandler->OnRightMouseDblClk(GetTextureX(x), GetTextureY(y));
}

bool CSharingMouseController::IsNeedMouseEvents()
{
	return m_needEvents;
}

void CSharingMouseController::SetDrawingTarget(IDrawingTarget* drawingTarget)
{
	m_drawingTarget = drawingTarget;
}

void CSharingMouseController::SetNeedEvent(bool needEvent)
{
	m_needEvents = needEvent;
}

void CSharingMouseController::SetDinamicTexture(IDynamicTexture* apIDynamicTexture)
{
	m_dynamictexture = apIDynamicTexture;
}

desktop::IMouseKeyboardHandler* CSharingMouseController::GetIMouseKeyboardHandler()
{
	if (!context)
		return NULL;
	desktop::IDesktopSharingManager* pSharingMan = context->mpSharingMan;
	if (!pSharingMan)
		return NULL;
	desktop::IMouseKeyboardHandler* pHandler = (desktop::IMouseKeyboardHandler*)pSharingMan->GetIMouseKeyboardHandler();
	if (!pHandler)
		return NULL;
	return pHandler;
}

void CSharingMouseController::UpdateActiveTarget()
{
	desktop::IMouseKeyboardHandler* pHandler = GetIMouseKeyboardHandler();
	if (pHandler)
		pHandler->SetActiveTarget(alpTarget.c_str());
	if (context && context->mpMapMan)
	{
		int currentRealityID = context->mpMapMan->GetCurrentRealityID();
		if (pHandler)
			pHandler->SetRealityId(currentRealityID);
	}
	//m_updateCnt++;
}

CSharingMouseController::~CSharingMouseController()
{
	if (m_ctrl)
	{
		m_ctrl->RemoveMouseListenerFromLastUsed(this);
	}
}
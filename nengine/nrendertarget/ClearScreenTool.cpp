
#include "StdAfx.h"
#include "ClearScreenTool.h"
#include "ToolsParameters.h"

CClearScreenTool::CClearScreenTool():
	m_color(0, 0, 0, 1)
{
}

void CClearScreenTool::SetDrawingTarget(IDrawingTarget* drawingTarget)
{
	m_drawingTarget = drawingTarget;
}

void CClearScreenTool::SetParameter(int paramID, void* param)
{
	switch (paramID)
	{
	case CLEAR_COLOR:
		{
			unsigned int tmp = (unsigned int)param;
			m_color = *(CColor4 *)&tmp;
		}
		break;
	}
}

void CClearScreenTool::OnMouseMove(int /*x*/, int /*y*/)
{
}

void CClearScreenTool::OnLMouseDown(int /*x*/, int /*y*/)
{
}

void CClearScreenTool::OnLMouseUp(int /*x*/, int /*y*/)
{
}

void CClearScreenTool::OnRMouseDown(int /*x*/, int /*y*/)
{
}

void CClearScreenTool::OnRMouseUp(int /*x*/, int /*y*/)
{
}

void CClearScreenTool::OnChar(int /*keyCode*/, int /*scancode*/)
{
}

void CClearScreenTool::RedoAction(void* /*data*/)
{
}

void CClearScreenTool::OnSelected()
{
	if (m_drawingTarget->IsCreatedNow())
	{
		m_drawingTarget->SetClearColor(m_color.r, m_color.g, m_color.b, m_color.a);
		m_drawingTarget->Clear();
		m_drawingTarget->SaveBackBuffer();
	}
}

void CClearScreenTool::OnDeselected()
{
}

void CClearScreenTool::OnEndRestoreState()
{
}

int CClearScreenTool::GetSynchRequiredType()
{
	return SYNCH_MOVE_NONE;
}

void CClearScreenTool::Destroy()
{
	MP_DELETE_THIS;
}

CClearScreenTool::~CClearScreenTool()
{
}

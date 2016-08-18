
#include "StdAfx.h"
#include "RectangleTool.h"
#include "GlobalVars.h"
#include "GlobalParams.h"
#include "ToolsParameters.h"
#include "CommonToolRoutines.h"
#include "RectangleCommonRoutines.h"

CRectangleTool::CRectangleTool():
	m_mode(SHAPE_MODE_FILLSTROKE),
	m_opacity(128),
	m_lastMouseX(0), 
	m_lastMouseY(0), 
	m_strokeWidth(10),
	m_isWorking(false),
	m_drawingTarget(NULL),
	m_color(0, 0, 0, 255),
	m_backColor(0, 255, 0, 255),
	m_isMoveMode(false)
{
	m_startX = -1000;
}

void CRectangleTool::SetDrawingTarget(IDrawingTarget* drawingTarget)
{
	m_drawingTarget = drawingTarget;
}

void CRectangleTool::SetParameter(int paramID, void* param)
{
	switch (paramID)
	{
	case RECTANGLE_COLOR:
		{
			unsigned int tmp = (unsigned int)param;
			m_color = *(CColor4 *)&tmp;
		}
		break;

	case RECTANGLE_BACK_COLOR:
		{
			unsigned int tmp = (unsigned int)param;
			m_backColor = *(CColor4 *)&tmp;
		}
		break;

	case RECTANGLE_STROKE_WIDTH:
		m_strokeWidth = (unsigned char)param;
		break;

	case RECTANGLE_OPACITY:
		m_opacity = (unsigned char)param;
		break;

	case RECTANGLE_MODE:
		m_mode = (unsigned char)param;
		break;
	}
}

void CRectangleTool::Draw()
{
	glRectX1 = (float)(m_startX);
	glRectX2 = (float)(m_endX);
	glRectY1 = (float)(m_startY);
	glRectY2 = (float)(m_endY);

	if ((m_startX < m_endX) && (m_startY >  m_endY))
	{
		glRectY1 = (float)(m_endY);
		glRectY2 = (float)(m_startY);
	}
	else if (m_startX > m_endX)
	{
		if (m_startY > m_endY)
		{
			glRectX1 = (float)(m_endX);
			glRectX2 = (float)(m_startX);
			glRectY1 = (float)(m_endY);
			glRectY2 = (float)(m_startY);
		}
		else
		{
			glRectX1 = (float)(m_startX);
			glRectX2 = (float)(m_endX);
		}
	}

	glRectX1 = m_drawingTarget->GetGLX((int)glRectX1);
	glRectX2 = m_drawingTarget->GetGLX((int)glRectX2);

	glRectY1 = m_drawingTarget->GetGLY((int)glRectY1);
	glRectY2 = m_drawingTarget->GetGLY((int)glRectY2);

	glRectStroke = fabs(m_drawingTarget->GetGLY(0) - m_drawingTarget->GetGLY(m_strokeWidth));

	glRectX0 = glRectX1 - glRectStroke;
	glRectX3 = glRectX2 + glRectStroke;

	glRectY0 = glRectY1 - glRectStroke;
	glRectY3 = glRectY2 + glRectStroke;
	
	glRectOpacity = m_opacity;

	if (m_drawingTarget->GetType() == RENDER_TARGET_MAINBUFFER)
	{
		glRectOpacity = (glRectOpacity == 255) ? 254 : glRectOpacity;
	}

	glRectMode = m_mode;

	glColorR = m_color.r;
	glColorG = m_color.g;
	glColorB = m_color.b;

	glBackColorR = m_backColor.r;
	glBackColorG = m_backColor.g;
	glBackColorB = m_backColor.b;

#ifndef TOOL_OPTIMIZATION
	m_drawingTarget->ApplyGeometry(ClearZBuffer);
	m_drawingTarget->ApplyGeometry(DrawSquareShape);
#else	
	m_drawingTarget->ApplyGeometry(DrawSquareShapeAndClearZBuffer);
#endif
}

void CRectangleTool::OnMouseMove(int x, int y)
{
	if (m_isWorking)
	{	
		if (!m_isMoveMode)
		{
			m_endX = x;
			m_endY = y;
		}
		else
		{
			MovingMiddle(x, y);
		}

#ifdef TOOL_OPTIMIZATION
		if (!g_isPlaying)
		{
#endif
			m_drawingTarget->RestoreBackBuffer();
			Draw();
#ifdef TOOL_OPTIMIZATION
		}
#endif
	}
}

void CRectangleTool::OnLMouseDown(int x, int y)
{
	m_isWorking = true;

	m_startX = x;
	m_startY = y;
	m_endX = m_startX;
	m_endY = m_startY;

	m_drawingTarget->SaveBackBuffer();
}

void CRectangleTool::OnLMouseUp(int x, int y)
{
	if (m_isWorking)	
	{	
		m_endX = x;
		m_endY = y;

		m_drawingTarget->RestoreBackBuffer();
		Draw();
		
	}

	m_isWorking = false;
}

void CRectangleTool::OnRMouseDown(int /*x*/, int /*y*/)
{
	if (m_startX != -1000)
	{
		m_isWorking = true;
		m_isMoveMode = true;
	}
}

void CRectangleTool::OnRMouseUp(int x, int y)
{
	if (m_isWorking)	
	{	
		m_drawingTarget->RestoreBackBuffer();
		MovingMiddle(x, y);
		Draw();
	}

	m_isWorking = false;
	m_isMoveMode = false;
}

void CRectangleTool::OnChar(int /*keyCode*/, int scancode)
{
		if (37 == scancode)
		{
			m_startX--;
			m_endX--;
		}
		else if (38 == scancode)
		{
			m_startY--;
			m_endY--;
		}
		else if (39 == scancode)
		{
			m_startX++;
			m_endX++;
		}
		else if (40 == scancode)
		{
			m_startY++;
			m_endY++;
		}

		if ((scancode == 37) || (scancode == 38) ||
			(scancode == 39) || (scancode == 40))
		{
			m_drawingTarget->RestoreBackBuffer();
			Draw();
		}
}

void CRectangleTool::RedoAction(void* /*data*/)
{
}

void CRectangleTool::OnSelected()
{
}

void CRectangleTool::OnDeselected()
{
	m_startX = -1000;
}

void CRectangleTool::OnEndRestoreState()
{
}

int CRectangleTool::GetSynchRequiredType()
{
	return SYNCH_KEYS;
}

void CRectangleTool::Destroy()
{
	MP_DELETE_THIS;
}

void CRectangleTool::MovingMiddle(int x, int y)
{
	int halfWidth = abs(m_startX - m_endX)/2;
	int halfHeight = abs(m_startY - m_endY)/2;

	m_startX = x - halfWidth;
	m_endX =  x + halfWidth;

	m_startY = y - halfHeight;
	m_endY = y + halfHeight;
}

CRectangleTool::~CRectangleTool()
{
}
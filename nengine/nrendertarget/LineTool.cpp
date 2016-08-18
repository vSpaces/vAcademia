
#include "StdAfx.h"
#include "Vector2D.h"
#include "LineTool.h"
#include "GlobalVars.h"
#include "GlobalParams.h"
#include "ToolsParameters.h"
#include "CommonToolRoutines.h"
#include "GlobalSingletonStorage.h"

float glLineX1, glLineX2, glLineX3, glLineX0;
float glLineY1, glLineY2, glLineY3, glLineY0;
GLubyte glLineOpacity;
bool glIsArrow1Enabled, glIsArrow2Enabled;

void DrawLine()
{
	g->stp.SetState(ST_CULL_FACE, false);
	g->stp.PrepareForRender();
	GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glVertex3f)(glLineX1, glLineY1, 0);
		GLFUNC(glVertex3f)(glLineX2, glLineY2, 0);
		GLFUNC(glVertex3f)(glLineX3, glLineY3, 0);
		GLFUNC(glVertex3f)(glLineX0, glLineY0, 0);
	GLFUNC(glEnd)();
	g->stp.SetState(ST_CULL_FACE, true);
}

void DrawLineShape()
{
	if (glLineOpacity != 255)
	{
		g->stp.SetState(ST_BLEND, true);
		g->stp.SetState(ST_ZTEST, true);
		GLFUNC(glDepthFunc)(GL_LESS);
		g->stp.SetBlendFunc(GL_ONE, GL_ONE);
		g->stp.SetColor(0, 0, 0, glLineOpacity);
		GLFUNC(glDisable)(GL_TEXTURE_2D);
		
		if ((glLineX1 != glLineX2) || (glLineY1 != glLineY2))
		{		
			DrawLine();
		}

		g->stp.SetState(ST_ZWRITE, true);
		g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	}

	g->stp.SetState(ST_ZTEST, false);
	GLFUNC(glDisable)(GL_TEXTURE_2D);

	if ((glLineX1 != glLineX2) || (glLineY1 != glLineY2))
	{
		g->stp.SetColor(glColorR, glColorG, glColorB, glLineOpacity);
		DrawLine();
		g->stp.SetColor(255, 255, 255);
	}

	g->stp.SetState(ST_ZTEST, true);
	GLFUNC(glEnable)(GL_TEXTURE_2D);

	if (glLineOpacity != 255)
	{
		g->stp.SetState(ST_ZTEST, false);
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
}

void DrawLineShapeAndClearZBuffer()
{
	DrawLineShape();
	ClearZBuffer();
}

CLineTool::CLineTool():
	m_opacity(128),
	m_lineWidth(3),
	m_lastMouseX(0), 
	m_lastMouseY(0), 
	m_isWorking(false),
	m_arrowLength(50.0f),
	m_drawingTarget(NULL),
	m_color(255, 0, 0, 255),
	m_isArrow1Enabled(false),
	m_isArrow2Enabled(false),
	m_isMoveMode(false)
{
	m_startX = -1000;
}

void CLineTool::SetDrawingTarget(IDrawingTarget* drawingTarget)
{
	m_drawingTarget = drawingTarget;
}

void CLineTool::SetParameter(int paramID, void* param)
{
	switch (paramID)
	{
	case LINE_COLOR:
		{
			unsigned int tmp = (unsigned int)param;
			m_color = *(CColor4 *)&tmp;
		}
		break;

	case LINE_WIDTH:
		m_lineWidth = (unsigned char)param;
		break;

	case LINE_OPACITY:
		m_opacity = (unsigned char)param;
		break;

	case LINE_ARROW1:
		m_isArrow1Enabled = ((int)param > 0);
		break;

	case LINE_ARROW2:
		m_isArrow2Enabled = ((int)param > 0);
		break;

	case LINE_ARROW_LENGTH:
		m_arrowLength = (float)((int)param);
		break;
	}
}

void CLineTool::PrepareLine(int startX, int startY, int endX, int endY)
{
	float stroke = fabs(m_drawingTarget->GetGLX(0) - m_drawingTarget->GetGLX(m_lineWidth));

	glLineX1 = m_drawingTarget->GetGLX(startX);
	glLineX2 = m_drawingTarget->GetGLX(endX);
	glLineY1 = m_drawingTarget->GetGLY(startY);
	glLineY2 = m_drawingTarget->GetGLY(endY);

	glLineX0 = m_drawingTarget->GetGLX(startX);
	glLineX3 = m_drawingTarget->GetGLX(endX);
	glLineY0 = m_drawingTarget->GetGLY(startY);
	glLineY3 = m_drawingTarget->GetGLY(endY);

	float dx1 = 0;
	float dy1 = 0;
	float dx2 = 0;
	float dy2 = 0;

	float length = sqrtf((glLineX1 - glLineX2) * (glLineX1 - glLineX2) + (glLineY1 - glLineY2) * (glLineY1 - glLineY2));
	if (length > 0)
	{
		float sinA = (glLineY1 - glLineY2) / length;
		float cosA = (glLineX1 - glLineX2) / length;

		dx1 = (stroke / 2) * sinA;
		dy1 = (stroke / 2) * cosA;
		dx2 = (stroke - (stroke / 2)) * sinA;
		dy2 = (stroke - (stroke / 2)) * cosA;
	}

	glLineX1 += dx1;
	glLineX2 += dx1;
	glLineY1 -= dy1;
	glLineY2 -= dy1;

	glLineX0 -= dx2;
	glLineX3 -= dx2;
	glLineY0 += dy2;
	glLineY3 += dy2;
}

void CLineTool::Draw()
{
	int deltaX = m_endX - m_startX;
	int deltaY = m_endY - m_startY;
	float deltaR = sqrtf((float)(deltaX * deltaX + deltaY * deltaY)) / m_arrowLength;
	float _deltaX = deltaX / deltaR;
	float _deltaY = deltaY / deltaR;
	CVector2D arrow1_1(_deltaX, _deltaY);
	CVector2D arrow1_2(_deltaX, _deltaY);
	CVector2D arrow2_1(-_deltaX, -_deltaY);
	CVector2D arrow2_2(-_deltaX, -_deltaY);
	arrow1_1.Rotate(30.0f);
	arrow1_2.Rotate(-30.0f);
	arrow2_1.Rotate(30.0f);
	arrow2_2.Rotate(-30.0f);
	
	glLineOpacity = m_opacity;

	if (m_drawingTarget->GetType() == RENDER_TARGET_MAINBUFFER)
	{
		glLineOpacity = (glLineOpacity == 255) ? 254 : glLineOpacity;
	}

	glColorR = m_color.r;
	glColorG = m_color.g;
	glColorB = m_color.b;

	glIsArrow1Enabled = m_isArrow1Enabled;
	glIsArrow2Enabled = m_isArrow2Enabled;

#ifndef TOOL_OPTIMIZATION
	m_drawingTarget->ApplyGeometry(ClearZBuffer);
#endif
	PrepareLine(m_startX, m_startY, m_endX, m_endY);
#ifdef TOOL_OPTIMIZATION
	m_drawingTarget->ApplyGeometry(DrawLineShapeAndClearZBuffer);
#else
	m_drawingTarget->ApplyGeometry(DrawLineShape);
#endif

	if (m_isArrow1Enabled)
	{		
		PrepareLine(m_startX, m_startY, m_startX + (int)arrow1_1.x, m_startY + (int)arrow1_1.y);
		m_drawingTarget->ApplyGeometry(DrawLineShape);
		PrepareLine(m_startX, m_startY, m_startX + (int)arrow1_2.x, m_startY + (int)arrow1_2.y);
		m_drawingTarget->ApplyGeometry(DrawLineShape);
	}

	if (m_isArrow2Enabled)
	{
		PrepareLine(m_endX, m_endY, m_endX + (int)arrow2_1.x, m_endY + (int)arrow2_1.y);
		m_drawingTarget->ApplyGeometry(DrawLineShape);
		PrepareLine(m_endX, m_endY, m_endX + (int)arrow2_2.x, m_endY + (int)arrow2_2.y);
		m_drawingTarget->ApplyGeometry(DrawLineShape);
	}
}

void CLineTool::OnMouseMove(int x, int y)
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

void CLineTool::OnLMouseDown(int x, int y)
{
	m_isWorking = true;

	m_startX = x;
	m_startY = y;
	m_endX = m_startX;
	m_endY = m_startY;

	m_drawingTarget->SaveBackBuffer();
}

void CLineTool::OnLMouseUp(int x, int y)
{
	if (m_isWorking)	
	{	
		m_endX = x;
		m_endY = y;

		m_drawingTarget->RestoreBackBuffer();
		Draw();
		
		//m_startX = m_endX;
		//m_startY = m_endY;
	}

	m_isWorking = false;
}

void CLineTool::OnRMouseDown(int /*x*/, int /*y*/)
{
	if (m_startX != -1000)
	{
		m_isWorking = true;
		m_isMoveMode = true;
	}
}

void CLineTool::OnRMouseUp(int x, int y)
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

void CLineTool::OnChar(int keyCode, int /*scancode*/)
{
	if (37 == keyCode)
	{
		m_startX--;
		m_endX--;
	}
	else if (38 == keyCode)
	{
		m_startY--;
		m_endY--;
	}
	else if (39 == keyCode)
	{
		m_startX++;
		m_endX++;
	}
	else if (40 == keyCode)
	{
		m_startY++;
		m_endY++;
	}

	if ((keyCode == 37) || (keyCode == 38) ||
		(keyCode == 39) || (keyCode == 40))
	{
		m_drawingTarget->RestoreBackBuffer();
		Draw();
	}
}

void CLineTool::RedoAction(void* /*data*/)
{
}

void CLineTool::OnSelected()
{
}

void CLineTool::OnDeselected()
{
	m_startX = -1000;
}

void CLineTool::OnEndRestoreState()
{
}

int CLineTool::GetSynchRequiredType()
{
	return SYNCH_KEYS;
}

void CLineTool::Destroy()
{
	MP_DELETE_THIS;
}

void CLineTool::MovingMiddle(int x, int y)
{
	int halfWidth = abs(m_startX - m_endX)/2;
	int halfHeight = abs(m_startY - m_endY)/2;

	m_startX = x - halfWidth;
	m_endX =  x + halfWidth;

	m_startY = y - halfHeight;
	m_endY = y + halfHeight;
}

CLineTool::~CLineTool()
{
}
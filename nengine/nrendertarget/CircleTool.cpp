
#include "StdAfx.h"
#include "CircleTool.h"
#include "GlobalVars.h"
#include "GlobalParams.h"
#include "ToolsParameters.h"
#include "CommonToolRoutines.h"
#include "GlobalSingletonStorage.h"

float glCircleX1, glCircleX2, glCircleY1, glCircleY2;
float glCircleCenterX, glCircleCenterY;
float glCircleRadiusX, glCircleRadiusY;
float glCircleStroke;
GLubyte glCircleOpacity;
int glCircleMode;

void DrawCircleBoundingBox()
{
	g->stp.SetState(ST_ZTEST, false);
	GLFUNC(glDisable)(GL_TEXTURE_2D);

	if ((glCircleX1 != glCircleX2) || (glCircleY1 != glCircleY2))
	{
		g->stp.SetColor(0, 0, 0);			

		g->stp.PrepareForRender();
		GLFUNC(glBegin)(GL_LINES);
			GLFUNC(glVertex3f)(glCircleX1, glCircleY1, 0);
			GLFUNC(glVertex3f)(glCircleX1, glCircleY2, 0);

			GLFUNC(glVertex3f)(glCircleX1, glCircleY2, 0);
			GLFUNC(glVertex3f)(glCircleX2, glCircleY2, 0);

			GLFUNC(glVertex3f)(glCircleX2, glCircleY2, 0);
			GLFUNC(glVertex3f)(glCircleX2, glCircleY1, 0);

			GLFUNC(glVertex3f)(glCircleX2, glCircleY1, 0);
			GLFUNC(glVertex3f)(glCircleX1, glCircleY1, 0);
		GLFUNC(glEnd)();
	}

	g->stp.SetState(ST_ZTEST, false);
	GLFUNC(glEnable)(GL_TEXTURE_2D);
}

void DrawCircle()
{
	float x1, x2 ,y1, y2;

	g->stp.PrepareForRender();
	GLFUNC(glBegin)(GL_TRIANGLES);

	for (int i = 0; i < SIN_COS_COUNT; i++)
	{			
		x1 = glCircleCenterX + cosTable[i] * glCircleRadiusX;
		y1 = glCircleCenterY + sinTable[i] * glCircleRadiusY;			
		x2 = glCircleCenterX + cosTable[i + 1] * glCircleRadiusX;
		y2 = glCircleCenterY + sinTable[i + 1] * glCircleRadiusY;

		GLFUNC(glVertex3f)(glCircleCenterX, glCircleCenterY, 0);
		GLFUNC(glVertex3f)(x1, y1, 0); 
		GLFUNC(glVertex3f)(x2, y2, 0);
	}

	GLFUNC(glEnd)();
}

void DrawCircleStroke()
{
	float x1, x2 ,y1, y2, x3, x4, y3, y4;
	float glCircleRadiusX2 = glCircleRadiusX + glCircleStroke;
	float glCircleRadiusY2 = glCircleRadiusY + glCircleStroke;

	g->stp.SetState(ST_CULL_FACE, false);

	g->stp.PrepareForRender();
	GLFUNC(glBegin)(GL_QUADS);
	for (int i = 0; i < SIN_COS_COUNT; i++)
	{			
		x1 = glCircleCenterX + cosTable[i] * glCircleRadiusX;
		y1 = glCircleCenterY + sinTable[i] * glCircleRadiusY;			
		x2 = glCircleCenterX + cosTable[i + 1] * glCircleRadiusX;
		y2 = glCircleCenterY + sinTable[i + 1] * glCircleRadiusY;
		x3 = glCircleCenterX + cosTable[i] * glCircleRadiusX2;
		y3 = glCircleCenterY + sinTable[i] * glCircleRadiusY2;			
		x4 = glCircleCenterX + cosTable[i + 1] * glCircleRadiusX2;
		y4 = glCircleCenterY + sinTable[i + 1] * glCircleRadiusY2;
	
		GLFUNC(glVertex3f)(x1,y1,0); 
		GLFUNC(glVertex3f)(x2,y2,0);
		GLFUNC(glVertex3f)(x4,y4,0);
		GLFUNC(glVertex3f)(x3,y3,0); 
	}
	GLFUNC(glEnd)();	

	g->stp.SetState(ST_CULL_FACE, true);
}

void DrawCircleShape()
{
	if (glCircleOpacity != 255)
	{
		g->stp.SetState(ST_BLEND, true);
		g->stp.SetState(ST_ZTEST, true);
		GLFUNC(glDepthFunc)(GL_LESS);
		g->stp.SetBlendFunc(GL_ONE, GL_ONE);
		g->stp.SetColor(0, 0, 0, glCircleOpacity);
		GLFUNC(glDisable)(GL_TEXTURE_2D);

		if ((glCircleRadiusX != 0) || (glCircleRadiusY != 0))
		{
			if ((glCircleMode == SHAPE_MODE_FILLSTROKE) || (glCircleMode == SHAPE_MODE_FILL))
			{
				DrawCircle();
			}
			if ((glCircleStroke > 0) && ((glCircleMode == SHAPE_MODE_FILLSTROKE) || (glCircleMode == SHAPE_MODE_STROKE)))
			{	
				DrawCircleStroke();
			}
		}
		
		g->stp.SetState(ST_ZWRITE, true);
		g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	}

	g->stp.SetState(ST_ZTEST, false);
	GLFUNC(glDisable)(GL_TEXTURE_2D);	
	if ((glCircleRadiusX != 0)||(glCircleRadiusY != 0))
	{
		if ((glCircleMode == SHAPE_MODE_FILLSTROKE) || (glCircleMode == SHAPE_MODE_FILL))
		{
			g->stp.SetColor(glColorR, glColorG, glColorB, glCircleOpacity);
			DrawCircle();
		}

		if ((glCircleStroke > 0) && ((glCircleMode == SHAPE_MODE_FILLSTROKE) || (glCircleMode == SHAPE_MODE_STROKE)))
		{	
			g->stp.SetColor(glBackColorR, glBackColorG, glBackColorB, glCircleOpacity);
			DrawCircleStroke();
		}
		g->stp.SetColor(255, 255, 255);		
	}

	g->stp.SetState(ST_ZTEST, true);
	GLFUNC(glEnable)(GL_TEXTURE_2D);

	if (glCircleOpacity != 255)
	{
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		g->stp.SetState(ST_BLEND, false);
	}
}

void DrawCircleShapeAndClearZBuffer()
{
	ClearZBuffer();
	DrawCircleShape();	
}

CCircleTool::CCircleTool():
	m_mode(SHAPE_MODE_STROKE),
	m_opacity(255),
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

void CCircleTool::SetDrawingTarget(IDrawingTarget* drawingTarget)
{
	m_drawingTarget = drawingTarget;
}

void CCircleTool::SetParameter(int paramID, void* param)
{
	switch (paramID)
	{
	case CIRCLE_COLOR:
		{
			unsigned int tmp = (unsigned int)param;
			m_color = *(CColor4 *)&tmp;
		}
		break;

	case CIRCLE_BACK_COLOR:
		{
			unsigned int tmp = (unsigned int)param;
			m_backColor = *(CColor4 *)&tmp;
		}
		break;

	case CIRCLE_STROKE_WIDTH:
		m_strokeWidth = (unsigned char)param;
		break;

	case CIRCLE_OPACITY:
		m_opacity = (unsigned char)param;
		break;

	case CIRCLE_MODE:
		m_mode = (unsigned char)param;
		break;
	}
}

void CCircleTool::Draw()
{
	glCircleCenterX = (fabs((float)(m_endX - m_startX)) / 2.0f);
	if (m_startX < m_endX)
	{	
		glCircleCenterX += m_startX;
	}
	else
	{
		glCircleCenterX += m_endX;
	}

	glCircleCenterY = (fabs((float)(m_endY - m_startY)) / 2.0f);
	if (m_startY < m_endY)
	{
		glCircleCenterY += m_startY;
	} 
	else
	{
		glCircleCenterY += m_endY;
	}

	glCircleCenterX = m_drawingTarget->GetGLX((int)glCircleCenterX);
	glCircleCenterY = m_drawingTarget->GetGLY((int)glCircleCenterY);

	glCircleRadiusX = fabs(m_drawingTarget->GetGLX(m_startX) - m_drawingTarget->GetGLX(m_endX)) / 2.0f;
	glCircleRadiusY = fabs(m_drawingTarget->GetGLY(m_startY) - m_drawingTarget->GetGLY(m_endY)) / 2.0f;

	glCircleStroke = fabs(m_drawingTarget->GetGLY(0) - m_drawingTarget->GetGLY(m_strokeWidth));
	
	glCircleOpacity = m_opacity;		

	if (m_drawingTarget->GetType() == RENDER_TARGET_MAINBUFFER)
	{
		glCircleOpacity = (glCircleOpacity == 255) ? 254 : glCircleOpacity;
	}

	glCircleMode = m_mode;

	glColorR = m_color.r;
	glColorG = m_color.g;
	glColorB = m_color.b;

	glBackColorR = m_backColor.r;
	glBackColorG = m_backColor.g;
	glBackColorB = m_backColor.b;

#ifndef TOOL_OPTIMIZATION
	m_drawingTarget->ApplyGeometry(ClearZBuffer);
	m_drawingTarget->ApplyGeometry(DrawCircleShape);
#else	
	m_drawingTarget->ApplyGeometry(DrawCircleShapeAndClearZBuffer);
#endif
}

void CCircleTool::OnMouseMove(int x, int y)
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

void CCircleTool::OnLMouseDown(int x, int y)
{
	m_isWorking = true;

	m_startX = x;
	m_startY = y;
	m_endX = m_startX;
	m_endY = m_startY;

	m_drawingTarget->SaveBackBuffer();
}

void CCircleTool::OnLMouseUp(int x, int y)
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

void CCircleTool::OnRMouseDown(int /*x*/, int /*y*/)
{
	if (m_startX != -1000)
	{
		m_isWorking = true;
		m_isMoveMode = true;
	}
}

void CCircleTool::OnRMouseUp(int x, int y)
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

void CCircleTool::OnChar(int /*keyCode*/, int scancode)
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

void CCircleTool::RedoAction(void* /*data*/)
{
}

void CCircleTool::OnSelected()
{
}

void CCircleTool::OnDeselected()
{
	m_startX = -1000;
}

void CCircleTool::OnEndRestoreState()
{
}

int CCircleTool::GetSynchRequiredType()
{
	return SYNCH_KEYS;
}

void CCircleTool::Destroy()
{
	MP_DELETE_THIS;
}

void CCircleTool::MovingMiddle(int x, int y)
{
	int halfWidth = abs(m_startX - m_endX)/2;
	int halfHeight = abs(m_startY - m_endY)/2;

	m_startX = x - halfWidth;
	m_endX =  x + halfWidth;

	m_startY = y - halfHeight;
	m_endY = y + halfHeight;
}

CCircleTool::~CCircleTool()
{
}

#include "StdAfx.h"
#include "PenTool.h"
#include "GlobalVars.h"
#include "GlobalParams.h"
#include "ToolsParameters.h"
#include "CommonToolRoutines.h"
#include "GlobalSingletonStorage.h"

void PenDraw()
{
	if (glPenAsLine)
	{
		g->stp.PrepareForRender();
		GLFUNC(glBegin)(GL_LINES);
			GLFUNC(glVertex2f)(glLastMouseX, glLastMouseY);
			GLFUNC(glVertex2f)(glCurrentMouseX,glCurrentMouseY);						
		GLFUNC(glEnd)();
	}
	else
	{
		float x1, x2 ,y1, y2;

		g->stp.PrepareForRender();		

		if (glPenDisplayListID != 0xFFFFFFFF)
		{
			for (float koef = 0.0f; koef <= 1.0f; koef += glPencilDelta)
			{
				float centerX = glCurrentMouseX * koef + glLastMouseX * (1.0f - koef);
				float centerY = glCurrentMouseY * koef + glLastMouseY * (1.0f - koef);
	
				g->stp.PushMatrix();
				GLFUNC(glTranslatef)(centerX, centerY, 0.0f);
				GLFUNC(glScalef)(glPencilRadius, glPencilRadius * 1.5f, glPencilRadius);
				GLFUNC(glCallList)(glPenDisplayListID);
				g->stp.PopMatrix();
			}
		}
		else
		{
			GLFUNC(glBegin)(GL_TRIANGLES);
			for (float koef = 0.0f; koef <= 1.0f; koef += glPencilDelta)
			{
				float centerX = glCurrentMouseX * koef + glLastMouseX * (1.0f - koef);
				float centerY = glCurrentMouseY * koef + glLastMouseY * (1.0f - koef);

				for (int i = 0; i < SIN_COS_COUNT; i++)
				{
					x1 = centerX + cosTable[i] * glPencilRadius;
					y1 = centerY + sinTable[i] * glPencilRadius * 1.5f;
					x2 = centerX + cosTable[i + 1] * glPencilRadius;
					y2 = centerY + sinTable[i + 1] * glPencilRadius * 1.5f;
					
					GLFUNC(glVertex3f)(centerX, centerY, 0);
					GLFUNC(glVertex3f)(x1, y1, 0); 
					GLFUNC(glVertex3f)(x2, y2, 0);
				}
			}
			GLFUNC(glEnd)();	
		}
	}
}

void PenToolDraw()
{	
	if (glPencilOpacity != 255)
	{
		g->stp.SetState(ST_BLEND, true);
		g->stp.SetState(ST_ZTEST, true);
		GLFUNC(glDepthFunc)(GL_LESS);
		g->stp.SetBlendFunc(GL_ONE, GL_ONE);

		if (glPenAsLine)
		{
			g->stp.SetState(ST_ZWRITE, false);
		}
		g->stp.SetColor(0, 0, 0, glPencilOpacity);
		GLFUNC(glDisable)(GL_TEXTURE_2D);

		PenDraw();
		
		g->stp.SetState(ST_ZWRITE, true);		
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	}

	g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	g->stp.SetState(ST_ZTEST, false);
	GLFUNC(glDepthFunc)(GL_LESS);
	GLFUNC(glDisable)(GL_TEXTURE_2D);

	if (!glPenAsLine)
	{
		GLFUNC(glEnable)(GL_POLYGON_OFFSET_FILL);
		GLFUNC(glPolygonOffset)(1.0, -12);
	}
	
	g->stp.SetColor(glColorR, glColorG, glColorB, glPencilOpacity);
	PenDraw();

	GLFUNC(glDepthFunc)(GL_LEQUAL);
	g->stp.SetState(ST_ZTEST, true);
	GLFUNC(glEnable)(GL_TEXTURE_2D);

	g->stp.SetColor(255, 255, 255); 

	if (!glPenAsLine)
	{
		GLFUNC(glPolygonOffset)(1.0, 0);
		GLFUNC(glDisable)(GL_POLYGON_OFFSET_FILL);
	}
	
	if (glPencilOpacity != 255)
	{
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		g->stp.SetState(ST_BLEND, false);
	}

}

CPenTool::CPenTool():
	m_penWidth(10),
	m_lastMouseX(0), 
	m_lastMouseY(0), 
	m_penOpacity(64),
	m_isWorking(false),
	m_drawingTarget(NULL),
	m_penColor(0, 0, 0, 255)
{
	
}

void CPenTool::SetDrawingTarget(IDrawingTarget* drawingTarget)
{
	m_drawingTarget = drawingTarget;
}

void CPenTool::SetParameter(int paramID, void* param)
{

	switch (paramID)
	{
	case PEN_COLOR:
		{
			unsigned int tmp = (unsigned int)param;
			m_penColor = *(CColor4 *)&tmp;
		}
		break;

	case PEN_WIDTH:
		m_penWidth = (unsigned char)param;
		break;

	case PEN_OPACITY:
		m_penOpacity = (unsigned char)param;
		break;
	}
}

void CPenTool::Draw(int x, int y, bool isDrawAnyway)
{
#ifdef TOOL_OPTIMIZATION
	if (!isDrawAnyway)
	if ((abs(m_lastMouseX - x) < 4) && (abs(m_lastMouseY - y) < 4))
	{
		return;
	}
#endif

	glLastMouseX = m_drawingTarget->GetGLX(m_lastMouseX);
	glLastMouseY = m_drawingTarget->GetGLY(m_lastMouseY);
	glCurrentMouseX = m_drawingTarget->GetGLX(x);
	glCurrentMouseY = m_drawingTarget->GetGLY(y);

	glPenAsLine = (m_penWidth == 1);
	glPencilRadius = fabs(m_drawingTarget->GetGLX(0) - m_drawingTarget->GetGLX(m_penWidth)) / 2.0f;
	glPencilOpacity = m_penOpacity;

	glColorR = m_penColor.r;
	glColorG = m_penColor.g;
	glColorB = m_penColor.b;

	if (abs(x - m_lastMouseX) > abs((y - m_lastMouseY)))
	{
		glPencilDelta = 1.0f / (fabs((float)(x - m_lastMouseX)));
	}
	else
	{
		glPencilDelta = 1.0f / (fabs((float)(y - m_lastMouseY)));
	}

#ifdef TOOL_OPTIMIZATION
	if (m_penWidth < 7)
	{
		glPencilDelta *= 2.0f;
	}
	else
	{
		glPencilDelta *= 4.0f;
	}
#endif

	if ((g_isPlaying) && (m_drawingTarget->IsOk()) && (m_drawingTarget->IsBindedToApplyGeometry()))
	{
		PenToolDraw();
	}
	else
	{
		m_drawingTarget->ApplyGeometry(PenToolDraw);
	}

	m_lastMouseX = x;
	m_lastMouseY = y;
}

void CPenTool::OnMouseMove(int x, int y)
{
	if (m_isWorking)
	{	
		Draw(x, y);
	}
}

void CPenTool::OnLMouseDown(int x, int y)
{
	m_isWorking = true;

	if ((g_isPlaying) && (m_drawingTarget->IsOk()))
	{
		m_drawingTarget->BindToApplyGeometry();
		ClearZBuffer();
	}
	else
	{
		m_drawingTarget->ApplyGeometry(ClearZBuffer);
	}

	m_lastMouseX = x - 1;
	m_lastMouseY = y - 1;

	Draw(x, y, true);

	m_lastMouseX = x;
	m_lastMouseY = y;
}

void CPenTool::OnLMouseUp(int x, int y)
{
	Draw(x, y);

	if ((g_isPlaying) && (m_drawingTarget->IsOk()))
	{
		m_drawingTarget->UnbindToApplyGeometry();
	}

	m_isWorking = false;
}

void CPenTool::OnRMouseDown(int /*x*/, int /*y*/)
{
}

void CPenTool::OnRMouseUp(int /*x*/, int /*y*/)
{
}

void CPenTool::OnChar(int /*keycode*/, int /*scancode*/)
{
}

void CPenTool::RedoAction(void* /*data*/)
{
}

void CPenTool::OnSelected()
{
}

void CPenTool::OnDeselected()
{
	/*if ((g_isPlaying) && (m_drawingTarget->IsOk()))
	{
		m_drawingTarget->UnbindToApplyGeometry();
	}*/
}

void CPenTool::OnEndRestoreState()
{
}

int CPenTool::GetSynchRequiredType()
{
	return SYNCH_MOVE_WHEN_LDOWN;
}

void CPenTool::Destroy()
{
	MP_DELETE_THIS;
}

CPenTool::~CPenTool()
{
}

#include "StdAfx.h"
#include "EraserTool.h"
#include "GlobalVars.h"
#include "GlobalParams.h"
#include "HelperFunctions.h"
#include "ToolsParameters.h"
#include "CommonToolRoutines.h"
#include "GlobalSingletonStorage.h"

GLubyte glEraserOpacity;
float glEraserDiameter;
float glEraserDelta;
float glEraserRadius;

void EraserDraw()
{
	g->stp.SetColor(0, 0, 0, glEraserOpacity);

	g->stp.SetState(ST_BLEND, true);
	g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
	GLFUNC(glBlendEquationEXT)(GL_FUNC_REVERSE_SUBTRACT);

	if (glEraserRadius == 1)
	{
		GLFUNC(glDisable)(GL_TEXTURE_2D);
		g->stp.PrepareForRender();
		GLFUNC(glBegin)(GL_LINES);
			GLFUNC(glVertex2f)(glLastMouseX, glLastMouseY);
			GLFUNC(glVertex2f)(glCurrentMouseX, glCurrentMouseY);						
		GLFUNC(glEnd)();
		g->stp.SetColor(255, 255, 255, 255);
		GLFUNC(glEnable)(GL_TEXTURE_2D);
	}
	else
	{
		float x1, x2 ,y1, y2;
		g->stp.SetState(ST_ZTEST, true);
		GLFUNC(glDepthFunc)(GL_LESS);
		GLFUNC(glPolygonOffset)(1.0, -12);
		GLFUNC(glEnable)(GL_POLYGON_OFFSET_FILL);
		GLFUNC(glDisable)(GL_TEXTURE_2D);

		g->stp.PrepareForRender();

		if (glEraserDisplayListID != 0xFFFFFFFF)
		{
			for (float koef = 0.0f; koef <= 1.0f; koef += glEraserDelta)
			{
				float centerX = glCurrentMouseX * koef + glLastMouseX * (1.0f - koef);
				float centerY = glCurrentMouseY * koef + glLastMouseY * (1.0f - koef);
	
				g->stp.PushMatrix();
				GLFUNC(glTranslatef)(centerX, centerY, 0.0f);
				GLFUNC(glScalef)(glEraserRadius, glEraserRadius * 1.5f, glEraserRadius);
				GLFUNC(glCallList)(glEraserDisplayListID);
				g->stp.PopMatrix();
			}
		}
		else
		{
			GLFUNC(glBegin)(GL_TRIANGLES);

			for (float koef = 0.0f; koef <= 1.0f; koef += glEraserDelta)
			{
				float centerX = glCurrentMouseX*koef + glLastMouseX*(1 - koef);
				float centerY = glCurrentMouseY*koef + glLastMouseY*(1 - koef);

				for (int i = 0; i < SIN_COS_COUNT_ERASER; i++)
				{
					x1 = centerX + cosTable[i * 2] * glEraserRadius;
					y1 = centerY + sinTable[i * 2] * glEraserRadius * 1.5f;
					x2 = centerX + cosTable[i * 2 + 2] * glEraserRadius;
					y2 = centerY + sinTable[i * 2 + 2] * glEraserRadius * 1.5f;

					GLFUNC(glVertex3f)(centerX, centerY, 0);
					GLFUNC(glVertex3f)(x1, y1, 0); 
					GLFUNC(glVertex3f)(x2, y2, 0);
				}
			}

			GLFUNC(glEnd)();
		}

		GLFUNC(glDepthFunc)(GL_LEQUAL);
		GLFUNC(glDisable)(GL_POLYGON_OFFSET_FILL);

		GLFUNC(glEnable)(GL_TEXTURE_2D);
		g->stp.SetColor(255, 255, 255);
	}

	GLFUNC(glBlendEquationEXT)(GL_FUNC_ADD);
	g->stp.SetState(ST_BLEND, false);
}

CEraserTool::CEraserTool():
	m_lastMouseX(0), 
	m_lastMouseY(0), 
	m_eraserWidth(10),
	m_isWorking(false),
	m_eraserOpacity(64),
	m_drawingTarget(NULL)
{
	
}

void CEraserTool::SetDrawingTarget(IDrawingTarget* drawingTarget)
{
	m_drawingTarget = drawingTarget;
}

void CEraserTool::SetParameter(int paramID, void* param)
{
	switch (paramID)
	{
	case ERASER_WIDTH:
		m_eraserWidth = (unsigned char)param;
		break;

	case ERASER_OPACITY:
		m_eraserOpacity = (unsigned char)param;
		break;
	}
}

void CEraserTool::Draw(int x, int y, bool isDrawAnyway)
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

	glEraserRadius = fabs(m_drawingTarget->GetGLX(0) - m_drawingTarget->GetGLX(m_eraserWidth)) / 2.0f;
	glEraserOpacity = m_eraserOpacity;

	if (abs(x - m_lastMouseX) > abs(y - m_lastMouseY))
	{
		glEraserDelta = 1.0f / (fabs((float)(x - m_lastMouseX)));
	}
	else
	{
		glEraserDelta = 1.0f / (fabs((float)(y - m_lastMouseY)));
	}

#ifdef TOOL_OPTIMIZATION
	if (m_eraserWidth < 7)
	{
		glEraserDelta *= 2.0f;
	}
	else
	{
		glEraserDelta *= 4.0f;
	}
#endif
	
	if ((g_isPlaying) && (m_drawingTarget->IsOk()) && (m_drawingTarget->IsBindedToApplyGeometry()))
	{
		EraserDraw();
	}
	else
	{
		m_drawingTarget->ApplyGeometry(EraserDraw);
	}
		
	m_lastMouseX = x;
	m_lastMouseY = y;
}

void CEraserTool::OnMouseMove(int x, int y)
{
	if (m_isWorking)
	{	
		Draw(x, y);
	}
}

void CEraserTool::OnLMouseDown(int x, int y)
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

void CEraserTool::OnLMouseUp(int x, int y)
{
	if (m_isWorking)
	{		
		Draw(x, y);
	}

	if ((g_isPlaying) && (m_drawingTarget->IsOk()))
	{
		m_drawingTarget->UnbindToApplyGeometry();
	}

	m_isWorking = false;
}

void CEraserTool::OnRMouseDown(int /*x*/, int /*y*/)
{
}

void CEraserTool::OnRMouseUp(int /*x*/, int /*y*/)
{
}

void CEraserTool::OnChar(int /*keycode*/, int /*scancode*/)
{
}

void CEraserTool::RedoAction(void* /*data*/)
{
}

void CEraserTool::OnSelected()
{
}

void CEraserTool::OnDeselected()
{
	if ((g_isPlaying) && (m_drawingTarget->IsOk()))
	{
		m_drawingTarget->UnbindToApplyGeometry();
	}
}

void CEraserTool::OnEndRestoreState()
{
}

int CEraserTool::GetSynchRequiredType()
{
	return SYNCH_MOVE_WHEN_LDOWN;
}

void CEraserTool::Destroy()
{
	MP_DELETE_THIS;
}

CEraserTool::~CEraserTool()
{
}
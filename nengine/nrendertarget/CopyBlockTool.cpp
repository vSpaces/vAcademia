
#include "StdAfx.h"
#include <Math.h>
#include <Assert.h>
#include "Texture.h"
#include "GlobalVars.h"
#include "GlobalParams.h"
#include "CopyBlockTool.h"
#include "ToolsParameters.h"
#include "HelperFunctions.h"
#include "CommonToolRoutines.h"
#include "GlobalSingletonStorage.h"
#include "RectangleCommonRoutines.h"

int glStartX, glStartY;
int glRectX, glRectY;
int glRectWidth, glRectHeight;
int glTextureID;

void MovingBlockDraw()
{
	g->tm.RefreshTextureOptimization();

	g->stp.SetState(ST_ZTEST, false);
	g->stp.SetState(ST_CULL_FACE, false);
	g->stp.PushMatrix();
	GLFUNC(glScalef)(1, -1, 1);
#pragma warning(push)
#pragma warning(disable : 4239)		
	g->sp.PutNormalSprite(glStartX, glStartY, glRectX, glRectY, glRectX + glRectWidth, glRectY + glRectHeight, glTextureID, CColor3(255, 255, 255));
#pragma warning(pop)
	g->stp.PopMatrix();
	g->stp.SetState(ST_CULL_FACE, true);
	g->stp.SetState(ST_ZTEST, true);
}

CCopyBlockTool::CCopyBlockTool():
	m_x(0),
	m_y(0),
	m_blockX(0),
	m_blockY(0),
	m_blockWidth(0),
	m_blockHeight(0),
	m_tempTextureID(-1),
	m_color(255, 0, 0, 128),
	m_workMode(WORK_MODE_FREE)
{
}

void CCopyBlockTool::OnStartMoving()
{
	m_workMode = WORK_MODE_MOVING;

	if (-1 == m_tempTextureID)
	{
		m_tempTextureID = g->tm.AddObject("temp_texture_for_copy_block_" + IntToStr((int)this));

		CTexture* texture = g->tm.GetObjectPointer(m_tempTextureID);
		texture->SetTextureWidth(m_drawingTarget->GetScaledWidth());
		texture->SetTextureHeight(m_drawingTarget->GetScaledHeight());

		unsigned int textureID = m_drawingTarget->CreateTempTexture();
		texture->SetNumber(textureID);
		texture->SetLoadedStatus(true);
		texture->SetYInvertStatus(false);
	}

	CTexture* texture = g->tm.GetObjectPointer(m_tempTextureID);
	m_drawingTarget->RestoreBackBuffer();
	m_drawingTarget->SaveBackBuffer(texture->GetNumber());

	m_x = m_blockWidth/2;
	m_y = m_blockHeight/2;

	DrawMoving();
}

void CCopyBlockTool::OnEndMoving()
{
	if (-1 != m_tempTextureID)
	{
		g->tm.DeleteObject(m_tempTextureID);
		m_tempTextureID = -1;
	}

	m_workMode = WORK_MODE_FREE;
}

void CCopyBlockTool::DrawMoving()
{
	glStartX = (m_x - m_blockWidth/2) / m_drawingTarget->GetInvScale();
	glStartY = (m_y - m_blockHeight/2) / m_drawingTarget->GetInvScale();
	
	glRectX = m_blockX ;
	glRectY = m_blockY ;
	glRectWidth = m_blockWidth;
	glRectHeight = m_blockHeight;
	glTextureID = m_tempTextureID;

	m_drawingTarget->RestoreBackBuffer();
	m_drawingTarget->ApplyGeometry(MovingBlockDraw);
}

void CCopyBlockTool::DrawSelecting()
{
	glRectX1 = (float)(m_blockX);
	glRectX2 = (float)(m_blockX2);
	glRectY1 = (float)(m_blockY);
	glRectY2 = (float)(m_blockY2);

	if ((m_blockX < m_blockX2) && (m_blockY > m_blockY2))
	{
		glRectY1 = (float)(m_blockY2);
		glRectY2 = (float)(m_blockY);
	}
	else if (m_blockX > m_blockX2)
	{
		if (m_blockY > m_blockY2)
		{
			glRectX1 = (float)(m_blockX2);
			glRectX2 = (float)(m_blockX);
			glRectY1 = (float)(m_blockY2);
			glRectY2 = (float)(m_blockY);
		}
		else
		{
			glRectX1 = (float)(m_blockX);
			glRectX2 = (float)(m_blockX2);
		}
	}

	glRectX1 = m_drawingTarget->GetGLX((int)glRectX1);
	glRectX2 = m_drawingTarget->GetGLX((int)glRectX2);

	glRectY1 = m_drawingTarget->GetGLY((int)glRectY1);
	glRectY2 = m_drawingTarget->GetGLY((int)glRectY2);

	glRectStroke = 0;

	glRectOpacity = m_color.a;
	glRectMode = SHAPE_MODE_FILL;

	glColorR = m_color.r;
	glColorG = m_color.g;
	glColorB = m_color.b;

	glBackColorR = m_color.r;
	glBackColorG = m_color.g;
	glBackColorB = m_color.b;

	m_drawingTarget->RestoreBackBuffer();
	m_drawingTarget->ApplyGeometry(ClearZBuffer);
	m_drawingTarget->ApplyGeometry(DrawSquareShape);
}

void CCopyBlockTool::SetDrawingTarget(IDrawingTarget* drawingTarget)
{
	m_drawingTarget = drawingTarget;
}

void CCopyBlockTool::SetParameter(int paramID, void* param)
{
	switch (paramID)
	{
	case SELECTION_COLOR:
		{
			unsigned int tmp = (unsigned int)param;
			m_color = *(CColor4 *)&tmp;
		}
		break;
	}
}

void CCopyBlockTool::OnMouseMove(int x, int y)
{
	if (WORK_MODE_MOVING == m_workMode)
	{
		m_x = x;
		m_y = y;

		DrawMoving();
	}
	else if (WORK_MODE_SELECTING == m_workMode)
	{
		m_blockX2 = x;
		m_blockY2 = y;

		DrawSelecting();
	}
}

void CCopyBlockTool::OnLMouseDown(int x, int y)
{
	if (WORK_MODE_FREE == m_workMode)
	{
		m_workMode = WORK_MODE_SELECTING;

		m_drawingTarget->SaveBackBuffer();

		m_blockX = x;
		m_blockY = y;
	}
	else if (WORK_MODE_MOVING == m_workMode)
	{
	}
}

void CCopyBlockTool::OnLMouseUp(int x, int y)
{
	if (WORK_MODE_SELECTING == m_workMode)
	{
		m_blockWidth = x - m_blockX;
		m_blockHeight = y - m_blockY;

		if (m_blockWidth < 0)
		{
			m_blockWidth = m_blockX - x;
			m_blockX -= m_blockWidth;
		}

		if (m_blockHeight < 0)
		{
			m_blockHeight = m_blockY - y;
			m_blockY -= m_blockHeight;
		}

		m_blockX /= m_drawingTarget->GetInvScale();
		m_blockY /= m_drawingTarget->GetInvScale();
		
		m_blockWidth /= m_drawingTarget->GetInvScale();
		m_blockHeight /= m_drawingTarget->GetInvScale();

		OnStartMoving();
	}
	else if (WORK_MODE_MOVING == m_workMode)
	{
		m_x = x;
		m_y = y;

		DrawMoving();
	}
}

void CCopyBlockTool::OnRMouseDown(int /*x*/, int /*y*/)
{
}

void CCopyBlockTool::OnRMouseUp(int /*x*/, int /*y*/)
{
}

void CCopyBlockTool::OnChar(int keyCode, int scancode)
{
	if (13 == keyCode)
	{
		m_workMode = WORK_MODE_FREE;
	}
	else if (37 == scancode)
	{
		m_x--; 
		DrawMoving();
	}
	else if (38 == scancode)
	{
		m_y--; 
		DrawMoving();
	}
	else if (39 == scancode)
	{
		m_x++; 
		DrawMoving();
	}
	else if (40 == scancode)
	{
		m_y++; 
		DrawMoving();
	}
}

void CCopyBlockTool::OnSelected()
{
	m_workMode = WORK_MODE_FREE;
}

void CCopyBlockTool::OnDeselected()
{
	OnEndMoving();
}

void CCopyBlockTool::OnEndRestoreState()
{	
}

void CCopyBlockTool::RedoAction(void* /*data*/)
{
}

int CCopyBlockTool::GetSynchRequiredType()
{
	return SYNCH_KEYS;
}

void CCopyBlockTool::Destroy()
{
	MP_DELETE_THIS;
}

CCopyBlockTool::~CCopyBlockTool()
{
	if (-1 != m_tempTextureID)
	{
		g->tm.DeleteObject(m_tempTextureID);
		m_tempTextureID = -1;
	}
}
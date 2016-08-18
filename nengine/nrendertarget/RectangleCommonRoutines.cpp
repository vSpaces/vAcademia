
#include "StdAfx.h"
#include "GlobalVars.h"
#include "GlobalParams.h"
#include "CommonToolRoutines.h"
#include "RectangleCommonRoutines.h"
#include "GlobalSingletonStorage.h"

float glRectX1, glRectX2, glRectX0, glRectX3;
float glRectY1, glRectY2, glRectY0, glRectY3;
GLubyte glRectOpacity;
int glRectMode;
float glRectStroke;

void DrawSquare()
{
	g->stp.SetState(ST_CULL_FACE, false);
	g->stp.PrepareForRender();
	GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glVertex2f)(glRectX1, glRectY1);
		GLFUNC(glVertex2f)(glRectX1, glRectY2);
		GLFUNC(glVertex2f)(glRectX2, glRectY2);
		GLFUNC(glVertex2f)(glRectX2, glRectY1);
	GLFUNC(glEnd)();
	g->stp.SetState(ST_CULL_FACE, true);
}

void DrawSquareStroke()
{
	g->stp.SetState(ST_CULL_FACE, false);
	g->stp.PrepareForRender();
	GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glVertex2f)(glRectX1, glRectY0);
		GLFUNC(glVertex2f)(glRectX1, glRectY1);
		GLFUNC(glVertex2f)(glRectX1, glRectY1);
		GLFUNC(glVertex2f)(glRectX2, glRectY1);
		GLFUNC(glVertex2f)(glRectX2, glRectY1);
		GLFUNC(glVertex2f)(glRectX2, glRectY0);
		GLFUNC(glVertex2f)(glRectX2, glRectY0);
		GLFUNC(glVertex2f)(glRectX1, glRectY0);

		GLFUNC(glVertex2f)(glRectX1, glRectY3);
		GLFUNC(glVertex2f)(glRectX1, glRectY2);
		GLFUNC(glVertex2f)(glRectX1, glRectY2);
		GLFUNC(glVertex2f)(glRectX2, glRectY2);
		GLFUNC(glVertex2f)(glRectX2, glRectY2);
		GLFUNC(glVertex2f)(glRectX2, glRectY3);
		GLFUNC(glVertex2f)(glRectX2, glRectY3);
		GLFUNC(glVertex2f)(glRectX1, glRectY3);

		GLFUNC(glVertex2f)(glRectX0, glRectY0);
		GLFUNC(glVertex2f)(glRectX0, glRectY3);
		GLFUNC(glVertex2f)(glRectX0, glRectY3);
		GLFUNC(glVertex2f)(glRectX1, glRectY3);
		GLFUNC(glVertex2f)(glRectX1, glRectY3);
		GLFUNC(glVertex2f)(glRectX1, glRectY0);
		GLFUNC(glVertex2f)(glRectX1, glRectY0);
		GLFUNC(glVertex2f)(glRectX0, glRectY0);

		GLFUNC(glVertex2f)(glRectX2, glRectY0);
		GLFUNC(glVertex2f)(glRectX2, glRectY3);
		GLFUNC(glVertex2f)(glRectX2, glRectY3);
		GLFUNC(glVertex2f)(glRectX3, glRectY3);
		GLFUNC(glVertex2f)(glRectX3, glRectY3);
		GLFUNC(glVertex2f)(glRectX3, glRectY0);
		GLFUNC(glVertex2f)(glRectX3, glRectY0);
		GLFUNC(glVertex2f)(glRectX2, glRectY0);
	GLFUNC(glEnd)();
	g->stp.SetState(ST_CULL_FACE, true);
}

void DrawSquareShape()
{
	if (glRectOpacity != 255)
	{
		g->stp.SetState(ST_BLEND, true);
		g->stp.SetState(ST_ZTEST, true);
		GLFUNC(glDepthFunc)(GL_LESS);
		g->stp.SetBlendFunc(GL_ONE, GL_ONE);
		g->stp.SetColor(0, 0, 0, glRectOpacity);
		GLFUNC(glDisable)(GL_TEXTURE_2D);

		if ((glRectX1 != glRectX2)||(glRectY1 != glRectY2))
		{
			if ((glRectMode == SHAPE_MODE_FILLSTROKE) || (glRectMode == SHAPE_MODE_FILL))
			{
				DrawSquare();		
			}
			if ((glRectMode == SHAPE_MODE_FILLSTROKE) || (glRectMode == SHAPE_MODE_STROKE))
			{	
				DrawSquareStroke();			
			}
		}

		g->stp.SetState(ST_ZWRITE, true);
		g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
	}

	g->stp.SetState(ST_ZTEST, false);
	GLFUNC(glDisable)(GL_TEXTURE_2D);
	
	if ((glRectX1 != glRectX2)||(glRectY1 != glRectY2))
	{
		if ((glRectMode == SHAPE_MODE_FILLSTROKE) || (glRectMode == SHAPE_MODE_FILL))
		{
			g->stp.SetColor(glColorR, glColorG, glColorB, glRectOpacity);		
			DrawSquare();
		}
		
		if ((glRectStroke > 0) && ((glRectMode == SHAPE_MODE_FILLSTROKE) || (glRectMode == SHAPE_MODE_STROKE)))
		{	
			g->stp.SetColor(glBackColorR, glBackColorG, glBackColorB, glRectOpacity);		
			DrawSquareStroke();			
		}

		g->stp.SetColor(255,255,255);		
	}

	g->stp.SetState(ST_ZTEST, true);
	GLFUNC(glEnable)(GL_TEXTURE_2D);
	
	if (glRectOpacity != 255)
	{
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		g->stp.SetState(ST_BLEND, false);
	}
}

void DrawSquareShapeAndClearZBuffer()
{
	ClearZBuffer();
	DrawSquareShape();
}
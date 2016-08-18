
#include "StdAfx.h"
#include "GlobalSingletonStorage.h"
#include "AuxilaryProcessor.h"
#include "TextureManager.h"
#include "CameraManager.h"
#include "NEngine.h"
#include "cal3d/memory_leak.h"

CAuxilaryProcessor::CAuxilaryProcessor():
	MP_STRING_INIT(m_edgesSrcFile)
{
}

void CAuxilaryProcessor::SetEdgesSrcFile(std::string path)
{
	m_edgesSrcFile = path;
}

void CAuxilaryProcessor::DrawEdges()
{
	g->stp.PopMatrix();
	g->stp.PushMatrix();

	g->stp.SetState(ST_ZTEST, false);

	g->stp.SetState(ST_ZWRITE, GL_FALSE);

	g->stp.SetState(ST_BLEND, true);
	g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	g->stp.SetColor(255, 255, 255, 255);

	g->cm.SetCamera2d();
		
	int fl2 = g->tm.GetObjectNumber(m_edgesSrcFile);
	g->tm.BindTexture(fl2);

	g->stp.PrepareForRender();

	GLFUNC(glBegin)(GL_QUADS);

	GLFUNC(glTexCoord2f)(0.0f, 0.0f);
	GLFUNC(glVertex2f)(-1.0f, -1.0f);
	GLFUNC(glTexCoord2f)(0.0f, 0.3f);
	GLFUNC(glVertex2f)(-1.0f, -0.4f);
	GLFUNC(glTexCoord2f)(1.0f, 0.3f);
	GLFUNC(glVertex2f)(1.0f, -0.4f);
	GLFUNC(glTexCoord2f)(1.0f, 0.0f);
	GLFUNC(glVertex2f)(1.0f, -1.0f);

	GLFUNC(glTexCoord2f)(0.0f, 0.7f);
	GLFUNC(glVertex2f)(-1.0f, 0.4f);
	GLFUNC(glTexCoord2f)(0.0f, 1.0f);
	GLFUNC(glVertex2f)(-1.0f, 1.0f);
	GLFUNC(glTexCoord2f)(1.0f, 1.0f);
	GLFUNC(glVertex2f)(1.0f, 1.0f);
	GLFUNC(glTexCoord2f)(1.0f, 0.7f);
	GLFUNC(glVertex2f)(1.0f, 0.4f);

	GLFUNC(glTexCoord2f)(0.0f, 0.3f);
	GLFUNC(glVertex2f)(-1.0f, -0.4f);
	GLFUNC(glTexCoord2f)(0, 0.7f);
	GLFUNC(glVertex2f)(-1.0f, 0.4f);
	GLFUNC(glTexCoord2f)(0.1f, 0.7f);
	GLFUNC(glVertex2f)(-0.8f, 0.4f);
	GLFUNC(glTexCoord2f)(0.1f, 0.3f);
	GLFUNC(glVertex2f)(-0.8f,-0.4f);

	GLFUNC(glTexCoord2f)(0.9f, 0.3f);
	GLFUNC(glVertex2f)(0.8f, -0.4f);
	GLFUNC(glTexCoord2f)(0.9f, 0.7f);
	GLFUNC(glVertex2f)(0.8f, 0.4f);
	GLFUNC(glTexCoord2f)(1.0f, 0.7f);
	GLFUNC(glVertex2f)(1.0f, 0.4f);
	GLFUNC(glTexCoord2f)(1.0f, 0.3f);
	GLFUNC(glVertex2f)(1.0f, -0.4f);

	GLFUNC(glEnd)();

	g->stp.SetBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
	g->stp.SetState(ST_BLEND, false);

	g->stp.SetState(ST_ZWRITE, GL_TRUE);
	g->stp.SetState(ST_ZTEST, true);
}

void CAuxilaryProcessor::AskEndWork(unsigned int maxTime)
{
	if (g->ne.time > maxTime)
	{
		PostMessage(g->ne.ghWnd, WM_CLOSE, 0,0);
	}
}

void CAuxilaryProcessor::FadeIn(int localTime, int allTime)
{
	if ((localTime >= 0) && (localTime <= allTime))
	{
		g->stp.PopMatrix();
		g->stp.PushMatrix();

		g->stp.SetState(ST_ZTEST, false);
		g->stp.SetState(ST_ZWRITE, false);

		g->cm.SetCamera2d();

		g->stp.SetState(ST_BLEND, true);
		g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		unsigned char alpha = (unsigned char)(255.0f * (float)localTime / (float)allTime);
		GLFUNC(glDisable)(GL_TEXTURE_2D);

		g->stp.SetColor(0, 0, 0, alpha);
		
		g->stp.PrepareForRender();

		GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glVertex2f)(-1.0f, -1.0f);
		GLFUNC(glVertex2f)(-1.0f, 1.0f);
		GLFUNC(glVertex2f)(1.0f, 1.0f);
		GLFUNC(glVertex2f)(1.0f, -1.0f);
		GLFUNC(glEnd)();

		g->stp.SetBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
		g->stp.SetState(ST_BLEND, false);

		g->stp.SetColor(255, 255, 255);

		g->stp.SetState(ST_ZWRITE, true);
		g->stp.SetState(ST_ZTEST, true);

		GLFUNC(glEnable)(GL_TEXTURE_2D);
	}
}

void CAuxilaryProcessor::FadeOut(int localTime, int allTime)
{
	if ((localTime >= 0) && (localTime <= allTime))
	{
		g->stp.PopMatrix();
		g->stp.PushMatrix();

		g->stp.SetState(ST_ZTEST, false);
		g->stp.SetState(ST_ZWRITE, false);
		
		g->cm.SetCamera2d();

		g->stp.SetState(ST_BLEND, true);
		g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		unsigned char alpha = 255 - (unsigned char)(255.0f * (float)(localTime) / float(allTime));
		GLFUNC(glDisable)(GL_TEXTURE_2D);

		g->stp.SetColor(0, 0, 0, alpha);

		g->stp.PrepareForRender();

		GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glVertex2f)(-1.0f, -1.0f);
		GLFUNC(glVertex2f)(-1.0f, 1.0f);
		GLFUNC(glVertex2f)(1.0f, 1.0f);
		GLFUNC(glVertex2f)(1.0f, -1.0f);
		GLFUNC(glEnd)();

		GLFUNC(glBlendFunc)(GL_SRC_COLOR, GL_DST_COLOR);
		g->stp.SetState(ST_BLEND, false);

		g->stp.SetColor(255, 255, 255);

		g->stp.SetState(ST_ZWRITE, true);
		g->stp.SetState(ST_ZTEST, true);

		GLFUNC(glEnable)(GL_TEXTURE_2D);
	}
}

void CAuxilaryProcessor::WhiteIn(int localTime, int allTime)
{
	if ((localTime >= 0) && (localTime <= allTime))
	{
		g->stp.PopMatrix();
		g->stp.PushMatrix();

		g->stp.SetState(ST_ZTEST, false);
		g->stp.SetState(ST_ZWRITE, false);
		
		g->cm.SetCamera2d();

		g->stp.SetState(ST_BLEND, true);
		g->stp.SetBlendFunc(GL_ONE, GL_ONE);

		unsigned char alpha = (unsigned char)(255.0f * (float)localTime / (float)allTime);
		GLFUNC(glDisable)(GL_TEXTURE_2D);
		g->stp.SetColor(alpha, alpha, alpha, alpha);

		g->stp.PrepareForRender();

		GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glVertex2f)(-1.0f, -1.0f);
		GLFUNC(glVertex2f)(-1.0f, 1.0f);
		GLFUNC(glVertex2f)(1.0f, 1.0f);
		GLFUNC(glVertex2f)(1.0f, -1.0f);
		GLFUNC(glEnd)();

		g->stp.SetBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
		g->stp.SetState(ST_BLEND, false);

		g->stp.SetColor(255, 255, 255);

		g->stp.SetState(ST_ZWRITE, true);
		g->stp.SetState(ST_ZTEST, true);

		GLFUNC(glEnable)(GL_TEXTURE_2D);
	}
}

void CAuxilaryProcessor::WhiteOut(int localTime, int allTime)
{
	if ((localTime >= 0) && (localTime <= allTime))
	{
		g->stp.PopMatrix();
		g->stp.PushMatrix();

		g->stp.SetState(ST_ZTEST, false);
		g->stp.SetState(ST_ZWRITE, false);

		g->cm.SetCamera2d();

		g->stp.SetState(ST_BLEND, true);
		g->stp.SetBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		unsigned char alpha = 255 - (unsigned char)(255.0f * (float)(localTime) / float(allTime));
		GLFUNC(glDisable)(GL_TEXTURE_2D);
		g->stp.SetColor(255, 255, 255, alpha);

		g->stp.PrepareForRender();

		GLFUNC(glBegin)(GL_QUADS);
		GLFUNC(glVertex2f)(-1.0f, -1.0f);
		GLFUNC(glVertex2f)(-1.0f, 1.0f);
		GLFUNC(glVertex2f)(1.0f, 1.0f);
		GLFUNC(glVertex2f)(1.0f, -1.0f);
		GLFUNC(glEnd)();

		g->stp.SetBlendFunc(GL_SRC_COLOR,GL_DST_COLOR);
		g->stp.SetState(ST_BLEND, false);

		g->stp.SetColor(255, 255, 255);

		g->stp.SetState(ST_ZWRITE, true);
		g->stp.SetState(ST_ZTEST, true);

		GLFUNC(glEnable)(GL_TEXTURE_2D);
	}
}

CAuxilaryProcessor::~CAuxilaryProcessor()
{
}
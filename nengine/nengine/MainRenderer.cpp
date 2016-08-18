
#include "StdAfx.h"
#include "MainRenderer.h"
#include "cal3d/memory_leak.h"

CMainRenderer::CMainRenderer():
	//m_rendererNum(STANDARD_RENDERER),
	m_mode(MODE_RENDER_ALL),
	m_modeIndex(3)
{
	/*m_renderers[STANDARD_RENDERER] = &m_standardRenderer;
	m_renderers[DISPLAYLIST_RENDERER] = &m_displayListRenderer;
	m_renderers[VA_RENDERER] = &m_vaRenderer;*/
}

/*void CMainRenderer::Draw(int num, unsigned short* fs, float* vx, float* tx, float* normals, 
	int* textureNumbers, int fsCount, int primType)
{
	m_renderers[m_rendererNum]->RenderModel(num, fs, vx, tx, normals,
		textureNumbers, fsCount, primType);
}*/

CMainRenderer::~CMainRenderer()
{

}
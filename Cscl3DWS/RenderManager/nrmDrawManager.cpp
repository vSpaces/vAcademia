#include "stdafx.h"
/*#include "nrmDrawManager.h"

rmIDrawManager::rmIDrawManager()
{
	m_pFramework = NULL;
	m_sprite = NULL;
	renderTargetTextureID = -1;
	m_iTextureID = -1;
}

rmIDrawManager::~rmIDrawManager()
{
	if ( renderTargetTextureID !=-1 ){m_pFramework->get_itextlib()->release_texture(renderTargetTextureID);}
}

void rmIDrawManager::SetFramework(iframework* pFramework)
{
	m_pFramework = pFramework;
}

void rmIDrawManager::StartDrawOnImage( isprite* pImage )
{
 	int iTextureID = pImage->get_textureID();
	if ( renderTargetTextureID == -1 )
	{
		m_iShader = m_pFramework->get_ishaderlib()->get_ishader(pImage->get_shaderid());
		m_iShader->set_texture(0, GetDrawTextureID( iTextureID ));
		m_iTextureID = iTextureID;//запоминаем текстуру на которой рисуем
	}
	else if ( m_iTextureID != iTextureID && iTextureID != renderTargetTextureID)
	{
		itexture* pitexture = m_pFramework->get_itextlib()->get_itexture(m_iTextureID);
		assert(pitexture);
		lpimagesurface destsurface = pitexture->get_image_surface(0);
		assert(destsurface);

		itexture* pitexture2 = m_pFramework->get_itextlib()->get_itexture(renderTargetTextureID);
		assert(pitexture2);
		lpimagesurface srcsurface = pitexture2->get_image_surface(0);
		assert(srcsurface);

		RECT rect = {0, 0, 
			m_pFramework->get_itextlib()->get_itexture(m_iTextureID)->get_size().width - 1,
			m_pFramework->get_itextlib()->get_itexture(m_iTextureID)->get_size().height - 1};
		POINT pnt = {0, 0};
		m_pFramework->get_irenderer()->copy_rects(srcsurface, &rect, 1, destsurface, &pnt);

		srcsurface->Release();
		destsurface->Release();
		m_pFramework->get_itextlib()->release_texture(renderTargetTextureID);
		renderTargetTextureID = -1;

		m_iShader->set_texture(0, m_iTextureID );

		m_iShader = m_pFramework->get_ishaderlib()->get_ishader(pImage->get_shaderid());
		m_iShader->set_texture(0, GetDrawTextureID( m_iTextureID ));
		m_iTextureID = iTextureID;	//запоминаем текстуру на которой рисуем
	}
}

int rmIDrawManager::GetDrawTextureID(int iTextureID)
{
	itexture* pTexture = m_pFramework->get_itextlib()->get_itexture(iTextureID);
	if(!pTexture)	return -1;
	
	DWORD width = pTexture->get_size().width;
	DWORD height = pTexture->get_size().height;
	
	renderTargetTextureID = m_pFramework->get_itextlib()->add_render_target_texture(
		width,height,pTexture->get_format());

	if( renderTargetTextureID == -1)
	{
		assert( FALSE);
		return -1;
	}
	if( !m_pFramework->get_irenderer()->begin_render_to_texture(renderTargetTextureID, -1))
	{
		m_pFramework->get_itextlib()->release_texture(renderTargetTextureID);
		return -1;
	}
	assert( m_pFramework->get_irenderer()->get_active_viewport());
	m_pFramework->get_irenderer()->get_active_viewport()->clear(0x00000000, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);

	isprite* spr = NULL;
	iloadable* pSpriteL = m_pFramework->get_iobjslib()->create_image();
	GET_INTERFACE(isprite, pSpriteL, &spr);
	assert(spr);

	if( !spr->create(width,height))
	{
		m_pFramework->get_itextlib()->release_texture(renderTargetTextureID);
		return -1;
	}
	ishader* spr_ishader = m_pFramework->get_ishaderlib()->get_ishader(spr->get_shaderid());
	assert( spr_ishader);
	spr_ishader->set_texture(0, iTextureID);
	spr->render();
	spr->free_object();

	m_pFramework->get_irenderer()->end_render_to_texture();
	return renderTargetTextureID;
}


void rmIDrawManager::DrawLine( int aiX1, int aiY1, int aiX2, int aiY2, const mlColor color, int aiWeight )
{
	point2d *points = new point2d[2];
	//начальная точка
	points[0].x = aiX1;
	points[0].y = aiY1;
	//конечная точка
	points[1].x = aiX2;
	points[1].y = aiY2;

	DWORD dwColour = RGB(color.r, color.g, color.b) | ((color.a)<<24);
	if( m_pFramework->get_irenderer()->begin_render_to_texture(renderTargetTextureID, -1))
	{
		//m_pFramework->get_irenderer()->draw_lines2D(points,1,dwColour);
		if (aiWeight == 1)
		{
			m_pFramework->get_irenderer()->draw_lines2D(points,1,dwColour);
		}else if ( aiWeight > 1	)
		{
			RECT rcRect = { aiX1, aiY1, aiX2, aiY2};
			m_pFramework->get_irenderer()->draw_line2D(rcRect,aiWeight,dwColour);
		}
		m_pFramework->get_irenderer()->end_render_to_texture();
	}
}

void rmIDrawManager::DrawEllipse(RECT rcRect, mlColor color,unsigned int uiWeight, mlColor adwColorFill  )
{
	if (m_pFramework->get_irenderer()->begin_render_to_texture(renderTargetTextureID, -1))
	{
		DWORD _dwColour = RGB(color.r, color.g, color.b) | ((color.a)<<24);
		DWORD _dwColourFill = RGB(adwColorFill.r, adwColorFill.g, adwColorFill.b) | ((adwColorFill.a)<<24);

		m_pFramework->get_irenderer()->draw_ellipse2D( rcRect, _dwColour,uiWeight,_dwColourFill);
		m_pFramework->get_irenderer()->end_render_to_texture();
	}
}

void rmIDrawManager::DrawRectangle( RECT rcRect, mlColor color,unsigned int uiWeight, mlColor adwColorFill  )
{
	if (m_pFramework->get_irenderer()->begin_render_to_texture(renderTargetTextureID, -1))
	{
		DWORD _dwColour = RGB(color.r, color.g, color.b) | ((color.a)<<24);
		DWORD _dwColourFill = RGB(adwColorFill.r, adwColorFill.g, adwColorFill.b) | ((adwColorFill.a)<<24);

		m_pFramework->get_irenderer()->draw_rectangle2D(rcRect, _dwColour,uiWeight,_dwColourFill);
		m_pFramework->get_irenderer()->end_render_to_texture();
	}
}*/
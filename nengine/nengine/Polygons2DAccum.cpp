
#include "StdAfx.h"
#include "Polygons2DAccum.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

static int stat_batchCount = 0;

CPolygons2DAccum::CPolygons2DAccum():
	m_scaleX(0.0f),
	m_scaleY(0.0f),
	m_lineWidth(1),
	m_vertexCount(0),
	m_textureCount(0),	
	m_lastTextureID(-1),
	m_translationX(0.0f),
	m_translationY(0.0f),
	m_isScissorsNeeded(false),
	m_isTexturingEnabled(true)
{
	m_vertexOffset = &m_vx[0];
	m_textureCoordsOffset = &m_tc[0];
}

void CPolygons2DAccum::AddVertex(const float x1, const float y1)
{
	if (m_vertexCount >= MAX_VERTEX_COUNT * 2)
	{
		return;
	}

	float xx = x1 * m_scaleX + m_translationX;
	float yy = y1 * m_scaleY + m_translationY;

	*m_vertexOffset = xx;
	m_vertexOffset++;
	*m_vertexOffset = yy;
	m_vertexOffset++;
	m_vertexCount++;	

	if (m_vertexCount%4 == 0)
	{
		unsigned char wrongCount = 0;
		for (int k = 0; k < 4; k++)
		{
			float x = *(m_vertexOffset - 2 - k * 2);
			float y = *(m_vertexOffset - 1 - k * 2);
			if ((x < -1.0f) || (x > 1.0f) || (y < -1.0f) || (y > 1.0f))
			{
				wrongCount++;
			}
		}

		if (4 == wrongCount)
		{
			m_vertexCount -= 4;
			m_vertexOffset -= 8;

			m_textureCoordsOffset -= 8;
		}
	}	
}

void CPolygons2DAccum::AddTextureCoords(const float u1, const float v1)
{
	if (m_vertexCount >= MAX_VERTEX_COUNT * 2)
	{
		return;
	}

	*m_textureCoordsOffset = u1;
	m_textureCoordsOffset++;
	*m_textureCoordsOffset = v1;
	m_textureCoordsOffset++;
}

void CPolygons2DAccum::ChangeMaterialExt(const int textureID, const int engineTextureID, const int blendMode)
{
	S2DMaterialInfo* const materialInfo = &m_materials[m_textureCount];
	materialInfo->textureOffset = m_vertexCount;
	materialInfo->textureID = textureID;
	materialInfo->engineTextureID = engineTextureID;
	materialInfo->blendMode = blendMode;
	materialInfo->r = m_r;
	materialInfo->g = m_g;
	materialInfo->b = m_b;
	materialInfo->a = m_a;
	materialInfo->isScissorsNeeded = m_isScissorsNeeded;
	materialInfo->scissorX1 = m_scissorX1;
	materialInfo->scissorY1 = m_scissorY1;
	materialInfo->scissorX2 = m_scissorX2;
	materialInfo->scissorY2 = m_scissorY2;
	materialInfo->isSmooth = m_isSmooth;
	materialInfo->lineWidth = m_lineWidth;

	if (m_textureCount != 0)
	{
		S2DMaterialInfo* const prevMaterialInfo = materialInfo - 1;
		materialInfo->isMajorDifferenceWithPrevious = materialInfo->HasMajorDifferenceWith(prevMaterialInfo);
		materialInfo->isColorChanged = (prevMaterialInfo->r == m_r) || (prevMaterialInfo->g == m_g) ||
					(prevMaterialInfo->b == m_b) || (prevMaterialInfo->a == m_a);
	}
	m_textureCount++;
}

void CPolygons2DAccum::ChangeMaterial(const int textureID, const int blendMode)
{
	S2DMaterialInfo* const materialInfo = &m_materials[m_textureCount];
	materialInfo->textureOffset = m_vertexCount;
	materialInfo->textureID = textureID;
	materialInfo->engineTextureID = -1;
	materialInfo->blendMode = blendMode;
	materialInfo->r = m_r;
	materialInfo->g = m_g;
	materialInfo->b = m_b;
	materialInfo->a = m_a;
	materialInfo->isScissorsNeeded = m_isScissorsNeeded;
	materialInfo->scissorX1 = m_scissorX1;
	materialInfo->scissorY1 = m_scissorY1;
	materialInfo->scissorX2 = m_scissorX2;
	materialInfo->scissorY2 = m_scissorY2;

	materialInfo->isSmooth = m_isSmooth;

	if (m_textureCount != 0)
	{
		S2DMaterialInfo* const prevMaterialInfo = materialInfo - 1;
		materialInfo->isMajorDifferenceWithPrevious = materialInfo->HasMajorDifferenceWith(prevMaterialInfo);
		materialInfo->isColorChanged = (prevMaterialInfo->r == m_r) || (prevMaterialInfo->g == m_g) ||
					(prevMaterialInfo->b == m_b) || (prevMaterialInfo->a == m_a);
	}
	m_textureCount++;
}

__forceinline void CPolygons2DAccum::SetMaterialParams(const S2DMaterialInfo* const materialInfo)
{
	switch (materialInfo->blendMode)
	{
	case BLEND_MODE_NO_BLEND:
		g->stp.SetState(ST_BLEND, false);
		break;

	case BLEND_MODE_SPRITE:
	case BLEND_MODE_TEXT:
	case BLEND_MODE_LINE:
		g->stp.SetState(ST_BLEND, true);
		g->stp.SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;		
	}

	g->stp.SetColor(materialInfo->r, materialInfo->g, materialInfo->b, materialInfo->a);
	g->stp.PrepareForRender();
}

void CPolygons2DAccum::DrawRange(const int materialID, const int startOffset, const int endOffset)
{
	const S2DMaterialInfo* const materialInfo = &m_materials[materialID];
/*
#ifdef _DEBUG
	int oglTextureID = materialInfo->textureID;
	int textureID = g->tm.GetTextureIDByOGLID_DEBUG(oglTextureID);
	CTexture* texture = g->tm.GetObjectPointer(textureID);	
#endif
*/	
    if (materialInfo->isScissorsNeeded)
	{
		g->stp.EnableScissors(materialInfo->scissorX1, materialInfo->scissorY1,
			materialInfo->scissorX2, materialInfo->scissorY2);
	}

	/*if (materialInfo->isColorChanged)
	{*/
		
	//}

	bool isTextureChanged = false;
	if (materialInfo->textureID != -1)
	{
		if (m_lastTextureID != materialInfo->textureID)
		{
			if (materialID != 0)
			{
				GLFUNC(glEnd)();
			}

			if (!m_isTexturingEnabled)
			{
				GLFUNC(glEnable)(GL_TEXTURE_2D);
				m_isTexturingEnabled = true;
			}

			GLFUNC(glBindTexture)(GL_TEXTURE_2D, materialInfo->textureID);
			if (materialInfo->engineTextureID != -1)
			{
				int filterMode = materialInfo->isSmooth ? GL_LINEAR : GL_NEAREST;
				CTexture* texture = g->tm.GetObjectPointer(materialInfo->engineTextureID);
				texture->SetCurrentFiltration(filterMode, filterMode, 1, 0);
			}
			/*int filterMode = materialInfo->isSmooth ? GL_LINEAR : GL_NEAREST;
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
			GLFUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);			*/
			m_lastTextureID = materialInfo->textureID;

			SetMaterialParams(materialInfo);
			isTextureChanged = true;

			if (materialInfo->blendMode == BLEND_MODE_LINE)
			{
				glLineWidth((GLfloat)materialInfo->lineWidth);
			}
			GLFUNC(glBegin)((materialInfo->blendMode != BLEND_MODE_LINE) ? GL_QUADS : GL_LINES);
			stat_batchCount++;
		}
	}
	else 
	{
		if (m_lastTextureID != -1)
		{
			stat_batchCount++;
			if (materialID != 0)
			{
				GLFUNC(glEnd)();
			}

			if (m_isTexturingEnabled)
			{
				GLFUNC(glDisable)(GL_TEXTURE_2D);
				m_isTexturingEnabled = false;
			}
			m_lastTextureID = -1;

			SetMaterialParams(materialInfo);
			isTextureChanged = true;

			if (materialInfo->blendMode == BLEND_MODE_LINE)
			{
				glLineWidth((GLfloat)materialInfo->lineWidth);
			}
			GLFUNC(glBegin)((materialInfo->blendMode != BLEND_MODE_LINE) ? GL_QUADS : GL_LINES);
		}		
	}

	if (!isTextureChanged)	
	{
		if (materialInfo->isMajorDifferenceWithPrevious)
		{			
			GLFUNC(glEnd)();
			if (materialInfo->isColorChanged)
			{
				g->stp.SetColor(materialInfo->r, materialInfo->g, materialInfo->b, materialInfo->a);
			}
			g->stp.PrepareForRender();
			if (materialInfo->blendMode == BLEND_MODE_LINE)
			{
				glLineWidth((GLfloat)materialInfo->lineWidth);
			}
			GLFUNC(glBegin)((materialInfo->blendMode != BLEND_MODE_LINE) ? GL_QUADS : GL_LINES);
		}
		else if (materialInfo->isColorChanged)
		{
			g->stp.SetColor(materialInfo->r, materialInfo->g, materialInfo->b, materialInfo->a);
            g->stp.PrepareForRender();
		}
	}

	const float* oddVertex = &m_vx[startOffset * 2];
	const float* evenVertex = &m_vx[startOffset * 2 + 1];
	const float* oddTextureCoords = &m_tc[startOffset * 2];
	const float* evenTextureCoords = &m_tc[startOffset * 2 + 1];
	
	for (int i = startOffset; i < endOffset; i++)
	{
		GLFUNC(glTexCoord2f)(*oddTextureCoords, *evenTextureCoords);		
		GLFUNC(glVertex2f)(*oddVertex, *evenVertex);

		oddTextureCoords += 2;
		evenTextureCoords += 2;

		oddVertex += 2;
		evenVertex += 2;
	}

	if (materialInfo->isScissorsNeeded)
	{
		g->stp.DisableScissors();		
	}
}

void CPolygons2DAccum::Draw()
{	
	/*if (g->rs.GetBool(CAVE_SUPPORT))
	{
		glDisable(GL_LIGHTING);
	}*/

	stat_batchCount = 0;

	m_isTexturingEnabled = true;
	m_lastTextureID = -1;

	if (m_materials[0].blendMode == BLEND_MODE_LINE)
	{
		if (m_isTexturingEnabled)
		{
			GLFUNC(glDisable)(GL_TEXTURE_2D);
			m_isTexturingEnabled = false;
		}
		g->stp.SetColor(m_materials[0].r, m_materials[0].g, m_materials[0].b, m_materials[0].a);
		g->stp.PrepareForRender();
		glLineWidth((GLfloat)m_materials[0].lineWidth);
		GLFUNC(glBegin)(GL_LINES);
	}	

	for (int i = 0; i < m_textureCount - 1; i++)
	{
		if (m_materials[i + 1].textureOffset > MAX_VERTEX_COUNT)
		{
			continue;
		}
		DrawRange(i, m_materials[i].textureOffset, m_materials[i + 1].textureOffset);
	}

	if (m_textureCount != 0)
	{
		if (m_vertexCount < MAX_VERTEX_COUNT)
		{
			DrawRange(m_textureCount - 1, m_materials[m_textureCount - 1].textureOffset, m_vertexCount);
		}
	}

	GLFUNC(glEnd)();

	if (!m_isTexturingEnabled)
	{
		GLFUNC(glEnable)(GL_TEXTURE_2D);
	}

	g->tm.RefreshTextureOptimization();
}

void CPolygons2DAccum::Start()
{
	m_vertexCount = 0;
	m_textureCount = 0;
	m_vertexOffset = &m_vx[0];
	m_textureCoordsOffset = &m_tc[0];
	m_isScissorsNeeded = false;
}

void CPolygons2DAccum::End()
{
	m_materials[m_textureCount + 1].textureOffset = -1;
	m_materials[m_textureCount + 1].textureID = -1;
	m_materials[m_textureCount + 1].blendMode = BLEND_MODE_NO_BLEND;
}

CPolygons2DAccum::~CPolygons2DAccum()
{
}
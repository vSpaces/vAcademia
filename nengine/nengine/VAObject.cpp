#include "stdafx.h"
#include "VAObject.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

CVAObject::CVAObject():
	m_elementType(GL_TRIANGLES),
	m_isDynamicContent(false),
	m_vertexDimension(3),
	m_verticesInFace(3),
	m_vertices(NULL),
	m_vertexCount(0),
	m_normalCount(0),
	m_indices(NULL),
	m_normals(NULL),
	m_faceCount(0)
{
	for (int i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		m_texturesDimension[i] = 2;
		m_textureCoords[i] = NULL;
		m_txCount[i] = 0;
	}
}

void CVAObject::SetDynamicContentStatus(bool isDynamicContent)
{
	m_isDynamicContent = isDynamicContent;
}

void CVAObject::SetUpdateType(unsigned int /*type*/)
{
	// do nothing
}

int CVAObject::GetUpdateType()const
{
	return UPDATE_TYPE_DYNAMIC_ALL;
}

unsigned int CVAObject::GetFaceCount()const
{
	return m_faceCount;
}

unsigned int CVAObject::GetVertexCount()const
{
	return m_vertexCount;
}

void CVAObject::SetVertexDimension(unsigned int dimension)
{
	assert(dimension <= MAX_VERTEX_DIMENSION);

	if (dimension > MAX_VERTEX_DIMENSION)
	{
		return;
	}

	m_vertexDimension = dimension;
}

void CVAObject::SetTextureDimension(unsigned int textureLevel, unsigned int dimension)
{
	assert(textureLevel < MAX_TEXTURE_COUNT);
	if (textureLevel >= MAX_TEXTURE_COUNT)
	{
		return;
	}

	assert(dimension <= MAX_TEXTURE_DIMENSION);
	if (dimension > MAX_TEXTURE_DIMENSION)
	{
		return;
	}

	m_texturesDimension[textureLevel] = dimension;
}

void CVAObject::SetPrimitiveType(unsigned int elementType)
{
	assert((GL_POINTS == elementType) || (GL_LINES == elementType) || (GL_TRIANGLES == elementType) ||	(GL_QUADS == elementType));

	switch (elementType)
	{
	case GL_POINTS:
		m_verticesInFace = 1;
		break;

	case GL_LINES:
		m_verticesInFace = 2;
		break;

	case GL_TRIANGLES:
		m_verticesInFace = 3;
		break;

	case GL_QUADS:
		m_verticesInFace = 4;
		break;

	default:
		return;
	}

	m_elementType = elementType;
}

void CVAObject::SetVertexDescSize(unsigned int size) 
{
	throw std::logic_error("Vertex Attributes not implemented in VAO and BEO");
}

void CVAObject::SetAttribDesc(const VertexAttribDesc* attribDesc, unsigned int attribCount) 
{
	throw std::logic_error("Vertex Attributes not implemented in VAO and BEO");
}

void CVAObject::SetVertexArray(const void* vertex, unsigned int vertexCount)
{
	assert(vertex);
	if (!vertex)
	{
		return;
	}

	if (m_isDynamicContent)
	{
		bool isResizeNeeded = (vertexCount > m_vertexCount) ;

		if (m_vertices)
		{
			// если памяти требуется больше, то старый удаляется, новый заводится
			if (isResizeNeeded)
			{
				MP_DELETE_ARR(m_vertices);
				m_vertices = MP_NEW_ARR(float, vertexCount * 3);
			}

			// если памяти требуется столько же, то остается прежний указатель
		}
		else
		{
			m_vertices = MP_NEW_ARR(float, vertexCount * 3);
		}

		memcpy(m_vertices, vertex, vertexCount * 3 * sizeof(float));
	}
	else
	{
		m_vertices = (float*)vertex;
	}

	m_vertexCount = vertexCount;
}

void CVAObject::SetTangentsArray(float* tangents)
{
	SetTextureDimension(2, 4);
	SetTextureCoordsArray(2, tangents);
}

void CVAObject::SetTextureCoordsArray(unsigned int textureLevel, float* textureCoords)
{
	assert(textureCoords);

	assert(textureLevel < MAX_TEXTURE_COUNT);
	if (textureLevel >= MAX_TEXTURE_COUNT)
	{
		return;
	}

	if (m_isDynamicContent)
	{
		if ((m_textureCoords[textureLevel]) && (m_vertexCount * m_texturesDimension[textureLevel] > m_txCount[textureLevel]))
		{
			MP_DELETE_ARR(m_textureCoords[textureLevel]);
			m_textureCoords[textureLevel] = NULL;
		}

		if (!m_textureCoords[textureLevel])
		{
			m_textureCoords[textureLevel] = MP_NEW_ARR(float, m_vertexCount* m_texturesDimension[textureLevel]);
			m_txCount[textureLevel] = m_vertexCount * m_texturesDimension[textureLevel];
		}

		memcpy(m_textureCoords[textureLevel], textureCoords, m_vertexCount * m_texturesDimension[textureLevel] * sizeof(float));
	}
	else
	{
		m_textureCoords[textureLevel] = textureCoords;
	}

}

void CVAObject::SetNormalArray(float* normals)
{
	assert(normals);
	if (!normals)
	{
		return;
	}

	if (m_isDynamicContent)
	{
		if ((m_normals) && (m_normalCount < m_vertexCount))
		{
			MP_DELETE_ARR(m_normals);
			m_normals = NULL;
		}

		if (!m_normals)
		{
			m_normals = MP_NEW_ARR(float, m_vertexCount * 3);
		}
		memcpy(m_normals, normals, m_vertexCount * 3 * sizeof(float));
	}
	else
	{
		m_normals = normals;
	}
}

void CVAObject::SetFaceArray(unsigned short* indices, unsigned int faceCount)
{
	assert(indices);
	if (indices == NULL)
	{		
		return;
	}

	if (m_isDynamicContent)
	{
		if ((m_indices) && (m_faceCount < faceCount))
		{
			MP_DELETE_ARR(m_indices);
			m_indices = NULL;
		}

		if (!m_indices)
		{
			m_indices = MP_NEW_ARR(unsigned short, faceCount * m_verticesInFace);
		}
		memcpy(m_indices, indices, faceCount *  m_verticesInFace * sizeof(unsigned short));
	}
	else
	{
		m_indices = indices;
	}

	m_faceCount = faceCount;
}


void CVAObject::PreRender()
{
	g->stp.SetState(ST_VERTEX_ARRAY_CLIENT_STATE, true);

	if (m_textureCoords[0])
	{
		g->stp.SetTextureArrayState(true);
	}

	if (m_normals)
	{
		g->stp.SetState(ST_NORMAL_ARRAY_CLIENT_STATE, true);
		GLFUNC(glNormalPointer)(GL_FLOAT, 0, m_normals);
	}

	if (m_indices)
	{
		g->stp.SetState(ST_INDEX_ARRAY_CLIENT_STATE, true);
	}

	for (int i = 0; i < 3; i++)
	{
		if (m_textureCoords[i])
		{
			// TO DO: need to add support to more than one texture
			//когда текстур исп-ся сразу несколько, то каждая ложится на свой текстурный уровень и для каждой свои текстурные координаты
			if (i == 0)	
			{
				GLFUNC(glTexCoordPointer)(m_texturesDimension[i], GL_FLOAT, 0, m_textureCoords[i]);
			}
			else if (i == 1)
			{
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE1_ARB);
				GLFUNC(glEnableClientState)(GL_TEXTURE_COORD_ARRAY);
				GLFUNC(glTexCoordPointer)(m_texturesDimension[i], GL_FLOAT, 0, m_textureCoords[i]);
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE0_ARB);
			}
			else if (i == 2)
			{
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE2_ARB);
				GLFUNC(glEnableClientState)(GL_TEXTURE_COORD_ARRAY);
				GLFUNC(glTexCoordPointer)(m_texturesDimension[i], GL_FLOAT, 0,m_textureCoords[i]);
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE0_ARB);
			}		
		}
		else
		{	
			if (i == 1)	
			{
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE1_ARB);
				GLFUNC(glDisableClientState)(GL_TEXTURE_COORD_ARRAY);
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE0_ARB);
			}
			else if (i == 2)
			{
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE2_ARB);
				GLFUNC(glDisableClientState)(GL_TEXTURE_COORD_ARRAY);
				GLFUNC(glClientActiveTextureARB)(GL_TEXTURE0_ARB);
			}		
		}
	}

	if (m_vertices)
	{
		GLFUNC(glVertexPointer)(m_vertexDimension, GL_FLOAT, 0, m_vertices);//g->stp.SetVertexBuffer(m_vertexBufferID, m_vertexDimension);
	}

}

void CVAObject::Render()
{
	PreRender();
	g->stp.PrepareForRender();

	if (!m_indices)
	{
		GATHER_STAT((int)(m_vertexCount / 3));
		GLFUNC(glDrawArrays)(m_elementType, 0, m_vertexCount);
	}
	else
	{
		GATHER_STAT(m_faceCount);
		GLFUNC(glDrawElements)(m_elementType, m_faceCount * m_verticesInFace, GL_UNSIGNED_SHORT, m_indices);
	}

	PostRender();
}

void CVAObject::RenderRange(int offset, int faceCount, int minIndex, int maxIndex)
{
	g->stp.PrepareForRender();

	if (!m_indices)
	{
		GATHER_STAT((int)(faceCount / 3));
		GLFUNC(glDrawArrays)(m_elementType, offset, faceCount);
	}
	else
	{
		if ((minIndex == -1) || (maxIndex == -1))
		{
			minIndex = 0;
			maxIndex = m_vertexCount - 1;
		}

		GATHER_STAT(faceCount);
		GLFUNC(glDrawRangeElements)(m_elementType, minIndex, maxIndex, faceCount * m_verticesInFace, GL_UNSIGNED_SHORT, (void *)(m_indices + offset * m_verticesInFace));
	}
}

void CVAObject::PostRender()
{
	if (m_normals)
	{
		g->stp.SetState(ST_NORMAL_ARRAY_CLIENT_STATE, false);
	}

	for (int i = 0;i < MAX_TEXTURE_COUNT; i++)
	if (m_textureCoords[i])
	{
		g->stp.SetTextureArrayState(false);
	}

	if (m_indices)
	{
		g->stp.SetState(ST_INDEX_ARRAY_CLIENT_STATE, false);
	}

	g->stp.SetState(ST_VERTEX_ARRAY_CLIENT_STATE, false);
}

CVAObject::~CVAObject()
{
	if (m_isDynamicContent)
	{
		if (m_vertices)
		{
			MP_DELETE_ARR(m_vertices);
		}

		if (m_normals)
		{
			MP_DELETE_ARR(m_normals);
		}

		if (m_indices)
		{
			MP_DELETE_ARR(m_indices);
		}

		for (int i = 0; i < MAX_TEXTURE_COUNT; i++)
		if (m_textureCoords[i])
		{
			MP_DELETE_ARR(m_textureCoords[i]);
		}
	}
}
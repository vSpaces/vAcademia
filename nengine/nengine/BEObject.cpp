#include "stdafx.h"
#include "BEObject.h"
#include "GlobalSingletonStorage.h"

CBEObject::CBEObject():
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

void CBEObject::SetDynamicContentStatus(bool isDynamicContent)
{
	m_isDynamicContent = isDynamicContent;
}

void CBEObject::SetUpdateType(unsigned int /*type*/)
{
	// do nothing
}

int CBEObject::GetUpdateType()const
{
	return UPDATE_TYPE_DYNAMIC_ALL;
}

unsigned int CBEObject::GetFaceCount()const
{
	return m_faceCount;
}

unsigned int CBEObject::GetVertexCount()const
{
	return m_vertexCount;
}

void CBEObject::SetVertexDimension(unsigned int dimension)
{
	assert(dimension <= MAX_VERTEX_DIMENSION);

	if (dimension > MAX_VERTEX_DIMENSION)
	{
		return;
	}

	m_vertexDimension = dimension;
}

void CBEObject::SetTextureDimension(unsigned int textureLevel, unsigned int dimension)
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

void CBEObject::SetPrimitiveType(unsigned int elementType)
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

void CBEObject::SetVertexDescSize(unsigned int size) 
{
	throw std::logic_error("Vertex Attributes not implemented in VAO and BEO");
}

void CBEObject::SetAttribDesc(const VertexAttribDesc* attribDesc, unsigned int attribCount) 
{
	throw std::logic_error("Vertex Attributes not implemented in VAO and BEO");
}

void CBEObject::SetVertexArray(const void* vertex, unsigned int vertexCount)
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

void CBEObject::SetTangentsArray(float* tangents)
{
	SetTextureDimension(2, 4);
	SetTextureCoordsArray(2, tangents);
}

void CBEObject::SetTextureCoordsArray(unsigned int textureLevel, float* textureCoords)
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

void CBEObject::SetNormalArray(float* normals)
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

void CBEObject::SetFaceArray(unsigned short* indices, unsigned int faceCount)
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


void CBEObject::PreRender()
{
}

void CBEObject::Render()
{
	PreRender();
	g->stp.PrepareForRender();

	assert(false);

	PostRender();
}

void CBEObject::RenderRange(int offset, int faceCount, int minIndex, int maxIndex)
{
	g->stp.PrepareForRender();

	if (!m_indices)
	{
		assert(false);
	}
	else
	{
		if ((minIndex == -1) || (maxIndex == -1))
		{
			minIndex = 0;
			maxIndex = m_vertexCount - 1;
		}

		GATHER_STAT(faceCount);		
		
		GLFUNC(glBegin)(m_elementType);

		for (unsigned int i = offset * m_verticesInFace; i < offset * m_verticesInFace + faceCount * m_verticesInFace; i += m_verticesInFace)
		for (unsigned int j = i; j < i + m_verticesInFace; j++)
		{
			unsigned int index = m_indices[j] * m_verticesInFace;
			unsigned int texIndex = m_indices[j] * 2;
			unsigned int normalIndex = m_indices[j] * 3;
			for (int t = 0; t < MAX_TEXTURE_COUNT; t++)
			if (m_textureCoords[t])
			{
				float* tc = &m_textureCoords[t][texIndex];
				GLFUNC(glMultiTexCoord2fARB)(GL_TEXTURE0_ARB + t, tc[0], tc[1]);
			}

			if (m_normals)
			{
				float* n = &m_normals[normalIndex];
				GLFUNC(glNormal3f)(n[0], n[1], n[2]);
			}

			float* vertex = &m_vertices[index];
			GLFUNC(glVertex3f)(vertex[0], vertex[1], vertex[2]);
		}

		GLFUNC(glEnd)();		
	}
}

void CBEObject::PostRender()
{
}

CBEObject::~CBEObject()
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
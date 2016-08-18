#include "stdafx.h"
#include "VBOObject.h"
#include "GlobalSingletonStorage.h"

#define GATHER_STAT(x)	stat_drawCount++; stat_polygonCount += (x); if (g->stp.GetState(ST_ALPHATEST) == 1) stat_alphatestPolygonCount += (x); else if (g->stp.GetState(ST_BLEND) == 1) stat_transparentPolygonCount += (x); else stat_solidPolygonCount += (x);

CVBOObject::CVBOObject():
	m_updateTextureCoordsType(GL_STATIC_DRAW),
	m_updateNormalsType(GL_STATIC_DRAW),
	m_updateVertexType(GL_STATIC_DRAW),
	m_updateFacesType(GL_STATIC_DRAW),
	m_updateType(UPDATE_TYPE_STATIC),
	m_elementType(GL_TRIANGLES),
	m_vertexBufferID(0xFFFFFFFF),
	m_normalBufferID(0xFFFFFFFF),
	m_indexBufferID(0xFFFFFFFF),
	m_isTexturingUsed(false),
	m_isNormalsUsed(false),
	m_isIndicesUsed(false),
	m_isAttribUsed(false),
	m_vertexDimension(3),
	m_vaoID(0xFFFFFFFF),
	m_verticesInFace(3),
	m_vertexCount(0),
	m_faceCount(0),
	m_attribDesc(nullptr),
	m_vertexDescSize(0)
{
	m_isUsedVAO = /*g->ei.IsExtensionSupported(GLEXT_arb_vertex_array_object)*/ false;

	for(int i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		m_texturesDimension[i] = 2;
		m_textureCoordBufferID[i] = 0xFFFFFFFF;
	}

	m_isIntel = (g->gi.GetVendorID() == VENDOR_INTEL);
}

void CVBOObject::SetUpdateType(unsigned int type)
{
	m_updateType = type;

	switch (m_updateType)
	{
	case UPDATE_TYPE_STATIC:
		m_updateTextureCoordsType = GL_STATIC_DRAW;
		m_updateNormalsType = GL_STATIC_DRAW;
		m_updateVertexType = GL_STATIC_DRAW;
		m_updateFacesType = GL_STATIC_DRAW;
		break;

	case UPDATE_TYPE_DYNAMIC_VERTEX_NORMAL:
		m_updateTextureCoordsType = GL_STATIC_DRAW;
		m_updateNormalsType = GL_DYNAMIC_DRAW;
		m_updateVertexType = GL_DYNAMIC_DRAW;
		m_updateFacesType = GL_STATIC_DRAW;
		break;

	case UPDATE_TYPE_DYNAMIC_ALL:
		m_updateTextureCoordsType = GL_DYNAMIC_DRAW;
		m_updateNormalsType = GL_DYNAMIC_DRAW;
		m_updateVertexType = GL_DYNAMIC_DRAW;
		m_updateFacesType = GL_DYNAMIC_DRAW;
		break;
	}
}

int CVBOObject::GetUpdateType()const
{
	return m_updateType;
}

unsigned int CVBOObject::GetFaceCount()const
{
	return m_faceCount;
}

unsigned int CVBOObject::GetVertexCount()const
{
	return m_vertexCount;
}

void CVBOObject::SetVertexDimension(unsigned int dimension)
{
	assert(dimension <= MAX_VERTEX_DIMENSION);

	if (dimension > MAX_VERTEX_DIMENSION)
	{
		return;
	}

	m_vertexDimension = dimension;
}

void CVBOObject::SetDynamicContentStatus(bool /*isDynamicContent*/)
{
}

void CVBOObject::SetTextureDimension(unsigned int textureLevel, unsigned int dimension)
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

void CVBOObject::SetPrimitiveType(unsigned int elementType)
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

void CVBOObject::SetVertexDescSize(unsigned int size) 
{
	m_vertexDescSize = size;
}

void CVBOObject::SetAttribDesc(const VertexAttribDesc* attribDesc, unsigned int attribCount) 
{
	m_attribDescCount = attribCount;
	m_attribDesc = new VertexAttribDesc[attribCount];
	memcpy(m_attribDesc, attribDesc, attribCount * sizeof(attribDesc[0]));
	m_isAttribUsed = true;
}

void CVBOObject::SetVertexArray(const void* vertex, unsigned int vertexCount)
{
	assert(vertex);
	if (!vertex)
	{
		return;
	}

	m_vertexCount = vertexCount;

	unsigned int size = (m_isAttribUsed)
		? vertexCount * m_vertexDescSize
		: vertexCount * m_vertexDimension * SCALAR_SIZE;

	if (0xFFFFFFFF == m_vertexBufferID)
	{
		GLFUNC(glGenBuffersARB)(1, &m_vertexBufferID);
		GLFUNC(glBindBufferARB)(GL_ARRAY_BUFFER, m_vertexBufferID);	
		GLFUNC(glBufferDataARB)(GL_ARRAY_BUFFER, size, vertex, m_updateVertexType);
	}
	else
	{
		GLFUNC(glBindBufferARB)(GL_ARRAY_BUFFER, m_vertexBufferID);	
		GLFUNC(glBufferSubDataARB)(GL_ARRAY_BUFFER, 0, size, vertex);
	}

	g->stp.SetVertexBuffer(-1, 0);
	
//	glBindBufferARB(GL_ARRAY_BUFFER, 0);
}

void CVBOObject::SetTangentsArray(float* tangents)
{
	SetTextureDimension(2, 4);
	SetTextureCoordsArray(2, tangents);
}

void CVBOObject::SetTextureCoordsArray(unsigned int textureLexel, float* textureCoords)
{
	assert(textureCoords);
	if (!textureCoords)
	{
		m_textureCoordBufferID[textureLexel] = 0xFFFFFFFF;
		m_isTexturingUsed = false;

		for (int i = 0; i < MAX_TEXTURE_COUNT; i++)
		if (0xFFFFFFFF == m_textureCoordBufferID[i])
		{
			m_isTexturingUsed = true;
		}

		return;
	}		

	assert(textureLexel < MAX_TEXTURE_COUNT);
	if (textureLexel >= MAX_TEXTURE_COUNT)
	{
		return;
	}

	unsigned int textureCoordBufferID = 0;	
	if (0xFFFFFFFF == m_textureCoordBufferID[textureLexel])
	{
		GLFUNC(glGenBuffersARB)(1, &textureCoordBufferID);		
		GLFUNC(glBindBufferARB)(GL_ARRAY_BUFFER, textureCoordBufferID);	
		GLFUNC(glBufferDataARB)(GL_ARRAY_BUFFER, m_vertexCount * m_texturesDimension[textureLexel] * SCALAR_SIZE, textureCoords, m_updateTextureCoordsType);
	}
	else
	{
		textureCoordBufferID = m_textureCoordBufferID[textureLexel];
		GLFUNC(glBindBufferARB)(GL_ARRAY_BUFFER, textureCoordBufferID);	
		GLFUNC(glBufferSubDataARB)(GL_ARRAY_BUFFER, 0, m_vertexCount * m_texturesDimension[textureLexel] * SCALAR_SIZE, textureCoords);
	}

//	glBindBufferARB(GL_ARRAY_BUFFER, 0);

	g->stp.SetTextureCoordsBuffer(m_texturesDimension[0], 0xFFFFFFFF, m_texturesDimension[1], 0xFFFFFFFF,
		m_texturesDimension[2], 0xFFFFFFFF);
	
	m_textureCoordBufferID[textureLexel] = textureCoordBufferID;
	m_isTexturingUsed = true;
	
}

void CVBOObject::SetNormalArray(float* normals)
{
	assert(normals);
	if (!normals)
	{
		m_isNormalsUsed = false;
		return;
	}

	if (0xFFFFFFFF == m_normalBufferID)
	{
		GLFUNC(glGenBuffersARB)(1, &m_normalBufferID);
		GLFUNC(glBindBufferARB)(GL_ARRAY_BUFFER, m_normalBufferID);	
		GLFUNC(glBufferDataARB)(GL_ARRAY_BUFFER, m_vertexCount * NORMALS_DIMENSION * SCALAR_SIZE, normals, m_updateNormalsType);
	}
	else
	{
		GLFUNC(glBindBufferARB)(GL_ARRAY_BUFFER, m_normalBufferID);	
		GLFUNC(glBufferSubDataARB)(GL_ARRAY_BUFFER, 0, m_vertexCount * NORMALS_DIMENSION * SCALAR_SIZE, normals);
	}
	
	//glBindBufferARB(GL_ARRAY_BUFFER, 0);
	g->stp.SetNormalBuffer(0);
	m_isNormalsUsed = true;	
}

void CVBOObject::SetFaceArray(unsigned short* indices, unsigned int faceCount)
{
	assert(indices);
	if (indices == NULL)
	{		
		m_isIndicesUsed = false;
		return;
	}

	if (0xFFFFFFFF == m_indexBufferID)
	{
		GLFUNC(glGenBuffersARB)(1, &m_indexBufferID);
		GLFUNC(glBindBufferARB)(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferID);	
		GLFUNC(glBufferDataARB)(GL_ELEMENT_ARRAY_BUFFER, faceCount * m_verticesInFace * INDICES_IN_FACE, indices, m_updateFacesType);
	}
	else
	{
		GLFUNC(glBindBufferARB)(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferID);	
		GLFUNC(glBufferSubDataARB)(GL_ELEMENT_ARRAY_BUFFER, 0, faceCount * m_verticesInFace * INDICES_IN_FACE, indices);
	}
	
	//glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
	g->stp.SetIndexBuffer(0);
	
	m_isIndicesUsed = true;
	m_faceCount = faceCount;
}

void CVBOObject::PreRender()
{
	if (m_isUsedVAO)
	{
		return;
	}

	g->stp.SetState(ST_VERTEX_ARRAY_CLIENT_STATE, true);

	if (m_isTexturingUsed)
	{
		g->stp.SetTextureArrayState(true);
	}

	if (m_isNormalsUsed)
	{
		g->stp.SetState(ST_NORMAL_ARRAY_CLIENT_STATE, true);
	}

	if (m_isIndicesUsed)
	{
		g->stp.SetState(ST_INDEX_ARRAY_CLIENT_STATE, true);
	}

	if (m_isNormalsUsed)
	{
		assert(m_normalBufferID != 0xFFFFFFFF);
		g->stp.SetNormalBuffer(m_normalBufferID);
	}

	if (m_isTexturingUsed)
	{
		// TO DO: need to add support to more than one texture

		g->stp.SetTextureCoordsBuffer(m_texturesDimension[0], m_textureCoordBufferID[0], m_texturesDimension[1], m_textureCoordBufferID[1], m_texturesDimension[2], m_textureCoordBufferID[2]);
	}
	
	assert(m_vertexBufferID != 0xFFFFFFFF);
	g->stp.SetVertexBuffer(m_vertexBufferID, m_vertexDimension);

	if (m_isIndicesUsed)
	{
		assert(m_indexBufferID != 0xFFFFFFFF);
		g->stp.SetIndexBuffer(m_indexBufferID);
	}

	if (m_isAttribUsed)
	{
		GLFUNC(glBindBufferARB)(GL_ARRAY_BUFFER, m_vertexBufferID);	
		for (unsigned int i = 0; i < m_attribDescCount; ++i)
		{
			const VertexAttribDesc& attrib = m_attribDesc[i];
			GLFUNC(glEnableVertexAttribArray)(attrib.Location);	
			GLFUNC(glVertexAttribPointer)(attrib.Location, attrib.Size, attrib.Type, false, m_vertexDescSize, reinterpret_cast<char*>(attrib.Offset));
		}
	}
}

void CVBOObject::Render()
{
	PreRender();
	g->stp.PrepareForRender();

	if (!m_isIndicesUsed)
	{
		GATHER_STAT((int)(m_vertexCount / 3));
		GLFUNC(glDrawArrays)(m_elementType, 0, m_vertexCount);
	}
	else
	{
		GATHER_STAT(m_faceCount);
		assert(m_indexBufferID != 0xFFFFFFFF);
		//glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferID);
		GLFUNC(glDrawElements)(m_elementType, m_faceCount * m_verticesInFace, GL_UNSIGNED_SHORT, NULL);
		//glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	PostRender();
}

void CVBOObject::RenderRange(int offset, int faceCount, int minIndex, int maxIndex)
{
	g->stp.PrepareForRender();
	
	if (!m_isIndicesUsed)
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
		if (!m_isIntel)
		{
			GLFUNC(glDrawRangeElements)(m_elementType, minIndex, maxIndex, faceCount * m_verticesInFace, GL_UNSIGNED_SHORT, (void *)(offset * m_verticesInFace * 2));
		}
		else
		{
			GLFUNC(glDrawElements)(m_elementType, faceCount * m_verticesInFace, GL_UNSIGNED_SHORT, (void *)(offset * m_verticesInFace * 2));
		}
	}
}

void CVBOObject::PostRender()
{
	if (m_isUsedVAO)
	{
		return;
	}

	if (m_isIndicesUsed)
	{
		g->stp.SetIndexBuffer(0);
	}

	if (m_isNormalsUsed)
	{
		g->stp.SetState(ST_NORMAL_ARRAY_CLIENT_STATE, false);
	}
	
	if (m_isTexturingUsed)
	{
		g->stp.SetTextureArrayState(false);
	}
	
	if (m_isIndicesUsed)
	{
		g->stp.SetState(ST_INDEX_ARRAY_CLIENT_STATE, false);
	}

	if (m_isAttribUsed)
	{
		GLFUNC(glBindBufferARB)(GL_ARRAY_BUFFER, m_vertexBufferID);	
		for (unsigned int i = 0; i < m_attribDescCount; ++i)
		{
			const VertexAttribDesc& attrib = m_attribDesc[i];
			GLFUNC(glDisableVertexAttribArray)(attrib.Location);
		}
	}

	g->stp.SetState(ST_VERTEX_ARRAY_CLIENT_STATE, false);
}

CVBOObject::~CVBOObject()
{
	if (m_vertexBufferID != 0xFFFFFFFF)
	{
		GLFUNC(glDeleteBuffersARB)(1, &m_vertexBufferID);	
	}

	if (m_indexBufferID != 0xFFFFFFFF)
	{
		GLFUNC(glDeleteBuffersARB)(1, &m_indexBufferID);	
	}
	
	if (m_normalBufferID != 0xFFFFFFFF)
	{
		GLFUNC(glDeleteBuffersARB)(1, &m_normalBufferID);
	}	

	for (int i = 0; i < MAX_TEXTURE_COUNT; ++i)
	if (m_textureCoordBufferID[i] != 0xFFFFFFFF)
	{
		GLFUNC(glDeleteBuffersARB)(1, &m_textureCoordBufferID[i]);
	}	
	if (m_attribDesc) {
		delete[] m_attribDesc;
	}
}
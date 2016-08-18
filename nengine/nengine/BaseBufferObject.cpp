#include "stdafx.h"
#include "BaseBufferObject.h"
#include "VAObject.h"
#include "BEObject.h"

CBaseBufferObject::CBaseBufferObject()
{
	if (!g->rs.GetBool(USE_VBO))
	{
		if (g->rs.GetBool(USE_VA))
		{
			m_bufferObject = MP_NEW(CVAObject);
		}
		else
		{
			m_bufferObject = MP_NEW(CBEObject);
		}
	}
	else
	{
		m_bufferObject = MP_NEW(CVBOObject);
	}
}

void CBaseBufferObject::SetDynamicContentStatus(bool isDynamicContent)
{
	m_bufferObject->SetDynamicContentStatus(isDynamicContent);
}

void CBaseBufferObject::SetUpdateType(unsigned int type)
{
	m_bufferObject->SetUpdateType(type);
}

int CBaseBufferObject::GetUpdateType()const
{
	return m_bufferObject->GetUpdateType();
}

unsigned int CBaseBufferObject::GetFaceCount()const
{
	return m_bufferObject->GetFaceCount();
}

unsigned int CBaseBufferObject::GetVertexCount()const
{
	return	m_bufferObject->GetVertexCount();
}

void CBaseBufferObject::SetVertexDimension(unsigned int dimension)
{
	m_bufferObject->SetVertexDimension(dimension);
}

void CBaseBufferObject::SetTextureDimension(unsigned int textureLevel, unsigned int dimension)
{
	m_bufferObject->SetTextureDimension(textureLevel,dimension);
}

void CBaseBufferObject::SetPrimitiveType(unsigned int elementType)
{
	m_bufferObject->SetPrimitiveType(elementType);
}

void CBaseBufferObject::SetVertexDescSize(unsigned int size) 
{
	m_bufferObject->SetVertexDescSize(size);
}

void CBaseBufferObject::SetAttribDesc(const VertexAttribDesc* attribDesc, unsigned int attribCount) 
{
	m_bufferObject->SetAttribDesc(attribDesc, attribCount);
}

void CBaseBufferObject::SetVertexArray(const void* vertex, unsigned int vertexCount)
{
	m_bufferObject->SetVertexArray(vertex,vertexCount);
}	

void CBaseBufferObject::SetTangentsArray(float* tangents)
{
	m_bufferObject->SetTangentsArray(tangents);
}

void CBaseBufferObject::SetTextureCoordsArray(unsigned int textureLexel, float* textureCoords)
{
	m_bufferObject->SetTextureCoordsArray(textureLexel,textureCoords);
}

void CBaseBufferObject::SetNormalArray(float* normals)
{
	m_bufferObject->SetNormalArray(normals);
}

void CBaseBufferObject::SetFaceArray(unsigned short* indices, unsigned int faceCount)
{
	m_bufferObject->SetFaceArray(indices,faceCount);
}


void CBaseBufferObject::PreRender()
{
	m_bufferObject->PreRender();
}

void CBaseBufferObject::Render()
{
	m_bufferObject->Render();
}

void CBaseBufferObject::RenderRange(int offset, int faceCount, int minIndex, int maxIndex)
{
	m_bufferObject->RenderRange(offset, faceCount, minIndex, maxIndex);
}

void CBaseBufferObject::PostRender()
{
	m_bufferObject->PostRender();
}

CBaseBufferObject::~CBaseBufferObject()
{
	if (!g->rs.GetBool(USE_VBO))
	{
		if (g->rs.GetBool(USE_VA))
		{
			CVAObject* va = (CVAObject*)m_bufferObject;
			MP_DELETE(va);
		}
		else
		{
			CBEObject* be = (CBEObject*)m_bufferObject;
			MP_DELETE(be);
		}
	}
	else
	{
		CVBOObject* vbo = (CVBOObject*)m_bufferObject;
		MP_DELETE(vbo);		
	}
}
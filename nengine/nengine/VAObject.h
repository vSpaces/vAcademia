#pragma once

#include <vector>
#include "IBaseBufferObject.h"

class CVAObject : public IBaseBufferObject 
{
public:
	CVAObject();
	virtual ~CVAObject();

	void SetPrimitiveType(unsigned int elementType);
	void SetVertexDimension(unsigned int dimension);
	void SetTextureDimension(unsigned int textureLevel, unsigned int dimension);
	void SetVertexDescSize(unsigned int size) override;
	void SetAttribDesc(const VertexAttribDesc* attribDesc, unsigned int attribCount) override;

	void SetVertexArray(const void* vertex, unsigned int vertexCount);
	void SetNormalArray(float* normal);
	void SetTangentsArray(float* tangents);
	void SetTextureCoordsArray(unsigned int textureLexel, float* textureCoords);
	void SetFaceArray(unsigned short* indices, unsigned int faceCount);

	void SetUpdateType(unsigned int type);
	int GetUpdateType()const;	

	void SetDynamicContentStatus(bool isDynamicContent);

	unsigned int GetFaceCount()const;
	unsigned int GetVertexCount()const;

	void PreRender();
	void Render();
	void RenderRange(int offset, int faceCount, int minIndex = -1, int maxIndex = -1);
	void PostRender();

private:
	unsigned int	m_elementType;
	unsigned int	m_vertexDimension;
	unsigned int	m_verticesInFace;

	float*		m_vertices;
	float*		m_normals;
	unsigned short*	m_indices; 

	unsigned int	m_texturesDimension[MAX_TEXTURE_COUNT];
	float*		m_textureCoords[MAX_TEXTURE_COUNT];

	unsigned int	m_vertexCount;
	unsigned int	m_normalCount;
	unsigned int 	m_txCount[MAX_TEXTURE_COUNT];
	unsigned int	m_faceCount;

	bool		m_isDynamicContent;
};

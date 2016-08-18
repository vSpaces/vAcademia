#pragma once

#include <vector>

#define UPDATE_TYPE_STATIC					0
#define UPDATE_TYPE_DYNAMIC_VERTEX_NORMAL	1
#define UPDATE_TYPE_DYNAMIC_ALL				2

#define MAX_TEXTURE_COUNT		8
#define SCALAR_SIZE				sizeof(float)
#define MAX_VERTEX_DIMENSION	4
#define MAX_TEXTURE_DIMENSION	4
#define NORMALS_DIMENSION		3
#define INDICES_IN_FACE			2

class CVBOObject : public IBaseBufferObject 
{
public:
	CVBOObject();
	virtual ~CVBOObject();
	
	void SetPrimitiveType(unsigned int elementType);
	void SetVertexDimension(unsigned int dimension);
	void SetTextureDimension(unsigned int textureLevel, unsigned int dimension);
	void SetVertexDescSize(unsigned int size) override;
	void SetAttribDesc(const VertexAttribDesc* attribDesc, unsigned int attribCount) override;

	void SetDynamicContentStatus(bool isDynamicContent);
	
	void SetVertexArray(const void* vertex, unsigned int vertexCount);
	//void SetVertexAttribArray(const void* buffer, unsigned int vertexCount, unsigned int sizeOfDesc);
	void SetNormalArray(float* normal);
	void SetTangentsArray(float* tangents);
	void SetTextureCoordsArray(unsigned int textureLexel, float* textureCoords);
	void SetFaceArray(unsigned short* indices, unsigned int faceCount);

	void SetUpdateType(unsigned int type);
    int GetUpdateType()const;	

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
	unsigned int	m_vertexDescSize;
		
	unsigned int	m_vertexBufferID;
	unsigned int	m_normalBufferID;
	unsigned int	m_indexBufferID;
	unsigned int	m_vaoID;
	
	unsigned int	m_texturesDimension[MAX_TEXTURE_COUNT];
	unsigned int	m_textureCoordBufferID[MAX_TEXTURE_COUNT];

	unsigned int	m_vertexCount;
	unsigned int	m_faceCount;
	
	bool			m_isNormalsUsed;
	bool			m_isIndicesUsed;
	bool			m_isTexturingUsed;
	bool			m_isAttribUsed;
	bool			m_isUsedVAO;
	bool			m_isIntel;

	unsigned int	m_updateTextureCoordsType;
	unsigned int	m_updateNormalsType;
	unsigned int	m_updateVertexType;
	unsigned int	m_updateFacesType;
	unsigned int	m_updateType;
	
	VertexAttribDesc* m_attribDesc;
	unsigned int	  m_attribDescCount;
};

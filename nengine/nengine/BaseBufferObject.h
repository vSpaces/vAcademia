#pragma once

#include "IBaseBufferObject.h"

#define UPDATE_TYPE_STATIC					0
#define UPDATE_TYPE_DYNAMIC_VERTEX_NORMAL	1
#define UPDATE_TYPE_DYNAMIC_ALL				2

class CBaseBufferObject : public IBaseBufferObject
{
public:
	CBaseBufferObject();
	~CBaseBufferObject();

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
	IBaseBufferObject* m_bufferObject;
};
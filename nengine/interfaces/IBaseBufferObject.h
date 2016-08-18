#pragma once

#define UPDATE_TYPE_STATIC			0
#define UPDATE_TYPE_DYNAMIC_VERTEX_NORMAL	1
#define UPDATE_TYPE_DYNAMIC_ALL			2

#define MAX_TEXTURE_COUNT		8
#define SCALAR_SIZE			sizeof(float)
#define MAX_VERTEX_DIMENSION		4
#define MAX_TEXTURE_DIMENSION		4
#define NORMALS_DIMENSION		3
#define INDICES_IN_FACE			2

#define GATHER_STAT(x)	stat_drawCount++; stat_polygonCount += (x); if (g->stp.GetState(ST_ALPHATEST) == 1) stat_alphatestPolygonCount += (x); else if (g->stp.GetState(ST_BLEND) == 1) stat_transparentPolygonCount += (x); else stat_solidPolygonCount += (x);

class IBaseBufferObject
{
public:
	struct VertexAttribDesc
	{
		unsigned int Location;
		unsigned int Size;
		unsigned int Type;
		int	Offset;
	};

	IBaseBufferObject(){};
	virtual ~IBaseBufferObject(){};

	virtual void SetPrimitiveType(unsigned int elementType) = 0;
	virtual void SetVertexDimension(unsigned int dimension) = 0;
	virtual void SetTextureDimension(unsigned int textureLevel, unsigned int dimension) = 0;
	virtual void SetVertexDescSize(unsigned int size) = 0;
	virtual void SetAttribDesc(const VertexAttribDesc* attribDesc, unsigned int attribCount) = 0;	
	
	virtual void SetVertexArray(const void* vertex, unsigned int vertexCount) = 0;
	virtual void SetNormalArray(float* normal) = 0;
	virtual void SetTangentsArray(float* tangents) = 0;
	virtual void SetTextureCoordsArray(unsigned int textureLexel, float* textureCoords) = 0;
	virtual void SetFaceArray(unsigned short* indices, unsigned int faceCount) = 0;

	virtual void SetUpdateType(unsigned int type) = 0;
	virtual int GetUpdateType()const = 0;	

	virtual void SetDynamicContentStatus(bool isDynamicContent) = 0;

	virtual unsigned int GetFaceCount()const = 0;
	virtual unsigned int GetVertexCount()const = 0;

	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void RenderRange(int offset, int faceCount, int minIndex = -1, int maxIndex = -1) = 0;
	virtual void PostRender() = 0;
};
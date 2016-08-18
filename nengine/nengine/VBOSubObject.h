
#pragma once

#include "VBOObject.h"

// Возвращаемые значения UpdateBillboardOrient
#define BILLBOARD_PROCESSED_OK						0
#define BILLBOARD_PROCESSED_FAILED					1
#define BILLBOARD_PROCESSED_SHARED_VERTICES_WARNING	2

class CVBOSubObject
{
public:
	CVBOSubObject();
	~CVBOSubObject();

	void UnmarkCorrectness();
	bool IsCorrected()const;

	void SetBillboardSize(float sizeXY, float sizeZ);
	unsigned int UpdateBillboardOrient(float* vertices, float* textureCoords, unsigned int vertexCount, unsigned short* indices, bool evenOnCPU);

	void SetIndexArray(unsigned short* indices, unsigned int startOffset, unsigned int faceCount);
	void SetBufferObject(CBaseBufferObject* vbo);
	void Render();

private:
	CBaseBufferObject* m_vbo;

	bool m_isCorrected;
	float m_sizeXY, m_sizeZ;

	unsigned int m_startOffset;
	unsigned int m_faceCount;

	unsigned int m_indexMinValue;
	unsigned int m_indexMaxValue;
};
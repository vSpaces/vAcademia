
#pragma once

#include <PolygonAccum.h>

#define MAX_VERTEX_COUNT	65535
#define MAX_MATERIALS		16384

#define BLEND_MODE_NO_BLEND	0
#define BLEND_MODE_SPRITE	1
#define BLEND_MODE_TEXT		2
#define BLEND_MODE_LINE		3

typedef struct _S2DMaterialInfo
{
	int textureID;
	int engineTextureID;
	int blendMode;
	int textureOffset;
	bool isScissorsNeeded;
	unsigned char r, g, b, a;
	int scissorX1;
	int scissorY1;
	int scissorX2;
	int scissorY2;
	bool isColorChanged;
	bool isMajorDifferenceWithPrevious;
	bool isSmooth;
	int lineWidth;

	_S2DMaterialInfo()
	{
		textureID = -1;
		engineTextureID = -1;
		blendMode = BLEND_MODE_NO_BLEND;
		textureOffset = -1;
		scissorX1 = -1;
		scissorY1 = -1;
		scissorX2 = -1;
		scissorY2 = -1;
		r = 255;
		g = 255;
		b = 255;
		a = 255;
		isScissorsNeeded = false;
		isMajorDifferenceWithPrevious = false;
		isColorChanged = false;
		isSmooth = false;
		lineWidth = 1;
	}

	bool HasMajorDifferenceWith(const _S2DMaterialInfo* const info)const
	{
		return ((info->blendMode != blendMode) || (info->isScissorsNeeded != isScissorsNeeded) || 
			//(info->r == r) || (info->g == g) || (info->b == b) || (info->a == a) ||
			(info->scissorX1 == scissorX1) || (info->scissorY1 == scissorY1) ||
			(info->scissorX2 == scissorX2) || (info->scissorY2 == scissorY2) || (info->isScissorsNeeded) ||
			(isScissorsNeeded));
	}
} S2DMaterialInfo;

class CPolygons2DAccum : public IPolygonsAccum
{
public:
	CPolygons2DAccum();
	~CPolygons2DAccum();

	void AddVertex(const float x1, const float y1);
	void AddTextureCoords(const float u1, const float v1);
	void ChangeMaterial(const int textureID, const int blendMode);

	__forceinline void ChangeMaterial(const int textureID, const int blendMode, const unsigned char r, 
									  const unsigned char g, const unsigned char b, const unsigned char a)
	{
		S2DMaterialInfo* const materialInfo = &m_materials[m_textureCount];
		materialInfo->textureOffset = m_vertexCount;
		materialInfo->textureID = textureID;
		materialInfo->engineTextureID = -1;
		materialInfo->blendMode = blendMode;
		materialInfo->r = r;
		materialInfo->g = g;
		materialInfo->b = b;
		materialInfo->a = a;
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
			materialInfo->isMajorDifferenceWithPrevious = prevMaterialInfo->HasMajorDifferenceWith(materialInfo);
			materialInfo->isColorChanged = (prevMaterialInfo->r == r) || (prevMaterialInfo->g == g) ||
					(prevMaterialInfo->b == b) || (prevMaterialInfo->a == a);
		}
		m_textureCount++;
	}

	__forceinline void ChangeMaterialExt(const int textureID, const int engineTextureID, const int blendMode, const unsigned char r, 
									  const unsigned char g, const unsigned char b, const unsigned char a)
	{
		S2DMaterialInfo* const materialInfo = &m_materials[m_textureCount];
		materialInfo->textureOffset = m_vertexCount;
		materialInfo->textureID = textureID;
		materialInfo->engineTextureID = engineTextureID;
		materialInfo->blendMode = blendMode;
		materialInfo->r = r;
		materialInfo->g = g;
		materialInfo->b = b;
		materialInfo->a = a;
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
			materialInfo->isMajorDifferenceWithPrevious = prevMaterialInfo->HasMajorDifferenceWith(materialInfo);
			materialInfo->isColorChanged = (prevMaterialInfo->r == r) || (prevMaterialInfo->g == g) ||
					(prevMaterialInfo->b == b) || (prevMaterialInfo->a == a);
		}
		m_textureCount++;
	}

	void ChangeMaterialExt(const int textureID, const int engineTextureID, const int blendMode);

	__forceinline void ChangeScissors(const int scissorX1, const int scissorY1, const int scissorX2, const int scissorY2)
	{
		m_isScissorsNeeded = true;
		m_scissorX1 = scissorX1;
		m_scissorY1 = scissorY1;
		m_scissorX2 = scissorX2;
		m_scissorY2 = scissorY2;
	}

	__forceinline void SetLineWidth(const unsigned char lineWidth)
	{
		m_lineWidth = lineWidth;
	}


	__forceinline void ChangeSmooth(const bool is_Smooth)
	{
		m_isSmooth = is_Smooth;
	}

	__forceinline void DisableScissors()
	{
		m_isScissorsNeeded = false;
	}

	__forceinline void ChangeOrientParams(const float translationX, const float translationY, const float scaleX, 
										  const float scaleY)
	{
		m_translationX = translationX;
		m_translationY = translationY;
		m_scaleX = scaleX;
		m_scaleY = scaleY;
	}

	__forceinline void SetColor(const unsigned char r, const unsigned char g, const unsigned char b, 
		const unsigned char a)
	{
		m_r = r;
		m_g = g;
		m_b = b;
		m_a = a;
	}

	void Start();
	void End();

	void Draw();

private:
	void DrawRange(const int materialID, const int startOffset, const int endOffset);

	void SetMaterialParams(const S2DMaterialInfo* const materialInfo);

	float m_vx[MAX_VERTEX_COUNT * 2];
	float m_tc[MAX_VERTEX_COUNT * 2];

	S2DMaterialInfo m_materials[MAX_MATERIALS];

	int m_vertexCount;
	int m_textureCount;

	int m_lastTextureID;

	bool m_isTexturingEnabled;

	float* m_vertexOffset;
	float* m_textureCoordsOffset;

	unsigned char m_r, m_g, m_b, m_a;

	bool m_isScissorsNeeded;
	bool m_isSmooth;
	int m_scissorX1, m_scissorY1;
	int m_scissorX2, m_scissorY2;

	float m_translationX, m_translationY;
	float m_scaleX, m_scaleY;

	unsigned char m_lineWidth;
};
#pragma once

class IPolygonsAccum
{
public:
	virtual void AddTextureCoords(const float u1, const float v1) = 0;
	virtual void AddVertex(const float x1, const float y1) = 0;
	virtual void ChangeMaterial(const int textureID, const int blendMode) = 0;
};
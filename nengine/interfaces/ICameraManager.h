#pragma once

class ICameraManager
{
public:
	virtual float GetPointDist(float x, float y, float z) = 0;
	virtual float GetPointDistSq(float x, float y, float z) = 0;
	virtual bool IsPointInFrustum_(float x, float y, float z) = 0;
	virtual bool IsBoundingBoxInFrustum_(float x, float y, float z, float xSize, float ySize, float zSize) = 0;
};
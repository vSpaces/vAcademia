
#pragma once

class CPoint3D
{
public:
	float x, y, z;

	CPoint3D(float xx, float yy, float zz);
	CPoint3D();
	~CPoint3D();

	void Set(float dx, float dy, float dz);
	void Set(CPoint3D& p);
};
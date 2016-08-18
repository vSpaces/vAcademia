
#pragma once

#include "Vector3D.h"

class CVector4D
{
public:
	float x, y, z, w;

public:
	__forceinline CVector4D(float px, float py, float pz, float pw)
	{
		x = px;
		y = py; 
		z = pz; 
		w = pw; 
	}

	__forceinline CVector4D(CVector3D pxyz, float pw)
	{
		x = pxyz.x;
		y = pxyz.y; 
		z = pxyz.z; 
		w = pw; 
	}

	__forceinline CVector4D()
	{
	}

	__forceinline operator float*()
	{
		return &x;
	}

	__forceinline operator const float*()const
	{
		return &x;
	}

	__forceinline ~CVector4D()
	{
	}
};
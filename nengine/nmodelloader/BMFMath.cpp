
#include "StdAfx.h"
#include <Math.h>
#include "BMFMath.h"
#include "Vector3D.h"
#include "GlobalSingletonStorage.h"
#include "../../cal3d/memory_leak.h"

void	AlignFloatToWorld(float& f)
{
	f = floorf(f) + 0.5f;
}

void	AlignVectorToWorld(CVector3D& v)
{
	AlignFloatToWorld(v.x);
	AlignFloatToWorld(v.y);
	AlignFloatToWorld(v.z);
}

void	AlignVectorcToWorld(float& x, float& y, float& z)
{
	AlignFloatToWorld(x);
	AlignFloatToWorld(y);
	AlignFloatToWorld(z);
}

bool IsVectorsCoplanar(const CVector3D& v1, const CVector3D& v2)
{
	CVector3D vtemp1(v1);
	vtemp1.Normalize();
	CVector3D vtemp2(v2);
	vtemp2.Normalize();

	float dot = vtemp1.Dot(vtemp2);

	if ((abs(1.0f - dot) < 1E-7) || (abs(dot + 1.0f) < 1E-7))
	{
		return true;
	}

	return false;
}

void ABGR2ARGBINVERTA(unsigned char* color)
{
	unsigned char temp = color[0];
	color[0] = color[2];
	color[2] = temp;
	color[3] = 255 - color[3];
}

void ClampValue(int& value, int min, int max)
{
	if (value > max)
	{
		value = max;
	}

	if (value < min)
	{
		value = min;
	}
}
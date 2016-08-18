
#include "StdAfx.h"
#include "Point3d.h"

CPoint3D::CPoint3D()
{
	x = y = z = 0;
}

CPoint3D::CPoint3D(float xx, float yy, float zz)
{
	x = xx;
	y = yy;
	z = zz;
}

void CPoint3D::Set(float dx, float dy, float dz)
{
	x = dx;
	y = dy;
	z = dz;
}

void CPoint3D::Set(CPoint3D& p)
{
	x = p.x;
	y = p.y;
	z = p.z;
}

CPoint3D::~CPoint3D()
{
}
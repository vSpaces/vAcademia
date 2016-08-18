// nrmMath3D.cpp: implementation of the nrmMath3D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nrmMath3D.h"
#include "transls.h"
#include "vector.h"
#include "quaternion.h"

nrmMath3D::nrmMath3D()
{

}

void	nrmMath3D::quatMult(ml3DRotation &rot1, ml3DRotation &rot2, ml3DRotation &rotRes)
{
	CalQuaternion	q1((float)rot1.x, (float)rot1.y, (float)rot1.z, (float)rot1.a);
	CalQuaternion	q2((float)rot2.x, (float)rot2.y, (float)rot2.z, (float)rot2.a);
	q1 *= q2;
	QUAT2MLROTATION(q1, rotRes);
}

void	nrmMath3D::quatBlend(ml3DRotation &rot1, ml3DRotation &rot2, double k, ml3DRotation &rotRes)
{
	CalQuaternion	q1((float)rot1.x, (float)rot1.y, (float)rot1.z, (float)rot1.a);
	CalQuaternion	q2((float)rot2.x, (float)rot2.y, (float)rot2.z, (float)rot2.a);
	q1.blend((float)k, q2);
	QUAT2MLROTATION(q1, rotRes);
}

void	nrmMath3D::vec3MultQuat(ml3DPosition &pos, ml3DRotation &rot, ml3DPosition &posRes)
{
	CalVector	v;
	CalQuaternion	q;
	MLPOSITION2VECTOR(pos, v);
	MLROTATION2QUAT(rot, q);
	v *= q;
	VECTOR2MLPOSITION(v, posRes);
}

nrmMath3D::~nrmMath3D()
{

}

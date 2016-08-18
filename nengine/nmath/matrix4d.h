
#pragma once

#include "Vector3D.h"
#include "Vector4D.h"

class CMatrix4D
{
public:
	float m[4][4];

	CMatrix4D();
	CMatrix4D(float val);
	CMatrix4D(const CMatrix4D&);

	CMatrix4D& operator =(const CMatrix4D&);
	CMatrix4D& operator +=(const CMatrix4D&);
	CMatrix4D& operator -=(const CMatrix4D&);
	CMatrix4D& operator *=(const CMatrix4D&);
	CMatrix4D& operator *=(float );
	CMatrix4D& operator /=(float );

	bool Invert();
	CMatrix4D& Transpose();

	CMatrix4D Inverse() const;
	CVector3D TransformPoint(const CVector3D& b) const;
	CVector4D TransformPoint(const CVector4D& b) const;
	CVector3D TransformDirection(const CVector3D& b) const;

	void Translate(const CVector3D& s);
	
	static CMatrix4D Identity();
	static CMatrix4D Scale(const CVector3D& s, float d = 1);
											
	static CMatrix4D RotateX(float);		
	static CMatrix4D RotateY(float);		
	static CMatrix4D RotateZ(float);		
	static CMatrix4D Rotate(const CVector3D& v, float angle);
	static CMatrix4D Rotate(float yaw, float pitch, float roll);
							
	static CMatrix4D MirrorX();
	static CMatrix4D MirrorY();
	static CMatrix4D MirrorZ();

	friend CMatrix4D operator +(const CMatrix4D&, const CMatrix4D&);
	friend CMatrix4D operator -(const CMatrix4D&, const CMatrix4D&);
	friend CMatrix4D operator *(const CMatrix4D&, const CMatrix4D&);
	friend CMatrix4D operator *(const CMatrix4D&, float);
	friend CMatrix4D operator *(float, const CMatrix4D&);
	friend CVector3D operator *(const CMatrix4D&, const CVector3D&);
};


#pragma once

class CVector2D
{
public:
	float x, y;

	CVector2D(float xx, float yy);
	CVector2D();
	~CVector2D();

	void Set(float dx, float dy);
	void Set(CVector2D& p);
	// normalize vector
	void Normalize();

	// return result of dot multiplication
	float Dot(CVector2D b);

	void Rotate(float angle);

	CVector2D operator -(CVector2D a);
	CVector2D operator +(CVector2D a);

	operator float*()
	{
		return &x;
	}

	operator const float*()const
	{
		return &x;
	}

	float GetLength();
};
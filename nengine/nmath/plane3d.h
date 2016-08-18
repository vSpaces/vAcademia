
#pragma once

#include <float.h>
#include "Vector3D.h"

// ошибка дл€ рассто€ни€ до плоскости
#define		COPLANAR_PL_DIST	0.1f					

// ошибка дл€ скал€рного произведелни€ нормалей
#define		COPLANAR_PL_DOT		0.01f					
#define		COPLANAR_PL_DOTm1	(1.0f - COPLANAR_PL_DOT)

class CPlane3D
{
public:
	CPlane3D(): 
		m_d(0),
		m_normal(0, 0, 1)
	{ 
	}
	
	CPlane3D(CVector3D n, float dist): 
		m_normal(n),
		m_d(dist)
	{
	}

	CPlane3D(CVector3D n, CVector3D v): 
		m_normal(n)
	{
		m_d = m_normal.Dot(v);
	}

	CPlane3D(CVector3D v1, CVector3D v2, CVector3D v3)
	{ 
		CVector3D e1, e2;
		e1 = v2 - v1;
		e2 = v3 - v1;
		m_normal = e1.Cross(e2);

		if (m_normal.GetLength() != 0.0f)
		{
			m_normal.Normalize();
		}
		
		m_d = v1.Dot(m_normal);
	}

	CPlane3D(const CPlane3D& in): 
		m_normal(in.m_normal), 
		m_d(in.m_d) 
	{
	}

	void operator =(const CPlane3D& in) 
	{ 
		m_normal = in.m_normal;
		m_d = in.m_d;
	}

	float GetDistance(CVector3D& v)
	{ 
		if (m_normal.GetLength() == 0.0f)
		{
			return FLT_MAX;
		}

		return m_normal.Dot(v) - m_d; 
	}

	bool IsCoplanar(CPlane3D& p)
	{
		float dot = m_normal.Dot(p.m_normal);
		return ((dot > COPLANAR_PL_DOTm1) && ((float)fabs(m_d - p.m_d) < COPLANAR_PL_DIST));
	}

	CVector3D GetProjection(CVector3D v)
	{
		float d = GetDistance(v);
		CVector3D in = m_normal;
		v -= in*d;
		return v;
	}

	bool GetIntersection(CVector3D origin, CVector3D delta, CVector3D& res)
	{
		float divKoef = (delta.x * m_normal.x + delta.y * m_normal.y + delta.z * m_normal.z);
		if (0.0f == divKoef)
		{
			return false;
		}
		
		float koef = (m_d - origin.x * m_normal.x - origin.y * m_normal.y - origin.z * m_normal.z) / divKoef;
		res = origin + delta * koef;

		if (koef < 0)
		{
			return false; 
		}

		return true;
	}

private:
	CVector3D m_normal;	
	float m_d;			
};

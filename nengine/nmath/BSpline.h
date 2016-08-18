
#pragma once

#pragma  warning (disable: 4702)

#include "CommonMathHeader.h"

#include "BSSegment.h"
#include "Vector3D.h"
#include "BSPoint.h"
#include <vector>

#define DEFAULT_BSPLINE_TANGENT		CVector3D(1, 0, 0)
#define DEFAULT_BSPLINE_NORMAL		CVector3D(0, 1, 0)
#define DEFAULT_BSPLINE_POSITION	CVector3D(0, 0, 0)

#define LENGTH_STEPS_PER_SEGMENT	100.0f
#define LENGTH_STEPS_INCREMENT		1.0f / LENGTH_STEPS_PER_SEGMENT

class CBSpline
{
public:
	CBSpline();
	CBSpline(const CBSpline &right);
	~CBSpline();

public:	
	void		Clear();

	void		AddPoint(const CVector3D& p1,const CVector3D& p2,const CVector3D& p3);
	void		AddPoint(CBSCPoint p, int index = -1);
	void		DeletePoint(unsigned int index);

	int			GetCount();
	float		GetLength();

	CVector3D	GetPosition(float t = 0);
	CVector3D	GetLinearPosition(float t = 0);
	CVector3D	GetTangent(float t);
	CVector3D	GetLinearTangent(float t);

	int			GetSegmentID(float t, float* st_l);

	CBSCPoint*	GetPoint(unsigned int index);

private:
	const MP_VECTOR<CBSCPoint*>& GetPoints()const;

	MP_VECTOR<CBSCSegment*> m_segments;
	float	m_length;	
	MP_VECTOR<CBSCPoint*> m_points;
};

__forceinline const MP_VECTOR<CBSCPoint*>& CBSpline::GetPoints()const
{
	return m_points;
}

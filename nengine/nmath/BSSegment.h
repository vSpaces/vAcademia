
#pragma once

#include "CommonMathHeader.h"

#pragma  warning (disable: 4702)

#include "BSPoint.h"
#include "Vector3D.h"
#include <vector>

#define	LINEAR_APPROXIMATION_COUNT	100

class CBSCSegment  
{
public:
	CBSCSegment();
	virtual ~CBSCSegment();

	CVector3D GetLinearPoint(float t);
	CVector3D GetPoint(float t);
	CVector3D GetTangent(float t);
	CVector3D GetLinearTangent(float t);
	double	  GetLength();
	void Create(CBSCPoint* p1, CBSCPoint* p2);

protected:
	void	CreateLinearPoints();

	MP_VECTOR<double>	m_linearPoints;
	double				m_linearLength;
	CBSCPoint*			m_start;
	CBSCPoint*			m_end;
};

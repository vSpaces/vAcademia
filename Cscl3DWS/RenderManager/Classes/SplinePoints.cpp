#include "stdafx.h"
#include ".\splinepoints.h"
#include "../nmath/BSpline.h"

//////////////////////////////////////////////////////////////////////////

_BoundaryPoints::_BoundaryPoints():
	MP_VECTOR_INIT(boundarySplinePoints)
{
}

CSplinePoints::CSplinePoints():
	MP_VECTOR_INIT(m_splinePoints)
{

}

CSplinePoints::~CSplinePoints()
{
	m_splinePoints.clear();
}

void CSplinePoints::initialize( CBSpline *pSpline, float aExact /* = 0.01f */)
{	
	int count = 1 / aExact;
	CVector3D vec = pSpline->GetLinearPosition( 0);
	minX.x = vec.x;
	maxX.x = vec.x;
	minY.y = vec.y;
	maxY.y = vec.y;
	add( vec);
	int minXNumber = 0;
	int minYNumber = 0;
	int maxXNumber = 0;
	int maxYNumber = 0;

	for ( int i = 1; i < count; i++)
	{
		float t = 0.01f * i;
		CVector3D vec = pSpline->GetLinearPosition( t);
		if ( vec.x < minX.x)
		{
			minX = vec;
			minXNumber = i;
		}
		if ( vec.x > maxX.x)
		{
			maxX = vec;
			maxXNumber = i;
		}
		if ( vec.y < minY.y)
		{
			minY = vec;
			minYNumber = i;
		}
		if ( vec.y > maxY.y)
		{
			maxY = vec;
			maxYNumber = i;
		}
		add( vec);
	}
	center.x = (minX.x + maxX.x) / 2;
	center.y = (minY.x + maxY.x) / 2;
	width = maxX.x - minX.x;
	height = maxY.x - minY.x;	
}

void CSplinePoints::add( CVector3D point)
{
	m_splinePoints.push_back( point);
}

BoundaryPoints *CSplinePoints::getBoundaryPoints( CVector3D avPos, float radius)
{
	m_boundaryPoints.boundarySplinePoints.clear();
	m_boundaryPoints.minPoint = CVector3D( 0.0f, 0.0f, 0.0f);
	m_boundaryPoints.bSmallRegion = false;

	if ( radius > width && radius > height)		
		m_boundaryPoints.bSmallRegion = true;

	int rad = radius * radius;
	int minDist = radius * radius;

	CSplinePoints::m_splinePointsIter vpointit = m_splinePoints.begin(),		
		vpointend = m_splinePoints.end();

	for ( ; vpointit != vpointend; vpointit++)
	{
		CVector3D point = *vpointit;
		CVector3D dist = point - avPos;
		float len = dist.GetLengthSq();
		if ( rad > len)
		{
			m_boundaryPoints.boundarySplinePoints.push_back( point);			
		}
		if ( minDist > len)
		{
			m_boundaryPoints.minPoint = point;
			minDist = len;
		}
	}
	
	return &m_boundaryPoints;
}

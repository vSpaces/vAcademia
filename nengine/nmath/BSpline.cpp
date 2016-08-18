
#include "stdafx.h"
#include <Assert.h>
#include "BSpline.h"
#include "BSPoint.h"
#include "BSSegment.h"
#include "GlobalSingletonStorage.h"

CBSpline::CBSpline():
	m_length(0),
	MP_VECTOR_INIT(m_segments),	
	MP_VECTOR_INIT(m_points)
{
}

CBSpline::CBSpline(const CBSpline &right):
	MP_VECTOR_INIT(m_segments),	
	MP_VECTOR_INIT(m_points)
{
	m_length = right.m_length;
}

void CBSpline::AddPoint(CBSCPoint p, int index)
{
	assert(-1 == index);	

	CBSCPoint*	new_point = MP_NEW(CBSCPoint);
	*new_point = p;

	index = m_points.size();
	m_points.push_back(new_point);

	if (m_points.size() > 1) 
	{	
		// it`s not first point, so add new segment
		CBSCSegment* newSegment = MP_NEW(CBSCSegment);
		newSegment->Create(m_points[m_points.size() - 2], m_points[m_points.size() - 1]);
		m_segments.push_back(newSegment);
		m_length += (float)newSegment->GetLength();
	}
}

void	CBSpline::AddPoint(const CVector3D& p1,const CVector3D& p2,const CVector3D& p3)
{
	AddPoint(CBSCPoint(p1, p2, p3));
}

int CBSpline::GetCount()
{
  return m_points.size();
}

void CBSpline::DeletePoint(unsigned int index)
{
	assert(index < m_points.size());
	m_points.erase(m_points.begin() + index);
}

CVector3D CBSpline::GetPosition(float t)
{
	float segmentStartLength;

	int segmentID = GetSegmentID(t, &segmentStartLength);
	if (-1 == segmentID)	
	{
		return DEFAULT_BSPLINE_POSITION;
	}

	CBSCSegment* segment = m_segments[segmentID];
	assert(segment);

	double segmentLength = segment->GetLength();
	if (0 == segmentLength)	
	{
		return DEFAULT_BSPLINE_POSITION;
	}

	return segment->GetPoint(segmentStartLength / (float)segmentLength);
}

CVector3D CBSpline::GetLinearPosition(float t)
{
	float segmentStartLength;
	int segmentID = GetSegmentID(t, &segmentStartLength);
	if (-1 == segmentID)	
	{
		return DEFAULT_BSPLINE_POSITION;
	}

	CBSCSegment* segment = m_segments[segmentID];
	assert(segment);

	double segmentLength = segment->GetLength();
	if (0 == segmentLength)	
	{
		return DEFAULT_BSPLINE_POSITION;
	}

	return segment->GetLinearPoint(segmentStartLength / (float)segmentLength);
}

CBSCPoint* CBSpline::GetPoint(unsigned int index)
{
	assert(index < m_points.size());
	return	m_points[index];
}

int CBSpline::GetSegmentID(float t, float* st_l)
{
	float lst = 0;
	float l2find = m_length * t;

	for (unsigned int i = 0; i < m_segments.size(); i++) 
	{
		float nl = lst + (float)m_segments[i]->GetLength();

		if (nl >= l2find - 0.01)
		{
			if (st_l)	
			{
				*st_l = l2find - lst;
			}

			return i;
		}

		lst += (float)m_segments[i]->GetLength();
	}

	return -1;
}

CVector3D CBSpline::GetTangent(float t)
{
	float segmentStartLength;
	int segmentID = GetSegmentID(t, &segmentStartLength);
	if (segmentID == -1)
	{
		return DEFAULT_BSPLINE_TANGENT;
	}
	CBSCSegment* segment = m_segments[segmentID];
	double segmentLength = segment->GetLength();
	if (0 == segmentLength)	
	{
		return DEFAULT_BSPLINE_TANGENT;
	}

	CVector3D v = segment->GetTangent(segmentStartLength / (float)segmentLength);
	v.Normalize();
	return v;
}

CVector3D CBSpline::GetLinearTangent(float t)
{
	float segmentStartLength;
	int segmentID = GetSegmentID(t, &segmentStartLength);
	if (-1 == segmentID)	
	{
		return DEFAULT_BSPLINE_TANGENT;
	}

	CBSCSegment* segment = m_segments[segmentID];
	double segmentLength = segment->GetLength();
	if (0 == segmentLength)
	{
		return DEFAULT_BSPLINE_TANGENT;
	}

	CVector3D v = segment->GetTangent(segmentStartLength / (float)segmentLength);
	v.Normalize();
	return v;
}

float CBSpline::GetLength()
{
	if (m_length == -1)
	{
		// calculate length
		m_length = 0;
		
		MP_VECTOR<CBSCSegment*>::iterator	is;
		for (is = m_segments.begin(); is != m_segments.end(); is++) 
		{
			m_length += (float)((*is)->GetLength());
		}
	}

	return m_length;
}

void CBSpline::Clear()
{
	MP_VECTOR<CBSCSegment*>::iterator	sit;
	for (sit = m_segments.begin(); sit != m_segments.end(); sit++)
	{
		MP_DELETE(*sit);
	}

	MP_VECTOR<CBSCPoint*>::iterator pit;
	for (pit = m_points.begin(); pit != m_points.end(); pit++)
	{
		MP_DELETE(*pit);
	}

	m_points.clear();
	m_segments.clear();

	m_length = -1;
}

CBSpline::~CBSpline()
{
	Clear();
}

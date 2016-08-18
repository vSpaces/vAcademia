
#pragma once

#include "CommonMathHeader.h"

#pragma  warning (disable: 4702)

#include "Vector3D.h"
#include "Queue.h"
#include "UserData.h"

#include <vector>

#define DEFAULT_PATHLINE_POSITION	CVector3D(0, 0, 0)

typedef struct _SPointsInPath
{
	CVector3D pnt;

	SSegment* segment1;
	SSegment* segment2;

	_SPointsInPath(CVector3D _pnt, SSegment* _segment1, SSegment* _segment2)
	{
		pnt = _pnt;

		segment1 = _segment1;
		segment2 = _segment2;
	}

} SPointsInPath;

class CPathLine
{
public:
	CPathLine();
	~CPathLine();

public:	
	void		Clear();

	void		AddPoint(const CVector3D& p, SSegment* segment1, SSegment* segment2);
	float		GetLength();

	CVector3D	GetPosition(float t = 0);
	CVector3D	GetLinearPosition(float t = 0);
	CVector3D	GetTangent(float t);

	unsigned int GetPointCount()const;
	CVector3D GetPoint(const unsigned int index);


	SSegment* GetFirstSegment(unsigned int index);
	SSegment* GetSecondSegment(unsigned int index);

	bool MarkSegmentIfNeeded(float t);

	void LogControlPoint(unsigned int index);

private:
	float	m_length;	

	MP_VECTOR<SPointsInPath*> m_points;

	__forceinline float GetPointDist(const CVector3D p1, const CVector3D& p2)
	{
		return sqrtf((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y) + (p2.z - p1.z) * (p2.z - p1.z));
	}
};



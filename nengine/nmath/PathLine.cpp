
#include "stdafx.h"
#include <Assert.h>
#include "PathLine.h"
#include "GlobalSingletonStorage.h"

CPathLine::CPathLine():
	m_length(0),
	MP_VECTOR_INIT(m_points)
{
}

void CPathLine::AddPoint(const CVector3D& pnt, SSegment* segment1, SSegment* segment2)
{	
	MP_NEW_P3(pip, SPointsInPath, pnt, segment1, segment2);
	m_points.push_back(pip);

	if (m_points.size() > 1) 
	{	
		m_length += GetPointDist(m_points[m_points.size() - 2]->pnt, pnt);
	}
}


CVector3D CPathLine::GetLinearPosition(float t)
{
	if (t > 1)
	{
		t = 1;
	}
	else if (t < 0)
	{
		t = 0;
	}

	if (m_points.size() == 0)
	{
		return DEFAULT_PATHLINE_POSITION;
	}

	float lst = 0;
	float l2find = m_length * t; 

	for (unsigned int i = 0; i < m_points.size()-1; i++) 
	{
		float length = GetPointDist(m_points[i]->pnt,m_points[i+1]->pnt);

		if (0 == length	)
		{
			return DEFAULT_PATHLINE_POSITION;
		}

		float nl = lst + length;

		if (nl >= l2find - 0.01)
		{
			float part;
			part = (l2find - lst)/length;

			CVector3D res  = m_points[i]->pnt*(1-part) + m_points[i+1]->pnt*part;
			return res; 
		}

		lst += length;
	}

	return DEFAULT_PATHLINE_POSITION;
}


CVector3D CPathLine::GetTangent(float t)
{
	if (m_points.size() == 0)
	{
		return DEFAULT_PATHLINE_POSITION;
	}

	if (t > 1)
	{
		t = 1;
	}
	else if (t < 0)
	{
		t = 0;
	}

	float lst = 0;
	float l2find = m_length * t;

	for (unsigned int i = 0; i < m_points.size()-1; i++) 
	{
		float length = GetPointDist(m_points[i]->pnt,m_points[i+1]->pnt);

		if (0 == length)
		{
			return DEFAULT_PATHLINE_POSITION;
		}

		float nl = lst + length;

		if (nl >= l2find - 0.01)
		{
			CVector3D v = m_points[i+1]->pnt - m_points[i]->pnt;
			v.Normalize();
			return v;
		}

		lst += length;
	}

	return DEFAULT_PATHLINE_POSITION;
}



float CPathLine::GetLength()
{
	if ((m_length == -1) && (m_points.size() > 0))
	{
		// calculate length
		m_length = 0;
		for (unsigned int i = 0; i < m_points.size()-1; i++) 
		{
			m_length += GetPointDist(m_points[i]->pnt,m_points[i+1]->pnt);
		}
	}

	return m_length;
}

void CPathLine::Clear()
{
	MP_VECTOR<SPointsInPath*>::iterator it;
	MP_VECTOR<SPointsInPath*>::iterator itEnd = m_points.end();

	for (it = m_points.begin(); it != itEnd; it++)
	{
		//m_points.erase(it);
		MP_DELETE(*it);
	}

	m_points.clear();

	m_length = -1;
}

unsigned int CPathLine::GetPointCount()const
{
	return m_points.size();
}

CVector3D CPathLine::GetPoint(const unsigned int index)
{
	if (index < m_points.size())
	{
		return m_points[index]->pnt;
	}
	else
	{
		return CVector3D(0, 0, 0);
	}
}

void CPathLine::LogControlPoint(unsigned int index)
{
	if (m_points.size() > index)
	{
		SPointsInPath* pnt = m_points[index];
		
		g->lw.WriteLn("Control point: ", pnt->pnt.x, " ", pnt->pnt.y, " ", pnt->pnt.z);
		if (pnt->segment1)
		{
			g->lw.WriteLn("Left segment: ", g->o3dm.GetObjectPointer(pnt->segment1->groupId)->GetName(), " groupID: ", pnt->segment1->groupId);
			if (pnt->segment1->groupId < 0)
			{
				g->pf.LogDynamicObject(pnt->segment1->groupId);
			}
			g->lw.WriteLn("Start: ", pnt->segment1->begin.x, " ", pnt->segment1->begin.y, " ", pnt->segment1->begin.z);
			g->lw.WriteLn("End: ", pnt->segment1->end.x, " ", pnt->segment1->end.y, " ", pnt->segment1->end.z);
			g->lw.WriteLn("active: ", pnt->segment1->active, " type: ", pnt->segment1->typeOfSegment, " length: ", pnt->segment1->length);
			
		}
		if (pnt->segment2)
		{
			g->lw.WriteLn("Right segment: ", g->o3dm.GetObjectPointer(pnt->segment2->groupId)->GetName(), " groupID: ", pnt->segment2->groupId);
			if (pnt->segment2->groupId < 0)
			{
				g->pf.LogDynamicObject(pnt->segment2->groupId);
			}
			g->lw.WriteLn("Start: ", pnt->segment2->begin.x, " ", pnt->segment2->begin.y, " ", pnt->segment2->begin.z);
			g->lw.WriteLn("End: ", pnt->segment2->end.x, " ", pnt->segment2->end.y, " ", pnt->segment2->end.z);
			g->lw.WriteLn("active: ", pnt->segment2->active, " type: ", pnt->segment2->typeOfSegment, " length: ", pnt->segment2->length);
		}
	}
}

bool CPathLine::MarkSegmentIfNeeded(float t)
{
	if (m_points.size() <= 2) 
		return false;

	float startLength = 0;
	float findLength = m_length * t;

	for (unsigned int i = 0; i < m_points.size()-1; i++) 
	{
		float length = GetPointDist(m_points[i]->pnt, m_points[i+1]->pnt);

		float totalLength = startLength + length;

		if (totalLength >= findLength - 0.01)
		{
			if (((m_points[i]->segment1) && (!m_points[i]->segment2)) && (i < m_points.size()-2))
			{
				i++;
			}
			else if (((m_points[i]->segment2) && (!m_points[i]->segment1)) && (i < m_points.size()-2))
			{
				i++;
			}

			bool isMarked = false;

			g->pf.FindPointsSegments(m_points[i]);

			if  ( (m_points[i]->segment1) && 
				( ((m_points[i+1]->segment1) && (m_points[i]->segment1 == m_points[i+1]->segment1))  
				|| ((m_points[i+1]->segment2) && (m_points[i]->segment1 == m_points[i+1]->segment2))))
			{
				C3DObject* obj = g->o3dm.GetObjectPointer(m_points[i]->segment1->groupId);

				if	((obj->GetUserData(USER_DATA_EDITABLE)) || (obj->GetAnimation()))
				{
					m_points[i]->segment1->active = false; 
					isMarked = true;
				}
			}
			else if ( (m_points[i]->segment2) && 
					( ((m_points[i+1]->segment1) && (m_points[i]->segment2 == m_points[i+1]->segment1))  
					|| ((m_points[i+1]->segment2) && (m_points[i]->segment2 == m_points[i+1]->segment2))))
			{
				C3DObject* obj = g->o3dm.GetObjectPointer(m_points[i]->segment2->groupId);

				if	( (obj->GetUserData(USER_DATA_EDITABLE)) || (obj->GetAnimation()))
				{
					m_points[i]->segment2->active = false; 
					isMarked = true;
				}
			}

			return isMarked;
		}

		startLength += length;
	}

	return false;
}

SSegment* CPathLine::GetFirstSegment(unsigned int index)
{
	if (index < m_points.size())
	{
		return m_points[index]->segment1;
	}
	else
	{
		return NULL;
	}
}

SSegment* CPathLine::GetSecondSegment(unsigned int index)
{
	if (index < m_points.size())
	{
		return m_points[index]->segment2;
	}
	else
	{
		return NULL;
	}
}

CPathLine::~CPathLine()
{
	Clear();
}

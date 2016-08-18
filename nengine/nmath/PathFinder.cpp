#include "stdafx.h"
#include "PathFinder.h"
#include "GlobalSingletonStorage.h"
#include <math.h>

#define MAX_DIST_TO_PATH		50
#define PATH_EDIT_LENGTH		50
#define PATH_ADD_HEGHT			3
#define MIN_INDENT				40
#define PART_COUNT				10
#define QUADR_INDENT			20

#define AVATAR_HALF_HEIGHT		100
#define AVATAR_HALF_WIDTH		60
#define	AVATAR_HALF_LENGTH		60	

#define	AVATAR		0 
#define	DINAMIC		1	

#define CONST_X		true

#define LEFT  1  /* двоичное 0001 */
#define RIGHT 2  /* двоичное 0010 */
#define BOT   4  /* двоичное 0100 */
#define TOP   8  /* двоичное 1000 */

/* вычисление кода точки
r : указатель на bounding box пути; p : указатель на точку отрезка */
#define CODE_POINT(r, p) \
	((((p)->x < (r)->min.x) ? LEFT : 0)  +  /* +1 если точка левее прямоугольника */ \
	(((p)->x > (r)->max.x) ? RIGHT : 0) +  /* +2 если точка правее прямоугольника */\
	(((p)->y < (r)->min.y) ? BOT : 0)   +  /* +4 если точка ниже прямоугольника */  \
	(((p)->y > (r)->max.y) ? TOP : 0))     /* +8 если точка выше прямоугольника */

#define FIND_OBJECTS_SEGMENTS_COMMON(id, action) MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itV = m_segments.find( id );	\
	std::vector<SSegment*>::iterator it; \
	std::vector<SSegment*>::iterator itEnd; \
	if (itV != m_segments.end())\
	{	\
	 it = (*itV).second.begin();	\
	 itEnd = (*itV).second.end();	\
	}	\
	else\
	{ \
		action ; \
	} \

#define FIND_OBJECTS_SEGMENTS_OR_RETURN(id)  FIND_OBJECTS_SEGMENTS_COMMON(id, return)
#define FIND_OBJECTS_SEGMENTS_OR_RETURN_FALSE(id) FIND_OBJECTS_SEGMENTS_COMMON(id, return false)
#define FIND_OBJECTS_SEGMENTS_OR_CONTINUE(id)  FIND_OBJECTS_SEGMENTS_COMMON(id, continue)
#define FIND_OBJECTS_SEGMENTS(id) FIND_OBJECTS_SEGMENTS_COMMON(id, )

#define CHANGE_IF_NEDEED_OBJECT_PATHES_BB(id) std::map<int, PathesBB>::iterator it2 = m_pathesBB.find( id ); \
	if (it2 != m_pathesBB.end())	\
	{	\
		ComparePointWithPathesBB(&(*it)->begin, &(*it2).second);	\
		ComparePointWithPathesBB(&(*it)->end, &(*it2).second);	\
	}	\

std::vector<SSegment*> m_segmentsInQueue; 

C3DObject* GetObjectPointer(int id)
{
	if ( id < 0)
	{
		id = -id;
	}

	return   g->o3dm.GetObjectPointer(id);
}


CPathFinder::CPathFinder():
	m_path(NULL),
	m_currentPoint(0,0,0),	
	m_currentObjectID(-1),
	m_isShowNearest(false),
	m_isDebugRenderingEnabled(false),
	m_firstPoint(0,0,0),
	m_isMoveToSegment(false),
	m_isShowNearestAfterChangePath(false),
	m_currentSegment(NULL),
	MP_VECTOR_INIT(m_dinamicObjects),
	MP_VECTOR_INIT(m_mergeDinamicObjects),
	MP_VECTOR_INIT(m_currentSegments),
	MP_VECTOR_INIT(m_nearestPathes),
	MP_MAP_INIT(m_provisionalDinamicObject),
	MP_MAP_INIT(m_inactiveDinamicObjects),
	MP_MAP_INIT(m_pathLinesByModelID),
	MP_VECTOR_INIT(m_intersectionPoints),
	MP_VECTOR_INIT(m_pathPoints),
	MP_MAP_INIT(m_segments),
	MP_MAP_INIT(m_pathesBB)
{
	for (int i = 0; i < 360; i++)
	{
		m_sinTable[i] = sinf(i * (float)M_PI / 180.0f);
		m_cosTable[i] = cosf(i * (float)M_PI / 180.0f);
	}
}

void CPathFinder::EnableDebugRendering(bool isEnabled)
{
	m_isDebugRenderingEnabled = isEnabled;
}

void CPathFinder::Init()
{
	static bool isDrawingInitializated = false;
	if (!isDrawingInitializated)
	{
		if (g->scm.GetActiveScene())
		{
			g->scm.GetActiveScene()->AddSceneObject(this);
			SetRenderPriority(-1000);
			isDrawingInitializated = true;
		}
	}

	g->mm.AddPropertyBlockLoadHandler("pathlines.bin", this);
}

bool CPathFinder::IsPointInPoly(std::vector<SSegment*>* v, float x, float y )
{
	std::vector<CVector3D*> uniquePoints;

	std::vector<SSegment*>::iterator it = v->begin();
	std::vector<SSegment*>::iterator itEnd = v->end();

	CVector3D curPoint = CVector3D(0,0,0);
	CVector3D prevPoint = CVector3D(0,0,0);
	CVector3D pnt = CVector3D(0,0,0);

	/*if (it!= itEnd)
	{
	curPoint = (*it)->begin;
	prevPoint = (*it)->end;
	pnt = (*it)->end;

	//uniquePoints.push_back(&(*it)->end);
	uniquePoints.push_back(&(*it)->begin);
	it++;
	}

	while (curPoint != pnt)
	{
	if ( ((*it)->begin == curPoint) && ((*it)->end!= prevPoint))
	{
	prevPoint = curPoint;
	curPoint = (*it)->end;

	uniquePoints.push_back(&curPoint);
	}
	else if (((*it)->end == curPoint)&& ((*it)->begin != prevPoint))
	{
	prevPoint = curPoint;
	curPoint = (*it)->begin;

	uniquePoints.push_back(&curPoint);
	}

	it++;

	if (it == itEnd)
	{
	it = v->begin();
	}
	*/

	for (; it!=itEnd; it++)
	{
		std::vector<CVector3D*>::iterator itp = uniquePoints.begin();
		std::vector<CVector3D*>::iterator itpEnd = uniquePoints.end();

		bool isFoundBegin = false;
		bool isFoundEnd = false;

		for ( ; itp != itpEnd; itp++)
		{
			if (*(*itp) == (*it)->begin)
			{
				isFoundBegin = true;
			}

			if (*(*itp) == (*it)->end)
			{
				isFoundEnd = true;
			}
		}

		if (!isFoundBegin)
		{
			uniquePoints.push_back(&(*it)->begin);
		}

		if (!isFoundEnd)
		{
			uniquePoints.push_back(&(*it)->end);
		}			
	}

	float* xp = (float*)_alloca(sizeof(float) * uniquePoints.size());
	float* yp = (float*)_alloca(sizeof(float) * uniquePoints.size());

	std::vector<CVector3D*>::iterator itp = uniquePoints.begin();
	std::vector<CVector3D*>::iterator itpEnd = uniquePoints.end();

	for (int i = 0; itp != itpEnd; itp++, i++)
	{
		xp[i] = (*itp)->x;
		yp[i] = (*itp)->y;
	}

	return IsPointInPoly(uniquePoints.size(), xp, yp, x, y);
}

bool CPathFinder::SetCurrentObjectID(int id)
{
	if (id != m_currentObjectID)
	{
		m_firstPoint = CVector3D(0,0,0);
		m_currentPoint = CVector3D(0,0,0);
		m_currentObjectID= id;
		return true;
	}

	return false;
}

void CPathFinder::SetVisible(bool isShow)
{
	m_isShowNearest = isShow;

	FindNearestPathes();
}

void CPathFinder::FindNearestPathes()
{
	m_nearestPathes.clear();
	m_nearestPathes.reserve(m_segments.size());

	m_eyePosition = g->cm.GetActiveCamera()->GetEyePosition();

	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator it = m_segments.begin();
	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itEnd = m_segments.end();	

	for ( ; it != itEnd; it++)
	{
		std::vector<SSegment*>::iterator itV = (*it).second.begin();
		std::vector<SSegment*>::iterator itVEnd =(*it).second.end();

		for ( ; itV != itVEnd; itV++)
		{
			float dist = g->cm.GetPointDistSq((*itV)->begin.x, (*itV)->begin.y, (*itV)->begin.z);
			dist = g->cm.GetPointDistSq((*itV)->end.x, (*itV)->end.y, (*itV)->end.z);
			if ((g->cm.GetPointDistSq((*itV)->begin.x, (*itV)->begin.y, (*itV)->begin.z) <= 25000000)||
				(g->cm.GetPointDistSq((*itV)->end.x, (*itV)->end.y, (*itV)->end.z) <= 25000000) )
			{
				m_nearestPathes.push_back(*itV);
			}
		}
	}
}

void CPathFinder::AddPointToPath(const CVector3D& point, SSegment* segment1, SSegment* segment2)
{
	m_pathPoints.push_back(point);
	if (m_path)
	{
		m_path->AddPoint(point, segment1, segment2);	
	}
}

void CPathFinder::AddSegment(const CVector3D firstPoint, const CVector3D secondPoint, int id,  unsigned char typeOfSegment)
{
	static bool isDrawingInitializated = false;

	m_isShowNearestAfterChangePath = true;

	if (!isDrawingInitializated)
	{
		if (g->scm.GetActiveScene())
		{
			g->scm.GetActiveScene()->AddSceneObject(this);
			SetRenderPriority(-1000);
			isDrawingInitializated = true;
		}
	}

	bool isNeedToAdd = true;

	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator it = m_segments.begin();
	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itEnd = m_segments.end();

	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itV = m_segments.find(id);

	if (itV != m_segments.end())
	{
		std::vector<SSegment*>::iterator it = (*itV).second.begin();
		std::vector<SSegment*>::iterator itEnd = (*itV).second.end();

		for ( ; it != itEnd; it++)
		if (((((*it)->begin == firstPoint) && ((*it)->end == secondPoint)) ||
			(((*it)->end == firstPoint) && ((*it)->begin == secondPoint))))
		{
			isNeedToAdd = false;
			break;
		}

	}
	else
	{
		std::vector<SSegment*> v;

		MP_NEW_P8(_segm, SSegment, firstPoint.x, firstPoint.y, firstPoint.z, secondPoint.x, secondPoint.y, secondPoint.z, id, typeOfSegment);
		v.push_back(_segm);

		float minX, maxX, minY, maxY;

		if (firstPoint.x < secondPoint.x)
		{
			minX = firstPoint.x;
			maxX = secondPoint.x;
		}
		else
		{
			minX = secondPoint.x;
			maxX = firstPoint.x;
		}

		if (firstPoint.y < secondPoint.y)
		{
			minY = firstPoint.y;
			maxY = secondPoint.y;
		}
		else
		{
			minY = secondPoint.y;
			maxY = firstPoint.y;
		}

		PathesBB vBB;
		vBB.min = CVector3D(minX, minY, firstPoint.z);
		vBB.max = CVector3D(maxX, maxY, secondPoint.z); 
		m_pathesBB.insert(std::map<int, PathesBB>::value_type(id, vBB));

		isNeedToAdd = false;

		//CheckInactiveObjectsInMainSegments();

		InsertInMainSegments(id, v);

		//m_segments.insert(MP_MAP<int, MP_VECTOR<SSegment*> >::value_type(id, v));

		//CheckInactiveObjectsInMainSegments();
	}

	if (isNeedToAdd)
	{
		std::map<int, PathesBB>::iterator itBB = m_pathesBB.find((*itV).first);	

		if (itBB != m_pathesBB.end())
		{
			ComparePointWithPathesBB(&firstPoint, &(*itBB).second);

			ComparePointWithPathesBB(&secondPoint, &(*itBB).second);
		}

		MP_NEW_P8(_segm, SSegment, firstPoint.x, firstPoint.y, firstPoint.z , secondPoint.x, secondPoint.y, secondPoint.z , id, typeOfSegment);
		(*itV).second.push_back(_segm);
	}
	m_currentPoint = secondPoint;
}

void CPathFinder::ComparePointWithPathesBB(const CVector3D* pnt, PathesBB* pathesBB)
{
	if  (pathesBB->min.x > pnt->x)
	{
		pathesBB->min.x = pnt->x;
	}
	else if (pathesBB->max.x < pnt->x)
	{
		pathesBB->max.x = pnt->x;
	}

	if  (pathesBB->min.y > pnt->y)
	{
		pathesBB->min.y= pnt->y;
	}
	else if (pathesBB->max.y < pnt->y)
	{
		pathesBB->max.y = pnt->y;
	}
}

bool CPathFinder::DeleteOrSelect3DObjectsSegment(int mouseX, int mouseY, CViewport* const viewport, CVector3D* vBegin, CVector3D* vEnd)
{
	FIND_OBJECTS_SEGMENTS_OR_RETURN_FALSE(m_currentObjectID);

	for ( ; it != itEnd; it++)
	{	
		CVector2D p1, p2;

		viewport->GetScreenCoords((*it)->begin.x, (*it)->begin.y, (*it)->begin.z + PATH_ADD_HEGHT, &p1.x, &p1.y);
		viewport->GetScreenCoords((*it)->end.x, (*it)->end.y, (*it)->end.z + PATH_ADD_HEGHT, &p2.x, &p2.y);

		float eps = 1; 

		float  qx[4], qy[4];

		if  (abs(p2.x-p1.x) > eps)
		{
			CalculateQuadrPoint(0, qx, qy, &p1, &p2);
			CalculateQuadrPoint(1, qx, qy, &p1, &p2);
			CalculateQuadrPoint(2, qx, qy, &p1, &p2);
			CalculateQuadrPoint(3, qx, qy, &p1, &p2);
		}
		else
		{
			// k = беск., x = const

			CalculateQuadrPoint(0, qx, qy, &p1, &p2, CONST_X);
			CalculateQuadrPoint(1, qx, qy, &p1, &p2, CONST_X);
			CalculateQuadrPoint(2, qx, qy, &p1, &p2, CONST_X);
			CalculateQuadrPoint(3, qx, qy, &p1, &p2, CONST_X);
		}

		if (IsPointInPoly(4, qx, qy, (float)mouseX, (float)mouseY))
		{
			CVector3D v;
			for (int i = 0; i < 4; i++)
			{
				v.Set(qx[i], qy[i], 0);
			}

			if ((vBegin == NULL) && (vEnd == NULL))
			{
				MP_DELETE(*it);
				(*itV).second.erase(it);
			}
			else
			{
				*vBegin = (*it)->begin;
				*vEnd = (*it)->end;

				m_currentSegment = (*it);

			}
			
			m_currentPoint = CVector3D(0,0,0);
			m_firstPoint = CVector3D(0,0,0);

			return true;
		}
	}
	return false;
}

void CPathFinder::SetTypeOfSelectSegment(unsigned char type)
{
	if (m_currentSegment)
		m_currentSegment->typeOfSegment = type;
}

void CPathFinder::RefreshSelectedSegmentPoints()
{
	m_currentSegment = NULL;
}

void CPathFinder::CalculateQuadrPoint(int i, float* qx, float* qy, CVector2D* p1, CVector2D* p2, bool isConstX)
{

	float k = (p2->y-p1->y)/(p2->x-p1->x);
	float b = -p1->x*k + p1->y;

	if (isConstX)
	{
		switch (i)
		{
		case 0:
			qx[0] = p1->x +10;
			qy[0] = p1->y; 
			break;
		case 1:
			qx[1] = p2->x + QUADR_INDENT;
			qy[1] = p2->y;
			break;
		case 2:
			qx[2] = p2->x - QUADR_INDENT;
			qy[2] = p2->y; 
			break;
		case 3:
			qx[3] = p1->x - QUADR_INDENT;
			qy[3] = p1->y;
			break;
		}
	}
	else
	{
		switch (i)
		{
		case 0:
			qx[0] = (float)(p1->x -k*(b+10-p1->y) )/(1 + k*k);  
			qy[0] = k*qx[0] + b+ QUADR_INDENT; 
			break;
		case 1:
			qx[1] = (float)(p2->x -k*(b+10-p2->y) )/(1 + k*k);  
			qy[1] = k*qx[1] + b+ QUADR_INDENT;
			break;
		case 2:
			qx[2] = (float)(p2->x -k*(b-10-p2->y) )/(1 + k*k);  
			qy[2] = k*qx[2] + b - QUADR_INDENT;
			break;
		case 3:
			qx[3] = (float)(p1->x -k*(b-10-p1->y) )/(1 + k*k);  
			qy[3] = k*qx[3] + b - QUADR_INDENT;
			break;
		}
	}
}

bool CPathFinder::IsPointInCurrent3DObjectsPath(int mouseX, int mouseY, CViewport* const viewport ,int objectID , bool* isExistCurrentObject)
{
	m_currentSegments.clear();
	m_currentSegments.reserve(10);
	m_firstPoint = CVector3D(0,0,0);

	FIND_OBJECTS_SEGMENTS_OR_RETURN_FALSE(objectID);

	for ( ; it != itEnd; it++)
	{
		CVector2D v;			
		viewport->GetScreenCoords((*it)->begin.x, (*it)->begin.y, (*it)->begin.z + PATH_ADD_HEGHT, &v.x, &v.y);
		if ((v.x > mouseX - 10) && ( v.x < mouseX + 10) && ((v.y > mouseY - 10) && (v.y < mouseY + 10)) )
		{
			m_currentPoint = (*it)->begin;
			m_currentSegments.push_back(*it);
		}

		viewport->GetScreenCoords((*it)->end.x, (*it)->end.y, (*it)->end.z + PATH_ADD_HEGHT, &v.x, &v.y);
		if ((v.x > mouseX - 10) && ( v.x < mouseX + 10) && ((v.y > mouseY - 10) && (v.y < mouseY + 10)) )
		{
			m_currentPoint = (*it)->end;
			m_currentSegments.push_back(*it);
		}

		if (isExistCurrentObject!= NULL)
		{
			*isExistCurrentObject = true;	
		}	
	}

	if (isExistCurrentObject!= NULL && *isExistCurrentObject == false)
	{
		m_firstPoint = viewport->GetSelected3DCoords();
	}

	if  (m_currentSegments.size()!=0)
	{
		return true;
	}

	return false;
}

CVector3D CPathFinder::GetCurrentPoint()
{
	return m_currentPoint;
}

void CPathFinder::SetCurrentPoint(CVector3D* v)
{
	m_currentPoint = *v;
}

void CPathFinder::MovePoint(CVector3D* v)
{
	std::vector<SSegment*>::iterator it = m_currentSegments.begin();
	std::vector<SSegment*>::iterator itEnd = m_currentSegments.end();	

	for ( ; it != itEnd; it++)
	{
		if ((*it)->begin == m_currentPoint)
		{
			(*it)->begin = *v;

			std::map<int, PathesBB>::iterator itBB = m_pathesBB.find( (*it)->groupId);
			if (itBB != m_pathesBB.end())
			{
				ComparePointWithPathesBB(&(*it)->begin, &(*itBB).second);
			}

		}
		else if ((*it)->end == m_currentPoint)
		{
			(*it)->end = *v;
			std::map<int, PathesBB>::iterator itBB = m_pathesBB.find( (*it)->groupId);

			if (itBB != m_pathesBB.end())
			{
				ComparePointWithPathesBB(&(*it)->end, &(*itBB).second);
			}
		}
	}

	m_currentPoint = *v;
}

std::vector<SSegment*> CPathFinder::FindCurrent3DObjectsPath()
{
	std::vector<SSegment*> v;

	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator it = m_segments.find(m_currentObjectID);
	
	if (it != m_segments.end())
		return (*it).second;

	return v;
}


void CPathFinder::ClearPath()
{
	m_pathPoints.clear();
	m_pathPoints.reserve(20);
}

void CPathFinder::FindPath(CPathLine* const path, CVector3D begin, CVector3D end)
{	
	g->lw.Write("Find path: (", begin.x, ",", begin.y, ",", begin.z, ") - ");
	g->lw.WriteLn("(", end.x, ",", end.y, ",", end.z, ")");	
	
	__int64 startTime =g->tp.GetMicroTickCount();	
	
	ClearPath(); 
	
	UpdateDinamicObjects();
	
	m_queue.Clear();

	m_path = path;

	m_begin = begin;
	m_end = end; 
	
	CVector3D oldBegin, oldEnd;

	oldBegin = begin; oldEnd = end;
			
	EditLength(begin, end);

	CVector3D newEnd = end;	

	SSegment* startSegm = NULL,* endSegm = NULL;

	ClearIntersectionPoints();	

	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator it = m_segments.begin();
	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itEnd = m_segments.end();

	for (; it!=itEnd; it++)
	{
		std::vector<SSegment*>::iterator itV = (*it).second.begin();
		std::vector<SSegment*>::iterator itVEnd = (*it).second.end();

		std::map<int, PathesBB>::iterator itBB = m_pathesBB.find((*it).first);
		if (itBB != m_pathesBB.end())
		{
			if (IsPathIntersectionStaticObjectsPath(&(*itBB).second ,&begin , &end))
			for (; itV!=itVEnd; itV++)
			{
				FindIntersectionPointWithSegment((*itV), begin, end);
			}
		}
	}

	bool isNeedToConstructPerpendicularInEnd = false;
	bool isNeedToConstructPerpendicularInBegin = false;

	int startIntersectionPointIndex = 0  , endIntersectionPointIndex = 0;

	if (m_intersectionPoints.size() == 1)
	{
		 if (!IsIntersectionPointInAddedSegment(&end, &oldEnd, &(*m_intersectionPoints[0]).pnt))
		 {
			isNeedToConstructPerpendicularInEnd = true;
		 }
		 if (!IsIntersectionPointInAddedSegment(&begin, &oldBegin, &(*m_intersectionPoints[0]).pnt)) 
		 {
			 isNeedToConstructPerpendicularInBegin = true;
		 }

		 begin = oldBegin ; end = oldEnd;

		 if ((isNeedToConstructPerpendicularInEnd) || (isNeedToConstructPerpendicularInBegin))
		 {			 			 
			 float minDistToStart = FLT_MAX, minDistToEnd = FLT_MAX;

			 for (it = m_segments.begin(); it!=itEnd; it++)
			 {
				 std::vector<SSegment*>::iterator itV = (*it).second.begin();
				 std::vector<SSegment*>::iterator itVEnd = (*it).second.end();

				 for (; itV!=itVEnd; itV++)
				 {
					 if ((isNeedToConstructPerpendicularInBegin) &&
						 (IsPointNearStaticPath(begin,(*itV)->begin, (*itV)->end, minDistToStart, (*itV))))
					 {
						 startSegm = (*itV);
					 }

					 if ((isNeedToConstructPerpendicularInEnd) &&
						 (IsPointNearStaticPath(end,(*itV)->begin,(*itV)->end, minDistToEnd, (*itV), endIntersectionPointIndex)))
					 {
						 endSegm = (*itV);
					 }
				 }
			 }
		 }
		 
		 SortIntersectionPoints(begin, end);
	}
	else if (m_intersectionPoints.size() != 0)
	{
		begin = oldBegin ; end = oldEnd;
		SortIntersectionPoints(begin, end);

		float distToFirstIntPoint = GetPointDist(begin, (*m_intersectionPoints[0]).pnt);
		float distToPerpPoint = 0;
		SSegment* tempStartSegm = NULL;
	
		CVector3D tempIntPoint = m_intersectionPoints[0]->pnt;
		SSegment* tempIntSegm = m_intersectionPoints[0]->segment;
		
		for (it = m_segments.begin(); it!=itEnd; it++)
		{
			std::vector<SSegment*>::iterator itV = (*it).second.begin();
			std::vector<SSegment*>::iterator itVEnd = (*it).second.end();

			float minDistToStart = FLT_MAX;

			for (; itV!=itVEnd; itV++)
			{
				if (IsPointNearStaticPath(begin,(*itV)->begin, (*itV)->end, minDistToStart, (*itV), 0))
				{
					distToPerpPoint = GetPointDist(begin, (*m_intersectionPoints[0]).pnt);
					tempStartSegm = (*itV);
				}
			}
		}

		if ((tempStartSegm != NULL) && (distToPerpPoint < distToFirstIntPoint))
		{
			startSegm = tempStartSegm;
			startIntersectionPointIndex = 0;
			m_intersectionPoints[0]->segment = tempStartSegm;
		}
		else
		{
			m_intersectionPoints[0]->pnt = tempIntPoint;
			m_intersectionPoints[0]->segment = tempIntSegm;
		}

	}
	
	if(m_intersectionPoints.size() >= 2)
	{
		if (m_intersectionPoints.size() > 2)
		{
			for (unsigned int i = 0; i < m_intersectionPoints.size(); i++)
			{
				int id =(*m_intersectionPoints[i]).segment->groupId;
				C3DObject* obj = GetObjectPointer(id);
				if ((obj->GetAnimation()) || (obj->GetUserData(USER_DATA_EDITABLE)!= NULL))
				{
					DeleteIntersectionPointsBetweenDinamicsPoints(i, id);
				}
			}
		}
		else
		{
			if ( m_intersectionPoints[0]->segment == m_intersectionPoints[1]->segment )
			{
				AddPointToPath(begin , NULL , NULL);
				AddPointToPath(m_intersectionPoints[0]->pnt, m_intersectionPoints[0]->segment , NULL);
				AddPointToPath(m_intersectionPoints[1]->pnt ,m_intersectionPoints[1]->segment , NULL);
				AddPointToPath(end, NULL, NULL);
				return;
			}
		}

		startIntersectionPointIndex = 0;

		int intersectionPointsCounter = m_intersectionPoints.size();

		int iterationCount = 0;

		std::vector<SPathNeededObjectsInfo*>::iterator itD = m_dinamicObjects.begin();	
		std::vector<SPathNeededObjectsInfo*>::iterator itEndD = m_dinamicObjects.end();

#define MAX_ITERATION_COUNT		50

		while ((intersectionPointsCounter > 0) &&
			(iterationCount < MAX_ITERATION_COUNT))
		{		
			int numberOfIntersectionPointsInOnePath = GetStartAndEndSegments(startSegm , endSegm, startIntersectionPointIndex);			

			if (iterationCount == (MAX_ITERATION_COUNT - 1))
			{
				assert(false);
			}
			else if (iterationCount ==0 )
			{
				AddPointToPath(begin, NULL, NULL);
			}
			
			CVector3D v(m_intersectionPoints[startIntersectionPointIndex]->pnt.x, m_intersectionPoints[startIntersectionPointIndex]->pnt.y, m_intersectionPoints[startIntersectionPointIndex]->pnt.z);

			/*if ((numberOfIntersectionPointsInOnePath == 1) && (startIntersectionPointIndex != m_intersectionPoints.size()-1))
			{
				AddPointToPath(v, m_intersectionPoints[startIntersectionPointIndex]->segment, NULL);
			}*/
			if (numberOfIntersectionPointsInOnePath != 1)
			{
				AddPointToPath(v, m_intersectionPoints[startIntersectionPointIndex]->segment, NULL);

				if (startSegm != endSegm)//если не встаем по перпендикуляру на путь из одного сегмента
				{
		
					bool isFoundIntersectwithPartToBegin = false;
					bool isFoundIntersectwithPartToEnd = false;

					itD = m_dinamicObjects.begin();

					for ( ; itD != itEndD; itD++)
					{
						FIND_OBJECTS_SEGMENTS_OR_CONTINUE((*itD)->id);

						for ( ; it!=itEnd; it++)
						{
							float ta,tb;
							if (IsSegmentsIntersect((*it), startSegm->begin,m_intersectionPoints[startIntersectionPointIndex]->pnt ,ta ,tb)) 
							{
								isFoundIntersectwithPartToBegin = true;
							}
							else if (IsSegmentsIntersect((*it), startSegm->end,m_intersectionPoints[startIntersectionPointIndex]->pnt ,ta ,tb)) 
							{
								isFoundIntersectwithPartToEnd = true;
							}

						}
					}
					if ((!isFoundIntersectwithPartToBegin) && (isFoundIntersectwithPartToEnd ))
					{
						m_queue.QWrite(SQElement(startSegm->begin, NULL, GetPointDist(m_intersectionPoints[startIntersectionPointIndex]->pnt, startSegm->begin), startSegm, true));

					}
					else if ((isFoundIntersectwithPartToBegin) && (!isFoundIntersectwithPartToEnd ))
					{
						m_queue.QWrite(SQElement(startSegm->end, NULL, GetPointDist(m_intersectionPoints[startIntersectionPointIndex]->pnt, startSegm->end), startSegm, true));
					}
					else if (((!isFoundIntersectwithPartToBegin) && (!isFoundIntersectwithPartToEnd )) || ((isFoundIntersectwithPartToBegin)&&(isFoundIntersectwithPartToEnd)))
					{
						m_queue.QWrite(SQElement(startSegm->begin, NULL, GetPointDist(m_intersectionPoints[startIntersectionPointIndex]->pnt, startSegm->begin), startSegm, true));
						m_queue.QWrite(SQElement(startSegm->end, NULL, GetPointDist(m_intersectionPoints[startIntersectionPointIndex]->pnt, startSegm->end), startSegm, true));
						SQElement* beginEl = m_queue.GetElement(m_queue.GetTailPosition()-2);
						SQElement* endEl = m_queue.GetElement(m_queue.GetTailPosition()-1);

						beginEl->SetPrevElement(endEl);
						endEl->SetPrevElement(beginEl);
					}
					
					CreatePath(endSegm);					
				}

				endIntersectionPointIndex = startIntersectionPointIndex + (numberOfIntersectionPointsInOnePath -1);
			
				if ((endIntersectionPointIndex >= 0) && (endIntersectionPointIndex < (int)m_intersectionPoints.size()))
				{
					v.Set(m_intersectionPoints[endIntersectionPointIndex]->pnt.x, m_intersectionPoints[endIntersectionPointIndex]->pnt.y, m_intersectionPoints[endIntersectionPointIndex]->pnt.z);
				}
					
				if (endIntersectionPointIndex == (int)(m_intersectionPoints.size()-1))
				{
					if (m_intersectionPoints[endIntersectionPointIndex-1]->segment->groupId == m_intersectionPoints[endIntersectionPointIndex]->segment->groupId)
					{
						AddPointToPath(v, m_intersectionPoints[endIntersectionPointIndex]->segment, NULL);
						end = newEnd;
					}
				}
				else
				{
					AddPointToPath(v, m_intersectionPoints[endIntersectionPointIndex]->segment, NULL);
				}
			}

			startIntersectionPointIndex += numberOfIntersectionPointsInOnePath;

			intersectionPointsCounter -= numberOfIntersectionPointsInOnePath;
			iterationCount++;
		}

		AddPointToPath(end , NULL, NULL);
	}
	else 
	{		
		AddPointToPath(begin , NULL , NULL);
		AddPointToPath(end, NULL, NULL);
	}
	
	__int64 endTime =g->tp.GetMicroTickCount();
	unsigned int delta = (unsigned int)(endTime - startTime);
	g->lw.WriteLn("Find path time: ");
	g->lw.WriteLn(delta);
}


void CPathFinder::DeleteIntersectionPointsBetweenDinamicsPoints(int i, int id)
{
	bool isFound = false;

	unsigned int j; 
	for(j=i+1; j< m_intersectionPoints.size(); j++)
	{
		if ((*m_intersectionPoints[j]).segment->groupId == id) // id dinamic object
		{
			isFound = true;
			break;
			//m_intersectionPoints.erase(m_intersectionPoints.begin()+i+1 ,m_intersectionPoints.begin()+j);
			//return;
		}
	}

	if (isFound)
	{
		if ( (i > 0) && ((j+1) < m_intersectionPoints.size()))
		{
			bool isNeedToRemoveDynamicSegments = true;
			int groupID = (*m_intersectionPoints[i-1]).segment->groupId;

			for (unsigned int k = i - 1; k <= j + 1; k++)
				if (((*m_intersectionPoints[k]).segment->groupId != id) &&
					((*m_intersectionPoints[k]).segment->groupId != groupID))
				{
					isNeedToRemoveDynamicSegments = false;
				}

				if (isNeedToRemoveDynamicSegments) 
				{
					m_intersectionPoints.erase(m_intersectionPoints.begin() + i);
					m_intersectionPoints.erase(m_intersectionPoints.begin() + j - 1);
				}
				else if ((j-i) > 1)
				{
					m_intersectionPoints.erase(m_intersectionPoints.begin()+i+1 ,m_intersectionPoints.begin()+j);
				}
		}
		else if ((j-i) > 1)
		{
			m_intersectionPoints.erase(m_intersectionPoints.begin()+i+1 ,m_intersectionPoints.begin()+j);
		}
	}
}

bool CPathFinder::IsIntersectionPointInAddedSegment(const CVector3D* begin, const CVector3D* end, const CVector3D* point)
{
	CVector3D* v1 = (CVector3D*)begin;
	CVector3D* v2 = (CVector3D*)end;


	if (begin->x > end->x)
	{
		v1 = (CVector3D*)end;
		v2 = (CVector3D*)begin;
	}

	if ((point->x >= v1->x) && (point->x <= v2->x))
	{
		return true;
	}

	return false;

};

//	1001|	1000	|1010
//	-----------------------
//	0001|	0000	|0010
//	-----------------------
//	0101|	0100	|0110
//	Биты (от младшего до старшего) значат «левее», «правее», «ниже», «выше».
//	у тех трёх частей плоскости, которые слева от прямоугольника, младший бит равен 1, и т д

bool CPathFinder::IsPathIntersectionStaticObjectsPath(const PathesBB* pathesBB , const CVector3D* p1 , const CVector3D* p2)
{	
	int code_a, code_b; /* код конечных точек отрезка */

	code_a = CODE_POINT(pathesBB, p1);
	code_b = CODE_POINT(pathesBB, p2);

	//Если оба кода равны нулю, то отрезок полностью находится в прямоугольнике. 
	//Если битовое И кодов не равно нулю, то отрезок не пересекает прямоугольник (т.к. это значит, что обе конечные точки отрезка находятся с одной стороны прямоугольника)
	if (code_a & code_b)
		return false;

	return true;
}

void CPathFinder::FindIntersectionPointWithSegment(SSegment* segment, const CVector3D& begin, const CVector3D& end, std::vector<SIntersectionPointsInfo*>* points)
{
	//  отрезки a и b 
	//  a -  A1(A1x,A1y) и A2(A2x,A2y)
	//  b -  B1(B1x,B1y) и B2(B2x,B2y)

	//  Уравнение прямой, на которой  лежит  a:
	//  Xa = A1x + ta(A2x - A1x)
	//  Ya = A1y + ta(A2y - A1y)
	//  ta [0,1];

	//  для отрезка b:
	//  Xb = B1x + tb(B2x - B1x)
	//  Yb = B1y + tb(B2y - B1y)

	//  приравнивая соотв коорд, получаем задачу нахождения параметров ta,tb, при которых бы выполнялись равенства:
	//  A1x + ta (A2x - A1x) = B1x + tb(B2x - B1x)
	//  A1y + ta (A2y - A1y) = B1y + tb(B2y - B1y)

	//  относительно ta,tb получаем:
	//  ta (A1x - A2x) + tb (B2x - B1x) = A1x - B1x
	//  ta (A1y - A2y) + tb (B2y - B1y) = A1y - B1y

	//  система:
	//  a1 x + b1 y = c1
	//  a2 x + b2 y = c2
	//  имеет следующее решение:
	//  x = dx/d
	//	y = dy/d,
	//	где d - определитель матрицы, 
	//	d = a1b2 - a2b1,
	//	dx = c1b2 - c2b1,
	//	dy = a1c2 - a2c1.
	//	В нашей системе относительно ta,tb:
	//  a1 = A1x - A2x 
	//	b1 = B2x - B1x
	//	c1 = A1x - B1x

	//	a2 = A1y - A2y
	//	b2 = B2y - B1y
	//	c2 = A1y - B1y


	float minLength = 0 , maxLength = 0;
	float ta,tb;

	if (IsSegmentsIntersect(segment, begin, end, ta, tb))
	{
			MP_NEW_P3(pnt, CVector3D, begin.x+ta*(end.x-begin.x), begin.y+ta*(end.y-begin.y), 0);
			
			float allLength = GetPointDist(segment->begin,segment->end);
			if (allLength != 0)
			{
				pnt->z = segment->begin.z +(GetPointDist(segment->begin, *pnt)/allLength)*(segment->end.z - segment->begin.z);
			}
			else
			{
				pnt->z = segment->begin.z;
			}

			if (points)
			{
				std::vector<SIntersectionPointsInfo *>::iterator it = points->begin();
				std::vector<SIntersectionPointsInfo *>::iterator itEnd = points->end();

				bool isNeedToAdd = true;

				for ( ; it != itEnd; it++)
				{
					if (((((*it)->pnt.x - 0.5 < (*pnt).x) && ((*it)->pnt.x + 0.5 > (*pnt).x) ) ||
						(((*it)->pnt.y - 0.5 < (*pnt).y) && ((*it)->pnt.y + 0.5 > (*pnt).y)))
						&&  (segment == (*it)->segment))
					{
						isNeedToAdd = false;
						break;
					}
				}

				if (isNeedToAdd)
				{
					MP_NEW_P2(_info, SIntersectionPointsInfo, *pnt, segment);
					points->push_back(_info);
				}

			}
			else
			{
				std::vector<SIntersectionPointsInfo *>::iterator it = m_intersectionPoints.begin();
				std::vector<SIntersectionPointsInfo *>::iterator itEnd = m_intersectionPoints.end();

				bool isNeedToAdd = true;

				for ( ; it != itEnd; it++)
				{
					if (((((*it)->pnt.x - 0.5 < (*pnt).x) && ((*it)->pnt.x + 0.5 > (*pnt).x) ) ||
						(((*it)->pnt.y - 0.5 < (*pnt).y) && ((*it)->pnt.y + 0.5 > (*pnt).y)))
						&&  (segment == (*it)->segment))
					{
						isNeedToAdd = false;
						break;
					}
				}

				if (isNeedToAdd)
				{
					MP_NEW_P2(_info, SIntersectionPointsInfo, *pnt, segment);
					m_intersectionPoints.push_back(_info);	
				}


				if ( (minLength == 0) ||(GetPointDist((*m_intersectionPoints[m_intersectionPoints.size()-1]).pnt , begin) < minLength))
				{
					minLength = GetPointDist((*m_intersectionPoints[m_intersectionPoints.size()-1]).pnt , begin);
				}

				if (GetPointDist((*m_intersectionPoints[m_intersectionPoints.size()-1]).pnt , begin) > maxLength)
				{
					maxLength = GetPointDist((*m_intersectionPoints[m_intersectionPoints.size()-1]).pnt , begin);
				}
			}
			
		}
}

void CPathFinder::CreatePath (SSegment* endSegm )
{
	if (endSegm == NULL)
	{
		return;
	}

	std::map<SSegment*, std::vector<int> > segmentsWithRound;

	SQElement* optimalPath = NULL;

	std::vector<SPointsInQueue> pointsInQueue;

	bool isWorking = true;
	while (isWorking)
	{
		SQElement* newElement = m_queue.QRead();
		if (newElement!= NULL) 
		{
			if (endSegm->begin == newElement->curPoint) 
			{
				AddElementToPath(endSegm->begin , &newElement, &optimalPath);
			}
			else if (endSegm->end == newElement->curPoint) 
			{
				AddElementToPath(endSegm->end , &newElement, &optimalPath);
			}
			else
			{
				FIND_OBJECTS_SEGMENTS_OR_RETURN(endSegm->groupId);

				for (; it!=itEnd; it++ )
				{
					std::vector<int>dinamicsIntersectSegmentsId;

					if ( (newElement->prevElement == NULL) ||  (!( ((*it)->begin == newElement->prevElement->curPoint ) || ((*it)->end == newElement->prevElement->curPoint) )))
					{
						if (((!optimalPath) || (optimalPath->length > newElement->length)) &&
							(!IsSegmentIntersectDinamicsPath((*it), dinamicsIntersectSegmentsId)) && ((*it)->active))
						{
							if ((*it)->begin == newElement->curPoint)
							{
								if  (IsNeedToAddPointsInQueue(pointsInQueue, &(*it)->end, newElement->length + (*it)->length))
								{
									if (dinamicsIntersectSegmentsId.size()!=0 )
									{
										segmentsWithRound.insert(std::map<SSegment*, std::vector<int> >::value_type((*it), dinamicsIntersectSegmentsId));
									}

									m_queue.QWrite(SQElement((*it)->end, newElement , newElement->length + (*it)->length, (*it)));
									m_segmentsInQueue.push_back((*it));
								}
							}
							else if ((*it)->end == newElement->curPoint)
							{
								if (IsNeedToAddPointsInQueue(pointsInQueue, &(*it)->begin, newElement->length + (*it)->length))
								{
									if (dinamicsIntersectSegmentsId.size()!=0 )
									{
										segmentsWithRound.insert(std::map<SSegment*, std::vector<int> >::value_type((*it), dinamicsIntersectSegmentsId));
									}

									m_segmentsInQueue.push_back((*it));
									m_queue.QWrite(SQElement((*it)->begin, newElement , newElement->length + (*it)->length, (*it)));
								}
							}
						}			
					}
				}
			}		
		}
		else
		{
			isWorking = false;
			break;
		}
	}
	
	if (optimalPath)
	{
		std::vector<SPointsInPath*> path;
		path.reserve(m_queue.GetTailPosition());

		while(!optimalPath->isBeginPoint)
		{
			std::map<SSegment*, std::vector<int> >::iterator itS = segmentsWithRound.find(optimalPath->segment);

			if (optimalPath->prevElement)
			{
				MP_NEW_P3(pip, SPointsInPath, optimalPath->curPoint, optimalPath->segment, optimalPath->prevElement->segment);
				path.push_back(pip);
			}
			else
			{
				MP_NEW_P3(pip, SPointsInPath, optimalPath->curPoint, optimalPath->segment, NULL);
				path.push_back(pip);
			}

			if (itS != segmentsWithRound.end())
			{
				std::vector<int>::iterator itD = (*itS).second.begin();
				std::vector<int>::iterator itDEnd = (*itS).second.end();

				float angWithOxSegment = CalculateAngleWithOx(&(*itS).first->begin,&(*itS).first->end);

				CVector3D normVS; //нормаль-вектор сегмента пути стат. объекта

				switch ((*itS).first->typeOfSegment)
				{
				case ANY_ROUND_SEGMENT_TYPE:

				case RIGHT_ROUND_SEGMENT_TYPE: 
					{
						angWithOxSegment -= (float)(M_PI/2);

						//Определяем координаты нормаль-вектора
						normVS.x = m_cosTable[GetAngleInDegrees(angWithOxSegment)]; 
						normVS.y = m_sinTable[GetAngleInDegrees(angWithOxSegment)];

					}
					break;
				case LEFT_ROUND_SEGMENT_TYPE: 
					{
						angWithOxSegment += (float)(M_PI/2);

						//Определяем координаты нормаль-вектора
						normVS.x = m_cosTable[GetAngleInDegrees(angWithOxSegment)]; 
						normVS.y = m_sinTable[GetAngleInDegrees(angWithOxSegment)];
					}
					break;
				}

				for ( ;itD != itDEnd; itD++)
				{
					FIND_OBJECTS_SEGMENTS_OR_CONTINUE((*itD));

					std::vector<SIntersectionPointsInfo*> points;

					std::vector<SSegment*>::iterator it1 = it;

					for ( ; it!=itEnd; it++)
					{
						FindIntersectionPointWithSegment((*it), (*itS).first->begin, (*itS).first->end, &points);	
					}

					if (points.size()!=0)
					{


						SIntersectionPointsInfo* v;

						for (int i = points.size()-1; i > 0 ; i--)
							for (int j = 0; j < i; j++)
							{
								float deltaX = abs((*itS).first->begin.x - (*itS).first->end.x);
								float deltaY = abs((*itS).first->begin.y - (*itS).first->end.y);

								if (deltaX >= deltaY)
								{
									if ( ( ((*itS).first->begin.x < (*itS).first->end.x) && (optimalPath->curPoint == (*itS).first->begin) && (points[j]->pnt.x > points[j+1]->pnt.x)) 
										|| (((*itS).first->begin.x > (*itS).first->end.x) && (optimalPath->curPoint == (*itS).first->end)&& (points[j]->pnt.x > points[j+1]->pnt.x )) 
										|| (((*itS).first->begin.x > (*itS).first->end.x) && (optimalPath->curPoint == (*itS).first->begin)&& (points[j]->pnt.x < points[j+1]->pnt.x )) 
										|| (((*itS).first->begin.x < (*itS).first->end.x) && (optimalPath->curPoint == (*itS).first->end)&& (points[j]->pnt.x < points[j+1]->pnt.x )) )
									{

										v = points[j];
										points[j] = points[j+1];
										points[j+1] = v;
									}
								}
								else
								{
									if ( ( ((*itS).first->begin.y < (*itS).first->end.y) && (optimalPath->curPoint == (*itS).first->begin) && (points[j]->pnt.y > points[j+1]->pnt.y)) 
										|| (((*itS).first->begin.y > (*itS).first->end.y) && (optimalPath->curPoint == (*itS).first->end)&& (points[j]->pnt.y > points[j+1]->pnt.y )) 
										|| (((*itS).first->begin.y > (*itS).first->end.y) && (optimalPath->curPoint == (*itS).first->begin)&& (points[j]->pnt.y < points[j+1]->pnt.y )) 
										|| (((*itS).first->begin.y < (*itS).first->end.y) && (optimalPath->curPoint == (*itS).first->end)&& (points[j]->pnt.y < points[j+1]->pnt.y )) )
									{

										v = points[j];
										points[j] = points[j+1];
										points[j+1] = v;
									}
								}	
							}

							MP_NEW_P3(pip, SPointsInPath, points[0]->pnt, points[0]->segment, NULL);
							path.push_back(pip);

							CVector3D vDinam = points[0]->segment->begin - points[0]->segment->end;

							if ( normVS.Dot(vDinam) >0 )
							{
								MP_NEW_P3(pip, SPointsInPath, points[0]->segment->begin, points[0]->segment, NULL);
								path.push_back(pip);

							}
							else
							{
								vDinam = points[0]->segment->end - points[0]->segment->begin;
								if (normVS.Dot(vDinam) > 0)
								{
									MP_NEW_P3(pip, SPointsInPath, points[0]->segment->end, points[0]->segment, NULL);
									path.push_back(pip);
								}
							}

							CVector3D prevPoint = path[path.size()-1]->pnt;

							SSegment* prevSegment = points[0]->segment;

							bool isFoundEnd = false;

							while (!isFoundEnd)//(prevSegment != points[points.size()-1]->segment)
							{
								it =it1;

								for (; it!=itEnd; it++)
								{
									if ((*it)!= prevSegment)
									{
										bool isFound = false;
										if ((*it)->begin == prevPoint)
										{												
											prevPoint = (*it)->end;
											isFound = true;
										}
										else if ((*it)->end == prevPoint)
										{												
											prevPoint = (*it)->begin;
											isFound = true;
										}

										if (isFound)
										{
											MP_NEW_V3(path[path.size()-1], SPointsInPath, prevPoint, prevSegment, (*it));											 

											prevSegment = (*it);

											if (prevSegment == points[points.size()-1]->segment)
											{
												isFoundEnd =true;
												break;
											}
										}
									}

								}


							}

							path[path.size()-1]->pnt =  points[points.size()-1]->pnt;
							path[path.size()-1]->segment1 =  points[points.size()-1]->segment;
							path[path.size()-1]->segment2 = NULL;
					}
				}
			}

			optimalPath = optimalPath->prevElement;
		}

		if (optimalPath->prevElement)
		{
			MP_NEW_P3(pip, SPointsInPath, optimalPath->curPoint, optimalPath->segment, optimalPath->prevElement->segment);
			path.push_back(pip);
		}
		else
		{
			MP_NEW_P3(pip, SPointsInPath, optimalPath->curPoint, optimalPath->segment, NULL);
			path.push_back(pip);
		}

		std::vector<SPointsInPath*>::reverse_iterator it = path.rbegin();	
		std::vector<SPointsInPath*>::reverse_iterator itEnd = path.rend();

		for ( ; it != itEnd; it++)
		{
			AddPointToPath((*it)->pnt, (*it)->segment1, (*it)->segment2);
		}
	}
}

bool CPathFinder::IsSegmentIntersectDinamicsPath(const SSegment* segment, std::vector<int>& dinamicsIntersectSegmentsId)
{
	C3DObject* obj =GetObjectPointer(segment->groupId);

	if	((obj->GetUserData(USER_DATA_EDITABLE)) || (obj->GetAnimation()))
		return false;

	std::vector<SPathNeededObjectsInfo*>::iterator itD = m_dinamicObjects.begin();	
	std::vector<SPathNeededObjectsInfo*>::iterator itEndD = m_dinamicObjects.end(); 

	for ( ; itD != itEndD; itD++)
	{
		FIND_OBJECTS_SEGMENTS_OR_CONTINUE((*itD)->id);

		for ( ; it!=itEnd; it++)
		{
			float ta,tb;
			if (IsSegmentsIntersect((*it), segment->begin, segment->end ,ta ,tb)) 
			{
				if (segment->typeOfSegment == COMMON_ROUND_SEGMENT_TYPE)
				{
					return true;
				}
				else
				{
					dinamicsIntersectSegmentsId.push_back((*it)->groupId);
					break;
				}			
			}
		}
	}
	return false;
}
	
bool CPathFinder::IsSegmentsIntersect(const SSegment* segment, CVector3D begin, CVector3D end, float& ta, float& tb)
{
	float d,da,db,eps=5;

	d =(begin.x-end.x) * (segment->end.y-segment->begin.y) - (begin.y-end.y)*(segment->end.x-segment->begin.x);
	da =(begin.x-segment->begin.x)*(segment->end.y-segment->begin.y) - (begin.y-segment->begin.y)*(segment->end.x-segment->begin.x);
	db =(begin.x-end.x)*(begin.y-segment->begin.y) - (begin.y-end.y)*(begin.x-segment->begin.x);

	if (abs(d) > eps) 
	{
		ta = da/d;
		tb = db/d;		
		if   ( (ta >= 0) && ( ta <= 1) &&  ( tb >=0 ) && (tb <= 1))
		{
			return true;
		}
	}
	return false;
}

bool CPathFinder::IsNeedToAddPointsInQueue(std::vector<SPointsInQueue>& pointsInQueue, CVector3D* point, float length)
{
	bool isExist = false;

	std::vector<SPointsInQueue>::iterator it = pointsInQueue.begin();	
	std::vector<SPointsInQueue>::iterator itEnd = pointsInQueue.end();

	for (; it!=itEnd; it++)
	{
		if (((*it).point == (*point)) && ((*it).length > length))
		{
			(*it).length = length;
			return true;
		}
		else if ((*it).point == (*point))
		{
			isExist = true;
		}
	}

	if (!isExist)
	{
		pointsInQueue.push_back(SPointsInQueue(point->x, point->y, point->z, length));
		return true;
	}

	return false;
}


void CPathFinder::AddElementToPath(CVector3D v, SQElement** newElement, SQElement** optimalPath)
{
	float length = (*newElement)->length + GetPointDist(v, (*newElement)->curPoint);

	if ( (!(*optimalPath)) || ((*optimalPath)->length > length) )
	{
		(*optimalPath) = (*newElement);
		(*newElement)->length = length;
	}
}

void CPathFinder::SetGoToSegmentMode(bool isMoveToSegment)
{
	m_isMoveToSegment = isMoveToSegment;
}

void CPathFinder::Draw()
{
	if (!m_isDebugRenderingEnabled)
	{
		return;
	}

	if (g->ne.GetWorkMode() == WORK_MODE_SNAPSHOTRENDER)
	{
		return;
	}

	if (g->rs.GetBool(SHADERS_USING))
	{
		g->sm.UnbindShader();
	}

	GLFUNC(glDisable)(GL_TEXTURE_2D);

	g->stp.SetState(ST_ZTEST, false);


	g->stp.SetColor(0, 255, 0);
	g->stp.PrepareForRender();
	GLFUNC(glLineWidth)(1);
	GLFUNC(glBegin)(GL_LINES);


	float cameraDisplaycement = GetDist(m_eyePosition, g->cm.GetActiveCamera()->GetEyePosition());

	std::vector<SPathNeededObjectsInfo*>::iterator itA = m_dinamicObjects.begin();
	std::vector<SPathNeededObjectsInfo*>::iterator itEndA = m_dinamicObjects.end();


	for( ; itA != itEndA; itA++)
	{
		if ((*itA)->typeOfObject == AVATAR)
		{
			FIND_OBJECTS_SEGMENTS_OR_CONTINUE((*itA)->id);
			for ( ; it != itEnd; it++)
			{
				GLFUNC(glVertex3f)((*it)->begin.x, (*it)->begin.y, (*it)->begin.z + PATH_ADD_HEGHT);
				GLFUNC(glVertex3f)((*it)->end.x, (*it)->end.y, (*it)->end.z + PATH_ADD_HEGHT);	
			}
		}
	}


	if (m_isShowNearest)	
	{
		if ((cameraDisplaycement >= 300) || (m_isShowNearestAfterChangePath))
		{
			FindNearestPathes();
		}

		int lastGroupID = -1;

		std::vector<SSegment*>::iterator it = m_nearestPathes.begin();
		std::vector<SSegment*>::iterator itEnd = m_nearestPathes.end();

		for ( ; it != itEnd; it++)
		{
			if ((*it)->groupId != lastGroupID)
			{
				g->stp.SetColor(0, 0, 255);
				std::map<int, PathesBB>::iterator itB = m_pathesBB.find((*it)->groupId);

				if (itB != m_pathesBB.end())
				{					
					GLFUNC(glVertex3f)((*itB).second.min.x, (*itB).second.min.y, (*itB).second.min.z + PATH_ADD_HEGHT);
					GLFUNC(glVertex3f)((*itB).second.max.x, (*itB).second.min.y, (*itB).second.min.z + PATH_ADD_HEGHT);
					GLFUNC(glVertex3f)((*itB).second.min.x, (*itB).second.max.y, (*itB).second.min.z + PATH_ADD_HEGHT);
					GLFUNC(glVertex3f)((*itB).second.max.x, (*itB).second.max.y, (*itB).second.min.z + PATH_ADD_HEGHT);
					GLFUNC(glVertex3f)((*itB).second.min.x, (*itB).second.min.y, (*itB).second.min.z + PATH_ADD_HEGHT);
					GLFUNC(glVertex3f)((*itB).second.min.x, (*itB).second.max.y, (*itB).second.min.z + PATH_ADD_HEGHT);
					GLFUNC(glVertex3f)((*itB).second.max.x, (*itB).second.min.y, (*itB).second.min.z + PATH_ADD_HEGHT);
					GLFUNC(glVertex3f)((*itB).second.max.x, (*itB).second.max.y, (*itB).second.min.z + PATH_ADD_HEGHT);
				}
				g->stp.SetColor(0, 255, 0);
				lastGroupID = (*it)->groupId;
			}

			DrawSegments((*it));
		}
	}
	else
	{
		std::vector<SSegment*> v = FindCurrent3DObjectsPath();
		for (unsigned int i = 0; i < v.size() ; i ++)
		{			
			DrawSegments(v[i]);
		}
	}


	if (m_currentSegment != NULL)
	{
		DrawSegments(m_currentSegment);
	}

	GLFUNC(glEnd)();

	g->stp.SetColor(255, 255, 0);
	g->stp.PrepareForRender();
	GLFUNC(glPointSize)(9);
	GLFUNC(glBegin)(GL_POINTS);

	if (m_isShowNearest)
	{
		g->stp.SetColor(255, 255, 0);

		std::vector<SSegment*>::iterator it = m_nearestPathes.begin();
		std::vector<SSegment*>::iterator itEnd = m_nearestPathes.end();

		for ( ; it != itEnd; it++)
		{
			GLFUNC(glVertex3f)((*it)->begin.x, (*it)->begin.y, (*it)->begin.z + PATH_ADD_HEGHT);
			GLFUNC(glVertex3f)((*it)->end.x, (*it)->end.y, (*it)->end.z + PATH_ADD_HEGHT);
		}

		if (m_firstPoint != CVector3D(0,0,0))
		{
			GLFUNC(glVertex3f)(m_firstPoint.x, m_firstPoint.y, m_firstPoint.z + PATH_ADD_HEGHT);
		}

		if (m_currentPoint != CVector3D(0,0,0))
		{
			g->stp.SetColor(255, 0, 0);
			GLFUNC(glVertex3f)(m_currentPoint.x, m_currentPoint.y, m_currentPoint.z + PATH_ADD_HEGHT);
		}
	}
	else
	{
		std::vector<SSegment*> v = FindCurrent3DObjectsPath();
		g->stp.SetColor(255, 255, 0);

		if (v.size()>1)
		{
			for (unsigned int i = 0; i < v.size(); i ++)
			{			
				GLFUNC(glVertex3f)(v[i]->begin.x, v[i]->begin.y, v[i]->begin.z + PATH_ADD_HEGHT);
				GLFUNC(glVertex3f)(v[i]->end.x, v[i]->end.y, v[i]->end.z + PATH_ADD_HEGHT);
			}

			if (m_currentPoint != CVector3D(0,0,0))
			{
				g->stp.SetColor(255, 0, 0);
				GLFUNC(glVertex3f)(m_currentPoint.x, m_currentPoint.y, m_currentPoint.z + PATH_ADD_HEGHT);
			}
		}

		if (m_firstPoint != CVector3D(0,0,0))
		{
			GLFUNC(glVertex3f)(m_firstPoint.x, m_firstPoint.y, m_firstPoint.z + PATH_ADD_HEGHT);
		}
	}
	GLFUNC(glEnd)();

	g->stp.SetColor(0, 255, 255);

	std::vector<SIntersectionPointsInfo*>::iterator it = m_intersectionPoints.begin();
	std::vector<SIntersectionPointsInfo*>::iterator itEnd = m_intersectionPoints.end();

	GLFUNC(glBegin)(GL_POINTS);
	for ( ; it != itEnd; it++)
	{
		GLFUNC(glVertex3f)((*it)->pnt.x, (*it)->pnt.y, (*it)->pnt.z);
	}
	GLFUNC(glEnd)();


	g->stp.SetColor(255, 0, 0);
	g->stp.PrepareForRender();
	GLFUNC(glLineWidth)(2);
	GLFUNC(glBegin)(GL_LINES);

	if (!m_isMoveToSegment)
	{		
		for (unsigned int i = 0; i < m_pathPoints.size(); i++)
		{			
			GLFUNC(glColor3ub)(GLubyte(255 * i / m_pathPoints.size()), 0, 0);
			GLFUNC(glVertex3f)(m_pathPoints[i].x, m_pathPoints[i].y, m_pathPoints[i].z + PATH_ADD_HEGHT);			
			if ((i != 0) && (i != m_pathPoints.size() - 1))
			{
				GLFUNC(glVertex3f)(m_pathPoints[i].x, m_pathPoints[i].y, m_pathPoints[i].z + PATH_ADD_HEGHT);				
			}
		}		
	}
	else if (m_currentSegment != NULL)
	{
		GLFUNC(glVertex3f)(m_currentSegment->begin.x, m_currentSegment->begin.y, m_currentSegment->begin.z + PATH_ADD_HEGHT);
		GLFUNC(glVertex3f)(m_currentSegment->end.x, m_currentSegment->end.y, m_currentSegment->end.z + PATH_ADD_HEGHT);
	}


	GLFUNC(glEnd)();
	GLFUNC(glLineWidth)(1);

	g->stp.SetState(ST_ZTEST, true);
	GLFUNC(glEnable)(GL_TEXTURE_2D);
	g->stp.SetColor(255, 255, 255);

	m_isShowNearestAfterChangePath = false;
}


void CPathFinder::DrawSegments(SSegment* segment)
{
	if (segment == NULL)
		return;

	if (segment == m_currentSegment)
	{
		g->stp.SetColor(255,0,255);
		GLFUNC(glVertex3f)(segment->begin.x, segment->begin.y, segment->begin.z + PATH_ADD_HEGHT);
		GLFUNC(glVertex3f)(segment->end.x, segment->end.y, segment->end.z + PATH_ADD_HEGHT);
	}

	if (segment->typeOfSegment != COMMON_ROUND_SEGMENT_TYPE)
	{
		if (segment != m_currentSegment)
		{
			g->stp.SetColor(255,0,255);
			GLFUNC(glVertex3f)(segment->begin.x, segment->begin.y, segment->begin.z + PATH_ADD_HEGHT);
			GLFUNC(glVertex3f)(segment->end.x, segment->end.y, segment->end.z + PATH_ADD_HEGHT);
		}
			
		float angWithOx = CalculateAngleWithOx(&segment->begin,&segment->end);

		if ((segment->typeOfSegment == RIGHT_ROUND_SEGMENT_TYPE) || (segment->typeOfSegment == ANY_ROUND_SEGMENT_TYPE))
		{
			angWithOx -= (float)(M_PI/2);
		}
		else if (segment->typeOfSegment == LEFT_ROUND_SEGMENT_TYPE)
		{
			angWithOx += (float)(M_PI/2);
		}

		//объявляем вектор в который поместим координаты нормаль-вектора
		CVector3D onev; 

		//Определяем координаты нормаль-вектора
		onev.x = m_cosTable[GetAngleInDegrees(angWithOx)]; // x = cos(alpha)
		onev.y = m_sinTable[GetAngleInDegrees(angWithOx)];//(float)sin(angWithOx); // y = sin(alpha)

		CVector3D rvec; 
		CVector3D middlePoint = CVector3D((segment->begin.x + segment->end.x)/2,(segment->begin.y + segment->end.y)/2, (segment->begin.z + segment->end.z)/2+ PATH_ADD_HEGHT);

		rvec = onev * 50 + middlePoint; //умножаем нормаль-вектор на rlen чтобы получить вектор с длинной rlen
		//и прибавляем к нему вектор - середину отрезка

		GLFUNC(glVertex3f)(rvec.x,rvec.y, middlePoint.z + PATH_ADD_HEGHT);
		GLFUNC(glVertex3f)(middlePoint.x,middlePoint.y, middlePoint.z+ PATH_ADD_HEGHT);

		if (segment->typeOfSegment == ANY_ROUND_SEGMENT_TYPE)
		{
			rvec *= -1;

			rvec = rvec + middlePoint*2; 

			GLFUNC(glVertex3f)(rvec.x,rvec.y, middlePoint.z + PATH_ADD_HEGHT);
			GLFUNC(glVertex3f)(middlePoint.x,middlePoint.y, middlePoint.z+ PATH_ADD_HEGHT);
		}

	}
	else if (segment != m_currentSegment)
	{
		if (segment->active)
		{
			g->stp.SetColor(0, 255, 0);
		}
		else
		{
			g->stp.SetColor(0, 0, 255);
		}

		GLFUNC(glVertex3f)(segment->begin.x,segment->begin.y, segment->begin.z + PATH_ADD_HEGHT);
		GLFUNC(glVertex3f)(segment->end.x, segment->end.y, segment->end.z + PATH_ADD_HEGHT);
	}

	g->stp.SetColor(0,255,0);
}

void CPathFinder::EditLength(CVector3D& begin , CVector3D& end)
{
	//Длина результата равна сумме длины входного отрезка и значения на которое нужно увеличить длину входного отрезка
	float rlen = GetPointDist(begin, end) + PATH_EDIT_LENGTH;
	float angWithOx;

	//вычисляем угол входного отрезка с осью Ох
	angWithOx = CalculateAngleWithOx(&begin,&end);

	//объявляем вектор в который поместим координаты нормаль-вектора
	CVector3D onev; 

	//Определяем координаты нормаль-вектора

	onev.x = m_cosTable[GetAngleInDegrees(angWithOx)]; // x = cos(alpha)
	onev.y = m_sinTable[GetAngleInDegrees(angWithOx)];//(float)sin(angWithOx); // y = sin(alpha)

	CVector3D rvec; 
	CVector3D oldEnd = end;
	CVector3D oldBegin = begin;

	if (begin.x <= end.x)
	{
		rvec = onev * rlen + begin; //умножаем нормаль-вектор на rlen чтобы получить вектор с длинной rlen
		//и прибавляем к нему вектор (начало отрезка)

		end.x = rvec.x;
		end.y = rvec.y;

		rvec.x *= -1;
		rvec.y *= -1;

		rvec = rvec + begin + oldEnd;

		begin.x = rvec.x;
		begin.y = rvec.y;

	}
	else 
	{
		rvec = onev * rlen + end; 

		begin.x = rvec.x;
		begin.y = rvec.y;

		rvec.x *= -1;
		rvec.y *= -1;

		rvec = rvec + end + oldBegin;

		end.x = rvec.x;
		end.y = rvec.y;

	}

	//в rvec будут координаты конца отрезка с длинной rlen, координаты начала это вектор begin
}
 
float CPathFinder::CalculateAngleWithOx(CVector3D* begin, CVector3D* end)
{
	if ( ( (end->x-begin->x)!= 0) && ((end->y-begin->y)!= 0))
	{
		return (float)atan(float((end->y-begin->y)/(end->x-begin->x)));
	}
	else if (end->y > begin->y)
	{
		return  (float)(M_PI/2);
	}
	else if (end->y < begin->y)
	{
		return (float)(M_PI * 3/2);
	}
	else if (end->y == begin->y)
	{
		if (begin->x > end->x)
		{
			return 0;
		}
		else
		{
			return (float)M_PI;
		}
	}

	return 0;
}

//p1 p2 -вектора концов отрезка 
//p0(c,d) - вектор точки 
//p(x, kx+b) - точка пересечения
//p = p1 + (p2 -p1)*t;
//[(x-c)^2+(kx+b-d)^2]min -> произв.=0
//


/*
Уравнение прямой, проходящей через две точки:
(y-y1)/(y2-y1)=(x-x1)/(x2-x1);
Y=(x-x1)*(y2-y1)/(x2-x1)+y1;
угловой коэф:
k=(y2-y1)/(x2-x1);
*/

bool CPathFinder::IsPointNearStaticPath(const CVector3D& p0, const CVector3D& p1, const CVector3D& p2, float& minDist ,SSegment* segment, int endIndex)
{
	#define EPS		0.05	
	float k , b; 
	CVector3D p;

	CVector3D* begin = (CVector3D *)&p1; 
	CVector3D* end = (CVector3D *)&p2; 

	if ( abs(p2.x-p1.x) > EPS)
	{
		k = (p2.y-p1.y)/(p2.x-p1.x);
		b = -p1.x*k + p1.y;
		p.x = (float)(p0.x -k*(b-p0.y) )/(1 + k*k);  
		p.y = k*p.x + b; 

		float  z = p1.z + (GetPointDist(p1,p)/(GetPointDist(p1,p2)))*(p2.z - p1.z);

		if (p2.x < p1.x)
		{
			end = (CVector3D *)&p1;
			begin = (CVector3D *)&p2;
		}

		if ( (p.x > begin->x) && (p.x < end->x)) 
		{
			float dist = GetPointDist(p,p0);

			if (endIndex != -1)
			{
				if ((minDist > dist) && (dist <= MAX_DIST_TO_PATH))
				{
					m_intersectionPoints[endIndex]->pnt.Set(p.x, p.y, z);
					m_intersectionPoints[endIndex]->segment = segment;
					minDist = dist;
					return true;
				}

			}
			else if ((dist <= MAX_DIST_TO_PATH) && (minDist > dist) )
			{
				if (m_intersectionPoints.size() ==1)
				{
					if (m_intersectionPoints[0]->segment->groupId == segment->groupId) // чтобы для 2 ТП от разных объектов не строил перп-р от начала
					{
						MP_NEW_P2(_info, SIntersectionPointsInfo, CVector3D(p.x, p.y, z), segment);
						m_intersectionPoints.push_back(_info);
						return true;
					}
				}
				else
				{
					m_intersectionPoints[m_intersectionPoints.size()-1]->pnt.Set(p.x, p.y, z);
					m_intersectionPoints[m_intersectionPoints.size()-1]->segment = segment;
					return true;
				}
			}
		}
	}
	else
	{
		//k - беск., х = const
		if (p1.y > p2.y)
		{
			end = (CVector3D *)&p1;
			begin = (CVector3D *)&p2;
		}

		p.x = p1.x;
		p.y = (end->y - begin->y)/2;

		float  z = p1.z + (GetPointDist(p1,p)/(GetPointDist(p1,p2)))*(p2.z - p1.z);

		if ((end->y <=  p.y) && (begin->y >=  p.y))
		{
			float dist = GetPointDist(p,p0);

			if (endIndex != -1)
			{
				if ((minDist > dist) && (dist <= MAX_DIST_TO_PATH))
				{
					m_intersectionPoints[endIndex]->pnt.Set(p.x, p.y, z);
					m_intersectionPoints[endIndex]->segment = segment;
					minDist = dist;
					return true;
				}

			}
			else if(dist <= MAX_DIST_TO_PATH)
			{
				if (m_intersectionPoints.size() ==1)
				{
					if (m_intersectionPoints[0]->segment->groupId == segment->groupId)
					{
						MP_NEW_P2(_info, SIntersectionPointsInfo, CVector3D(p.x, p.y, z), segment);
						m_intersectionPoints.push_back(_info);
						return true;
					}
				}
				else
				{
					m_intersectionPoints[m_intersectionPoints.size()-1]->pnt.Set(p.x, p.y, z);
					m_intersectionPoints[m_intersectionPoints.size()-1]->segment = segment;
					return true;
				}
			}

			return true;
		}

	}

	return false;
}


int CPathFinder::GetStartAndEndSegments(SSegment*& startSegm, SSegment*& endSegm, int pointIndex)
{
	
	int id = (*m_intersectionPoints[pointIndex]).segment->groupId;
	startSegm = (*m_intersectionPoints[pointIndex]).segment;

	int i  = pointIndex + 1, numberOfIntersectionPointsInOnePath = 1; 

	while (i < (int)m_intersectionPoints.size())
	{
		if (id == (*m_intersectionPoints[i]).segment->groupId)
		{
			numberOfIntersectionPointsInOnePath++;

			endSegm = (*m_intersectionPoints[i]).segment;
		}
		else
		{
			break;
		}

		i++;
	}

	return numberOfIntersectionPointsInOnePath;
}


void CPathFinder::SortIntersectionPoints(const CVector3D begin, const CVector3D end)
{
	SIntersectionPointsInfo v;
	float deltaX = abs(begin.x - end.x);
	float deltaY = abs(begin.y - end.y);
	for (int i = m_intersectionPoints.size() -1; i > 0 ; i--)
		for (int j = 0; j < i; j++)
		{
		  if (deltaX > deltaY)
		  {
			if ( ((begin.x < end.x) && (*m_intersectionPoints[j]).pnt.x > (*m_intersectionPoints[j+1]).pnt.x)
			  || ((begin.x > end.x) && (*m_intersectionPoints[j]).pnt.x < (*m_intersectionPoints[j+1]).pnt.x))
			{
				v = (*m_intersectionPoints[j]);
				(*m_intersectionPoints[j]) = (*m_intersectionPoints[j+1]);
				(*m_intersectionPoints[j+1]) = v;
			}
		  }
		  else
		  {
			  if ( ((begin.y < end.y) && (*m_intersectionPoints[j]).pnt.y > (*m_intersectionPoints[j+1]).pnt.y)
				  || ((begin.y > end.y) && (*m_intersectionPoints[j]).pnt.y < (*m_intersectionPoints[j+1]).pnt.y))
			  {
				  v = (*m_intersectionPoints[j]);
				  (*m_intersectionPoints[j]) = (*m_intersectionPoints[j+1]);
				  (*m_intersectionPoints[j+1]) = v;
			  }
		  }
		}
}

void CPathFinder::ClearIntersectionPoints()
{
	std::vector<SIntersectionPointsInfo*>::iterator it = m_intersectionPoints.begin();
	std::vector<SIntersectionPointsInfo*>::iterator itEnd = m_intersectionPoints.end();

	for (; it != itEnd; it++)
	{
		MP_DELETE(*it);
	}

	m_intersectionPoints.clear();
	m_intersectionPoints.reserve(20);
}

void CPathFinder::OnPropertyBlockLoaded(const void* data, const unsigned int size, const std::string& /*fileName*/, const unsigned int modelID)
{
	if ((0 == size) || (!data))
	{
		return;
	}

	std::map<int, SPathLinesBlock>::iterator it = m_pathLinesByModelID.find(modelID);	

	if (it != m_pathLinesByModelID.end())
	{
		m_pathLinesByModelID.erase(it);
	}

	unsigned char* savedData = MP_NEW_ARR(unsigned char, size);
	memcpy(savedData, data, size);
	SPathLinesBlock block(savedData, size);
	m_pathLinesByModelID.insert(std::map<int, SPathLinesBlock>::value_type(modelID, block));
}

void CPathFinder::OnObjectCreated(int objectID, int modelID, bool isUpdate)
{
	{
		MP_MAP<int, MP_VECTOR<SSegment*> >::iterator it = m_segments.find(objectID);
		if ((it != m_segments.end()) && (!isUpdate))
		{
			return;
		}
	}	

	// если пришел аватар - object->GetAnimation()!= NULL
	C3DObject* object = GetObjectPointer(objectID);

	if ((!object) || (object->IsNeedToSwitchOffPathes()))
	{
		return;
	}
	
	std::wstring str = object->GetName();

	if (str == L"__o3de_phantom_copy_" )
	{
		return;
	}

	CVector3D v = object->GetCoords();
	CQuaternion q = object->GetRotation()->GetAsDXQuaternion();

	if (object->GetAnimation()!= NULL)
	{
		MP_NEW_P7(_do, SPathNeededObjectsInfo, v.x + 1, v.y, v.z, q, objectID, object, AVATAR);
		m_dinamicObjects.push_back(_do);
 		CreateQuadrPath(objectID, -1);
		return;
	}	
	else if (!object->IsVisible())
	{
		return;
	}

	std::map<int, SPathLinesBlock>::iterator it = m_pathLinesByModelID.find(modelID);

	if (it != m_pathLinesByModelID.end())
	{
		// pathlines для модели есть, надо добавить сегменты с id = objectID

		SPathLinesBlock pathLinesBlock = (*it).second;		
		// pathLinesBlock.data - данные
		// pathLinesBlock.size - их размер
		// x, y, z - надо добавить к относительным координатам пути

		if (object->GetUserData(USER_DATA_EDITABLE)!= NULL)
		{
			MP_NEW_P7(_do, SPathNeededObjectsInfo, v.x + 1, v.y, v.z, q, objectID, object, DINAMIC);
			m_dinamicObjects.push_back(_do);
		}

		if (pathLinesBlock.size == 13 * sizeof(char))
		{
			CreateQuadrPath(objectID, modelID);
		}
		else if (pathLinesBlock.size == 15 * sizeof(char))
		{
			CreateCirclePath(objectID, modelID);
		}
		else if ((pathLinesBlock.size % (sizeof(float)*6) == 0 ) || ((pathLinesBlock.size - 4 )%(sizeof(float)*6+1) == 0))
		{
			q.Normalize();

			int size = 0;
			int offset = 0;

			if (pathLinesBlock.size % (sizeof(float)*6) == 0)
			{
				size = pathLinesBlock.size/sizeof(float);
			}
			else
			{
				size = (pathLinesBlock.size-4)/25*6;
			}

			int format = 0;

			for(int i = 0 ; i < size; i = i + 6)
			{
				CVector3D v1, v2;
				unsigned char typeOfSegment = 0;
				

				if (i == 0)
				{
					if ((*(pathLinesBlock.data + 1) == 'P') && (*(pathLinesBlock.data  + 2) == 'L') && (*(pathLinesBlock.data + 3) == '1'))
					{
						format = 1;
						pathLinesBlock.data +=4;
					}
				}

				if (!ParseSegment(pathLinesBlock, i , v, q, &v1, offset))
				{
					break;
				}
				if (!ParseSegment(pathLinesBlock, i+3, v, q,  &v2, offset))
				{
					break;
				}

				if (format == 1)
				{
					typeOfSegment = *(unsigned char*)(pathLinesBlock.data + (i+6)*sizeof(float) + offset);
					offset++;
				}

				AddSegment(CVector3D(v1.x, v1.y, v1.z), CVector3D(v2.x, v2.y, v2.z), objectID, typeOfSegment);	
			}

			if (format == 1)
			{
				pathLinesBlock.data -=4;
			}
			
		}
		else 
		{
			return;
		}
	}
}

void CPathFinder::LogDynamicObject(int id)
{
	std::vector<SPathNeededObjectsInfo*>::iterator itA = m_dinamicObjects.begin();

	while ( itA != m_dinamicObjects.end())
	{
		if (((*itA)->id == id) || ((*itA)->id == -id))
		{
			g->lw.WriteLn("object: ", (*itA)->object->GetName());
			CVector3D coords = (*itA)->object->GetCoords();
			g->lw.WriteLn("coords: ", coords.x, " ", coords.y, " ", coords.z);

			break;
		}
		itA++;
	}
}

//Проверять не изменилась ли позиция и поворот.
//Если изменились, то заменить координаты сегментов (без удаления сегментов и добавления новых).
void CPathFinder::UpdateAvatar()
{	
	if (!g->phom.GetControlledObject())
	{
		return;
	}

	std::vector<SPathNeededObjectsInfo*>::iterator itA = m_dinamicObjects.begin();

	while ( itA != m_dinamicObjects.end())
	{
		if ((*itA)->typeOfObject == AVATAR)
		{
			bool isNeedToDelete = (*itA)->object->IsDeleted();
			
			if (g->phom.GetControlledObject())
				if ((*itA)->object == g->phom.GetControlledObject()->GetObject3D())
				{					
					isNeedToDelete = true;
				}

				if (isNeedToDelete)
				{
					MP_MAP<int, MP_VECTOR<SSegment*> >::iterator it1 = m_segments.find((*itA)->id);

					if (Delete3DObjectsPath(it1))
					{
						m_isShowNearestAfterChangePath = true;
					}
					
					MP_DELETE(*itA);
					itA = m_dinamicObjects.erase(itA);				
				}
				else 
				{
					itA++;
				}
		}
		else
		{
			itA++;
		}
	}

	itA = m_dinamicObjects.begin(); 

	std::vector<SPathNeededObjectsInfo*>::iterator itEndA = m_dinamicObjects.end();

	CVector3D coord;
	CQuaternion rotate;

	for( ; itA != itEndA; itA++)
	{
		if ((*itA)->typeOfObject != AVATAR)
			continue;

		coord = (*itA)->object->GetCoords();
		rotate = (*itA)->object->GetRotation()->GetAsDXQuaternion();

		if ( coord != (*itA)->coord || rotate != (*itA)->rotate )
		{
			(*itA)->coord = coord;
			(*itA)->rotate = rotate;

			FIND_OBJECTS_SEGMENTS_OR_CONTINUE((*itA)->id);

			int numberOfChangedSegment = 0;

			for( ; it != itEnd; it++)
			{
				CVector3D minBB = CVector3D(-AVATAR_HALF_WIDTH, - AVATAR_HALF_LENGTH, (*itA)->coord.z);
				CVector3D maxBB = CVector3D(AVATAR_HALF_WIDTH, AVATAR_HALF_LENGTH, (*itA)->coord.z);

				UpdateSegmentsQuadrPoints(numberOfChangedSegment, &minBB, &maxBB, &rotate, &coord, &(*it)->begin, &(*it)->end);

				UpdatePathesBB(numberOfChangedSegment, (*itA)->id, (*it));

				numberOfChangedSegment++;
			}
		}
	}
}

bool CPathFinder::IsCanBeMerged(SPathNeededObjectsInfo* info)
{
	if (info->update == FULL_UPDATE)
	{
		return false;
	}

	if (info->object)
	{
		CVector3D coord = info->object->GetCoords();
		CQuaternion	rotate = info->object->GetRotation()->GetAsDXQuaternion();
		bool isCoordsChanged = coord != info->coord || rotate != info->rotate;

		if (isCoordsChanged)
		{
			return false;
		}
	}

	return true;
}

void CPathFinder::CheckInactiveObjectsInMainSegments()
{
	return;
	/*std::vector<SPathNeededObjectsInfo*>::iterator itA = m_dinamicObjects.begin();

	while ( itA != m_dinamicObjects.end())
	{
		if ((*itA)->id < 0)
		{
			MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itV = m_segments.find(-((*itA)->id));	

			if (itV != m_segments.end())
			{
				int ii = 0;
			}
		}
		
		itA++;
	}

	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator it = m_segments.begin();
	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itEnd = m_segments.end();


	for ( ; it != itEnd; it++)
	{
		std::vector<SSegment*>::iterator itV = (*it).second.begin();
		std::vector<SSegment*>::iterator itVEnd = (*it).second.end();

		for ( ; itV != itVEnd; itV++)
		{ 
			if ((*itV)->groupId < 0)
			{
				// проверяем нет ли такого объекта во временных, в элементе которого он не ключ
				MP_MAP<int, MP_VECTOR<int> >::iterator itPr = m_provisionalDinamicObject.begin();
				MP_MAP<int, MP_VECTOR<int> >::iterator itPrEnd = m_provisionalDinamicObject.end();

				for (; itPr !=itPrEnd; itPr++)
				//if (itPr != m_provisionalDinamicObject.end())
				{
					for (int i=0; i< (*itPr).second.size(); i++)
					{
						if ((*itPr).second[i] == (-(*itV)->groupId))
						{
							//MP_MAP<int, MP_VECTOR<int> >::iterator itCh = m_provisionalDinamicObject.find((*itPr).second[i]);
							//{
								if ((*itPr).first !=(*itPr).second[i])
								{
									MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itS = m_segments.find(-(*itPr).first);
									if (itS != m_segments.end())
									{
										int ii = 0;
									}

									int ii = 0;
								}
							//}
						}
					}
				}
			}
		}
	}*/
}

void CPathFinder::UpdateDinamicObjects()
{
	if (!g->phom.GetControlledObject())
	{
		return;
	}

	// восстановление аватаров, проверяем, что живы
	{
		std::vector<CSkeletonAnimationObject *>::iterator it = g->skom.GetLiveObjectsBegin();
		std::vector<CSkeletonAnimationObject *>::iterator itEnd = g->skom.GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		if ((!(*it)->GetParent()) && ((*it)->GetObject3D()) &&
			((*it)->GetObject3D()->IsVisible()))			
		if ((*it)->GetObject3D() != g->phom.GetControlledObject()->GetObject3D())
		{
			int objectID = (*it)->GetObject3D()->GetID();

			bool isNeedToCreate = false;

			MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itS = m_segments.find(objectID);

			if (itS == m_segments.end())
			{
				isNeedToCreate = true;

				// если нет в m_segments, смотрим в неактивных
				MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itInAct = m_inactiveDinamicObjects.find(objectID);

				if ( itInAct != m_inactiveDinamicObjects.end())
				{
					MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itGroupD = m_segments.find(-objectID);

					// если не нашли в m_segments, смотрим в векторе временных объектов 
					if (itGroupD == m_segments.end())
					{
						// если нет в неактивных по ключу, смотрим ID объединенных объектов с данным
						MP_MAP<int, MP_VECTOR<int> >::iterator itPr = m_provisionalDinamicObject.begin();
						MP_MAP<int, MP_VECTOR<int> >::iterator itPrEnd = m_provisionalDinamicObject.end();

						for (; itPr !=itPrEnd; itPr++)
						{
							for (unsigned int i=0; i< (*itPr).second.size(); i++)
							{
								if ((*itPr).second[i] == objectID)
								{	
									itGroupD = m_segments.find(-(*itPr).first);
									if (itGroupD!=m_segments.end())
									{
										isNeedToCreate = false;
										break;
									}
								}
							}
						}
					}
					else
					{
						isNeedToCreate = false;
					}
				}
			}
			

			if (isNeedToCreate)
			{
				float x, y, z;
				(*it)->GetObject3D()->GetCoords(&x, &y, &z);
				OnObjectCreated(objectID, -1);
			}
		}
	}

	std::vector<SPathNeededObjectsInfo*>::iterator itA = m_dinamicObjects.begin();

	while ( itA != m_dinamicObjects.end())
	{
		if ( ((*itA)->typeOfObject == DINAMIC) || ((*itA)->typeOfObject == AVATAR))
		{
			bool isNeedToDelete = (*itA)->object->IsDeleted();
			bool isVisible =  (*itA)->object->IsVisible();

			bool isDelete = false;

			if (g->phom.GetControlledObject())
			if ((*itA)->object == g->phom.GetControlledObject()->GetObject3D())
			{
				isNeedToDelete = true;
			}

			if (((*itA)->object->GetAnimation()) && ((*itA)->object->GetParent()))
			{
				isNeedToDelete = true;
			}

			if ((isNeedToDelete) || (!isVisible) )
			{
				MP_MAP<int, MP_VECTOR<SSegment*> >::iterator it1 = m_segments.find((*itA)->id);
				if ( it1 != m_segments.end())
				{
					int id = (*itA)->id;
					isDelete = ReturnObjectsFromInactiveList(id);
					if (id < 0)
					{
						it1 = m_segments.find(-id);
					}

					Delete3DObjectsPath(it1);			
					m_isShowNearestAfterChangePath = true;
				}

				if (!isDelete)
				{
					MP_DELETE(*itA);
					itA = m_dinamicObjects.erase(itA);
				}				
			}
			else 
			{
				itA++;
			}
		}
		else
		{
			itA++;
		}
	}

	itA = m_dinamicObjects.begin();

	CVector3D coord;
	CQuaternion rotate;

	bool isSomethingMerged = false;

	for( ; itA !=  m_dinamicObjects.end(); itA++)
	{
		//std::string name =(*itA)->object->GetName();

		//if ((*itA)->typeOfObject != DINAMIC)
			//continue;

		coord = (*itA)->object->GetCoords();
		rotate = (*itA)->object->GetRotation()->GetAsDXQuaternion();
		bool isCoordsChanged = coord != (*itA)->coord || rotate != (*itA)->rotate;

		if ((isCoordsChanged) || ((*itA)->update != NO_UPDATE)) 
		{
			(*itA)->coord = coord;
			(*itA)->rotate = rotate;

			if ( ((*itA)->update == FULL_UPDATE) || (isCoordsChanged) )
			if (ReturnObjectsFromInactiveList((*itA)->id))
			{
				itA = m_dinamicObjects.begin();
				itA--;				
				continue;
			}

			(*itA)->update = NO_UPDATE;
			
			FIND_OBJECTS_SEGMENTS_OR_CONTINUE((*itA)->id);

			int numberOfChangedSegment = 0;
			
			std::vector<SSegment*> objectsSegment;

			if ((*itA)->id > 0)
			{
				for (; it!=itEnd; it++)
				{
					objectsSegment.push_back((*it));
				}
			}
			else
			{
				MP_MAP<int, MP_VECTOR<int> >::iterator it = m_provisionalDinamicObject.find(-(*itA)->id);
				if (it != m_provisionalDinamicObject.end())
				{
					for (unsigned int k = 0; k < (*it).second.size(); k++)
					{
						MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itIAO = m_inactiveDinamicObjects.find((*it).second[k]);
						if (itIAO != m_inactiveDinamicObjects.end())
						{
							std::vector<SSegment*>::iterator it = (*itIAO).second.begin();
							std::vector<SSegment*>::iterator itEnd = (*itIAO).second.end();
							for (; it!=itEnd; it++)
							{
								objectsSegment.push_back((*it));
							}
						}
					}
				}
			}

			it = objectsSegment.begin();
			itEnd = objectsSegment.end();

			SLODLevel* level = (*itA)->object->GetLODGroup()->GetLodLevel(0);
			int modelID = level ? level->GetModelID() : -1;

			std::map<int, SPathLinesBlock>::iterator itPL = m_pathLinesByModelID.find(modelID);

			if ((*itA)->id >= 0)
			if ( ((*itA)->typeOfObject == AVATAR) || ((itPL != m_pathLinesByModelID.end()) && (objectsSegment.size() != 0)))
			{
				int i = 0;
				int format = 0;

				SPathLinesBlock pathLinesBlock = (*itPL).second;

				for (; it!=itEnd; it++)
				{
					(*it)->active = true;

					if ((*itA)->typeOfObject == AVATAR)
					{
						CVector3D minBB = CVector3D(-AVATAR_HALF_WIDTH, - AVATAR_HALF_LENGTH, (*itA)->coord.z);
						CVector3D maxBB = CVector3D(AVATAR_HALF_WIDTH, AVATAR_HALF_LENGTH, (*itA)->coord.z);

						UpdateSegmentsQuadrPoints(numberOfChangedSegment, &minBB, &maxBB, &rotate, &coord, &(*it)->begin, &(*it)->end);

						UpdatePathesBB(numberOfChangedSegment, (*itA)->id, (*it));

						numberOfChangedSegment++;
					}
					else if (pathLinesBlock.size == 13 * sizeof(char))
					{
						CVector3D minBB, maxBB;

						BoundingBoxPointsConversion((*itA)->id, modelID, rotate, coord, minBB, maxBB);

						minBB.z = (*itA)->coord.z;

						UpdateSegmentsQuadrPoints(numberOfChangedSegment, &minBB, &maxBB, &rotate, &coord, &(*it)->begin, &(*it)->end);

						UpdatePathesBB( numberOfChangedSegment, (*itA)->id, (*it));

						numberOfChangedSegment++;

					}
					else if (pathLinesBlock.size == 15 * sizeof(char))
					{
						CVector3D minBB, maxBB;

						BoundingBoxPointsConversion((*itA)->id, modelID, rotate, coord, minBB, maxBB);

						minBB.z = 0.0f;

						UpdateSegmentsCirclePoints(i, &minBB, &maxBB, &rotate, &coord, &(*it)->begin, &(*it)->end);

						UpdatePathesBB(i, (*itA)->id, (*it));	

						i++;

					}
					else
					{
						//std::string name1 = g->o3dm.GetObjectPointer((*itA)->id)->GetName();
						rotate.Normalize();
						
						int offset = 0;

						if (i == 0)
						{
							if ((*(pathLinesBlock.data +1) == 'P') && (*(pathLinesBlock.data  + 2) == 'L') && (*(pathLinesBlock.data + 3) == '1'))
							{
								format = 1;
								pathLinesBlock.data +=4; 
							}
						}

						
						ParseSegment(pathLinesBlock, i, coord, rotate, &(*it)->begin, offset);
						ParseSegment(pathLinesBlock, i+3, coord, rotate,  &(*it)->end, offset);

						if (format == 1)
						{
							offset++;
						}
						else
						{
							i += 6;
						} 
					}
				}

				if (format == 1)
				{
					pathLinesBlock.data -=4; 
				}
				
			}
			
			if (MergeGraphIfNeed(&objectsSegment))
			{
				isSomethingMerged = true; 
			}
		}

		if ((isSomethingMerged) && (itA + 1 == m_dinamicObjects.end())) 
		{ 
			if (m_mergeDinamicObjects.size() > 0)
	{
		std::vector<SPathNeededObjectsInfo*>::iterator itD = m_mergeDinamicObjects.begin();
		std::vector<SPathNeededObjectsInfo*>::iterator itDEnd = m_mergeDinamicObjects.end();

		for (; itD != itDEnd; itD++)
		{
			m_dinamicObjects.push_back((*itD));
		}

		m_mergeDinamicObjects.clear();
	}

			itA = m_dinamicObjects.begin();	
			itA--; 
			isSomethingMerged = false; 
		}		
	}

	if (m_mergeDinamicObjects.size() > 0)
	{
		std::vector<SPathNeededObjectsInfo*>::iterator itD = m_mergeDinamicObjects.begin();
		std::vector<SPathNeededObjectsInfo*>::iterator itDEnd = m_mergeDinamicObjects.end();

		for (; itD != itDEnd; itD++)
		{
			m_dinamicObjects.push_back((*itD));
		}

		m_mergeDinamicObjects.clear();
	}
}

//Если один из составляющих временный объект объектов сдвинулся, то надо вернуть все составляющие его объекты в список активных,
//временный объект удалить
bool CPathFinder::ReturnObjectsFromInactiveList(int paramId, std::vector<SPathNeededObjectsInfo*>::iterator* /*itA*/)
{
	bool isDelete = false;

	if (m_inactiveDinamicObjects.size() != 0)
	{
		int id = paramId;

		MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itInAct = m_inactiveDinamicObjects.find(abs(paramId));

		// объект правда в числе неактивных
		if ( itInAct != m_inactiveDinamicObjects.end())
		{
			MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itGroupD = m_segments.find(-paramId);

			std::vector<SPathNeededObjectsInfo*>::iterator itD = m_dinamicObjects.begin();
			std::vector<SPathNeededObjectsInfo*>::iterator itDEnd = m_dinamicObjects.end();

			// именно этот объект заменен с инвертированным id
			if (itGroupD != m_segments.end())
			{
				// удаляем из m_segments
				Delete3DObjectsPath(itGroupD);								
			}
			else // объект входит в состав объединенного объекта, но не он заменен с инвертированным id
			{
				// найдем id у объединенного объекта
				MP_MAP<int, MP_VECTOR<int> >::iterator itPr = m_provisionalDinamicObject.begin();
				MP_MAP<int, MP_VECTOR<int> >::iterator itPrEnd = m_provisionalDinamicObject.end();

				id = -1;

				for (; itPr !=itPrEnd; itPr++)
				{
					for (unsigned int i=0; i< (*itPr).second.size(); i++)
					{
						if ((*itPr).second[i] == abs(paramId))
						{
							id = (*itPr).first;
							break;
						}
					}
				}

				if (id != -1) // id найден
				{
					itGroupD = m_segments.find(-id);					

					// удаляем из m_segments
					Delete3DObjectsPath(itGroupD);											
				}
			}

			// удаляем из m_dinamicObjects
			for( ; itD!= itDEnd; itD++)
			{
				if ((*itD)->id == -id)
				{
					MP_DELETE(*itD);
					m_dinamicObjects.erase(itD);
					isDelete = true;
					break;
				}
			}

			// вместо удаленного временного надо вернуть его составные части
			MP_MAP<int, MP_VECTOR<int> >::iterator itPr = m_provisionalDinamicObject.find(id);

			if ( itPr !=  m_provisionalDinamicObject.end()) // части объекта найдены
			{
				std::vector<int>::iterator itAct = (*itPr).second.begin();
				std::vector<int>::iterator itActEnd = (*itPr).second.end();

				for (; itAct!= itActEnd; itAct++) // идем по составным частям
				{
					itD = m_dinamicObjects.begin();
					itDEnd = m_dinamicObjects.end();

					// находим в m_dinamicObjects составную часть и проставляем ей принудительное обновление
					for( ; itD != itDEnd; itD++)
					{
						if ((*itD)->id == (*itAct))
						{
							(*itD)->update = FULL_UPDATE;
							break;
						}						
					}

					// находим сегменты составной части
					MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itInActObject = m_inactiveDinamicObjects.find((*itAct));

					if (itInActObject != m_inactiveDinamicObjects.end())
					{
						// возвращаем их в m_segments
						//CheckInactiveObjectsInMainSegments();						

						/*MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itS1 = m_segments.find(-(*itAct));
						if (itS1 != m_segments.end())
						{
							m_segments.erase(itS1);
						}*/

						InsertInMainSegments((*itAct), (*itInActObject).second);

						//CheckInactiveObjectsInMainSegments();

						m_inactiveDinamicObjects.erase(itInActObject);
					}
				}

				m_provisionalDinamicObject.erase(itPr);
			}

		}
	}

	return isDelete;
}

void CPathFinder::InsertInMainSegments(int id, std::vector<SSegment*> vec)
{
	CheckInactiveObjectsInMainSegments();
	m_segments.insert(MP_MAP<int, MP_VECTOR<SSegment*> >::value_type(id, MAKE_MP_VECTOR(vec, SSegment*)));
	CheckInactiveObjectsInMainSegments();
}

bool CPathFinder::ProcessIntersectSegments(std::vector<SSegment*>::iterator it, std::vector<SSegment*>::iterator itEnd,
										   std::vector<SSegment*>& newPath, std::vector<SIntersectionPointsInfo*>& points,
										   std::vector<int>& dinamicObjectsIDWithIntersect)
{
#define MIN(a, b) ((a > b) ? b : a)
#define MAX(a, b) ((a < b) ? b : a)
#define SMALL_STEP	2.0f

	bool isChanged = false;

	for ( ; it != itEnd; it++)
	{
		bool isIntersectSegment = false;				

		bool isCanAddWithoutCheck = false;	

		bool isAddWithBeginPoint = true, isAddWithEndPoint = true;

		bool isEndIntersectPoint = false;
		bool isBeginIntersectPoint = false;
		
		unsigned int j;
		for (j=0 ; j < points.size(); j++)
		{					
			if ((*it)->begin == points[j]->pnt)
			{
				isBeginIntersectPoint = true;
				isCanAddWithoutCheck = true;						
			}
			
			if ((*it)->end == points[j]->pnt)
			{
				isEndIntersectPoint = true;						
				isCanAddWithoutCheck = true;						
			}
		}

		for (unsigned int i = 0 ; i < dinamicObjectsIDWithIntersect.size(); i++)
		{
			if (dinamicObjectsIDWithIntersect[i] != (*it)->groupId)
			{
				MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itInAc= m_inactiveDinamicObjects.find(dinamicObjectsIDWithIntersect[i]);

				if (itInAc!= m_inactiveDinamicObjects.end())
				{
					float x = (*it)->begin.x;
					float y = (*it)->begin.y;

					if (isBeginIntersectPoint)
					{
						CVector2D delta((*it)->end.x - (*it)->begin.x, (*it)->end.y - (*it)->begin.y);
						delta.Normalize();
						x += delta.x * SMALL_STEP;
						y += delta.y * SMALL_STEP;
					}

					if (IsPointInPoly(&(*itInAc).second, x, y))
					{
						isAddWithBeginPoint = false;
					}

					x = (*it)->end.x;
					y = (*it)->end.y;

					if (isEndIntersectPoint)
					{
						CVector2D delta((*it)->end.x - (*it)->begin.x, (*it)->end.y - (*it)->begin.y);
						delta.Normalize();
						x -= delta.x * SMALL_STEP;
						y -= delta.y * SMALL_STEP;
					}

					if	(IsPointInPoly(&(*itInAc).second, x, y ))
					{
						isAddWithEndPoint = false;
					}

				}		
			}
		}							

		for (j=0 ; j < points.size(); j++)
		{					
			if (points[j]->segment == (*it))
			{
				SSegment* segment = points[j]->segment;
				
				SSegment* segments[2];
				segments[0] = segments[1] = NULL;

				if (isAddWithBeginPoint)
				{
					MP_NEW_V8(segments[0], SSegment, (*it)->begin.x, (*it)->begin.y, (*it)->begin.z , points[j]->pnt.x, points[j]->pnt.y, points[j]->pnt.z , (*it)->groupId,  (*it)->typeOfSegment);					 
					newPath.push_back(segments[0]);
					isIntersectSegment = true;
					isCanAddWithoutCheck = false;
				}

				if (isAddWithEndPoint)
				{
					MP_NEW_V8(segments[1], SSegment, points[j]->pnt.x, points[j]->pnt.y, points[j]->pnt.z, (*it)->end.x, (*it)->end.y, (*it)->end.z  , (*it)->groupId,  (*it)->typeOfSegment);
					newPath.push_back(segments[1]);
					isIntersectSegment = true;
					isCanAddWithoutCheck = false;
				}

				for (unsigned int k = 0; k< points.size(); k++)
				if (k != j)
				{
					if (segment == 	points[k]->segment) 
					{
						for (int s = 0; s < 2; s++)
						if (segments[s] != NULL)
						{
							float x1 = MIN(segments[s]->begin.x, segments[s]->end.x);
							float x2 = MAX(segments[s]->begin.x, segments[s]->end.x);
							if ((points[k]->pnt.x > x1) && (points[k]->pnt.x < x2))
							{
								points[k]->segment = segments[s];
								isChanged = true;
							}
						}
					}
				}				
			}
		}

		if (((!isIntersectSegment) && (isAddWithBeginPoint) && (isAddWithEndPoint)) || (isCanAddWithoutCheck))
		{
			MP_NEW_P8(_segm, SSegment, (*it)->begin.x, (*it)->begin.y, (*it)->begin.z , (*it)->end.x, (*it)->end.y, (*it)->end.z, (*it)->groupId,  (*it)->typeOfSegment);
			newPath.push_back(_segm);
		}
	}

	return isChanged;
}

void CPathFinder::UpdateBoundingBox(int objectID)
{
	FIND_OBJECTS_SEGMENTS_OR_RETURN(objectID);

	PathesBB bb;
	bb.min = CVector3D(100000000.0f, 100000000.0f, 0.0f);
	bb.max = CVector3D(-100000000.0f, -100000000.0f, 0.0f);	

	for ( ; it != itEnd; it++)
	{
        if ((*it)->begin.x > bb.max.x)
		{
			bb.max.x = (*it)->begin.x;
		}

		if ((*it)->begin.y > bb.max.y)
		{
			bb.max.y = (*it)->begin.y;
		}

		if ((*it)->end.x > bb.max.x)
		{
			bb.max.x = (*it)->end.x;
		}

		if ((*it)->end.y > bb.max.y)
		{
			bb.max.y = (*it)->end.y;
		}

		if ((*it)->begin.x < bb.min.x)
		{
			bb.min.x = (*it)->begin.x;
		}

		if ((*it)->begin.y < bb.min.y)
		{
			bb.min.y = (*it)->begin.y;
		}

		if ((*it)->end.x < bb.min.x)
		{
			bb.min.x = (*it)->end.x;
		}

		if ((*it)->end.y < bb.min.y)
		{
			bb.min.y = (*it)->end.y;
		}
	}

	C3DObject* obj = GetObjectPointer(objectID);
	bb.min.z = obj->GetCoords().z;
	bb.max.z = obj->GetCoords().z;

	std::map<int, PathesBB>::iterator itBB = m_pathesBB.find(objectID);
	if (itBB != m_pathesBB.end())
	{
		(*itBB).second = bb;
	}
	else
	{
		m_pathesBB.insert(std::map<int, PathesBB>::value_type(objectID, bb));
	}
}

bool CPathFinder::MergeGraphIfNeed(std::vector<SSegment*>* objectSegments)
{
	if ((*objectSegments).size() == 0)
	{
		return false;
	}

	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itTest= m_inactiveDinamicObjects.find((*objectSegments)[0]->groupId);

	if (itTest != m_inactiveDinamicObjects.end())
	{
		return false;
	}

	std::vector<SPathNeededObjectsInfo*>::iterator itD = m_dinamicObjects.begin();
	std::vector<SPathNeededObjectsInfo*>::iterator itEndD = m_dinamicObjects.end();

	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itVec = m_segments.find((*objectSegments)[0]->groupId);

	if (itVec == m_segments.end())
	{
		return false;
	}

	std::vector<SIntersectionPointsInfo*> points;
	std::vector<int> dinamicObjectsIDWithIntersect;
	std::vector<SSegment*> newPath;

	unsigned int size = 0;

	for (; itD!=itEndD; itD++)
	//if ((*itD)->update != FULL_UPDATE)
	if ((IsCanBeMerged(*itD)) && ((*itD)->id != -(*objectSegments)[0]->groupId))
	{
		if ((*objectSegments)[0]->groupId != (*itD)->id)
		{
			FIND_OBJECTS_SEGMENTS_OR_CONTINUE((*itD)->id);

			size = points.size();

			// находим точки пересечения динамического объекта со всеми остальными динамическими объектами
			for (; it!=itEnd; it++)			
			{
				// если объект входит в число неактивных то исключаем его их поиска ТП с ним, а берем ТП с временным объектом, в кот он входит
				bool isInAct = false;

				if (m_provisionalDinamicObject.size() != 0)
				{
					MP_MAP<int, MP_VECTOR<int> >::iterator itProv = m_provisionalDinamicObject.find((*it)->groupId);

					if (itProv != m_provisionalDinamicObject.end())
					{
						isInAct = true;
					}
					else
					{
						itProv = m_provisionalDinamicObject.begin();
						MP_MAP<int, MP_VECTOR<int> >::iterator itProvEnd = m_provisionalDinamicObject.end();

						for (; itProv != itProvEnd; itProv++)
						{
							std::vector<int>::iterator itAct = (*itProv).second.begin();
							std::vector<int>::iterator itActEnd = (*itProv).second.end();

							for (; itAct!= itActEnd; itAct++)
							{
								if ((*it)->groupId == (*itAct))
								{
									isInAct = true;
									break;
								}
							}
						}
					}
				}

				if (isInAct) continue;

				for (unsigned int i = 0; i < (*objectSegments).size(); i++)
				{	
					unsigned int sizePoints = points.size();

					FindIntersectionPointWithSegment((*objectSegments)[i], (*it)->begin, (*it)->end, &points);

					if (sizePoints != points.size())
					{
						MP_NEW_P2(_info, SIntersectionPointsInfo, points[points.size()-1]->pnt, *it);
						points.push_back(_info);
					}
				}
			}

			if (size != points.size())
			{
				if ((*itD)->id > 0)
				{
					dinamicObjectsIDWithIntersect.push_back((*itD)->id);				
				}
				else
				{
					MP_MAP<int, MP_VECTOR<int> >::iterator it = m_provisionalDinamicObject.find(-(*itD)->id);
					if (it != m_provisionalDinamicObject.end())
					for (unsigned int k = 0; k < (*it).second.size(); k++)
					{
						dinamicObjectsIDWithIntersect.push_back((*it).second[k]);
					}
				}

				if ((*itD)->id > 0)
				{
					m_inactiveDinamicObjects.insert(MP_MAP<int, MP_VECTOR<SSegment*> >::value_type((*itD)->id, MAKE_MP_VECTOR((*itV).second, SSegment*)));
				}
			}
		}
	}

	bool isFoundEqualID = (dinamicObjectsIDWithIntersect.size() > 0);

	while (isFoundEqualID)
	{
		isFoundEqualID = false;

		for (unsigned int i = 0; i < dinamicObjectsIDWithIntersect.size() - 1; i++)
		{
			for (unsigned int j = i + 1; j < dinamicObjectsIDWithIntersect.size(); j++)
			if (dinamicObjectsIDWithIntersect[i] == dinamicObjectsIDWithIntersect[j])
			{
				dinamicObjectsIDWithIntersect.erase(dinamicObjectsIDWithIntersect.begin() + j);
				isFoundEqualID = true;
				break;
			}

			if (isFoundEqualID)
			{
				break;
			}
		}		
	}

	bool isGraphMerged = false;

	//все сегменты проверяются и остаются только те, которые обоими концами находятся не внутри одного из путей динамического объекта
	if (points.size() != 0)
	{
		dinamicObjectsIDWithIntersect.push_back((*objectSegments)[0]->groupId);
		if ((*objectSegments)[0]->groupId > 0)
		{
			m_inactiveDinamicObjects.insert(MP_MAP<int, MP_VECTOR<SSegment*> >::value_type((*objectSegments)[0]->groupId, MAKE_MP_VECTOR((*itVec).second, SSegment*)));
		}

		bool isChanged = false;

		for (unsigned int i = 0; i< dinamicObjectsIDWithIntersect.size(); i++)
		{
			FIND_OBJECTS_SEGMENTS_OR_CONTINUE(dinamicObjectsIDWithIntersect[i]);

			/*std::map<int, PathesBB>::iterator itBB = m_pathesBB.find(dinamicObjectsIDWithIntersect[i]);
			if ( itBB != m_pathesBB.end())
			{
				if (minX  > (*itBB).second.min.x) 
				{
					minX = (*itBB).second.min.x;
				}

				if (maxX < (*itBB).second.max.x) 
				{
					maxX = (*itBB).second.max.x;
				}

				if (minY > (*itBB).second.min.y) 
				{
					 minY = (*itBB).second.min.y;
				}

				if (maxY < (*itBB).second.max.y) 
				{
					maxY= (*itBB).second.max.y;
				}

				z = (*itBB).second.min.z;
			}*/

			std::vector<SSegment *> temp;

			if (dinamicObjectsIDWithIntersect[i] < 0)
			{
				MP_MAP<int, MP_VECTOR<int> >::iterator it1 = m_provisionalDinamicObject.find(-dinamicObjectsIDWithIntersect[i]);
				if (it1 != m_provisionalDinamicObject.end())
				{
					for (unsigned int k = 0; k < (*it1).second.size(); k++)
					{
						MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itIAO = m_inactiveDinamicObjects.find((*it1).second[k]);
						if (itIAO != m_inactiveDinamicObjects.end())
						{
							std::vector<SSegment*>::iterator it = (*itIAO).second.begin();
							std::vector<SSegment*>::iterator itEnd = (*itIAO).second.end();
							for (; it!=itEnd; it++)
							{
								temp.push_back((*it));
							}
						}
					}
				}

				it = temp.begin();
				itEnd = temp.end();
			}

			bool _isChanged = ProcessIntersectSegments(it, itEnd, newPath, points, dinamicObjectsIDWithIntersect);
			isChanged = _isChanged || isChanged;

			MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itSegm = m_segments.find(dinamicObjectsIDWithIntersect[i]);

			if (itSegm != m_segments.end())
			{
				Delete3DObjectsPath(itSegm);		
			}

			itSegm = m_segments.find(-dinamicObjectsIDWithIntersect[i]);

			if (itSegm != m_segments.end())
			{
				Delete3DObjectsPath(itSegm);	
			}	
		}

		int iterationCount = 0;
		while ((isChanged) && (iterationCount < 10))
		{
			std::vector<SSegment *> oldPath = newPath;
			newPath.clear();
			std::vector<SSegment *>::iterator it = oldPath.begin();
			std::vector<SSegment *>::iterator itEnd = oldPath.end();
			isChanged = ProcessIntersectSegments(it, itEnd, newPath, points, dinamicObjectsIDWithIntersect);
			iterationCount++;
		}

		/*std::vector<int> _dinamicObjectsIDWithIntersect = dinamicObjectsIDWithIntersect;
		dinamicObjectsIDWithIntersect.clear();

		for (int k = 0; k < _dinamicObjectsIDWithIntersect.size(); k++)
		{
			int id = _dinamicObjectsIDWithIntersect[k];
			if (id > 0)
			{
				dinamicObjectsIDWithIntersect.push_back(id);
			}
			else
			{
				MP_MAP<int, MP_VECTOR<int> >::iterator it = m_provisionalDinamicObject.find(-id);
				for (int k = 0; k < (*it).second.size(); k++)
				{
					dinamicObjectsIDWithIntersect.push_back((*it).second[k]);
				}
			}
		}*/

		{
			std::vector<SSegment *>::iterator it = newPath.begin();
			std::vector<SSegment *>::iterator itEnd = newPath.end();

			for ( ; it != itEnd; it++)
			{
				(*it)->groupId = -(*objectSegments)[0]->groupId;
			}
		}

		if (newPath.size() != 0)
		{
			/*PathesBB vBB;
			vBB.min = CVector3D(minX, minY, z);
			vBB.max = CVector3D(maxX, maxY, z); 

			m_pathesBB.insert(std::map<int, PathesBB>::value_type(-(*objectSegments)[0]->groupId, vBB));*/
			//CheckInactiveObjectsInMainSegments();

			//m_segments.insert(MP_MAP<int, MP_VECTOR<SSegment*> >::value_type(-(*objectSegments)[0]->groupId, newPath));

			InsertInMainSegments(-(*objectSegments)[0]->groupId,  newPath);

			//CheckInactiveObjectsInMainSegments();

			isGraphMerged = true;

			UpdateBoundingBox(-(*objectSegments)[0]->groupId);

			C3DObject* object = g->o3dm.GetObjectPointer((*objectSegments)[0]->groupId);
			CVector3D v = object->GetCoords();
			CQuaternion q = object->GetRotation()->GetAsDXQuaternion();

            MP_NEW_P7(oi, SPathNeededObjectsInfo, v.x, v.y, v.z, q, -(*objectSegments)[0]->groupId, object, DINAMIC);
			m_mergeDinamicObjects.push_back(oi);

			for (unsigned int i = 0; i < dinamicObjectsIDWithIntersect.size(); i ++)
			{
				itD = m_dinamicObjects.begin();
				itEndD = m_dinamicObjects.end();

				for (; itD!= itEndD; itD++)
				{
					if ((*itD)->id == dinamicObjectsIDWithIntersect[i])
					{
						(*itD)->update = NO_UPDATE;
					}
				}

				int id = dinamicObjectsIDWithIntersect[i];
				MP_MAP<int, MP_VECTOR<int> >::iterator it = m_provisionalDinamicObject.find(id);
				if (it != m_provisionalDinamicObject.end())
				{
					m_provisionalDinamicObject.erase(it);
				}

				MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itSegm = m_segments.find(id);
				Delete3DObjectsPath(itSegm);


				if (id != (*objectSegments)[0]->groupId)
				{
					itSegm = m_segments.find(-id);
					Delete3DObjectsPath(itSegm);					
				}
				

				/*if (id != (*objectSegments)[0]->groupId)
				{
					MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itSegm = m_segments.find(-id);
					if (itSegm != m_segments.end())
					{
						Delete3DObjectsPath(itSegm);
					}
				}*/
			}


			m_provisionalDinamicObject.insert(MP_MAP<int, MP_VECTOR<int> >::value_type((*objectSegments)[0]->groupId, MAKE_MP_VECTOR(dinamicObjectsIDWithIntersect, int)));
			CheckInactiveObjectsInMainSegments();


		}
	}

	m_isShowNearestAfterChangePath = true;

	return isGraphMerged;
}

void CPathFinder::UpdatePathesBB(int i, int id, const SSegment* segment)
{
	std::map<int, PathesBB>::iterator it = m_pathesBB.find(id); 

	if (it != m_pathesBB.end())	
	{	
		float minX, maxX, minY, maxY;

		if (segment->begin.x < segment->end.x)
		{
			minX = segment->begin.x;
			maxX = segment->end.x;
		}
		else
		{
			minX = segment->end.x;
			maxX = segment->begin.x;
		}

		if (segment->begin.y < segment->end.y)
		{
			minY = segment->begin.y;
			maxY =segment->end.y;
		}
		else
		{
			minY = segment->end.y;
			maxY = segment->begin.y;
		}

		if (0 == i) 
		{
			(*it).second.min = CVector3D(minX, minY, segment->begin.z);
			(*it).second.max = CVector3D(maxX, maxY, segment->end.z); 
		}
		else
		{
			ComparePointWithPathesBB(&segment->begin, &(*it).second);
			ComparePointWithPathesBB(&segment->end, &(*it).second);
		}	
	}
}

void CPathFinder::UpdateSegmentsQuadrPoints(int numberOfChangedSegment, CVector3D* minBB, CVector3D* maxBB, CQuaternion* rotate, CVector3D* coord, CVector3D* vBegin, CVector3D* vEnd)
{
	CVector3D v1,v2;

	switch(numberOfChangedSegment)
	{
	case 0:	
		v1 = CVector3D(*minBB);
		v2 = CVector3D(minBB->x, maxBB->y, minBB->z);
		break;

	case 1:
		v1 = CVector3D(minBB->x, maxBB->y, minBB->z);
		v2 = CVector3D(maxBB->x, maxBB->y, minBB->z);
		break;

	case 2:
		v1 = CVector3D(maxBB->x, maxBB->y, minBB->z);
		v2 = CVector3D(maxBB->x, minBB->y, minBB->z);
		break;

	case 3:
		v1 = CVector3D(maxBB->x, minBB->y, minBB->z);
		v2 = CVector3D(*minBB);
		break;
	}

	
	rotate->Normalize();

	v1 *= *rotate;
	v2 *= *rotate;

	v1 += *coord;
	v2 += *coord;

	*vBegin = CVector3D(v1.x, v1.y, minBB->z);
	*vEnd =  CVector3D(v2.x, v2.y, minBB->z);
}

void CPathFinder::UpdateSegmentsCirclePoints(int i, CVector3D* minBB, CVector3D* maxBB, CQuaternion* rotate, CVector3D* coord, CVector3D* vBegin, CVector3D* vEnd)
{
	CVector3D circleVec1, circleVec2;

	minBB->x -= MIN_INDENT/2;
	minBB->y -= MIN_INDENT/2;

	maxBB->x += MIN_INDENT/2;
	maxBB->y += MIN_INDENT/2;

	float rX = abs(minBB->x - maxBB->x)/2;
	float rY = abs(minBB->y - maxBB->y)/2;

	float angle = (float)i / PART_COUNT  * (float)M_PI * 2.0f;
	float x = rX * m_cosTable[GetAngleInDegrees(angle)];;
	float y = rY * m_sinTable[GetAngleInDegrees(angle)];;

	circleVec1 = CVector3D(x , y, minBB->z);

	if (i ==  PART_COUNT - 1)
	{
		angle = 0;
		x = rX;
		y = 0;
	}
	else
	{
		angle = (float)(i+1)/ PART_COUNT  * (float)M_PI * 2.0f;
		x = rX * m_cosTable[GetAngleInDegrees(angle)];//cosf(angle);
		y = rY * m_sinTable[GetAngleInDegrees(angle)]; //sinf(angle);
	}

	circleVec2 = CVector3D(x, y, minBB->z);

	circleVec1 *= *rotate;
	circleVec2 *= *rotate;

	circleVec1 += *coord;
	circleVec2 += *coord;

	*vBegin = circleVec1;
	*vEnd = circleVec2;
}



bool CPathFinder::ParseSegment(SPathLinesBlock pathLinesBlock, int i , CVector3D coord, CQuaternion rotate, CVector3D* vec, int offset)
{
	if (sizeof(float) * (i + 2) + offset + 4 > pathLinesBlock.size)
	{
		return false;
	}

	CVector3D v = CVector3D(*(float*)(pathLinesBlock.data + sizeof(float)*i + offset),*(float*)(pathLinesBlock.data + sizeof(float)*(i+1)+ offset),*(float*)(pathLinesBlock.data + sizeof(float)*(i+2)+ offset));					

	v *= rotate;
	v += coord;

	*vec = CVector3D(v.x, v.y , v.z);

	return true;
}

void CPathFinder::CreateQuadrPath(int objectID, int modelID)
{
	CVector3D minBB, maxBB, v;

	CQuaternion q;

	if (modelID != -1)
	{
		BoundingBoxPointsConversion(objectID, modelID, q, v, minBB, maxBB);
	}
	else
	{
		minBB = CVector3D(-AVATAR_HALF_WIDTH, - AVATAR_HALF_LENGTH, -AVATAR_HALF_HEIGHT);
		maxBB = CVector3D(AVATAR_HALF_WIDTH, AVATAR_HALF_LENGTH, AVATAR_HALF_HEIGHT);
	}

	CVector3D q1,q2,q3,q4;

	q1 = CVector3D(minBB);
	q2 = CVector3D(minBB.x, maxBB.y, minBB.z);
	q3 = CVector3D(maxBB.x, maxBB.y, minBB.z);
	q4 = CVector3D(maxBB.x, minBB.y, minBB.z);

	q1 *=q;
	q2 *=q;
	q3 *=q;
	q4 *=q;

	AddSegment(CVector3D(q1.x + v.x, q1.y + v.y, q1.z + v.z), CVector3D(q2.x + v.x, q2.y + v.y, q2.z + v.z), objectID, COMMON_ROUND_SEGMENT_TYPE);
	AddSegment(CVector3D(q2.x + v.x, q2.y + v.y, q2.z + v.z), CVector3D(q3.x + v.x, q3.y + v.y, q3.z + v.z), objectID, COMMON_ROUND_SEGMENT_TYPE);
	AddSegment(CVector3D(q3.x + v.x, q3.y + v.y, q3.z + v.z), CVector3D(q4.x + v.x, q4.y + v.y, q4.z + v.z), objectID, COMMON_ROUND_SEGMENT_TYPE);
	AddSegment(CVector3D(q4.x + v.x, q4.y + v.y, q4.z + v.z), CVector3D(q1.x + v.x, q1.y + v.y, q1.z + v.z), objectID, COMMON_ROUND_SEGMENT_TYPE);		
}

void CPathFinder::CreateCirclePath(int objectID, int modelID)
{
	CVector3D minBB, maxBB, v, circleVec1, circleVec2;

	CQuaternion q;

	BoundingBoxPointsConversion(objectID, modelID, q, v, minBB, maxBB);

	minBB.x -= MIN_INDENT/2;
	minBB.y -= MIN_INDENT/2;

	maxBB.x += MIN_INDENT/2;
	maxBB.y += MIN_INDENT/2;

	float rX = abs(minBB.x - maxBB.x)/2;
	float rY = abs(minBB.y - maxBB.y)/2;

	for (int i = 0; i < PART_COUNT; i++)
	{
		float angle = (float)i / PART_COUNT  * (float)M_PI * 2.0f;
		float x = rX * m_cosTable[GetAngleInDegrees(angle)];;
		float y = rY * m_sinTable[GetAngleInDegrees(angle)];;

		circleVec1 = CVector3D(x , y, minBB.z);
 
		if (i ==  PART_COUNT - 1)
		{
			angle = 0;
			x = rX;
			y = 0;
		}
		else
		{
			angle = (float)(i+1)/ PART_COUNT  * (float)M_PI * 2.0f;
			x = rX * m_cosTable[GetAngleInDegrees(angle)];;
			y = rY * m_sinTable[GetAngleInDegrees(angle)];;
		}
	
		circleVec2 = CVector3D(x, y, minBB.z);

		circleVec1 *=q;
		circleVec2 *=q;

		circleVec1 += v;
		circleVec2 += v;


		AddSegment(CVector3D(circleVec1.x, circleVec1.y, circleVec1.z), CVector3D(circleVec2.x, circleVec2.y, circleVec2.z), objectID, COMMON_ROUND_SEGMENT_TYPE);		
	}
}

void CPathFinder::BoundingBoxPointsConversion(int objectID, int modelID, CQuaternion& q, CVector3D& v, CVector3D& minBB, CVector3D& maxBB)
{
	C3DObject* object = GetObjectPointer(objectID);

	v = object->GetCoords();

	q = object->GetRotation()->GetAsDXQuaternion();

	q.Normalize();

	CVector3D scale;

	CModel* model = g->mm.GetObjectPointer(modelID);	

	maxBB.x = model->GetModelMaxX();
	maxBB.y = model->GetModelMaxY();
	maxBB.z = model->GetModelMaxZ();
	minBB.x = model->GetModelMinX();
	minBB.y = model->GetModelMinY();
	minBB.z = model->GetModelMinZ();

	scale = object->GetScale3D();

	minBB.x *= scale.x;
	minBB.y *= scale.y;
	minBB.z *= scale.z;
	maxBB.x *= scale.x;
	maxBB.y *= scale.y;
	maxBB.z *= scale.z;

	minBB.x -= MIN_INDENT;
	minBB.y -= MIN_INDENT;

	maxBB.x += MIN_INDENT;
	maxBB.y += MIN_INDENT;
}

void CPathFinder::CopyPath(int srcObjectID, int destObjectID)
{
	C3DObject* object = GetObjectPointer(srcObjectID);
	CVector3D objCoords = object->GetCoords();

	CQuaternion q = object->GetRotation()->GetAsDXQuaternion();
	q.Normalize();
	q.w *= -1;

	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itV = m_segments.find(srcObjectID);

	if (itV != m_segments.end())
	{
		CVector3D currentPoint = m_currentPoint;

		Delete3DObjectsPath(destObjectID);

		C3DObject* destObject = GetObjectPointer(destObjectID);
		CVector3D destCoords = destObject->GetCoords();

		CQuaternion destRotation = destObject->GetRotation()->GetAsDXQuaternion();
		destRotation.Normalize();

		std::vector<SSegment*>::iterator it = (*itV).second.begin();
		std::vector<SSegment*>::iterator itEnd = (*itV).second.end();

		for ( ; it != itEnd; it++)
		{
			CVector3D begin = (*it)->begin;
			begin -= objCoords;
			begin *= q;
			begin *= destRotation;
			begin += destCoords;

			CVector3D end = (*it)->end;
			end -= objCoords;
			end *= q;
			end *= destRotation;
			end += destCoords;

			AddSegment(begin, end, destObjectID, (*it)->typeOfSegment);
		}

		m_currentPoint = currentPoint;

		m_isShowNearestAfterChangePath = true;
	}
}

void CPathFinder::Delete3DObjectsPath(int objectID)
{
	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itV = m_segments.find(objectID);

	if ( itV!= m_segments.end())
	{
		Delete3DObjectsPath(itV);		
	}
}

bool CPathFinder::Delete3DObjectsPath(MP_MAP<int, MP_VECTOR<SSegment*> >::iterator it)
{
	if (it != m_segments.end())
	{		
		/*std::vector<SSegment*>& segmentsArr = (*it).second;

		std::vector<SSegment*>::iterator it1 = segmentsArr.begin();
		std::vector<SSegment*>::iterator it1End = segmentsArr.end();
		for ( ; it1 != it1End; it1++)
		{
			MP_DELETE(*it1);
		}*/

		m_segments.erase(it);
		m_isShowNearestAfterChangePath = true;
		return true;
	}
	else
	{
		return false;
	}

	CheckInactiveObjectsInMainSegments();
}

bool CPathFinder::IsObjectWithGeneratedPath(const C3DObject* object, std::vector<SSegment*>* v)
{
	SLODLevel* level = object->GetLODGroup()->GetLodLevel(0);
	if (!level)
	{
		return false;
	}

	int modelID = level->GetModelID();

	std::map<int, SPathLinesBlock>::iterator itPL = m_pathLinesByModelID.find(modelID);

	if (itPL != m_pathLinesByModelID.end())
	{
		*v = FindCurrent3DObjectsPath();
		if (v->size() != 0)
		{
			return true;
		}
	}
	
	return false;
}

int CPathFinder::GetAngleInDegrees(float angle)
{
	return (((int)((angle*180/M_PI)+0.5))%360+360)%360;
}

void CPathFinder::OnObjectDeleted(int objectID)
{
	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator it =  m_segments.find(objectID);
	if (it != m_segments.end())
	{
		 Delete3DObjectsPath(it);		 
	}
	else
	{
		ReturnObjectsFromInactiveList(objectID);
	}

	std::map<int, PathesBB>::iterator itBB = m_pathesBB.find(objectID);
	if (itBB != m_pathesBB.end())
	{
		m_pathesBB.erase(itBB);
	}

	m_isShowNearestAfterChangePath = true;	

	std::vector<SPathNeededObjectsInfo*>::iterator itD= m_dinamicObjects.begin();
	std::vector<SPathNeededObjectsInfo*>::iterator itDEnd = m_dinamicObjects.end();

	for ( ; itD != itDEnd; itD++)
	if ((*itD)->id == objectID)
	{
		m_dinamicObjects.erase(itD);
		return;
	}
}

void CPathFinder::FindPointsSegments(SPointsInPath* point)
{
	int id = 0;
	bool isFound = false;

	if (point->segment1)
	{
		id = point->segment1->groupId;
	}
	else if (point->segment2)
	{
		id = point->segment2->groupId;
	}

	FIND_OBJECTS_SEGMENTS_OR_RETURN(id);

	for ( ; it!=itEnd; it++)
	{
		if ( ((*it)->begin == point->pnt) || ((*it)->end == point->pnt))
		{
			if (isFound)
			{
				point->segment1 = (*it);
				return;
			}
			else
			{
				point->segment2 = (*it);
				isFound = true;
			}

		}
	}
}

CPathFinder::~CPathFinder()
{
	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator it = m_segments.begin();
	MP_MAP<int, MP_VECTOR<SSegment*> >::iterator itEnd = m_segments.end();


	for ( ; it != itEnd; it++)
	{
		std::vector<SSegment*>::iterator itV = (*it).second.begin();
		std::vector<SSegment*>::iterator itVEnd = (*it).second.end();

		for ( ; itV != itVEnd; itV++)
		{ 
			MP_DELETE(*itV);
		}
	}
	

	{
		std::vector<SPathNeededObjectsInfo*>::iterator it = m_dinamicObjects.begin();
		std::vector<SPathNeededObjectsInfo*>::iterator itEnd = m_dinamicObjects.end();

		for ( ; it != itEnd; it++)
		{
			MP_DELETE(*it);
		}
	}

	

	if (g->scm.GetActiveScene())
	{
		g->scm.GetActiveScene()->RemoveSceneObject(this);
	}	

	ClearIntersectionPoints();

	{
		std::map<int, SPathLinesBlock>::iterator it = m_pathLinesByModelID.begin();
		std::map<int, SPathLinesBlock>::iterator itEnd = m_pathLinesByModelID.end();

		for ( ; it != itEnd; it++)
		{
			SPathLinesBlock pathLinesBlock = (*it).second;	
			MP_DELETE_ARR(pathLinesBlock.data);
		}
	}

	m_currentSegments.clear();
}
#pragma once

#include "CommonMathHeader.h"

#include "IPropertyBlockLoadHandler.h"
#include "Vector3D.h"
#include "PathLine.h"
#include "SceneObject.h"
#include <malloc.h>
#include <map>
#include "Viewport.h"

#define COMMON_ROUND_SEGMENT_TYPE	0
#define RIGHT_ROUND_SEGMENT_TYPE	1
#define LEFT_ROUND_SEGMENT_TYPE		2
#define ANY_ROUND_SEGMENT_TYPE		3

#define MERGE_PATHES				4
#define	FULL_UPDATE					5
#define NO_UPDATE					6

typedef struct _SPointsInQueue
{
	CVector3D point;

	float length;

	_SPointsInQueue(float x, float y, float z , float _length)
	{
		point.x = x;
		point.y = y;
		point.z = z;

		length = _length;
	}

} SPointsInQueue;

typedef struct _SPathLinesBlock
{
	unsigned char* data;
	unsigned int size;

	_SPathLinesBlock()
	{
		data = NULL;
		size = 0;
	}

	_SPathLinesBlock(unsigned char* _data, unsigned int _size)
	{
		data = _data;
		size = _size;
	}

} SPathLinesBlock;

typedef struct _SIntersectionPointsInfo
{
	CVector3D pnt;
	
	SSegment* segment;

	_SIntersectionPointsInfo()
	{
		pnt = CVector3D(0,0,0);
		segment = NULL;
	}

	_SIntersectionPointsInfo(CVector3D v, SSegment* _segment)
	{
		pnt = v;

		segment = _segment;
	}

} SIntersectionPointsInfo;

typedef struct _SPathNeededObjectsInfo
{
	CVector3D coord;

	CQuaternion rotate;

	int id;

	C3DObject* object;

	bool typeOfObject;

	unsigned int update;

	_SPathNeededObjectsInfo (float _x, float _y, float _z, CQuaternion _rotate, int _id, C3DObject* _object, bool _typeOfObject)
	{
		coord.x = _x;
		coord.y = _y;
		coord.z = _z;

		rotate = _rotate;

		id = _id;
		object = _object;
		typeOfObject = _typeOfObject;

		update = MERGE_PATHES;
	}

} SPathNeededObjectsInfo;

typedef struct _PathesBB
{
	CVector3D max;
	CVector3D min;
	
	_PathesBB()
	{
		min = CVector3D(0,0,0);
		max = CVector3D(0,0,0);
	}

	_PathesBB(const _PathesBB& object)
	{
		min = object.min;
		max = object.max; 
	}

} PathesBB;

#define	DEGREE		360

class CPathFinder : public ISceneObject, public IPropertyBlockLoadHandler
{
public:
	CPathFinder();
	~CPathFinder();

	void FindPath(CPathLine* const path ,  CVector3D begin, CVector3D end);
	void AddSegment(const CVector3D firstPoint, const CVector3D secondPoint, int id, unsigned char typeOfSegment);
	bool IsPointInCurrent3DObjectsPath(int mouseX, int mouseY, CViewport* const viewport ,int objectID, bool* isExistCurrentObject = NULL);
	std::vector<SSegment*> FindCurrent3DObjectsPath();

	void LogDynamicObject(int id);

	CVector3D GetCurrentPoint();
	void SetCurrentPoint(CVector3D* v);

	bool DeleteOrSelect3DObjectsSegment(int mouseX, int mouseY, CViewport* const viewport, CVector3D* vBegin = NULL, CVector3D* vEnd =NULL);

	void Delete3DObjectsPath(int objectID);
	void CreateQuadrPath(int objectID, int modelID);
	void CreateCirclePath(int objectID, int modelID);
	void CopyPath(int srcObjectID, int destObjectID);

	void Draw();

	void MovePoint(CVector3D* v);

    void OnPropertyBlockLoaded(const void* data, const unsigned int size, const std::string& fileName, const unsigned int modelID);
	void OnObjectCreated(int objectID, int modelID, bool isUpdate = false);
	void OnObjectDeleted(int objectID);

	void Init();

	void SetVisible(bool isShow);
	bool SetCurrentObjectID(int id);
	int  GetCurrentObjectID();

	void EnableDebugRendering(bool isEnabled);

	bool IsObjectWithGeneratedPath(const C3DObject* object, std::vector<SSegment*>* v);

	void SetGoToSegmentMode(bool isMoveToSegment);
	void SetTypeOfSelectSegment(unsigned char type);

	void RefreshSelectedSegmentPoints();
	void FindPointsSegments(SPointsInPath* point);

private:
	void CheckInactiveObjectsInMainSegments();
	void InsertInMainSegments(int id, std::vector<SSegment*> vec);

	MP_MAP<int, MP_VECTOR<SSegment*> > m_segments;
	MP_MAP<int, PathesBB> m_pathesBB;

	void CreatePath(SSegment* endSegm);

	void FindNearestPathes();

	void AddPointToPath(const CVector3D& point, SSegment* segment1,  SSegment* segment2);
	void ClearPath();
	void ClearIntersectionPoints();

	void FindIntersectionPointWithSegment(SSegment* segment, const CVector3D& begin, const CVector3D& end, std::vector<SIntersectionPointsInfo*>* points = NULL);
	bool IsPointNearStaticPath(const CVector3D& p0, const CVector3D& p1, const CVector3D& p2, float& minDist, SSegment* segment, int endIndex = -1 );

	void AddElementToPath(CVector3D v, SQElement** newElement, SQElement** optimalPath);

	//увеличение пути с концов
	void EditLength(CVector3D& begin , CVector3D& end);

	//сортировка вектора точек пересечения
	void SortIntersectionPoints(const CVector3D begin, const CVector3D end);
	int GetStartAndEndSegments(SSegment*& startSegm, SSegment*& endSegm , int pathCount);


	void BoundingBoxPointsConversion(int objectID, int modelID, CQuaternion& q, CVector3D& v, CVector3D& minBB, CVector3D& maxBB);

	void UpdateAvatar();
	void UpdateDinamicObjects();

	bool ParseSegment(SPathLinesBlock pathLinesBlock, int i, CVector3D coord, CQuaternion rotate, CVector3D* vec, int offset);
	void UpdateSegmentsQuadrPoints(int numberOfChangedSegment, CVector3D* minBB, CVector3D* maxBB, CQuaternion* rotate, CVector3D* coord, CVector3D* vBegin, CVector3D* vEnd);
	void UpdateSegmentsCirclePoints(int i, CVector3D* minBB, CVector3D* maxBB, CQuaternion* rotate, CVector3D* coord, CVector3D* vBegin, CVector3D* vEnd);
	void UpdatePathesBB(int i, int id, const SSegment* segment);

	void GoTo(CVector3D* vBegin, CVector3D* vEnd);

	int GetAngleInDegrees(float angle);

	void ComparePointWithPathesBB(const CVector3D* pnt, PathesBB* pathesBB);

	bool IsPathIntersectionStaticObjectsPath(const PathesBB* pathesBB , const CVector3D* begin, const CVector3D* end);

	void CalculateQuadrPoint(int i, float* qx, float* qy, CVector2D* p1, CVector2D* p2, bool isConstX = false);
	
	bool IsIntersectionPointInAddedSegment(const CVector3D* begin, const CVector3D* end, const CVector3D* point);

	void DeleteIntersectionPointsBetweenDinamicsPoints(int i, int id);

	bool IsNeedToAddPointsInQueue(std::vector<SPointsInQueue>& pointsInQueue, CVector3D* point, float length);

	bool IsSegmentIntersectDinamicsPath(const SSegment* segment, std::vector<int>& dinamicsIntersectSegmentsId);

	bool IsSegmentsIntersect(const SSegment* segment, CVector3D begin, CVector3D end, float& ta, float& tb);

	float CalculateAngleWithOx(CVector3D* begin, CVector3D* end);

	void DrawSegments(SSegment* segment);

	bool MergeGraphIfNeed(std::vector<SSegment*>* segmentsInPath);

	bool ReturnObjectsFromInactiveList(int paramId, std::vector<SPathNeededObjectsInfo*>::iterator* itA = NULL);

	bool IsPointInPoly(std::vector<SSegment*>* v, float x, float y );

	bool ProcessIntersectSegments(std::vector<SSegment*>::iterator it, std::vector<SSegment*>::iterator itEnd,
										   std::vector<SSegment*>& newPath, std::vector<SIntersectionPointsInfo*>& points,
										   std::vector<int>& dinamicObjectsIDWithIntersect);

	bool Delete3DObjectsPath(MP_MAP<int, MP_VECTOR<SSegment*> >::iterator it);

	void UpdateBoundingBox(int objectID);

	bool IsCanBeMerged(SPathNeededObjectsInfo* info);

	MP_VECTOR<CVector3D> m_pathPoints;

	MP_VECTOR<SIntersectionPointsInfo*> m_intersectionPoints;

	MP_MAP<int, SPathLinesBlock> m_pathLinesByModelID;

	CPathLine* m_path;
	CQueue m_queue;

	__forceinline float GetPointDist(const CVector3D p1, const CVector3D p2)
	{
		return sqrtf((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
	}

	__forceinline float GetDist(const CVector3D p1, const CVector3D p2)
	{
		return sqrtf((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y) + (p2.z - p1.z) * (p2.z - p1.z));
	}

	__forceinline bool IsPointInPoly(int npol, float * xp, float * yp, float x, float y )
	{
		int i, j;
		bool c = false;

		for ( i = 0, j = npol - 1; i < npol; j = i++)
		{
			if ((((yp [i] <= y) && (y < yp [j])) || ((yp [j] <= y) && (y < yp [i]))) && (x < (xp [j] - xp [i]) * (y - yp [i]) / (yp [j] - yp [i]) + xp [i]))
				c = !c;
		}

		return c;
	}

	CVector3D m_currentPoint, m_firstPoint;	

	MP_MAP<int, MP_VECTOR<SSegment*> > m_inactiveDinamicObjects;

	MP_MAP<int, MP_VECTOR<int> > m_provisionalDinamicObject;

	MP_VECTOR<SSegment*> m_currentSegments;

	MP_VECTOR<SSegment*> m_nearestPathes;

	MP_VECTOR<SPathNeededObjectsInfo*> m_dinamicObjects;
	MP_VECTOR<SPathNeededObjectsInfo*> m_mergeDinamicObjects;

	bool m_isShowNearest;

	CVector3D m_eyePosition;

	CVector3D m_begin, m_end;

	SSegment* m_currentSegment;

	int m_currentObjectID;

	bool m_isDebugRenderingEnabled;

	bool m_isMoveToSegment;

	float m_cosTable[DEGREE];
	float m_sinTable[DEGREE];

	bool m_isShowNearestAfterChangePath;
};

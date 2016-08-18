
#pragma once

#include "CommonRenderManagerHeader.h"

#include "3DObject.h"
#include "Vector3D.h"
#include "RayTraceTriMesh.h"
#include "IChangesListener.h"

typedef struct _SLocationRange
{
	C3DObject* obj;
	CRayTraceTriMesh* triMesh;

	_SLocationRange()
	{
		obj = NULL;
		triMesh = NULL;
	}

	~_SLocationRange();
} SLocationRange;

typedef MP_VECTOR<SLocationRange*> TLocationRangeList;

class CCameraInLocation : public CBaseChangesInformer, public IChangesListener
{
public:
	CCameraInLocation();
	~CCameraInLocation();

	void	AddLocationRange(C3DObject* obj);
	void	OnChanged(int eventID);

	void	EnterLocation(std::string name, float x1, float y1, float z1, float x2, float y2, float z2);
	void	LeaveLocation();

	bool	CheckForIntersection(CVector3D& start, CVector3D& direction);

	bool	IsCameraInAmfiteatr()const;
	bool	IsCameraInTheHouse()const;

private:
	bool	CCameraInLocation::CheckForIntersection(TLocationRangeList& locationRanges,
									CVector3D& start, CVector3D& direction);

	TLocationRangeList m_limitedLocationRanges;
	TLocationRangeList m_unlimitedLocationRanges;

	bool m_inLocation;
	bool m_inTheHouse;
	bool m_isLocationAmfiteatr;

	float m_locationX1,	m_locationY1, m_locationZ1;
	float m_locationX2, m_locationY2, m_locationZ2;

	MP_STRING m_locationName;
	MP_MAP<MP_STRING, int> m_amfiteatrLocationNames;
	MP_MAP<MP_STRING, int> m_houseLocationNames;
};
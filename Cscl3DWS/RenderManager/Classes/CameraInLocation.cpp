
#include "StdAfx.h"
#include "XMLList.h"
#include "LODGroup.h"
#include "CameraInLocation.h"
#include "PlatformDependent.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"
#include "UserData.h"

#define LIMITED_RANGE_MAX_SIZE		5000
#define UNLIMITED_RANGE_MAX_SIZE	500000

_SLocationRange::~_SLocationRange()
{
	if (triMesh)
	{
		MP_DELETE(triMesh);
	}
}

void UpdateCameraCollisionIfNeeded(C3DObject* obj)
{
	CQuaternion quat = obj->GetRotation()->GetAsDXQuaternion();

	if ((!obj->GetLODGroup()) || (!obj->GetLODGroup()->GetLodLevel(0)))
	{
		return;
	}

	int modelID = obj->GetLODGroup()->GetLodLevel(0)->GetModelID();
	CModel* _model = g->mm.GetObjectPointer(modelID);

	if (_model->GetPostfix().size() > 0)
	if ((!quat.IsEmptyRotation()) && (!obj->GetRotation()->IsEmptyRotation()))
	{
		obj->GetRotation()->SetEmptyRotation(true);

		obj->GetRotation()->SetRotationModel(_model);

		obj->GetRotation()->ApplyRotationToVertexs();

		obj->GetLODGroup()->UpdateBoundingBox(obj);
	}
}

CCameraInLocation::CCameraInLocation():
	m_inTheHouse(false),
	m_inLocation(false),
	m_isLocationAmfiteatr(false),
	MP_STRING_INIT(m_locationName),
	MP_MAP_INIT(m_amfiteatrLocationNames),
	MP_MAP_INIT(m_houseLocationNames),	
	MP_VECTOR_INIT(m_limitedLocationRanges),
	MP_VECTOR_INIT(m_unlimitedLocationRanges)
{
	gRM->SetCameraInLocation(this);
}

void	CCameraInLocation::AddLocationRange(C3DObject* obj)
{
	SLocationRange* locRange = MP_NEW(SLocationRange);
	locRange->obj = obj;

	if (obj->GetLODGroup()->IsGeometryLoaded())
	{
		locRange->triMesh = MP_NEW(CRayTraceTriMesh);
		obj->GetLODGroup()->InitTriMesh(locRange->triMesh);

		UpdateCameraCollisionIfNeeded(obj);
	}
	else
	{
		obj->AddChangesListener(this);
	}

	if ((int)obj->GetUserData(USER_DATA_LEVEL) == LEVEL_SEA)
	{
		m_unlimitedLocationRanges.push_back(locRange);
	}
	else
	{
		m_limitedLocationRanges.push_back(locRange);
	}

	OnAfterChanges();
}

void CheckForLoading(std::vector<SLocationRange*>::iterator it, std::vector<SLocationRange*>::iterator itEnd)
{
	for ( ; it != itEnd; it++)
	if ((*it)->obj->GetLODGroup())
	if (!(*it)->triMesh)
	{
		if ((*it)->obj->GetLODGroup()->IsGeometryLoaded())
		{
			(*it)->triMesh = MP_NEW(CRayTraceTriMesh);
			(*it)->obj->GetLODGroup()->InitTriMesh((*it)->triMesh);

			UpdateCameraCollisionIfNeeded((*it)->obj);
		}
	}
}

void	CCameraInLocation::OnChanged(int /*eventID*/)
{
	CheckForLoading(m_limitedLocationRanges.begin(), m_limitedLocationRanges.end());
	CheckForLoading(m_unlimitedLocationRanges.begin(), m_unlimitedLocationRanges.end());
}

void	CCameraInLocation::EnterLocation(std::string name, float x1, float y1, float z1, float x2, float y2, float z2)
{
	static bool isListInitialized = false;

	if (!isListInitialized)
	{
		CXMLList amfiteatrList(GetApplicationRootDirectory() + L"amfiteatrs.xml", true);
		std::string locationName;
		while (amfiteatrList.GetNextValue(locationName))
		{
			m_amfiteatrLocationNames.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(locationName), 1));
		}

		CXMLList houseLocationList(GetApplicationRootDirectory() + L"houses.xml", true);		
		while (houseLocationList.GetNextValue(locationName))
		{
			m_houseLocationNames.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(locationName), 1));
		}

		isListInitialized = true;
	}


	m_inLocation = true;
	m_locationName = name;
	m_isLocationAmfiteatr = (m_amfiteatrLocationNames.find(MAKE_MP_STRING(m_locationName)) != m_amfiteatrLocationNames.end());
	m_inTheHouse = (m_houseLocationNames.find(MAKE_MP_STRING(m_locationName)) != m_houseLocationNames.end());
	m_locationX1 = x1;
	m_locationY1 = y1;
	m_locationZ1 = z1;
	m_locationX2 = x2;
	m_locationY2 = y2;
	m_locationZ2 = z2;
}

void	CCameraInLocation::LeaveLocation()
{
	m_inLocation = false;
	m_inTheHouse = false;
	m_locationName = "";
	m_isLocationAmfiteatr = false;
}

bool	CCameraInLocation::IsCameraInAmfiteatr()const
{
	return m_isLocationAmfiteatr;
}

bool	CCameraInLocation::IsCameraInTheHouse()const
{
	return m_inTheHouse;
}

bool	CCameraInLocation::CheckForIntersection(TLocationRangeList& locationRanges,
									CVector3D& start, CVector3D& direction)
{
	TLocationRangeList::iterator it = locationRanges.begin();
	
	for ( ; it != locationRanges.end(); )
	{
		if ((*it)->obj->IsDeleted())
		{
			if ((*it)->triMesh)
			{
				MP_DELETE((*it)->triMesh);
			}
			it = locationRanges.erase(it);
		}
		else
		{
			it++;
		}
	}

	it = locationRanges.begin();
	TLocationRangeList::iterator itEnd = locationRanges.end();

	for ( ; it != itEnd; it++)
	if ((*it)->triMesh)
	{	
		float x, y, z;
		(*it)->obj->GetCoords(&x, &y, &z);

		if ((x >= m_locationX1) && (y >= m_locationY1) && (z >= m_locationZ1) &&
			(x <= m_locationX2) && (y <= m_locationY2) && (z <= m_locationZ2))
		{
#pragma warning(push)
#pragma warning(disable : 4239)
			if ((*it)->triMesh->CheckRayIntersect(start - (*it)->obj->GetCoords(), direction))
#pragma warning(pop)
			{	
				return true;
			}	
		}
	}

	return false;
}

bool	CCameraInLocation::CheckForIntersection(CVector3D& start, CVector3D& direction)
{
	m_locationX1 = start.x - LIMITED_RANGE_MAX_SIZE;
	m_locationY1 = start.y - LIMITED_RANGE_MAX_SIZE;
	m_locationZ1 = start.z - LIMITED_RANGE_MAX_SIZE;
	m_locationX2 = start.x + LIMITED_RANGE_MAX_SIZE;
	m_locationY2 = start.y + LIMITED_RANGE_MAX_SIZE;
	m_locationZ2 = start.z + LIMITED_RANGE_MAX_SIZE;

	if (CheckForIntersection(m_limitedLocationRanges, start, direction))
	{
		return true;
	}

	m_locationX1 = start.x - UNLIMITED_RANGE_MAX_SIZE;
	m_locationY1 = start.y - UNLIMITED_RANGE_MAX_SIZE;
	m_locationZ1 = start.z - UNLIMITED_RANGE_MAX_SIZE;
	m_locationX2 = start.x + UNLIMITED_RANGE_MAX_SIZE;
	m_locationY2 = start.y + UNLIMITED_RANGE_MAX_SIZE;
	m_locationZ2 = start.z + UNLIMITED_RANGE_MAX_SIZE;


	if (CheckForIntersection(m_unlimitedLocationRanges, start, direction))
	{
		return true;
	}

	return false;
}

CCameraInLocation::~CCameraInLocation()
{}
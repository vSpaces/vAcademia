
#include "stdafx.h"
#include "nrm3DObject.h"
#include "nrmanager.h"
#include "mapmanager.h"
#include "UserData.h"
#include "MapManagerServer.h"
#include "ZoneObjectsTypes.h"
#include "nrmPlugin.h"
#include <algorithm>
#include "oms_context.h"
#include "SyncMan.h"
#include "iasynchresourcemanager.h"
#include "nrmCharacter.h"
#include "VoipManClient.h"

// HANDLES
#include "RealityHandler.h"
#include "ZonesHandler.h"
#include "CreatePortalsHandler.h"
#include "ObjectStatusHandler.h"
#include "DeleteObjectHandler.h"
#include "CreateAvatarHandler.h"
#include "CreateObjectsHandler.h"
#include "ObjectsByNameHandler.h"
#include "ObjectsHandler.h"

#include "GlobalSingletonStorage.h"
#include "../../ObjectsCreator.h"
#include "HelperFunctions.h"

#include "CameraInLocation.h"
#include <windows.h>

#include "Rect2D.h"
#include "IniFile.h"
#include "PlatformDependent.h"
#include "WhiteBoardManager.h"

#include "__resource_log.h"

#include "RMContext.h"

#include <sstream>
#include <locale.h>

#include <float.h>
#include "UrlBuilder.h"
#include "InfoMan.h"

#define SYNCH_ZONE_BOUND_AREA_SIZE			100
#define MAX_DELETING_TIME	2

#define PLOCATION_IS_NULL(PLOC) (PLOC == NULL || (int)PLOC == 1)

using namespace MapManagerProtocol;

SObjectInfoForUpdate::SObjectInfoForUpdate():
	MP_STRING_INIT(location),
	MP_STRING_INIT(className),
	MP_WSTRING_INIT(userProps),
	MP_STRING_INIT(name),
	MP_STRING_INIT(res)
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
	bx = 0.0;
	by = 0.0;
	bz = 0.0;
	rx = 0.0;
	ry = 0.0;
	rz = 0.0;
	rw = 0.0;
	scalex = 0.0;
	scaley = 0.0;
	scalez = 0.0; 
	zoneID = 0;
	type = 0;
	objectID = 0;
	bornRealityID = 0;
	userData = 0;
	level = 0;
}

CMapManager::CMapManager(oms::omsContext* context, CNRManager* manager): 
				m_objectZone(context),
				nrmObject(NULL),
				m_mainObject(NULL),
				m_scene3d(NULL),
				m_lastUpdateTime(0),
				m_lastKinectHandleTime(0),
				m_currentRealityID(0),
				m_savedDistanceKoef(-1),
				m_sceneChangedCounter(0),
				m_prevRealityID(0xFFFFFFFF),
				m_currentZoneID(INCORRECT_ZONEID),
				m_lastCheckedForTrackObjectCount(0),
				m_acceptRegisterQueries(true),
				m_groundLoadedMessageWasSend(false),
				m_updateProgressZonesCounter(0),
				m_teleportingRealityID(0xFFFFFFFF),
				m_context(context),
				m_callbacks(NULL),
				m_createdAvatarID(0xFFFFFFFF),
				m_needTeleport(false),
				m_isRender3DNeeded(false),
				m_lastLogTimeOnUpdate(0),
				m_currentLocation(NULL),
				repeatQueries( false),
				m_isInZone(false),
				bIsNotFly(false),
				m_bModuleLoaded(false),
				MP_VECTOR_INIT(m_receivedMessages),
				MP_VECTOR_INIT(m_trackedScreens),
				MP_MAP_INIT(m_lastVisibleScreensInCurrentLocation),
				MP_MAP_INIT(m_realitiesCache),
				MP_VECTOR_INIT(m_filter),
				MP_VECTOR_INIT(m_locationsInfo),
				MP_VECTOR_INIT(m_objectsForRemove),
				MP_VECTOR_INIT(m_childrenForRemove),
				MP_VECTOR_INIT(m_asynchResources),
				MP_VECTOR_INIT(m_selectedObjects),
				MP_VECTOR_INIT(m_zones),
				MP_VECTOR_INIT(m_activeZones),
				MP_VECTOR_INIT(m_communicationAreasInfo),
				MP_VECTOR_INIT(m_vCurrentCommunicationArea)
{
	gRM->SetMapManager(this);

	if (m_context != NULL)
	{
		m_context->mpMapMan = this;
		MP_NEW_V(m_server, CMapManagerServer, m_context);
	}
	else
	{
		m_server = NULL;
	}

	SetRenderManager(manager);
	ReadFilterFile();

	MP_NEW_V(m_commonZone, CMapManagerZone, m_context);
	m_commonZone->SetLoadingQueueInfoPtr(&m_loadingQueueInfo);
	
	gRM->resLib->GetAsynchResMan()->SetSeriousChangesListener(this);
	gRM->resLib->GetAsynchResMan()->SetPriorityUpdater(this);
}

void CMapManager::StartKinect()
{
	if (m_mainObject)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_mainObject->GetAnimation();
		if (sao)
		{
			sao->EnableKinect();
		}	
	}
}

void CMapManager::StopKinect()
{
	if (m_mainObject)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_mainObject->GetAnimation();
		if (sao)
		{
			sao->DisableKinect();
		}	
	}	
}

void CMapManager::SetKinectMode(int iMode)
{
	if (m_mainObject)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_mainObject->GetAnimation();
		if (sao)
		{
			if (iMode == 0)
			{
				sao->SetKinectSeatedMode(false);
			}
			else if (iMode == 1)
			{
				sao->SetKinectSeatedMode(true);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}


void CMapManager::SetMarkerType(int iMarker)
{
	if (m_mainObject)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_mainObject->GetAnimation();
		if (sao)
		{
			if (iMarker == 3) // маркер не выбран
			{
				sao->SetMarkerType(3);
			}
			else if (iMarker == 2) // красный цвет
			{
				sao->SetMarkerType(2);
			}
			else if (iMarker == 1) // зеленый цвет
			{
				sao->SetMarkerType(1);
			}
			else if (iMarker == 0) // синий цвет
			{
				sao->SetMarkerType(0);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}

void CMapManager::EnableFaceTracking()
{
	if (m_mainObject)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_mainObject->GetAnimation();
		if (sao)
		{
			sao->EnableFaceTracking();
		}
	}
}

void CMapManager::DisableFaceTracking()
{
	if (m_mainObject)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_mainObject->GetAnimation();
		if (sao)
		{
			sao->DisableFaceTracking();
		}
	}
}

bool CMapManager::GetKinectPoints(float* points)
{
	if (!m_mainObject)
	{
		return false;
	}

	if (!m_mainObject->GetAnimation())
	{
		return false;
	}

	CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_mainObject->GetAnimation();
	if (!sao->GetKinectObject())
	{
		return false;
	}

	memcpy(points, sao->GetKinectObject()->GetSkeletonXYZ(), 20 * 3 * sizeof(float));
	return true;
}

void CMapManager::GetKinectState(bool& abHeadFound, bool& abLeftHandFound,  bool& abLeftLegFound,  bool& abRightHandFound,  bool& abRightLegFound,  bool& abOneSkeletFound, bool& abMarkersNotFound)
{
	abHeadFound = false;
	abLeftHandFound = false;
	abRightHandFound = false;
	abLeftLegFound = false;
	abRightLegFound = false;
	abOneSkeletFound = true;
	abMarkersNotFound = false;

	if (m_mainObject)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_mainObject->GetAnimation();
		if (sao)
		{
			sao->GetKinectTrackedParts(abHeadFound, abLeftHandFound, abRightHandFound, abLeftLegFound, 
				abRightLegFound, abOneSkeletFound, abMarkersNotFound);
			abOneSkeletFound = !abOneSkeletFound;
		}
	}
}

int CMapManager::InitOculusRift(bool isNeedToEnable)
{
	if (isNeedToEnable)
	{
		IOculusRiftSupport::InitError initError;
		IOculusRiftSupport* obj = m_oculusRift.GetOculusObject(initError);
		if (!obj)
		{
			return initError;
		}
		g->scm.GetActiveScene()->SetOculusRiftObject(obj);
		gRM->scene2d->SetVisible(false);
		if (m_context && m_context->mpSM)
		{
			m_context->mpSM->OnOculusRiftFound();
		}
		m_oculusRift.OnStart();
	}
	else
	{
		gRM->scene2d->SetVisible(true);
		g->scm.GetActiveScene()->SetOculusRiftObject(NULL);
		m_oculusRift.OnStop();
	}

	return 0;
}

void CMapManager::GetOculusRiftDesktopParams(int& wResolution, int& hResolution, int& displayX, int& displayY)
{
	IOculusRiftSupport* oculus = g->scm.GetActiveScene()->GetOculusRift();
	if (oculus) {
		oculus->GetDesktopParams(wResolution, hResolution, displayX, displayY);
	}
}

bool CMapManager::InitKinect(bool isNeedToEnable)
{	
	if (isNeedToEnable)
	{
		if (m_kinect.GetKinectObject(m_mainObject))
		{
			if (m_context && m_context->mpSM)
			{
				m_context->mpSM->OnKinectFound();
			}
		}
		else
		{
			return false;
		}
	}
	else if (m_mainObject)
	{
		CSkeletonAnimationObject* sao = (CSkeletonAnimationObject*)m_mainObject->GetAnimation();
		if (sao)
		{
			sao->SetKinectObject(NULL);
		}
	}

	return true;
}

bool CMapManager::InitGamepad(bool isNeedToEnable)
{
	if (isNeedToEnable)
	{
		if (m_context && m_context->mpSM)
		{
			return m_context->mpSM->OnControllerFound();
		}

		return false;
	}
	else
	{
		if (m_context && m_context->mpSM)
		{
			m_context->mpSM->OnControllerLost();
		}
	}

	return true;
}

void CMapManager::TrackVisibleScreensInCurrentLocation(int firstObject)
{
	SLocationInfo* mainAvatarLocation = GetObjectLocation(m_mainObject);

	if (!mainAvatarLocation)
	{
		return;
	}

	for (int objectID = firstObject; objectID < g->o3dm.GetCount(); objectID++)
	{
		C3DObject* obj = g->o3dm.GetObjectPointer(objectID);
		nrm3DObject* nrmObj = (nrm3DObject*)obj->GetUserData(USER_DATA_NRMOBJECT);

		if ((nrmObj) && (GetObjectLocation(obj)) && (mainAvatarLocation->name == GetObjectLocation(obj)->name))
		{
			USES_CONVERSION;
			if (!nrmObj->GetMLMedia())
			{
				continue;
			}			
			if (!nrmObj->GetMLMedia()->GetI3DObject())
			{
				continue;
			}
			wchar_t* _class = nrmObj->GetMLMedia()->GetI3DObject()->GetClassName();
			if (!_class)
			{
				continue;
			}
			std::string className = W2A(_class);
			if (className.find("panel_object") != -1)
			{
				m_trackedScreens.push_back(obj);		
				if (obj->GetLODGroup()->GetLodLevel(0))
				{
					int modelID = obj->GetLODGroup()->GetLodLevel(0)->GetModelID();
					CModel* model = g->mm.GetObjectPointer(modelID);
					for (int i = 0; i < model->GetTextureCount(); i++)
					if ((model->GetTextureOffset(i + 1) - model->GetTextureOffset(i) == 2) ||
						 (model->GetTextureOffset(i + 1) - model->GetTextureOffset(i) == 4))
					{
						int textureID = model->GetTextureNumber(i);
						int materialID = model->GetMaterial(i);
						CMaterial* material = g->matm.GetObjectPointer(materialID);
						int textureID2 = material->GetTextureID(0);
						if (textureID2 != -1)
						{
							textureID = textureID2;
						}
						CTexture* texture = g->tm.GetObjectPointer(textureID);
						texture->SetNeedToCheckVisibility(true);
						break;
					}
				}
			}
		}
	}
}

void CMapManager::StartTrackVisibleScreensInCurrentLocation()
{
	m_trackedScreens.clear();

	TrackVisibleScreensInCurrentLocation(0);

	m_lastCheckedForTrackObjectCount = g->o3dm.GetLiveObjectsCount();
}

std::map<std::wstring, int> CMapManager::GetVisibleScreensInCurrentLocation()
{
	std::map<std::wstring, int> res;
	if ((g->gi.GetVendorID() == VENDOR_INTEL) || (IsIconic(g->ne.ghWnd)))
	{
		return res;
	}

	if (m_lastCheckedForTrackObjectCount != g->o3dm.GetCount())
	{
		TrackVisibleScreensInCurrentLocation(m_lastCheckedForTrackObjectCount);
		m_lastCheckedForTrackObjectCount = g->o3dm.GetCount();
	}	

	std::vector<C3DObject *>::iterator it = m_trackedScreens.begin();
	std::vector<C3DObject *>::iterator itEnd = m_trackedScreens.end();

	for ( ; it != itEnd; it++)
	if (!(*it)->IsDeleted())
	{
		CTexture* texture = NULL;

		if (!(*it)->GetLODGroup()->GetLodLevel(0))
		{
			continue;
		}

		int modelID = (*it)->GetLODGroup()->GetLodLevel(0)->GetModelID();
		CModel* model = g->mm.GetObjectPointer(modelID);

		bool isTextureChanged = false;
		
		for (int i = 0; i < model->GetTextureCount(); i++)
		if ((model->GetTextureOffset(i + 1) - model->GetTextureOffset(i) == 2) ||
			 (model->GetTextureOffset(i + 1) - model->GetTextureOffset(i) == 4))
		{
			int textureID = model->GetTextureNumber(i);
			int materialID = model->GetMaterial(i);
			CMaterial* material = g->matm.GetObjectPointer(materialID);
			int textureID2 = material->GetTextureID(0);
			if (textureID2 != -1)
			{
				textureID = textureID2;
			}
			texture = g->tm.GetObjectPointer(textureID);
			if (!texture->IsNeededToCheckVisibility())
			{
				isTextureChanged = true;
			}
			texture->SetNeedToCheckVisibility(true);
			break;
		}

		if (((*it)->IsVisibleNow()) && (texture))
		{
			int pixelCount = 0;
			if (isTextureChanged)
			{
				MP_MAP<MP_WSTRING, int>::iterator itL = m_lastVisibleScreensInCurrentLocation.find( MAKE_MP_WSTRING( (*it)->GetName()));
				if (itL != m_lastVisibleScreensInCurrentLocation.end())
				{
					pixelCount = (*itL).second;
				}
			}
			else
			{
				pixelCount = texture->GetOcclusionQueryPixels();
			}
			res.insert(std::map<std::wstring, int>::value_type((*it)->GetName(), pixelCount));		
		}
		else
		{
			res.insert(std::map<std::wstring, int>::value_type((*it)->GetName(), 0));
		}
	}

	m_lastVisibleScreensInCurrentLocation.clear();
	std::map<std::wstring, int>::iterator rit = res.begin();
	std::map<std::wstring, int>::iterator ritEnd = res.end();

	for ( ; rit != ritEnd; rit++)
	{
		m_lastVisibleScreensInCurrentLocation.insert(MP_MAP<MP_WSTRING, int>::value_type(MAKE_MP_WSTRING((*rit).first), (*rit).second));
	}	

	return res;
}

void CMapManager::EndTrackVisibleScreensInCurrentLocation()
{
	m_trackedScreens.clear();

	std::vector<CTexture *>::iterator it = g->tm.GetLiveObjectsBegin();
	std::vector<CTexture *>::iterator itEnd = g->tm.GetLiveObjectsEnd();

	for ( ; it != itEnd; it++)
	{
		(*it)->SetNeedToCheckVisibility(false);
	}
}

cmRealityType CMapManager::GetCurrentRealityType()
{
	cmIRealityInfo* realityInfo = NULL;
	GetRealityInfo(GetCurrentRealityID(), realityInfo);
	if (realityInfo)
	{
		return realityInfo->GetType();
	}
	else
	{
		return cm::RT_ZERO;
	}
}

cmRealityType CMapManager::GetPrevRealityType()
{
	cmIRealityInfo* realityInfo = NULL;
	GetRealityInfo(m_prevRealityID, realityInfo);
	if (realityInfo)
	{
		return realityInfo->GetType();
	}
	else
	{
		return cm::RT_ZERO;
	}
}

bool	CMapManager::IsPointInActiveZones(const float x, const float y)
{
	CVector3D coords = GetMainObjectCoords();

	float centerX, centerY;
	float size = SYNCH_ZONE_SIZE * 1.5f;
	ZoneID::get_zone_center_from_abs_coords(coords.x, coords.y, centerX, centerY, SYNCH_ZONE_SIZE);
	
	return ((x < centerX - size) || (x > centerX + size) || (y < centerY - size) || (y > centerY + size)) ? false : true;
}

void	CMapManager::SetMainObjectCoords(float x, float y, float z)
{
	m_mainObjectCoords.x = x;
	m_mainObjectCoords.y = y;
	m_mainObjectCoords.z = z;

	ZoneID zn = ZoneID::get_zone_id_from_xy(x, y);
	g->lw.WriteLn("Main object coords: ", x, ", ", y);
	OnZoneEntered(zn);

	g->rs.SetInt(MAX_TEXTURE_LOADING_TIME, 5000);
	g->rs.SetInt(MAX_CLOTHES_LOADING_FRAME_TIME, 5000);
	g->rs.SetInt(MAX_COMPOSITE_TEXTURES_FRAME_TIME, 5000);
	g->rs.SetInt(MAX_SAO_LOADING_FRAME_TIME, 500);
}

CVector3D CMapManager::GetMainObjectCoords()
{
	if (m_mainObject)
	{
		return m_mainObject->GetCoords();
	}
	else
	{
		return m_mainObjectCoords;
	}
}

void	CMapManager::OnChanged(int eventID)
{
	if (eventID == 0)
	{
		m_sceneChangedCounter = 4;
	}
}

void	CMapManager::UpdateSyncObjects( std::vector<ZONE_OBJECT_INFO *>& syncZonesInfo)
{
	vecServerZoneObjects::iterator it = syncZonesInfo.begin();
	vecServerZoneObjects::iterator itEnd = syncZonesInfo.end();
	for (;  it != itEnd;  it++)
	{
		ZONE_OBJECT_INFO* info = *it;
		if (info == NULL)
			continue;

		// регистрирует описание синхронизируемого объекта в SyncMan
		m_context->mpSyncMan->PutMapObject( *info);
		MP_DELETE(info);
		*it = NULL;
	}
}

void	CMapManager::UpdateObjectsInfo(std::vector<ZONE_OBJECT_INFO *>& info)
{
	m_commonZone->UpdateObjectsInfo(info);
	StartPhysics();
}

void	CMapManager::OnGroundLoaded(ZONE_OBJECT_INFO* info)
{
	CVectorZones::iterator	it = m_zones.begin();
	CVectorZones::iterator	 end = m_zones.end();

	for ( ; it != end; it++)
	{
		if ((*it)->IsObjectIn(info->translation, info->boundsVec))
		{
			(*it)->SetGroundLoadedStatus(true);
		}
	}

	m_commonZone->SetGroundLoadedStatus(true);
}

void	CMapManager::Clear()
{
	while (!m_zones.empty())
	{
		MP_DELETE(*m_zones.begin());
		m_zones.erase(m_zones.begin());
	}

	MP_DELETE(m_commonZone);
}

void	CMapManager::ReloadWorld()
{
	m_groundLoadedMessageWasSend = false;
	WriteLog("m_groundLoadedMessageWasSend changed to false [1]");

	ReloadWorldImpl();
}

void CMapManager::ReloadWorldImpl()
{
	if (m_mainObject)
	{
		ZoneID	zone = GetObjectZone(m_mainObject);
		assert(zone.is_correct());
		OnZoneEntered(zone);
	}
}

int CMapManager::GetCurrentRealityID()
{
	return m_currentRealityID;
}

int CMapManager::GetPrevRealityID()
{
	return m_prevRealityID;
}

bool	CMapManager::Remove(rmml::mlMedia* object)
{
	unsigned int bornRealityID = 0;
	unsigned int id = object->GetSynchID(bornRealityID, false);

	// отсылаем запрос на удаление объекта с сервера карты
	//m_server->delete_object(id, bornRealityID);

	// отсылаем запрос на удаление объекта с сервера синхронизации
	m_context->mpSyncMan->DeleteObject(id, bornRealityID);

	// удаляем объект на этом клиенте (на остальных должен удалиться через сервер синхронизации)
	// 27.01.2011: на этом клиенте объект на данный момент просто скрыт, а удалится он тоже через сервер синхронизации.
	// Если это удаление здесь раскомментарить, то удаляемые объекты могут появляться вновь из-за
	// того, что с сервера синхронизации могут приходить для него какие-то остаточные пакеты, 
	// приняв которые клиент шлет запрос серверу на воссоздание объекта
	//m_context->mpSM->DeleteObject(object);

	return true;
}

// отправить Mapserver-у запрос на создание объекта
bool CMapManager::CreateObject(const rmml::mlObjectInfo4OnServerCreation* apObjectDesc, unsigned int auRmmlQueryID)
{
	SObjectInfoForUpdate objectInfo;
	int zoneU, zoneV;
	objectInfo.bx = 1.0f;
	objectInfo.by = 1.0f;
	objectInfo.bz = 1.0f;
	if(apObjectDesc->mBounds.x > 0)
		objectInfo.bx = (float)apObjectDesc->mBounds.x;
	if(apObjectDesc->mBounds.y > 0)
		objectInfo.by = (float)apObjectDesc->mBounds.y;
	if(apObjectDesc->mBounds.z > 0)
		objectInfo.bz = (float)apObjectDesc->mBounds.z;

	ZoneID _zoneID;
	_zoneID = _zoneID.get_zone_id_from_xy((float)apObjectDesc->mPos.x, (float)apObjectDesc->mPos.y);
	_zoneID.get_zone_uv_from_xy((float)apObjectDesc->mPos.x, (float)apObjectDesc->mPos.y, zoneU, zoneV);
	float localX, localY;
	_zoneID.get_zone_center_from_xy(zoneU, zoneV, localX, localY);	
	objectInfo.zoneID = _zoneID.id;

	int _localX = (int)((float)apObjectDesc->mPos.x - localY);
	int _localY = (int)((float)apObjectDesc->mPos.y - localX);
	objectInfo.x = (float)_localX;
	objectInfo.y = (float)_localY;
	objectInfo.z = (float)apObjectDesc->mPos.z;

	USES_CONVERSION;
	objectInfo.name = apObjectDesc->mszName;
	objectInfo.res = W2A(apObjectDesc->mwcSrc);
	objectInfo.className = apObjectDesc->mszClassName;
	if(apObjectDesc->mwcParams != NULL)
		objectInfo.userProps = apObjectDesc->mwcParams;

	objectInfo.type = vespace::VE_UNDEFINED;
	switch(apObjectDesc->musType){
		case MLMT_OBJECT:
			objectInfo.type = vespace::VE_OBJECT3D;
			break;
		case MLMT_GROUP:
			objectInfo.type = vespace::VE_GROUP3D;
			break;
		case MLMT_COMMINICATION_AREA:
			objectInfo.type = vespace::VE_COMMINICATIONAREABOUNDS;
			break;
		case MLMT_CHARACTER:
			objectInfo.type = vespace::VE_AVATAR;
			break;
		case MLMT_PARTICLES:
			objectInfo.type = vespace::VE_PARTICLES;
			break;
	}

	objectInfo.userData = (int)(apObjectDesc->mCallback.mpParams);

	objectInfo.rx = (float)apObjectDesc->mRot.x;
	objectInfo.ry = (float)apObjectDesc->mRot.y;
	objectInfo.rz = (float)apObjectDesc->mRot.z;
	objectInfo.rw = (float)apObjectDesc->mRot.a;

	objectInfo.scalex = (float)apObjectDesc->mScl.x;
	objectInfo.scaley = (float)apObjectDesc->mScl.y;
	objectInfo.scalez = (float)apObjectDesc->mScl.z;

	objectInfo.objectID = 0;
	objectInfo.bornRealityID = m_currentRealityID;

	objectInfo.level = apObjectDesc->miLevel;

	objectInfo.location = apObjectDesc->mszLocation;

	// По идее тут надо вызвать: m_server->createObject(objectInfo);
	m_server->create_object( objectInfo, auRmmlQueryID);

	return true;
}


std::vector<rmml::ml3DBounds> CMapManager::GetVisibleLocations()
{
	std::vector<rmml::ml3DBounds> rectangles;	

	std::vector<SLocationInfo *>::iterator it = m_locationsInfo.begin();
	std::vector<SLocationInfo *>::iterator itEnd = m_locationsInfo.end();
	
	for ( ; it != itEnd; it++)
	{
		SLocationInfo* info = (SLocationInfo *)(*it);		
		CQuaternion rotation(info->rotation.x, info->rotation.y, info->rotation.z, info->rotation.w);
		rotation.Normalize();
		CVector3D corners[8];

		int cornerID = 0;

		for (int x = -1; x <= 1; x += 2)
		for (int y = -1; y <= 1; y += 2)
		for (int z = -1; z <= 1; z += 2)
		{
			corners[cornerID].Set(info->rx * x, info->ry * y, info->rz * z);
			corners[cornerID] *= rotation;			
			cornerID++;
		}
				
		if (g->cm.IsBoundingBoxInFrustum(info->position.x, info->position.y, info->position.z, &corners[0]))
		{
			rmml::ml3DBounds bnd;

			CVector3D minC(FLT_MAX, FLT_MAX, FLT_MAX), maxC(FLT_MIN, FLT_MIN, FLT_MIN);

			for (cornerID = 0; cornerID < 8; cornerID++)
			{
				if (corners[cornerID].x > maxC.x)
				{
					maxC.x = corners[cornerID].x;
				}

				if (corners[cornerID].y > maxC.y)
				{
					maxC.y = corners[cornerID].y;
				}

				if (corners[cornerID].z > maxC.z)
				{
					maxC.z = corners[cornerID].z;
				}

				if (corners[cornerID].x < minC.x)
				{
					minC.x = corners[cornerID].x;
				}

				if (corners[cornerID].y < minC.y)
				{
					minC.y = corners[cornerID].y;
				}

				if (corners[cornerID].z < minC.z)
				{
					minC.z = corners[cornerID].z;
				}
			}

			bnd.xMin = info->position.x + minC.x;
			bnd.yMin = info->position.y + minC.y;
			bnd.zMin = info->position.z + minC.z;
			bnd.xMax = info->position.x + maxC.x;
			bnd.yMax = info->position.y + maxC.y;
			bnd.zMax = info->position.z + maxC.z;

			rectangles.push_back(bnd);
		}
	}
	return rectangles;
}

std::vector<rmml::ml3DBounds> CMapManager::GetVisibleCommunicationAreas()
{
	std::vector<rmml::ml3DBounds> rectangles;	

	std::vector<SLocationInfo *>::iterator it = m_communicationAreasInfo.begin();
	std::vector<SLocationInfo *>::iterator itEnd = m_communicationAreasInfo.end();

	for ( ; it != itEnd; it++)
	{
		SLocationInfo* info = (SLocationInfo *)(*it);
		CQuaternion rotation(info->rotation.x, info->rotation.y, info->rotation.z, info->rotation.w);
		rotation.Normalize();
		CVector3D corners[8];

		int cornerID = 0;

		for (int x = -1; x <= 1; x += 2)
			for (int y = -1; y <= 1; y += 2)
				for (int z = -1; z <= 1; z += 2)
				{
					//corners[cornerID].Set(info->rx * x, info->ry * y, info->rz * z);
					corners[cornerID].Set(info->scale.x * x, info->scale.y * y, info->scale.z * z);
					corners[cornerID] *= rotation;			
					cornerID++;
				}

				if (g->cm.IsBoundingBoxInFrustum(info->position.x, info->position.y, info->position.z, &corners[0]))
				{
					rmml::ml3DBounds bnd;

					CVector3D minC(FLT_MAX, FLT_MAX, FLT_MAX), maxC(FLT_MIN, FLT_MIN, FLT_MIN);

					for (cornerID = 0; cornerID < 8; cornerID++)
					{
						if (corners[cornerID].x > maxC.x)
						{
							maxC.x = corners[cornerID].x;
						}

						if (corners[cornerID].y > maxC.y)
						{
							maxC.y = corners[cornerID].y;
						}

						if (corners[cornerID].z > maxC.z)
						{
							maxC.z = corners[cornerID].z;
						}

						if (corners[cornerID].x < minC.x)
						{
							minC.x = corners[cornerID].x;
						}

						if (corners[cornerID].y < minC.y)
						{
							minC.y = corners[cornerID].y;
						}

						if (corners[cornerID].z < minC.z)
						{
							minC.z = corners[cornerID].z;
						}
					}

					bnd.xMin = info->position.x + minC.x;
					bnd.yMin = info->position.y + minC.y;
					bnd.zMin = info->position.z + minC.z;
					bnd.xMax = info->position.x + maxC.x;
					bnd.yMax = info->position.y + maxC.y;
					bnd.zMax = info->position.z + maxC.z;

					rectangles.push_back(bnd);
				}
	}
	return rectangles;
}

int CMapManager::IsLocationCanBeCreated(rmml::ml3DPosition* aLeftTopPoint, rmml::ml3DPosition* aRightBottomPoint )
{
	int errorCode = 0;
	if ((!m_context) || (!m_context->mpSM))
	{
		return OMS_OK;
	}
	// Проверяем не вышли ли мы в море ну и еще что-нить
	// Проверим 5 краевых точек
	//    A --------------B
	//    |       C       |
	//    D---------------E
	
	// A
	errorCode = 3;
	CVector3D point = g->phom.GetProjectionToLand(CVector3D((float)aLeftTopPoint->x, (float)aLeftTopPoint->y, 0));
	if (point.z<0)
		return errorCode;
	// B
	point = g->phom.GetProjectionToLand(CVector3D((float)aRightBottomPoint->x, (float)aLeftTopPoint->y, 0));
	if (point.z<0)
		return errorCode;
	// D
	point = g->phom.GetProjectionToLand(CVector3D((float)aLeftTopPoint->x, (float)aRightBottomPoint->y, 0));
	if (point.z<0)
		return errorCode;
	// E
	point = g->phom.GetProjectionToLand(CVector3D((float)aRightBottomPoint->x, (float)aRightBottomPoint->y, 0));
	if (point.z<0)
		return errorCode;
	// C


	assert(m_context);
	assert(m_context->mpSM);

	// Проверяем нет ли пересечения с имеющимися локациями
		std::vector<rmml::ml3DBounds> locations = GetVisibleLocations();
		std::vector<rmml::ml3DBounds>::iterator it = locations.begin();
		std::vector<rmml::ml3DBounds>::iterator itEnd = locations.end();

		if (locations.size()!=0)
	{

		errorCode = 2;
		for ( ; it != itEnd; it++)
		{
				rmml::ml3DBounds location = (rmml::ml3DBounds)(*it);
			// Проверяем пересекается ли локация с точками
			// критерий не пересечения двух прямоугольников 
			// r1.left>r2.right or r2.left>r1.right or r1.top>r2.bottom or r2.top>r1.bottom
			// r1 - это прямоугольник из точек aLeftTopPoint, aRightBottomPoint
			float r1_left = aLeftTopPoint->x;
			float r1_right = aRightBottomPoint->x;
			float r1_top = aLeftTopPoint->y;
			float r1_bottom = aRightBottomPoint->y;
			// r2 - это прямоугольник с локацией
				float r2_left = location.xMin;
				float r2_right = location.xMax;
				float r2_top =
					location.yMin;
				float r2_bottom = location.yMax;
			// Ну и походу само сравнение
			if (r1_left>r2_right || r2_left>r1_right || r1_top>r2_bottom || r2_top>r1_bottom)
				continue;
			else
				return errorCode;
		}
		
		return 0;
	}
	return OMS_OK;

}

rmml::mlMedia* CMapManager::GetAvatarByName(std::string& name)
{
	int objectID = g->o3dm.GetObjectNumber(name);
	C3DObject* obj3d = g->o3dm.GetObjectPointer(objectID);
	if ((obj3d) && (obj3d->GetUserData(USER_DATA_NRMOBJECT)))
	{	
		rmml::mlMedia* pmlObject = ((nrmObject*)obj3d->GetUserData(USER_DATA_NRMOBJECT))->GetMLMedia();
		if (pmlObject)
		{
			return pmlObject;
		}
	}
	return NULL;
}

voip::IVoipCharacter* CMapManager::GetIVoipAvatarByName(const char* name)
{
	std::string avatarName( name );
	rmml::mlMedia* mlAvatar = GetAvatarByName(avatarName);
	if( mlAvatar && mlAvatar->GetType() == MLMT_CHARACTER)
	{
		nrmCharacter* apCharacter = (nrmCharacter*)(mlAvatar->GetSafeMO());
		return (voip::IVoipCharacter*)apCharacter;
	}
	return NULL;
}

/** Метод вызывается каждый кадр. Проверяет - не пришел ли ответ по какому то из событий.
\return void
\details
*/
void	CMapManager::CheckForLoadedMapInfo()
{
	m_server->lock_data_handlers();

	static CCreateObjectsHandler _createObjectHandler;

	static CObjectsHandler _objectHandler;

	static CCreateAvatarHandler _createAvatarHandler;

	std::vector<ReceivedMessage>::iterator it = m_receivedMessages.begin()
												, end = m_receivedMessages.end();
	for (; it!=end; it++)
	{
		ReceivedMessage*	pReceivedMessage = (ReceivedMessage*)&(*it);
		CTempPacketIn*		pTempPacketIn = pReceivedMessage->pCommonPacketIn;
		MP_NEW_P2(pQueryIn, CCommonPacketIn, pTempPacketIn->GetDataBuffer(), pTempPacketIn->GetDataSize());
		/*if ((m_activeDataAsynchObjects[k] != NULL) &&
			(m_activeDataAsynchObjects[k]->IsReady()))
		{
			assert(m_activeDataAsynchObjects[k]);
		*/
		switch( pReceivedMessage->iMessageID)
		{
			case	ZONES_DESC_QUERY_ID:
			{
				// g->lw.WriteLn("[RENDERMANAGER-LOG] Zones description was received");
				CZonesHandler::Handle( pQueryIn);
				break;
			}
			case	ZONES_OBJECTS_DESC_QUERY_ID:
			{
				// g->lw.WriteLn("[RENDERMANAGER-LOG] Objects description was received");
				_objectHandler.Handle( pQueryIn, m_context, m_currentRealityID);
				// сбросим текушие приоритеты загрузки
				UpdateLoadingPriority(NULL);	
				break;
			}

			case	OBJECTS_DESC_QUERY_ID:
			{
				static CCreateObjectsHandler createObjectHandler;
				_createObjectHandler.Handle( pQueryIn);
				break;
			}

			case	AVATAR_DESC_QUERY_ID:
			{
				static CCreateAvatarHandler createAvatarHandler;
				// g->lw.WriteLn("[RENDERMANAGER-LOG] CreateAvatar description was received");
				_createAvatarHandler.Handle( pQueryIn, m_context, &m_createdAvatarID);
				break;
			}

			case	REALITY_DESC_QUERY_ID:
			{
				// g->lw.WriteLn("[RENDERMANAGER-LOG] Reality description was received");
				OnRealityDesc( pQueryIn);
				break;
			}

			case	DELETE_OBJECT_QUERY_ID:
			{
				// g->lw.WriteLn("[RENDERMANAGER-LOG] DeleteObject description was received");
				OnDeleteObject( pQueryIn);
				break;
			}

			case	OBJECT_STATUS_QUERY_ID:
			{
				CObjectStatusHandler::Handle( pQueryIn, m_callbacks);
				break;
			}

			case	CREATE_LOCATION_QUERY_ID:
			{
				static CObjectsHandler objectHandle;
				g->lw.WriteLn("[RENDERMANAGER-LOG] Location description was received");
				_objectHandler.Handle( pQueryIn, m_context, m_currentRealityID);
				// сбросим текушие приоритеты загрузки
				UpdateLoadingPriority(NULL);	
				break;
			}
		}
		MP_DELETE(pTempPacketIn);
	}
	
	m_receivedMessages.clear();
	m_server->unlock_data_handlers();
}


void CMapManager::OnDeleteObject(CCommonPacketIn* apQueryIn)
{
	CDeleteObjectHandler handler(apQueryIn);
	unsigned int objectID;
	unsigned int bornRealityID;

	if (handler.Handle( &objectID, &bornRealityID, m_context->mpLogWriter))
	{			
		m_context->mpSM->DeleteObject(objectID, bornRealityID);
		m_commonZone->DeleteObject(objectID);
		
	}
}

void CMapManager::OnRealityDesc(CCommonPacketIn* apQueryIn)
{
	CRealityHandler handler(apQueryIn);
	CRealityInfo oRealityInfo;

	if (handler.Handle( oRealityInfo, m_context->mpLogWriter))
	{
		CRealitiesMap::iterator it = m_realitiesCache.find(oRealityInfo.GetRealityID());
		if (it == m_realitiesCache.end())
		{
			m_realitiesCache[ oRealityInfo.GetRealityID()] = oRealityInfo;
		}

		NotifyRealityLoaded(m_realitiesCache[oRealityInfo.GetRealityID()]);
	}
}

void CMapManager::NotifyRealityLoaded(CRealityInfo& realityInfo)
{
	if (realityInfo.GetRealityID() == m_teleportingRealityID)
	{
		ATLASSERT( m_teleportInfo.uRealityID == m_teleportingRealityID);
		m_teleportingRealityID = 0xFFFFFFFF;
		CheckLocation();
		if( m_callbacks != NULL)
			m_callbacks->onTeleported( &m_teleportInfo);
	}
}

void	CMapManager::OnMemoryOverflow()
{
	DeletePendingObjects(false);
}

void	CMapManager::DeletePendingObjects(bool isTimeLimited)
{
	__int64 startTime = g->tp.GetTickCount();
#pragma warning(push)
#pragma warning(disable : 4239)
	CRect2D& zonesRect = GetZonesRect();
#pragma warning(pop)

	while (m_objectsForRemove.size() > 0)
	{
		std::vector<int>::iterator iter = m_objectsForRemove.begin();

		C3DObject* obj = g->o3dm.GetObjectPointer(*iter);

	#pragma warning(push)
	#pragma warning(disable : 4239)
		CRect2D& rct = GetObjectRect(obj);
	#pragma warning(pop)
		if (!rct.IsIntersected(zonesRect))
		if (!obj->IsDeleted())
		if (obj->GetUserData(USER_DATA_NRMOBJECT) != NULL)
		{
			nrmObject* nrmObj = (nrmObject*)obj->GetUserData(USER_DATA_NRMOBJECT);
			if (nrmObj)
			{
				mlMedia* rmmlObject = nrmObj->GetMLMedia();
				if (rmmlObject)
				{
					DeleteObject(rmmlObject);
				}
			}
		}
		else
		{
		#ifdef RESOURCE_LOADING_LOG
			g->lw.WriteLn("DeleteObject ", g->o3dm.GetObjectPointer((*iter))->GetName());
		#endif
			g->o3dm.DeleteObject(*iter);
		}

		m_objectsForRemove.erase(iter);

		if ((g->tp.GetTickCount() - startTime >= MAX_DELETING_TIME) && (isTimeLimited))
		{
			break;
		}
	}

	while (m_childrenForRemove.size() > 0)
	{
		if (gRM->nrMan->DestroyMO(m_childrenForRemove[0]) != OMS_OK)
			assert(false);

		m_childrenForRemove.erase(m_childrenForRemove.begin());

		if ((g->tp.GetTickCount() - startTime >= MAX_DELETING_TIME) && (isTimeLimited))
		{
			break;
		}
	}
}

void	CMapManager::Update(DWORD time)
{
	m_server->update();

	static bool isFirstUpdate = true;

	if (isFirstUpdate)
	{
		g->lw.WriteLn("First CMapManager::Update");
	}

	isFirstUpdate = false;

	/*std::vector<C3DObject *>::iterator it = g->o3dm.GetLiveObjectsBegin();
	std::vector<C3DObject *>::iterator itEnd = g->o3dm.GetLiveObjectsEnd();

	for ( ; it != itEnd; it++)
	{
		std::string name = (*it)->GetName();
		if (name.find("_reality__") != -1)
		{
			int pos = name.find("__");
			name = name.substr(0, pos);

			int objectID = g->o3dm.GetObjectNumber(name);
			if (objectID != -1)
			{
				assert(false);
			}
		}
	}*/

//	CheckForLoadedMapInfo();

	g->pm.CustomBlockStart(CUSTOM_BLOCK_OBJECTS_MANAGEMENT);

	m_objectZone.SetMaxPriority();
	m_objectZone.Update(time, true);

	m_lastLogTimeOnUpdate += time;

	bool isUpdated = false;
	if (m_lastLogTimeOnUpdate > 5000)
	{
		isUpdated = true;
		if (m_sceneChangedCounter > 0)
		{
			std::string	sJscript = "function __rm_generated_scenechanged() { if( typeof(scene.onChanged) == \"function\") scene.onChanged();} __rm_generated_scenechanged();";
			ExecJS(sJscript.c_str());			
			m_sceneChangedCounter--;
		}

		static long passedCnt = 0;

#ifndef RESOURCE_LOADING_LOG
		if (passedCnt%4 == 0)
#endif
		if (!m_mainObject)
		{
			g->lw.WriteLn("[RENDERMANAGER-LOG] Update: Main object is null");
		}
		else
		{
			float x, y, z;
			m_mainObject->GetCoords(&x, &y, &z);
			g->lw.WriteLn("[RENDERMANAGER-LOG] Update: Main object coords: ", FloatToStr(x), " ", FloatToStr(y), " ", FloatToStr(z));
		}

		m_lastLogTimeOnUpdate = 0;
		passedCnt = (passedCnt + 1)%4;
	}

	if (!m_mainObject)
	{
		g->pm.CustomBlockEnd(CUSTOM_BLOCK_OBJECTS_MANAGEMENT);
		return;
	}

	CheckLocation();

	CheckCoordinateXY();
	MoveAvatarOnUpdate();

	m_oculusRift.Update();

	DeletePendingObjects();
	
	void* apUserData = m_mainObject->GetUserData(USER_DATA_NRMOBJECT);
	if ( apUserData)
	{
		nrmObject* apnrmObject  = (nrmObject *)apUserData;

		mlMedia* pmlMedia = apnrmObject->GetMLMedia();
		if (pmlMedia != NULL && pmlMedia->IsSynchInitialized())
		{
			ZoneID	zone = GetObjectGlobalZone(m_mainObject);
			assert(zone.is_correct());
	
			if( zone != m_currentZoneID)
			{
				ZoneID	zone = GetObjectZone(m_mainObject);
				OnZoneEntered( zone);
			}

			if( pmlMedia->GetType() == MLMT_CHARACTER)
			{
				nrmCharacter* apnrmCharacter = (nrmCharacter*)apnrmObject;

				if (g->ne.time - m_lastKinectHandleTime > 75)
				{
					apnrmCharacter->handleKinectState();
					m_lastKinectHandleTime = g->ne.time;
				}
			}
		}
	}
		
	if (m_activeZones.size() == 0)
	{
		g->pm.CustomBlockEnd(CUSTOM_BLOCK_OBJECTS_MANAGEMENT);
		return;
	}

	ZoneID currentZoneID = GetObjectGlobalZone( m_mainObject);

	UpdateLoadingPriority(g->cm.GetActiveCamera());

	bool bIsNotFlyTemp = (g->phom.GetControlledObject()) && (g->phom.GetControlledObject()->IsNotFly());
	if (!m_groundLoadedMessageWasSend)
	{
		if ((g->phom.GetControlledObject() && g->phom.GetControlledObject()->IsOnGround()) ||
			(m_commonZone->GetGroundLoadedStatus() == GROUND_LOADED_BY_TIMEOUT))
		{
			ZoneID	zoneID = GetObjectZone(m_mainObject);
			CMapManagerZone*	pzone = GetZone( zoneID);
			if (pzone)
			{
				bool isGroundLoadedByRayTraceCheck = false;
				if ((isUpdated) || (pzone->GetGroundLoadedStatus() == GROUND_OBJECT_LOADED))
				{
					float z;
					if (g->phom.TraceRayZ(m_mainObject->GetCoords().x, m_mainObject->GetCoords().y, &z))
					{
						isGroundLoadedByRayTraceCheck = true;
					}
				}

				if (((pzone->GetGroundLoadedStatus() == GROUND_OBJECT_LOADED) && (isGroundLoadedByRayTraceCheck)) ||
					(m_commonZone->GetGroundLoadedStatus() == GROUND_LOADED_BY_TIMEOUT))
				{
					g->lw.WriteLn("[RENDERMANAGER-LOG] Start render 3d");

					StartRender3D();

					m_groundLoadedMessageWasSend = true;
					WriteLog("m_groundLoadedMessageWasSend changed to true [2]");

					std::string	sJscript = "function __rm_generated_sceneloaded() { if( typeof(scene.onGroundLoaded) == \"function\") scene.onGroundLoaded();} __rm_generated_sceneloaded();";
					ExecJS(sJscript.c_str());
					UpdateVoipConnection();
					if( m_context->mpInfoMan->IsTestMode())
					{
						m_context->mpInfoMan->ShowWindow( true);
						m_context->mpInfoMan->StartAutoTesting();
					}
				}
			}
		}
	}
	else
	{
		if( bIsNotFly != bIsNotFlyTemp)
		{
			bIsNotFly = bIsNotFlyTemp;
			std::string	sJscript = "if( typeof(scene.onGroundLoadStatus) == \"function\") scene.onGroundLoadStatus(";
			sJscript += (bIsNotFly ? "true" : "false");
			sJscript += ");";
			ExecJS(sJscript.c_str());
		}
	}

	m_objectZone.Update( time, true);

	m_updateProgressZonesCounter++;
	if( m_updateProgressZonesCounter >= m_activeZones.size())
		m_updateProgressZonesCounter = 0;

	DWORD counter = 0;
	CVectorZones::iterator	zit = m_activeZones.begin(),
							zend = m_activeZones.end();


	for (;zit!=zend;zit++, counter++)
	{
		(*zit)->Update( time, counter==m_updateProgressZonesCounter?true:false);
	}

	m_commonZone->Update( time, true);

	g->pm.CustomBlockEnd(CUSTOM_BLOCK_OBJECTS_MANAGEMENT);
}

bool	CMapManager::TraceGroundLevelAtPoint( float ax, float ay, float* az)
{
	bool isGroundLoadedByRayTraceCheck = false;
	ZoneID zoneID = ZoneID::get_zone_id_from_xy( ax, ay);
	CMapManagerZone*	pzone = GetZone( zoneID);
	if (pzone)
	{
		if ((pzone->GetGroundLoadedStatus() == GROUND_OBJECT_LOADED) || 
			(pzone->GetGroundLoadedStatus() == GROUND_LOADED_BY_TIMEOUT))
		{
			float z;
			if (g->phom.TraceRayZ( ax, ay, &z))
			{
				isGroundLoadedByRayTraceCheck = true;
			}
			if( az)
				*az = z;
		}
	}

	return isGroundLoadedByRayTraceCheck;
}

void	CMapManager::SetScene(CScene* scene)
{
	m_scene3d = scene;

	gRM->wbMan->SetMapManager(this);
}

nrmPlugin*	CMapManager::CreatePlugin( )
{
	MP_NEW_P(plugin, nrmPlugin, NULL);
	plugin->SetRenderManager(m_pRenderManager);

	return plugin;
}

/*
<script>
	var	temp = new {type};
	[temp._class = {class_name}];
	[temp.src = {res_path}];
	temp._parent = {scene3d_target};
</script>

<script>
	temp.props = eval('[
						{ "prop_name": "value"}
						{ "prop_name": "value"}
						{ "prop_name": "value"}
						{ "prop_name": "value"}
					   ]')
					 );
</script>
*/

void	CMapManager::ExecJS(const wchar_t* js, wchar_t** result)
{
	USES_CONVERSION;
	assert(m_pRenderManager);
	assert(m_pRenderManager->GetSceneManager());
	wchar_t* wcResult = NULL;
	m_pRenderManager->GetSceneManager()->ExecScript(js, -1, &wcResult);
	if (result)
	{
		*result = wcResult;
	}
}

void	CMapManager::ExecJS(const char* js, wchar_t** result)
{
	USES_CONVERSION;
	ExecJS( A2W(js), result);
}

bool	CMapManager::IsJSObjClassDefined(LPCTSTR className)
{
	USES_CONVERSION;
	assert(m_pRenderManager);
	assert(m_pRenderManager->GetSceneManager());
	return m_pRenderManager->GetSceneManager()->IsJSObjClassDefined(A2W(className));
}

bool	CMapManager::LoadPrimObject(std::string& fileName, std::string& objName, IAsynchResource** asynchResource)
{
	assert(asynchResource);
	return m_primObjectsLoader.LoadPrimObject(fileName, objName, asynchResource);
}

int		CMapManager::LoadClassFile(std::string& className)
{
	if( !m_context->mpComMan)
		return CLASS_LOADING;

	if( !m_classScriptsLoader.IsLocalClass(className) && !m_bModuleLoaded)
	{
		wchar_t	buf[255];
		unsigned int moduleIdLength = 0;
		bool moduleIsReady = false;
		if( !m_context->mpResM->GetModuleIDAndReady( buf, sizeof(buf)/sizeof(buf[0]), &moduleIdLength, &moduleIsReady))
			return CLASS_LOADING;

		if( moduleIdLength == 0)
			return CLASS_LOADING;

		if( !moduleIsReady)
			return CLASS_LOADING;

		m_bModuleLoaded = true;
	}

	return m_classScriptsLoader.LoadClass(className);
}

int CMapManager::LoadClassFile( const char* className )
{
	std::string classNameStr( className);
	return  LoadClassFile( classNameStr);
}

void create_scripted_object_callback(void* obj, void* params)
{
	mlObjectInfo4Creation* pMLInfo = (mlObjectInfo4Creation*)params;
	assert(pMLInfo->mpMapManager != NULL);
	ZONE_OBJECT_INFO* pInfo = (ZONE_OBJECT_INFO*)pMLInfo->mpZoneObjectInfo;
	if (!pInfo)
	{
		return;
	}
	
	mlMedia* pmlMedia = (mlMedia*)obj;
	assert(pmlMedia != NULL);
	moMedia* pObject = pmlMedia->GetSafeMO();

	// ?? - проверить тип
	nrmObject*	pnrmObject = (nrmObject*)pmlMedia->GetSafeMO();
	if ((pnrmObject) && (!ML_MO_IS_NULL(pnrmObject)))
	{
		nrm3DObject* nrm3DObj = (nrm3DObject*)pnrmObject;
		if (nrm3DObj->m_wasCallbackPerformed)
		{
			return;
		}
		if (nrm3DObj->m_obj3d)
		{
			USES_CONVERSION;
			wchar_t* wideName = pmlMedia->GetStringProp("name");
			std::wstring rmmlName;
			if (wideName)
			{
				rmmlName = wideName;
				if (rmmlName != nrm3DObj->m_obj3d->GetName())
				{
					g->o3dm.RenameObject(nrm3DObj->m_obj3d->GetID(), rmmlName);
				}
			}			

			nrm3DObj->m_obj3d->SetNeedSwitchOffPathes(pInfo->isNeedToSwitchOffPathes);

			if (pInfo->isNeedToSwitchOffPathes)
				g->pf.Delete3DObjectsPath(nrm3DObj->m_obj3d->GetID());

			nrm3DObj->m_wasCallbackPerformed = true;
			nrm3DObj->m_obj3d->SetUserData(USER_DATA_LOD, (void*)pInfo->lod);
			nrm3DObj->m_obj3d->SetUserData(USER_DATA_LEVEL, (void*)pInfo->level);
			nrm3DObj->m_obj3d->SetCoords(pInfo->translation.x, pInfo->translation.y, pInfo->translation.z);
			nrm3DObj->m_obj3d->SetScale(pInfo->scale.x, pInfo->scale.y, pInfo->scale.z);
			if (pInfo->type == vespace::VE_COMMINICATIONAREABOUNDS || pMLInfo->musType == MLMT_COMMINICATION_AREA)
			{
				nrm3DObj->m_obj3d->SetUserData(USER_DATA_IS_COMMUNICATION_AREA, (void*)true);
			}

			// сервер присылает для аватаров невалидные повороты иногда почему-то - nap
			CMapObjectQuaternion r = pInfo->rotation;
			if (nrm3DObj->m_obj3d->GetAnimation())
			if ((r.x == 0.0f) && (r.y == 0.0f) && (r.z == 0.0f))
			{
				r.z = 1.0f;
				r.w = 0.0f;
			}

			CRotationPack rot;
			rot.SetAsDXQuaternion(r.x, r.y, r.z, r.w);
			nrm3DObj->m_obj3d->SetRotation(&rot);

			if (!nrm3DObj->GetICharacter())
				if (nrm3DObj->m_obj3d->GetLODGroup())
				{
					bool isLoaded = nrm3DObj->m_obj3d->GetLODGroup()->IsGeometryLoaded();
					if (isLoaded)
					{
						nrm3DObj->UnpickableCheck();
						nrm3DObj->CheckCollisionsChanged();
					}
				}

			if (pMLInfo->isMyAvatar)
			{
				gRM->mapMan->SetAvatar(pmlMedia, *pInfo);
			}
			
			if (nrm3DObj->m_obj3d->GetLODGroup())
			{
				nrm3DObj->m_obj3d->GetLODGroup()->UpdateObject(nrm3DObj->m_obj3d);
				nrm3DObj->m_obj3d->GetLODGroup()->OnObjectCreated(nrm3DObj->m_obj3d);
			}
			
			nrm3DObj->SetBoundsFromDB(pMLInfo->mBounds);

			// установим у него правильный идентификатор зоны
			/* Alex 24.03.2011
			Устанавливать зону с клиента теперь не надо - сервер сам распределяет объекты в зависимости от координат

			pmlMedia->ZoneChanged(pInfo->zoneID.id);
			*/

			/* Alex 24.03.2011
			Тут локацию устанавливать не надо.
			При создании синхронизируемого объекта ему придут свойста, в которых будет локация. Ну или не будет если объект новый ))
			В любом случае ПОСЛЕ установки свойств в mlSceneManager::CreateObject->ApplySyncState локация будет пересчитана,
			и ЕСЛИ ОНА ИЗМЕНИТСЯ - то это будет отослано на сервер. Иначе посылаться ничего не будет.

			SLocationInfo* pLocInfo = ((CMapManager*)pMLInfo->mpMapManager)->GetObjectLocation(nrm3DObj->m_obj3d);
			if(pLocInfo != NULL){
				pmlMedia->SetLocationSID(pLocInfo->name.GetString());
			}*/
			}
		}

	// смасштабировать 
	if(pmlMedia->mType == MLMT_CHARACTER)
	{
		nrmCharacter* pCharacter = (nrmCharacter*)pObject;
		if (pCharacter->m_obj3d)
		{
//			pCharacter->m_obj3d->SetScale(pInfo->scale.x, pInfo->scale.y, pInfo->scale.z);
		}
	}
	else if(pmlMedia->mType == MLMT_OBJECT)
	{
//		nrm3DObject* p3DObject = (nrm3DObject*)pObject;
	}
	else if(pmlMedia->mType == MLMT_GROUP)
	{
//		nrm3DGroup* p3DGroup = (nrm3DGroup*)pObject;
	}
}

mlMedia* CMapManager::GetScene3DPtr()
{
	CComBSTR scene3d_target = "scene.objects";
	return m_context->mpSM->GetObjectByTarget( scene3d_target);
}

bool CMapManager::IsScene3DReady()
{
	return (GetScene3DPtr() != NULL);
}

EFillingObjectInfoResult CMapManager::FillObjectInfo( const ZONE_OBJECT_INFO& aZoneObjectInfo, rmml::mlObjectInfo4Creation& anObjectInfo )
{
	if (!AllowCreateByNapFilter(aZoneObjectInfo.objectName.c_str(), aZoneObjectInfo.className, aZoneObjectInfo.level,
		aZoneObjectInfo.type, aZoneObjectInfo.translation))
	{
		return FOI_FILTERED;
	}

	unsigned short	type;

	if ((aZoneObjectInfo.className.size() == 0) && aZoneObjectInfo.type == vespace::VE_OBJECT3D)
		return FOI_STATIC_OBJECT;
	else if ((aZoneObjectInfo.type == vespace::VE_GROUP3D) || (aZoneObjectInfo.type == vespace::VE_PRIMOBJECT3D))
		type = MLMT_GROUP;
	else if (aZoneObjectInfo.type == vespace::VE_OBJECT3D)
		type = MLMT_OBJECT;
	else if (aZoneObjectInfo.type == vespace::VE_AVATAR)
		type = MLMT_CHARACTER;
	else if (aZoneObjectInfo.type == vespace::VE_COMMINICATIONAREABOUNDS)
		type = MLMT_COMMINICATION_AREA;
	else if (aZoneObjectInfo.type == vespace::VE_PARTICLES)
		type = MLMT_PARTICLES;
	else if (aZoneObjectInfo.type == 0 && aZoneObjectInfo.complexObjectID.objectID == OBJ_ID_UNDEF)
		return FOI_WRONG_INFO;
	else
		return FOI_STATIC_OBJECT;


	if (aZoneObjectInfo.className.size() != 0)
	{
		if (LoadClassFile(aZoneObjectInfo.className.c_str()) == CLASS_NOT_EXISTS)
		{
			assert(FALSE);
			return FOI_CLASS_NOT_FOUND;
		}
	}

	std::string sResPath = aZoneObjectInfo.resPath;
	if ((sResPath.size() > 0) && (sResPath.find(":") == -1))
	{
		sResPath = "geometry\\" + sResPath;
	}
	CComBSTR bsResPath = sResPath.c_str();

	anObjectInfo.isMyAvatar = aZoneObjectInfo.isMyAvatar;
	anObjectInfo.muObjectID = aZoneObjectInfo.complexObjectID.objectID;
	anObjectInfo.muBornRealityID = aZoneObjectInfo.complexObjectID.bornRealityID;
	anObjectInfo.muRealityID = aZoneObjectInfo.complexObjectID.realityID;
	anObjectInfo.musType = type;	// RMML-тип объекта (обычно MLMT_OBJECT или MLMT_GROUP)
	anObjectInfo.mszName = aZoneObjectInfo.objectName.c_str();	// имя объекта
	anObjectInfo.mszClassName = aZoneObjectInfo.className.c_str();	// имя класса объекта
	anObjectInfo.mwcSrc = bsResPath;	// Путь к ресурсам
	anObjectInfo.mpParent = GetScene3DPtr();	// родительская 3D-сцена или 3D-группа
	anObjectInfo.mCallback.mFunc = create_scripted_object_callback;
	anObjectInfo.mCallback.mpParams = &anObjectInfo;
	anObjectInfo.mpMapManager = this;	
	anObjectInfo.mpZoneObjectInfo = &aZoneObjectInfo;
	anObjectInfo.creatingTime = aZoneObjectInfo.creatingTime;
	anObjectInfo.muLevel = aZoneObjectInfo.level;
	anObjectInfo.isHighPriority = aZoneObjectInfo.isHighPriority;
	if (aZoneObjectInfo.boundsVec.x > 0){
		CalVector bounds;
		ToCalVector( bounds, aZoneObjectInfo.boundsVec);
		bounds *= aZoneObjectInfo.rotation;
		anObjectInfo.mBounds.x = bounds.x;
		anObjectInfo.mBounds.y = bounds.y;
		anObjectInfo.mBounds.z = bounds.z;
	}	

	// Установим User Defined Properties
	//anObjectInfo.mwcParams = aZoneObjectInfo.get_paramWS("rmml");
	anObjectInfo.mwcParams = aZoneObjectInfo.GetParams();
	if (anObjectInfo.mwcParams != NULL && anObjectInfo.mwcParams[0] == 0)
		anObjectInfo.mwcParams = NULL;

	anObjectInfo.objectFullState = aZoneObjectInfo.objectFullState;
	anObjectInfo.miCreationHandlerID = aZoneObjectInfo.rmmlCreationHandlerID;

	return FOI_SUCCESS;
}

void CMapManager::ClearLocationBounds()
{
	std::vector<SLocationInfo *>::iterator it = m_locationsInfo.begin();
	std::vector<SLocationInfo *>::iterator itEnd = m_locationsInfo.end();

	for ( ; it != itEnd; it++)
	{
		MP_DELETE(*it);
	}

	m_locationsInfo.clear();
	m_currentLocation = NULL;
}

// удаление всех объектоы границ локаций, созданных не в нулевой реальности
void CMapManager::ClearLocationBoundsRNZ(){
	std::vector<SLocationInfo *>::iterator it = m_locationsInfo.begin();
	std::vector<SLocationInfo *>::iterator itEnd = m_locationsInfo.end();

	for ( ; it != itEnd; it++)
	{
		if((*it)->bornReality != 0 && (*it)->type!=vespace::VE_TEMPLOCATIONBOUNDS)
		{
			SLocationInfo * location = (*it);
			if(m_currentLocation == location)
				m_currentLocation = (SLocationInfo*)1; // в mlSynch переводится в число
			MP_DELETE(location);
			it = m_locationsInfo.erase(it);
			it--;
			itEnd = m_locationsInfo.end();
		}
	}
}

SLocationInfo *CMapManager::CreateLocationInfo( ZONE_OBJECT_INFO *info, std::string &name)
{
	/*
	/*std::string name, float x, float y, float z, float rx, float ry, float rz, 
	float rotX, float rotY, float rotZ, float rotW, std::string params, unsigned int bornReality, unsigned int type, unsigned int objectID
	*/
	/*
	info->objectName, info->translation.x, info->translation.y, info->translation.z, info->boundsVec.x, info->boundsVec.y, info->boundsVec.z,
	info->rotation.x, info->rotation.y, info->rotation.z, info->rotation.w,
	info->GetParamsString(), info->complexObjectID.bornRealityID, info->type, info->complexObjectID.objectID
	*/
	bool isMainLocation = true;
	name = info->objectName;
	int pos = info->objectName.find("___");
	if (pos != -1)
	{
		int ofs = pos + 3;
		name = name.substr(ofs, name.size() - ofs);
		isMainLocation = false;
	}
	float rotX = info->rotation.x;
	float rotY = info->rotation.y;
	float rotZ = info->rotation.z;
	float rotW = info->rotation.w;

	if ((0.0f == rotX) && (0.0f == rotY) && (0.0f == rotZ) && (1.0f == rotW))
	{
		rotZ = 1.0f;
		rotW = 0.0f;
	}

	MP_NEW_P16(loc, SLocationInfo, name, info->translation.x, info->translation.y, info->translation.z,info->boundsVec.x, info->boundsVec.y, info->boundsVec.z, info->GetParams(), isMainLocation, rotX, rotY, rotZ, rotW, info->complexObjectID.bornRealityID, info->type, info->complexObjectID.objectID);
	loc->SetScale( &info->scale);

	return loc;
}

void CMapManager::AddLocationBounds( ZONE_OBJECT_INFO *info)
{
	std::string name;
	SLocationInfo *loc = CreateLocationInfo( info, name);
	std::vector<SLocationInfo*>::iterator it = m_locationsInfo.begin();
	std::vector<SLocationInfo*>::iterator itEnd = m_locationsInfo.end();

	bool isFound = false;
	for ( ; it != itEnd; it++)
	if (((*it)->name == name) && ((*it)->position.x == info->translation.x) 
		&& ((*it)->position.y == info->translation.y) && ((*it)->position.z == info->translation.z))
	{
		isFound = true;
	}

	if (!isFound)
	{
		m_locationsInfo.push_back(loc);

		std::vector<C3DObject *>::iterator iter = g->o3dm.GetLiveObjectsBegin();
		std::vector<C3DObject *>::iterator iterEnd = g->o3dm.GetLiveObjectsEnd();

		// ищем все объекты, попадающие в эту локацию и выставлем им новую локацию
		for ( ; iter != iterEnd; iter++)
		{
			C3DObject* obj = *iter;

			if (obj->GetUserData(USER_DATA_NRMOBJECT) == NULL)
			{
				continue;
			}

			CVector3D objPos = obj->GetCoords();
			CMapObjectVector pos( objPos.x, objPos.y, objPos.z);
			pos -= loc->position;
			pos *= loc->rotation;
			pos += loc->position;

			// если объект в локации
			if ((pos.x >= (loc)->position.x - (loc)->rx) && (pos.x <= (loc)->position.x + (loc)->rx) &&
				(pos.y >= (loc)->position.y - (loc)->ry) && (pos.y <= (loc)->position.y + (loc)->ry) &&
				(pos.z >= (loc)->position.z - (loc)->rz) && (pos.z <= (loc)->position.z + (loc)->rz))
			{
				nrmObject* nrmObj = (nrmObject*)obj->GetUserData(USER_DATA_NRMOBJECT);
				if (nrmObj)
				{
					mlMedia* pmlMedia = nrmObj->GetMLMedia();
					if (pmlMedia != NULL)
					{
						USES_CONVERSION;
						pmlMedia->SetLocationSID( A2W(loc->name.c_str()));
					}
				}
			}
		}

		/*SLocationInfo* mainLocation = NULL;
		it = m_locationsInfo.begin();		
		itEnd = m_locationsInfo.end();
		for ( ; it != itEnd; it++)
			if (((*it)->name == name) && ((*it)->isMainLocation))
			{
				mainLocation = *it;
				break;
			}

		if (mainLocation)
		{
			it = m_locationsInfo.begin();		

			for ( ; it != itEnd; it++)
			if ((*it)->name.find( name) != -1)
			{
				if( *it)->params.find( mainLocation->params.c_str()) == -1)
					(*it)->params = mainLocation->params.c_str();
				if( (*it)->ID != mainLocation->ID)
					(*it)->ID = mainLocation->ID;
			}
		}*/
	}else{
		MP_DELETE(loc);
	}
}

void CMapManager::AddCommunicationAreaScales( ZONE_OBJECT_INFO *info)
{ 
	std::string name;
	SLocationInfo *loc = CreateLocationInfo( info, name);
	std::vector<SLocationInfo*>::iterator it = m_communicationAreasInfo.begin();
	std::vector<SLocationInfo*>::iterator itEnd = m_communicationAreasInfo.end();

	bool isFound = false;
	for ( ; it != itEnd; it++)
		if (((*it)->name == name) && ((*it)->position.x == info->translation.x) 
			&& ((*it)->position.y == info->translation.y) && ((*it)->position.z == info->translation.z))
		{
			isFound = true;
		}
	if (!isFound)
	{
		m_communicationAreasInfo.push_back(loc);

		std::vector<C3DObject *>::iterator iter = g->o3dm.GetLiveObjectsBegin();
		std::vector<C3DObject *>::iterator iterEnd = g->o3dm.GetLiveObjectsEnd();

		// ищем все объекты, попадающие в эту локацию общения и выставлем им новую локацию общения
		for ( ; iter != iterEnd; iter++)
		{
			C3DObject* obj = *iter;

			if (obj->GetUserData(USER_DATA_NRMOBJECT) == NULL)
			{
				continue;
			}

			CVector3D objPos = obj->GetCoords();
			CMapObjectVector pos( objPos.x, objPos.y, objPos.z);
			pos -= loc->position;
			pos *= loc->rotation;
			pos += loc->position;

			// если объект в зоне общения
			double radius = (double)loc->scale.x;
			double radiusTest = sqrt((pos.x - loc->position.x)*(pos.x - loc->position.x) + (pos.y - loc->position.y)*(pos.y - loc->position.y));
			if( radius >= radiusTest && (pos.z >= loc->position.z - loc->scale.z) && (pos.z <= loc->position.z + (loc)->scale.z))
			{
				nrmObject* nrmObj = (nrmObject*)obj->GetUserData(USER_DATA_NRMOBJECT);
				if (nrmObj)
				{
					mlMedia* pmlMedia = nrmObj->GetMLMedia();
					if (pmlMedia != NULL)
					{
						USES_CONVERSION;
						pmlMedia->SetCommunicationAreaSID( A2W(loc->name.c_str()));
					}
				}
			}
		}
	}else{
		MP_DELETE(loc);
	}
}

const char* CMapManager::GetObjectLocationName(mlMedia* mlObj)
{
	if (!mlObj)
	{
		return NULL;
	}

	nrm3DObject* nrmObj = (nrm3DObject*)mlObj->GetSafeMO();
	if (!nrmObj)
	{
		return NULL;
	}

	if (!nrmObj->m_obj3d)
	{
		return NULL;
	}

	SLocationInfo*	info = GetObjectLocation(nrmObj->m_obj3d);
	if (!info)
	{
		return NULL;
	}

	return info->name.c_str();
}

SLocationInfo* CMapManager::GetObjectLocation(C3DObject* obj)
{
	if (!obj)
	{
		return NULL;
	}

	/*std::string name = obj->GetName();
	if (name.find("sq1_schlclass") != -1)
	{
		int ii = 0;
	}*/

	CVector3D pos = obj->GetCoords();
	return GetLocationByXYZ(pos.x, pos.y, pos.z);
}

std::string CMapManager::GetObjectCommunicationAreaName(mlMedia* mlObj)
{
	std::string sAreaName;
	if (!mlObj)
	{
		return sAreaName;
	}

	nrm3DObject* nrmObj = (nrm3DObject*)mlObj->GetSafeMO();
	if (!nrmObj)
	{
		return sAreaName;
	}

	if (!nrmObj->m_obj3d)
	{
		return sAreaName;
	}


	std::vector<SLocationInfo *> vCommunicationArea;
	GetObjectCommunicationArea(nrmObj->m_obj3d, vCommunicationArea);
	
	if (vCommunicationArea.size() < 1)
	{
		return sAreaName;
	}
		
	GetCommunicationAreaName( vCommunicationArea, sAreaName);
	return sAreaName;
}

void CMapManager::GetObjectCommunicationArea(C3DObject* obj, std::vector<SLocationInfo *> &av_CommunicationArea)
{
	if (!obj)
	{
		return;
	}
	CVector3D pos = obj->GetCoords();
	GetCommunicationAreaByXYZ(pos.x, pos.y, pos.z, av_CommunicationArea);
}


SLocationInfo* CMapManager::GetLocationByID(const wchar_t* apwcLocationID)
{
	if( !apwcLocationID)	
		return NULL;

	USES_CONVERSION;
	std::string sQueryLocationID( (char*)W2A(apwcLocationID));	
	sQueryLocationID = StringToLower(sQueryLocationID);

	std::vector<SLocationInfo *>::iterator it = m_locationsInfo.begin();
	std::vector<SLocationInfo *>::iterator itEnd = m_locationsInfo.end();

	for ( ; it != itEnd; it++)
	{
		std::string sLocationID( (*it)->name);	
		sLocationID = StringToLower(sLocationID);
		if( sLocationID == sQueryLocationID)
			return (*it);
	}

	return NULL;
}

SLocationInfo* CMapManager::GetLocationByXYZ(float x, float y, float z)
{
	CVector3D pos(x, y, z);

	SLocationInfo* newLocation = NULL;
	std::vector<SLocationInfo *>::iterator it = m_locationsInfo.begin();
	std::vector<SLocationInfo *>::iterator itEnd = m_locationsInfo.end();

	for ( ; it != itEnd; it++)
	{
		CMapObjectVector checkPos( x, y, z);
		checkPos.rotate( (*it)->position, (*it)->rotation);
		if ((checkPos.x >= (*it)->position.x - (*it)->rx) && (checkPos.x <= (*it)->position.x + (*it)->rx) &&
			(checkPos.y >= (*it)->position.y - (*it)->ry) && (checkPos.y <= (*it)->position.y + (*it)->ry))
		{
			if( (*it)->type != vespace::VE_COMMINICATIONAREABOUNDS)
			{				
				newLocation = (*it);
				break;
			}
		}
	}
	return newLocation;
}

int CMapManager::IsCurrentCommunicationArea( SLocationInfo *apArea)
{
	MP_VECTOR<SLocationInfo *>::iterator iter = m_vCurrentCommunicationArea.begin();
	for( int i = 0;iter != m_vCurrentCommunicationArea.end(); iter++,i++)
	{
		SLocationInfo *pCurrentArea = (*iter);
		if( pCurrentArea->name == apArea->name)
			return i;
	}
	return -1;
}

void CMapManager::GetCommunicationAreaByXYZ(float x, float y, float z, std::vector<SLocationInfo *> &avCommunicationArea)
{
	CVector3D pos(x, y, z);

	std::vector<SLocationInfo *>::iterator it = m_communicationAreasInfo.begin();
	std::vector<SLocationInfo *>::iterator itEnd = m_communicationAreasInfo.end();

	for ( ; it != itEnd; it++)
	{
		CMapObjectVector checkPos( x, y, z);
		checkPos.rotate( (*it)->position, (*it)->rotation);
		double radius = (double)(*it)->scale.x;
		double radiusTest = sqrt((checkPos.x - (*it)->position.x)*(checkPos.x - (*it)->position.x) + (checkPos.y - (*it)->position.y)*(checkPos.y - (*it)->position.y));
		if( radius >= radiusTest)
		{			
			avCommunicationArea.push_back( (*it));		}		
	}
}

// Получить точки пересечения отрезка (ptP1, ptP2) с границами локации
// Возвращает количество пересечений и сами пересечения в ptCP1 и ptCP2
int CMapManager::GetCrossingsWithLocationBounds(const SLocationInfo *pLocInfo, const CPoint &ptP1, const CPoint &ptP2, CPoint &ptCP1, CPoint &ptCP2){
	CCrossingLine crossingLine;

	int iCrossCount = 0;	// количество пересечений

	CPoint ptBL1, ptBL2; // концы отрезка границы
	int iaSigns[] = {
		-1, +1, +1, +1, +1, -1, -1, -1, -1, +1
	};
	int idx = 0;
	for(int i = 0; i < 4; i++){
		ptBL1.x = pLocInfo->position.x + iaSigns[idx + 0] * pLocInfo->rx;
		ptBL1.y = pLocInfo->position.y + iaSigns[idx + 1] * pLocInfo->ry;
		ptBL2.x = pLocInfo->position.x + iaSigns[idx + 2] * pLocInfo->rx;
		ptBL2.y = pLocInfo->position.y + iaSigns[idx + 3] * pLocInfo->ry;
		CrossResultRec *pCrossResult = crossingLine.GetResult(ptP1, ptP2, ptBL1, ptBL2);
		if(pCrossResult->type == ctOnBounds || pCrossResult->type == ctInBounds){
			if(iCrossCount == 0) ptCP1 = pCrossResult->pt; else ptCP2 = pCrossResult->pt;
			iCrossCount++;
		}
		idx += 2;
	}

	return iCrossCount;
}

// исходя из ближайшей точке локации к искомому объекту, проверяем пересечение с двумя отрезками
bool CMapManager::GetCrossingLocationBounds( SLocationInfo *pLocInfo, CPoint &ptObj, CPoint &ptMyAvatar, CPoint &pt1, CPoint &ptRes)
{
	CPoint pt21;
	CPoint pt22;

	if( ceil( pLocInfo->position.x + pLocInfo->rx) == ceil( pt1.x))
	{
		pt21.x = pLocInfo->position.x + pLocInfo->rx;
		pt22.x = pLocInfo->position.x - pLocInfo->rx;
	}
	else
	{
		pt21.x = pLocInfo->position.x - pLocInfo->rx;
		pt22.x = pLocInfo->position.x + pLocInfo->rx;
	}

	if( ceil( pLocInfo->position.y + pLocInfo->ry) == ceil( pt1.y))
	{
		pt21.y = pLocInfo->position.y - pLocInfo->ry;
		pt22.y = pLocInfo->position.y + pLocInfo->ry;
	}
	else
	{
		pt21.y = pLocInfo->position.y + pLocInfo->ry;
		pt22.y = pLocInfo->position.y - pLocInfo->ry;		
	}

	bool bRes = false;
	CCrossingLine crossingLine;
	CrossResultRec *pCrossResult = crossingLine.GetResult( ptObj, ptMyAvatar, pt1, pt21);
	if(pCrossResult->type == ctOnBounds || pCrossResult->type == ctInBounds)
	{
		ptRes.x = pCrossResult->pt.x;
		ptRes.y = pCrossResult->pt.y;
		bRes = true;
	}	
	pCrossResult = crossingLine.GetResult( ptObj, ptMyAvatar, pt1, pt22);
	if(pCrossResult->type == ctOnBounds || pCrossResult->type == ctInBounds)
	{
		if( !bRes)
		{
			ptRes.x = pCrossResult->pt.x;
			ptRes.y = pCrossResult->pt.y;
		}
		else if( (pCrossResult->pt.x - ptObj.x) * (pCrossResult->pt.x - ptObj.x) + (pCrossResult->pt.y - ptObj.y) * (pCrossResult->pt.y - ptObj.y)
			< (ptRes.x - ptObj.x) * (ptRes.x - ptObj.x) + (ptRes.y - ptObj.y) * (ptRes.y - ptObj.y))
		{
			ptRes.x = pCrossResult->pt.x;
			ptRes.y = pCrossResult->pt.y;
		}
		bRes = true;
	}
	else if(!bRes)
	{
		g->lw.WriteLn("[CROSS] FALSE 1");
		g->lw.WriteLn("[CROSS] ptObj.x=", ptObj.x, "  ptObj.y=", ptObj.y, "  ptMyAvatar.x=", ptMyAvatar.x, "  ptMyAvatar.y=", ptMyAvatar.y);
		g->lw.WriteLn("[CROSS] pt1.x=", pt1.x, "  pt1.y=", pt1.y);
		g->lw.WriteLn("[CROSS] pt21.x=", pt21.x, "  pt21.y=", pt21.y, "  pt22.x=",  pt22.x, "  pt22.y=", pt22.y);
		g->lw.WriteLn("[CROSS] CEIL pt1.x=", ceil( pt1.x), "  pt1.y=", ceil( pt1.y));
		g->lw.WriteLn("[CROSS] CEIL pt21.x=", ceil( pt21.x), "  pt21.y=", ceil( pt21.y), "  pt22.x=",  ceil( pt22.x), "  pt22.y=", ceil( pt22.y));
		g->lw.WriteLn("[CROSS] pLocInfo->rx=", pLocInfo->rx, "  pLocInfo->ry=", pLocInfo->ry);
		g->lw.WriteLn("[CROSS] pLocInfo->position.x + pLocInfo->rx=", (pLocInfo->position.x + pLocInfo->rx), "  pLocInfo->position.y + pLocInfo->ry=", (pLocInfo->position.y + pLocInfo->ry));
		g->lw.WriteLn("[CROSS] pLocInfo->position.x - pLocInfo->rx=", (pLocInfo->position.x - pLocInfo->rx), "  pLocInfo->position.y - pLocInfo->ry=", (pLocInfo->position.y - pLocInfo->ry));
		g->lw.WriteLn("[CROSS] FALSE 2");
	}
	return bRes;
}

bool CMapManager::GetNewPositionObjectOnBoundLocation( const wchar_t *apwcLocationID, rmml::ml3DPosition &myAvatarPos, rmml::ml3DPosition &objPos, rmml::ml3DPosition &ptObjNew)
{
	std::vector<SLocationInfo *>::iterator it = m_locationsInfo.begin();
	std::vector<SLocationInfo *>::iterator itEnd = m_locationsInfo.end();
	ptObjNew.x = 0;
	ptObjNew.y = 0;
	CPoint ptResPos(0,0);	// 

	USES_CONVERSION;
	std::string sQueryLocationID( (char*)W2A(apwcLocationID));
	sQueryLocationID = StringToLower(sQueryLocationID);
	bool bFindPos = false;

	for ( ; it != itEnd; it++)
	{
		std::string sLocationID( (*it)->name);	
		sLocationID = StringToLower(sLocationID);
		if( sLocationID != sQueryLocationID)
			continue;
		CMapObjectVector checkPosMyAvatar( myAvatarPos.x, myAvatarPos.y, myAvatarPos.z);
		checkPosMyAvatar.rotate( (*it)->position, (*it)->rotation);
		CMapObjectVector checkPos( objPos.x, objPos.y, objPos.z);
		checkPos.rotate( (*it)->position, (*it)->rotation);
		CPoint ptObj( checkPos.x, checkPos.y);
		CPoint ptMyAvatar( checkPosMyAvatar.x, checkPosMyAvatar.y);
		/* алтернативный вариант от Tandy. Можно удалять
		const SLocationInfo *pLocInfo = (*it);
		CPoint ptCP1; CPoint ptCP2;
		int iCrossingCount = GetCrossingsWithLocationBounds(pLocInfo, ptMyAvatar, ptObj, ptCP1, ptCP2);
		for(int i = 0; i < iCrossingCount; i++){
			CPoint ptCP = (i == 0)?ptCP1:ptCP2;
			// ??
		}
		*/
		CPoint pt1;
		SLocationInfo *pLocInfo = (*it);
		// находим ближайшую к объекту крайную точку баунда локации
		if( abs(pLocInfo->position.x + pLocInfo->rx - ptObj.x) < abs(pLocInfo->position.x - pLocInfo->rx - ptObj.x))
			pt1.x = pLocInfo->position.x + pLocInfo->rx;
		else
			pt1.x = pLocInfo->position.x - pLocInfo->rx;
		
		if( abs(pLocInfo->position.y + pLocInfo->ry - ptObj.y) < abs(pLocInfo->position.y - pLocInfo->ry - ptObj.y))
			pt1.y = pLocInfo->position.y + pLocInfo->ry;
		else
			pt1.y = pLocInfo->position.y - pLocInfo->ry;
		CPoint ptRes( 0,0);
		if( GetCrossingLocationBounds(pLocInfo, ptObj, ptMyAvatar, pt1, ptRes))
		{
			{
				int dx = 10;
				if ( (int)(ptRes.x) <= (int)(pLocInfo->position.x - pLocInfo->rx))
					ptRes.x = pLocInfo->position.x - pLocInfo->rx + dx;
				if ( (int)(ptRes.x) >= (int)(pLocInfo->position.x + pLocInfo->rx))
					ptRes.x = pLocInfo->position.x + pLocInfo->rx - dx;
				int dy = 10;
				if ( (int)(ptRes.y) <= (int)(pLocInfo->position.y - pLocInfo->ry))
					ptRes.y = pLocInfo->position.y - pLocInfo->ry + dy;
				if ( (int)(ptRes.y) >= (int)(pLocInfo->position.y + pLocInfo->ry))
					ptRes.y = pLocInfo->position.y + pLocInfo->ry - dy;

				// поворачиваем обратно в мировые координаты
				CMapObjectVector bpos(ptRes.x, ptRes.y, checkPos.z);
				CMapObjectQuaternion bq((*it)->rotation);
				bq.x = -bq.x; bq.y = -bq.y; bq.z = -bq.z;
				bpos.rotate( (*it)->position, bq);

				if(!bFindPos){
					ptObjNew.x = bpos.x;
					ptObjNew.y = bpos.y;
					ptObjNew.z = checkPos.z;
					bFindPos = true;
				}else{
					if(((bpos.x - objPos.x) * (bpos.x - objPos.x) + (bpos.y - objPos.y) * (bpos.y - objPos.y)) 
					 < ((ptObjNew.x - objPos.x) * (ptObjNew.x - objPos.x) + (ptObjNew.y - objPos.y) * (ptObjNew.y - objPos.y)) 
					){
						ptObjNew.x = bpos.x;
						ptObjNew.y = bpos.y;
						ptObjNew.z = checkPos.z;
					}

				}
			}			
		}		
	}
	if( !bFindPos)
		return false;
	return true;
}

void CMapManager::CheckLocation()
{
	SLocationInfo* newLocation = GetObjectLocation(m_mainObject);
	std::vector<SLocationInfo *> vCommunicationArea;
	GetObjectCommunicationArea(m_mainObject, vCommunicationArea);

	std::vector<SLocationInfo *>::iterator iter = vCommunicationArea.begin();	
	bool bChangeCL = vCommunicationArea.size() != m_vCurrentCommunicationArea.size() ? true : false;
	if( !bChangeCL)
	{
		for( ;iter != vCommunicationArea.end(); iter++)
		{
			SLocationInfo *pArea = (*iter);
			//std::vector<SLocationInfo *>::iterator iterCurr = m_vCurrentCommunicationArea.begin();
			if( IsCurrentCommunicationArea( pArea) == -1)
			{
				bChangeCL = true;
				break;
			}
		}
	}
	if( bChangeCL)
	{
		m_vCurrentCommunicationArea.clear();
		if( vCommunicationArea.size() > 0)
			m_vCurrentCommunicationArea.assign( vCommunicationArea.begin(), vCommunicationArea.end());		
	}
	if (m_currentLocation != newLocation)
	{
		bChangeCL = true;
		if (!((!PLOCATION_IS_NULL(m_currentLocation)) && !PLOCATION_IS_NULL(newLocation) && (m_currentLocation->name == newLocation->name)))
		{
			m_currentLocation = newLocation;
			OnLocationChanged();
		}
		else
		{
			m_currentLocation = newLocation;
		}
	}
	if( bChangeCL)
		OnCommunicationAreaChanged();

}

void CMapManager::CheckCoordinateXY()
{
	if (!m_mainObject)
	{
		return;
	}

	float newX, newY, tmpZ;
	m_mainObject->GetCoords(&newX, &newY, &tmpZ);

	if( (newX*newX + newY*newY) > 1)	// это заглушка. На сервер не нужно посылать координаты, пока аватар стоит в 0
	{
		nrmObject* pnrmObject = (nrmObject *)m_mainObject->GetUserData(USER_DATA_NRMOBJECT);
		if( pnrmObject)
		{
			mlMedia* pmlMedia = pnrmObject->GetMLMedia();
			if (pmlMedia && pmlMedia->GetI3DObject())
			{	
				pmlMedia->GetI3DObject()->SyncronizeCoords();
			}
		}
	}

	// for resort objects in list for creating

	static int oldAvatarX = -1;
	static int oldAvatarY = -1;

	if ((-1 == oldAvatarX) && (-1 == oldAvatarY))
	{
		oldAvatarX = newX;
		oldAvatarY = newY;
	}

	bool isChanged = (fabs(newX - oldAvatarX) > 2000) || (fabs(newY - oldAvatarY) > 2000);
	if (isChanged)
	{
		m_commonZone->ResortObjectsByDistance();
		oldAvatarX = newX;
		oldAvatarY = newY;
	}
}

void CMapManager::GetObjectsInLocation(std::vector<mlObjectInLocation>& objects)
{
	std::vector<mlObjectInLocation>::iterator it = m_selectedObjects.begin();
	std::vector<mlObjectInLocation>::iterator itEnd = m_selectedObjects.end();

	for ( ; it != itEnd; it++)
	{
		objects.push_back(*it);
	}
}

const void CMapManager::GetSynchID(unsigned int &objID, unsigned int &bornRealityID)
{
	if (!m_mainObject)
	{
		return;
	}

	nrmObject* nrmObj = (nrmObject *)m_mainObject->GetUserData(USER_DATA_NRMOBJECT);
	if (nrmObj)
	{
		mlMedia* pmlMedia = nrmObj->GetMLMedia();
		if (pmlMedia)
		{
			objID = pmlMedia->GetSynchID(bornRealityID, false);
		}
	}
}

unsigned int CMapManager::GetCurrentLocationName(wchar_t* aLocationName, unsigned int aNumberOfElements)
{
	if (aLocationName == NULL)
	{
		if (!PLOCATION_IS_NULL(m_currentLocation))
		{
			return m_currentLocation->name.length() + 1;
		}
		return 1;
	}
	memset(aLocationName, 0, aNumberOfElements*sizeof(wchar_t));

	if (!PLOCATION_IS_NULL(m_currentLocation))
	{
		USES_CONVERSION;
		wchar_t* convertedString = A2W(m_currentLocation->name.c_str());
		wcscpy_s(aLocationName, aNumberOfElements, convertedString);
		return m_currentLocation->name.length() + 1;
	}

	return 1;
}

void CMapManager::GetCommunicationAreaName( std::vector<SLocationInfo *> vCommunicationArea, std::string &asName)
{
	std::vector<SLocationInfo *>::iterator iter = vCommunicationArea.begin();
	for( ;iter != vCommunicationArea.end(); iter++)
	{
		SLocationInfo *pArea = (*iter);
		if( asName.size() > 0)
			asName += "-";
		asName += pArea->name.c_str();
	}
}


std::string CMapManager::GetCurrentCommunicationAreaName()
{
	std::string sCurrentName;
	GetCommunicationAreaName( m_vCurrentCommunicationArea, sCurrentName);
	return sCurrentName;
}

int CMapManager::SelectObjectsInLocation()
{
	if (!m_mainObject)
	{
		return 0;
	}

	m_selectedObjects.clear();
		
	SLocationInfo* objLocation = GetObjectLocation(m_mainObject);
	
	if (!objLocation)
	{
		return 0;
	}

	std::vector<C3DObject *>::iterator it = g->o3dm.GetLiveObjectsBegin();
	std::vector<C3DObject *>::iterator itEnd = g->o3dm.GetLiveObjectsEnd();

	for ( ; it != itEnd; it++)
	{
		C3DObject* obj = *it;

		if (obj->GetUserData(USER_DATA_NRMOBJECT) == NULL)
		{
			continue;
		}

		CVector3D pos = obj->GetCoords();

		SLocationInfo* newLocation = GetObjectLocation(obj);
		
		if (newLocation == objLocation)
		{
			nrmObject* nrmObj = (nrmObject*)obj->GetUserData(USER_DATA_NRMOBJECT);
			if (nrmObj)
			{
				mlMedia* pmlMedia = nrmObj->GetMLMedia();
			
				if ((pmlMedia) && (pmlMedia->IsSynchronized()))
				{
					mlObjectInLocation objectInLocation;
					objectInLocation.objectID  = pmlMedia->GetSynchID(objectInLocation.bornRealityID, false);
					if (objectInLocation.objectID != OBJ_ID_UNDEF)
					{
						m_selectedObjects.push_back(objectInLocation);
					}
				}
			}
		}
	}

	return m_selectedObjects.size();
}

void CMapManager::OnLocationChanged()
{
	if (!m_mainObject)
	{
		return;
	}

	std::wstring	sJscript;
	USES_CONVERSION;
	if (!PLOCATION_IS_NULL(m_currentLocation))
	{
		sJscript = L"if( typeof(scene.onLocationChanged) == \"function\") scene.onLocationChanged('";
		sJscript += m_currentLocation->GetParams();
		sJscript += L"','";
		sJscript += A2W(m_currentLocation->name.c_str());
		sJscript += L"');";
		nrmObject* nrmObj = (nrmObject*)m_mainObject->GetUserData(USER_DATA_NRMOBJECT);
		if (nrmObj)
		{
			mlMedia* pmlMedia = nrmObj->GetMLMedia();
			if (pmlMedia)
			{	
				unsigned int bornRealityID = 0;
				unsigned int id = pmlMedia->GetSynchID(bornRealityID, false);
				m_context->mpSM->LocationChanged(A2W(m_currentLocation->name.c_str()), id, bornRealityID);
				// Перешел в новую аудиторию
			}
		}
		gRM->cameraInLocation->EnterLocation(m_currentLocation->name,
			m_currentLocation->position.x - m_currentLocation->rx,
			m_currentLocation->position.y - m_currentLocation->ry, m_currentLocation->position.z - m_currentLocation->rz,
			m_currentLocation->position.x + m_currentLocation->rx,
			m_currentLocation->position.y + m_currentLocation->ry, m_currentLocation->position.z + m_currentLocation->rz);
	}
	else
	{
		sJscript = L"if( typeof(scene.onLocationChanged) == \"function\") scene.onLocationChanged(null);";
		nrmObject* nrmObj = (nrmObject*)m_mainObject->GetUserData(USER_DATA_NRMOBJECT);
		if (nrmObj)
		{
			mlMedia* pmlMedia = nrmObj->GetMLMedia();
			if (pmlMedia)
			{	
				unsigned int bornRealityID = 0;
				unsigned int id = pmlMedia->GetSynchID(bornRealityID, false);
				m_context->mpSM->LocationChanged(L"", id, bornRealityID);
			}
		}
		gRM->cameraInLocation->LeaveLocation();
		// Вышел из аудитории и находится вне какой-либо аудитории
	}
	ExecJS(sJscript.c_str());
	UpdateVoipConnection();
}

void CMapManager::OnCommunicationAreaChanged()
{
	if (!m_mainObject)
	{
		return;
	}
	if( m_context->mpVoipMan)
	{
		m_context->mpVoipMan->OnCommunicationAreaChanged();		
	}

	USES_CONVERSION;
	std::string	sJscript;
	if( m_vCurrentCommunicationArea.size() > 0)
	{
		sJscript = "if( typeof(scene.onCommunicationAreasChanged) == \"function\") scene.onCommunicationAreasChanged('";
		MP_VECTOR<SLocationInfo *>::iterator iter = m_vCurrentCommunicationArea.begin();
		//sJscript = "{\"areas\": [";
		for( ; iter != m_vCurrentCommunicationArea.end(); iter++)
		{
			if( iter != m_vCurrentCommunicationArea.begin())
				sJscript += ", ";
			SLocationInfo *pArea = (*iter);
			const wchar_t* apwcParams = pArea->params.c_str();
			sJscript += "{\"params\":"; sJscript += W2A(apwcParams ? apwcParams : L""); sJscript +=  ", ";
			sJscript += "\"name\":\""; sJscript += pArea->name.c_str(); sJscript += "\"}";
		}
		//sJscript += " ] }";		
		sJscript += "');";
		nrmObject* nrmObj = (nrmObject*)m_mainObject->GetUserData(USER_DATA_NRMOBJECT);
		if (nrmObj && m_context->mpSM != NULL)
		{
			mlMedia* pmlMedia = nrmObj->GetMLMedia();
			if (pmlMedia)
			{	
				unsigned int bornRealityID = 0;
				unsigned int id = pmlMedia->GetSynchID(bornRealityID, false);
				USES_CONVERSION;
				m_context->mpSM->CommunicationAreaChanged(A2W(GetCurrentCommunicationAreaName().c_str()), id, bornRealityID);
			}
		}
	}
	else
	{
		sJscript = "if( typeof(scene.onCommunicationAreasChanged) == \"function\") scene.onCommunicationAreasChanged(null);";
		nrmObject* nrmObj = (nrmObject*)m_mainObject->GetUserData(USER_DATA_NRMOBJECT);
		if (nrmObj && m_context->mpSM != NULL)
		{
			mlMedia* pmlMedia = nrmObj->GetMLMedia();
			if (pmlMedia)
			{	
				unsigned int bornRealityID = 0;
				unsigned int id = pmlMedia->GetSynchID(bornRealityID, false);
				m_context->mpSM->CommunicationAreaChanged( L"", id, bornRealityID);
			}
		}		
	}	
	ExecJS(sJscript.c_str());
}

SCRIPTED_OBJECT_DESC	CMapManager::CreateScriptedObject(ZONE_OBJECT_INFO*	info)
{
	SCRIPTED_OBJECT_DESC	desc;
	assert(info);
	if (m_scene3d == NULL)
		return desc;

	assert(m_pRenderManager);
	if (m_pRenderManager == NULL)
		return desc;

	assert(m_pRenderManager->GetSceneManager());
	if (!m_pRenderManager->GetSceneManager())
	{
		return desc;
	}

	mlObjectInfo4Creation objectInfo;

	if ((info->className.size() == 0) && (info->type == vespace::VE_OBJECT3D))
	{
		desc.obj3d = CreateStaticObject( info);
		if (desc.obj3d)
		{
			desc.obj3d->GetLODGroup()->OnObjectCreated(desc.obj3d);
		}
		return desc;
	}

	EFillingObjectInfoResult fillingObjectInfoResult = FillObjectInfo( *info, objectInfo);
	switch (fillingObjectInfoResult)
	{
	case FOI_SUCCESS:
		break;
	case FOI_CLASS_NOT_FOUND:
		desc.obj3d = NULL;
		desc.ismobject = NULL;
		return desc;
	case FOI_WRONG_INFO:
		// на сервере возникла ошибка при создании объекта
		// выдаем уведомление об этом в rmml
		if(info->rmmlCreationHandlerID != NULL && m_callbacks != NULL){
			m_callbacks->onObjectCreated(info->rmmlCreationHandlerID, 0, 0, 1, L"Undefined object server error");
		}
		return desc;
	}

	assert( m_acceptRegisterQueries);
	m_acceptRegisterQueries = false;

	mlMedia* pmlMedia = NULL;

	if ((info->type == vespace::VE_AVATAR) &&
		(info->rotation.x == 0.0f) &&
		(info->rotation.y == 0.0f) &&
		(info->rotation.z == 0.0f) &&
		(info->rotation.w == 1.0f))
	{
		info->rotation.set(0, 0, 1, 0);
	}

	if(info->rmmlCreationHandlerID != NULL)
	{
		objectInfo.miCreationHandlerID = info->rmmlCreationHandlerID;
	}

	if (info->type == vespace::VE_AVATAR &&
		(info->GetObjectID()).objectID == m_createdAvatarID &&
		(info->GetObjectID()).bornRealityID == 0) // Это аватар?
	{
		m_createdAvatarID = 0xFFFFFFFF;
		
		pmlMedia = m_context->mpSM->CreateAvatar( objectInfo);
		if (pmlMedia != NULL)
		{
			SetAvatar( pmlMedia, *info);
		}
	} 
	else
	{
		objectInfo.mPos.x = info->translation.x;
		objectInfo.mPos.y = info->translation.y;
		objectInfo.mPos.z = info->translation.z;
		pmlMedia = m_context->mpSM->CreateObject( objectInfo);
	}

	m_acceptRegisterQueries = true;
	
	// послать уведомление в SceneManager о том, что объект удачно создался (или не создался)
	bool bCreated = false;
	if( pmlMedia)
	{
		nrm3DObject* nrmObj = (nrm3DObject*)pmlMedia->GetSafeMO();

		USES_CONVERSION;
		if (!ML_MO_IS_NULL(nrmObj))
		{ 
			if (nrmObj->m_obj3d)
			if (nrmObj->m_obj3d->GetName() != pmlMedia->GetStringProp("name"))
			{
				g->o3dm.RenameObject(nrmObj->m_obj3d->GetID(), W2A(pmlMedia->GetStringProp("name")));
			}
		
			bCreated = true;
		}
		else
		{			
			g->lw.WriteLn("[ERROR] error creating MO for ", info->objectName);
		}

		if (!pmlMedia->GetI3DObject())
		{
			USES_CONVERSION;
#ifdef DEBUG
			MessageBox(NULL, W2A(pmlMedia->GetStringProp("name")), "Попытка создания ошибочного объекта", MB_ICONERROR);
#endif
			return desc;
		}

		// ?? - проверить тип
		nrmObject*	pnrmObject = (nrmObject*)pmlMedia->GetSafeMO();
		if( !ML_MO_IS_NULL(pnrmObject))
		{
			nrm3DObject* nrm3DObj = (nrm3DObject*)pnrmObject;
			if (nrm3DObj->m_obj3d)
			{
				desc.obj3d = nrm3DObj->m_obj3d;
				desc.obj3d->SetUserData(USER_DATA_LOD, (void *)info->lod);
				desc.obj3d->SetUserData(USER_DATA_LEVEL, (void *)info->level);				
			}
		}
		desc.ismobject = pmlMedia;
	}
	else
	{
		g->lw.WriteLn("[ERROR] error creating RMML object for ", info->objectName);
	}

	m_loadingQueueInfo.DeleteObject(info->complexObjectID.objectID);

	if ((desc.obj3d) && (desc.obj3d->GetLODGroup()))
	{
		desc.obj3d->GetLODGroup()->OnObjectCreated(desc.obj3d);
	}

	return desc;
}

CScene*		CMapManager::GetScene()
{
	return m_scene3d;
}

C3DObject*	CMapManager::GetAvatar()
{
	return m_mainObject;
}

void	CMapManager::ExtractIncludes( IAsynchResource* asynch, std::vector<std::string>&	includeFilesVector)
{
	if( !asynch)
		return;

	USES_CONVERSION;

	wchar_t* pIncludes[255];
	int includesCount = m_context->mpSM->ExtractIncludes( A2W(asynch->GetFileName()), (const wchar_t**)pIncludes, sizeof(pIncludes)/sizeof(pIncludes[0]));

	for (int i=0; i<includesCount; i++)
	{
		includeFilesVector.push_back( W2A(pIncludes[i]));
	}
}

C3DObject*	CMapManager::CreateStaticObject(ZONE_OBJECT_INFO* info)
{
	if (m_scene3d == NULL)
		return NULL;

	assert(info);

	std::string resPath = info->resPath;
	if ((resPath.size() > 0) && (resPath.find(":") == -1))
	{
		resPath = ":geometry\\" + resPath;
	}

	C3DObject* obj = NULL;
	if (!CreateObject3D(resPath, info->className, &obj, info->objectName, info->level, NULL, NULL, (info->level == LEVEL_GROUND)))
	{
		return NULL;
	}
	CRotationPack rot;
	rot.SetAsDXQuaternion(info->rotation.x, info->rotation.y, info->rotation.z, info->rotation.w);

	if (obj->IsDeleted())
	{
		obj->UnmarkDeleted();
	}

	obj->SetRotation(&rot);
	obj->SetCoords(info->translation.x, info->translation.y, info->translation.z);
	obj->SetScale(info->scale.x, info->scale.y, info->scale.z);
	obj->GetLODGroup()->UpdateObject(obj);
	
	return obj;
}

ZoneID	CMapManager::GetObjectGlobalZone(C3DObject* obj)
{
	if (!obj)
	{
		return ZoneID(INCORRECT_ZONEID);
	}

	ZoneID zoneID = ZoneID::get_zone_id_from_xy(obj->GetCoords().x,	obj->GetCoords().y);

	return zoneID;
}

ZoneID	CMapManager::GetObjectZone(C3DObject* obj)
{
	assert(obj);
	if (!obj)	
	{
		return ZoneID(INCORRECT_ZONEID);
	}

	ZoneID zoneID = ZoneID::get_zone_id_from_xy(obj->GetCoords().x, obj->GetCoords().y);
	return zoneID;
}

bool	CMapManager::AddZoneIfNotExists(ZoneID	zoneID)
{
	CMapManagerZone* zone = GetZone(zoneID);
	if (!zone)
	{
		MP_NEW_V(zone, CMapManagerZone, m_context);
		zone->SetLoadingQueueInfoPtr(&m_loadingQueueInfo);
		zone->SetZoneID( zoneID);
//		zone->set_bounds( m_server->GetZone_bounds( zoneID));
		m_zones.push_back( zone);

		return true;
	}

	return false;
}

void	CMapManager::SetConnectedRealityID( unsigned int aConnectedRealityID)
{
	m_currentRealityID = aConnectedRealityID;
	m_prevRealityID = m_currentRealityID;	
}

void	CMapManager::OnObjectsRecieved(unsigned int aPrevRealityID, unsigned int aCurrentRealityID,
									   const std::vector<ZONE_OBJECT_INFO *>& aNewObjects)
{
	if (m_currentRealityID != aCurrentRealityID)
		return;
	m_prevRealityID = m_currentRealityID;

	if (aPrevRealityID == aCurrentRealityID || aPrevRealityID == 0xFFFFFFFF)
		return;


	std::vector<unsigned int> vNewObjectIDs;
	vecServerZoneObjects::const_iterator itNewObjects = aNewObjects.begin();
	for (;  itNewObjects != aNewObjects.end();  itNewObjects++)
	{
		ZONE_OBJECT_INFO* pObjectInfo = (*itNewObjects);
		if (pObjectInfo->className.size() == 0)
			continue;

		vNewObjectIDs.push_back( pObjectInfo->GetObjectID().objectID);
	}

	CLogValue logValue(
		"[MapMan]: OnObjectsRecieved prevReality", aPrevRealityID, " aCurrentRealityID=", aCurrentRealityID,"\n"
		);
	m_context->mpLogWriter->WriteLnLPCSTR( logValue.GetData());

	m_context->mpSM->DeletePreviousRealityObjects( vNewObjectIDs);
	m_context->mpSyncMan->DeleteObjectsExceptReality( m_currentRealityID);

	DeleteTempLocations(vNewObjectIDs);
	//DeleteCommunicationsLocations(vNewObjectIDs);
}

void	CMapManager::RemoveLocation(const char* objName){
	USES_CONVERSION;
	std::string name  = objName;

	std::vector<SLocationInfo *>::iterator it = m_locationsInfo.begin();
	while (it != m_locationsInfo.end())
	{
		if ((*it)->name == name)
		{
			if( (*it) == m_currentLocation)
				m_currentLocation = (SLocationInfo*)1; // в mlSynch переводится в число
			MP_DELETE(*it);
			it = m_locationsInfo.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void	CMapManager::RemoveCommunicationArea(const char* objName){
	USES_CONVERSION;
	std::string name  = objName;

	std::vector<SLocationInfo *>::iterator it = m_communicationAreasInfo.begin();
	while (it != m_communicationAreasInfo.end())
	{
		if ((*it)->name == name)
		{
			int pos = IsCurrentCommunicationArea( (*it));			
			MP_DELETE(*it);
			it = m_communicationAreasInfo.erase(it);
			if( pos > -1)
			{
				m_vCurrentCommunicationArea.erase( m_vCurrentCommunicationArea.begin() + pos);
				OnCommunicationAreaChanged();
			}
		}
		else
		{
			it++;
		}
	}
}

void	CMapManager::DeleteTempLocations(const std::vector<unsigned int>& aExcludeObjects){

	std::vector<SLocationInfo *>::iterator it = m_locationsInfo.begin();
	std::vector<SLocationInfo *>::iterator itEnd = m_locationsInfo.end();

	for ( ; it != itEnd; it++)
	{
		if ((*it)->type == vespace::VE_TEMPLOCATIONBOUNDS)
		{
			std::vector<unsigned int>::const_iterator it_ex = std::find(aExcludeObjects.begin(), aExcludeObjects.end(), (*it)->ID);
			if (it_ex != aExcludeObjects.end())
				continue;
			m_context->mpSM->DeleteObject((*it)->ID, (*it)->bornReality);
			m_commonZone->DeleteObject((*it)->ID);
		}
	}
}

/*
Алгоритм восстановления квадратов при попадании в новую зону:
-------------------------------------------------------------
1. Запросить список объектов зоны

2. Определить список новых объектов, т.е. тех, которые раньше были не видны и не синхронизировались

3. Для всех новых объектов сделать запрос текущего состояния, которое ключает в себя:
 3.1. Переменные рендер-менеджера - (позиция, движение, точка перехода)
 3.2. Переменные сцена-менеджера - переменные и все функции от последнего слепка данного объекта. 
		А слепок делаем по завершению выполнения синхронизированного действия или по времени.

		(
			или можно самому скрипту давать событие на
				1. получить состояние
				2. установить состояние
			тогда скрипт сам рулит тем что отдавать и как устанавливать
		)

Алгоритм записи событий и восстановения состояния:
1. В процессе работы у синхронизируемых объектов вызываются методы и изменяются
2. 
*/

void	CMapManager::DeleteObject(mlMedia* rmmlObject)
{
	moMedia** childrenArray = NULL;
	m_context->mpSM->DeleteObject(rmmlObject, &childrenArray);
	while (*childrenArray != NULL)
	{
		moMedia* obj = *childrenArray;
		if (obj->GetICharacter())
		{
			nrmCharacter* character = (nrmCharacter *)obj;
			character->PrepareForDestroy();
		}
		if (obj->GetI3DObject())
		{
			nrm3DObject* obj3d = (nrm3DObject *)obj;
			if (obj3d->m_obj3d)
			{
				g->lw.WriteLn("[OBJECT-WARNING] CMapManager::DeleteObject for ", obj3d->m_obj3d->GetName());
			}
		}
		m_childrenForRemove.push_back(obj);
		childrenArray++;
	}
}

CRect2D CMapManager::GetZonesRect()
{
	if (!m_mainObject)
	{
		return CRect2D(0, 0, 0, 0);
	}

	ZoneID zoneID = GetObjectGlobalZone(m_mainObject);
	int ax, ay;
	float cx, cy;
	zoneID.get_zoneuv_from_zone_id(zoneID, ax, ay);
	zoneID.get_zone_center_from_xy(ax, ay, cx, cy);
	CRect2D zonesRect(cx - ZONE_SIZE * 3 / 2, cy - ZONE_SIZE * 3 / 2, cx + ZONE_SIZE * 3 / 2, cy + ZONE_SIZE * 3 / 2);

	return zonesRect;
}

CRect2D CMapManager::GetObjectRect(C3DObject* obj3d)
{
	if (!obj3d)
	{
		return CRect2D(0, 0, 0, 0);
	}

	float x, y, z;
	obj3d->GetCoords(&x, &y, &z);

	float minX, minY, minZ; 
	float maxX, maxY, maxZ; 
	obj3d->GetBoundingBox(minX, minY, minZ, maxX, maxY, maxZ);

	CRect2D rct(x + minX, y + minY, x + maxX, y + maxY);
	return rct;
}

void	CMapManager::DeleteNonZoneObjects()
{
	// временная фигня для стаса, чтобы можно было отключить удаление объектов
	if (g->rs.GetInt(VIDEO_UPDATE_DELTA_TIME) == 73)
	{
		return;
	}

#ifdef RESOURCE_LOADING_LOG
	if (m_mainObject)
	{
		float xx, yy, zz;
		m_mainObject->GetCoords(&xx, &yy, &zz);
		g->lw.WriteLn("My avatar coords: ", xx, " ", yy, " ", zz);
	}
#endif

#pragma warning(push)
#pragma warning(disable : 4239)
	CRect2D& zonesRect = GetZonesRect();
#pragma warning(pop)

	// сначала уберем из очереди удаления те объекты, которые стали попадать
	// в текущие зоны
	std::vector<int> savedObjects;
	savedObjects.reserve(m_objectsForRemove.size());

	std::vector<int>::iterator iter = m_objectsForRemove.begin();
	std::vector<int>::iterator iterEnd = m_objectsForRemove.end();

	for ( ; iter != iterEnd; iter++)
	{
		C3DObject* obj = g->o3dm.GetObjectPointer(*iter);

	#pragma warning(push)
	#pragma warning(disable : 4239)
		CRect2D& rct = GetObjectRect(obj);
	#pragma warning(pop)

		if (!rct.IsIntersected(zonesRect))
		{
			savedObjects.push_back(*iter);
		}
	}

	if (savedObjects.size() != m_objectsForRemove.size())
	{
		m_objectsForRemove.clear();

		iter = savedObjects.begin();
		iterEnd = savedObjects.end();

		for ( ; iter != iterEnd; iter++)
		{
			m_objectsForRemove.push_back(*iter);
		}
	}

	// --сначала уберем из очереди удаления те объекты, которые стали попадать
	// --в текущие зоны

	// Теперь добавим на удаление новые объекты

	m_objectsForRemove.reserve(g->o3dm.GetCount());

	std::vector<C3DObject*>::iterator it = g->o3dm.GetLiveObjectsBegin();
	std::vector<C3DObject*>::iterator itEnd = g->o3dm.GetLiveObjectsEnd();

	if (m_mainObject)
	for ( ; it != itEnd; it++)
	if ((*it != m_mainObject) && ((*it)->GetLODGroup() != m_mainObject->GetLODGroup()))
	{		
		nrmObject* rmObj = (nrmObject*)((*it)->GetUserData(USER_DATA_NRMOBJECT));

		if ((!rmObj) && ((*it)->GetAnimation()))
			if (((*it)->GetParent()) && ((*it)->GetParent()->GetAnimation()))		
			{
				rmObj = (nrmObject*)((*it)->GetParent()->GetUserData(USER_DATA_NRMOBJECT));
			}

		if (rmObj)
		{
			unsigned int bornReality;
			if (rmObj->GetMLMedia())
			{
				if (rmObj->GetMLMedia()->GetSynchID(bornReality, true) == OBJ_ID_UNDEF)
				{
					continue;
				}
			}
		}

	#pragma warning(push)
	#pragma warning(disable : 4239)
		CRect2D& rct = GetObjectRect(*it);
	#pragma warning(pop)

		if (!rct.IsIntersected(zonesRect))
		{			
			m_objectsForRemove.push_back((*it)->GetID());
		}
	}

	m_commonZone->DeleteNonZoneObjects(zonesRect);
}

bool	CMapManager::IsInZone()const
{
	return m_isInZone;
}

void	CMapManager::OnZoneEntered(ZoneID zoneID)
{
	assert(m_server);
	assert(zoneID.is_correct());

	m_isInZone = true;

	/*// Alex 2011-07-23
	if( m_currentZoneID == zoneID)
		return;*/

	CVector3D crd = GetMainObjectCoords();
	g->lw.WriteLn("[RENDERMANAGER-LOG] OnZoneEntered: ", IntToStr(zoneID.id), " (", crd.x, ", ", crd.y, ", ", crd.z, ")");

	m_currentZoneID = zoneID;
	
	if (m_mainObject)
	{
		nrmObject* nrmObj = (nrmObject*)m_mainObject->GetUserData(USER_DATA_NRMOBJECT);
		if (nrmObj)
		{
			mlMedia* pmlMedia = nrmObj->GetMLMedia();
			if (pmlMedia)
			{	
				pmlMedia->ZoneChanged(zoneID.id);
			}
		}
	}

	std::vector<ZoneID> tmpZonesIDs;
	GetNeighbourZonesIDs(zoneID, tmpZonesIDs);

	std::vector<ZoneID>::iterator it = tmpZonesIDs.begin();
	std::vector<ZoneID>::iterator itEnd = tmpZonesIDs.end();

	for ( ; it != itEnd; it++)
	{
		AddZoneIfNotExists(*it);
	}
	
	QueryZonesObjects(tmpZonesIDs);

	UpdateVoipConnection();

	DeleteNonZoneObjects();

	if (m_mainObject)
	{
		std::string	sJscript = "function __rm_generated_zonechanged() { if( typeof(scene.OnZoneEntered) == \"function\") scene.OnZoneEntered();} __rm_generated_zonechanged();";
		ExecJS(sJscript.c_str());
	}
}

bool CMapManager::QueryZonesObjects( const std::vector<ZoneID>& aZonesIDs)
{
	unsigned int uPrevRealityID = GetPrevRealityID();
	unsigned int uRealityID = GetCurrentRealityID();
	ZoneID currentZone = GetCurrentZoneID();

	std::vector<ZoneID> fullQueryZones;

	for (unsigned int k = 0; k < aZonesIDs.size(); k++)
	{
		ZoneIdentifier zoneID = aZonesIDs[k].id;
		fullQueryZones.push_back(zoneID);
	}

	// Послать запрос на сервер - для получения списка объектов по зонам.
	// По идее тут надо просто сказать серверу что клиент перешел в новую зону.
	m_server->get_zones_objects_start( uPrevRealityID, uRealityID, currentZone, fullQueryZones);

	return true;
}

void	CMapManager::GetNeighbourZonesIDs(ZoneID zoneID, std::vector<ZoneID>& am_zones)
{
	if (zoneID.id == 0xFFFFFFFF)
	{
		return;
	}

	int	zoneIDX, zoneIDY;
	ZoneID::get_zoneuv_from_zone_id(zoneID, zoneIDX, zoneIDY);

	//assert(zoneIDX > -16000);		
	//assert(zoneIDY > -16000);
	//assert(zoneIDX < 16000);	
	//assert(zoneIDY < 16000);

	am_zones.push_back(ZoneID::get_zone_id_from_zoneuv(zoneIDX - 1, zoneIDY - 1));
	am_zones.push_back(ZoneID::get_zone_id_from_zoneuv(zoneIDX - 0, zoneIDY - 1));
	am_zones.push_back(ZoneID::get_zone_id_from_zoneuv(zoneIDX + 1, zoneIDY - 1));

	am_zones.push_back(ZoneID::get_zone_id_from_zoneuv(zoneIDX - 1, zoneIDY - 0));
	am_zones.push_back(zoneID);
	am_zones.push_back(ZoneID::get_zone_id_from_zoneuv(zoneIDX + 1, zoneIDY - 0));

	am_zones.push_back(ZoneID::get_zone_id_from_zoneuv(zoneIDX - 1, zoneIDY + 1));
	am_zones.push_back(ZoneID::get_zone_id_from_zoneuv(zoneIDX - 0, zoneIDY + 1));
	am_zones.push_back(ZoneID::get_zone_id_from_zoneuv(zoneIDX + 1, zoneIDY + 1));
}

CMapManagerZone*	CMapManager::GetZone(ZoneID	zoneID)
{
	if (zoneID.id == 0xFFFFFFFF)
	{
		return &m_objectZone;
	}

	CVectorZones::iterator	it = m_zones.begin(),
							end = m_zones.end();
	for( ;it!=end;++it)
	if( (*it)->GetZoneID() == zoneID)
	{
		return *it;
	}

	return NULL;
}

void	CMapManager::UpdateResourceLoadingPriority(IAsynchResource* asynchRes)
{
    int zoneNum = 0;
		int priority = -1;

		int objectsUsingResourceCount = asynchRes->GetObjectCount();

		float distSq = FLT_MAX;

		for (int i = 0; i < objectsUsingResourceCount; i++)
		{
			C3DObject* obj = NULL;
			float koef = 1.0f;
			asynchRes->GetObject(i, (void**)(&obj), &koef);

			ZONE_OBJECT_INFO info;
			info.obj3d = obj;
			info.lod = asynchRes->GetLOD();
			info.level = (unsigned int)obj->GetUserData(USER_DATA_LEVEL);

			int _priority = -1;
		
			switch (asynchRes->GetMainPriority())
			{
			case RES_TYPE_BMF: // он же RES_TYPE_MESH, он же RES_TYPE_SKELETON
				_priority = GetObjectPriority(&info, asynchRes->GetExactObject());
				break;

			case RES_TYPE_TEXTURE:
				_priority = GetTexturePriority(&info, asynchRes->GetExactObject());
				break;

			case RES_TYPE_SOUND:
				_priority = GetSoundPriority(&info);	
				break;

			default:
				break;
			}

			if (_priority != -1)
			{
				_priority = (int)(_priority * koef);
			}

			if (_priority > priority)
			{
				priority = _priority;
				float _distSq;
				zoneNum = GetZoneNum(&info, _distSq);
				if (distSq > _distSq)
				{
					distSq = _distSq;
				}
			}
		}		

		// если объекты не используют текстуру,
		if (0 == objectsUsingResourceCount)
		{
			if (asynchRes->GetMainPriority() == RES_TYPE_TEXTURE)
			{
				switch (asynchRes->GetScriptPriority())
				{
				case 255:
				case ELP_UI_Visible:
					{
						bool isVisible = false;

						CTexture* texture = (CTexture*)(asynchRes->GetExactObject());
						if (texture)
						if (texture->GetUserData(USER_DATA_SPRITE))
						{
							std::vector<void *>* owners = texture->GetOwnersPtr();
				
							std::vector<void *>::iterator it = owners->begin();
							std::vector<void *>::iterator itEnd = owners->end();

							for ( ; it != itEnd; it++)
							{
								CSprite* sprite = (CSprite *)(*it);
								if (sprite->IsVisible())
								{
									isVisible = true;
								}
							}							
						}							
						if (isVisible)
						{
							priority = PRIORITY_VERY_BIG;
						}
						else
						{
							priority = PRIORITY_LEVEL_LAST;
						}
					}						
					break;
					
				case ELP_UI:
					priority = PRIORITY_VERY_BIG;
					break;

				case ELP_ShowNow:
					priority = PRIORITY_BIG;
					break;

				case ELP_ShowLater:
					priority = PRIORITY_MEDIUM;
					break;

				case ELP_Garbage:
					priority = PRIORITY_GARBAGE;
					break;
				}				
			}
			else
			{
				// это ошибка! для нетекстур всегда должен быть объект, к которому привязан ресурс!
				// для скриптов дб выставлен скриптовый приоритет ELP_XML
				// для фоновых звуков дб выставлен скриптовый приоритет ELP_BACKGROUND_SOUND
				if (asynchRes->GetScriptPriority() == ELP_XML)
				{
					priority = PRIORITY_VERY_BIG;
				}
				else if (asynchRes->GetScriptPriority() == ELP_BACKGROUND_SOUND)
				{
					priority = PRIORITY_MEDIUM;					
				}
				else
				{
					//assert(false);
					priority = PRIORITY_GARBAGE;					
				}
			}
		}
		else switch (asynchRes->GetScriptPriority())
		{
			case ELP_ShowNow:
				asynchRes->SetImportantSlideStatus(distSq < 3000.0f * 3000.0f);				
				break;

			case ELP_ShowLater:
				priority /= 2;
				break;

			case ELP_Garbage:
				priority = PRIORITY_GARBAGE;
				break;
		}

		if (priority != -1)
		{
			asynchRes->SetPriority((float)priority);

			asynchRes->SetZoneNum(zoneNum);
		}
		else
		{
//			gRM->resLib->GetAsynchResMan()->DeleteAsyncResourse(asynchRes, NULL, g->rl);
		}
}

void	CMapManager::UpdateLoadingPriority(CCamera3D* camera)
{
	if (camera)
	{
		SetCamera(camera);
	}
	else
	{
		return;
	}

	m_commonZone->UpdateLoadingPriority(camera);

	CVectorZones::iterator it = m_zones.begin();
	CVectorZones::iterator itEnd = m_zones.end();
	for( ; it != itEnd; it++)
	{
		(*it)->UpdateLoadingPriority(camera);
	}

	if ((g->ne.time - m_lastUpdateTime < 1000) && (m_lastUpdateTime != 0))
	{
		return;
	}
	else
	{
		m_lastUpdateTime = g->ne.time;
	}

	m_asynchResources.clear();
	m_asynchResources.reserve(gRM->resLib->GetAsynchResMan()->GetAllAsynchResourcesCount());
	gRM->resLib->GetAsynchResMan()->GetAllAsynchResources(m_asynchResources);

	std::vector<IAsynchResource *>::iterator iter = m_asynchResources.begin();
	std::vector<IAsynchResource *>::iterator iterEnd = m_asynchResources.end();

	for ( ; iter != iterEnd; iter++)
	{
		IAsynchResource* asynchRes = *iter;

		UpdateResourceLoadingPriority(asynchRes);
	}
}

void	CMapManager::ReloadZonesInfo()
{
	g->ne.SetWorkMode(WORK_MODE_DESTROY);
	
	for (int i = 0; i < g->o3dm.GetCount(); i++)
	{
		g->o3dm.DeleteObject(i);
	}

	g->ne.SetWorkMode(WORK_MODE_NORMALRENDER);

	if (m_mainObject)
	{
		ZoneID zone = GetObjectZone(m_mainObject);
		assert(zone.is_correct());
		OnZoneEntered(zone);
	}
}

// Удалить временную локацию
oms::omsresult CMapManager::DeleteLocation()
{
	//CVector3D mainCoords = GetMainObjectCoords();

	m_server->delete_location();
	g->lw.WriteLn("Delete temporary location query to server");

	return OMS_OK;
}

// запрос на сервер о создании временной локации
CVector3D *CMapManager::GetLocationVector3D(rmml::ml3DPosition* leftTopPoint,  rmml::ml3DPosition* rightBottomPoint, rmml::ml3DPosition* outsidePoint)
{	
	//    A --------------B
	//    |               |
	//    D---------------C

	// A
	CVector3D *pPpoints = MP_NEW_ARR( CVector3D, 5);
	pPpoints[0] = g->phom.GetProjectionToLand(CVector3D((float)leftTopPoint->x, (float)leftTopPoint->y, 0));
	pPpoints[1] = g->phom.GetProjectionToLand(CVector3D((float)rightBottomPoint->x, (float)leftTopPoint->y, 0));
	pPpoints[2] = g->phom.GetProjectionToLand(CVector3D((float)rightBottomPoint->x, (float)rightBottomPoint->y, 0));
	pPpoints[3] = g->phom.GetProjectionToLand(CVector3D((float)leftTopPoint->x, (float)rightBottomPoint->y, 0));
	pPpoints[4] = g->phom.GetProjectionToLand(CVector3D((float)outsidePoint->x, (float)outsidePoint->y, 0));
	return pPpoints;
}

// запрос на сервер о создании временной локации
oms::omsresult CMapManager::CreateLocation(rmml::ml3DPosition* leftTopPoint,  rmml::ml3DPosition* rightBottomPoint, rmml::ml3DPosition* outsidePoint, const wchar_t* apwcLocationName)
{

	CVector3D *pPoints = GetLocationVector3D(leftTopPoint, rightBottomPoint, outsidePoint);	

	m_server->create_location(5, pPoints, apwcLocationName);
	g->lw.WriteLn("Create temporary location query to server");
	return OMS_OK;
}

// запросить с сервера данные для создания аватара и создать его (выдает ссылку на объект)
oms::omsresult CMapManager::CreateMyAvatar(const wchar_t* apwcURL)
{
	if (m_context == NULL || m_context->mpSM == NULL)
		return OMS_OK;

	assert(m_context);
	assert(m_context->mpSM);

	// Послать запрос на сервер на получение моего аватара
	m_context->mpSM->SetInfoState( 0);
	m_server->get_avatar_start( apwcURL);

	g->lw.WriteLn("CreateMyAvatar query to server");

	return OMS_OK;
}

oms::omsresult CMapManager::CreateObjects(cmObjectIDs* objects, unsigned int objectsCount)
{
	if ((!m_context) || (!m_context->mpSM))
	{
		return OMS_OK;
	}

	assert(m_context);
	assert(m_context->mpSM);

	// Послать запрос на создание объектов
	m_server->get_objects_start(objects, objectsCount, m_currentRealityID);

	return OMS_OK;
}

// установить объект, который считать аватаром клиента
void CMapManager::SetAvatar(rmml::mlMedia* apObject, const ZONE_OBJECT_INFO& objectInfo)
{
	nrmObject*	pnrmObject = (nrmObject*)apObject->GetSafeMO();
	if ((((unsigned)pnrmObject )&~3)==0)
	{
		return;
	}	

	nrm3DObject* pnrm3DObject = (nrm3DObject*)pnrmObject;

	if( m_mainObject == pnrm3DObject->m_obj3d)
		return;

	m_mainObject = pnrm3DObject->m_obj3d;

	if (!m_mainObject)
	{
		// пытаемся обработать то, что аватар не создался
		// создаем аватара в упрощенном виде в простой функции
		ExecJS("scene.getMyAvatar().OnMyAvatarCreatingFailed();");
		// пытаемся получить 3д-объект снова
		m_mainObject = pnrm3DObject->m_obj3d;
	}

	m_mainObjectInfo = objectInfo;

	if (m_mainObject != NULL)
	{
		m_mainObject->SetCoords(m_mainObjectCoords.x, m_mainObjectCoords.y, m_mainObjectCoords.z);
		m_mainObject->SetLODDistanceCanBeChangedStatus(false);

		if (m_mainObject->GetLODGroup())
		if (g->phom.GetControlledObjectID() == 0xFFFFFFFF)
		{
			m_mainObject->GetLODGroup()->SetPhysicsType(PHYSICS_OBJECT_TYPE_BOX);
			m_mainObject->SetUserData(USER_DATA_LEVEL, LEVEL_BOX_COLLISION);
			// чтобы пересоздался физический объект, даже если он успел создаться статическим
			m_mainObject->SetPhysics(false);
			m_mainObject->SetPhysics(true);
			g->phom.SetControlledObjectID(m_mainObject->GetPhysicsObjID());

			if( apObject->GetType() == MLMT_CHARACTER)
			{
				((nrmCharacter*)pnrm3DObject)->ResetSynchKinectState();
			}
		}		

		m_mainObject->SetCoords(m_mainObjectCoords.x, m_mainObjectCoords.y, m_mainObjectCoords.z);
	}
	else
	{
		// С сервера не пришло описание вашего аватара или его создание в скрипте завершилось неудачно (src не был выставлен)
		return;		
	}
	
	OnLocationChanged();
	OnCommunicationAreaChanged();

	if (m_mainObject)
	{
	nrmObject* nrmObj = (nrmObject*)m_mainObject->GetUserData(USER_DATA_NRMOBJECT);
	if (nrmObj)
	{
		mlMedia* pmlMedia = nrmObj->GetMLMedia();
		if (pmlMedia)
		{	
			pmlMedia->ZoneChanged(GetObjectZone(m_mainObject).id);
		}
	}
	}	

	g->rs.SetInt(MAX_TEXTURE_LOADING_TIME, 5000);

	g->lw.WriteLn("[RENDERMANAGER-LOG] Set my avatar is successful done");
}

bool CMapManager::IsAvatar(rmml::mlMedia* apObject)
{ 
	if (!apObject->GetI3DObject())
	{
		USES_CONVERSION;
#ifdef DEBUG
		MessageBox(NULL, W2A(apObject->GetStringProp("name")), "Попытка удаления ошибочного объекта", MB_ICONERROR);
#endif
		return false;
	}

	nrm3DObject* pnrm3DObject = (nrm3DObject*)apObject->GetSafeMO();

	if (ML_MO_IS_NULL(pnrm3DObject))
	{
		return false;
	}

	if (m_mainObject != pnrm3DObject->m_obj3d)
	{
		return false;
	}

	return true;
}

bool CMapManager::SetCallbacks(cm::cmIMapManCallbacks* callbacks)
{
	m_callbacks = callbacks;

	return true;
}

void	CMapManager::UpdateVisibilityDistance()
{
	bool isMyHome = (GetCurrentRealityType() == cm::RT_USER);
	bool isMyHomePrev = (GetPrevRealityType() == cm::RT_USER);
    
	if ((0 == m_currentRealityID) || (isMyHome))
	{
		if (m_savedDistanceKoef != -1) 
		{
			g->rs.SetInt(NORMAL_LOD_SCALE, m_savedDistanceKoef);
			m_context->mpSM->OnVisibilityDistanceKoefChanged(m_savedDistanceKoef);
			m_savedDistanceKoef = -1;			
		}
		m_commonZone->SetTruncateMode(false);		
	}
	else
	{
		if ((0 == m_prevRealityID) || (isMyHomePrev) || (-1 == m_savedDistanceKoef))
		{
			m_savedDistanceKoef = g->rs.GetInt(NORMAL_LOD_SCALE);
		}

		int koef = g->gi.IsLowEndVideocard() ? 400 : 100;
		g->rs.SetInt(NORMAL_LOD_SCALE, koef);
		m_commonZone->SetTruncateMode(g->gi.IsLowEndVideocard());
		m_context->mpSM->OnVisibilityDistanceKoefChanged(koef);
	}	
}

bool	CMapManager::TeleportReality( cmTeleportInfo& aTeleportInfo)
{
	bool realityIsChanged = (m_currentRealityID == aTeleportInfo.uRealityID) ? false : true; 

	m_prevRealityID = m_currentRealityID;
	m_currentRealityID = aTeleportInfo.uRealityID;
	m_teleportingRealityID = aTeleportInfo.uRealityID;

	UpdateVisibilityDistance();

	if(realityIsChanged)
	{
		ClearLocationBoundsRNZ(); // Tandy: удаляем объекты границ локаций ненулевой реальности (возможно стоит вызывать в другом месте)
	}

	CRealitiesMap::iterator iter = m_realitiesCache.find(aTeleportInfo.uRealityID);
	if (iter != m_realitiesCache.end())
	{
		NotifyRealityLoaded(iter->second);
	}
	else
	{
		ATLASSERT( FALSE);
		//m_server->query_reality_info( m_currentRealityID);
	}

	return realityIsChanged;
}

void CMapManager::HandleTeleportError( unsigned int auErrorCode, const wchar_t* apwcDescription, const wchar_t* apwcEventInfo)
{
	if (m_callbacks != NULL)
		m_callbacks->onTeleportError( auErrorCode, apwcDescription, apwcEventInfo);
}

int CMapManager::GetAvatarLocationURL(wchar_t* url, int aiBufferSize)
{
	if( !url)
		return 0;
	if( aiBufferSize == 0)
		return 0;

	std::wstring	wsLocationString;
	
	CUrlBuilder urlBuilder;
	if( m_mainObject != NULL)
	{
		CVector3D pos = m_mainObject->GetCoords();
		urlBuilder.setCoords(pos.x, pos.y, pos.z);
		urlBuilder.setReality( GetCurrentRealityID());
		CRotationPack *pRot = m_mainObject->GetRotation();
		if(pRot !=NULL)
		{
			float ra = 0, rx = 0, ry = 0, rz = 0;
			pRot->GetRotating( &rx, &ry, &rz, &ra);
			urlBuilder.setRotations(ra, rx, ry, rz);
		}

		// после восстановления связи аватар запрашивается для создания в текущей реальности
		wsLocationString = urlBuilder.toString();
	}
	else
	{
		// при начальном создании аватара передаем ссылку на URL, который передан через командную строку
		if( m_context->mpSM->GetLocationURL() != NULL)
			wsLocationString = m_context->mpSM->GetLocationURL();
	}

	if( wsLocationString.length() == 0)
		return 0;

	if( wsLocationString.length() < aiBufferSize)
		rtl_wcscpy( url, aiBufferSize, wsLocationString.c_str());
	else
		rtl_wcsncpy( url, aiBufferSize, wsLocationString.c_str(), aiBufferSize-1);
		
	return wcslen(url);
}

void CMapManager::GetAvatarQuaternion( float& aX, float& aY, float& aZ, float& aW)
{
	if( m_mainObject != NULL)
	{
		CRotationPack *pRot = m_mainObject->GetRotation();
		CQuaternion quat = pRot->GetAsDXQuaternion();
		aX = quat.x;
		aY = quat.y;
		aZ = quat.z;
		aW = quat.w;
	}
}

void CMapManager::GetAvatarPosition( float& aX, float& aY, float& aZ, char** aLocation)
{
	CVector3D v = GetMainObjectCoords();
	aX = v.x;
	aY = v.y;
	aZ = v.z;

	if (aLocation)
	{
		SLocationInfo* loc = GetLocationByXYZ(v.x, v.y, v.z);
		if (loc)
		{
			*aLocation = (char*)loc->name.c_str(); 
		}
		else
		{
			*aLocation = NULL;
		}
	}
}

// перемещает аватар
void CMapManager::MoveAvatar(const cmTeleportInfo& teleportInfo)
{
	m_teleportInfo = teleportInfo;
	m_needTeleport = true;

	float z;
	if( m_groundLoadedMessageWasSend)
		m_groundLoadedMessageWasSend = TraceGroundLevelAtPoint(teleportInfo.x, teleportInfo.y, &z);

	if( m_groundLoadedMessageWasSend)
		WriteLog("m_groundLoadedMessageWasSend changed to true [3]");
	else
		WriteLog("m_groundLoadedMessageWasSend changed to false [4]");

	MoveAvatarOnUpdate();
}

// перемещает аватар
void CMapManager::MoveAvatarOnUpdate()
{
	if (!m_needTeleport)
		return;

	g->lw.WriteLn("[RENDERMANAGER-LOG] MoveAvatarOnUpdate: Before teleport - x: ", m_teleportInfo.x, ", y: ", m_teleportInfo.y, ", z: ", m_teleportInfo.z, ", realetyID: ", m_teleportInfo.uRealityID);

	CalVector v( m_teleportInfo.x, m_teleportInfo.y, m_teleportInfo.z);
	CalQuaternion q( m_teleportInfo.rx, m_teleportInfo.ry, m_teleportInfo.rz, m_teleportInfo.ra);

	if (m_mainObject != NULL)
	{	
		m_mainObject->SetCoords(v.x, v.y, v.z);

		if ( TeleportReality( m_teleportInfo) /*reality is changed*/)
		{
			ReloadWorld();

			if (m_mainObject)
			{
				nrmObject* nrmObj = (nrmObject*)m_mainObject->GetUserData(USER_DATA_NRMOBJECT);
				if (nrmObj)
				{
					mlMedia* pmlMedia = nrmObj->GetMLMedia();
					if (pmlMedia != NULL)
					{
						pmlMedia->RealityChanged(m_currentRealityID);
					}
				}
			}
		}

		if (m_mainObject->GetRotation())
		{
			m_mainObject->GetRotation()->SetAsDXQuaternion(q.x, q.y, q.z, q.w);
		}

		m_needTeleport = false;
	}
}

const wchar_t* gszFilterFile = L"filter.txt";

// читает файл фильтра объектов, которые можно создавать
// формат: 
// 1-я срока: 
//   '*' - создавать все объекты, кроме перечисленных ниже (иначе создавать только перечисленные объекты)
// остальные строки:
//   '...*' - (не)создавать все объекты, начинающиеся на указанный префикс
//   '*...[*]' - (не)создавать все объекты, включающие в себя указанную подстроку 
//   '{тип}' - (не)создавать объекты, имеющие заданный тип. (пока не знаю зачем, поэтому не реализовано)
// все в строках, идущее после пробела считается комментарием
void CMapManager::ReadFilterFile()
{
	WCHAR lpcStrBuffer[MAX_PATH*2+2];
	::GetModuleFileNameW( GetModuleHandleW(NULL), lpcStrBuffer, MAX_PATH);
	WCHAR lpcExeFullPathName[MAX_PATH*2+2];
	LPWSTR pTemp;
	::GetFullPathNameW((const WCHAR *)lpcStrBuffer,
		MAX_PATH*2,
		lpcExeFullPathName,
		&pTemp);
	WCHAR lpcExeName[MAX_PATH];
	wcscpy(lpcExeName,pTemp);
	// cut '.exe'
	int ii=0;
	for(; lpcExeName[ii]; ii++);
	for(; ii>0 && lpcExeName[ii]!=L'.'; ii--);
	if(ii>0) lpcExeName[ii]=0;
	// cut name form pathName
	*pTemp=0;
	// compile INI-file FullPathName
	WCHAR lpcIniFullPathName[MAX_PATH*2+2];
	wcscpy(lpcIniFullPathName,lpcExeFullPathName);
	wcscat(lpcIniFullPathName, gszFilterFile);

	FILE* file = _wfopen(lpcIniFullPathName, L"r");
	if(file == NULL)
		return;
	while(!feof(file)){
#define LINE_MAX_SIZE 1000
		char pszLine[LINE_MAX_SIZE+1];	memset( pszLine, 0, sizeof(pszLine)/sizeof(pszLine[0]));
		/*char* lpcRet = */fgets(pszLine, LINE_MAX_SIZE, file);
		pszLine[LINE_MAX_SIZE] = '\0';
		// берем из строки все, что до первого пробела (остальное будем считать комментарием)
		char pszMask[LINE_MAX_SIZE+1];
		char* pcMask = pszMask;
		for(const char* pcLine = pszLine; *pcLine != '\0'; pcLine++, pcMask++){
			if(*pcLine == ' ' || *pcLine == 13 || *pcLine == 10)
				break;
			*pcMask = *pcLine;
		}
		*pcMask = '\0';
		if(*pszMask == '\0')
			continue;
		m_filter.push_back(MAKE_MP_STRING(std::string(pszMask)));
	}
	fclose(file);
}

// проверяет в соответствии с Nap-фильтром, можно ли создавать объект?
bool CMapManager::AllowCreateByNapFilter(const std::string& /*objName*/, const std::string& className,
										 const int level, const int type, const CMapObjectVector &translation)
{
	static bool isInitializated = false;
	static bool isNeedToHideNonGroundObjects = false;
	static bool isNeedToHideNonGroundObjectsExceptNearest = false;
	static bool isNeedToHideOtherAvatars = false;
	static bool isNeedToHideScriptedObjects = false;
	static bool isNeedToHideScriptedObjectsExceptNearest = false;
	static bool isNeedToHideNotes = false;

	if (!isInitializated)
	{
		CIniFile iniFile(GetApplicationRootDirectory() + L"filter.ini");
		isNeedToHideNonGroundObjects = iniFile.GetBool("hide", "non_ground_objects");
		isNeedToHideNonGroundObjectsExceptNearest = iniFile.GetBool("hide", "non_ground_objects_except_nearest");
		isNeedToHideOtherAvatars = iniFile.GetBool("hide", "other_avatars");
		isNeedToHideScriptedObjects = iniFile.GetBool("hide", "scripted_objects");
		isNeedToHideScriptedObjectsExceptNearest = iniFile.GetBool("hide", "scripted_objects_except_nearest");
		isNeedToHideNotes = iniFile.GetBool("hide", "notes");

		if (isNeedToHideNotes)
		{
			std::string	sJscript = "function __rm_generated_disablenotes() { scene.disableNotes = true; } __rm_generated_disablenotes();";
			ExecJS(sJscript.c_str());
		}

		isInitializated = true;
	}

	if ((vespace::VE_AVATAR == type) && (m_mainObject != NULL) && (isNeedToHideOtherAvatars))
	{
		return false;
	}

	if ((vespace::VE_OBJECT3D == type) && (isNeedToHideNonGroundObjects) && (className.size() == 0) &&
		(LEVEL_SEA != level) && (LEVEL_GROUND != level))
	{
		return false;
	}

	bool isNeedDistanceCheck = false;

	if ((vespace::VE_OBJECT3D == type) && (isNeedToHideNonGroundObjectsExceptNearest) && (className.size() == 0) &&
		(LEVEL_SEA != level) && (LEVEL_GROUND != level))
	{
		isNeedDistanceCheck = true;
	}

	if ((vespace::VE_OBJECT3D == type) && (isNeedToHideScriptedObjects) && (className.size() > 0))
	{
		return false;
	}

	if ((vespace::VE_OBJECT3D == type) && (isNeedToHideScriptedObjectsExceptNearest) && (className.size() > 0))
	{
		isNeedDistanceCheck = true;
	}

	if (isNeedDistanceCheck)
	{
		CVector3D avatarCoords = GetMainObjectCoords();
		avatarCoords.z = 0;
		avatarCoords.x -= translation.x;
		avatarCoords.y -= translation.y;
		if (avatarCoords.GetLengthSq() > 4000 * 4000)
		{
			return false;
		}
	}

	return true;
}

// проверяет в соответствии с фильтром, можно ли создавать объект?
bool CMapManager::AllowCreateByFilter(const char* objName, CMapObjectVector &translation)
{
	if(m_filter.size() == 0)
		return true; 
	const char* pcLocation = "";
	SLocationInfo* locinfo = GetLocationByXYZ(translation.x, translation.y, translation.z);
	if(locinfo != NULL){
		pcLocation = locinfo->name.c_str();
	}
	MP_VECTOR<MP_STRING>::const_iterator vci = m_filter.begin();
	bool bExclude = false;
	for( ; vci != m_filter.end(); vci++){
		const char* pcLine = (*(vci)).c_str();
		// если '*', то 
		if(pcLine[0] == '*' && pcLine[1] == '\0'){
			// если в самом начале
			if(vci == m_filter.begin())
				bExclude = true;
			continue;
		}
		bool bMatched = false;
		bool bLocationMatched = true;
		const char* pcMask = pcLine;
		// если в маске указана локация
		if(pcLine[0] == ':'){
			bool bLocationMatched = false;
			const char* pcMaskLoc = (pcLine + 1);
			const char* pcLoc = pcLocation;
			for(;; pcMaskLoc++, pcLoc++){
				if(*pcMaskLoc == *pcLoc){
					// если закончились и локация и маска
					if(*pcMaskLoc == '\0'){
						bLocationMatched = true; // то совпадает
						pcLine = pcMaskLoc;
						break;
					}
					continue;
				}
				// если локация в фильтре закончилась
				if(*pcMaskLoc == ':'){
					if(*pcLoc == '\0'){
						bLocationMatched = true; // то совпадает
					}
					pcLine = pcMaskLoc + 1;
					break;
				}
				// если маска закончилась звездочкой, то оставшаяся часть локации подходит
				if(*pcMaskLoc == '*'){
					for(; *pcMaskLoc != '\0' && *pcMaskLoc != ':'; pcMaskLoc++);
					bLocationMatched = true;
					pcLine = pcMaskLoc + 1;
				}
				break;
			}
			pcMask = pcLine;
		}
		const char* pcName = objName;
		// если начинается на '*'
		if(pcLine[0] == '*' && pcLine[1] != '\0'){
			// значит сравнивать надо хитрее:
			// находим первый совпадающий символ 
			const char* pcLastMatchBegin = objName;
			for(;;){
				const char* pcMatchBegin = NULL;
				const char* pcMask = pcLine + 1;
				for(; *pcName != '\0'; pcName++){
					if(*pcName == *pcMask){
						pcMatchBegin = pcName;
						break;
					}
				}
				if(pcMatchBegin == NULL)
					break;
				// и сравниваем остатки вхождения
				for(; ; pcName++, pcMask++){
					if(*pcName == *pcMask){
						// если закончились и имя и маска
						if(*pcName == '\0'){
							bMatched = true; // то совпадает
							break;
						}
						continue;
					}
					// если маска закончилась звездочкой, то оставшаяся часть имени подходит
					if(*pcMask == '*'){
						bMatched = true;
					}
					break;
				}
				if(bMatched){
					return !bExclude;
				}
				// если вхождение не совпадает, 
				// то снова ищем совпадающий символ
				pcLastMatchBegin = pcMatchBegin;
				pcName = pcMatchBegin + 1;
				if(*pcName == '\0')
					break;
			}
		}else{
			for(; ; pcName++, pcMask++){
				if(*pcName == *pcMask){
					// если закончились и имя и маска
					if(*pcName == '\0'){
						bMatched = true; // то совпадает
						break;
					}
					continue;
				}
				// если маска закончилась звездочкой, то оставшаяся часть имени подходит
				if(*pcMask == '*'){
					bMatched = true;
				}
				/*
				// если кончилось имя, а маска нет, значит не совпадает (маска длиннее имени)
				if(*pcName == '\0')
				break;
				// если кончилась маска, а имя нет, значит не совпадает (имя длиннее маски)
				if(*pcMask == '\0')
				break;
				*/
				break;
			}
		}
		if(bLocationMatched && bMatched){
			return !bExclude;
		}
	}
	// не соответствует ни одной маске
	return bExclude;
}

CVectorZones *CMapManager::GetActiveZones()
{
	return &m_activeZones;
}

bool CMapManager::IsActiveZone(ZoneID zoneID)
{
	for (unsigned int i = 0; i < m_activeZones.size(); i++)
	if (m_activeZones[i]->GetZoneID() == zoneID)
	{
		return true;
	}
	
	return false;
}

void TraceRealities(CRealitiesMap m_realitiesCache, unsigned int realityID)
{
	ATLTRACE("GetRealityInfo\n");

	CRealitiesMap::iterator itReality = m_realitiesCache.find(realityID);
	if (itReality == m_realitiesCache.end())
		return;
	cmIRealityInfo* realityInfo = &(itReality->second);
	ATLTRACE("realityInfo.ID= %d \n",realityInfo->GetRealityID());
	TraceRealities(m_realitiesCache, realityInfo->GetParentRealityID());
}

bool CMapManager::GetRealityInfo(unsigned int realityID, cmIRealityInfo*& realityInfo)
{
	realityInfo = NULL;

	CRealitiesMap::iterator itReality = m_realitiesCache.find(realityID);
	if (itReality == m_realitiesCache.end())
	{
		return false;
	}

	realityInfo = &(itReality->second);

	return true;
}

ZoneID CMapManager::GetCurrentZoneID()
{
	return m_currentZoneID;
}

unsigned long CMapManager::GetCurrentZoneId()
{
	return m_currentZoneID.id;
}

void CMapManager::ActivateZones()
{
	ActivateZones(m_currentZoneID.id);
}

void CMapManager::ActivateZones( ZoneIdentifier aCurrentZone)
{
	if (aCurrentZone == INCORRECT_ZONEID)
		return;

	if (GetCurrentZoneID().id != INCORRECT_ZONEID && aCurrentZone != GetCurrentZoneID().id)
		return;

	ZoneID zoneID( aCurrentZone);
	std::vector<ZoneID> zones;
	GetNeighbourZonesIDs( zoneID, zones);
	m_activeZones.clear();
	for (unsigned int k = 0; k < zones.size(); k++)
	{
		AddZoneIfNotExists( zones[k].id);
		m_activeZones.push_back( GetZone( zones[k].id));
	}
}

void CMapManager::StartRender3D()
{
	m_isRender3DNeeded = true;
}

void CMapManager::StartPhysics()
{
	m_isPhysicsNeeded = true;
}

bool CMapManager::IsRender3DNeeded()const
{
	return m_isRender3DNeeded;
}

bool CMapManager::IsPhysicsNeeded()const
{
	return m_isPhysicsNeeded;
}

void CMapManager::SetIsRender3DNeeded( bool abVal)
{
	 m_isRender3DNeeded = abVal;
}

bool CMapManager::UpdateVoipConnection()
{
	if( !m_context->mpVoipMan)
		return false;
	if( !m_mainObject)
		return false;

	long long longAvatarID = (long long)m_mainObjectInfo.GetObjectID();

	if (!PLOCATION_IS_NULL(m_currentLocation))
	{
		m_context->mpVoipMan->Connect( m_currentLocation->name.c_str()
			, GetCurrentRealityID()
			, m_mainObjectInfo.objectName.c_str()/*sUserName*/
			, "vacadem"/*password*/
			, longAvatarID);
	}
	else
	{
		//! Когда мы вне локаций подключаемся к предопределенной группе
		m_context->mpVoipMan->Connect( ""
			, GetCurrentRealityID()
			, m_mainObjectInfo.objectName.c_str()/*sUserName*/
			, "vacadem"/*password*/
			, longAvatarID);
	}

	return true;
}

void CMapManager::UpdateActiveZones()
{
	if (!m_mainObject)
	{
		return;
	}

	ZoneID zoneID = GetObjectGlobalZone(m_mainObject);

	std::vector<ZoneID> tmpZonesIDs;
	GetNeighbourZonesIDs(zoneID, tmpZonesIDs);

	m_activeZones.clear();
	AddZoneIfNotExists(zoneID);
	m_activeZones.push_back(GetZone(zoneID));

	std::vector<ZoneID>::iterator it = tmpZonesIDs.begin();
	std::vector<ZoneID>::iterator itEnd = tmpZonesIDs.end();

	for ( ; it != itEnd; it++)
	{
		AddZoneIfNotExists(*it);
		m_activeZones.push_back(GetZone(*it));
	}
}

bool CMapManager::IsObjectCreating(unsigned int objectID)
{
	return m_loadingQueueInfo.IsObjectInQueue(objectID);
}

int CMapManager::HandleServerMessage( unsigned char aMessageID, unsigned char* aData, int aDataSize)
{
	MP_NEW_P2(pQueryIn, CCommonPacketIn, aData, aDataSize);

	static CCreateObjectsHandler createObjectHandler;

	static CObjectsHandler objectHandler;

	static CCreateAvatarHandler createAvatarHandler;

	switch( aMessageID)
	{
	case	ZONES_DESC_QUERY_ID:
		{
			g->lw.WriteLn("[RENDERMANAGER-LOG] Zones description was received");
			CZonesHandler::Handle( pQueryIn);
			break;
		}
	case	ZONES_OBJECTS_DESC_QUERY_ID:
		{
			g->lw.WriteLn("[RENDERMANAGER-LOG] Objects description was received");
			objectHandler.Handle( pQueryIn, m_context, m_currentRealityID);
			// сбросим текушие приоритеты загрузки
			UpdateLoadingPriority(NULL);	
			break;
		}

	case	ZONES_STATIC_OBJECTS_QUERY_ID:
		{
			g->lw.WriteLn("[RENDERMANAGER-LOG] Static objects description was received");
			objectHandler.Handle( pQueryIn, m_context, m_currentRealityID, true);
			// сбросим текушие приоритеты загрузки
			UpdateLoadingPriority(NULL);	
			break;
		}

	case	OBJECTS_DESC_QUERY_ID:
		{
			createObjectHandler.Handle( pQueryIn);
			break;
		}

	case	AVATAR_DESC_QUERY_ID:
		{
			g->lw.WriteLn("[RENDERMANAGER-LOG] CreateAvatar description was received");
			createAvatarHandler.Handle( pQueryIn, m_context, &m_createdAvatarID);
			break;
		}

	case	REALITY_DESC_QUERY_ID:
		{
			g->lw.WriteLn("[RENDERMANAGER-LOG] Reality description was received");
			OnRealityDesc( pQueryIn);
			break;
		}

	case	DELETE_OBJECT_QUERY_ID:
		{
			g->lw.WriteLn("[RENDERMANAGER-LOG] DeleteObject description was received");
			OnDeleteObject( pQueryIn);
			break;
		}

	case	OBJECT_STATUS_QUERY_ID:
		{
			CObjectStatusHandler::Handle( pQueryIn, m_callbacks);
			break;
		}

	case	CREATE_LOCATION_QUERY_ID:
		{
			g->lw.WriteLn("[RENDERMANAGER-LOG] Location description was received");
			objectHandler.Handle(pQueryIn, m_context, m_currentRealityID);
			// сбросим текушие приоритеты загрузки
			UpdateLoadingPriority(NULL);
			break;
		}
	}

	return 0;
}

// Обработать обрыв связи с сервером
void CMapManager::OnConnectLost()
{
	// ??
}

// Обработать восстановление связи с сервером
void CMapManager::OnConnectRestored()
{
	//if( m_mainObject != NULL)
	//{
	//	CUrlBuilder urlBuilder;
	//	CVector3D pos = m_mainObject->GetCoords();
	//	urlBuilder.setCoords(pos.x, pos.y, pos.z);
	//	
	//	
	//	urlBuilder.setReality( GetCurrentRealityID());
	//	CRotationPack *pRot = m_mainObject->GetRotation();
	//	if(pRot !=NULL)
	//	{
	//		float ra = 0, rx = 0, ry = 0, rz = 0;
	//		pRot->GetRotating( &rx, &ry, &rz, &ra);
	//		urlBuilder.setRotations(ra, rx, ry, rz);
	//	}

	//	// после восстановления связи аватар запрашивается для создания в текущей реальности
	//	CreateMyAvatar( urlBuilder.toString().c_str());
	//}
	//else
	//{
	//	// при начальном создании аватара передаем ссылку на URL, который передан через командную строку
	//	CreateMyAvatar( m_context->mpSM->GetLocationURL());
	//}
}

// получить описание локации в точке с заданными координатами
bool CMapManager::GetLocationByXYZ(float afX, float afY, float afZ, rmml::ml3DLocation &aLocation)
{
	SLocationInfo* pLocation = GetLocationByXYZ(afX, afY, afZ);
	if(pLocation == NULL){
		aLocation.ID = L"";
		aLocation.params = L"";
		return false;
	}
	aLocation.ID = pLocation->name.c_str();
	aLocation.params = pLocation->params.c_str();
	return true;
}

char* CMapManager::GetLocationNameByXYZ(float afX, float afY, float afZ)
{
	SLocationInfo* pLocation = GetLocationByXYZ(afX, afY, afZ);
	if(pLocation == NULL)
	{
		return NULL;
	}

	return (char *)pLocation->name.c_str();
}

// получить параметры локации в виде строки (это значение из objects_map.params)
bool CMapManager::GetLocationParams( const wchar_t* apwcLocationID, wchar_t* paramsBuffer, unsigned int auParamsBufferSize)
{
	if( !apwcLocationID)
		return false;

	if( !paramsBuffer)
		return false;

	if( auParamsBufferSize == 0)
		return false;

	SLocationInfo* pLocation = GetLocationByID( apwcLocationID);
	if( pLocation == NULL)
		return false;

	if( pLocation->params.size() >= auParamsBufferSize)
		return false;

	/*USES_CONVERSION;
	wchar_t* apwQueriedLocationParams = A2W( pLocation->params.c_str());
	wcscpy( paramsBuffer, apwQueriedLocationParams);*/

	rtl_wcscpy( paramsBuffer, auParamsBufferSize - 1, pLocation->GetParams());

	return true;
}

// Сохраняет в лог текущую очередь загрузки
MapManagerError CMapManager::GetRealityGuardName(unsigned int auRealityID, wchar_t* apwcBuffer, unsigned int auBufferLength)
{
	if( auBufferLength == 0)
		return MapManagerErrors::ERROR_BUFFER_IS_TOO_SMALL;

	unsigned int aGuardRealityFound = false;
	unsigned int aNoGuardRealityFound = false;
	unsigned int aGuardRealityID = INCORRECT_REALITY_ID;
	cmIRealityInfo* apRealityInfo = NULL;

	while ( GetRealityInfo( auRealityID, apRealityInfo))
	{
		if( apRealityInfo->GetGuardFlags() & RealityObjectGuardFlags::WAIT_GUARD_OBJECT)
		{
			aGuardRealityFound = true;
			aGuardRealityID = apRealityInfo->GetRealityID();
		}
		if( apRealityInfo->GetGuardFlags() & RealityObjectGuardFlags::USE_PARENT_FLAGS)
		{
			auRealityID = apRealityInfo->GetParentRealityID();
		}
		else
		{
			aNoGuardRealityFound = true;
			break;
		}
	}

	if( !aGuardRealityFound)
	{
		if( auRealityID == 0 || aNoGuardRealityFound)
		{
			*apwcBuffer = 0;
			return MapManagerErrors::ERROR_NO_ERROR;
		}
		return MapManagerErrors::ERROR_REALITY_NOT_FOUND;
	}

	CWComString guardName;
	guardName.Format(L"realityGuard_%u", aGuardRealityID);

	if( rtl_wcscpy( apwcBuffer, auBufferLength-1, guardName.GetBuffer()) != RTLERROR_NOERROR)
		return MapManagerErrors::ERROR_BUFFER_IS_TOO_SMALL;

	return MapManagerErrors::ERROR_NO_ERROR;
}

// Сохраняет в лог текущую очередь загрузки
void CMapManager::DumpLoadingQueue()
{
	if( !m_context->mpLogWriter)
		return;

	m_context->mpLogWriter->WriteLn("");
	m_context->mpLogWriter->WriteLn("=====================================================");
	m_context->mpLogWriter->WriteLn("=============LOADING QUEUE===========================");
	m_context->mpLogWriter->WriteLn("=====================================================");
	m_context->mpLogWriter->WriteLn("");
	m_context->mpLogWriter->WriteLn("Common zone");
	m_commonZone->DumpLoadingQueue();

	int iZoneIndex = 0;
	CVectorZones::iterator it = m_zones.begin();
	CVectorZones::iterator itEnd = m_zones.end();
	for( ; it != itEnd; it++, iZoneIndex++)
	{
		m_context->mpLogWriter->WriteLn("Zone: ", iZoneIndex);
		(*it)->DumpLoadingQueue();
	}
}

void CMapManager::OnLogout( int iCode)
{
	m_mainObject = NULL;
}

void CMapManager::WriteLog(LPCSTR alpcLog)
{
	if( !m_context)
		return;
	if( !m_context->mpLogWriter)
		return;

	m_context->mpLogWriter->WriteLnLPCSTR( alpcLog);
}

CMapManager::~CMapManager(void)
{
	Clear();

	if (m_server != NULL)
	{
		MP_DELETE(m_server);
	}}
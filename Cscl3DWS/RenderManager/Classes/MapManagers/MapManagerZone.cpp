
#include "StdAfx.h"
#include <Assert.h>
#include "IniFile.h"
#include "UserData.h"
#include "LastScreen.h"
#include "nrm3DObject.h"
#include "NRManager.h"
#include "MapManager.h"
#include "MapManagerZone.h" 
#include "MapManagerServer.h"
#include "ZoneObjectsTypes.h"
#include "PlatformDependent.h"
#include "oms_context.h"

#include "userdata.h"
#include <algorithm>

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

#include "__resource_log.h"

#define MIN_LAST_SCREEN_TIME				10000
#define AVATAR_CREATE_PAUSE					5000
#define MIN_LAST_SCREEN_TIME_DIV			5
#define MIN_LAST_SCREEN_TIME_ATI_MUL		1.5f
#define MAX_GROUND_LOADING_TIME				40000

CMapManagerZone::CMapManagerZone(oms::omsContext* aContext):
	m_firstObjectTime(0),
	m_skipFrameCount(0),
	m_context(aContext),		
	m_isZoneVisible(false),
	m_isTruncateMode(false),
	m_isGroundLoaded(false),
	m_updateObjectsCount(0),
	m_areMostObjectsLoaded(0),
	m_lastAvatarCreateTime(0),
	MP_VECTOR_INIT(m_asynchLoadedObjects),
	MP_VECTOR_INIT(m_nonPermittedZones),
	MP_VECTOR_INIT(m_progressLoadedObjects)
{
	assert(aContext);	
	m_zoneID.id = 0;	

	LoadNonPermittedZonesList();
}

void CMapManagerZone::SetTruncateMode(bool isEnabled)
{
	m_isTruncateMode = isEnabled;
}

bool CMapManagerZone::IsObjectCanBeCreated(ZONE_OBJECT_INFO* info)
{
	if (!m_isTruncateMode)
	{
		return true;
	}

	if ((info->type == vespace::VE_AVATAR) ||
		(info->type == vespace::VE_LOCATIONBOUNDS) ||
		(info->type == vespace::VE_TEMPLOCATIONBOUNDS) /*||
		(info->type == vespace::VE_COMMINICATIONAREABOUNDS)*/)
	{
		return true;
	}	

	if ((info->level == LEVEL_GROUND) || (info->level == LEVEL_SEA))
	{
		return true;
	}
	
	CVector3D coords = ((CMapManager*)m_context->mpMapMan)->GetMainObjectCoords();
	float dx = info->translation.x - coords.x;
	float dy = info->translation.y - coords.y;
	float dist = sqrt(dx * dx + dy * dy);
	int pos = info->resPath.find_last_of(";");
	pos = info->resPath.find_last_of(";", pos - 1);
	pos = info->resPath.find_last_of(";", pos - 1);
	int visibleDist = rtl_atoi(info->resPath.c_str() + pos + 1) * 100 / g->rs.GetInt(NORMAL_LOD_SCALE);
    if (dist > visibleDist + 2000)
	{
		if (info->obj3d)
		{
			g->lw.WriteLn("[OBJECT-WARNING] Object cannot be created but due to distance ", info->objectName);
		}
		return false;
	}

	return true;
}

int CMapManagerZone::GetMinLastScreenTime()
{
	int minLastScreenTime = MIN_LAST_SCREEN_TIME;
	if (g->gi.GetVendorID() == VENDOR_ATI)
	{
		minLastScreenTime = (int)(minLastScreenTime * MIN_LAST_SCREEN_TIME_ATI_MUL);
	}

	if (gRM->mapMan->GetCurrentRealityType() == cm::RT_USER)
	{
		minLastScreenTime /= MIN_LAST_SCREEN_TIME_DIV;
	}

	return minLastScreenTime;
}

void CMapManagerZone::LoadNonPermittedZonesList()
{
	std::wstring iniFN = GetApplicationRootDirectory();
	iniFN += L"\\player.ini";
	CIniFile ini(iniFN);

	std::string server = ini.GetString("paths", "server");
	if (("www.virtuniver.loc" == server) || ("192.168.0.77" == server) || ("192.168.0.78" == server))
	{
		return;
	}

	std::string zonesString = ini.GetString("run", "invisible_zones");
	std::string currentZoneNum = "";
	for (unsigned int k = 0; k < zonesString.size(); k++)
	{
		if (zonesString[k] != '|')
		{
			char tmp[2];
			tmp[0] = zonesString[k];
			tmp[1] = 0;

			currentZoneNum += tmp;
		}
		else
		{
			int zoneID = rtl_atoi(currentZoneNum.c_str());
			m_nonPermittedZones.push_back(zoneID);
			currentZoneNum = "";
		}
	}
}


void	CMapManagerZone::DeleteObject(unsigned int aObjectID)
{
	vecServerZoneObjects::iterator it = m_progressLoadedObjects.begin();
	vecServerZoneObjects::iterator itEnd = m_progressLoadedObjects.end();
	bool isSet = false; 
	for ( ; it != itEnd; it++)
		if ((*it)->complexObjectID.objectID == aObjectID)
		{
			isSet = true;
			break;
		}
	if (isSet)
	{
		g->lw.WriteLn("[OBJECT-WARNING] object was deleted in Zone::DeleteObject");

		MP_DELETE(*it);
		m_progressLoadedObjects.erase(it);
	}

}

void	CMapManagerZone::SetGroundLoadedStatus(bool isGroundLoaded)
{
	m_isGroundLoaded = isGroundLoaded;
}

void CMapManagerZone::Clear()
{
}

void CMapManagerZone::SetZoneID(ZoneID zoneID)
{
	m_zoneID = zoneID;
}

void	CMapManagerZone::SetVisible(bool isVisible)
{
	m_isZoneVisible = isVisible;
}

bool	CMapManagerZone::IsZonePermitted(unsigned int zoneID)
{
	std::vector<unsigned int>::iterator it = m_nonPermittedZones.begin();
	std::vector<unsigned int>::iterator itEnd = m_nonPermittedZones.end();

	for ( ; it != itEnd; it++)
	{
		if (*it == zoneID)
		{
			return false;
		}
	}

	return true;
}

void	CMapManagerZone::UpdateObjectsInfo(std::vector<ZONE_OBJECT_INFO *>& info)
{
	if (0 == m_firstObjectTime)
	{
		m_firstObjectTime = g->ne.time;
	}
	if ((info.size() == 1) && (m_progressLoadedObjects.size() == 0) && (!gRM->mapMan->GetAvatar()) && (!gRM->mapMan->IsInZone()))
	{
		gRM->mapMan->SetMainObjectCoords((*info.begin())->translation.x, (*info.begin())->translation.y, (*info.begin())->translation.z);
	}

	int oldCount = m_progressLoadedObjects.size();

	m_progressLoadedObjects.reserve(info.size());

	ZONE_OBJECT_INFO* objectInfo = NULL;
	vecServerZoneObjects::iterator it = info.begin();
	vecServerZoneObjects::iterator itEnd = info.end();	

	for ( ; it != itEnd; it++)
	{
		objectInfo = (*it);		

		if (!IsZonePermitted(objectInfo->zoneID.id))
		{
			MP_DELETE(objectInfo);
			g->lw.WriteLn("[OBJECT-WARNING] Object was deleted because it's zone is not permitted");
			continue;
		}

		if ((objectInfo->zoneID == m_zoneID.id) || (m_zoneID.id == 0))
		{
			if (objectInfo->createImmediately)
			{
				vecServerZoneObjects::iterator fit = m_progressLoadedObjects.begin();
				for (; fit != m_progressLoadedObjects.end(); fit++)
				{
					if( ((*fit)->level%2 != 1) && ((*fit)->type!= vespace::VE_LOCATIONBOUNDS) &&
						((*fit)->type!= vespace::VE_AVATAR))
						break;
				}
				if( fit == m_progressLoadedObjects.end())
					m_progressLoadedObjects.push_back( objectInfo);
				else
					m_progressLoadedObjects.insert( fit, objectInfo);
			}
			else
			{
				m_progressLoadedObjects.push_back(objectInfo);
			}
		}
		else
		{
			MP_DELETE(objectInfo);
		}
	}

	assert(m_progressLoadedObjects.size() - oldCount == info.size());

	m_updateObjectsCount++;
	if (m_updateObjectsCount > 1)
	{
		ResortObjectsByDistance();
	}
}

void	CMapManagerZone::ResortObjectsByDistance()
{
	std::vector<ZONE_OBJECT_INFO *> newList, emptyList; 
	int t1 = g->tp.GetTickCount();
	gRM->mapManServer->sort_zones_objects(m_progressLoadedObjects, newList, emptyList, false);
	int t2 = g->tp.GetTickCount();
	g->lw.WriteLn("resort finished in ", t2 - t1, " ms");

	assert(emptyList.size() == 0);
	assert(newList.size() == m_progressLoadedObjects.size());	
	m_progressLoadedObjects = newList;
}

void	CMapManagerZone::SetLoadingQueueInfoPtr(CLoadingQueueInfo* ptr)
{
	m_loadingQueueInfo = ptr;
}

void	CMapManagerZone::UpdateProgressLoading()
{
	static bool isObjectsReceived = false;
	static int objectsReceivedTime = 0;

	if (!gRM->mapMan->GetScene3DPtr())
	{
		g->lw.WriteLn("Creating object cycle is not started because of scene is not created yet!");
		return;
	}

	int minLastScreenTime = GetMinLastScreenTime();

	if (isObjectsReceived)
#ifndef _DEBUG
		if (g->ne.time - objectsReceivedTime > minLastScreenTime)
#endif	
	{
		if (2 != m_areMostObjectsLoaded)
		{
			gRM->mapMan->ExecJS("scene.OnMostObjectsLoaded();");
			g->rs.SetInt(MAX_TEXTURE_LOADING_TIME, 10);
			g->rs.SetInt(MAX_CLOTHES_LOADING_FRAME_TIME, 5);
			g->rs.SetInt(MAX_COMPOSITE_TEXTURES_FRAME_TIME, 5);
			g->rs.SetInt(MAX_SAO_LOADING_FRAME_TIME, 10);
			g->rs.SetBool(SKIP_FRAMES_TEXTURE_LOADING, false);
			g->rl->GetAsynchResMan()->SetDownloadMode(DOWNLOAD_MODE_MODERATE_LOADING);
			m_areMostObjectsLoaded = 2;
			gRM->lastScreen->Hide();			
		}
	}

 	if (m_skipFrameCount > 0)
	{
		m_skipFrameCount--;
		return;
	}

	if (m_progressLoadedObjects.size() == 0)	
	{
		return;
	}

	static int objectCount = 0;
	if ((m_progressLoadedObjects.size() > 40) && (!isObjectsReceived))
	{
		isObjectsReceived = true;
		objectsReceivedTime = (int)g->ne.time;
		objectCount = m_progressLoadedObjects.size();
	}

	
#ifdef _DEBUG
	const int divKoef = 8;
#else
	const int divKoef = 8;
#endif

	__int64	startLoadingTime = g->tp.GetTickCount();
	__int64	maxFrameLoadingTime = gRM->lastScreen->IsVisible() ? 150 : 15;	// ms
	if (!gRM->lastScreen->IsVisible())
	{
		g->rs.SetInt(MAX_TEXTURE_LOADING_TIME, 10);
		g->rs.SetBool(SKIP_FRAMES_TEXTURE_LOADING, false);
		g->rl->GetAsynchResMan()->SetDownloadMode(DOWNLOAD_MODE_MODERATE_LOADING);
	}
#ifdef _DEBUG
	maxFrameLoadingTime = 50; // да, будут фризы, но иначе объекты создаются очень долго под отладкой
#endif
	__int64	currentTime = startLoadingTime;

	while (currentTime - startLoadingTime < maxFrameLoadingTime)
	{
		vecServerZoneObjects::iterator it = m_progressLoadedObjects.begin();
		vecServerZoneObjects::iterator itEnd = m_progressLoadedObjects.end();

		if (m_zoneID.id != 0)
		while ((it != itEnd) && ((*it)->zoneID != m_zoneID))
		{
			assert(FALSE);	
			g->lw.WriteLn("[OBJECT-WARNING] object was deleted because of another zone");
			MP_DELETE(*m_progressLoadedObjects.begin()); // alex
			m_progressLoadedObjects.erase(m_progressLoadedObjects.begin());
			if (m_progressLoadedObjects.size() == 0)
			{
				it = itEnd;
			}
			else
			{
				it = m_progressLoadedObjects.begin();
			}
		}

		it = m_progressLoadedObjects.begin();

		if (it == itEnd)
		{
			break;
		}		
        
		if ((*it)->type == vespace::VE_AVATAR)
		{
			m_lastAvatarCreateTime = g->ne.time;
		}
		//
	
		bool isCorrected = false;

		while (!isCorrected) 
		{
			isCorrected = true;

			if ((*it)->className.size() > 0)
			{
				int loadResult = gRM->mapMan->LoadClassFile((*it)->className);

				if (loadResult == CLASS_LOADING)
				{
					it++;	
					if (it == m_progressLoadedObjects.end())
					{						
						return;
					}
					isCorrected = false;
				}
				else if (loadResult == CLASS_NOT_EXISTS)
				{
					(*it)->className = "";
				}				
			}
		}

		//__int64 docurrentTime = g->tp.GetTickCount();
		
		std::string objectName = (*it)->objectName;
		objectName = StringToLower(objectName);

		int objectID = g->o3dm.GetObjectNumber(objectName);
		if (objectID != -1)
		{
			C3DObject* obj = g->o3dm.GetObjectPointer(objectID);

			if ((*it)->level != 5)
			{
				obj->SetVisible(true);
			}
		}
		else
		{
			ZONE_OBJECT_INFO* zinfo = (*it);
			assert( zinfo->GetObjectID() == (*it)->GetObjectID());

#ifdef RESOURCE_LOADING_LOG
			g->lw.WriteLn("AddNewObject ", zinfo->objectName.GetBuffer(0));
#endif
			if (zinfo->type == vespace::VE_AVATAR)
			{
				g->lw.WriteLn("Start avatar create");
			}
	
	
			int t1 = (int)g->tp.GetTickCount();
			//g->lw.WriteLn("AddNewObject ", zinfo->objectName.GetBuffer(0), " ", zinfo->level, " ", g->cm.GetPointDist(zinfo->translation.x, zinfo->translation.y, zinfo->translation.z), " ci = ", zinfo->createImmediately ? "true" : "false");
			if (IsObjectCanBeCreated(zinfo))
			{
				AddNewObject(zinfo);	
			}
			else
			{
				g->lw.WriteLn("[OBJECT-WARNING] Object cannot be created due to IsObjectCanBeCreated result ", zinfo->objectName);
			}
			int t2 = (int)g->tp.GetTickCount();
			if (zinfo->className.size() == 0)
			{
				stat_normalObjectsTime += (t2 - t1);
			}
			else
			{
				if (zinfo->type == vespace::VE_AVATAR)
				{
					stat_avatarObjectsTime += (t2 - t1);
					g->lw.WriteLn("Created avatar in ", t2 - t1, " ms");
				}
				else
				{
					stat_scriptedObjectsTime += (t2 - t1);
				}
			}
		}

		MP_DELETE(*it); // alex
		m_progressLoadedObjects.erase(it);

		if (m_progressLoadedObjects.size() == 0)	
		{
			break;
		}

		currentTime = g->tp.GetTickCount();
	}

	int delta = (int)(currentTime - startLoadingTime);
	if (delta > (int)(maxFrameLoadingTime * 1.5f))
	{
		m_skipFrameCount = (int)((delta - delta%maxFrameLoadingTime) / maxFrameLoadingTime);
	}

	if (isObjectsReceived)
	{
		if (m_progressLoadedObjects.size() < (unsigned int)(objectCount / divKoef))
		{
			if (0 == m_areMostObjectsLoaded)
			{
				m_areMostObjectsLoaded = 1;
			}
		}
		else
		{
			unsigned int objectCountOst = (unsigned int)(objectCount / divKoef);
			unsigned int objectCountNotCreatedYet = m_progressLoadedObjects.size() - objectCountOst;
			unsigned int objectCountForCreating = objectCount - objectCountOst;
			int progressValue = 100 - (int)(100.0f * (float)objectCountNotCreatedYet / (float)objectCountForCreating);
			if (progressValue > 100)
			{
				progressValue = 100;
			}
			if (progressValue < 0)
			{
				progressValue = 0;
			}

#ifndef _DEBUG
			int timeProgressValue = (int)(g->ne.time - objectsReceivedTime) * 100 / minLastScreenTime;			
			if (progressValue < timeProgressValue)
			{				
				progressValue = timeProgressValue;
			}
#endif	

			std::string js = "scene.SetRMProgress(";
			js += IntToStr(progressValue);
			js += ");";

			gRM->mapMan->ExecJS(js.c_str());
		}
	}
}

ZONE_OBJECT_INFO*	CMapManagerZone::GetObjectInfo(const ZONE_OBJECT_ID& objectID)
{
	return NULL;
}

void	CMapManagerZone::NotifyMlMediaOnZoneChanged(ZONE_OBJECT_INFO* info)
{
	if (info->obj3d != NULL)
	{
		nrmObject* nrmObj = (nrmObject*)info->obj3d->GetUserData(USER_DATA_NRMOBJECT);
		if (nrmObj)
		{
			mlMedia* pmlMedia = nrmObj->GetMLMedia();
			if (pmlMedia != NULL)
			{
				pmlMedia->ZoneChanged(info->zoneID.id);
			}
		}
	}
}

void	CMapManagerZone::AddNewObject(ZONE_OBJECT_INFO* info)
{
	bool isTempLocation = false;	
//	ILogWriter* lw = gRM->mapMan->GetILogWriter();
	if ((info->type == vespace::VE_LOCATIONBOUNDS) || (info->type == vespace::VE_TEMPLOCATIONBOUNDS))
	{
		/*
		перенес на этап сортировки объектов при разборе пакета - Nap
		gRM->mapMan->AddLocationBounds(info->objectName, info->translation.x, info->translation.y, info->translation.z,
			info->boundsVec.x, info->boundsVec.y, info->boundsVec.z, 
			info->rotation.x, info->rotation.y, info->rotation.z, info->rotation.w, info->GetParams(), info->complexObjectID.bornRealityID, info->type, info->complexObjectID.objectID);*/
		if (info->type == vespace::VE_LOCATIONBOUNDS && (info->className.size() == 0))
		{
/* // если раскомментарить и закомментарить последующий return, то станут видны границы локации
// временно для отладки
info->type = vespace::VE_OBJECT3D;
info->className = "CUserLocationArea";
char buf[2000];
sprintf(buf, "{'points':[{'x':-%d,'y':-%d},{'x':-%d,'y':%d},{'x':%d,'y':%d},{'x':%d,'y':-%d}]}", (int)info->boundsVec.x, (int)info->boundsVec.y, (int)info->boundsVec.x, (int)info->boundsVec.y, (int)info->boundsVec.x, (int)info->boundsVec.y, (int)info->boundsVec.x, (int)info->boundsVec.y);
info->SetParams(buf);
isTempLocation = true;
*/
			return;
		}
		else
		{
			info->type = vespace::VE_OBJECT3D;
			isTempLocation = true;
		}
	}	

	MP_NEW_P(localinfo, ZONE_OBJECT_INFO, *info);
	
	localinfo->objectName = StringToLower(localinfo->objectName);

	if (!gRM->mapMan->AllowCreateByNapFilter(localinfo->objectName.c_str(), 
		localinfo->className, localinfo->level, localinfo->type, localinfo->translation))
	{
		//g->lw.WriteLn("[OBJECT-WARNING] AllowCreateByNapFilter return false for ", localinfo->objectName);
		MP_DELETE(localinfo);
		return;
	}

	if ((localinfo->type != vespace::VE_AVATAR) &&
		(!gRM->mapMan->AllowCreateByFilter(localinfo->objectName.c_str(), localinfo->translation)))
	{
		//g->lw.WriteLn("[OBJECT-WARNING] AllowCreateByFilter return false for ", localinfo->objectName);
		MP_DELETE(localinfo);
		return;
	}

	if (info->type == vespace::VE_TREE)
	{
		MP_DELETE(localinfo);
		return;
	}

	
	SCRIPTED_OBJECT_DESC desc = gRM->mapMan->CreateScriptedObject(localinfo);

	if (info->groupID != 0xFFFFFFFF) //если есть группа, то связать объект с группой
	{
		GroupInfo* groupInfo = gRM->groupCtrl->FindGroupInList(info->groupID);

		if (groupInfo != NULL)
		if (groupInfo->groupType != vespace::VE_GROUP_LECTORE_TYPE)
		{
			gRM->groupCtrl->OnGroup(std::string(info->objectName), groupInfo->groupName, groupInfo->groupType); 
		}
		else if ((groupInfo->groupType == vespace::VE_GROUP_LECTORE_TYPE) && (desc.obj3d))
		{			
			desc.obj3d->SetUserData(USER_DATA_OBJECT_IN_LECTURE_GROUP, desc.obj3d);
		}
	}	
	if ( (info->type == vespace::VE_GROUP_LECTORE_TYPE) || (info->type ==vespace::VE_GROUP_PIVOT_TYPE) )//если группа, то создать 
	{
			GroupInfo* groupInfo = MP_NEW(GroupInfo);
			groupInfo->groupName = info->objectName;
			groupInfo->groupType = info->type;
			gRM->groupCtrl->AddGroupToList(info->complexObjectID.objectID, groupInfo);
			gRM->groupCtrl->OnChangeGroup(groupInfo->groupName, CVector3D(info->translation.x,info->translation.y,info->translation.z) );		
	}

	if( desc.obj3d)
	{
			desc.obj3d->SetNeedSwitchOffPathes(info->isNeedToSwitchOffPathes);
			if (info->isNeedToSwitchOffPathes)
			{
				g->pf.Delete3DObjectsPath(desc.obj3d->GetID());
			}
	}

	localinfo->obj3d = desc.obj3d;

	if(localinfo->obj3d)
	{
		if (isTempLocation)
		{
			localinfo->obj3d->SetUserData(USER_DATA_IS_LOCATION, (void*)true);
		}
	}
	NotifyMlMediaOnZoneChanged(localinfo);
	m_asynchLoadedObjects.push_back(localinfo);
}

void	CMapManagerZone::Update(DWORD /*time*/, bool isUpdate)
{
	if (isUpdate)
	{
		UpdateProgressLoading();
	}
}

void	CMapManagerZone::DeleteNonZoneObjects(CRect2D& zonesRect)
{
	vecServerZoneObjects::iterator it = m_progressLoadedObjects.begin();
	vecServerZoneObjects::iterator itEnd = m_progressLoadedObjects.end();

	std::vector<ZONE_OBJECT_INFO *> savedObjects;
	savedObjects.reserve(m_progressLoadedObjects.size());	

	for ( ; it != itEnd; it++)
	if ((*it)->type != vespace::VE_AVATAR)
	{
		CRect2D rct((*it)->translation.x - (*it)->boundsVec.x, (*it)->translation.y - (*it)->boundsVec.y,
			(*it)->translation.x + (*it)->boundsVec.x, (*it)->translation.y + (*it)->boundsVec.y);
		
		if (rct.IsIntersected(zonesRect))
		{
			savedObjects.push_back(*it);
		}
		else
		{	
			if ((*it)->className.size() > 0)
			{
				m_loadingQueueInfo->DeleteObject((*it)->complexObjectID.objectID);
			}

			g->lw.WriteLn("[OBJECT-WARNING] remove object from queue (DeleteNonZoneObjects) ", (*it)->objectName);

			MP_DELETE(*it);
		}
	}
	else
	{
		savedObjects.push_back(*it);
	}

	if (savedObjects.size() != m_progressLoadedObjects.size())
	{
		m_progressLoadedObjects.clear();
		m_progressLoadedObjects.reserve(savedObjects.size());

		vecServerZoneObjects::iterator it = savedObjects.begin();
		vecServerZoneObjects::iterator itEnd = savedObjects.end();

		for ( ; it != itEnd; it++)
		{
			m_progressLoadedObjects.push_back(*it);
		}
	}
}

void	CMapManagerZone::UpdateLoadingPriority(CCamera3D* camera)
{
	if (!camera)
	{
		return;
	}

	std::vector<ZONE_OBJECT_INFO *>::iterator it = m_asynchLoadedObjects.begin();
	for( ; it != m_asynchLoadedObjects.end(); it++)
	{
		if ((*it)->level == 1)
		{
			m_isGroundLoaded = true;
			gRM->mapMan->OnGroundLoaded(*it);
		}

		MP_DELETE(*it);
	}

	m_asynchLoadedObjects.clear();
}

bool	CMapManagerZone::IsBoundsContains(const CMapObjectVector& point)
{
	float centerX, centerY;
	m_zoneID.get_zone_center_from_xy(m_zoneID, centerX, centerY);

	if ((point.x >= centerX - ZONE_SIZE / 2.0f) && (point.x <= centerX + ZONE_SIZE / 2.0f) &&
		(point.y >= centerY - ZONE_SIZE / 2.0f) && (point.y <= centerY + ZONE_SIZE / 2.0f))
	{
		return true;
	}

	return false;
}

bool	CMapManagerZone::IsObjectIn(const CMapObjectVector& point, const CMapObjectVector& radius)
{
	if (IsBoundsContains(point))
	{
		return true;
	}

	float x[4], y[4];
	float centerX, centerY;

	m_zoneID.get_zone_center_from_xy(m_zoneID, centerX, centerY);
	x[1] = centerX - ZONE_SIZE / 2.0f;
	y[1] = centerY - ZONE_SIZE / 2.0f;
	x[2] = centerX + ZONE_SIZE / 2.0f;
	y[2] = centerY - ZONE_SIZE / 2.0f;
	x[3] = centerX + ZONE_SIZE / 2.0f;
	y[3] = centerY + ZONE_SIZE / 2.0f;
	x[0] = centerX - ZONE_SIZE / 2.0f;
	y[0] = centerY + ZONE_SIZE / 2.0f;

	for (int i = 0; i < 4; i++)
	if ((x[i] >= point.x - radius.x) && (x[i] <= point.x + radius.x) &&
		(y[i] >= point.y - radius.y) && (y[i] <= point.y + radius.y))
	{
		return true;
	}


	return false;
}

int	CMapManagerZone::GetGroundLoadedStatus()
{
	if ((m_firstObjectTime != 0) && (g->ne.time - m_firstObjectTime > MAX_GROUND_LOADING_TIME))
	{
		return GROUND_LOADED_BY_TIMEOUT;	
	}
	return m_isGroundLoaded ? GROUND_OBJECT_LOADED : GROUND_NOT_LOADED;
}

void	CMapManagerZone::DeleteAllObjects()
{
	// удалить все объекты, кроме 3D объектов
	std::vector<ZONE_OBJECT_INFO *>::iterator it = m_asynchLoadedObjects.begin();
	for( ; it != m_asynchLoadedObjects.end(); it++)
	{
		MP_DELETE(*it);
	}

	m_asynchLoadedObjects.clear();
}

// Сохраняет в лог текущую очередь загрузки
void	CMapManagerZone::DumpLoadingQueue()
{
	ATLASSERT( m_context->mpLogWriter);
	std::vector<ZONE_OBJECT_INFO *>::iterator it = m_asynchLoadedObjects.begin()
												, end = m_asynchLoadedObjects.end();

	for( ; it != m_asynchLoadedObjects.end(); it++)
	{	
		ZONE_OBJECT_INFO*	objectInfo = (ZONE_OBJECT_INFO*)(*it);
		m_context->mpLogWriter->WriteLn("ID:", objectInfo->complexObjectID.objectID,
										", realityID:", objectInfo->complexObjectID.realityID,
										", bornRealityID:", objectInfo->complexObjectID.bornRealityID,
										", src:", objectInfo->resPath);
	}
}

CMapManagerZone::~CMapManagerZone(){	Clear();}
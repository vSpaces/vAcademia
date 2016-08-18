#include "StdAfx.h"
#include "MapServerDataBlock.h"
#include "MapManagerServer.h"
#include <qpsort.h>
#include <algorithm>

#include "GetZonesStaticObjectsQueryOut.h"
#include "GetZonesDescQueryOut.h"
#include "GetZonesDescQueryIn.h"
#include "GetZonesObjectsDescQueryOut.h"
#include "GetZonesObjectsDescQueryIn.h"
#include "GetObjectsDescQueryOut.h"
#include "ObjectsDescUpdateOut.h"
#include "RealityDescQueryOut.h"

#include "ObjectsDescIn.h"
#include "DeleteObjectsByIDQueryOut.h"
#include "DeleteObjectsByNameQueryOut.h"
#include "AvatarDescQueryOut.h"
#include "LocationQueryOut.h"
#include "LocationDeleteQueryOut.h"
#include "NatureSettingsQueryOut.h"
#include "NatureSettingsIn.h"
#include "SyncMan.h"

#include "ZLIBENGN.H"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

#include <userdata.h>

using namespace MapManagerProtocol;

#define DIST_SPLIT_COUNT	10
#define DIST_SPLIT_RANGE	2000
#define OBJECT_COUNT		100

int GetLodDist(std::string& res)
{
	if (res.size() == 0)
	{
		return 0;
	}

	std::string _res = "";

	int pos = res.size() - 1;
	while ((res[pos] != ';') && (pos > 0))
	{
		pos--;
	}

	pos--;

	if (pos <= 0)
	{
		return 0;
	}

	while ((res[pos] != ';') && (pos > 0))
	{
		pos--;
	}

	pos--;

	if (pos <= 0)
	{
		return 0;
	}

	while ((res[pos] != ';') && (pos > 0))
	{
		_res = res[pos] + _res;
		pos--;
	}

	return rtl_atoi(_res.c_str());
}

CMapManagerServer::CMapManagerServer(oms::omsContext* context) :
	packageID(0),
	pComManager(NULL),
	m_context(context),
	MP_VECTOR_INIT(m_getObjectsEndTasks)
{
	InitializeCriticalSection(&m_cs);
	gRM->SetMapManagerServer(this);
	bOldServer = false;
	wchar_t pwcValue[1024] = L"";
	m_context->mpApp->GetSetting( L"old_server", pwcValue, sizeof(pwcValue)/sizeof(pwcValue[0]), L"compatability");
	bOldServer = _wtoi( pwcValue);
}

template<class T>
void memparse_object(byte* buf, int& pos, T& val, int allSize)
{
	if ((unsigned int)(pos + sizeof(val)) > (unsigned int)allSize)
	{
		ATLTRACE("ERROR!!! MESSAGE FROM MAPSERVER IN ERROR FORMAT!!!");
		assert(FALSE);
		return;
	}

	memcpy( &val, &buf[pos], sizeof(val));	pos += sizeof(val);
}

void memparse_string(byte* buf, int& pos, CComString& val, int allSize)
{
	unsigned int strl;
	memparse_object<unsigned int>( buf, pos, strl, allSize);

	if ((unsigned int)(pos + strl) > (unsigned int)allSize)
	{
		ATLTRACE("ERROR!!! MESSAGE FROM MAPSERVER IN ERROR FORMAT!!!");
		assert(FALSE);
		return;
	}

	memcpy( val.GetBufferSetLength(strl), &buf[pos], strl);
	pos += strl;
}

void memparse_wstring(byte* buf, int& pos, std::wstring& val, int allSize)
{
	unsigned int strl;
	memparse_object<unsigned int>( buf, pos, strl, allSize);

	if ((unsigned int)(pos + strl) > (unsigned int)allSize)
	{
		ATLTRACE("ERROR!!! MESSAGE FROM MAPSERVER IN ERROR FORMAT!!!");
		assert(FALSE);
		return;
	}
	val.reserve(strl / sizeof(wchar_t) + 1);
	memcpy( (void*)val.c_str(), &buf[pos], strl);
	((wchar_t*)val.c_str())[strl / sizeof(wchar_t)] = L'\0';
	pos += strl;
}

void CMapManagerServer::get_zones_objects_start( unsigned int aPrevRealityID, unsigned int aRealityID,
					   ZoneID aCurrentZone, const std::vector<ZoneID>& aFullQueryZones)
{
	{
		CGetZonesStaticObjectsQueryOut queryOut(aPrevRealityID, aRealityID, aCurrentZone);
		m_context->mpSyncMan->GetServerSession()->SendQuery( ZONES_STATIC_OBJECTS_QUERY_ID, queryOut.GetData(), queryOut.GetDataSize());
	}

	for (int i = 0; i < aFullQueryZones.size(); i++)
	if (aCurrentZone.id != aFullQueryZones[i].id)
	{
		CGetZonesStaticObjectsQueryOut queryOut(aPrevRealityID, aRealityID, aFullQueryZones[i]);
		m_context->mpSyncMan->GetServerSession()->SendQuery( ZONES_STATIC_OBJECTS_QUERY_ID, queryOut.GetData(), queryOut.GetDataSize());
	}

	CGetZonesObjectsDescQueryOut queryOut(aPrevRealityID, aRealityID, aCurrentZone, aFullQueryZones);
	m_context->mpSyncMan->GetServerSession()->SendQuery( ZONES_OBJECTS_DESC_QUERY_ID, queryOut.GetData(), queryOut.GetDataSize());
}

void CMapManagerServer::Unpack(CCommonPacketIn* apQueryIn, char** unpackBuffer, unsigned long* unpackedSize)
{
	BYTE* pBuffer = (BYTE*)apQueryIn->GetData();
	unsigned long messageSize = apQueryIn->GetDataSize();
		
	*unpackedSize = (unsigned long)(*(unsigned int*)(pBuffer));
	*unpackBuffer = MP_NEW_ARR(char, *unpackedSize);

	messageSize -= 4;	// убрали размер данных

	ZlibEngine zip;
	int decompResult = zip.decompmem((char*)(pBuffer + 4), &messageSize, *unpackBuffer, unpackedSize);
}

void CMapManagerServer::FreeUnpackBuffer(char* unpackBuffer)
{
	if (unpackBuffer)
	{
		MP_DELETE_ARR(unpackBuffer);
	}
}

bool CMapManagerServer::getObjectsEnd(bool isStaticObjectsOnly, CCommonPacketIn* pMapDataAsynchObject,
									  unsigned int aRealityID, std::vector<ZONE_OBJECT_INFO *>& zonesInfo, bool abFilterPacketsByReality, IObjectReceivedHandler* objectHandler)
{
	char* unpackBuffer = NULL;
	unsigned long unpackedSize = 0;

	Unpack(pMapDataAsynchObject, &unpackBuffer, &unpackedSize);
	if (0 == unpackedSize)
		return false;

	CObjectsDescIn objectsPacket((BYTE*)unpackBuffer, unpackedSize, bOldServer);

	// создаем асинхронный таск на разбор больших списков объектов с сервера объектов
	CGetObjectsEndTask* task = MP_NEW(CGetObjectsEndTask);

	// устанавливаем параметры таска
	task->SetParams(unpackBuffer, unpackedSize, aRealityID, zonesInfo, abFilterPacketsByReality, objectHandler, isStaticObjectsOnly, bOldServer);

	unsigned short objectCount = objectsPacket.GetObjectCountByPos();

	if (objectCount > OBJECT_COUNT )
	{
		//добавляем таск в очередь на выполнение
		m_getObjectsEndTasks.push_back(task);

		m_context->mpSyncMan->SetStateToAnalysisObjectsList(true);
		g->taskm.AddTask(task, MAY_BE_MULTITHREADED, PRIORITY_HIGH);
	}
	else
	{
		g->taskm.AddTask(task, MUST_RUN_IN_MAIN_THREAD, PRIORITY_HIGH);		

		get_actual_zones_info(task, zonesInfo, isStaticObjectsOnly);

		std::vector<ZONE_OBJECT_INFO *> mapZonesInfo;
		std::vector<ZONE_OBJECT_INFO *> syncZonesInfo;
		sort_zones_objects( zonesInfo, mapZonesInfo, syncZonesInfo);

		CObjectsDescIn* objectsPacket = task->GetObjectsPacket();


		// отправляем хэндлеру спискок обработанных объектов
		if (objectHandler)
		{
			objectHandler->ReceivedHandle(objectsPacket->GetCurrentRealityID(), mapZonesInfo, syncZonesInfo);
		}
		else
		{
			assert(false);
		}

		MP_DELETE(task);
	}

	return true;
}

void CMapManagerServer::get_actual_zones_info(CGetObjectsEndTask* task, std::vector<ZONE_OBJECT_INFO *>& _zonesInfo, bool isStaticObjectsOnly)
{
	CObjectsDescIn* objectsPacket = task->GetObjectsPacket();
	gRM->mapMan->ActivateZones(objectsPacket->GetCurrentZoneID());

	_zonesInfo = task->GetZonesInfo(); 

	if (!isStaticObjectsOnly)
	{
		gRM->mapMan->OnObjectsRecieved(objectsPacket->GetPrevRealityID(), objectsPacket->GetCurrentRealityID(), _zonesInfo);
	}
}

void CMapManagerServer::update()
{
	if (m_getObjectsEndTasks.size() > 0)	
	{
		std::vector<CGetObjectsEndTask*>::iterator it = m_getObjectsEndTasks.begin();
		for (; it != m_getObjectsEndTasks.end(); it++)
		{
			CGetObjectsEndTask* task = *it;
			if (task->IsPerformed())				
			{
				std::vector<ZONE_OBJECT_INFO *> _zonesInfo;				

				get_actual_zones_info(task, _zonesInfo, task->IsStaticObjectsOnly());

				std::vector<ZONE_OBJECT_INFO *> zonesInfo;
				std::vector<ZONE_OBJECT_INFO *> syncZonesInfo;

				sort_zones_objects(_zonesInfo, zonesInfo, syncZonesInfo);				

				IObjectReceivedHandler* objectHandler = task->GetReceivedHandler();

				CObjectsDescIn* objectsPacket = task->GetObjectsPacket();

				// отправляем хэндлеру спискок обработанных объектов
				if (objectHandler)
				{
					objectHandler->ReceivedHandle(objectsPacket->GetCurrentRealityID(), zonesInfo, syncZonesInfo);
				}
				else
				{
					assert(false);
				}

				MP_DELETE(*it);
				m_getObjectsEndTasks.erase(it);

				if (m_getObjectsEndTasks.size() == 0)
				{
					m_context->mpSyncMan->SetStateToAnalysisObjectsList(false);
				}

				break;
			}
		}
	}
}

void CMapManagerServer::get_zones_objects_end(bool isStaticObjectsOnly, CCommonPacketIn* pMapDataAsynchObject, unsigned int aRealityID,
											  std::vector<ZONE_OBJECT_INFO *>& zonesInfo, std::vector<ZONE_OBJECT_INFO *>& syncZonesInfo, bool abFilterPacketsByReality, IObjectReceivedHandler* objectHandler)
{
	std::vector<ZONE_OBJECT_INFO *> _zonesInfo;	

	if (!getObjectsEnd(isStaticObjectsOnly, pMapDataAsynchObject, aRealityID, _zonesInfo, abFilterPacketsByReality, objectHandler))
		return;

	/*if (zonesInfo.size() > 0)
	{
		g->lw.WriteLn("sorted objects: ====");
		it = zonesInfo.begin();
		itEnd = zonesInfo.end();
		for ( ; it != itEnd; it++)
		{
			ZONE_OBJECT_INFO* info = *it;
			g->lw.WriteLn(info->objectName.GetBuffer(0), " ", info->level, " ", info->dist, " ", info->lodDist, " ", info->createImmediately ? "true" : "false");	
		}

		g->lw.WriteLn("end sorted objects ====");
	}*/
}

void CMapManagerServer::sort_zones_objects(std::vector<ZONE_OBJECT_INFO *> _zonesInfo,  std::vector<ZONE_OBJECT_INFO *>& zonesInfo, std::vector<ZONE_OBJECT_INFO *>& syncZonesInfo, bool isFirstTimeSort)
{

	CVector3D mainCoords = gRM->mapMan->GetMainObjectCoords();

	if (isFirstTimeSort)
	{
		g->lw.WriteLn("get_zones_objects_end: ", mainCoords.x, " ", mainCoords.y, " ", mainCoords.z, " count = ", _zonesInfo.size());
	}
	else
	{
		g->lw.WriteLn("resort objects: ", mainCoords.x, " ", mainCoords.y, " ", mainCoords.z, " count = ", _zonesInfo.size());
	}

	/////////////////////////////////////////////////////////////////////////////
	// пересчет из локальных координат в глобальные
	/////////////////////////////////////////////////////////////////////////////

	float eyeX = mainCoords.x;
	float eyeY = mainCoords.y;

	bool isLocationsAdded = false;

	vecServerZoneObjects::iterator it = _zonesInfo.begin();
	vecServerZoneObjects::iterator itEnd = _zonesInfo.end();
	if (isFirstTimeSort)
	for (;  it != itEnd;  it++)
	{
		ZONE_OBJECT_INFO* info = *it;

#ifdef RESOURCE_LOADING_LOG
		g->lw.WriteLn("receive from server desc: ", info->objectName.GetBuffer(0));
#endif

		if (info->zoneID.id == 0xFFFFFFFF)
			continue;

		int centerX, centerY;

		// объект находится в глобальной зоне
		info->zoneID.get_zoneuv_from_zone_id(info->zoneID, centerX, centerY);
		CMapObjectVector translation = info->translation;
		translation.x += centerX * ZONE_SIZE + ZONE_SIZE / 2.0f;
		translation.y += centerY * ZONE_SIZE + ZONE_SIZE / 2.0f;

		if ((info->level == LEVEL_GROUND) || (info->level == LEVEL_SEA))
			if (((mainCoords.x >= translation.x - info->boundsVec.x) && (mainCoords.x <= translation.x + info->boundsVec.x) &&
				(mainCoords.y >= translation.y - info->boundsVec.y) && (mainCoords.y <= translation.y + info->boundsVec.y))
				)
			{
				info->createImmediately = true;
			}

		info->translation = translation;
		info->dist  = ((unsigned int)(*it)->translation.x - (unsigned int)eyeX) * ((unsigned int)(*it)->translation.x - (unsigned int)eyeX);
		info->dist += ((unsigned int)(*it)->translation.y - (unsigned int)eyeY) * ((unsigned int)(*it)->translation.y - (unsigned int)eyeY);
		info->dist = (int)sqrtf(info->dist);

		if ((info->type == vespace::VE_LOCATIONBOUNDS) || (info->type == vespace::VE_TEMPLOCATIONBOUNDS))
		{
			info->createImmediately = true;
			gRM->mapMan->AddLocationBounds( info);
			/*gRM->mapMan->AddLocationBounds(info->objectName, info->translation.x, info->translation.y,
				info->translation.z, info->boundsVec.x, info->boundsVec.y, info->boundsVec.z,
				info->rotation.x, info->rotation.y, info->rotation.z, info->rotation.w,
				info->GetParamsString(), info->complexObjectID.bornRealityID, info->type, info->complexObjectID.objectID);*/
			isLocationsAdded = true;
		}
		else if (info->type == vespace::VE_COMMINICATIONAREABOUNDS)
		{
			info->createImmediately = true;
			gRM->mapMan->AddCommunicationAreaScales( info);
		}
		if (info->className.size() > 0)
		{
			info->dist *= 3;
		}

		info->lodDist = GetLodDist(info->resPath);
	}

	zonesInfo.reserve(_zonesInfo.size());
	syncZonesInfo.reserve(_zonesInfo.size());
	int locationCount = 0;

	it = _zonesInfo.begin();
	for (;  it != itEnd;  it++)
	{
		ZONE_OBJECT_INFO* info = *it;
		if (info)
			if (info->IsSynchronized())
			{
				syncZonesInfo.push_back(info);
				*it = NULL;

				//для синхронизируемых объектов-неаватаров  определяем локацию 
				if (info->type != vespace::VE_AVATAR)
				{
					SLocationInfo* locinfo = gRM->mapMan->GetLocationByXYZ(info->translation.x, info->translation.y,
						info->translation.z);
					if (locinfo)
					{
						info->locationName = locinfo->name.c_str();
					}
				}
			
			}
			else if ((*it)->createImmediately == true)
			{
				if ((*it)->type != vespace::VE_LOCATIONBOUNDS)
				{
					zonesInfo.push_back(*it);					
				}		
				else
				{
					locationCount++;
					MP_DELETE(*it);
				}
				*it = NULL;
			}
			else if ( (info->type == vespace::VE_GROUP_LECTORE_TYPE) || (info->type == vespace::VE_GROUP_PIVOT_TYPE) )//если группа, то создать 
			{
				zonesInfo.push_back(info);
				*it = NULL;
			}
	}

	int mainObjectLocationID = -1;
	SLocationInfo* locInfo = gRM->mapMan->GetLocationByXYZ(mainCoords.x, mainCoords.y, mainCoords.z);
	if (locInfo)
	{
		mainObjectLocationID = locInfo->ID;
	}

	if (mainObjectLocationID != -1)
		for (it = _zonesInfo.begin();  it != itEnd; it++)
			if (*it)
			{
				SLocationInfo* locInfo = gRM->mapMan->GetLocationByXYZ((*it)->translation.x, (*it)->translation.y, 
					(*it)->translation.z);
				if ((locInfo) && (locInfo->ID == mainObjectLocationID))
				{
					zonesInfo.push_back(*it);
					*it = NULL;
				}		
			}

			{
				for (int k = 0; k < DIST_SPLIT_COUNT; k++)
				{
					unsigned int startDist = k * DIST_SPLIT_RANGE;
					//startDist *= startDist;
					unsigned int endDist =  (k + 1) * DIST_SPLIT_RANGE;
					//endDist *= endDist;
					endDist = (k == DIST_SPLIT_COUNT - 1) ? 0xFFFFFFFF : endDist;

					it = _zonesInfo.begin();
					for ( ; it != itEnd; it++)
					{
						ZONE_OBJECT_INFO* info = *it;
						if (info == NULL)
							continue;
						if ((info->dist >= startDist) && (info->dist < endDist) && (info->dist <= info->lodDist))
						{
							zonesInfo.push_back(*it);
							*it = NULL;
						}
					}
				}

				it = _zonesInfo.begin();
				for ( ; it != itEnd; it++)
				{
					ZONE_OBJECT_INFO* info = *it;
					if (info == NULL)
						continue;
					zonesInfo.push_back(info);
					*it = NULL;
				}
			}

			/*for (int i = 0; i < _zonesInfo.size(); i++)
			{
				bool isFound = false;

				if (_zonesInfo[i] == NULL)
				{
					continue;
				}

				for (int k = 0; k < zonesInfo.size(); k++)
				if (zonesInfo[k] == _zonesInfo[i])
				{
					isFound = true;
				}

				for (int j = 0; j < syncZonesInfo.size(); j++)
				if (syncZonesInfo[j] == _zonesInfo[i])
				{
					isFound = true;
				}

				if (!isFound)
				{
					int ii = 0;
				}
			}*/

			assert(zonesInfo.size() + syncZonesInfo.size() + locationCount == _zonesInfo.size());

			if (isLocationsAdded)
			{
				m_context->mpSyncMan->OnLocationsListChanged();
			}
}

void CMapManagerServer::create_location(unsigned int auiPointCount, CVector3D points[], const wchar_t* apwcLocationName)
{
	CLocationQueryOut queryOut(auiPointCount, points, apwcLocationName);
	m_context->mpSyncMan->GetServerSession()->SendQuery(CREATE_LOCATION_QUERY_ID, queryOut.GetData(), queryOut.GetDataSize());

}

void CMapManagerServer::delete_location()
{
//	ATLASSERT(false, "Why DELETE_OBJECT_QUERY_ID is used for delete_location");
	CMapServerPacketOutBase queryOut;
//	ATLASSERT( false, "ОТЛАДИТЬ");
	m_context->mpSyncMan->GetServerSession()->SendQuery(DELETE_LOCATION_QUERY_ID, queryOut.GetData(), queryOut.GetDataSize());
}

void	CMapManagerServer::get_avatar_start(const wchar_t* apwcURL)
{
	CAvatarDescQueryOut queryOut( apwcURL);
	m_context->mpSyncMan->GetServerSession()->SendQuery(AVATAR_DESC_QUERY_ID, queryOut.GetData(), queryOut.GetDataSize());
}

void	CMapManagerServer::get_objects_start(cm::cmObjectIDs* anObjects, unsigned int anObjectsCount, unsigned int aRealityID)
{
	if (anObjects == NULL || anObjectsCount == 0)
		return;

	CGetObjectsDescQueryOut queryOut(aRealityID, anObjectsCount, anObjects);
	m_context->mpSyncMan->GetServerSession()->Send(OBJECTS_DESC_QUERY_ID, queryOut.GetData(), queryOut.GetDataSize());
}

// Создать объект
void CMapManagerServer::create_object(SObjectInfoForUpdate& aObjectInfo, unsigned int auRmmlQueryID)
{
	CObjectsDescUpdateOut queryOut(1/*objCount*/
		, auRmmlQueryID
		, aObjectInfo.name, aObjectInfo.res, 
		aObjectInfo.x, aObjectInfo.y, aObjectInfo.z, 
		aObjectInfo.bx, aObjectInfo.by, aObjectInfo.bz, 
		aObjectInfo.rx, aObjectInfo.ry, aObjectInfo.rz, aObjectInfo.rw,
		aObjectInfo.scalex, aObjectInfo.scaley, aObjectInfo.scalez, 
		aObjectInfo.zoneID, aObjectInfo.className, aObjectInfo.userProps, aObjectInfo.type, 
		aObjectInfo.objectID, aObjectInfo.bornRealityID, aObjectInfo.level, aObjectInfo.location);
	m_context->mpSyncMan->GetServerSession()->Send( OBJECT_CREATE_QUERY_ID, queryOut.GetData(), queryOut.GetDataSize());
}

// Послать запрос на получение параметров реальности
/*void CMapManagerServer::query_reality_info(unsigned int realityID)
{
	CRealityDescQueryOut packetOut(realityID);
	m_context->mpSyncMan->GetServerSession()->Send(REALITY_DESC_QUERY_ID, packetOut.GetData(), packetOut.GetDataSize());
}*/

void CMapManagerServer::set_reality( unsigned int aReality)
{
	ATLASSERT( false, "ОТЛАДИТЬ");	// на серваке не обрабатывается
	m_context->mpSyncMan->GetServerSession()->Send(REALITY_SETTINGS_QUERY_ID, (BYTE*)&aReality, 4);
}

void	CMapManagerServer::lock_data_handlers()
{
	EnterCriticalSection(&m_cs);
}

void	CMapManagerServer::unlock_data_handlers()
{
	LeaveCriticalSection(&m_cs);
}

CMapManagerServer::~CMapManagerServer(void)
{
	DeleteCriticalSection(&m_cs);}
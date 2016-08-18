#pragma once

#include "CommonRenderManagerHeader.h"

#define MAP_MANAGER		1

#include "ZoneObjectsTypes.h"
#include "MapManagerPriority.h"
#include "MapManagerStructures2.h"
#include "LoadingQueueInfo.h"
#include "Rect2D.h"

class CMapManager;
class CMapTreeManager;

// Карта объектов по идентификаторам
typedef	MP_MAP<ZONE_OBJECT_ID, ZONE_OBJECT_INFO*>	mapZoneObjects;
// Массив описания объектов
typedef	MP_VECTOR<ZONE_OBJECT_INFO*>		vecZoneObjects;
// Массив серверных описания объектов
typedef	MP_VECTOR<ZONE_OBJECT_INFO*>		vecServerZoneObjects;

typedef struct _ZoneVersion
{
	DWORD	version;
	//
	CComString	to_string()
	{
		char	buf[30];	ZeroMemory(buf, sizeof(buf)/sizeof(buf[0]));
		itoa( version, (char*)&buf, 10);
		return CComString((LPSTR)buf);
	}
} ZoneVersion;

#define GROUND_NOT_LOADED			0
#define GROUND_OBJECT_LOADED		1
#define GROUND_LOADED_BY_TIMEOUT	2

//////////////////////////////////////////////////////////////////////////
class CMapManagerZone
{
public:
	CMapManagerZone(oms::omsContext* aContext);
	~CMapManagerZone(void);

public:
	void	SetLoadingQueueInfoPtr(CLoadingQueueInfo* ptr);

	void	DeleteAllObjects();

	void	SetGroundLoadedStatus(bool status);
	int		GetGroundLoadedStatus();

	bool	IsBoundsContains(const CMapObjectVector& point);
	bool	IsObjectIn(const CMapObjectVector& point, const CMapObjectVector& radius);

	void	SetVisible(bool isVisible);

	void	ResortObjectsByDistance();

	ZoneID	GetZoneID()
	{ 
		return m_zoneID;
	}
	void	SetZoneID(ZoneID azoneID);

	ZONE_OBJECT_INFO*	GetObjectInfo(const ZONE_OBJECT_ID& objectID);

	void	Update(DWORD time, bool isUpdate);				// приходит каждый фрейм
	void	UpdateLoadingPriority(CCamera3D* camera);
	void	UpdateObjectsInfo(std::vector<ZONE_OBJECT_INFO *>& info);

	void	NotifyMlMediaOnZoneChanged(ZONE_OBJECT_INFO* info);
//	bool	is_registered(ivisibleobject* object);
	ZONE_OBJECT_INFO	UnregisterObject(const ZONE_OBJECT_ID& objectID);

	void	AddNewObject(ZONE_OBJECT_INFO* info);

	void	DeleteNonZoneObjects(CRect2D& zonesRect);
	void	DeleteObject(unsigned int aObjectID);

	// Сохраняет в лог текущую очередь загрузки
	void	DumpLoadingQueue();

	void	SetTruncateMode(bool isEnabled);

private:
	bool	IsZonePermitted(unsigned int zoneID);
	void	Clear();
	void	UpdateProgressLoading();
	void	LoadNonPermittedZonesList();
	int		GetMinLastScreenTime();
	bool	IsObjectCanBeCreated(ZONE_OBJECT_INFO* info);
	
	CLoadingQueueInfo* m_loadingQueueInfo;

	bool			m_isZoneVisible;
	ZoneVersion		m_zoneVersion;		
	oms::omsContext* m_context;
	vecZoneObjects	m_asynchLoadedObjects;
	bool			m_isGroundLoaded;
	
	int				m_skipFrameCount;
	int				m_updateObjectsCount;
	int				m_areMostObjectsLoaded;
	__int64			m_lastAvatarCreateTime;
	__int64			m_firstObjectTime;

	MP_VECTOR<unsigned int> m_nonPermittedZones;

	vecServerZoneObjects	m_progressLoadedObjects;	

protected:
	ZoneID			m_zoneID;
	bool m_isTruncateMode;
};

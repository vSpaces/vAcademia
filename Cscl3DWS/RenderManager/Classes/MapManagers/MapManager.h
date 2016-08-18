#pragma once

#include "../CommonRenderManagerHeader.h"

#include "RealityInfo.h"
#include "3DObject.h"
#include "LoadingQueueInfo.h"
#include "OculusRift.h"
#include "Kinect.h"

/************************************************************************/
/* Описание работы с сервером карты:
1. При логинации пользователь
*/
/************************************************************************/


/************************************************************************/
/* Класс отвечает за достоверность клиентской и серверной частей карт.
Отслеживает перемещение статических и динамических объектов, проверяет их
версии и перегружает в случае необходимости.

Принцип работы следующий:
1. При попадании пользователя в один из квадратов карты, класс запрашивает информацию о 
новых квадратах у сервера. К новым относятся квадрат, в котором находится пользователь и 
все соседние квадраты, которых не было видно на предыдущем кадре.
Информация о квадрате включает в себя:
- число объектов
- по каждому объекту:
--имя объекта
--его версию
--путь к его ресурсам
--координаты и размеры обхекта

2. Сравниваются новые данные с данными, имеющимися на локальном компьютере (локальное хранилище объектов, кэш)

3. При обнаружении новых объектов или при изменении версии того или иного объекта создаются новые объекты, 
	которые отсылаются на загрузку.

PS: рендеринг объектов осуществляется при помощи OctTree, которое находится в сцене
/************************************************************************/

/************************************************************************/
/* Для работы с сервером вводим дополнительный объект
/************************************************************************/

/************************************************************************/
/* Класс также управляет загрузкой объекта. При обнаружении объекта, который нужно 
загрузить, запоминается ссылка на них, после чего на каждом апдейте устанавливается 
его приоритет, в зависимости от расстояния и видимости, до полной загрузки объекта.
/************************************************************************/


#include "ClassScriptsLoader.h"
#include "PrimObjectsLoader.h"
#include "MapManagerZone.h"
#include "CreateObjectZone.h"
#include "NatureSettings.h"
#include "nrmObject.h"
#include "oms_context.h"
#include "LogWriter.h"
#include "MapManagerPriority.h"
#include "Scene.h"
#include "TempPacketIn.h"
#include <VoipManClient.h>
#include "CrossingLine.h"
#include "IPriorityUpdater.h"


using namespace cm;
class	CMapManagerServer;

namespace Protocol
{
	class	CCommonPacketIn;
};
using namespace Protocol;

typedef	MP_VECTOR<CMapManagerZone*>			 CVectorZones;
typedef MP_MAP<ZONE_OBJECT_ID, ZoneIdentifier> CMapFixDBErrors;
typedef MP_MAP<unsigned int, CRealityInfo>	 CRealitiesMap;

typedef	struct _ZonesDescQueryResult
{
	MP_VECTOR<ZoneID>	zonesIDs;
} ZonesDescQueryResult;

typedef struct _SCRIPTED_OBJECT_DESC
	{
		C3DObject*	obj3d;
		mlMedia*	ismobject;
		_SCRIPTED_OBJECT_DESC()
		{
			obj3d = NULL;
			ismobject = NULL;
		}
	} SCRIPTED_OBJECT_DESC;

// информация об объекте для обновления данных об объекте на сервере
// или для создания нового объекта
struct SObjectInfoForUpdate
{
	MP_STRING name;	// строковый идентификатор объекта (если "", то будет оставлен как есть или сгенерируется автоматически)
	MP_STRING res;		// src объекта или группы
	// положение
	float x;
	float y;
	float z;
	// bounds-ы (относительно центра)
	float bx;
	float by; 
	float bz; 
	// поворот
	float rx;
	float ry;
	float rz;
	float rw;
	// масштаб
	float scalex;
	float scaley;
	float scalez; 
	// идентификатор зоны
	unsigned int zoneID;
	MP_STRING className;
	MP_WSTRING userProps;
	// тип: 0 - не определен, 1 - object, 2 - group
	int type;
	// идентификатор объекта
	unsigned int objectID;
	unsigned int bornRealityID;
	// ??
	int userData;
	// "уровень": ?? 4 - стенки
	int level;
	// идентификатор локации
	MP_STRING location;

	SObjectInfoForUpdate();
};

#define PMO_IS_NULL(PMO) ((((unsigned)PMO)&~3)==0)

//////////////////////////////////////////////////////////////////////////
class CMapManager : public nrmObject,
					public cm::cmIMapManager,
					public CMapManagerPriority,
					public IChangesListener,
					public IPriorityUpdater
{
public:
	CMapManager(oms::omsContext* context, CNRManager* manager);
	~CMapManager(void);

	// реализация iscenemanager
public:
	bool	IsPointInActiveZones(const float x, const float y);
	
	void	SetMainObjectCoords(float x, float y, float z);
	CVector3D GetMainObjectCoords();

	void StartTrackVisibleScreensInCurrentLocation();
	std::map<std::wstring, int> GetVisibleScreensInCurrentLocation();
	void EndTrackVisibleScreensInCurrentLocation();

	bool	GetKinectPoints(float* points);

	void	StartRender3D();
	void	StartPhysics();
	bool	IsRender3DNeeded()const;
	bool	IsPhysicsNeeded()const;
	void	SetIsRender3DNeeded( bool abVal);

	int		InitOculusRift(bool isNeedToEnable);
	void	GetOculusRiftDesktopParams(int& wResolution, int& hResolution, int& displayX, int& displayY);

	bool	InitKinect(bool isNeedToEnable);
	bool	InitGamepad(bool isNeedToEnable) override;

	void	CheckForLoadedMapInfo();
	void	CheckForLostedLods();

	void	UpdateObjectsInfo(std::vector<ZONE_OBJECT_INFO *>& info);
	void	UpdateSyncObjects(std::vector<ZONE_OBJECT_INFO *>& info);

	void	OnChanged(int eventID);

	void	OnZoneEntered(ZoneID zoneID);

	bool	IsInZone()const;	

	virtual void OnLogout( int iCode);

	mlMedia* GetScene3DPtr();
	bool IsScene3DReady();

public:
	void OnRealityDesc(CCommonPacketIn* apQueryIn);
	void OnDeleteObject(CCommonPacketIn* apQueryIn);
	void OnLocationCreated(CCommonPacketIn* apQueryIn);

private:
	void NotifyRealityLoaded( CRealityInfo& realityInfo);

	void TrackVisibleScreensInCurrentLocation(int firstObject);

	CRect2D GetObjectRect(C3DObject* obj3d);
	CRect2D GetZonesRect();

public:
	// приходит каждый фрейм
	void	Update(DWORD time);							
	// устанавливаем сцены, в которую будем добавлять объекты
	void	SetScene(CScene* scene);								
	C3DObject*	CreateStaticObject(ZONE_OBJECT_INFO* info);
	void	ReloadZonesInfo();
	
	SCRIPTED_OBJECT_DESC CreateScriptedObject(ZONE_OBJECT_INFO* info);
	nrmPlugin* CreatePlugin();
	void ObjectLeaveZone(const ZONE_OBJECT_INFO& info, CMapManagerZone* zone);
	CScene* GetScene();
	C3DObject* GetAvatar();

	void	ExtractIncludes( IAsynchResource* asynch, std::vector<std::string>&	includeFilesVector);

	CVector3D *GetLocationVector3D(rmml::ml3DPosition* leftTopPoint,  rmml::ml3DPosition* rightBottomPoint, rmml::ml3DPosition* outsidePoint);
// реализация cmIMapManager
public:
	// реализация cmIMapManager
	// запросить с сервера данные для создания аватара и создать его
	virtual oms::omsresult CreateMyAvatar(const wchar_t* apwcURL);
	// Получить список локаций
	virtual  std::vector<rmml::ml3DBounds>  GetVisibleLocations();
	// Получить список локаций общения (зон общения)
	virtual  std::vector<rmml::ml3DBounds>  GetVisibleCommunicationAreas();
	
	// Проверяет, может ли быть создана локация по указанным координатам
	virtual int IsLocationCanBeCreated(rmml::ml3DPosition* aLeftPoint, rmml::ml3DPosition* aRightPoint );
	// Обрабатывает сообщение от сервера
	virtual int HandleServerMessage( unsigned char aMessageID, unsigned char* aData, int aDataSize);
	// Сохраняет в лог текущую очередь загрузки
	virtual void DumpLoadingQueue();
	virtual oms::omsresult CreateObjects( cmObjectIDs* objects, unsigned int objectsCount);
	// Создать временную локацию
	virtual oms::omsresult CreateLocation(rmml::ml3DPosition* leftTopPoint,  rmml::ml3DPosition* rightBottomPoint, rmml::ml3DPosition* outsidePoint, const wchar_t* apwcLocationName);	
	// Удалить временную локацию
	virtual oms::omsresult DeleteLocation();
	// 
	bool SetCallbacks(cm::cmIMapManCallbacks* callbacks);
	void HandleTeleportError( unsigned int auErrorCode, const wchar_t* apwcDescription, const wchar_t* apwcEventInfo);
	virtual int GetCurrentRealityID();
	cmRealityType GetCurrentRealityType();
	virtual int GetPrevRealityID();
	// возвращает URL места, где находится аватар
	virtual int GetAvatarLocationURL(wchar_t* url, int aiBufferSize);
	// возвращает координаты аватара
	virtual void GetAvatarPosition( float& aX, float& aY, float& aZ, char** aLocation = NULL);
	virtual void GetAvatarQuaternion( float& aX, float& aY, float& aZ, float& aW);

	// перемещает аватар
	virtual void MoveAvatar( const cmTeleportInfo& teleportInfo);
	// проверяет является объект аватаром
	virtual bool IsAvatar(rmml::mlMedia* apObject);
	// установить объект, который считать аватаром клиента
	void SetAvatar(rmml::mlMedia* object, const ZONE_OBJECT_INFO& objectInfo);
	//получить ссылку на аватара по имени
	virtual rmml::mlMedia* GetAvatarByName(std::string& name);
	virtual voip::IVoipCharacter* GetIVoipAvatarByName(const char* name);
	// отправить MapServer-у запрос на удаление объекта из базы данных
	bool Remove(rmml::mlMedia* object);
	// отправить MapServer-у запрос на создание объекта
	bool CreateObject(const rmml::mlObjectInfo4OnServerCreation* objectDesc, unsigned int auRmmlQueryID);
	// получить описание реальности
	bool GetRealityInfo( unsigned int realityID, cmIRealityInfo*& realityInfo);
	// 
	ZoneID GetCurrentZoneID();
	virtual unsigned long GetCurrentZoneId();
	//
	void ActivateZones();
	void ActivateZones(ZoneIdentifier aCurrentZone);
	// Установить соединение с сервером голосовой связи.
	// В качестве параметра нужно бы уже передавать имя группы.
	bool UpdateVoipConnection();

	// читает файл фильтра объектов, которые можно создавать
	void ReadFilterFile();
	// проверяет в соответствии с фильтром, можно ли создавать объект?
	bool AllowCreateByFilter(const char* objName, CMapObjectVector &translation);
	// проверяет в соответствии с Nap-фильтром, можно ли создавать объект?
	bool AllowCreateByNapFilter(const std::string& objName, const std::string& className,
										 const int level, const int type, const CMapObjectVector &translation);
	// Возвращает описание зоны объекта
	const char* GetObjectLocationName(mlMedia* mlObj);
	std::string GetObjectCommunicationAreaName(mlMedia* mlObj);	
	void DeleteObject(mlMedia* rmmlObject);
	void RemoveLocation();
	void RemoveCommunicationArea();
	void DeleteTempLocations(const std::vector<unsigned int>& aExcludeObjects);
	void RemoveLocation(const char* objName);
	void RemoveCommunicationArea(const char* objName);
	bool IsObjectCreating(unsigned int objectID);
	// Загружает файл с классом объекта
	int LoadClassFile( const char* className);
	// Заполняет описание объекта для его созданния
	EFillingObjectInfoResult FillObjectInfo( const ZONE_OBJECT_INFO& aZoneObjectInfo, rmml::mlObjectInfo4Creation& anObjectInfo);
	// Обработать обрыв связи с сервером
	void OnConnectLost();
	// Обработать восстановление связи с сервером
	void OnConnectRestored();
	// получить описание локации в точке с заданными координатами
	bool GetLocationByXYZ(float afX, float afY, float aFZ, rmml::ml3DLocation &aLocation);
	// получить название локации в точке с заданными координатами
	char* GetLocationNameByXYZ(float afX, float afY, float afZ);
	// получить параметры локации в виде строки (это значение из objects_map.params)
	bool GetLocationParams( const wchar_t* apwcLocationID, wchar_t* paramsBuffer, unsigned int auParamsBufferSize);
	// вычисляет имя объекта, который должен использоваться для ожидания входа в реальность.
	MapManagerError GetRealityGuardName(unsigned int auRealityID, wchar_t* apwcBuffer, unsigned int auBufferLength);

public:
	int		LoadClassFile(std::string& className);
	bool	LoadPrimObject(std::string& fileName, std::string& objName, IAsynchResource** asynchResource);
	void	ExecJS(const wchar_t* js, wchar_t** result = NULL);
	void	ExecJS(const char* js, wchar_t** result = NULL);
	bool	IsJSObjClassDefined(LPCTSTR className);

public:
	void	UpdateResourceLoadingPriority(IAsynchResource* asynchRes);

private:
	void	Clear();
	ZoneID	GetObjectZone(C3DObject* obj);
	bool	QueryZonesObjects( const std::vector<ZoneID>& aZonesIDs);
	void	GetNeighbourZonesIDs(ZoneID	zoneID, std::vector<ZoneID>& zones);
	void	ReloadWorld();
	void	MoveAvatarOnUpdate();
//	void	QueryRealityInfo( unsigned int realityID);
	void	UpdateLoadingPriority(CCamera3D* camera);
	bool	TeleportReality( cmTeleportInfo& aTeleportInfo);
	bool	TraceGroundLevelAtPoint( float ax, float ay, float* az);

	void	DeleteNonZoneObjects();	

	cmRealityType GetPrevRealityType();

public:
	ZoneID	GetObjectGlobalZone(C3DObject* obj);
	CMapManagerZone*	GetZone(ZoneID zoneID);
	bool	AddZoneIfNotExists(ZoneID zoneID);
	void	SetConnectedRealityID( unsigned int aConnectedRealityID);
	void	OnObjectsRecieved(unsigned int aPrevRealityID, unsigned int aCurrentRealityID, const std::vector<ZONE_OBJECT_INFO *>& aNewObjects);
	void	OnGroundLoaded(ZONE_OBJECT_INFO* info);
	void	UpdateActiveZones();
	void	ReloadWorldImpl();
	void	CollectSynchObjects( std::vector< mlMedia*>& aSynchObjects);

public:
	SLocationInfo *CreateLocationInfo( ZONE_OBJECT_INFO *info, std::string &nam);
	void AddLocationBounds( ZONE_OBJECT_INFO *info);
	void AddCommunicationAreaScales( ZONE_OBJECT_INFO *info);
	void CheckLocation();
	void CheckCoordinateXY();
	void OnLocationChanged();
	void OnCommunicationAreaChanged();
	void ClearLocationBounds();
	// удаление всех объектоы границ локаций, созданных не в нулевой реальности
	void ClearLocationBoundsRNZ();
	void GetObjectsInLocation(std::vector<mlObjectInLocation >& objects);
	int SelectObjectsInLocation();
	unsigned int GetCurrentLocationName(wchar_t* aLocationName, unsigned int aNumberOfElements);

	void GetCommunicationAreaName( std::vector<SLocationInfo *> vCommunicationArea, std::string &asName);
	std::string GetCurrentCommunicationAreaName();
	const void GetSynchID(unsigned int &objID, unsigned int &bornRealityID);

	SLocationInfo* GetObjectLocation(C3DObject* obj);
	void GetObjectCommunicationArea(C3DObject* obj, std::vector<SLocationInfo *> &av_CommunicationArea);
	SLocationInfo* GetLocationByXYZ(float x, float y, float z);

	int IsCurrentCommunicationArea( SLocationInfo *apArea);
	void GetCommunicationAreaByXYZ(float x, float y, float z, std::vector<SLocationInfo *> &avCommunicationArea);
	SLocationInfo* GetLocationByID(const wchar_t* apwcLocationID);

private:
	bool GetCrossingLocationBounds( SLocationInfo *pLocInfo, CPoint &ptObj, CPoint &ptMyAvatar, CPoint &pt1, CPoint &ptRes);
	// Получить точки пересечения отрезка (ptP1, ptP2) с границами локации
	// Возвращает количество пересечений и сами пересечения в ptCP1 и ptCP2
	int GetCrossingsWithLocationBounds(const SLocationInfo *pLocInfo, const CPoint &ptP1, const CPoint &ptP2, CPoint &ptCP1, CPoint &ptCP2);

	void UpdateVisibilityDistance();

public:
	bool GetNewPositionObjectOnBoundLocation( const wchar_t *apwcLocationID, rmml::ml3DPosition &myAvatarPos, rmml::ml3DPosition &objPos, rmml::ml3DPosition &ptObjNew);
	void OnMemoryOverflow();

	void StartKinect();
	void StopKinect();
	void SetKinectMode(int iMode);
	// ---
	void SetMarkerType(int iMarker);
	void EnableFaceTracking();
	void DisableFaceTracking();
	// ---
	void GetKinectState(bool& abHeadFound, bool& abLeftHandFound,  bool& abLeftLegFound,  bool& abRightHandFound,  bool& abRightLegFound,  bool& abOneSkeletFound, bool& abMarkersNotFound);

private:
	void	DeletePendingObjects(bool isTimeLimited = true);
	void	WriteLog(LPCSTR alpcLog);

	bool repeatQueries;

private:
	oms::omsContext*    m_context;
	C3DObject*			m_mainObject;
	ZONE_OBJECT_INFO	m_mainObjectInfo;
	ZoneID				m_currentZoneID;
	CVectorZones		m_zones;
	CVectorZones		m_activeZones;
	CCreateObjectZone	m_objectZone;
	CMapManagerServer*	m_server;
	CScene*				m_scene3d;
	bool				m_acceptRegisterQueries;
	bool				m_groundLoadedMessageWasSend;
	cmTeleportInfo		m_teleportInfo;
	bool				m_needTeleport;
	MP_VECTOR<mlObjectInLocation> m_selectedObjects;
	CMapManagerZone*	m_commonZone;
	CLoadingQueueInfo	m_loadingQueueInfo;
	bool				m_bModuleLoaded;

	CVector3D			m_mainObjectCoords;

	MP_VECTOR<int>	m_objectsForRemove;
	MP_VECTOR<moMedia*> m_childrenForRemove;

	MP_VECTOR<IAsynchResource *> m_asynchResources;	

	DWORD m_updateProgressZonesCounter;	

	MP_VECTOR<SLocationInfo *> m_locationsInfo;
	MP_VECTOR<SLocationInfo *> m_communicationAreasInfo;
	SLocationInfo* m_currentLocation;
	MP_VECTOR<SLocationInfo *> m_vCurrentCommunicationArea;

	CRITICAL_SECTION cs;

	unsigned int m_createdAvatarID;

	cm::cmIMapManCallbacks* m_callbacks;

	MP_VECTOR<MP_STRING> m_filter;

	CClassScriptsLoader m_classScriptsLoader;
	CPrimObjectsLoader m_primObjectsLoader;
	CRealitiesMap       m_realitiesCache;
	unsigned int        m_currentRealityID;
	unsigned int        m_prevRealityID;
	unsigned int        m_teleportingRealityID;

	__int64		m_lastKinectHandleTime;
	__int64		m_lastUpdateTime;

	bool m_isInZone;
	bool m_isRender3DNeeded;
	bool m_isPhysicsNeeded;
	__int64 m_startRender3DTime;
	__int64 m_lastLogTimeOnUpdate;

	int m_sceneChangedCounter;
	bool bIsNotFly;

	int m_savedDistanceKoef;

	MP_VECTOR<C3DObject *> m_trackedScreens;
	MP_MAP<MP_WSTRING, int> m_lastVisibleScreensInCurrentLocation;
	unsigned int m_lastCheckedForTrackObjectCount;

public:
	typedef struct _ReceivedMessage
	{
		unsigned int		iMessageID;
		MapManagerProtocol::CTempPacketIn*	pCommonPacketIn;
	} ReceivedMessage;
	MP_VECTOR<ReceivedMessage>	m_receivedMessages;
	
	CVectorZones* GetActiveZones();
	bool IsActiveZone(ZoneID zoneID);

	ILogWriter *GetILogWriter()
	{
		return m_context->mpLogWriter;
	}

	COculusRift m_oculusRift;
	CKinect m_kinect;
};
//
// $RCSfile: oms_context.h,v $
// $Revision: 1.96 $
// $State: Exp $
// $Locker:  $
//
// last change: $Date: 2010/03/02 14:40:07 $ $Author: linda $

#ifndef __omscontext_h___
#define __omscontext_h___

struct ZONE_OBJECT_INFO;
class CComString;
struct IProxySettings;
enum ProxyMode;
struct IConnection;

#include "oms_messages.h"

namespace oms {

  typedef unsigned int omsresult;
  #define OMS_FAILED(_omsresult) ((_omsresult) & 0x80000000)
  #define OMS_SUCCEEDED(_omsresult) (!((_omsresult) & 0x80000000))
  #define OMS_OK                              0
  #define OMS_ERROR_FAILURE                   ((omsresult) 0x80004005L)
  #define OMS_ERROR_ILLEGAL_VALUE             ((omsresult) 0x80070057L)
  #define OMS_ERROR_INVALID_ARG               OMS_ERROR_ILLEGAL_VALUE
  #define OMS_ERROR_NOT_IMPLEMENTED           ((omsresult) 0x80004001L)

  #define OMS_ERROR_BASE                      ((omsresult) 0xC1F40000)
  #define OMS_ERROR_NOT_INITIALIZED           (OMS_ERROR_BASE + 1)
  #define OMS_ERROR_NOT_SUPPORTED             (OMS_ERROR_BASE + 0x1F0)
  #define OMS_ERROR_RESOURCE_NOT_FOUND        (OMS_ERROR_BASE + 0x501) 
  #define OMS_ERROR_RESOURCE_FOUND_TIMEOUT    (OMS_ERROR_BASE + 0x502) 

}

#include "mi.h"
#include "mw.h"
#include <map>
#include <string>
#include <windef.h>
/*#ifndef LPVOID
typedef void *LPVOID;
#endif
#ifndef LONG
typedef long LONG;
#endif*/

#include <memory>

//#include "iclientsession.h"

namespace rmml {
  struct mlISceneManager;
  struct mlISinchronizer;
  struct ml3DPosition;
  struct ml3DBounds;
  class mlMedia;
  struct mlObjectInfo4OnServerCreation;
  struct mlObjectInfo4Creation;
  struct ml3DLocation;
}

namespace sm {
  struct smISoundManager;
}

namespace ws3d {
	struct IC3DWSModule;
}

namespace voip {
	struct IVoipCharacter;
}

namespace res {
  struct resIResourceManager;
  struct IResMan;
}

namespace lgs {
	struct ILoggerBase;
	struct ILogger;
}

namespace trc {
  struct trcITracer{
    virtual void trace(const char*,...)=0;
	virtual void setTraceMode(bool isWorking) = 0;
	// включение выключение режима посылки сообщений лога Серверу.
	virtual void StartSendingToServer(lgs::ILoggerBase* aLogSender) = 0;
	virtual void StopSendingToServer() = 0;
  };
}

namespace rm {
  struct rmIRenderManager;
}

namespace cs {
	struct iclientsession;
	class CServerInfo;
}

namespace preview {
	struct IPreviewRecorder
	{
		// начать запись
		virtual void StartRecording(unsigned int aRecordID, unsigned int aDuration) = 0;
		// добавить фрейм к текущей записи
		virtual void AddEvent( wchar_t * alpcEvent, wchar_t * alpcCameraName) = 0;
		// добавить статистику о просмотре экранов
		virtual void AddStatistic( wchar_t * alpcStatistic) = 0;
		// добавить фрейм к текущей записи
		virtual bool AddFrame( rmml::mlMedia* apObject, wchar_t * alpcCameraName, char countScreenImage) = 0;
		// добавить фрейм к текущей сессии
		virtual bool AddStatisticFrame( rmml::mlMedia* apObject) = 0;
		// добавить сообщение чата к текущей записи
		virtual void AddChatMessage( int aType, wchar_t *wsLogin, wchar_t *wsText, bool abCloudedEnabled) = 0;
		// остановить запись
		virtual void StopRecording(unsigned int aRecordID) = 0;
		// получить адрес превью текущей записи
		virtual void GetPreviewCurrentRecordAddress( int aRecordID, wchar_t* &alpPreviewCurrentRecordAddress) = 0;
		// установить адрес сервера превью
		virtual void SetPreviewServerAddress( unsigned int aiRecordID, char *lpServerAddress, unsigned int aiServerPort, char *lpPreviewHttpHost, char *lpStoragePath) = 0;
		// удалить запись (отсылает запрос на сервер)
		virtual bool DeleteRecord(unsigned int aRecordID) = 0;
		// удалить записи (отсылает запрос на сервер)
		virtual bool DeleteRecords( unsigned int *pRecIDs, unsigned int count) = 0;
		// установить кадр на объект
		virtual wchar_t* ApplyFrameToTarget( const char* lpcTarget, int bpp, int width, int height, const void* bits, int quality) = 0;
		// сохранить изображение во временное хранилище(н-р аватара)
		virtual  wchar_t* AddImage( rmml::mlMedia* apObject, const char* lpcName, int srcX, int srcY, int srcWidth, int srcHeight, int destWidth, int destHeight) = 0;
		// установить время начала проигрывания звука
		virtual void AddPreviewAudio( unsigned int auiRecordID, const char* alpcFileName) = 0;
		// установить картинку по умолчанию из картинки alpcFileName
		virtual void SetPreviewDefaultImage( unsigned int auiRecordID, wchar_t *alpcFileName) = 0;
		// получить продолжительность записи
		virtual unsigned int getRecordingDuration() = 0;
		// начинаем отсылать статистика просмотра экранов
		virtual void StartStatisticRecording(unsigned int recordId, unsigned int aDuration) = 0;
		// заканчиваем отсылать статистика просмотра экранов
		virtual void StopStatisticRecording() = 0;
	};
}

namespace cm {

	struct cmIComManCallbacks{
		virtual void onQueryComplete(int aiID, const wchar_t* apwcResponse=NULL, int* aiErrorCode=NULL)=0;
	};

	struct IConnectionManager {
		virtual IConnection* getConnection(cs::CServerInfo* aServerInfo, int& aConenctionResult, DWORD aConnectionSessionID) = 0;
	};


	struct ichangeconnection {
		// Изменились параметры сетевых настроек
		virtual void OnChangeConnectionType(void* aProxyInfo) = 0;

		virtual void SubscribeToNotify(ichangeconnection* aSubscriber) = 0;
		virtual void UnSubscribeFromNotify(ichangeconnection* aSubscriber) = 0;
	};

	struct IChangeHttpSettings {
		// Изменились параметры сетевых настроек
		virtual void OnChangeConnectionSettings(void* aProxyMode, void* aProxySettings) = 0;
	};

	struct cmICommunicationManager{		
		virtual long getProxy(/*[out]*/ BYTE* aProxyUsing, /*[out]*/ BSTR* aProxy, /*[out]*/ BSTR* aProxyUser, /*[out]*/ BSTR* aProxyPassword)=0;
		virtual void CreateSyncServerSession(cs::iclientsession** ppClientSession)=0;
		virtual void CreateServiceServerSession(cs::iclientsession** ppClientSession)=0;
		virtual void CreateVoipSelfRecordingServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo)=0;
		virtual void CreateVoipRecordServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo)=0;
		virtual void CreateVoipPlayServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo)=0;
		virtual void CreateSharingServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo)=0;
		virtual void CreatePreviewServerSession(cs::iclientsession** ppClientSession, cs::CServerInfo *apServerInfo)=0;
		virtual void CreateServiceWorldServerSession(cs::iclientsession** ppClientSession)=0;
		virtual void DestroyServerSession(cs::iclientsession** ppClientSession)=0;
		virtual bool SetCallbacks(cmIComManCallbacks* apCallbacks)=0;
		virtual const wchar_t* Query(const wchar_t* apwcQuery, const wchar_t* apwcParams=NULL, int* aiErrorCode=NULL)=0;
		virtual const wchar_t* QueryPost(const wchar_t* apwcQuery, const wchar_t* apwcParams=NULL, int* aiErrorCode=NULL)=0;
		virtual unsigned char* QueryData(const wchar_t* apwcQuery, const wchar_t* apwcParams=NULL, 
			unsigned long* aDataSize=NULL, unsigned char** aData=NULL, int* aiErrorCode=NULL)=0;
		virtual int SendQuery(const wchar_t* apwcQuery, const wchar_t* apwcParams=NULL)=0;
		virtual int SendQuery(const wchar_t* apwcQuery, const wchar_t* apwcParams, cmIComManCallbacks* aHandler)=0;
		// возвращает true, если авторизация прошла успешно
		virtual void LogLoginBeginTime()=0;
		virtual unsigned int GetConnectionStatus()=0;
		virtual const wchar_t* GetServerName()=0;
		virtual const wchar_t* GetServerIP()=0;
		virtual const wchar_t* GetClientStringID()=0;
		virtual bool ConnectToWorld(const wchar_t* apwcURL)=0;
		virtual unsigned long long GetSessionID()=0;
		virtual int SetLoginData(unsigned int aDataSize, const wchar_t* aData)=0;
		virtual long PutPathIni(wchar_t *newVal)=0;
		virtual void SetLocalBase( wchar_t *newVal)=0;
		virtual void SetRepositoryBase( wchar_t *newVal)=0;
		//virtual void SetUserBase( wchar_t *newVal)=0;
		virtual void SetConnectType(byte aConnectType)=0;
		virtual void ReloadUserSettings() = 0;
		virtual void SetProxyLoginPass(const wchar_t * aProxyUser, const wchar_t * aProxyPassword, unsigned char aProxyType, const wchar_t * aProxySetting)=0;
		virtual void CloseAllSessions()=0;
		virtual void DestroyComman()=0;
		virtual std::string GetSessionsInfoList()=0;
		virtual void changeLogWriteEnabled(bool isLogEnabled)=0;

		// Возвращает IP адрес компьютера
		virtual const std::wstring GetMyIP() = 0;
		// 
		virtual unsigned char GetProxiesMask() = 0;
		virtual long GetRecommendedPacketSize(/*int aThreadsCount*/) = 0;
		virtual cm::ichangeconnection* GetConnectionChanger() = 0;
		virtual cm::IConnectionManager* GetConnectionManager() = 0;
		virtual void SetNetworkBreaksEmulationDelay( int auDelay, int auConnectionsMask) = 0;
		virtual bool IsWinSocketConnection() = 0;

		virtual void DropAndFreezeConnection( USHORT	signature) = 0;
		virtual void UnfreezeConnection( USHORT	signature) = 0;

		virtual void Update() = 0;
		virtual const wchar_t *GetHost() = 0;
		virtual const wchar_t *GetBuildType() = 0;
	};


	/**
	* Типы реальностей
	*/
	enum cmRealityType
	{
		RT_ZERO, RT_CUSTOM, RT_RECORDING, RT_PLAYING, RT_EDITING, RT_USER
	};

	struct RealityObjectGuardFlags
	{
		const static unsigned int USE_NO_GUARD = 0;
		const static unsigned int USE_PARENT_FLAGS = 1;
		const static unsigned int WAIT_GUARD_OBJECT = 2;
	};

	/**
	* Данные о реальности
	*/
	struct cmIRealityInfo {
		virtual unsigned int GetRealityID() const = 0;
		virtual unsigned int GetGuardFlags() const = 0;
		virtual unsigned int GetRealityDepth() const = 0;
		virtual unsigned int GetParentRealityID() const = 0;
		virtual cmRealityType GetType() const = 0;
		virtual long long GetTime() const = 0;
		virtual const wchar_t* GetName() const = 0;
		virtual void* GetData() const = 0;
		virtual void SetData( void* pData) = 0;
	};

	/**
	* Данные для телепортации
	*/
	struct cmTeleportInfo {
		// Кординаты
		float x, y, z;
		// Поворот
		float rx, ry, rz, ra;
		// Реальность
		unsigned int uRealityID;
	};

	/**
	* Интерфейс обратных вызовов из менеджера карты
	*/
	struct cmIMapManCallbacks{
		virtual void onTeleported( cmTeleportInfo* aTeleportInfo)=0;
		virtual void onTeleportError(int aiError, const wchar_t* apwcError, const wchar_t* apwcEventInfo)=0;
		// объект был создан по запросу из Player.map.CreateObject()
		// (aiUserData - ссылка на объект, переданный в CreateObject)
		virtual void onObjectCreated(int aiUserData, unsigned int auiObjectID, unsigned int auiBornRealityID, int aiError, const wchar_t* apwcError)=0;
		// Получен статус объекта
		virtual void onReceivedObjectStatus(unsigned int auObjectID, unsigned int auBornRealityID, int aiStatus, void* apUserData) = 0;
	};

	struct cmObjectIDs
	{
		unsigned int objectID;
		unsigned int bornRealityID;

		cmObjectIDs() {}
		cmObjectIDs( unsigned int anObjectID, unsigned int aBornRealityID)
		{
			objectID = anObjectID;
			bornRealityID = aBornRealityID;
		}
		bool operator==( const cmObjectIDs& aRight) const
		{
			if (objectID != aRight.objectID)
				return false;
			if (bornRealityID != aRight.bornRealityID)
				return false;
			return true;
		}
		bool operator!=( const cmObjectIDs& aRight) const
		{
			return !(*this == aRight);
		}
	};

	class mlObjectInLocation{
	public:
		unsigned int objectID;
		unsigned int bornRealityID;
	};

	#define CLASS_LOADING		0
	#define CLASS_LOADED		1
	#define CLASS_NOT_EXISTS	2

	// Результат заполнения описания объекта для его созданния
	enum EFillingObjectInfoResult
	{
		// Описание объекта успешно запонено
		FOI_SUCCESS,
		// Описание объекта не запонено: не найден класс
		FOI_CLASS_NOT_FOUND,
		// Описание объекта не запонено: этот объект статичный
		FOI_STATIC_OBJECT,
		// Описание объекта не запонено: на сервере возникла ошибка при создании объекта
		FOI_WRONG_INFO,
		// Описание объекта отброшено фильтром
		FOI_FILTERED
	};

	typedef unsigned int MapManagerError;

	struct MapManagerErrors
	{
		const static MapManagerError ERROR_NO_ERROR = 0;
		const static MapManagerError ERROR_REALITY_NOT_FOUND = 1;
		const static MapManagerError ERROR_BUFFER_IS_TOO_SMALL = 2;
	};

	/**
	* Интерфейс, возвращаемый при создании менеджера карты
	*/
	struct cmIMapManager{
		virtual bool	GetKinectPoints(float* points) = 0;
		virtual oms::omsresult CreateObjects( cmObjectIDs* anObjects, unsigned int anObjectsCount) = 0;
		// Создать временную локацию
		virtual oms::omsresult CreateLocation(rmml::ml3DPosition* leftTopPoint,  rmml::ml3DPosition* rightBottomPoint, rmml::ml3DPosition* outsidePoint, const wchar_t* apwcLocationName) = 0;
		// Удалить временную локацию
		virtual oms::omsresult DeleteLocation() = 0;		
		// Установить интервейс обратных вызовов
		virtual bool SetCallbacks(cmIMapManCallbacks* apCallbacks)=0;
		// обработать ошибку телепортации
		virtual void HandleTeleportError( unsigned int auErrorCode, const wchar_t* apwcDescription, const wchar_t* apwcEventInfo) = 0;
		// возвращает URL места, где находится аватар
		virtual int GetAvatarLocationURL( wchar_t* url, int aiBufferSize)=0;
		// возвращает координаты аватара
		virtual void GetAvatarPosition( float& aX, float& aY, float& aZ, char** aLocation = NULL )=0;
		//возвращает поворот аватара
		virtual void GetAvatarQuaternion( float& aX, float& aY, float& aZ, float& aW) = 0;
		// Получаем количество объектов в локации
		virtual int SelectObjectsInLocation() = 0;
		// Получить имя текущей локации
		virtual unsigned int GetCurrentLocationName(wchar_t* aLocationName, unsigned int aNumberOfElements) = 0;
		// Получить имя текущей локации общения
		virtual std::string GetCurrentCommunicationAreaName() = 0;
		virtual const void GetSynchID(unsigned int &objID, unsigned int &bornRealityID) = 0;
		// Получить интерфейс на обработку звука аватаром по имени аватара
		virtual voip::IVoipCharacter* GetIVoipAvatarByName(const char* name) = 0;
		// Получаем ид-ры и реальность рождения объектов для записи
		virtual void GetObjectsInLocation(std::vector<mlObjectInLocation>& objects) = 0;
		virtual int GetCurrentRealityID()=0;
		virtual unsigned long GetCurrentZoneId()=0;
		// перемещает аватар
		virtual void MoveAvatar( const cmTeleportInfo& aTeleportInfo)=0;
		// проверяет является объект аватаром
		virtual bool IsAvatar(rmml::mlMedia* apObject)=0;
		// устанавливает объект в качестве аватара
		virtual void SetAvatar(rmml::mlMedia* apObject, const ZONE_OBJECT_INFO& objectInfo)=0;
		// отправить MapServer-у запрос на фиксацию текущих координат 
		//virtual bool FixPos(rmml::mlMedia* apObject)=0;
		// отправить MapServer-у запрос на удаление объекта из базы данных
		virtual bool Remove(rmml::mlMedia* apObject)=0;
		// отправить MapServer-у запрос на создание объекта
		virtual bool CreateObject(const rmml::mlObjectInfo4OnServerCreation* apObjectDesc, unsigned int auRmmlQueryID)=0;
		// получить статус объекта
		//virtual oms::omsresult GetObjectStatus(unsigned int auObjectID, unsigned int auBornRealityID, void* apUserData) = 0;
		// получить описание реальности
		virtual bool GetRealityInfo( unsigned int auRealityID, cmIRealityInfo*& aRealityInfo) = 0;
		// активизация видимых зон
		virtual void ActivateZones() = 0;
		// Возвращает описание зоны объекта
		virtual const char* GetObjectLocationName(rmml::mlMedia* mlObj) = 0;
		virtual std::string GetObjectCommunicationAreaName(rmml::mlMedia* mlObj) = 0;
		virtual void DeleteObject(rmml::mlMedia* rmmlObject) = 0;
		virtual bool IsObjectCreating(unsigned int objectID) = 0;
		// Загружает файл с классом объекта
		virtual int LoadClassFile( const char* className) = 0;
		// Заполняет описание объекта для его созданния
		virtual EFillingObjectInfoResult FillObjectInfo( const ZONE_OBJECT_INFO& aZoneObjectInfo, rmml::mlObjectInfo4Creation& anObjectInfo)=0;
		// Проверяет, лежит ли точка в видимых зонах
		virtual bool IsPointInActiveZones(const float x, const float y) = 0;
		virtual std::vector< rmml::ml3DBounds > GetVisibleLocations() = 0;
		virtual std::vector< rmml::ml3DBounds > GetVisibleCommunicationAreas() = 0;
		// Проверяет, может ли быть создана локация по указанным координатам
		virtual int IsLocationCanBeCreated(rmml::ml3DPosition* aLeftPoint, rmml::ml3DPosition* aRightPoint ) = 0;
		// Обрабатывает сообщение от сервера
		virtual int HandleServerMessage( unsigned char aMessageID, unsigned char* aData, int aDataSize) = 0;
		// Сохраняет в лог текущую очередь загрузки
		virtual void DumpLoadingQueue() = 0;
		// Обработать обрыв связи с сервером
		virtual void OnConnectLost() = 0;
		// Обработать восстановление связи с сервером
		virtual void OnConnectRestored() = 0;
		// получить описание локации в точке с заданными координатами
		virtual bool GetLocationByXYZ(float afX, float afY, float aFZ, rmml::ml3DLocation &aLocation) = 0;
		// получить имя локации в точке с заданными координатами
		virtual char* GetLocationNameByXYZ(float afX, float afY, float afZ) = 0;
		// получить параметры локации в виде строки (это значение из objects_map.params)
		virtual bool GetLocationParams( const wchar_t* apwcLocationID, wchar_t* paramsBuffer, unsigned int auParamsBufferSize) = 0;
		// установить флаг необходимо ли проводить рендеринг сцены
		virtual void SetIsRender3DNeeded( bool abVal) = 0;
		// код ошибки при отмены авторизации
		virtual void OnLogout( int iCode) = 0;
		// получить новые координаты перемещаемого объекта в на границе локации локации
		virtual bool GetNewPositionObjectOnBoundLocation( const wchar_t *apwcLocationID, rmml::ml3DPosition &myAvatarPos, rmml::ml3DPosition &objPos, rmml::ml3DPosition &ptObjNew) = 0;
		// Начать отслеживать видимые экраны
		virtual void StartTrackVisibleScreensInCurrentLocation() = 0;
		// Вернуть список видимых экранов с количеством видимых пикселей
		virtual std::map<std::wstring, int> GetVisibleScreensInCurrentLocation() = 0;
		// Закончить отслеживать видимые экраны
		virtual void EndTrackVisibleScreensInCurrentLocation() = 0;
		virtual bool UpdateVoipConnection() = 0;
		// проверить флаг необходимо ли проводить рендеринг сцены
		virtual bool IsRender3DNeeded()const = 0;		
		// начать захват движений
		virtual void StartKinect() = 0;
		// остановить захват движений
		virtual void StopKinect() = 0;
		// установить режим для кинекта (сидя, стоя)
		virtual void SetKinectMode(int iMode) = 0;
		// получить состояние кинекта
		virtual void GetKinectState(bool& abHeadFound, bool& abLeftHandFound,  bool& abLeftLegFound,  bool& abRightHandFound,  bool& abRightLegFound, bool& abOneSkeletFound, bool& abMarkersNotFound) = 0;

		// установить тип маркера для кинекта (красный, зелёный, синий)
		virtual void SetMarkerType(int iMarker) = 0;

		//включить\выключить трекинг головы
		virtual void EnableFaceTracking() = 0;
		virtual void DisableFaceTracking() = 0;

		// вычисляет имя объекта, который должен использоваться для ожидания входа в реальность.
		virtual MapManagerError GetRealityGuardName(unsigned int auRealityID, wchar_t* apwcBuffer, unsigned int auBufferLength) = 0;

		virtual bool IsScene3DReady() = 0;

		virtual int InitOculusRift(bool isNeedToEnable) = 0;
		virtual void GetOculusRiftDesktopParams(int& wResolution, int& hResolution, int& displayX, int& displayY) = 0;

		virtual bool InitKinect(bool isNeedToEnable) = 0;
		
		virtual bool InitGamepad(bool isNeedToEnable) = 0;
	};
}

namespace sync
{
	struct syncISyncManager;
}

namespace sync
{
	struct syncIRecordManagerCallback;
}

namespace service
{
	struct IServiceManager;
	struct IServiceWorldManager;
	struct IIPadManager;
}

namespace info
{
	struct IInfoManager;
}

namespace voip
{
	struct IVoipManager;
	//struct IVoipManagerClient;
}

namespace desktop
{
	struct IDesktopManager;
	struct IDesktopSharingManager;
}

namespace re
{
	struct IRecordEditor;
}

struct IAsyncResourcesLoadingStat;

namespace oms {
struct omsIApplication{
	virtual bool execSysCommand(const wchar_t* apwcCommandLine, unsigned int* apErrorCode) = 0;
	// вызвать команду у родителя окна
	virtual void ExternCommand(const wchar_t* apwcCmd, const wchar_t* apwcPar)=0;
	//
	virtual void SendDebugInfo(unsigned char acInfoType, void* hwndDebugger, 
							   unsigned char* apData, unsigned int auDataLength)=0;
	// минимизировать главное окно
	virtual void minimize()=0;
	// закрыть главное окно (выйти из программы)
	virtual void exit()=0;
	// загрузить dll
	virtual unsigned long LoadLibrary(const wchar_t* apcDllName)=0;
	// освободить dll
	virtual void FreeLibrary(unsigned long auHModule)=0;
	// получить адрес функции в dll
	virtual void* GetProcAddress(unsigned long auHModule, const char* apcProcName)=0;
	// получить строку GUID
	virtual void GetStringGUID( wchar_t* apwcGUID, unsigned short aSizeGUID)=0;
	// изменить размер главного окна
	virtual bool ResizeWindow(unsigned auClientWidth, unsigned auClientHeight, int aiFullScreen)=0;
	virtual void ResizeWindowForOculus(unsigned auClientWidth, unsigned auClientHeight, unsigned x, unsigned y)=0;
	// получить текущее время (timeGetTime для отладки)
	virtual unsigned long GetTimeStamp()=0;
	// получить версию плеера в строковом виде
	virtual bool GetVersion(wchar_t* apwcVersion, unsigned short aSizeVersion)=0;
	// получить язык в строковом виде, на котором следует выводить все сообщения (ru, en, ...)
	virtual bool GetLanguage(wchar_t* apwcLanguage, unsigned short aSizeLanguage)=0;
	// сохранить установку в реестре
	virtual bool SetSetting(const wchar_t* apwcName, const wchar_t* apwcValue)=0;
	// получить установку из реестра
	virtual bool GetSetting(const wchar_t* apwcName, wchar_t* apwcValue, unsigned short aSizeValue, const wchar_t* apwcSectionName = L"")=0;
	// получить уставновку из ini
	virtual bool GetSettingFromIni(const char* apwcSection, const char* apwcName, char* apwcValue, unsigned short aSizeValue)=0;
	// получить флаг о работе в дебаговой версии
	virtual bool GetDebugVersion()=0;
	// через стандартное диалоговое окно получить путь к файлу на локальном компьютере 
	virtual bool GetLocalFilePath(const wchar_t* apszFilter, const wchar_t* apwcStartPath, bool abExists, wchar_t* pwcPath, unsigned short aSizePath, const wchar_t* apszDefaultName)=0;
	// получить аргументы процесса (заданные в командной строке, например)
	virtual wchar_t** GetArguments()=0;
	// провести трансляцию скан-кодов в ascii и unicode
	virtual void TranslateKeyCode(unsigned short muKeyCode, unsigned short& muKeyStates, unsigned short muScanCode, unsigned short& muAscii, unsigned short& muUnicode, bool& mbAlt, bool& mbCtrl, bool& mbShift)=0;
    // отдает параметры десктопа
	virtual void GetDesktopSize(int& offsetX, int& width, int& height) = 0;
	// отдает параметры расширенного десктопа
	virtual void GetWideDesktopParams(int& width, int& height, int& startOffset) = 0;
	// отдает параметры тулбара винды
	virtual void GetToolbarParams(int& toolbarOffsetX, int& toolbarOffsetY, int& toolbarWidth, int& toolbarHeight) = 0;
	// сообщаем об успешной авторизации
	virtual void OnAuthorizated( int aiTypeSendLog) = 0;
	// активно ли окно приложения на данный момент?
	virtual bool IsActive() = 0;
	// проверить, нажата ли указанная клавиша на клавиатуре?
	virtual bool IsKeyDown(unsigned short muKeyCode, unsigned short muScanCode)=0;
	// показать диалоговое окно с сообщением
	virtual int ShowMessageBox(const wchar_t* apwcMessage, const wchar_t* apwcTitle = NULL)=0;
	// код ошибки при отмены авторизации
	virtual void OnLogout( int iCode) = 0;
	// размер памяти, занятый программой
	virtual bool GetConsumedMemorySize(unsigned long& PageFaultCount
											, unsigned long& PeakWorkingSetSize
											, unsigned long& WorkingSetSize
											, unsigned long& QuotaPeakPagedPoolUsage
											, unsigned long& QuotaPagedPoolUsage
											, unsigned long& QuotaPeakNonPagedPoolUsage
											, unsigned long& QuotaNonPagedPoolUsage
											, unsigned long& PagefileUsage
											, unsigned long& PeakPagefileUsage) = 0;

	virtual bool SetArguments(const wchar_t* apwcArgs)=0;
	virtual void SendMessage2Launcher( int id, unsigned long message) = 0;
	virtual void HandleDebugBreakKeyboardRequest( ) = 0;
	virtual void HandleMakeDumpKeyboardRequest( ) = 0;
	// Возвращает true - если пользователь включил режим ctrl + alt + shift + d
	virtual bool GetKeysShortcutsEnabled( ) = 0;
};
}

class ILogWriter;

namespace oms {

  /**
  * Базовый класс контекста открытых мультимедиа систем 
  * содержит ссылки на интерфейсы менеджеров и других блоков
  */
  class omsContext // OMS_DECL
  {
  public:
    rmml::mlISceneManager*          mpSM;
    rm::rmIRenderManager*           mpRM;
    oms::omsIInput*                 mpInput;
    oms::mwIWindow*					mpWindow;
    sm::smISoundManager*            mpSndM;
    res::resIResourceManager*       mpResM;
	res::IResMan*					mpIResM;
    trc::trcITracer*				mpTracer;
    rmml::mlISinchronizer*          mpSynch;
	ws3d::IC3DWSModule*				mp3DWS;
    oms::omsIApplication*	        mpApp;
	cm::cmICommunicationManager*	mpComMan;
	cm::cmIMapManager*				mpMapMan;
	sync::syncISyncManager*			mpSyncMan;
	service::IServiceManager*		mpServiceMan;
	service::IServiceWorldManager*	mpServiceWorldMan;
	info::IInfoManager*				mpInfoMan;
	ILogWriter*						mpLogWriter;
	ILogWriter*						mpLogWriterLastMsgAsync;
	//voip::IVoipManager*				mpVoipMan;
	voip::IVoipManager*		mpVoipMan;
	//voip::IVoipManagerClient*		mpVoipManClient;
	preview::IPreviewRecorder*		mpPreviewRecorder;
	desktop::IDesktopManager*		mpDesktopMan;
	lgs::ILogger*					mpLogger;
	desktop::IDesktopSharingManager*	mpSharingMan;
	re::IRecordEditor*				mpRecordEditor;
	IAsyncResourcesLoadingStat*		mpResLoadingStat;
	service::IIPadManager*	mpIPadMan;

  public:
    omsContext(void)
      : mpRM(0), mpSM(0),
        mpInput(0), mpWindow(0),
        mpSndM(NULL), mpResM(NULL), mpIResM(NULL), mpTracer(NULL),
        mpSynch(NULL), mpApp(NULL), mp3DWS(NULL),
        mpComMan(NULL), 
		mpMapMan(NULL), mpSyncMan(NULL),mpServiceMan(NULL), mpServiceWorldMan(NULL), mpInfoMan(NULL),
		mpLogWriter(NULL), mpLogWriterLastMsgAsync(NULL), mpVoipMan(NULL), /*mpVoipManClient(NULL),*/ mpPreviewRecorder(NULL),
		mpDesktopMan(NULL), mpLogger(NULL), mpSharingMan(NULL), mpRecordEditor(NULL), mpResLoadingStat(NULL), mpIPadMan(NULL)
    {
    }

	~omsContext(void){}

  public:
    static omsContext &instance();
    static bool init();
    static void uninit();
  };

}

#endif 
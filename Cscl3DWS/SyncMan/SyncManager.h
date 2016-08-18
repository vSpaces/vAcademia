#pragma once
#include "SyncMan.h"
#include "imessagehandler.h"
#include "iclientsession.h"
#include "ObjectStateQueue.h"

#include "MapObjectQueue.h"
#include <json/json.h>

// Received Type - типы принимаемых сообщений
#define RT_UpdatePacket				1  // Объект измененился
#define RT_BadUpdatePacket  		2  // Запрошенный объект не найден
#define RT_DeleteObject				3  // Объект удален
#define RT_SyncMessage				4  // Обработай сообщение владельцу объекта
#define RT_ConnectRecord			5  // Клиент подконнектился к записи
#define RT_LogRecordsSend	        6  // Записи лога переданы
#define RT_SeanceFinished			9	// Сеанс завершен
#define RT_LogSeanceCreated			10  // Сеанс воспроизведения создан
#define RT_ReceivedSeances			11  // Получен список сеансов воспроизведения для записи лога
#define RT_ReceivedFullSeanceInfo	12	// Получена полная инфа о сеансе включая URL
#define RT_ReceivedObjectStatus		14  // Получен статус объекта
#define RT_SeancePaused				15  // Получен статус объекта
#define RT_SessionCheck				16  // Запрос сервака на валидность сессии
#define RT_SeanceLoaded				17  // Сеанс воспроизведения загружен на сервере
#define RT_DisconnectRecord			18  // Клиент отконнектился от записи в аудитории
#define RT_UpdatePlayingTime		19  // С сервака пришло новое время до новой ключевой точки
#define RT_SeancePlayed				20  // С сервака пришла мессага что сеанс проигрывается
#define RT_SeanceRewinded			21  // С сервака пришла мессага что сеанс перемотан на начало
#define RT_SeanceFinishedPlaying	22  // С сервака пришла мессага что сеанс перемотан на начало
#define RT_ReceivedPrecizeURL		24  // С сервака пришел уточненный URL
#define RT_SeanceLeaved				25  // С сервака пришел уточненный URL
//#define RT_LessonCreated			26  // С сервака пришло подтвержение что в локации создано занятие.
#define RT_UserListReceived			27	// С сервера пришел список пользователей в реальности
#define RT_ServerError				28	// С сервера пришла ошибка с кодом
#define RT_EventEntered				29	// С сервера пришла нотификация о присоединении к эвенту
#define RT_EventLeaved				30	// С сервера пришла нотификация о отсоединении от эвента
#define RT_EventUpdateInfo			34	// С сервера пришло сообщение о том что эвент был изменен

#define RT_ObjectCreationResult		31	// С сервера пришел ответ о создании объекта
#define RT_StopRecord				32	// С сервера пришло сообщение о том что запись этого клиента была остановлена
#define RT_SeekPos					33	// С сервера пришло сообщение о том что запись была перемотана до определенной позиции
#define RT_EventRecordsSend			35	// Записи переданы с сервера
#define RT_ReceivedPrecizeURL		24  // С сервака пришел уточненный URL
#define RT_ReceivedSeancesInfo		39  // С сервака пришла информация о сеансах
#define RT_ReceivedEventUserRightsInfo		40  // С сервака пришла информация о правах пользователя в событии
#define RT_ReceivedEventUserRightUpdate		42  // С сервака пришло обновление о правах моего пользователя в событии
#define RT_ReceivedEventUserBanUpdate		43  // С сервака пришло о бане моего пользователя в событии
#define RT_ErrorCreateTempLocation			44	// C сервака пришло сообщение об ошибки создания временной локации
#define RT_ReceivedEventsInfoIntoLocation	45  // С сервака пришла информация о запланированных событиях и событиях в не нулевой реальности 

// принимаемые сообщения редактора записей
#define RT_RecordEditorCutComplete			46  // Релуьтат выполнения операции Cut 
#define RT_RecordEditorCopyComplete			47  // Релуьтат выполнения операции Copy 
#define RT_RecordEditorPasteComplete		48  // Релуьтат выполнения операции Paste 
#define RT_RecordEditorDeleteComplete		49  // Релуьтат выполнения операции Delete
#define RT_RecordEditorSaveComplete			50  // Релуьтат выполнения операции Save
#define RT_RecordEditorSaveAsComplete		51  // Релуьтат выполнения операции SaveAs
#define RT_RecordEditorPlayComplete			52  // Релуьтат выполнения операции Play 
#define RT_RecordEditorPauseComplete		53  // Релуьтат выполнения операции Pause
#define RT_RecordEditorRewindComplete		54  // Релуьтат выполнения операции Rewind
#define RT_RecordEditorSeekComplete			55  // Релуьтат выполнения операции Seek
#define RT_RecordEditorUndoComplete			56  // Релуьтат выполнения операции Undo
#define RT_RecordEditorRedoComplete			57  // Релуьтат выполнения операции Redo
#define RT_RecordEditorOpenComplete			58  // Релуьтат выполнения операции Open

#define RT_LocationEntered					59  // Пользователь сменил локацию на сервере

#define RT_InfoMessage						60  // Информационное сообщение

// сообщения Map Server
#define MRT_FIRST							127
#define MRT_ZONES_DESC_QUERY_ID				128
#define MRT_ZONES_OBJECTS_DESC_QUERY_ID		129
#define MRT_PORTALS_STATES_QUERY_ID			130
#define MRT_PORTALS_STATES_CHANGE_QUERY_ID	131
#define MRT_OBJECTS_DESC_QUERY_ID			132
#define MRT_OBJECT_UPDATE_QUERY_ID			133
#define MRT_OBJECT_DELETE_BY_NAME_QUERY_ID	134
#define MRT_OBJECT_DESC_BY_NAME_QUERY_ID	135
#define MRT_AVATAR_DESC_QUERY_ID			136
// NatureSettingsQueryID					137
#define MRT_REALITY_SETTINGS_QUERY_ID		138
#define MRT_OBJECT_CREATE_QUERY_ID			139
#define MRT_OBJECT_DELETE_QUERY_ID			140
#define MRT_REALITY_DESC_QUERY_ID			141
#define MRT_OBJECT_STATUS_QUERY_ID			142
#define MRT_CREATE_LOCATION_QUERY_ID		143
#define MRT_DELETE_OBJECT_QUERY_ID			144
#define MRT_LAST							145



namespace rmml {
struct mlSysSynchProps;
struct syncObjectProp;
struct syncObjectState;
}

namespace SyncManProtocol {
class CTempPacketIn;
}

using namespace rmml;


class CSyncManager :
	public sync::syncISyncManager,
	public cs::imessagehandler,
	public cs::isessionlistener,
	protected cm::cmIComManCallbacks
{
public:
	static int iWrongSynchPacket;
	CSyncManager( omsContext* aContext);
	virtual ~CSyncManager();

	// Подключись к серверу синхронизации
	virtual void Connect();
	// Отключись от сервера синхронизации
	virtual void Disconnect();
	// Удалить менеджер синхронизации
	virtual void Release();

	// получить хеш пакета состояния
	virtual unsigned int GetHashObj( const syncObjectState& object_all_props, bool abLogPacket);
	// Отослать на сервер данные об изменении синхронизируемых свойств объекта
	virtual void SendObjectState(const syncObjectState& object_props);
	// Получить следующее изменение состояния синхронизируемого объекта
	virtual bool NextObjectState(syncObjectState* &object_props);
	// Получить следующее изменение состояния для конкретного синхронизируемого объекта
	virtual bool NextObjectState(ZONE_OBJECT_INFO* aZoneObjectInfo, syncObjectState* &object_props);
	// Освободить данные об изменениях синхронизируемых свойств
	virtual void FreeObjectState(syncObjectState* &object_props);
	// Освободить данные об изменениях синхронизируемых свойств
	virtual void FreeCachedObjectState(syncObjectState* &object_props);
	// Положить описание объекта в очередь на создание
	bool PutMapObject( const ZONE_OBJECT_INFO& aZoneObjectInfo);
	// установить позицию аватара для сортировки очереди объектов по удаленности
	void SetAvatarPosition(const float x, const float y, char* locationName);

	// Взять описание объекта из очереди
	bool NextMapObject( ZONE_OBJECT_INFO*& aZoneObjectInfo);
	// Удалить объекты, у которых currentRealityID == auRealityID
	void DeleteObjectsExceptReality( unsigned int auRealityID);
	// Освободить данные описания объекта
	void FreeMapObject( ZONE_OBJECT_INFO*& aZoneObjectInfo);
	bool IsNewObjectWasAdded();
	bool IsObjectQueueEmpty();
	// Возвращает количество бижайщих объектов из очереди на создание
	int GetNearestMapObjectCount() const;

	// Флаг на асинхронный разбор больших списков объектов с сервера объектов,
	// чтобы ProcessSyncMessages() не выполнялся
	void SetStateToAnalysisObjectsList(bool state);
	bool GetStateToAnalysisObjectsList();

	// Передай состояние объекта
	virtual void SendObject( unsigned char* aData, unsigned int aDataSize);
	// Запроси состояние объекта
	virtual void QueryObject( unsigned int anObjectID, unsigned int aBornRealityID,unsigned int aRealityID, long long aCreatingTime, bool checkFrame=true);
	// Добавить запрос состояния объекта, который давно не получал обновление состояния
	virtual void AddQueryLatentedObject( unsigned int anObjectID, unsigned int aBornRealityID, unsigned int aCurrentRealityID, unsigned int auiHash);
	// Запроси состояния объектов, который давно не получал обновление состояния
	virtual void QueryLatentedObjects();
	// Передай синхронизируемое сообщение
	virtual void SendMessage( unsigned int anObjectID, unsigned int aBornRealityID, unsigned char* aData, unsigned short aDataSize);
	// создать сеанс воспроизведения лога по URL
	virtual omsresult createSeanceByURL(syncSeanceInfo& aSeanceInfo, unsigned int auIDSeanceRmmlObject);
	// Посылаем URL при телепортации на сервак для уточнения
	virtual omsresult PreciseURL(const wchar_t* apwcURL);
	// Начни запись
	virtual omsresult StartRecord(int aiRecordMode, const wchar_t* apwcRecordName, int& aiErrorCode);
	// Останови запись
	virtual omsresult StopRecord( unsigned int& aRecordID, int& aiErrorCode, syncRecordInfo** aRecordInfo);
	// Установить интерфейс, принимающий обратные вызовы от SyncManager-а
	virtual bool SetDefaultCallbacks(sync::syncISyncManCallbacks* apCallbacks, sync::syncIRecordManagerCallback* apRMCallback, sync::syncIEventManagerCallback* apEventManagerCallbacks);
	// изменить на сервере информацию о записи 
	virtual omsresult UpdateRecordInfo(syncRecordInfo& aRecordInfo);
	// получить набор записей (auID - некий идентификатор набора записей, который надо передать в callback)
	virtual omsresult GetRecords(unsigned int auID, int aiBegin, int aiCount, const wchar_t* apwcFilter, const wchar_t* apwcOrderBy, sync::syncIRecordManagerCallback * pCallback);
	// получить набор сеансов воспроизведения по конкретной записи лога
	virtual omsresult GetSeances(unsigned int auID, unsigned int auSeancesObjectID);
	// начать воспроизведение записи (отсылает запрос на сервер)
	virtual omsresult Play(unsigned int auID);
	// переместить воспроизведение записи (отсылает запрос на сервер)
	virtual omsresult Seek( unsigned long seekPos);
	// установить логин пользователя, управляющего воспроизведением записи
	virtual omsresult SetPilotLogin( const wchar_t* alpcPilotLogin, unsigned int auCurrentEventID);
	// получить информацию о сеансе
	virtual omsresult QueryFullSeanceInfo(unsigned int auIDSeanceRmmlObject, unsigned int auID);
	// разрешить/запретить пользователю посещать определенную локацию в определенной реальности
	virtual bool SetLocationEnteringPermissionForUser( const wchar_t* apwcUserLogin, const wchar_t* apwcLocationID, unsigned int auRealityID, bool abAllow);
	
	// ! jedi закомментировал так как на сервере обработка локации идет через свойства синхронизации аватара
	// Аватар сменил локацию
	//virtual omsresult AvatarChangeLocation(const wchar_t* apwcLocationName,  unsigned int auiObjectID, unsigned int auiBornRealityID);
	
	// выполняет обработку всех поступивших за кадр сообщений от сервера синхронизации, которые несут инфу о состояниях объектов
	virtual void ProcessSyncMessages();
	// удалить запись с таким идентификатором (отсылает запрос на сервер)
	virtual omsresult DeleteRecord(unsigned int auID);
	// удалить запись в корзину (отсылает запрос на сервер)
	virtual omsresult DeleteRecordIntoRecycleBin(unsigned int auID);
	// удалить записи (отсылает запрос на сервер)
	virtual omsresult DeleteRecords( unsigned int *pRecIDs, unsigned int count);
	// получаем список пользователей на сервере
	virtual omsresult GetUserList( unsigned int realityID);
	// остановить воспроизведение записи
	virtual omsresult Stop();
	// приостановить воспроизведение записи
	virtual omsresult Pause();
	// перемотать запись на начало
	virtual omsresult Rewind();
	// удалить объект
	virtual omsresult DeleteObject(unsigned int auID, unsigned int auRealityID);
	// получить статус объекта
	virtual omsresult GetObjectStatus(unsigned int auObjectID, unsigned int auBornRealityID);
	// клиент закрывается
	virtual omsresult CloseClient();
	virtual omsresult GetEventRecords(unsigned int auID, int aiBegin, int aiCount, const wchar_t* apwcFilter, const wchar_t* apwcOrderBy, sync::syncIRecordManagerCallback2 *pCallback);

	virtual omsresult Save( const wchar_t* apwcAuthor, const wchar_t* apwcCreationTime, const wchar_t* apwcName, const wchar_t* apwcDescription);

	// Операции по работе с редактором записей
	// вырезание сегмента записи
	virtual void RecordEditorCut(unsigned int startPos, unsigned int endPos);
	// вставка сегмента записи
	virtual void RecordEditorPaste(unsigned int pastePos);
	// копирование сегмента записи
	virtual void RecordEditorCopy(unsigned int startPos, unsigned int endPos);
	// удаление сегмента записи
	virtual void RecordEditorDelete(unsigned int startPos, unsigned int endPos);
	// отмена операции редактора записей (отсылает запрос на сервер)	
	virtual void RecordEditorUndo();
	// повтор операции редактора записей (отсылает запрос на сервер)	
	virtual void RecordEditorRedo();
	// открытие записи для редактирования (отсылает запрос на сервер)
	virtual void RecordEditorOpen(unsigned int aRecordID);
	// сохранение записи из редактора записей (отсылает запрос на сервер)
	virtual void RecordEditorSave();
	// сохранение записи под новым именем из редактора записей  (отсылает запрос на сервер)
	virtual void RecordEditorSaveAs(const wchar_t* apwcName, const wchar_t* apwcDescription, bool abOverwrite, bool abPublicate);
	// начать воспроизведение записи
	virtual void RecordEditorPlay();
	// приоставновить всопроизведение записи
	virtual void RecordEditorPause();
	// Перемотать вочпроизведние записи на начало
	virtual void RecordEditorRewind();
	// перемотать воспроизведение записи
	virtual void RecordEditorSeek(unsigned int aSeekPos);


	virtual void SetLogSyncMessage(bool bVal);

	// вернуть сессию для отправки сообщений. Временное решение
	virtual cs::iclientsession* GetServerSession();

	// код ошибки при отмены авторизации
	virtual void OnLogout( int iCode);

	virtual omsresult GetEventUserRight( unsigned int eventID);
	virtual omsresult SetEventUserRights( unsigned int eventID, wchar_t *apwcDescr);
	virtual omsresult SetEventUserDefaultRights( unsigned int eventID, unsigned int  aRight);
	virtual omsresult SetEventUserBan( unsigned int eventID, wchar_t *apwcDescr);

	// Отправляет на сервер пакет восстановления синхронизации после разрыва связи
	// В пакете отправляется последнее состояние аватар и желаемый URL для телепортации
	virtual void ConnectToWorld( const wchar_t *apwcRequestedUrl, const syncObjectState* mpcAvatarState);

	// получить хеш пакета состояния
	void LogBinSyncState( const syncObjectState& aProps);

	BEGIN_IN_MESSAGE_MAP()
		//IN_MESSAGE2_CHECKSUM( RT_UpdatePacket, OnUpdatePacket)
		IN_MESSAGE_ONE_OF_RANGE(RT_UpdatePacket, OnAnySyncMessageReceived)
		//IN_MESSAGE2( RT_BadUpdatePacket, OnBadUpdatePacket)
		IN_MESSAGE_ONE_OF_RANGE(RT_BadUpdatePacket, OnAnySyncMessageReceived)
		//IN_MESSAGE2( RT_DeleteObject, OnDeleteObject)
		IN_MESSAGE_ONE_OF_RANGE(RT_DeleteObject, OnAnySyncMessageReceived)
		//
		IN_MESSAGE2( RT_SyncMessage, OnMessage)
		IN_MESSAGE2( RT_ConnectRecord, OnRecordConnected)
		IN_MESSAGE2( RT_DisconnectRecord, OnRecordDisconnected)
		IN_MESSAGE2( RT_StopRecord, OnRecordStoped)
		IN_MESSAGE2( RT_LogRecordsSend, OnGettingLogRecords)
		IN_MESSAGE2( RT_SeanceFinished, OnSeanceFinished)
		IN_MESSAGE2( RT_LogSeanceCreated, OnLogSeanceCreated)
		IN_MESSAGE2( RT_ReceivedSeances, OnReceivedSeances)
		IN_MESSAGE2( RT_ReceivedFullSeanceInfo, OnReceivedFullSeanceInfo)
		IN_MESSAGE2( RT_ReceivedObjectStatus, OnReceivedObjectStatus)
		IN_MESSAGE2( RT_SeancePaused, OnSeancePaused)
		IN_MESSAGE2( RT_SessionCheck, OnSessionCheck)
		IN_MESSAGE2( RT_SeanceLoaded, OnSeanceLoaded)
		IN_MESSAGE2( RT_UpdatePlayingTime, OnUpdatePlayingTime)
		IN_MESSAGE2( RT_SeancePlayed, OnSeancePlayed)
		IN_MESSAGE2( RT_SeanceRewinded, OnSeanceRewinded)
		IN_MESSAGE2( RT_SeekPos, OnSeanceSeekPos)
		IN_MESSAGE2( RT_SeanceFinishedPlaying, OnSeanceFinishedPlaying)
		IN_MESSAGE2( RT_ReceivedPrecizeURL, OnReceivedPrecizeURL)
		IN_MESSAGE2( RT_SeanceLeaved, OnSeanceLeaved)
		IN_MESSAGE2( RT_UserListReceived, OnUserListReceived)
		IN_MESSAGE2( RT_ServerError, OnServerErrorReceived)
		IN_MESSAGE2( RT_EventEntered, OnEventEntered)
		IN_MESSAGE2( RT_EventLeaved, OnEventLeaved)
		IN_MESSAGE2( RT_EventUpdateInfo, OnEventUpdateInfo)
		IN_MESSAGE2( RT_ObjectCreationResult, OnObjectCreationResult)
		IN_MESSAGE2( RT_EventRecordsSend, OnGettingEventRecords)
		IN_MESSAGE2( RT_ReceivedSeancesInfo, OnReceivedSeancesInfo)
		IN_MESSAGE2( RT_ReceivedEventUserRightsInfo, OnReceivedEventUserRightsInfo)
		//IN_MESSAGE2( RT_ReceivedEventUserRolesUpdate, OnReceivedEventUserRolesUpdate)	
		IN_MESSAGE2( RT_ReceivedEventUserRightUpdate, OnReceivedEventUserRightUpdate)	
		IN_MESSAGE2( RT_ReceivedEventUserBanUpdate, OnReceivedEventUserBanUpdate)
		IN_MESSAGE2( RT_ErrorCreateTempLocation, OnErrorCreateTempLocation)
		IN_MESSAGE2( RT_ReceivedEventsInfoIntoLocation, OnReceivedEventsInfoIntoLocation)
		IN_MESSAGE2( RT_LocationEntered, OnLocationEntered)
		IN_MESSAGE2( RT_InfoMessage, OnInfoMessage)
		////////////////////////////////////////////////////////////////////////////////////////
		// Сообщения для RecordEditor
		////////////////////////////////////////////////////////////////////////////////////////
		IN_MESSAGE2( RT_RecordEditorCutComplete, OnRecordEditorCutComplete)
		IN_MESSAGE2( RT_RecordEditorCopyComplete, OnRecordEditorCopyComplete)
		IN_MESSAGE2( RT_RecordEditorPasteComplete, OnRecordEditorPasteComplete)
		IN_MESSAGE2( RT_RecordEditorDeleteComplete, OnRecordEditorDeleteComplete)
		IN_MESSAGE2( RT_RecordEditorSaveComplete, OnRecordEditorSaveComplete)
		IN_MESSAGE2( RT_RecordEditorSaveAsComplete, OnRecordEditorSaveAsComplete)
		IN_MESSAGE2( RT_RecordEditorPlayComplete, OnRecordEditorPlayComplete)
		IN_MESSAGE2( RT_RecordEditorPauseComplete, OnRecordEditorPauseComplete)
		IN_MESSAGE2( RT_RecordEditorRewindComplete, OnRecordEditorRewindComplete)
		IN_MESSAGE2( RT_RecordEditorSeekComplete, OnRecordEditorSeekComplete)
		IN_MESSAGE2( RT_RecordEditorUndoComplete, OnRecordEditorUndoComplete)
		IN_MESSAGE2( RT_RecordEditorRedoComplete, OnRecordEditorRedoComplete)
		IN_MESSAGE2( RT_RecordEditorOpenComplete, OnRecordEditorOpenComplete)
		////////////////////////////////////////////////////////////////////////////////////////
		// Сообщения для MapSessionWrapper
		////////////////////////////////////////////////////////////////////////////////////////
		IN_MESSAGE_ONE_OF_RANGE(MRT_AVATAR_DESC_QUERY_ID, OnAnySyncMessageReceived)
		IN_MESSAGE_ONE_OF_RANGE(MRT_ZONES_DESC_QUERY_ID, OnAnySyncMessageReceived)
		IN_MESSAGE_ONE_OF_RANGE(MRT_ZONES_OBJECTS_DESC_QUERY_ID, OnAnySyncMessageReceived)
		IN_MESSAGE_ONE_OF_RANGE(MRT_OBJECTS_DESC_QUERY_ID, OnAnySyncMessageReceived)
		IN_MESSAGE_ONE_OF_RANGE(MRT_PORTALS_STATES_QUERY_ID, OnAnySyncMessageReceived)
		IN_MESSAGE_ONE_OF_RANGE(MRT_OBJECT_DESC_BY_NAME_QUERY_ID, OnAnySyncMessageReceived)
		IN_MESSAGE_ONE_OF_RANGE(MRT_OBJECT_UPDATE_QUERY_ID, OnAnySyncMessageReceived)
		IN_MESSAGE_ONE_OF_RANGE(MRT_REALITY_DESC_QUERY_ID, OnAnySyncMessageReceived)
		IN_MESSAGE_ONE_OF_RANGE(MRT_OBJECT_STATUS_QUERY_ID, OnAnySyncMessageReceived)
		IN_MESSAGE_ONE_OF_RANGE(MRT_CREATE_LOCATION_QUERY_ID, OnAnySyncMessageReceived)
		IN_MESSAGE_ONE_OF_RANGE(MRT_DELETE_OBJECT_QUERY_ID, OnAnySyncMessageReceived)
	END_IN_MESSAGE_MAP()

	void OnConnectLost();
	void OnConnectRestored();

	void OnLocationsListChanged();

	void CheckHashAvatarsThread();
	void CheckHashObjectsThread();

protected:
	omsContext* pContext;
	cs::iclientsession* pClientSession;
	unsigned int iRecordID;
	sync::syncISyncManCallbacks* pCallbacks;
	sync::syncIRecordManagerCallback * pRMCallBack;
	sync::syncIEventManagerCallback* pEventManagerCallbacks;
	unsigned int IDLogObject;
	syncRecordInfo oRecordInfo;
	//bool bCheckHash;
	//HANDLE eventCheckHash;
	// храним инфу о сеансе

protected:
	void onQueryComplete(int aiID, const wchar_t* apwcResponse=NULL, int* aiErrorCode=NULL);

	void OnUpdatePacket( BYTE* aData, int aDataSize, DWORD aCheckSum);
	void OnBadUpdatePacket( BYTE* aData, int aDataSize);
	void OnDeleteObject( BYTE* aData, int aDataSize);
	void OnMessage( BYTE* aData, int aDataSize);
	void OnRecordConnected( BYTE* aData, int aDataSize);
	void OnRecordDisconnected( BYTE* aData, int aDataSize);
	void OnRecordStoped( BYTE* aData, int aDataSize);
	void OnUpdatePlayingTime( BYTE* aData, int aDataSize);
	void OnGettingLogRecords( BYTE* aData, int aDataSize);
	void OnSeanceFinished( BYTE* aData, int aDataSize);
	void OnLogSeanceCreated( BYTE* aData, int aDataSize);
	void OnReceivedSeances( BYTE* aData, int aDataSize);
	void OnReceivedFullSeanceInfo( BYTE* aData, int aDataSize);
	void OnReceivedObjectStatus( BYTE* aData, int aDataSize);
	void OnSeancePaused( BYTE* aData, int aDataSize);
	void OnSessionCheck( BYTE* aData, int aDataSize);
	void OnSeanceLoaded( BYTE* aData, int aDataSize);
	void OnSeancePlayed( BYTE* aData, int aDataSize);
	void OnSeanceRewinded( BYTE* aData, int aDataSize);
	void OnSeanceSeekPos( BYTE* aData, int aDataSize);
	void OnSeanceFinishedPlaying( BYTE* aData, int aDataSize);
	void OnReceivedPrecizeURL( BYTE* aData, int aDataSize);
	void OnSeanceLeaved( BYTE* aData, int aDataSize);
	void OnUserListReceived( BYTE* aData, int aDataSize);
	void OnServerErrorReceived( BYTE* aData, int aDataSize);
	void OnEventEntered( BYTE* aData, int aDataSize);
	void OnEventUpdateInfo( BYTE* aData, int aDataSize);
	void OnEventLeaved( BYTE* aData, int aDataSize);
	void OnObjectCreationResult( BYTE* aData, int aDataSize);
	void OnGettingEventRecords( BYTE* aData, int aDataSize);
	void OnReceivedSeancesInfo( BYTE* aData, int aDataSize);
	void OnReceivedEventsInfoIntoLocation( BYTE* aData, int aDataSize);
	void OnLocationEntered( BYTE* aData, int aDataSize);
	void OnInfoMessage( BYTE* aData, int aDataSize);
	void OnReceivedEventUserRightsInfo( BYTE* aData, int aDataSize);	
	void OnReceivedEventUserRolesUpdate( BYTE* aData, int aDataSize);
	void OnReceivedEventUserRightUpdate( BYTE* aData, int aDataSize);	
	void OnReceivedEventUserBanUpdate( BYTE* aData, int aDataSize);
	void OnErrorCreateTempLocation( BYTE* aData, int aDataSize);

	////////////////////////////////////////////////////////////////////////////////////////
	// Сообщения для recordEditor
	////////////////////////////////////////////////////////////////////////////////////////
	void OnRecordEditorCutComplete( BYTE* aData, int aDataSize);
	void OnRecordEditorCopyComplete( BYTE* aData, int aDataSize);
	void OnRecordEditorPasteComplete( BYTE* aData, int aDataSize);
	void OnRecordEditorDeleteComplete( BYTE* aData, int aDataSize);
	void OnRecordEditorSaveComplete( BYTE* aData, int aDataSize);
	void OnRecordEditorSaveAsComplete( BYTE* aData, int aDataSize);
	void OnRecordEditorPlayComplete( BYTE* aData, int aDataSize);
	void OnRecordEditorPauseComplete( BYTE* aData, int aDataSize);
	void OnRecordEditorRewindComplete( BYTE* aData, int aDataSize);
	void OnRecordEditorSeekComplete( BYTE* aData, int aDataSize);
	void OnRecordEditorUndoComplete( BYTE* aData, int aDataSize);
	void OnRecordEditorRedoComplete( BYTE* aData, int aDataSize);
	void OnRecordEditorOpenComplete( BYTE* aData, int aDataSize);
	
	////////////////////////////////////////////////////////////////////////////////////////
	// Сообщения для MapSessionWrapper
	////////////////////////////////////////////////////////////////////////////////////////
	void OnAnySyncMessageReceived( BYTE aMessageID, BYTE* aData, int aDataSize, int aCheckSum);

private:
	syncObjectState* CreateSyncObjectState( BYTE* aData, int aDataSize, DWORD aCheckSum, std::string& anErrorText);
	bool PutSyncData( BYTE* aData, int aDataSize, DWORD aCheckSum, std::string& anErrorText);
	void PushObjectStateToQueue( syncObjectState* aObjectState);
	syncObjectState* CreateNotFoundSyncObject( unsigned int anObjectID, unsigned int aBornRealityID, unsigned int aRealityID);
	void PutNotFoundSyncObject( unsigned int anObjectID, unsigned int aBornRealityID, unsigned int aRealityID);
	bool NextMapObject( ZONE_OBJECT_INFO*& aZoneObjectInfo, ZONE_OBJECT_INFO* aFirst);

private:
	void syncObjectSysPropsToJsonValue( Json::Value &root, mlSysSynchProps *sys_props);
	void syncObjectPropsToJsonValue( Json::Value &root, std::vector<syncObjectProp>::iterator &iter, int index);
	void syncObjectStateToJsonValue( Json::Value &root, syncObjectState *object_props);
	void writeLog( const char* alpcLog);

	int syncDataLogWriteThread();
	static DWORD WINAPI syncDataLogWriteThread_(LPVOID param);

	void GetLanguage( CWComString &sLang);


private:
	bool IsReady();
	bool IsConnected();

private:
	CRITICAL_SECTION mutex;
	CRITICAL_SECTION syncMessageCS;

	// Очередь обновления состояния объектов
	CObjectStateQueue objectStateQueue;

	// Очередь описания объектов
	CMapObjectQueue mapObjectQueue;

	// 
	FILE *fpSynchLog;
	HANDLE hSendEvent;
	HANDLE hSyncDataLogThread;
	MP_LIST< std::string> oSyncSendQueue;
	// Критическая секция на изменения oSyncSendQueue
	CRITICAL_SECTION csSyncSendQueue;
	bool bSyncLog;
	bool m_isNewObjectAdded;

	bool m_isAsynchAnalysisObjectsList;

	MP_MAP<std::string, int> m_classLoaded;

	// Очередь событий синхронизации. Нужна для правильной очередности обработки событий в rmml и RM
	typedef struct _ReceivedMessage
	{
		unsigned int		uiMessageID;
		unsigned int		uiCheckSum;
		SyncManProtocol::CTempPacketIn*	pCommonPacketIn;
	} ReceivedMessage;
	typedef MP_VECTOR<ReceivedMessage>	VecReceivedMessage;
	typedef VecReceivedMessage::iterator	VecReceivedMessageIt;
	VecReceivedMessage		m_receivedMessages;
};
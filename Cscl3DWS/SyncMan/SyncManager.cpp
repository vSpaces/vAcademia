#include "StdAfx.h"

#include "SyncManager.h"

#include "atltime.h"
#include "UrlParser.h"

#include "SyncTempPacketIn.h"
#include "QueryObjectOut.h"
#include "SyncMessageOut.h"
#include "StartRecordOut.h"
#include "StopRecordOut.h"
#include "QueryLogRecordsOut.h"
#include "PlayOut.h"
#include "SeekOut.h"
#include "SetPilotLoginOut.h"
#include "SaveOut.h"
#include "DeleteRecordSegmentOut.h"
#include "CreateSeanceOut.h"
#include "GetSeancesOut.h"
#include "DeleteRecordOut.h"
#include "UpdateRecordInfoOut.h"
#include "DeleteObjectOut.h"
#include "GetObjectStatusOut.h"
#include "QueryFullSeanceInfoOut.h"
#include "PreciseURLOut.h"
#include "ReceivedPrecisedURLIn.h"
#include "AvatarChangeLocationOut.h"
#include "SendObjectOut.h"
#include "QueryEventRecordsOut.h"
#include "QueryLatentedObjectsOut.h"
#include "EventUserRightsUpdateOut.h"
#include "EventUserBanUpdateOut.h"
#include "EventUserDefaultRightsOut.h"
#include "EventGetUserRightOut.h"
#include "RestoreSyncronizationOut.h"
#include "RecordEditorCutOut.h"
#include "RecordEditorCopyOut.h"
#include "RecordEditorPasteOut.h"
#include "RecordEditorDeleteOut.h"
#include "RecordEditorSaveAsOut.h"
#include "RecordEditorSeekOut.h"
#include "RecordEditorOpenOut.h"
#include "SetLocationEnteringPermissionForUserQueryOut.h"

#include "RecordStartedIn.h"
#include "RecordDisconnectedIn.h"
#include "RecordStopedIn.h"
#include "DeleteObjectIn.h"
#include "SyncMessageIn.h"
#include "PrepareToPlayIn.h"
#include "PlayerClosedIn.h"
#include "GettingLogRecordsIn.h"
#include "ReceiveBadSyncDataIn.h"
#include "ReceivedSeancesIn.h"
#include "ReceivedSeanceInfoIn.h"
#include "LogSeanceCreatedIn.h"
#include "ReceivedObjectStatusIn.h"
#include "SeancePausedIn.h"
#include "SeanceLoadedIn.h"
#include "UpdatePlayingTimeIn.h"
#include "PrecisedURLIn.h"
#include "LeavedSeanceIn.h"
#include "UpdatePacketIn.h"
#include "DeleteRecordsOut.h"
#include "GetUserListOut.h"
#include "GetUserListIn.h"
#include "ServerErrorIn.h"
#include "EventNotifyIn.h"
#include "EventLeavedNotifyIn.h"
#include "ObjectCreateResultQueryIn.h"
#include "DeleteRecordIntoRecycleBinOut.h"
#include "GettingEventRecordsIn.h"
#include "SeancesInfoIn.h"
#include "EventUserRightsInfoIn.h"
#include "EventUserRightUpdateIn.h"
#include "EventUserBanUpdateIn.h"
#include "ErrorCreateTempLocationIn.h"
#include "EventsInfoIntoLocationIn.h"
#include "LocationEnteredIn.h"

#include "RecordEditorCutIn.h"
#include "RecordEditorCopyIn.h"
#include "RecordEditorPasteIn.h"
#include "RecordEditorDeleteIn.h"
#include "RecordEditorSaveIn.h"
#include "RecordEditorSaveAsIn.h"
#include "RecordEditorPlayIn.h"
#include "RecordEditorPauseIn.h"
#include "RecordEditorRewindIn.h"
#include "RecordEditorSeekIn.h"
#include "RecordEditorUndoIn.h"
#include "RecordEditorRedoIn.h"
#include "RecordEditorOpenIn.h"

#include "InfoMessageIn.h"

#include "ILogWriter.h"
#include "ZLIBENGN.H"
#include "IRecordEditor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "DataBuffer.h"
#include "mlSynchState.h"
#include "rmml.h"
#include "CrashHandlerInThread.h"

using namespace rmml;

// Send Type - типы посылаемых сообщений
#define ST_SyncData           1  // Изменил состояние объекта
#define ST_QueryLatentedObjectsState	2	// Проверь состояния объектов, который у меня давно не получал обновление
#define ST_QueryObjectState   3  // Дай состояние объекта
#define ST_SyncMessage        4  // Отправил сообщение владельцу объекта
#define ST_StartRecord        5  // Начни запись
#define ST_StopRecord         6  // Закончи запись
#define ST_QueryLogRecords    7  // Получить записи
#define ST_Play               8  // Начни воспроизведение записи
#define ST_Stop               9  // Останови воспроизведение записи
#define ST_Delete             10 // удаление записи навсегда
#define ST_UpdateRecord       11 // Обновление информации по записи
#define ST_GetSeances		  13 // получить список сеансов с сервера
#define ST_Rewind             14  // перемотка
#define ST_QueryFullSeanceInfo 15  // получить полную инфу о сеансе по id
#define ST_DeleteObject       16 // удалить объект
#define ST_GetSeanceByURL     17 // получить сеанс по URL
#define ST_GetObjectStatus    18 // получить статус объекта
#define ST_Pause			  19	// пауза
#define ST_ClientClose		  20	// клиент закрывается
#define ST_SessionCheckAnswer 21	// ответ на запрос сервака на валидность сессии
#define ST_SetSessionDebugMode	22	// устанавливает на серваке режим отладки, в котором сервер не будет посылать оповещения на клиент о валидности сессиии
#define ST_AvatarChangeLocation	23	// Передает на сервак инфу о том что аватар поменял локацию
#define ST_PreciseURL 			25	// Посылаем URL на сервер при телепортации
//#define ST_CreateLesson 		26	// Создаем занятие на сервере
#define ST_DeleteRecords        27 // Удалить записи
#define ST_GetUserList	        28 // Запрашиваем список пользователей
#define ST_Seek					29  // перемотка воспроизведения записи
#define ST_DeleteRecordSegment	30  // удаление куска записи - не используется
#define ST_RecordSave			31  // сохранение записи  - не используется
#define ST_RecordEditorUndo		32  // отмена операции редактора записей
#define ST_RecordEditorRedo		33  // повтор операции редактора записей
#define ST_SetPilotLogin		34  // установить логин пользователя, управляющего воспроизведением записи
#define ST_DeleteIntoRecycleBin	35 // удаление записи в корзину
#define ST_QueryEventRecords	36  // Получить записи cо всеми их событиями
#define ST_EventUserDefaultRightsUpdate 40 // изменены права по умолчании пользователей в событии
#define ST_EventUserRightsUpdate 41 // изменены права пользователей в событии
#define ST_EventUserBanUpdate	42 // пользователь забанен в событии
#define ST_EventGetUserRight	43  // запросить свои права в событии
#define ST_RecordEditorCut		44  // операция редактора записей - вырезать кусок записи
#define ST_RecordEditorCopy		45  // операция редактора записей - скопировать кусок записи
#define ST_RecordEditorPaste	46  // операция редактора записей - вставить кусок записи
#define ST_RecordEditorDelete	47  // операция редактора записей - удалить кусок записи
#define ST_RecordEditorSave		48  // операция редактора записей - сохранить изменения в записи
#define ST_RecordEditorSaveAs	49  // операция редактора записей - созхранить измененную запись как новую
#define ST_RecordEditorPlay		50  // операция редактора записей - начать воспроизведение записи
#define ST_RecordEditorPause	51  // операция редактора записей - приоставновить воспроизведние записи
#define ST_RecordEditorRewind	52  // операция редактора записей - пермотать воспроизведение записи на начало
#define ST_RecordEditorSeek		53  // операция редактора записей - перемотать воспроизведение записи в нужную позицию
#define ST_RecordEditorOpen		54  // операция редактора записей - открыть запись для редактирования
#define ST_ConnectToWorld		55  // запросить свои права в событии
#define ST_SetLocationEnteringPermission		56	// разрешить/запретить пользователю посещать определенную локацию в определенной реальности

#define QUEUE_SIZE_TO_LOG				1000

using namespace SyncManProtocol;

int CSyncManager :: iWrongSynchPacket = 0;
CQueryLatentedObjectsOut *pQueryLatentedObjectsOut = NULL;

CSyncManager::CSyncManager( omsContext* aContext)
:MP_LIST_INIT(oSyncSendQueue),
MP_MAP_INIT(m_classLoaded),
MP_VECTOR_INIT(m_receivedMessages)
{
	m_isNewObjectAdded = false;
	bSyncLog = false;
	pContext = aContext;
	if (pContext != NULL)
		pContext->mpSyncMan = this;
	pClientSession = NULL;
	pCallbacks = NULL;
	pEventManagerCallbacks = NULL;
	IDLogObject = -1;
	fpSynchLog = NULL;
	InitializeCriticalSection(&mutex);
	InitializeCriticalSection(&syncMessageCS);
	InitializeCriticalSection(&csSyncSendQueue);
	hSendEvent = CreateEvent( NULL, TRUE, FALSE, NULL);
	
	CSimpleThreadStarter syncThreadStarter(__FUNCTION__);
	syncThreadStarter.SetRoutine(syncDataLogWriteThread_);
	syncThreadStarter.SetParameter(this);
	hSyncDataLogThread = syncThreadStarter.Start();	

	SetThreadPriority(hSyncDataLogThread, THREAD_PRIORITY_LOWEST);

	mapObjectQueue.SetLogWriter( aContext->mpLogWriter);
	mapObjectQueue.SetContext(aContext);
	objectStateQueue.SetLogWriter( aContext->mpLogWriter);

	m_isAsynchAnalysisObjectsList = false;
}

CSyncManager::~CSyncManager()
{
	Disconnect();
	//bCheckHash = false;
	//SetEvent( eventCheckHash);	
	SetEvent( hSendEvent);
	WaitForSingleObject( hSyncDataLogThread, 2000);
	CloseHandle(hSyncDataLogThread);
	hSyncDataLogThread = NULL;
	EnterCriticalSection(&csSyncSendQueue);
	if(fpSynchLog!=NULL)
	{
		fprintf(fpSynchLog, "%s", "]");
		fclose(fpSynchLog);
	}
	LeaveCriticalSection(&csSyncSendQueue);
	DeleteCriticalSection(&csSyncSendQueue);
	DeleteCriticalSection(&mutex);
	DeleteCriticalSection(&syncMessageCS);
}

void CSyncManager::Connect()
{
	if (pContext == NULL || pContext->mpComMan == NULL)
		return;

	pContext->mpComMan->CreateSyncServerSession( &pClientSession);
	if (pClientSession == NULL)
		return;

	pClientSession->SetMessageHandler( this);
	pClientSession->SetSessionListener( this);
#ifdef _DEBUG
	pClientSession->Send( ST_SetSessionDebugMode, NULL, 0);
#endif
}

void CSyncManager::Disconnect()
{
	if (pContext == NULL || pContext->mpComMan == NULL || pClientSession == NULL)
		return;

	pContext->mpComMan->DestroyServerSession( &pClientSession);
	pClientSession = NULL;
}

void CSyncManager::Release()
{
	MP_DELETE_THIS;
}

bool CSyncManager::IsReady()
{
	return (pClientSession != NULL);
}

// проверяем есть ли соединение с сервером
bool CSyncManager::IsConnected()
{
	return (IsReady() && pClientSession->GetStatus() == 1);
}

void CSyncManager::SetLogSyncMessage(bool abVal)
{
	bSyncLog = abVal;
}

void CSyncManager::syncObjectSysPropsToJsonValue( Json::Value &root, mlSysSynchProps *sys_props)
{
	Json::Value sysprops(Json::objectValue);
	Json::Value &value_mwVersion = sysprops["mwServerVersion"];
	if(sys_props->miSet){
		if(sys_props->miSet & MLSYNCH_FLAGS_SET_MASK)
		{	
			Json::Value &value_miFlags = sysprops["miFlags"];
			value_miFlags = sys_props->miFlags;
		}
		if(sys_props->miSet & MLSYNCH_OWNER_CHANGED_MASK)
		{	
			Json::Value &value_msOwner = sysprops["msOwner"];
			USES_CONVERSION;
			value_msOwner = W2A(sys_props->msOwner.c_str());
		}
		if(sys_props->miSet & MLSYNCH_ZONE_CHANGED_MASK)
		{
			Json::Value &value_miZone = sysprops["miZone"];
			value_miZone = sys_props->miZone;
		}
		if(sys_props->miSet & MLSYNCH_REALITY_SET_MASK)
		{
			Json::Value &value_muReality = sysprops["muReality"];
			value_muReality = sys_props->muReality;
		}

		if(sys_props->miSet & MLSYNCH_COORDINATE_SET_MASK)
		{
			Json::Value &value_mfCoordinateZ = sysprops["mfCoordinateZ"];
			value_mfCoordinateZ = sys_props->mfCoordinateZ;
		}
		if (sys_props->miSet & MLSYNCH_COORDINATE_XY_SET_MASK)
		{
			Json::Value &value_mfCoordinateX = sysprops["mfCoordinateX"];
			value_mfCoordinateX = sys_props->mfCoordinateX;
			Json::Value &value_mfCoordinateY = sysprops["mfCoordinateY"];
			value_mfCoordinateY = sys_props->mfCoordinateY;
		}
		if(sys_props->miSet & MLSYNCH_SERVERTIME_SET_MASK)
		{
			Json::Value &value_serverTime = sysprops["serverTime"];
			value_serverTime = sys_props->serverTime;
		}
		if (sys_props->miSet & MLSYNCH_BIN_STATE_CHANGED_MASK)
		{
			char binStateString[50];
			binStateString[0] = '\0';
			rtl_sprintf(binStateString, sizeof(binStateString), "version( %u), size( %u)", sys_props->mBinState.GetVersion(), sys_props->mBinState.GetSize());
			Json::Value &value_binState = sysprops["binState"];
			value_binState = binStateString;
		}
		if(sys_props->miSet & MLSYNCH_LOCATION_CHANGED_MASK)
		{	
			Json::Value &value_msLocation = sysprops["msLocation"];
			USES_CONVERSION;
			value_msLocation = W2A(sys_props->msLocation.c_str());
		}
		if(sys_props->miSet & MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK)
		{	
			Json::Value &value_msCommunicationArea = sysprops["msCommunicationArea"];
			USES_CONVERSION;
			value_msCommunicationArea = W2A(sys_props->msCommunicationArea.c_str());
		}
	}
	Json::Value &value_sys_props = root[ "sys_props" ];
	value_sys_props = sysprops;
}

void CSyncManager::syncObjectPropsToJsonValue( Json::Value &root, std::vector<syncObjectProp>::iterator &iter, int index)
{
	Json::Value props(Json::objectValue);

	Json::Value &value_name = props["name"];
	value_name = iter->name;
	Json::Value &value_binValue = props["binValue"];
	for(unsigned int i=0;i<iter->binValue.size();i++)
	{
		value_binValue[i]=iter->binValue[i];
	}
	Json::Value &value_ai = root[ index ];
	value_ai = props;
}

void CSyncManager::syncObjectStateToJsonValue( Json::Value &state, syncObjectState *object_props)
{
	//Json::Value state(Json::objectValue);
	//object_props->string_id
	Json::Value &value_T = state[ "type" ];
	value_T = object_props->type;
	Json::Value &value_uiID = state[ "uiID" ];
	value_uiID = object_props->uiID;
	Json::Value &value_uiBornRealityID = state[ "uiBornRealityID" ];
	value_uiBornRealityID = object_props->uiBornRealityID;
	Json::Value &value_uiRealityID = state[ "uiRealityID" ];
	value_uiRealityID = object_props->uiRealityID;
	Json::Value &value_string_id = state[ "string_id" ];
	value_string_id = object_props->string_id;
	Json::Value &value_creation_info = state[ "creation_info" ];
	USES_CONVERSION;
	value_creation_info = W2A(object_props->creation_info.c_str());

	syncObjectSysPropsToJsonValue(state, &object_props->sys_props);
	
	Json::Value objectProps(Json::arrayValue);
	std::vector<syncObjectProp>::iterator iterP = object_props->props.begin();
	int index = 0;
	for(index=0;iterP!=object_props->props.end();iterP++,index++)
		syncObjectPropsToJsonValue(objectProps, iterP, index);

	if(object_props->props.size()>0)
	{
		Json::Value &value_props = state[ "props" ];
		value_props = objectProps;
	}
	
	Json::Value sub_objects(Json::arrayValue);
	//std::vector<syncObjectState>::iterator iterS = object_props->sub_objects.begin();
	for(unsigned int i=0;i<object_props->sub_objects.size();i++)
	{
		Json::Value &value_ai = sub_objects[ index ];
		syncObjectStateToJsonValue(value_ai, &object_props->sub_objects[i]);
	}
	if(object_props->sub_objects.size()>0)
	{
		Json::Value &value_sub_objects = state[ "sub_objects" ];
		value_sub_objects = sub_objects;
	}
}

int  CSyncManager::syncDataLogWriteThread()
{
	while( true && WaitForSingleObject( hSendEvent, INFINITE) != WAIT_FAILED)
	{
		if(!bSyncLog)
			return 0;
		EnterCriticalSection( &csSyncSendQueue);
		if(fpSynchLog == NULL)
		{
			CComString filePath = pContext->mpLogWriter->GetLogFilePath().c_str();
			filePath.Replace("\\", "/");
			//int pos = filePath.ReverseFind('/');
			int pos = filePath.Find("main.log", 0);
			filePath = filePath.Left(pos+1);
			filePath += "sync.log";
			//fpSynchLog = fopen(filePath, "wt");
			fopen_s(&fpSynchLog, filePath, "w+");
			fprintf(fpSynchLog, "%s", "[\n");
			fflush(fpSynchLog);
		}
		if (oSyncSendQueue.empty())
		{
			fprintf(fpSynchLog, "%s", "],");
			fclose(fpSynchLog);
			fpSynchLog = NULL;
			LeaveCriticalSection( &csSyncSendQueue);
			break;
		}
		std::string it = (*oSyncSendQueue.begin());
		oSyncSendQueue.pop_front();
		LeaveCriticalSection( &csSyncSendQueue);
		//_write(fpSynchLog->_file, it.c_str(), it.size());
		fprintf(fpSynchLog, "%s", it.c_str());
		fflush(fpSynchLog);
		ResetEvent( hSendEvent);
		//fprintf(fp, "%s", )
		//fwrite(s.c_str(), 1, s.size(), fpSynchLog);
	}
	return 0;
}

DWORD WINAPI CSyncManager::syncDataLogWriteThread_(LPVOID param)
{
	CSyncManager* aSyncManager = (CSyncManager*)param;
	return aSyncManager->syncDataLogWriteThread();
}

/*unsigned int getHash(const byte *s, unsigned int)
{
unsigned byte* curr = s;
unsigned int count = 0x7654321**curr;
while(*curr)
{
//count += 37 * count + *curr;			
count = 13 * count + *curr;
curr++;
}
//count = count * 19L + 12451L % 8882693L;
return count;
}*/

#define get16bits(d) (*((const unsigned short *) (d)))

unsigned int SuperFastHash( unsigned char * data, unsigned int len)
{
	unsigned int hash = len, tmp;
	int rem;


	if (len <= 0 || data == NULL) return 0;

	rem = len & 3;
	len >>= 2;

	/* Main loop */
	for (; len > 0; len--)
	{
		hash  += get16bits (data);
		tmp    = (get16bits (data + 2) << 11) ^ hash;
		hash   = (hash << 16) ^ tmp;
		data  += 4;
		hash  += hash >> 11;
	}

	/* Handle end cases */
	switch (rem)
	{
	case 3:
		hash += get16bits (data);
		hash ^= hash << 16;
		hash ^= data[2] << 18;
		hash += hash >> 11;
		break;
	case 2:
		hash += get16bits (data);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1:
		hash += *data;
		hash ^= hash << 10;
		hash += hash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

void CSyncManager::LogBinSyncState( const syncObjectState& aProps)
{
	CSendObjectOut aPacket;	
	aPacket.Create((syncObjectState&)aProps);

	CLogValue log("Hash packet: ", aPacket.GetData(), aPacket.GetDataSize());
	pContext->mpLogWriter->WriteLn( log);
	CLogValue log2("ClientVersion : ", aProps.mwClientVersion, ", serverVersion : ", aProps.mwServerVersion);
	pContext->mpLogWriter->WriteLn( log2);
}

unsigned int CSyncManager::GetHashObj( const syncObjectState& object_all_props, bool abLogPacket)
{
	CSendObjectOut packetForHash;	
	packetForHash.Create((syncObjectState&)object_all_props);

#ifdef DEBUG
	/*if( abLogPacket)
		LogBinSyncState( object_all_props, packetForHash);*/
#endif

	return SuperFastHash( packetForHash.GetData(), packetForHash.GetDataSize());
}

// Отослать на сервер данные об изменении синхронизируемых свойств объекта
void CSyncManager::SendObjectState( const syncObjectState& object_props)
{	
	if(!IsConnected())
		return;

	EnterCriticalSection(&mutex);
	CSendObjectOut packetOut;
	packetOut.Create((syncObjectState&)object_props);
	pClientSession->Send( ST_SyncData, packetOut.GetData(), packetOut.GetDataSize());
	if(bSyncLog)
	{
		Json::StyledWriter styledWriter;
		Json::Value root(Json::objectValue);
		Json::Value &value_time = root["time"];
		value_time = (unsigned int)GetTickCount();
		Json::Value &state = root["state"];
		//Json::Value state(Json::objectValue);
		syncObjectStateToJsonValue(state, (syncObjectState*)&object_props);
		//root["state"] = state;
		std::string s = styledWriter.write(root, true);
		EnterCriticalSection( &csSyncSendQueue);
		oSyncSendQueue.push_back( s);
		LeaveCriticalSection( &csSyncSendQueue);
		if(hSendEvent!=NULL)
			SetEvent( hSendEvent);
	}
	LeaveCriticalSection(&mutex);
}

// получить данные об изменениях синхронизируемых свойств
bool CSyncManager::NextObjectState( syncObjectState* &object_props)
{
	return objectStateQueue.Pop( object_props);
}

// Получить следующее изменение состояния для конкретного синхронизируемого объекта
bool CSyncManager::NextObjectState(ZONE_OBJECT_INFO* aZoneObjectInfo, syncObjectState* &object_props)
{
	return mapObjectQueue.NextObjectState( aZoneObjectInfo, object_props);
}

// освободить данные об изменениях синхронизируемых свойств
void CSyncManager::FreeObjectState(syncObjectState* &object_props)
{
	objectStateQueue.DeleteObjectState( object_props);
}

// Освободить данные об изменениях синхронизируемых свойств
void CSyncManager::FreeCachedObjectState(syncObjectState* &object_props)
{
	mapObjectQueue.FreeCachedObjectState(object_props);
}

syncObjectState* CSyncManager::CreateSyncObjectState( BYTE* aData, int aDataSize, DWORD aCheckSum, std::string& anErrorText)
{
	CUpdatePacketIn updatePacketIn(aData, aDataSize);
	syncObjectState* objectState = objectStateQueue.NewSyncObjectState();
	bool isCorrect = updatePacketIn.Analyse(*objectState);
	if (!isCorrect)
	{
		anErrorText = updatePacketIn.analizingErrorInfo.message;
		objectStateQueue.DeleteObjectState( objectState);
		objectState = NULL;
		ATLASSERT( FALSE);
	}	
	return objectState;
}

void CSyncManager::PushObjectStateToQueue( syncObjectState* aObjectState)
{
	unsigned int queueSize = objectStateQueue.Push( aObjectState);
	if( queueSize != 0 && (queueSize % QUEUE_SIZE_TO_LOG) == 0)
	{
		if( pContext != NULL && pContext->mpLogWriter)
			pContext->mpLogWriter->WriteLn("[CSyncManager::PutSyncData] queueSize = ", queueSize);
	}
}

bool CSyncManager::PutSyncData( BYTE* aData, int aDataSize, DWORD aCheckSum, std::string& anErrorText)
{
	syncObjectState* objectState = CreateSyncObjectState( aData, aDataSize, aCheckSum, anErrorText);
	if (objectState != NULL)
	{
		// если в списке объектов на создание есть объект этого стейта, сохраним стейт вместе с объектом
		if( !mapObjectQueue.CacheSyncState( objectState))
		{
			// если в списке объектов на создание этого объекта нет, сохраним его в отдельно очереди
			PushObjectStateToQueue( objectState);
		}
		else
		{
			objectStateQueue.DetachSyncObjectState( objectState);
		}
	}
	return (objectState != NULL);
}

syncObjectState* CSyncManager::CreateNotFoundSyncObject( unsigned int anObjectID, unsigned int aBornRealityID, unsigned int aRealityID)
{
	syncObjectState* objectState = objectStateQueue.NewSyncObjectState();
	objectState->type = syncObjectState::ST_NOTFOUND;
	objectState->uiID = anObjectID;
	objectState->uiBornRealityID = aBornRealityID;
	objectState->uiRealityID = aRealityID;
	return objectState;
}

void CSyncManager::PutNotFoundSyncObject( unsigned int anObjectID, unsigned int aBornRealityID, unsigned int aRealityID)
{
	syncObjectState* objectState = CreateNotFoundSyncObject( anObjectID, aBornRealityID, aRealityID);

	// если в списке объектов на создание есть объект этого стейта, сохраним стейт вместе с объектом
	if( !mapObjectQueue.CacheSyncState( objectState))
	{
		PushObjectStateToQueue( objectState);
	}
	else
	{
		objectStateQueue.DetachSyncObjectState( objectState);
	}
}

bool CSyncManager::PutMapObject( const ZONE_OBJECT_INFO& aZoneObjectInfo)
{
	ZONE_OBJECT_INFO* objectInfo = mapObjectQueue.NewObjectInfo();
	*objectInfo = aZoneObjectInfo;
	m_isNewObjectAdded = true;

	if (objectInfo->pSynchData != NULL)
	{
		std::string errorText;
		objectInfo->objectFullState = CreateSyncObjectState( objectInfo->pSynchData, objectInfo->uSynchDataSize, 0, errorText);
		if (!objectInfo->objectFullState)
		{
			if (iWrongSynchPacket==0)
			{
				// MessageBox( NULL, "Неверный формат пакета синхронизации. Покажите лог программистам", _T("Error"), MB_OK | MB_ICONINFORMATION );
				if(pContext->mpLogWriter!=NULL)
					pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] Synch packet format error [2]. CUpdatePacketIn::Analyse failed.");
			}
			iWrongSynchPacket++;
		}
		else
		{
			//??
		}
	}
	else
	{
		objectInfo->objectFullState = CreateNotFoundSyncObject( objectInfo->complexObjectID.objectID,
			objectInfo->complexObjectID.bornRealityID, objectInfo->complexObjectID.realityID);
	}

	// предзагрузка скриптов классов для объектов
	// было: грузятся непосредство в момент необходимости создать объект

	//если className есть, то вызывать для загрузки скрипта класса -->

	if (aZoneObjectInfo.className.size() > 0)
	{
		std::map<std::string, int>::iterator it = m_classLoaded.find(aZoneObjectInfo.className);
		if (it == m_classLoaded.end())
		{
			int loadResult = pContext->mpMapMan->LoadClassFile(aZoneObjectInfo.className.c_str());
			m_classLoaded.insert(std::map<std::string, int>::value_type(aZoneObjectInfo.className, loadResult));
		}
	}

	// Map - объект приходит с полным состоянием, поэтому кэш всех остальных состояний можно удалить
	objectStateQueue.DeleteAllObjectStates( aZoneObjectInfo.complexObjectID.objectID
											, aZoneObjectInfo.complexObjectID.bornRealityID
											, aZoneObjectInfo.complexObjectID.realityID);

	if (!mapObjectQueue.Push( objectInfo))
	{
		FreeMapObject( objectInfo);
		return false;
	}
	else
	{
		//??
	}

	return true;
}

void CSyncManager::SetAvatarPosition(const float x, const float y, char* locationName)
{
	mapObjectQueue.SetAvatarPosition(x, y, locationName);
}


bool CSyncManager::NextMapObject( ZONE_OBJECT_INFO*& aZoneObjectInfo, ZONE_OBJECT_INFO* aFirst)
{
	int objectIndex = 0;
	while (mapObjectQueue.GetObjectInfoByIndex(objectIndex, aZoneObjectInfo))
	{
		if (aZoneObjectInfo->className.size() == 0)
		{
			mapObjectQueue.RemoveObjectInfoByIndex(objectIndex);
			return true;
		}

		int loadResult = pContext->mpMapMan->LoadClassFile( aZoneObjectInfo->className.c_str());
		
		if (CLASS_NOT_EXISTS == loadResult)
		{
			aZoneObjectInfo->className = "";
		}
		else if (loadResult != CLASS_LOADING)
		{			
			mapObjectQueue.RemoveObjectInfoByIndex(objectIndex);
			return true;			
		}
		
		objectIndex++;
	}

	return false;
}

bool CSyncManager::NextMapObject( ZONE_OBJECT_INFO*& aZoneObjectInfo)
{
	return NextMapObject( aZoneObjectInfo, NULL);
}

// Удалить объекты, у которых currentRealityID == auRealityID
void CSyncManager::DeleteObjectsExceptReality( unsigned int auRealityID)
{
	mapObjectQueue.DeleteObjectsExceptReality( auRealityID);
	objectStateQueue.DeleteObjectsExceptReality( auRealityID);
}

void CSyncManager::FreeMapObject( ZONE_OBJECT_INFO*& aZoneObjectInfo )
{
	if (aZoneObjectInfo == NULL)
		return;
	if (aZoneObjectInfo->objectFullState != NULL)
		FreeObjectState( aZoneObjectInfo->objectFullState);
	mapObjectQueue.DeleteObjectInfo( aZoneObjectInfo);
}

bool CSyncManager::IsNewObjectWasAdded()
{
	bool isNewObjectAdded = m_isNewObjectAdded;
	m_isNewObjectAdded = false;
	return isNewObjectAdded;
}

bool CSyncManager::IsObjectQueueEmpty()
{
	return mapObjectQueue.IsEmpty();
}

int CSyncManager::GetNearestMapObjectCount() const
{
	return mapObjectQueue.GetNearestMapObjectCount();
}

void CSyncManager::SendObject( unsigned char* aData, unsigned int aDataSize)
{
	if (!IsReady())
		return;
	pClientSession->Send( ST_SyncData, aData, aDataSize);
}

void CSyncManager::QueryObject( unsigned int anObjectID, unsigned int aBornRealityID,unsigned int aRealityID, long long aCreatingTime, bool checkFrame)
{
	if(!IsConnected())
		return;

	CQueryObjectOut queryObjectOut(anObjectID, aBornRealityID, aRealityID, aCreatingTime, checkFrame);
	pClientSession->Send( ST_QueryObjectState, (unsigned char*)queryObjectOut.GetData(), queryObjectOut.GetDataSize());
}

void CSyncManager::AddQueryLatentedObject( unsigned int anObjectID, unsigned int aBornRealityID, unsigned int aCurrentRealityID, unsigned int auiHash)
{
	if (!IsReady())
		return;

	if( pQueryLatentedObjectsOut == NULL)
		pQueryLatentedObjectsOut = MP_NEW( CQueryLatentedObjectsOut);
	pQueryLatentedObjectsOut->AddObject( anObjectID, aBornRealityID, aCurrentRealityID, auiHash);
}

// Запроси состояние объектов, который давно не получал обновление состояния
void CSyncManager::QueryLatentedObjects()
{	
	if (pQueryLatentedObjectsOut == NULL)
		return;

	if( pQueryLatentedObjectsOut->GetDataSize()>4)
	{
		pQueryLatentedObjectsOut->SaveCount();
		if (IsConnected())
			pClientSession->Send( ST_QueryLatentedObjectsState, (unsigned char*)pQueryLatentedObjectsOut->GetData(), pQueryLatentedObjectsOut->GetDataSize());
	}
	MP_DELETE(pQueryLatentedObjectsOut);
}

void CSyncManager::SendMessage( 
	unsigned int anObjectID, unsigned int aBornRealityID,
	unsigned char* aData, unsigned short aDataSize)
{
	if (!IsReady())
		return;

	CSyncMessageOut syncMessageOut(anObjectID, aBornRealityID, NULL, 0, aData, aDataSize);
	pClientSession->Send( ST_SyncMessage, syncMessageOut.GetData(), syncMessageOut.GetDataSize());
}

void CSyncManager::onQueryComplete(int aiID, const wchar_t* apwcResponse, int* aiErrorCode)
{
	pClientSession->Send( ST_StartRecord, NULL, 0);
}


// Установить интерфейс, принимающий обратные вызовы от SyncManager-а
bool CSyncManager::SetDefaultCallbacks(sync::syncISyncManCallbacks* apCallbacks, sync::syncIRecordManagerCallback* apRMCallback, sync::syncIEventManagerCallback* apEventManagerCallbacks)
{
	pCallbacks = apCallbacks;
	pRMCallBack = apRMCallback;
	pEventManagerCallbacks = apEventManagerCallbacks;
	return true;
}

void CSyncManager::OnUpdatePacket( BYTE* aData, int aDataSize, DWORD aCheckSum)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	std::string errorText;
	if (!PutSyncData( aData, aDataSize, aCheckSum, errorText))
	{
		if (iWrongSynchPacket==0)
		{
			if(pContext->mpLogWriter!=NULL)
				pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] Synch packet format error [1]. CUpdatePacketIn::Analyse failed.");
			// MessageBox( NULL, "Неверный формат пакета синхронизации. Покажите лог программистам", _T("Error"), MB_OK | MB_ICONINFORMATION );
		}
		iWrongSynchPacket++;
	}
}

void CSyncManager::OnBadUpdatePacket( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CReceiveBadSyncDataIn receiveBadSyncDataIn(aData, aDataSize);
	if(!receiveBadSyncDataIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CReceiveBadSyncDataIn");
		ATLASSERT( FALSE);
		return;
	}
	PutNotFoundSyncObject( receiveBadSyncDataIn.GetObjectID(),
		receiveBadSyncDataIn.GetBornRealityID(), receiveBadSyncDataIn.GetRealityID());
}

void CSyncManager::OnDeleteObject( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CDeleteObjectIn deleteObjectIn(aData, aDataSize);
	if(!deleteObjectIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CDeleteObjectIn");
		ATLASSERT( FALSE);
		return;
	}

	unsigned int objID = deleteObjectIn.GetObjectID();
	unsigned int bornRealityID = deleteObjectIn.GetBornRealityID();

	//если объект уже существует в рммл
	if (pContext->mpSM->IsObjectExist(objID, bornRealityID))
	{
		pContext->mpSM->DeleteObject(objID, bornRealityID);
	}
	else // иначе надо удалить объект из очереди mapObjectQueue
	{
		mapObjectQueue.DeleteObject(objID, bornRealityID);
	}
	
	objectStateQueue.DeleteAllObjectStates( deleteObjectIn.GetObjectID(),
		deleteObjectIn.GetBornRealityID(), pContext->mpMapMan->GetCurrentRealityID());
}

void CSyncManager::OnMessage( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;
	
	CSyncMessageIn messageIn(aData, aDataSize);
	if(!messageIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CSyncMessageIn");
		ATLASSERT( FALSE);
		return;
	}

	pContext->mpSM->HandleSyncMessage(
		messageIn.GetObjectID(), messageIn.GetBornRealityID(),
		messageIn.GetSenderName(), messageIn.GetData(), messageIn.GetDataLength());
}

void CSyncManager::OnConnectLost()
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	pContext->mpLogWriter->WriteLnLPCSTR( "[SYNCMAN]: The connect with SyncServer was lost");
	pContext->mpMapMan->OnConnectLost();
	pContext->mpSM->OnServerDisconnected(rmml::ML_SERVER_MASK_SYNC, NULL);
}

void CSyncManager::OnConnectRestored()
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	pContext->mpLogWriter->WriteLnLPCSTR( "[SYNCMAN]: The connect with SyncServer was restored");

	wchar_t	wcUrlBuffer[ 256];
	if( pContext->mpMapMan->GetAvatarLocationURL( wcUrlBuffer, countof(wcUrlBuffer)) != 0)
		pContext->mpSM->RestoreSynchronization( wcUrlBuffer);
	else
		pContext->mpSM->RestoreSynchronization( L"");

	//pContext->mpMapMan->OnConnectRestored();
	//pContext->mpSM->RestoreSynchronization( wcUrlBuffer);
	pContext->mpSM->OnServerConnected(rmml::ML_SERVER_MASK_SYNC, true);
}


// ---------------------------- Работа с сеансами и записями лога ----------------------------------------


// Посылаем URL при телепортации на сервак для уточнения
omsresult CSyncManager::PreciseURL(const wchar_t* apwcURL)
{
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CPreciseURLOut outData(apwcURL);
	pClientSession->SendQuery( ST_PreciseURL, outData.GetData(), outData.GetDataSize());
	return OMS_ERROR_NOT_INITIALIZED;

}

// создать сеанс воспроизведения лога по URL
omsresult CSyncManager::createSeanceByURL(syncSeanceInfo& aSeanceInfo, unsigned int auIDSeanceRmmlObject)
{
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CCreateSeanceOut outData(auIDSeanceRmmlObject, aSeanceInfo.mpwcURL.c_str());
	pClientSession->Send( ST_GetSeanceByURL, outData.GetData(), outData.GetDataSize());
	return OMS_ERROR_NOT_INITIALIZED;

}

void CSyncManager::GetLanguage( CWComString &sLang)
{
	if( pContext != NULL && pContext->mpApp != NULL)
		pContext->mpApp->GetLanguage((wchar_t*)sLang.GetBufferSetLength(30), 29);
	if( sLang == L"rus")
		sLang = L"ru_RU";
	else if( sLang == L"eng")
		sLang = L"en_US";
}

omsresult CSyncManager::StartRecord(int aiRecordMode,  const wchar_t* apwcRecordName, int& aiErrorCode)
{
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	if (pContext == NULL || pContext->mpMapMan == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	int recordParentRealityID = pContext->mpMapMan->GetCurrentRealityID();

	unsigned short size = pContext->mpMapMan->GetCurrentLocationName(NULL, 0);
	if (size==0 )
	{
		// Запись вне локации пока недоступна
		aiErrorCode = 1;
		return OMS_ERROR_FAILURE;
	}

	wchar_t * pwcLocationName = MP_NEW_ARR( wchar_t, size);
	pContext->mpMapMan->GetCurrentLocationName(pwcLocationName, size);
	pContext->mpLogWriter->WriteLnLPCSTR( "[SYNCMAN]: Start record packet go to the server");

	CWComString sLang;
	GetLanguage(sLang);	
	CStartRecordOut startRecordOut(aiRecordMode, recordParentRealityID, pwcLocationName, apwcRecordName, sLang.GetBuffer());
	pClientSession->Send(ST_StartRecord, startRecordOut.GetData(), startRecordOut.GetDataSize());
	MP_DELETE_ARR( pwcLocationName);
	aiErrorCode = 0;
	return OMS_OK;
}

omsresult CSyncManager::StopRecord( unsigned int& aRecordID, int& aiErrorCode, syncRecordInfo** aRecordInfo)
{
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CStopRecordOut stopRecordOut( aRecordID);
	pClientSession->Send( ST_StopRecord, stopRecordOut.GetData(), stopRecordOut.GetDataSize());

	CComString sLog;
	sLog.Format( "[SYNCMAN]: Stop record packet go to the server. RecordID = ", aRecordID);
	pContext->mpLogWriter->WriteLnLPCSTR( sLog.GetBuffer());

	aRecordID = iRecordID;
	aiErrorCode = 0;
	*aRecordInfo = &oRecordInfo;

	return OMS_OK;
}
// изменить на сервере информацию о записи 
omsresult CSyncManager::UpdateRecordInfo(syncRecordInfo& aRecordInfo){
	if (aRecordInfo.mpwcPreview.empty()) 
		aRecordInfo.mpwcPreview = L"none";
	if (aRecordInfo.mpwcSubject.empty()) 
		aRecordInfo.mpwcSubject = L"none";
	if (aRecordInfo.mpwcLevel.empty()) 
		aRecordInfo.mpwcLevel = L"none";
	if (aRecordInfo.mpwcLesson.empty()) 
		aRecordInfo.mpwcLesson = L"none";
	if (aRecordInfo.mpwcTags.empty()) 
		aRecordInfo.mpwcTags = L"none";

	pContext->mpLogWriter->WriteLnLPCSTR( "[SYNCMAN]: Update record packet go to the server");
	CUpdateRecordInfoOut updateRecord(aRecordInfo.muRecordID
										, aRecordInfo.mpwcName.c_str()
										, aRecordInfo.mpwcSubject.c_str()
										, aRecordInfo.mpwcLesson.c_str()
										, aRecordInfo.mpwcLevel.c_str()
										, aRecordInfo.mpwcPreview.c_str()
										, aRecordInfo.mpwcDescription.c_str()
										, aRecordInfo.mbPublishNow
										, aRecordInfo.mpwcTags.c_str() );
	pClientSession->Send( ST_UpdateRecord, updateRecord.GetData(), updateRecord.GetDataSize());

	return OMS_ERROR_NOT_INITIALIZED;
}

omsresult CSyncManager::GetSeances(unsigned int auID, unsigned int auSeancesObjectID)
{
	// TODO obsolete
	ATLASSERT( false);
	CGetSeancesOut getSeances(auSeancesObjectID, auID);
	pClientSession->SendQuery( ST_GetSeances, getSeances.GetData(), getSeances.GetDataSize());
	pContext->mpLogWriter->WriteLnLPCSTR( "[SYNCMAN]: Get seances packet go to the server");
	return OMS_ERROR_NOT_INITIALIZED;
}

// получить набор записей (auID - некий идентификатор набора записей, который надо передать в callback)
omsresult CSyncManager::GetRecords(unsigned int auID, int aiBegin, int aiCount, const wchar_t* apwcFilter, const wchar_t* apwcOrderBy, sync::syncIRecordManagerCallback * pCallback)
{
	// TODO obsolete
	ATLASSERT( false);
	IDLogObject = auID;
	if (pClientSession == NULL)
		return false;
	if (wcscmp(apwcFilter,L""))
	{
		std::wstring sFilter = L" AND " + (std::wstring)apwcFilter;
		CQueryLogRecordsOut queryLogRecordsOut(auID, aiBegin, aiCount, sFilter.c_str(), apwcOrderBy, pCallback);
		pClientSession->SendQuery( ST_QueryLogRecords, queryLogRecordsOut.GetData() , queryLogRecordsOut.GetDataSize());
	}
	else
	{	CQueryLogRecordsOut queryLogRecordsOut(auID, aiBegin, aiCount, apwcFilter, apwcOrderBy, pCallback);
		pClientSession->SendQuery( ST_QueryLogRecords, queryLogRecordsOut.GetData() , queryLogRecordsOut.GetDataSize());
	}	
	return true;
}

// получить набор записей (auID - некий идентификатор набора записей, который надо передать в callback)
omsresult CSyncManager::GetEventRecords(unsigned int auID, int aiBegin, int aiCount, const wchar_t* apwcFilter, const wchar_t* apwcOrderBy, sync::syncIRecordManagerCallback2 * pCallback)
{
	// TODO obsolete
	//ATLASSERT( false);
	IDLogObject = auID;
	if (pClientSession == NULL)
		return false;
	if (wcscmp(apwcFilter,L""))
	{
		std::wstring sFilter = L" AND " + (std::wstring)apwcFilter;
		CQueryEventRecordsOut queryEventRecordsOut(auID, aiBegin, aiCount, sFilter.c_str(), apwcOrderBy, pCallback);
		pClientSession->SendQuery( ST_QueryEventRecords, queryEventRecordsOut.GetData() , queryEventRecordsOut.GetDataSize());
	}
	else
	{	CQueryEventRecordsOut queryEventRecordsOut(auID, aiBegin, aiCount, apwcFilter, apwcOrderBy, pCallback);
	pClientSession->SendQuery( ST_QueryEventRecords, queryEventRecordsOut.GetData() , queryEventRecordsOut.GetDataSize());
	}	
	return true;
}

// удалить запись (отсылает запрос на сервер)
omsresult CSyncManager::DeleteRecord(unsigned int auID){ 
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CDeleteRecordOut deleteRecord(auID);
	pClientSession->Send( ST_Delete, deleteRecord.GetData(), deleteRecord.GetDataSize());

	return OMS_OK;
}

// удалить запись в корзину (отсылает запрос на сервер)
omsresult CSyncManager::DeleteRecordIntoRecycleBin(unsigned int auID){ 
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CDeleteRecordIntoRecycleBinOut deleteRecord(auID);
	pClientSession->Send( ST_DeleteIntoRecycleBin, deleteRecord.GetData(), deleteRecord.GetDataSize());

	return OMS_OK;
}


// удалить записи (отсылает запрос на сервер)
omsresult CSyncManager::GetUserList( unsigned int realityID){
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CGetUserListOut getUserListOut( realityID);
	pClientSession->Send( ST_GetUserList, getUserListOut.GetData(), getUserListOut.GetDataSize());

	return OMS_OK;
}

// удалить записи (отсылает запрос на сервер)
omsresult CSyncManager::DeleteRecords( unsigned int *pRecIDs, unsigned int count){
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CDeleteRecordsOut deleteRecords( pRecIDs, count);
	pClientSession->Send( ST_DeleteRecords, deleteRecords.GetData(), deleteRecords.GetDataSize());

	return OMS_OK;
}

// начать воспроизведение записи (отсылает запрос на сервер)	
omsresult CSyncManager::Play(unsigned int auID){
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CPlayOut playOut(auID);
	pClientSession->Send( ST_Play, playOut.GetData(), playOut.GetDataSize());

	return OMS_OK;
}

// переместить воспроизведение записи (отсылает запрос на сервер)	
omsresult CSyncManager::Seek( unsigned long seekPos){
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CSeekOut seekOut( seekPos);
	pClientSession->Send( ST_Seek, seekOut.GetData(), seekOut.GetDataSize());

	return OMS_OK;
}

// установить логин пользователя, управляющего воспроизведением записи
omsresult CSyncManager::SetPilotLogin( const wchar_t* alpcPilotLogin, unsigned int auCurrentEventID)
{
	if( !alpcPilotLogin || wcslen(alpcPilotLogin) == 0)
		return OMS_ERROR_FAILURE;

	CSetPilotLogin setPilotLoginOut( alpcPilotLogin, auCurrentEventID);
	pClientSession->Send( ST_SetPilotLogin, setPilotLoginOut.GetData(), setPilotLoginOut.GetDataSize());

	return OMS_OK;
}

// переместить воспроизведение записи (отсылает запрос на сервер)	
omsresult CSyncManager::Save( const wchar_t* apwcAuthor, const wchar_t* apwcCreationTime, const wchar_t* apwcName, const wchar_t* apwcDescription){
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CSaveOut seekOut( apwcAuthor, apwcCreationTime, apwcName, apwcDescription);
	pClientSession->Send( ST_RecordSave, seekOut.GetData(), seekOut.GetDataSize());

	return OMS_OK;
}

// отмена операции редактора записей (отсылает запрос на сервер)	
void  CSyncManager::RecordEditorUndo(){
	if (pClientSession == NULL)
		return;

	pClientSession->Send( ST_RecordEditorUndo, NULL, 0);
}

// повтор операции редактора записей (отсылает запрос на сервер)	
void CSyncManager::RecordEditorRedo(){
	if (pClientSession == NULL)
		return;

	pClientSession->Send( ST_RecordEditorRedo, NULL, 0);
}

// вырезание сегмента записи
void CSyncManager::RecordEditorCut(unsigned int startPos, unsigned int endPos)
{
	if (pClientSession == NULL)
		return;

	CRecordEditorCutOut recordEditorCutOut(startPos, endPos);
	pClientSession->Send( ST_RecordEditorCut, recordEditorCutOut.GetData(), recordEditorCutOut.GetDataSize());
}
// вставка сегмента записи
void CSyncManager::RecordEditorPaste(unsigned int pastePos)
{
	if (pClientSession == NULL)
		return;

	CRecordEditorPasteOut recordEditorPasteOut(pastePos);
	pClientSession->Send( ST_RecordEditorPaste, recordEditorPasteOut.GetData(), recordEditorPasteOut.GetDataSize());
}

// копирование сегмента записи
void CSyncManager::RecordEditorCopy(unsigned int startPos, unsigned int endPos)
{
	if (pClientSession == NULL)
		return;

	CRecordEditorCopyOut recordEditorCopyOut(startPos, endPos);
	pClientSession->Send( ST_RecordEditorCopy, recordEditorCopyOut.GetData(), recordEditorCopyOut.GetDataSize());
}
// удаление сегмента записи
void CSyncManager::RecordEditorDelete(unsigned int startPos, unsigned int endPos)
{
	if (pClientSession == NULL)
		return;

	CRecordEditorDeleteOut recordEditorDeleteOut(startPos, endPos);
	pClientSession->Send( ST_RecordEditorDelete, recordEditorDeleteOut.GetData(), recordEditorDeleteOut.GetDataSize());
}

// открытие записи для редактирования (отсылает запрос на сервер)
void CSyncManager::RecordEditorOpen(unsigned int aRecordID)
{
	if (pClientSession == NULL)
		return;

	CRecordEditorOpenOut recordEditorOpenOut( aRecordID);
	pClientSession->Send( ST_RecordEditorOpen, recordEditorOpenOut.GetData(), recordEditorOpenOut.GetDataSize());
}

// сохранение записи из редактора записей (отсылает запрос на сервер)
void CSyncManager::RecordEditorSave()
{
	if (pClientSession == NULL)
		return;

	pClientSession->Send( ST_RecordEditorSave, NULL, 0);
}

// сохранение записи под новым именем из редактора записей  (отсылает запрос на сервер)
void CSyncManager::RecordEditorSaveAs(const wchar_t* apwcName, const wchar_t* apwcDescription, bool abOverwrite, bool abPublicate)
{
	if (pClientSession == NULL)
		return;

	CRecordEditorSaveAsOut recordEditorSaveAsOut(apwcName, apwcDescription, abOverwrite, abPublicate);
	pClientSession->Send( ST_RecordEditorSaveAs, recordEditorSaveAsOut.GetData(), recordEditorSaveAsOut.GetDataSize());
}

// начать воспроизведение записи
void CSyncManager::RecordEditorPlay()
{
	if (pClientSession == NULL)
		return;

	pClientSession->Send( ST_RecordEditorPlay, NULL, 0);
}

// приоставновить всопроизведение записи
void CSyncManager::RecordEditorPause()
{
	if (pClientSession == NULL)
		return;

	pClientSession->Send( ST_RecordEditorPause, NULL, 0);
}

// Перемотать вочпроизведние записи на начало
void CSyncManager::RecordEditorRewind()
{
	if (pClientSession == NULL)
		return;

	pClientSession->Send( ST_RecordEditorRewind, NULL, 0);
}

// перемотать воспроизведение записи
void CSyncManager::RecordEditorSeek(unsigned int aSeekPos)
{
	if (pClientSession == NULL)
		return;

	CRecordEditorSeekOut recordEditorSeekOut(aSeekPos);
	pClientSession->Send( ST_RecordEditorSeek, recordEditorSeekOut.GetData(), recordEditorSeekOut.GetDataSize());
}

// начать воспроизведение записи (отсылает запрос на сервер)
omsresult CSyncManager::QueryFullSeanceInfo(unsigned int auIDSeanceRmmlObject, unsigned int auID){
	CQueryFullSeanceInfoOut seanceInfoOut(auIDSeanceRmmlObject,auID);
	pClientSession->SendQuery( ST_QueryFullSeanceInfo, seanceInfoOut.GetData(), seanceInfoOut.GetDataSize());
	return OMS_OK;
}

// остановить воспроизведение записи
omsresult CSyncManager::Stop(){
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	pClientSession->Send( ST_Stop, NULL, 0);

	return OMS_OK;
}

// приостановить воспроизведение записи
omsresult CSyncManager::Pause(){
	// ??
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	pClientSession->Send( ST_Pause, NULL, 0);

	return OMS_ERROR_NOT_INITIALIZED;
}


omsresult CSyncManager::CloseClient(){
	// ??
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	pClientSession->Send( ST_ClientClose, NULL, 0);

	return OMS_ERROR_NOT_INITIALIZED;
}

// перемотать запись на начало
omsresult CSyncManager::Rewind(){
//Rewind
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;
	pClientSession->Send( ST_Rewind, NULL, 0);
	return OMS_OK;
}

// удалить объект
omsresult CSyncManager::DeleteObject(unsigned int auID, unsigned int auRealityID)
{
	CDeleteObjectOut deleteOut(auID, auRealityID);
	pClientSession->Send( ST_DeleteObject, deleteOut.GetData(), deleteOut.GetDataSize());
	return OMS_OK;
}

// получить статус объекта
omsresult CSyncManager::GetObjectStatus(unsigned int auObjectID, unsigned int auBornRealityID)
{
	if(!IsConnected())
		return OMS_ERROR_NOT_IMPLEMENTED;
	CGetObjectStatusOut getObjectStatusOut(auObjectID, auBornRealityID);
	pClientSession->SendQuery( ST_GetObjectStatus, getObjectStatusOut.GetData(), getObjectStatusOut.GetDataSize());
	return OMS_OK;
}

// запросить свои права (отсылает запрос на сервер)	
omsresult CSyncManager::GetEventUserRight( unsigned int eventID){
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CEventGetUserRightOut packetOut( eventID);
	pClientSession->Send( ST_EventGetUserRight, packetOut.GetData(), packetOut.GetDataSize());
	return OMS_OK;
}

//  права пользователей (отсылает запрос на сервер)	
omsresult CSyncManager::SetEventUserRights( unsigned int eventID, wchar_t *apwcDescr){
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CEventUserRightsUpdateOut packetOut( eventID, apwcDescr);
	pClientSession->Send( ST_EventUserRightsUpdate, packetOut.GetData(), packetOut.GetDataSize());
	return OMS_OK;
}

// изменить права пользователей (отсылает запрос на сервер)	
omsresult CSyncManager::SetEventUserDefaultRights( unsigned int eventID, unsigned int  aRight){
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CEventUserDefaultRightsOut packetOut( eventID, aRight);
	pClientSession->Send( ST_EventUserDefaultRightsUpdate, packetOut.GetData(), packetOut.GetDataSize());
	return OMS_OK;
}

// изменить забанить ( разбанить) пользователя (отсылает запрос на сервер)	
omsresult CSyncManager::SetEventUserBan( unsigned int eventID, wchar_t *apwcDescr){
	if (pClientSession == NULL)
		return OMS_ERROR_NOT_INITIALIZED;

	CEventUserBanUpdateOut packetOut( eventID, apwcDescr);
	pClientSession->Send( ST_EventUserBanUpdate, packetOut.GetData(), packetOut.GetDataSize());
	return OMS_OK;
}

bool CSyncManager::SetLocationEnteringPermissionForUser( const wchar_t* apwcUserLogin, const wchar_t* apwcLocationID, unsigned int auRealityID, bool abAllow)
{
	if( !apwcUserLogin)
		return false;

	if( !apwcLocationID)
		return false;

	CSetLocationEnteringPermissionForUserQueryOut queryOut(apwcUserLogin, apwcLocationID, auRealityID, abAllow);
	pClientSession->Send(ST_SetLocationEnteringPermission, queryOut.GetData(), queryOut.GetDataSize());

	return true;
}

void CSyncManager::ConnectToWorld( const wchar_t *apwcRequestedUrl, const syncObjectState* mpcAvatarState)
{
	CRestoreSyncronizationOut packetOut;
	packetOut.Create( apwcRequestedUrl, mpcAvatarState);

	CComString sLog;
	sLog.Format("[SYNC] RestoreSyncronization (RS): apwcRequestedUrl = %S, syncDataSize = %u"
						, apwcRequestedUrl ? apwcRequestedUrl : L"is NULL"
						, packetOut.GetDataSize());
	writeLog(sLog.GetBuffer());

	if (pClientSession == NULL)
	{
		writeLog("[SYNC] RSError: pClientSession == NULL");
		return;
	}

	pClientSession->Send( ST_ConnectToWorld, packetOut.GetData(), packetOut.GetDataSize());

	writeLog("[SYNC] RS ST_ConnectToWorld sended to server");
}

//void CSyncManager::OnUpdatePacket( BYTE* aData, int aDataSize)
// ------------------- Обработка сообщений поступивших с сервака --------------------------
// идет запись последовательности действий в запись лога
void CSyncManager::OnRecordConnected( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordConnectedIn recordStartedInfo(aData, aDataSize); 
	if(!recordStartedInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordConnectedIn");
		ATLASSERT( FALSE);
		return;
	}
	oRecordInfo = recordStartedInfo.recordInfo;
	syncRecordInfo *recInfo = recordStartedInfo.getRecordInfo();
	pContext->mpLogWriter->WriteLnLPCSTR( CLogValue("[SYNCMAN]: An RecordStarted packet has received",recInfo->muRecordID).GetData());

	if (pCallbacks != NULL)
	{
		//if (recInfo->muRecordID == 0 )
		if( recordStartedInfo.GetErrorCode() != 0)
		{
			pEventManagerCallbacks->onRecordStartFailed(recordStartedInfo.GetErrorCode(), recordStartedInfo.GetAdditionalObjectInfo());
		}
		else
		{
			pEventManagerCallbacks->onRecordConnected(recordStartedInfo.recordInfo.muRecordID
										, recInfo->mpwcAuthor.c_str()
										, recordStartedInfo.GetIsAuthorOrAssistant(), recordStartedInfo.GetDuration(), recordStartedInfo.GetLocationName());
		}
	}
}
// сообщение с сервера о том что в локации запись закончена
void CSyncManager::OnRecordStoped( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;
	CRecordStopedIn recordStopedInfo(aData, aDataSize); 
	if(!recordStopedInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordStopedIn");
		ATLASSERT( FALSE);
		return;
	}
	oRecordInfo.mpwcDescription = recordStopedInfo.getDescription();
	oRecordInfo.mpwcTags = recordStopedInfo.getTags();

	CComString s;
	s.Format("OnRecordStoped::recordID = %d, duration = %d", recordStopedInfo.getRecordID(), recordStopedInfo.getDuration());
	writeLog( s.GetBuffer());
	if (pEventManagerCallbacks != NULL)
		pEventManagerCallbacks->onRecordStoped(recordStopedInfo.getRecordID(), recordStopedInfo.getDuration(), &oRecordInfo);

}

// сообщение с сервера о том что клиент отконнектился от записи в аудитории
void CSyncManager::OnRecordDisconnected( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordDisconnectedIn recordDisconnectedInfo(aData, aDataSize); 
	if(!recordDisconnectedInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordDisconnectedIn");
		ATLASSERT( FALSE);
		return;
	}

	if (pEventManagerCallbacks != NULL)
		pEventManagerCallbacks->onRecordDisconnected(recordDisconnectedInfo.getRecordID(), recordDisconnectedInfo.GetIsAuthor(), recordDisconnectedInfo.GetIsNeedToAsk());
}

void CSyncManager::OnUpdatePlayingTime( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CUpdatePlayingTimeIn updatePlayingTimeInfo(aData, aDataSize); 
	if(!updatePlayingTimeInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CUpdatePlayingTimeIn");
		ATLASSERT( FALSE);
		return;
	}

	if (pCallbacks != NULL)
		pCallbacks->onUpdatePlayingTime(updatePlayingTimeInfo.getTime());
}

void CSyncManager::OnSeanceSeekPos( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CUpdatePlayingTimeIn updatePlayingTimeInfo(aData, aDataSize); 
	if(!updatePlayingTimeInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CUpdatePlayingTimeIn");
		ATLASSERT( FALSE);
		return;
	}

	if (pCallbacks != NULL)
		pCallbacks->onSeanceSeekPos(updatePlayingTimeInfo.getTime());
}


// Создан сеанс воспроизведения записи лога
void CSyncManager::OnLogSeanceCreated( BYTE* aData, int aDataSize)
{
	CLogSeanceCreatedIn logSeanceCreatedInfo(aData, aDataSize);
	if (!logSeanceCreatedInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CLogSeanceCreatedIn");
		ATLASSERT( FALSE);
		return;
	}

	int iError=0;
	if (logSeanceCreatedInfo.GetSeanceID() == 0) iError = 1;
	if (pCallbacks != NULL)
	{
		pCallbacks->onSeanceCreated(logSeanceCreatedInfo.GetSeanceID(), 0, logSeanceCreatedInfo.GetSeanceMode(), logSeanceCreatedInfo.GetAuthor(), logSeanceCreatedInfo.GetSeanceURL(), iError);
	}
}

void CSyncManager::OnSeanceFinished( BYTE* aData, int aDataSize)
{
	ATLASSERT(FALSE);	// устаревший метод
	/*if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CPlayerClosedIn playerClosedIn(aData, aDataSize);
	if(!playerClosedIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CPlayerClosedIn");
		ATLASSERT( FALSE);
		return;
	}

	wchar_t wsUrlArr[ 2000];
	wchar_t* wsUrl = wsUrlArr;
	wsUrl[0] = 0;
	short len = pContext->mpMapMan->GetAvatarLocationURL( wsUrl, sizeof(wsUrlArr)/sizeof(wsUrlArr[0]));
	CUrlParser url( wsUrl);
	url.AddParam( L"rn", L"0");

	if (pCallbacks != NULL)
		pCallbacks->onSeanceClosed(playerClosedIn.GetSeanceID());

	pContext->mpMapMan->Teleport( url.GetFullURL());*/
}


// Получен список сеансов воспроизведения для записи лога
void CSyncManager::OnReceivedSeances( BYTE* aData, int aDataSize)
{
	ATLASSERT(FALSE);	// устаревший метод
	/*if (pContext == NULL || pContext->mpSM == NULL)
		return;
	int iError=0;

	CReceivedSeancesIn receivedSeances(aData, aDataSize);
	if(!receivedSeances.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CReceivedSeancesIn");
		ATLASSERT( FALSE);
		return;
	}
	IDLogObject = receivedSeances.GetIDLogObject();
	if (pCallbacks != NULL && IDLogObject!=-1)
		pCallbacks->onReceivedSeancesInfo(receivedSeances.GetSeancesCount(), (syncSeanceInfo **)receivedSeances.pSeanceInfo, receivedSeances.GetIDLogObject(),iError);*/
}

// Получен список сеансов воспроизведения для записи лога
void CSyncManager::OnReceivedFullSeanceInfo( BYTE* aData, int aDataSize)
{
	ATLASSERT(FALSE);	// устаревший метод
	/*if (pContext == NULL || pContext->mpSM == NULL)
		return;
	int iError=0;

	CReceivedSeanceInfoIn receivedSeanceInfo(aData, aDataSize);
	if(!receivedSeanceInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CReceivedSeanceInfoIn");
		ATLASSERT( FALSE);
		return;
	}
	IDLogObject = receivedSeanceInfo.GetIDLogObject();

	if (pCallbacks != NULL && IDLogObject!=-1)
		pCallbacks->onReceivedFullSeanceInfo(IDLogObject , &receivedSeanceInfo.pSeanceInfo, iError);*/
	
}

// С сервера получен список записей лога
void CSyncManager::OnGettingLogRecords( BYTE* aData, int aDataSize)
{
	ATLASSERT(FALSE);	// устаревший метод
	//if (pContext == NULL || pContext->mpSM == NULL)
	//	return;

	//unsigned long messageSize = aDataSize - 4;

	//unsigned long unpackedSize = (unsigned long)(*(unsigned int*)(aData+4));
	//char *unpackBuffer = MP_NEW_ARR( char, unpackedSize);

	//ZlibEngine zip;
	///*int decompResult = */zip.decompmem((char*)(aData+8), &messageSize, unpackBuffer, &unpackedSize);

	//CGettingLogRecordsIn logRecordsInfo((BYTE*)unpackBuffer, unpackedSize);
	//if(!logRecordsInfo.Analyse())
	//{
	//	if(pContext->mpLogWriter!=NULL)
	//		pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CGettingLogRecordsIn");
	//	ATLASSERT( FALSE);
	//	return;
	//}

	//int iError = 0;
	//if (logRecordsInfo.pCallBack != NULL && IDLogObject!=-1)
	//{
	//	syncCountRecordInfo countRec = logRecordsInfo.GetTotalCountRecord();
	//	logRecordsInfo.pCallBack->onReceivedRecordsInfo(countRec, logRecordsInfo.GetCountRecord(), (syncRecordInfo **)logRecordsInfo.pRecordInfo, IDLogObject,iError);
	//}

	//MP_DELETE_ARR( unpackBuffer);
}

// С сервера получен список записей 
void CSyncManager::OnGettingEventRecords( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	unsigned long messageSize = aDataSize - 4;

	unsigned long unpackedSize = (unsigned long)(*(unsigned int*)(aData));
	char *unpackBuffer = MP_NEW_ARR( char, unpackedSize);

	ZlibEngine zip;
	int decompResult = zip.decompmem((char*)(aData+4), &messageSize, unpackBuffer, &unpackedSize);

	CGettingEventRecordsIn eventRecordsInfo((BYTE*)unpackBuffer, unpackedSize);
	if(!eventRecordsInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CGettingEventRecordsIn");
		ATLASSERT( FALSE);
		return;
	}

	int iError = 0;
	if (eventRecordsInfo.pCallBack != NULL && IDLogObject!=-1)
	{
		unsigned int totalCount = eventRecordsInfo.GetTotalCountRecord();
		eventRecordsInfo.pCallBack->onReceivedRecordsExtendedInfo(totalCount, eventRecordsInfo.GetCountRecord(), (syncRecordExtendedInfo **)eventRecordsInfo.pRecordInfo, IDLogObject,iError);
	}

	MP_DELETE_ARR( unpackBuffer);
}

// С сервера получен запрос на проверку валидности сессии
void CSyncManager::OnSessionCheck( BYTE* aData, int aDataSize)
{
// Посылаем обратно на сервак
	if (pClientSession == NULL)
		return ;
	pClientSession->Send( ST_SessionCheckAnswer, NULL, 0);

}

// С сервера получена комманда остановить воспроизведение сеанса
void CSyncManager::OnSeancePaused( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CSeancePausedIn seancePaused(aData, aDataSize);
	if(!seancePaused.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CSeancePausedIn");
		ATLASSERT( FALSE);
		return;
	}
	if (pCallbacks != NULL)
		pCallbacks->onSeancePaused(seancePaused.GetCurrentRealityID());

}

// С сервера пришла инфа о том что сеанс воспроизведения загружен на серваке
void CSyncManager::OnSeanceLoaded( BYTE* aData, int aDataSize)
{	
	// ??????
	CSeanceLoadedIn seanceLoaded(aData, aDataSize);
	if(!seanceLoaded.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CSeanceLoadedIn");
		ATLASSERT( FALSE);
		return;
	}
	if (pCallbacks != NULL)
		pCallbacks->onJoinedToSeance(seanceLoaded.GetRecordID(), seanceLoaded.GetSeanceID(), seanceLoaded.IsAuthor(), seanceLoaded.IsSuspended(), seanceLoaded.GetMinutes());
}


// С сервера получен статус объекта
void CSyncManager::OnReceivedObjectStatus( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CReceivedObjectStatusIn receivedObjectStatusIn(aData, aDataSize);
	if(!receivedObjectStatusIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CReceivedObjectStatusIn");
		ATLASSERT( FALSE);
		return;
	}
	if (pCallbacks != NULL)
		pCallbacks->onReceivedObjectStatus(receivedObjectStatusIn.GetObjectID(), 
			receivedObjectStatusIn.GetBornRealityID(), receivedObjectStatusIn.GetStatus());
}

void CSyncManager::OnSeancePlayed( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	if (pCallbacks != NULL)
		pCallbacks->onSeancePlayed();

}

void CSyncManager::OnSeanceRewinded( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	if (pCallbacks != NULL)
		pCallbacks->onSeanceRewinded();
}

void CSyncManager::OnSeanceFinishedPlaying( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	if (pCallbacks != NULL)
		pCallbacks->onSeanceFinishedPlaying();
}


void CSyncManager::OnSeanceLeaved( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;
	
	CLeavedSeanceIn info(aData, aDataSize);
	if(!info.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CLeavedSeanceIn");
		ATLASSERT( FALSE);
		return;
	}

	if (pCallbacks != NULL && IDLogObject!=-1)
		pCallbacks->onSeanceLeaved();

}

void CSyncManager::OnEventUpdateInfo( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;
	CEventNotifyIn eventNotifyInfo(aData, aDataSize);
	if(!eventNotifyInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] EventEnteredIn");
		ATLASSERT( FALSE);
		return;
	}
	if (pEventManagerCallbacks != NULL)
		pEventManagerCallbacks->onEventUpdateInfo(eventNotifyInfo.GetEventID(), eventNotifyInfo.GetEventLocationID(), eventNotifyInfo.GetEventRealityID(), eventNotifyInfo.GetPlayingRecordID(), eventNotifyInfo.GetJSONDescription());
}

void CSyncManager::OnEventEntered( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CEventNotifyIn eventNotifyInfo(aData, aDataSize);
	if(!eventNotifyInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] EventEnteredIn");
		ATLASSERT( FALSE);
		return;
	}
	if (pEventManagerCallbacks != NULL) 
		pEventManagerCallbacks->onEventEntered(eventNotifyInfo.GetEventID(), eventNotifyInfo.GetEventLocationID(), eventNotifyInfo.GetEventRealityID(), eventNotifyInfo.GetPlayingRecordID(), eventNotifyInfo.GetBaseRecordID(), eventNotifyInfo.GetEventRecordID(), eventNotifyInfo.GetIsEventRecording(), eventNotifyInfo.GetJSONDescription());
}

void CSyncManager::OnEventLeaved( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CEventLeavedNotifyIn eventNotifyInfo(aData, aDataSize);	
	if(!eventNotifyInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] EventLeavedIn");
		ATLASSERT( FALSE);
		return;
	}
	if (pEventManagerCallbacks != NULL)
		pEventManagerCallbacks->onEventLeaved(eventNotifyInfo.GetEventID(), eventNotifyInfo.GetPlayingRecordID());
}

void CSyncManager::OnObjectCreationResult( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	ObjectCreateResultQueryIn objectCreateResultQueryIn(aData, aDataSize);
	if(!objectCreateResultQueryIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] ObjectCreationResult");
		ATLASSERT( FALSE);
		return;
	}
	if (pCallbacks != NULL)
		pCallbacks->onObjectCreationResult(objectCreateResultQueryIn.GetCreationErrorCode()
											, objectCreateResultQueryIn.GetObjectID()
											, objectCreateResultQueryIn.GetBornRealityID()
											, objectCreateResultQueryIn.GetRmmlHandlerID());
}

void CSyncManager::OnServerErrorReceived( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CServerErrorIn serverErrorInfo(aData, aDataSize);
	if(!serverErrorInfo.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] ServerInfoIn");
		ATLASSERT( FALSE);
		return;
	}

	if (pCallbacks != NULL)
		pCallbacks->onServerErrorReceived(serverErrorInfo.GetErrorCode());

}

void CSyncManager::OnReceivedPrecizeURL( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CPrecisedURLIn info(aData, aDataSize);
	if(!info.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CPrecisedURLIn");
		return;
	}

	if( info.GetErrorCode() == 0)
	{
		pContext->mpSM->OnPreciseURL( info.GetURL());
	}
	else
	{
		pContext->mpMapMan->HandleTeleportError( info.GetErrorCode(), L"", info.GetEnteredEventInfo());
	}
}

void CSyncManager::OnUserListReceived( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
	{
		return;
	}

	CGetUserListIn info(aData, aDataSize);

	if (!info.Analyse())
	{
		if (pContext->mpLogWriter != NULL)
		{
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CGetUserListLIn");
		}
		ATLASSERT( FALSE);

		return;
	}

	if (pCallbacks != NULL)
	{
		pCallbacks->onUserListReceived(info.GetRealityID(), info.GetUserCount(), info.GetUserList());
	}
}

void CSyncManager::OnReceivedSeancesInfo( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CSeancesInfoIn info(aData, aDataSize);
	if(!info.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CSeancesInfoIn");
		return;
	}
	pContext->mpSM->OnSeancesInfo( info.GetSeancesInfo(), (bool)info.GetIsFull());	
}

void CSyncManager::OnReceivedEventsInfoIntoLocation( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CEventsInfoIntoLocationIn info(aData, aDataSize);
	if(!info.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CEventsInfoIntoLocationIn");
		return;
	}
	pContext->mpSM->OnEventsInfoIntoLocation( info.GetEventsInfoIntoLocation());	
}

void CSyncManager::OnLocationEntered( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CLocationEnteredIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CJsonEventIn(OnLocationEntered)");
		return;
	}
	if( pEventManagerCallbacks != NULL)
		pEventManagerCallbacks->onLocationEntered( packetIn.GetLocationID(), packetIn.GetJSONDescription());	
}

void CSyncManager::OnInfoMessage( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CInfoMessageIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CInfoMessageIn");
		return;
	}
	if( pCallbacks != NULL)
		pCallbacks->onInfoMessage( packetIn.GetMessageCode(), packetIn.GetMessage());	
}


void CSyncManager::OnReceivedEventUserRightsInfo( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CEventUserRightsInfoIn info(aData, aDataSize);
	if(!info.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CEventUserRightsInfoIn");
		return;
	}
	if( pEventManagerCallbacks != NULL)
		pEventManagerCallbacks->onEventUserRightsInfo( info.GetEventID(), info.GetEventUserCaps());	
}

void CSyncManager::OnReceivedEventUserRightUpdate( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CEventUserRightUpdateIn info(aData, aDataSize);
	if(!info.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CEventUserRightUpdateIn");
		return;
	}
	if( pEventManagerCallbacks != NULL)
		pEventManagerCallbacks->onEventUserRightUpdate( info.GetEventID(), info.GetChangedEventUserCaps());
}

void CSyncManager::OnReceivedEventUserBanUpdate( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CEventUserBanUpdateIn info(aData, aDataSize);
	if(!info.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CCEventUserBanUpdateIn");
		return;
	}
	if( pEventManagerCallbacks != NULL)
		pEventManagerCallbacks->onEventUserBanUpdate( info.GetEventName(), info.GetEventUrl(), info.GetIsBan());
}

void CSyncManager::OnErrorCreateTempLocation( BYTE* aData, int aDataSize)
{
	if (pContext == NULL || pContext->mpSM == NULL)
		return;

	CErrorCreateTempLocationIn info(aData, aDataSize);
	if(!info.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CErrorCreateTempLocationIn");
		return;
	}
	if( pCallbacks != NULL)
		pCallbacks->onErrorCreateTempLocation( info.GetErrorCode());
}

void CSyncManager::OnRecordEditorCutComplete( BYTE* aData, int aDataSize)
{
	if ( pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordEditorCutIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordEditorCutIn");
		return;
	}

	pContext->mpSM->OnCutOperationComplete( packetIn.GetErrorCode(), packetIn.GetRecordDuration());
}

void CSyncManager::OnRecordEditorCopyComplete( BYTE* aData, int aDataSize)
{
	if ( pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordEditorCopyIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordEditorCopyIn");
		return;
	}

	pContext->mpSM->OnCopyOperationComplete( packetIn.GetErrorCode());
}

void CSyncManager::OnRecordEditorPasteComplete( BYTE* aData, int aDataSize)
{
	if ( pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordEditorPasteIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordEditorPasteIn");
		return;
	}

	pContext->mpSM->OnPasteOperationComplete( packetIn.GetErrorCode(), packetIn.GetRecordDuration());
}

void CSyncManager::OnRecordEditorDeleteComplete( BYTE* aData, int aDataSize)
{
	if ( pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordEditorDeleteIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordEditorDeleteIn");
		return;
	}

	pContext->mpSM->OnDeleteOperationComplete( packetIn.GetErrorCode(), packetIn.GetRecordDuration());
}

void CSyncManager::OnRecordEditorSaveComplete( BYTE* aData, int aDataSize)
{
	if ( pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordEditorSaveIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordEditorSaveIn");
		return;
	}

	pContext->mpSM->OnSaveOperationComplete( packetIn.GetErrorCode());
}

void CSyncManager::OnRecordEditorSaveAsComplete( BYTE* aData, int aDataSize)
{
	if ( pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordEditorSaveAsIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordEditorSaveAsIn");
		return;
	}

	pContext->mpSM->OnSaveAsOperationComplete( packetIn.GetErrorCode());
}

void CSyncManager::OnRecordEditorPlayComplete( BYTE* aData, int aDataSize)
{
	if ( pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordEditorPlayIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordEditorPlayIn");
		return;
	}

	pContext->mpSM->OnPlayOperationComplete( packetIn.GetErrorCode());
}

void CSyncManager::OnRecordEditorPauseComplete( BYTE* aData, int aDataSize)
{
	if ( pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordEditorPauseIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordEditorPauseIn");
		return;
	}

	pContext->mpSM->OnPauseOperationComplete( packetIn.GetErrorCode());
}

void CSyncManager::OnRecordEditorRewindComplete( BYTE* aData, int aDataSize)
{
	if ( pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordEditorRewindIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordEditorRewindIn");
		return;
	}

	pContext->mpSM->OnRewindOperationComplete( packetIn.GetErrorCode());
}

void CSyncManager::OnRecordEditorSeekComplete( BYTE* aData, int aDataSize)
{
	if ( pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordEditorSeekIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordEditorSeekIn");
		return;
	}

	pContext->mpSM->OnSeekOperationComplete( packetIn.GetErrorCode());
}

void CSyncManager::OnRecordEditorUndoComplete( BYTE* aData, int aDataSize)
{
	if ( pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordEditorUndoIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordEditorUndoIn");
		return;
	}

	pContext->mpSM->OnUndoOperationComplete( packetIn.GetErrorCode(), packetIn.GetRecordDuration());
}

void CSyncManager::OnRecordEditorRedoComplete( BYTE* aData, int aDataSize)
{
	if ( pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordEditorRedoIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordEditorRedoIn");
		return;
	}

	pContext->mpSM->OnRedoOperationComplete( packetIn.GetErrorCode(), packetIn.GetRecordDuration());
}

void CSyncManager::OnRecordEditorOpenComplete( BYTE* aData, int aDataSize)
{
	if ( pContext == NULL || pContext->mpSM == NULL)
		return;

	CRecordEditorOpenIn packetIn(aData, aDataSize);
	if(!packetIn.Analyse())
	{
		if(pContext->mpLogWriter!=NULL)
			pContext->mpLogWriter->WriteLnLPCSTR("[ANALYSE FALSE] CRecordEditorOpenIn");
		return;
	}

	pContext->mpSM->OnOpenOperationComplete( packetIn.GetErrorCode(), packetIn.GetRecordName(), packetIn.GetRecordDescription(), packetIn.GetRecordDuration(), packetIn.GetRecordAuthor(), packetIn.GetRecordCreationDate(), packetIn.GetRecordLocation(), packetIn.GetRecordLanguage(), packetIn.GetRecordURL());
}

void CSyncManager::OnAnySyncMessageReceived( BYTE aMessageID, BYTE* aData, int aDataSize, int aCheckSum)
{
	ATLASSERT( pContext != NULL);
	ATLASSERT( pContext->mpMapMan != NULL);
	
	MP_NEW_P2( pPackageIn, CTempPacketIn, aData, aDataSize);
	if (!pPackageIn)
		return;

	ReceivedMessage	receivedMessage;
	receivedMessage.uiMessageID = aMessageID;
	receivedMessage.uiCheckSum = aCheckSum;
	receivedMessage.pCommonPacketIn = pPackageIn;

	EnterCriticalSection(&syncMessageCS);
	m_receivedMessages.push_back( receivedMessage);
	LeaveCriticalSection(&syncMessageCS);
}

void CSyncManager::SetStateToAnalysisObjectsList(bool state)
{
	m_isAsynchAnalysisObjectsList = state;
}

bool CSyncManager::GetStateToAnalysisObjectsList()
{
	return m_isAsynchAnalysisObjectsList;
}

// выполняет обработку всех поступивших за кадр сообщений от сервера синхронизации, которые несут инфу о состояниях объектов
void CSyncManager::ProcessSyncMessages()
{
	if (m_isAsynchAnalysisObjectsList)
		return;

	EnterCriticalSection(&syncMessageCS);
	VecReceivedMessageIt it = m_receivedMessages.begin();
	for( ; it!=m_receivedMessages.end(); it++)
	{
		if (m_isAsynchAnalysisObjectsList)
		{
			m_receivedMessages.erase( m_receivedMessages.begin(), it);
			LeaveCriticalSection(&syncMessageCS);
			return;
		}

		ReceivedMessage* apReceivedMessage = (ReceivedMessage*)&(*it);

		BYTE* pData = apReceivedMessage->pCommonPacketIn->GetDataBuffer();
		int iDataSize = apReceivedMessage->pCommonPacketIn->GetDataSize();
		switch( apReceivedMessage->uiMessageID)
		{
		case RT_UpdatePacket:
			OnUpdatePacket( pData, iDataSize, apReceivedMessage->uiCheckSum);
			break;
		case RT_BadUpdatePacket:
			OnBadUpdatePacket( pData, iDataSize);
			break;
		case RT_DeleteObject:
			OnDeleteObject( pData, iDataSize);
			break;
		default:
			if( MRT_FIRST <= apReceivedMessage->uiMessageID
				&&  MRT_LAST >= apReceivedMessage->uiMessageID)
			{
				pContext->mpMapMan->HandleServerMessage( apReceivedMessage->uiMessageID, pData, iDataSize);
			}
		}
		MP_DELETE( apReceivedMessage->pCommonPacketIn);
	}
	m_receivedMessages.clear();
	LeaveCriticalSection(&syncMessageCS);
}

// вернуть сессию для отправки сообщений. Временное решение
cs::iclientsession* CSyncManager::GetServerSession()
{
	return pClientSession;
}

// код ошибки при отмены авторизации
void CSyncManager::OnLogout( int iCode)
{
	ZONE_OBJECT_INFO* info = NULL;
	while (mapObjectQueue.Pop(info))
	{
		mapObjectQueue.DeleteObjectInfo( info);
	}
}

void CSyncManager::OnLocationsListChanged()
{
	mapObjectQueue.OnLocationsListChanged();
}

void CSyncManager::writeLog( const char* alpcLog)
{
	if( pContext->mpLogWriter != NULL)
		pContext->mpLogWriter->WriteLn( alpcLog);
}

/*void CSyncManager::CheckHashAvatarsThread()
{
	while( bCheckHash)
	{
		std::vector<mlObjectInfo> vElements;
		pContext->mpSM->GetRMMLObjectTree( vElements, 2);
		for(int i=0;i<vElements.size();i++)
		{
			//AddObject( &vElements[i]);
		}
		WaitForSingleObject( eventCheckHash, 5000);	
	}

}

void CSyncManager::CheckHashObjectsThread()
{
	while( bCheckHash)
	{
		std::vector<mlObjectInfo> vElements;
		pContext->mpSM->GetRMMLObjectTree( vElements, 1);
		for(int i=0;i<vElements.size();i++)
		{
			//AddObject( &vElements[i]);
		}
		WaitForSingleObject( eventCheckHash,30000);	
	}}*/
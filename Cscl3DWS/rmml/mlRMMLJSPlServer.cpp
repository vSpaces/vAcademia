#include "mlRMML.h"
#include "config/oms_config.h"
#include "SyncMan.h"
#include "mlLogRecord.h"
#include "mlJSONRPCJSO.h"
#include "config/prolog.h"
#include "ILogWriter.h"
#include "mdump.h"

namespace rmml {

/**
 * Реализация глобального JS-объекта "Server"
 */

JSBool JSServerGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool JSServerSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){ return JS_TRUE; }
	
JSClass JSRMMLServerClass = {
	"_Server", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, 
	JSServerGetProperty, JSServerSetProperty,
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL,
	NULL, NULL, NULL
};

enum {
	JSPROP_connectionStatus=1,
	JSPROP_name,
	JSPROP_ip,
	JSPROP_clientStringID,
	TIDE_mlRMMLServer
};

enum { //EVID_mlRMMLServer=TIDE_mlRMMLServer-1,
	EVID_onConnected=1,
	EVID_onConnectionFailed,
	EVID_onDisconnected,
	EVID_onServerRestart,
	EVID_onLogout,
	EVID_onServerTO,
	EVID_onSeancesInfoReceived,
	EVID_onProxyAuthError,
	EVID_onConnectToServerError,
	EVID_onErrorCreateTempLocation,
	EVID_onEventsInfoIntoLocationReceived,
	EVID_onIPadSessionChangeSlide,
	EVID_onSyncNotify,
	TEVIDE_mlRMMLServer
};
/*
this.LOGIN_SERVER_STATUS_BIT = 1;
this.RES_SERVER_STATUS_BIT = 2;
this.LOG_SERVER_STATUS_BIT = 4;
this.MAP_SERVER_STATUS_BIT = 8;
this.SYNC_SERVER_STATUS_BIT = 16;
this.SERVICE_SERVER_STATUS_BIT = 32;
#define	VOIP_RECORD_SERVER_STATUS_BIT	0x40
#define	VOIP_PLAY_SERVER_STATUS_BIT		0x80
*/

JSPropertySpec _JSServerPropertySpec[] = {
	JSPROP_SC(LOGIN_SERVER, 1)
	JSPROP_SC(RES_SERVER, 2)
	JSPROP_SC(LOG_SERVER, 3)
	JSPROP_SC(MAP_SERVER, 4)
	JSPROP_SC(SYNC_SERVER, 5)
	JSPROP_SC(SERVICE_SERVER, 6)
	JSPROP_SC(VOIP_RECORD_SERVER, 7)
	JSPROP_SC(VOIP_PLAY_SERVER, 8)
	JSPROP_RO(connectionStatus)	// статус соединения (0 - нет соединения, 1 - есть соединение, 2 - соединение разорвано)
	JSPROP_RO(name) // доменноое имя сервера
	JSPROP_RO(ip) // IP-адрес сервера
	JSPROP_RO(clientStringID)	// строковый идентификатор клиента (делается из логина и возвращается сервером)
	{ 0, 0, 0, 0, 0 }
};

JSFUNC_DECL(query)
JSFUNC_DECL(queryPost)
JSFUNC_DECL(sendQuery)
JSFUNC_DECL(login)
JSFUNC_DECL(logout)
JSFUNC_DECL(createLocation)
JSFUNC_DECL(deleteLocation)

////////////////////////soap/////////
JSFUNC_DECL(getService)

JSFUNC_DECL(startRecording)
JSFUNC_DECL(saveRecord)
JSFUNC_DECL(startRecordPlaying)
JSFUNC_DECL(stopRecordPlaying)
JSFUNC_DECL(getProxiesMask)
JSFUNC_DECL(setProxyAuthorization)
JSFUNC_DECL(isWinSocketConnection)
JSFUNC_DECL(startIpadSession)
JSFUNC_DECL(destroyIpadSession)
JSFUNC_DECL(setIPadImageParam)


///// JavaScript Function Table
JSFunctionSpec _JSServerFunctionSpec[] = {
	JSFUNC(query,2)		// запросить с сервера данные (возвращает данные)
	JSFUNC(queryPost,2)		// запросить с сервера данные (возвращает данные)
	JSFUNC(sendQuery,2) // послать на сервер запрос (возвращает JS-объект)
	JSFUNC(login,6)		// авторизация пользователя на сервере CSCL
	JSFUNC(logout,0)	// закрыть сессию
	JSFUNC(createLocation,0) // создать временную локацию
	JSFUNC(deleteLocation,0) // создать временную локацию

////////////////////////soap/////////
	JSFUNC(getService,0)

	JSFUNC(startRecording, 0)	// начать запись лога (поставить отметку в логе о начале записи)
	JSFUNC(saveRecord, 1)		// сохранить запись в логе 
	JSFUNC(startRecordPlaying, 1)	// начать воспроизведение записи в логе
	JSFUNC(stopRecordPlaying, 0)	// остановить воспроизведение записи в логе
	JSFUNC(getProxiesMask, 0)	// вернуть максу по используемым прокси серверам
	JSFUNC(setProxyAuthorization, 3) // установить логин пароль для авторизации прокси
	JSFUNC(isWinSocketConnection, 0) // узнать используем ли прокси
	JSFUNC(startIpadSession, 2) // начать сессию ipad с объектом	
	JSFUNC(destroyIpadSession, 0) // закончить сессию ipad с объектом
	JSFUNC(setIPadImageParam, 4) // передать src картинки ipad-у

	{ 0, 0, 0, 0, 0 }
};

void CreateServerJSObject(JSContext *cx, JSObject *ajsoPlayer){
    JSObject* jsoServer = JS_DefineObject(cx, ajsoPlayer, GJSONM_SERVER, &JSRMMLServerClass, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
	JSBool bR;
	bR=JS_DefineProperties(cx, jsoServer, _JSServerPropertySpec);
	bR=JS_DefineFunctions(cx, jsoServer, _JSServerFunctionSpec);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, ajsoPlayer);
	JS_SetPrivate(cx,jsoServer,pSM);
	CreateSRecorderJSObject(cx, jsoServer);
	pSM->mjsoServer = jsoServer;
	//
}

JSBool JSServerGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){ 
	if(JSVAL_IS_INT(id)){
		int iID=JSVAL_TO_INT(id);
		if(iID & JSPROPSC_ID_MASK){
			int iServerID = (iID & ~JSPROPSC_ID_MASK);
			int iServerMask = 1 << (iServerID - 1); // возвращаем маску сервера (как в connectionStatus)
			*vp = INT_TO_JSVAL(iServerMask);
			return JS_TRUE;
		}
		mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
		cm::cmICommunicationManager* pComMan = pSM->GetContext()->mpComMan;
		if(pComMan == NULL){
			JS_ReportError(cx, "Communication manager is not set");
			return JS_TRUE;
		}
		switch(iID){
		case JSPROP_connectionStatus:{
			unsigned int iStatus = pComMan->GetConnectionStatus();
			*vp = INT_TO_JSVAL(iStatus);
			}break;
		case JSPROP_name:
			// ??
			break;
		case JSPROP_ip:
			// ??
			break;
		case JSPROP_clientStringID:
			const wchar_t* pwcClientStringID = pSM->GetClientStringID();
			if(pwcClientStringID == NULL){
				*vp = JS_GetEmptyStringValue(cx);
			}else{
				*vp = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwcClientStringID));
			}
			break;
		}
		// уведомляем слушателей
		char chEvID=iID-1;
//		pSM->NotifyModuleListeners(chEvID);
	}
	return JS_TRUE; 
}

void TraceServerError(JSContext *cx, int aiErrorCode){
	if(aiErrorCode == 0) return;
	const char* pcErrorText = "";

//	qeNoError = 0, qeServerError, qeQueryNotFound, qeScriptError, qeReadError,
//	qeNotImplError, qeServerNotResolved, qeTimeOut

	switch(aiErrorCode){
	case 1: // qeServerError
		pcErrorText = "Unknown server error";
		break;
	case 2: // qeQueryNotFound
		pcErrorText = "Query not found";
		break;
	case 3: // qeScriptError
		pcErrorText = "Script error";
		break;
	case 4: // qeReadError
		pcErrorText = "Read error";
		break;
	case 5: // qeNotImplError
		pcErrorText = "Not implemented";
		break;
	case 6: // qeServerNotResolved
		pcErrorText = "Server name is not resolved";
		break;
	case 7: // qeTimeOut
		pcErrorText = "Time out while waiting query response";
		break;
	}
	if(*pcErrorText != '\0')
		JS_ReportWarning(cx, "Server error (%d): %s ", aiErrorCode, pcErrorText);
	else
		JS_ReportWarning(cx, "Server error code: %d", aiErrorCode);
}

char GetServerEventID(const wchar_t* apwcEventName){
	if(isEqual(apwcEventName,L"onConnected"))
		return (char)EVID_onConnected;
	if(isEqual(apwcEventName,L"onConnectionFailed"))
		return (char)EVID_onConnectionFailed;
	if(isEqual(apwcEventName,L"onDisconnected"))
		return (char)EVID_onDisconnected;
	if(isEqual(apwcEventName,L"onRestart"))
		return (char)EVID_onServerRestart;
	if(isEqual(apwcEventName,L"onLogout"))
		return (char)EVID_onLogout;
	if(isEqual(apwcEventName,L"onServerTO"))
		return (char)EVID_onServerTO;
	if(isEqual(apwcEventName,L"onSeancesInfoReceived"))
		return (char)EVID_onSeancesInfoReceived;
	if(isEqual(apwcEventName,L"onProxyAuthError"))
		return (char)EVID_onProxyAuthError;
	if(isEqual(apwcEventName,L"onConnectToServerError"))
		return (char)EVID_onConnectToServerError;
	if(isEqual(apwcEventName,L"onErrorCreateTempLocation"))
		return (char)EVID_onErrorCreateTempLocation;
	if(isEqual(apwcEventName,L"onEventsInfoIntoLocationReceived"))
		return (char)EVID_onEventsInfoIntoLocationReceived;
	if(isEqual(apwcEventName,L"onIPadSessionChangeSlide"))
		return (char)EVID_onIPadSessionChangeSlide;
	if(isEqual(apwcEventName,L"onSyncNotify"))
		return (char)EVID_onSyncNotify;

	return -1;
}

wchar_t* GetServerEventName(char aidEvent){
	switch(aidEvent){
	case (char)EVID_onConnected:
		return L"onConnected";
	case (char)EVID_onConnectionFailed:
		return L"onConnectionFailed";
	case (char)EVID_onDisconnected:
		return L"onDisconnected";
	case (char)EVID_onServerRestart:
		return L"onRestart";
	case (char)EVID_onLogout:
		return L"onLogout";
	case (char)EVID_onServerTO:
		return L"onServerTO";
	case (char)EVID_onSeancesInfoReceived:
		return L"onSeancesInfoReceived";
	case (char)EVID_onProxyAuthError:
		return L"onProxyAuthError";
	case (char)EVID_onConnectToServerError:
		return L"onConnectToServerError";
	case (char)EVID_onErrorCreateTempLocation:
		return L"onErrorCreateTempLocation";
	case (char)EVID_onEventsInfoIntoLocationReceived:
		return L"onEventsInfoIntoLocationReceived";
	case (char)EVID_onIPadSessionChangeSlide:
		return L"onIPadSessionChangeSlide";
	case (char)EVID_onSyncNotify:
		return L"onSyncNotifye";		

	}
	return L"???";
}

bool mlSceneManager::addServerListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	char chEvID=aidEvent-1;
	if(chEvID >= ML_SERVER_EVENTS_COUNT) return false;
	v_elems* pv=&(maServerListeners[chEvID]);
	v_elems::const_iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl) return true;
	}
	pv->push_back(apMLEl);
	return true;
}

void mlSceneManager::NotifyServerListeners(char chEvID){
	if(chEvID < ML_SERVER_EVENTS_COUNT){
		v_elems* pv=&(maServerListeners[chEvID-1]);
		v_elems::const_iterator vi;
		for(vi=pv->begin(); vi != pv->end(); vi++ ){
			mlRMMLElement* pMLEl=*vi;
			pMLEl->EventNotify(chEvID,(mlRMMLElement*)GOID_Server);
		}
	}
}

bool mlSceneManager::removeServerListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	if(aidEvent==-1){
		// и удалим его из списка слушателей всех событий
		for(int ii=0; ii<ML_SERVER_EVENTS_COUNT; ii++){
			v_elems* pv=&maServerListeners[ii];
			v_elems::iterator vi;
			for(vi=pv->begin(); vi != pv->end(); vi++ ){
				if(*vi==apMLEl){
					pv->erase(vi);
					break;
				}
			}
		}
		return true;
	}
	v_elems* pv=&(maServerListeners[aidEvent-1]);
	v_elems::iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl){
			pv->erase(vi);
			return true;
		}
	}
	return true;
}


// установлена (или восстановлена) связь с одним из серверов
void mlSceneManager::OnServerConnected(int aServerMask, bool abRestored){
	mlSynchData oData;
	oData << aServerMask;
	oData << abRestored;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_serverOnConnected, (unsigned char*)oData.data(), oData.size());
}

// установка (или восстановление) связи с одним из серверов закончилось неудачно
void mlSceneManager::OnServerConnectionError(int aServerMask, int aiError){
	mlSynchData oData;
	oData << aServerMask;
	oData << aiError;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_serverOnConnectionFailed, (unsigned char*)oData.data(), oData.size());
}

// потеряна связь с одним из серверов
void mlSceneManager::OnServerDisconnected(int aServerMask, int aiCause){
	mlSynchData oData;
	oData << aServerMask;
	oData << aiCause;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_serverOnDisconnected, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::OnServerConnectedInternal(mlSynchData& aData){
	int iServerMask = 0;
	aData >> iServerMask;
	bool bRestored = false;
	aData >> bRestored;
	jsval jsv = INT_TO_JSVAL(iServerMask);
	JS_SetProperty(cx, GetEventGO(), "server", &jsv);
	jsv = BOOLEAN_TO_JSVAL(bRestored);
	JS_SetProperty(cx, GetEventGO(), "restored", &jsv);
	NotifyServerListeners(EVID_onConnected);
	JS_DeleteProperty(cx, GetEventGO(), "server");
	JS_DeleteProperty(cx, GetEventGO(), "restored");
}

void mlSceneManager::OnServerConnectionErrorInternal(mlSynchData& aData){
	int iServerMask = 0;
	aData >> iServerMask;
	int iError = 0;
	aData >> iError;
	jsval jsv = INT_TO_JSVAL(iServerMask);
	JS_SetProperty(cx, GetEventGO(), "server", &jsv);
	jsv = INT_TO_JSVAL(iError);
	JS_SetProperty(cx, GetEventGO(), "error", &jsv);
	NotifyServerListeners(EVID_onConnectionFailed);
	JS_DeleteProperty(cx, GetEventGO(), "server");
	JS_DeleteProperty(cx, GetEventGO(), "error");
}

// потеряна связь с одним из серверов
void mlSceneManager::OnIPadSessionChangeSlide( int changeSlide){
	mlSynchData oData;
	oData << changeSlide;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_serverIPadSessionChangeSlide, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::OnIPadSessionChangeSlideInternal(mlSynchData& aData){
	int changeSlide = 0;
	aData >> changeSlide;	
	jsval jsv = INT_TO_JSVAL(changeSlide);
	JS_SetProperty(cx, GetEventGO(), "deltaSlide", &jsv);
	NotifyServerListeners(EVID_onIPadSessionChangeSlide);
	JS_DeleteProperty(cx, GetEventGO(), "deltaSlide");	
}

// удаляет объект
omsresult mlSceneManager::DeleteObjectMyAvatar( mlMedia* apObject)
{
	GUARD_JS_CALLS(cx);
	mlRMMLElement* pMLEl = (mlRMMLElement*)apObject;


	/*if (mpContext->mpMapMan->IsAvatar( pMLEl))
	{
		CLogValue logValue("[ERROR][RMML.OBJECTS]: Can not  MP_DELETE( my avatar in mlSceneManager::DeleteObject");
		mpContext->mpLogWriter->WriteLn( logValue);
		return OMS_ERROR_FAILURE;
	}*/
	//mpContext->mpMapMan->DeleteAvatar( pMLEl);

	NotifyLinksElemDestroying(pMLEl);

	// вызвать функцию изменения статуса
	jsval vFunc=JSVAL_NULL;
#define ON_MODESTROYING_FUNC_NAME L"onDestroying"
	if(wr_JS_GetUCProperty(cx, pMLEl->mjso, ON_MODESTROYING_FUNC_NAME, -1, &vFunc))
	{
		if(JSVAL_IS_REAL_OBJECT(vFunc)){
			if(JS_ObjectIsFunction(cx,JSVAL_TO_OBJECT(vFunc))){
				unsigned char ucArgC = 0;
				jsval* vArgs=NULL;
				jsval v;
				JS_CallFunctionValue(cx, pMLEl->mjso, vFunc, ucArgC, vArgs, &v);
			}
		}
	}

	ResetMouse(pMLEl);
	scriptedObjectManager.UnregisterObject( pMLEl);
	pMLEl->SetParent( NULL);
	ResetMouse(pMLEl);
//	if(appaObjects == NULL){
		pMLEl->ForceMODestroy();
/*	}else{
		mvMOToDeleteOut.clear();
		pMLEl->ForceMODestroy(&mvMOToDeleteOut);
		mvMOToDeleteOut.push_back(NULL);
		*appaObjects = &mvMOToDeleteOut[0];
	}*/
	CallGC(pMLEl);
	//LogRefs( cx, NULL, pMLEl->mjso);
	// ?? на объект могут остаться прямые ссылки, 
	// поэтому нужен механизм постепенного их нахождения и удаления

	CLogValue logValue("[RMML.OBJECTS]: my Avatar '", (const char*)cLPCSTR( pMLEl->GetName()), "' has been deleted.");
	mpContext->mpLogWriter->WriteLn( logValue);
	return OMS_OK;
}

omsresult mlSceneManager::DeleteObjectsOnLogout()
{
	GUARD_JS_CALLS(cx);

	if (mpMyAvatar == NULL)
		return OMS_OK;

	CLogValue logValue(
		"[RMML.OBJECTS]: DeleteObjectsOnLogout"
		);
	mpContext->mpLogWriter->WriteLn( logValue);

	ObjectsVector vObjectsToDelete;
	scriptedObjectManager.GetObjects( vObjectsToDelete);

	ObjectsVector::iterator itObjectsToDelete = vObjectsToDelete.begin();
	for( ; itObjectsToDelete != vObjectsToDelete.end(); itObjectsToDelete++)
	{
		mlRMMLElement* pMLEl = (*itObjectsToDelete);
		if (pMLEl == NULL)
			continue;		
		if (pMLEl == mpMyAvatar)
		{
			/*if (mpContext->mpMapMan->IsAvatar( pMLEl)){
				mlTraceError( cx, "[RMML.OBJECTS] Can not delete my avatar (objectID: %u\n)", auObjectID);
				//return;
			}*/

			CLogValue logValue(
				"[RMML.OBJECTS] my Avatar ", (LPCSTR)cLPCSTR( pMLEl->GetName()), " deleting"
				);
			mpContext->mpLogWriter->WriteLnLPCSTR( logValue.GetData());

			DeleteObjectMyAvatar( pMLEl);
			mpMyAvatar = NULL;
			continue;
		}
		if (!pMLEl->IsSynchronized())
			continue;
		DeleteObjectInternal( pMLEl->GetID(), pMLEl->GetBornRealityID());
	}

	return OMS_OK;
}

void mlSceneManager::OnLogoutInternal(mlSynchData& aData){
	int iCodeError = 0;
	aData >> iCodeError;

	if(mpContext->mpApp != NULL)
		mpContext->mpApp->OnLogout( iCodeError);

	DeleteObjectsOnLogout();
	m_isMyAvatarCreated = false;

	jsval vCodeError = JSVAL_NULL;
	vCodeError = INT_TO_JSVAL(iCodeError);
	JS_SetProperty(cx, GetEventGO(), "errorCode", &vCodeError );
	NotifyServerListeners(EVID_onLogout);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
}

void mlSceneManager::OnNotifyServerTOInfoInternal(mlSynchData& aData){
	int iMinutesTO = 0;
	aData >> iMinutesTO;

	/*if(mpContext->mpApp != NULL)
		mpContext->mpApp->OnLogout( iCodeError);

	DeleteObjectsOnLogout();
	m_isMyAvatarCreated = false;*/

	jsval vMinutesTO = JSVAL_NULL;
	vMinutesTO = INT_TO_JSVAL(iMinutesTO);
	JS_SetProperty(cx, GetEventGO(), "minutesTO", &vMinutesTO );
	NotifyServerListeners(EVID_onServerTO);
	JS_DeleteProperty(cx, GetEventGO(), "minutesTO");
}

void mlSceneManager::OnSeancesInfoReceivedInternal(mlSynchData& aData)
{
	wchar_t* seancesInfo;
	bool bFull;
	aData >> seancesInfo;	
	aData >> bFull;
	mlString sDescription(L"[");
	sDescription = sDescription + seancesInfo+ L"]";

	unsigned int uLength = sDescription.size();
	//JSContext *mcx = (mpSM->GetJSContext());
	jsval v;
	JSBool bR = JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), (const jschar*)sDescription.c_str(), uLength, "seancesInfo", 1, &v);
	if (JSVAL_IS_REAL_OBJECT(v))
	{	
		JSObject *obj=JSVAL_TO_OBJECT(v);
		JS_GetElement(cx,obj,0,&v);
		if (JSVAL_IS_REAL_OBJECT(v))
		{
			JS_SetProperty(cx, GPSM(cx)->GetEventGO(), "seancesInfo", &v);
			JSObject *objSeancesInfo = JSVAL_TO_OBJECT(v);
			jsval v1 = BOOLEAN_TO_JSVAL(bFull);
			JS_SetProperty(cx, objSeancesInfo, "isFull", &v1);
			NotifyServerListeners(EVID_onSeancesInfoReceived);
			JS_DeleteProperty(cx, GPSM(cx)->GetEventGO(), "seancesInfo");
			JS_DeleteProperty(cx, GPSM(cx)->GetEventGO(), "isFull");
		}
	}
}

void mlSceneManager::OnEventsInfoIntoLocationReceivedInternal(mlSynchData& aData)
{
	wchar_t* seancesInfo;
	bool bFull;
	aData >> seancesInfo;	
	aData >> bFull;
	mlString sDescription(L"[");
	sDescription = sDescription + seancesInfo+ L"]";

	unsigned int uLength = sDescription.size();
	//JSContext *mcx = (mpSM->GetJSContext());
	jsval v;
	JSBool bR = JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), (const jschar*)sDescription.c_str(), uLength, "seancesInfo", 1, &v);
	if (JSVAL_IS_REAL_OBJECT(v))
	{	
		JSObject *obj=JSVAL_TO_OBJECT(v);
		JS_GetElement(cx,obj,0,&v);
		if (JSVAL_IS_REAL_OBJECT(v))
		{
			JS_SetProperty(cx, GPSM(cx)->GetEventGO(), "eventsInfoIntoLocation", &v);
			JSObject *objSeancesInfo = JSVAL_TO_OBJECT(v);
			jsval v1 = BOOLEAN_TO_JSVAL(bFull);			
			NotifyServerListeners(EVID_onEventsInfoIntoLocationReceived);
			JS_DeleteProperty(cx, GPSM(cx)->GetEventGO(), "eventsInfoIntoLocation");
		}
	}
}

void mlSceneManager::OnProxyAuthErrorInternal(mlSynchData& aData)
{
	wchar_t* sSettingName;
	int iProxyType;
	wchar_t* sProxyServer;

	aData >> sSettingName;
	JSString* jssSchemeName = wr_JS_NewUCStringCopyZ(cx, sSettingName);
	jsval vSchemeName = STRING_TO_JSVAL(jssSchemeName);

	aData >> iProxyType;
	jsval vType = INT_TO_JSVAL(iProxyType);

	aData >> sProxyServer;
	JSString* jssServer = wr_JS_NewUCStringCopyZ(cx, sProxyServer);
	jsval vServer = STRING_TO_JSVAL(jssServer);


	JS_SetProperty(cx, GetEventGO(), "proxyType", &vType);
	JS_SetProperty(cx, GetEventGO(), "settingName", &vSchemeName);
	JS_SetProperty(cx, GetEventGO(), "proxyServer", &vServer);
	NotifyServerListeners(EVID_onProxyAuthError);
	JS_DeleteProperty(cx, GetEventGO(), "proxyType");
	JS_DeleteProperty(cx, GetEventGO(), "settingName");
	JS_DeleteProperty(cx, GetEventGO(), "proxyServer");
}

void mlSceneManager::OnConnectToServerErrorInternal(mlSynchData& aData)
{
	int errorCode;
	aData >> errorCode;
	jsval vErrorCode = INT_TO_JSVAL(errorCode);

	JS_SetProperty(cx, GetEventGO(), "errorCode", &vErrorCode);
	NotifyServerListeners(EVID_onConnectToServerError);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
}

enum SERVER_ERRORS {SERVER_RESTART };

void mlSceneManager::onSyncServerErrorInternal(mlSynchData& aData)
{
	// Читаем код ошибки от серваера
	unsigned int iServerError;
	aData >> iServerError;

	if (iServerError==SERVER_RESTART)
	{
		NotifyServerListeners(EVID_onServerRestart);
	}

}

void mlSceneManager::OnServerDisconnectedInternal(mlSynchData& aData){
	int iServerMask = 0;
	aData >> iServerMask;
	int iCause = 0;
	aData >> iCause;
	jsval jsv = INT_TO_JSVAL(iServerMask);
	JS_SetProperty(cx, GetEventGO(), "server", &jsv);
	jsv = INT_TO_JSVAL(iCause);
	JS_SetProperty(cx, GetEventGO(), "cause", &jsv);
	NotifyServerListeners(EVID_onDisconnected);
	JS_DeleteProperty(cx, GetEventGO(), "server");
	JS_DeleteProperty(cx, GetEventGO(), "cause");
}

// abGetQuery == true - GET Query
// abGetQuery == false - POST Query
JSBool JSFUNC_queryAny(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval, bool abGetQuery){
	*rval = JSVAL_NULL;
	if(argc < 1 || !JSVAL_IS_STRING(argv[0]) || argc > 2){
		JS_ReportError(cx, "query method must get one or two string arguments");
		return JS_TRUE;
	}
	JSString* jssQuery = JSVAL_TO_STRING(argv[0]);
	JSString* jssParams = NULL;
	if(argc >= 2){
		if(!JSVAL_IS_STRING(argv[1])){
			JS_ReportError(cx, "Second argument of query method must be a string");
			return JS_TRUE;
		}
		jssParams = JSVAL_TO_STRING(argv[1]);
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpComMan == NULL){
		JS_ReportError(cx, "Communication manager is not set");
		return JS_TRUE;
	}
	wchar_t* jscQuery = wr_JS_GetStringChars(jssQuery);
	wchar_t* jscParams = jssParams == NULL ? NULL : wr_JS_GetStringChars(jssParams);
	int iErrorCode = 0;
	const wchar_t* pwcResult = NULL;
	if( abGetQuery)
		pwcResult = pSM->GetContext()->mpComMan->Query(jscQuery, jscParams, &iErrorCode);
	else
		pwcResult = pSM->GetContext()->mpComMan->QueryPost(jscQuery, jscParams, &iErrorCode);
	if(iErrorCode > 0)
		TraceServerError(cx, iErrorCode);
	if(pwcResult != NULL){
		*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwcResult));
	}
	return JS_TRUE;
}

JSBool JSFUNC_query(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	return JSFUNC_queryAny( cx, obj, argc, argv, rval, true/*GET query*/);
}

JSBool JSFUNC_queryPost(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	return JSFUNC_queryAny( cx, obj, argc, argv, rval, false/*POST query*/);
}

JSBool JSFUNC_login(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	if(argc < 1 || argc > 6){
		JS_ReportError(cx, "login method must get two or four or six arguments");
		return JS_TRUE;
	}
	if (!JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx, "login method must get first string argument");
		return JS_TRUE;
	}
	if (!JSVAL_IS_STRING(argv[1])){
		JS_ReportError(cx, "login method must get second string argument");
		return JS_TRUE;
	}
	if (!JSVAL_IS_BOOLEAN(argv[2])){
		JS_ReportError(cx, "login method must get third bool argument");
		return JS_TRUE;
	}
	if (!JSVAL_IS_BOOLEAN(argv[3])){
		JS_ReportError(cx, "login method must get four bool argument");
		return JS_TRUE;
	}
	if (!JSVAL_IS_STRING(argv[4])){
		JS_ReportError(cx, "login method must get fifth string argument as loginKey");
		return JS_TRUE;
	}
	if (!JSVAL_IS_STRING(argv[5])){
		JS_ReportError(cx, "login method must get sixth string argument as updaterKey");
		return JS_TRUE;
	}
	JSString* jssLogin = JSVAL_TO_STRING(argv[0]);
	JSString* jssPassword = JSVAL_TO_STRING(argv[1]);
	JSBool jssIsAnonymous = JSVAL_TO_BOOLEAN(argv[2]);
	JSBool jssIsEncodingPassword = JSVAL_TO_BOOLEAN(argv[3]);
	JSString* jssLoginKey = JSVAL_TO_STRING(argv[4]);
	JSString* jssUpdaterKey = JSVAL_TO_STRING(argv[5]);

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpComMan == NULL){
		JS_ReportError(cx, "Communication manager is not set");
		return JS_TRUE;
	}
	wchar_t* jscLogin = wr_JS_GetStringChars(jssLogin);
	wchar_t* jscPassword = jssPassword == NULL ? NULL : wr_JS_GetStringChars(jssPassword);
	wchar_t* jscLoginKey = wr_JS_GetStringChars(jssLoginKey);
	wchar_t* jscUpdaterKey = wr_JS_GetStringChars(jssUpdaterKey);
	GPSM(cx)->SetServiceCallbacks();

	std::wstring wsPassword = jscPassword;	
	int passwordLength = wsPassword.length();

	// размер шифрованного пароля
	if (passwordLength == 81)
	{
		jssIsEncodingPassword = 1;
	}
	else
	{
		jssIsEncodingPassword = 0;
	}

	if (pSM->GetContext()->mpLogWriter)
	{
		pSM->GetContext()->mpLogWriter->WriteLn("Length of password in JSFUNC_login: ", passwordLength);
		pSM->GetContext()->mpLogWriter->WriteLn("jssIsEncodingPassword: ", jssIsEncodingPassword);
	}

	if ( !jssIsEncodingPassword)
	{
		if ( pSM->GetContext() != NULL && pSM->GetContext()->mpRM != NULL)
		{
			// пароль не шифрованный			
			pSM->GetContext()->mpRM->GetEncodingPassword(wsPassword);
			jssIsEncodingPassword = 1;
		}
	}

	// устанавливаем здесь параметры для краш-системы	
	SetUserMDump(jscLogin, wsPassword.c_str(), jssIsEncodingPassword);
	
	int iRequestID = GPSM(cx)->GetRequestList()->GetNextServiceIDRequest();
	pSM->GetContext()->mpServiceWorldMan->LoginAsync(iRequestID, jscLogin, wsPassword.c_str(), jssIsAnonymous, -1, jssIsEncodingPassword, jscLoginKey, jscUpdaterKey);
	
	JSObject* jsoRequest = mlHTTPRequest::newJSObject(cx);
	// сохраним ссылку на объект, пока ему не придет ответ с сервера или не возникнет ошибка
	SAVE_FROM_GC(cx, pSM->GetPlServerGO(), jsoRequest)
		mlHTTPRequest* pRequest = (mlHTTPRequest*)JS_GetPrivate(cx, jsoRequest);
	//pRequest->pResultHandler = new mlHTTPResultLogin();
	pRequest->pResultHandler = NULL;
	pRequest->msQuery = L"login.php"; // save debug info
	pRequest->SetID(iRequestID);
	pRequest->SetRequestID(iRequestID); 
	GPSM(cx)->GetRequestList()->AddRequest(pRequest);
	jsval vStatus = INT_TO_JSVAL(1);
	JS_SetProperty(cx, jsoRequest, "status", &vStatus);
	//mlTrace(cx, "sendQuery ID = %d, pointer=%x, php=%S\n", iID, pRequest, pRequest->msQuery.c_str());

	*rval = OBJECT_TO_JSVAL(jsoRequest);
	return JS_TRUE;
}

JSBool JSFUNC_logout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	if(argc > 2){
		JS_ReportError(cx, "logout method must one or not arguments");
		return JS_TRUE;
	}
	jsint jsCode = 0;
	if( argc == 1)
		jsCode = JSVAL_TO_INT(argv[0]);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	//GPSM(cx)->SetServiceCallbacks();
	pSM->GetContext()->mpServiceWorldMan->Logout( jsCode);
	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

JSBool JSFUNC_createLocation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	omsContext* pContext = pSM->GetContext();

	if(argc != 4){
		JS_ReportWarning(cx, "JSFUNC_createLocation-method must get three arguments: two points and location name");
		return JS_TRUE;
	}


	ml3DPosition leftPoint;
	ml3DPosition rightPoint;
	ml3DPosition outsidePoint;

	// 1 argument
	if(!JSVAL_IS_REAL_OBJECT(argv[0])){
		JS_ReportError(cx,"First parameter of createLocation(..) must be a 3D-point object Left-top point");
		return JS_TRUE;
	}

	JSObject* jsoLeftPoint = JSVAL_TO_OBJECT(argv[0]);
	if(!mlPosition3D::IsInstance(cx, jsoLeftPoint)){
		JS_ReportError(cx,"First parameter of createLocation(..) must be a 3D-point object Left-top point");
		return JS_TRUE;
	}

	mlPosition3D* pLeftPoint = (mlPosition3D*)JS_GetPrivate(cx, jsoLeftPoint);
	leftPoint = pLeftPoint->GetPos();
// 2 argument
	if(!JSVAL_IS_REAL_OBJECT(argv[1])){
		JS_ReportError(cx,"Secound parameter of createLocation(..) must be a 3D-point object right-bottom point");
		return JS_TRUE;
	}

	JSObject* jsoRightPoint = JSVAL_TO_OBJECT(argv[1]);
	if(!mlPosition3D::IsInstance(cx, jsoRightPoint)){
		JS_ReportError(cx,"Secound parameter of createLocation(..) must be a 3D-point object right-bottom point");
		return JS_TRUE;
	}

	mlPosition3D* pRightPoint = (mlPosition3D*)JS_GetPrivate(cx, jsoRightPoint);
	rightPoint = pRightPoint->GetPos();

// 3 argument
	if(!JSVAL_IS_REAL_OBJECT(argv[2])){
		JS_ReportError(cx,"Third parameter of createLocation(..) must be a 3D-point object outside point");
		return JS_TRUE;
	}

	JSObject* jsoOutsidePoint = JSVAL_TO_OBJECT(argv[2]);
	if(!mlPosition3D::IsInstance(cx, jsoRightPoint)){
		JS_ReportError(cx,"Third parameter of createLocation(..) must be a 3D-point object outside point");
		return JS_TRUE;
	}

	mlPosition3D* pOutsidePoint = (mlPosition3D*)JS_GetPrivate(cx, jsoRightPoint);
	outsidePoint = pOutsidePoint->GetPos();

// 4 argument		
	if(!JSVAL_IS_STRING(argv[3])){
		JS_ReportError(cx,"Third parameter of createLocation(..) must be a string");
		return JS_TRUE;
	}

	JSString* jssLocationName = JSVAL_TO_STRING(argv[3]);
	wchar_t* jscLocationName = wr_JS_GetStringChars(jssLocationName);

	pSM->GetContext()->mpMapMan->CreateLocation(&leftPoint, &rightPoint, &outsidePoint, jscLocationName);

	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

JSBool JSFUNC_deleteLocation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_FALSE;

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->GetContext()->mpMapMan->DeleteLocation();

	*rval = BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;	
}

JSBool JSFUNC_startRecording(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	JS_ReportError(cx, "Log-manager is not set");
	return JS_TRUE;
}

JSBool JSFUNC_isWinSocketConnection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if( pSM->GetContext()->mpComMan)
	{
		bool bRes = pSM->GetContext()->mpComMan->IsWinSocketConnection();
		if( bRes)
			*rval = JSVAL_TRUE;
	}
	return JS_TRUE;
}

JSBool JSFUNC_startIpadSession(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	if(argc != 2){
		JS_ReportWarning(cx, "JSFUNC_startIpadSession-method must get two arguments: link");
		return JS_TRUE;
	}
	if(!JSVAL_IS_REAL_OBJECT(argv[0]))
	{
		JS_ReportWarning(cx, "JSFUNC_startIpadSession-method argv[0] is not object");
		return JS_TRUE;
	}
	if(!JSVAL_IS_REAL_OBJECT(argv[1]))
	{
		JS_ReportWarning(cx, "JSFUNC_startIpadSession-method argv[1] is not object");
		return JS_TRUE;
	}
	JSObject *jsobj = JSVAL_TO_OBJECT(argv[0]);
	if(!mlSynchLinkJSO::IsInstance( cx, jsobj))
	{
		JS_ReportWarning(cx, "JSFUNC_startIpadSession-method jsobj is not link object");
		return JS_TRUE;
	}

	JSObject *jsobjImg = JSVAL_TO_OBJECT(argv[1]);
	if(!mlSynchLinkJSO::IsInstance( cx, jsobjImg))
	{
		JS_ReportWarning(cx, "JSFUNC_startIpadSession-method jsobj is not link object");
		return JS_TRUE;
	}
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);

	mlSynchLinkJSO *pLink = (mlSynchLinkJSO*)JS_GetPrivate( cx, jsobj); 
	unsigned int bornReality = 0;
	unsigned int objectID = pLink->GetObjectID( bornReality);
	JSObject *jsObject = pLink->GetRef();
	
	mlSynchLinkJSO *pLinkImg = (mlSynchLinkJSO*)JS_GetPrivate( cx, jsobjImg); 
	JSObject *jsObjectImg = pLinkImg->GetRef();
	if(!mlRMMLImage::IsInstance( cx, jsObjectImg))
	{
		JS_ReportWarning(cx, "JSFUNC_startIpadSession-method jsObject is not link object");
		return JS_TRUE;
	}
	mlRMMLImage *pImg = (mlRMMLImage*)JS_GetPrivate( cx, jsObjectImg); 
	moMedia *pMlMedia = pImg->GetMO();
	//mlMedia *pMlMedia = pImg->GetMLMedia();
	mlRMMLElement *pElem = (mlRMMLElement*)JS_GetPrivate( cx, jsObject); 
	wchar_t *pName = pElem->GetName();
	if (pSM->GetContext()->mpIPadMan)
		pSM->GetContext()->mpIPadMan->StartSession( pName, objectID, bornReality, pSM->uCurrentRealityID, pMlMedia);
	return JS_TRUE;
}

JSBool JSFUNC_destroyIpadSession(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	if(argc != 0){
		JS_ReportWarning(cx, "JSFUNC_destroyIpadSession-method must no arguments");
		return JS_TRUE;
	}
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if (pSM->GetContext()->mpIPadMan)
		pSM->GetContext()->mpIPadMan->DestroySession();	
	return JS_TRUE;
}

JSBool JSFUNC_setIPadImageParam(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	*rval = JSVAL_FALSE;
	if(argc != 4){
		JS_ReportWarning(cx, "JSFUNC_setSrcIpadBoard-method must get four arguments");
		return JS_TRUE;
	}	
	if(!JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportWarning(cx, "JSFUNC_setSrcIpadBoard-method argv[0] is not string");
		return JS_TRUE;
	}
	if(!JSVAL_IS_DOUBLE(argv[1]) && !JSVAL_IS_INT(argv[1]))
	{
		JS_ReportWarning(cx, "JSFUNC_setSrcIpadBoard-method argv[1] is not double or int");
		return JS_TRUE;
	}
	if(!JSVAL_IS_DOUBLE(argv[2]) && !JSVAL_IS_INT(argv[2]))
	{
		JS_ReportWarning(cx, "JSFUNC_setSrcIpadBoard-method argv[2] is not double or int");
		return JS_TRUE;
	}
	if(!JSVAL_IS_DOUBLE(argv[3]) && !JSVAL_IS_INT(argv[3]))
	{
		JS_ReportWarning(cx, "JSFUNC_setSrcIpadBoard-method argv[3] is not double or int");
		return JS_TRUE;
	}
	JSString *jsSrc = JSVAL_TO_STRING(argv[0]);
	mlString sSrc = wr_JS_GetStringChars(jsSrc);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	double prop = 0;
	double scaleX = 0;
	double scaleY = 0;

	ML_JSVAL_TO_DOUBLE(prop, argv[1]);
	ML_JSVAL_TO_DOUBLE(scaleX, argv[2]);
	ML_JSVAL_TO_DOUBLE(scaleY, argv[3]);

	if (pSM->GetContext()->mpIPadMan)
		pSM->GetContext()->mpIPadMan->SetIPadImageParam( sSrc.c_str(), (float)prop, (float)scaleX, (float)scaleY);	
	return JS_TRUE;
}


JSBool JSFUNC_setProxyAuthorization(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;

	if(argc != 4){
		JS_ReportWarning(cx, "JSFUNC_setProxyAuthorization-method must get four arguments: login, pass, proxyType, settings name");
		return JS_TRUE;
	}

	// 1 argument
	if(!JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx,"First parameter of setProxyAuthorization(..) must be a string - login");
		return JS_TRUE;
	}

	// 2 argument
	if(!JSVAL_IS_STRING(argv[1])){
		JS_ReportError(cx,"Second parameter of setProxyAuthorization(..) must be a string - password");
		return JS_TRUE;
	}

	// 3 argument
	if(!JSVAL_IS_INT(argv[2])){
		JS_ReportError(cx,"Third parameter of setProxyAuthorization(..) must be a number - proxyType");
		return JS_TRUE;
	}

	// 4 argument
	if(!JSVAL_IS_STRING(argv[1])){
		JS_ReportError(cx,"Four parameter of setProxyAuthorization(..) must be a string - setting name");
		return JS_TRUE;
	}

	JSString* jssLogin = JSVAL_TO_STRING(argv[0]);
	mlString sLogin = wr_JS_GetStringChars(jssLogin);

	JSString* jssPassword = JSVAL_TO_STRING(argv[1]);
	mlString sPassword = wr_JS_GetStringChars(jssPassword);

	jsint jsProxyType = JSVAL_TO_INT(argv[2]);

	JSString* jssSettingName = JSVAL_TO_STRING(argv[3]);
	mlString sSettingName = wr_JS_GetStringChars(jssSettingName);

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);

	if( pSM->GetContext()->mpComMan)
	{
		pSM->GetContext()->mpComMan->SetProxyLoginPass(sLogin.c_str(), sPassword.c_str(), (int)jsProxyType, sSettingName.c_str());
		*rval = JSVAL_TRUE;
	}

	return JS_TRUE;
}

JSBool JSFUNC_saveRecord(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = INT_TO_JSVAL(-1);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	JS_ReportError(cx, "Log-manager is not set");
	return JS_TRUE;
}

JSBool JSFUNC_startRecordPlaying(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	JS_ReportError(cx, "Log-manager is not set");
	return JS_TRUE;
}

JSBool JSFUNC_stopRecordPlaying(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	JS_ReportError(cx, "Log-manager is not set");
	return JS_TRUE;
}

JSBool JSFUNC_getProxiesMask(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	unsigned char auProxiesMask = 0;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);

	if( pSM->GetContext()->mpComMan)
		auProxiesMask = pSM->GetContext()->mpComMan->GetProxiesMask();

	*rval = INT_TO_JSVAL((int)auProxiesMask);
	return JS_TRUE;
}


JSPropertySpec mlHTTPRequest::_JSPropertySpec[] = {
	{ "status", JSPROP_status, JSPROP_ENUMERATE, 0, 0},
	{ "response", JSPROP_response, JSPROP_ENUMERATE, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlHTTPRequest::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(HTTPRequest,mlHTTPRequest,0)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDPROPENUMOP
MLJSCLASS_IMPL_END(mlHTTPRequest)

/////////////////////////////soap//////////////////

void getJSONService(JSContext *cx, JSObject *obj, jsval *rval, jschar* jscServiceName, jschar* jscServer)
{
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	service::IBaseServiceManager* pServiceMan = NULL;
	JSObject* jsoJSON = NULL;
	jsoJSON = mlJSONRPCJSO::newJSObject(cx);
	mlJSONRPCJSO* pJSON=(mlJSONRPCJSO*)JS_GetPrivate(cx,jsoJSON);

	if (isEqual( (const wchar_t *) jscServer, L"world"))
		pJSON->SetManager(pSM->GetContext()->mpServiceWorldMan);
	else
		pJSON->SetManager(pSM->GetContext()->mpServiceMan);

	wchar_t wchServiceName[100];
	swprintf_s(wchServiceName, 100, (const wchar_t *)jscServiceName);

	pJSON->SetServiceName( wchServiceName);

	pJSON->QueryService();

	*rval = OBJECT_TO_JSVAL(jsoJSON);
}

JSBool JSFUNC_getService(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpComMan == NULL){
		JS_ReportError(cx, "Communication manager is not set");
		return JS_TRUE;
	}

	// Недостаточно аргументов
	if(argc < 2){
		JS_ReportError(cx, "GetService-method must get two arguments");
		return JS_TRUE;
	}

	jsval vArg = argv[0];
	// Это имя сервиса
	if(!JSVAL_IS_STRING(vArg)){
		JS_ReportWarning(cx, "First argument of getService must be a name of service");
		return JS_TRUE;
	}
	wchar_t* sServiceName = wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));			

	if (isEqual(sServiceName , L"soap"))
	{
		JSObject* jsoSoap = NULL;
		jsoSoap = mlSoapJSO::newJSObject(cx);
		mlSoapJSO* pSoap=(mlSoapJSO*)JS_GetPrivate(cx,jsoSoap);
		wchar_t* jscName = NULL;

		JSString * jssParams = JSVAL_TO_STRING(argv[1]);
		wchar_t* jscParams = wr_JS_GetStringChars(jssParams);
		int iErrorCode = 0;
		const wchar_t* wchQwery = L"soap/getSoapService.php";

		wchar_t wchParam[30];
		int j = swprintf_s(wchParam,30,L"servname=");
		swprintf_s(wchParam+j,30-j,jscParams);

		const wchar_t* pwcResult = pSM->GetContext()->mpComMan->Query(wchQwery, wchParam, &iErrorCode);
		if(iErrorCode > 0)
			TraceServerError(cx, iErrorCode);
		if(pwcResult != NULL){
			*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwcResult));
		}
		wchar_t * mstr = (wchar_t *)pwcResult;

		//end get wsdl
		//get uid_client for service
		const wchar_t* pwcResult_uid = pSM->GetContext()->mpComMan->Query(L"soap/getUID.php", wchParam, &iErrorCode);
		if(iErrorCode > 0)
			TraceServerError(cx, iErrorCode);
		if(pwcResult != NULL){
			*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwcResult_uid));
		}

		wchar_t * uid = (wchar_t *)pwcResult_uid;
		size_t requiredSize = 0;
		wcstombs_s(&requiredSize, NULL, 0, uid, 0); // C4996
		unsigned char * pwc;
		pwc = MP_NEW_ARR(unsigned char, requiredSize + 1);
		size_t converted = 0;
		wcstombs_s(&converted, (char *) pwc, requiredSize + 1, uid, requiredSize); 
		TRACE(cx,"pwc2 %s\n",pwc);

		if(!pSoap->Initialize(jscName,cx,obj,mstr,uid)){
			*rval = JSVAL_NULL;
			return JS_TRUE;
		}

		*rval = OBJECT_TO_JSVAL(jsoSoap);
	}

	if (isEqual(sServiceName , L"json"))
	{
		JSString * jssParams = JSVAL_TO_STRING(argv[1]);
		jschar* jscServiceName = JS_GetStringChars(jssParams);
		jschar* jscServer = (jschar *) L"";
		if (argc > 2)
		{
			jssParams = JSVAL_TO_STRING(argv[2]);
			jscServer = JS_GetStringChars(jssParams);
		}
		getJSONService(cx, obj, rval, jscServiceName, jscServer);
	}
	return JS_TRUE;

}

/////////////////////////////soap//////////////////

JSBool mlHTTPRequest::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlHTTPRequest);
		default:
			switch(iID){
			case JSPROP_status:
				// ??
				break;
			case JSPROP_response:
				// ??
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

JSBool mlHTTPRequest::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlHTTPRequest);
		default:
			switch(iID){
			case JSPROP_status:
				// ??
				break;
			case JSPROP_response:
				// ??
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

mlHTTPRequest::~mlHTTPRequest(){
	// удалить из списка
	//mlTrace(mcx, "mlHTTPRequest::~mlHTTPRequest ID = %d, pointer=%x, php=%S\n", miID, this, msQuery.c_str());
	GPSM(mcx)->GetRequestList()->RemoveRequest(this);
}

mlServerCallbackManager::mlServerCallbackManager(mlSceneManager* apSM){
	mpSM = apSM;
	mbCallbacksIsSet = false;
}

mlServiceCallbackManager::mlServiceCallbackManager(mlSceneManager* apSM){
	mpSM = apSM;
	mbCallbacksSMIsSet = false;
	mbCallbacksWSMIsSet = false;
}

void mlServerCallbackManager::SetCallbacks(JSContext *cx){
	if(mbCallbacksIsSet) return;
	omsContext* pOMSContext = mpSM->GetContext();
	if(pOMSContext == NULL) return;
	cm::cmICommunicationManager* pComMan = pOMSContext->mpComMan;
	if(pOMSContext->mpComMan == NULL)
		return;
	mbCallbacksIsSet = pOMSContext->mpComMan->SetCallbacks(this);
}

void mlServiceCallbackManager::onServiceNotify(int aiID, const wchar_t* apwcResponse, int* apiErrorCode)
{
	mlSynchData serviceData;
	serviceData<<aiID;
	serviceData<<apwcResponse;
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_serviceOnNotify, (unsigned char*)serviceData.data(), serviceData.size());
}

void mlServiceCallbackManager::onMethodComplete(int aRequestID, int aRMMLID, const wchar_t* apwcResponse, bool isService, int apiErrorCode)
{
		mlSynchData serviceData;
		serviceData<<aRequestID;
		serviceData<<aRMMLID;
		serviceData<<isService;
		serviceData<<apwcResponse;
		serviceData<<apiErrorCode;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_serviceOnReceivedMethodResult, (unsigned char*)serviceData.data(), serviceData.size());
}

void mlServerCallbackManager::onQueryComplete(int aiID, const wchar_t* apwcResponse, int* apiErrorCode){
	if(aiID < 0) return;
	// Поместить ответ в очередь событий
	omsContext* pOMSContext = mpSM->GetContext();
	if(pOMSContext == NULL) return;
	mlSynchData oData;
	oData << aiID;
	oData << *apiErrorCode;
	if(apwcResponse != NULL)
		oData << apwcResponse;
	else
		oData << L"";
//if(apwcResponse != NULL)
//mlTrace(mpSM->GetJSContext(),"onQueryComplete:%S\n", apwcResponse);
//else 
//mlTrace(mpSM->GetJSContext(),"onQueryComplete:NULL\n", apwcResponse);
	pOMSContext->mpInput->AddCustomEvent(mlSceneManager::CEID_serverQueryComplete, (unsigned char*)oData.data(), oData.size());
}

void mlServerCallbackManager::onQueryCompleteInternal(mlSynchData& aData){
	int iID = 0;
	int iErrorCode = 0;
	wchar_t* pwcResponse = NULL;
	aData >> iID;
	aData >> iErrorCode;
	aData >> pwcResponse;

	mlHTTPRequest* pRequest = mpSM->GetRequestList()->GetRequestByID(iID, iID);
	if(pRequest == NULL)
		return;

	mpSM->GetRequestList()->RemoveRequest(pRequest);
	if( iErrorCode != 0)
	{

	}

	if(pRequest->pResultHandler!=NULL)
	{
		pRequest->pResultHandler->onCompleteFunction(mpSM, pwcResponse, &iErrorCode);
	}
	pRequest->onQueryComplete(0, pwcResponse, &iErrorCode);
}

void mlHTTPRequest::onQueryComplete(int isService, const wchar_t* apwcResponse, int* apiErrorCode){
	
	if(apiErrorCode != NULL){
		jsval vErrorCode = INT_TO_JSVAL(*apiErrorCode);
		JS_SetProperty(mcx, mjso, "errorCode", &vErrorCode);
	}
	jsval vStatus = INT_TO_JSVAL(2);
	if (*apiErrorCode > 0) {
		vStatus = INT_TO_JSVAL(0);
	}

	JS_SetProperty(mcx, mjso, "status", &vStatus);

	if(apwcResponse != NULL){

		if (isService)
		{
			// Инициализация сервиса
			JSObject* jsoJSONService = NULL;
			jsoJSONService = mlJSONRPCJSO::newJSObject(mcx);
			mlJSONRPCJSO* pJSONService=(mlJSONRPCJSO*)JS_GetPrivate(mcx,jsoJSONService);
			mlString sResult(L"[");
			sResult = sResult + apwcResponse + L"]";
			pJSONService->Initialize(mcx,sResult);
			jsval vJSONProps=OBJECT_TO_JSVAL(pJSONService->mjso);
			JS_SetProperty(mcx, mjso, "result", &vJSONProps);

			mlSceneManager* pSM = GPSM(mcx);
			if( pSM->GetContext()->mpServiceMan)
				pSM->GetContext()->mpServiceMan->RegisterService(GetID(), (int)pJSONService->mjso); 
		}
		else
		{
			JSString* jssResponse = wr_JS_NewUCStringCopyZ(mcx, apwcResponse);
			jsval vResponse = STRING_TO_JSVAL(jssResponse);
			JS_SetProperty(mcx, mjso, "response", &vResponse);

			mlString sResult(L"[");
			sResult = sResult + apwcResponse + L"]";

			jsval vJSONProps = NULL;
			JSBool bR = wr_JS_EvaluateUCScript(mcx, JS_GetGlobalObject(mcx), sResult.c_str(), sResult.size(), "result", 1, &vJSONProps);

			if (JSVAL_IS_REAL_OBJECT(vJSONProps))
			{	JSObject *obj=JSVAL_TO_OBJECT(vJSONProps);
				JS_GetElement(mcx,obj,0,&vJSONProps);
				if (JSVAL_IS_REAL_OBJECT(vJSONProps))
				{
					JS_SetProperty(mcx, mjso, "result", &vJSONProps);
				}
			}
		}


	}
//jsval vD = JSVAL_NULL;
//GET_SAVED_FROM_GC(mcx, GPSM(mcx)->GetPlServerGO(), mjso, (&vD));
//if(!JSVAL_IS_REAL_OBJECT(vD)){
//	return; // для отлова ситуации, когда ответ дается какому-то запросу повторно (т.е. скорее всего не тому запросу, какому надо)
//}
	FREE_FOR_GC(mcx, GPSM(mcx)->GetPlServerGO(), mjso);
	// вызвать функцию изменения статуса
	jsval vFunc=JSVAL_NULL;
	#define ON_STATUS_FUNC_NAME L"onStatus"
	if(!wr_JS_GetUCProperty(mcx, mjso, ON_STATUS_FUNC_NAME, -1, &vFunc))
	{
		mlTraceWarning(mcx, "Query object is not found\n");
		return;
	}
	if(!JSVAL_IS_REAL_OBJECT(vFunc)){
		mlTraceWarning(mcx, "Method %S not found\n", ON_STATUS_FUNC_NAME);
		return;
	}
	if(!JS_ObjectIsFunction(mcx,JSVAL_TO_OBJECT(vFunc))){
		mlTraceWarning(mcx, "%S is not a function\n", ON_STATUS_FUNC_NAME);
		return;
	}
	unsigned char ucArgC = 0;
	jsval* vArgs=NULL;
	jsval v;
	JS_CallFunctionValue(mcx, mjso, vFunc, ucArgC, vArgs, &v);
}

JSBool JSFUNC_sendQuery(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_NULL;
	if(argc < 1 || !JSVAL_IS_STRING(argv[0]) || argc > 2){
		JS_ReportError(cx, "sendQuery method must get one or two string arguments");
		return JS_TRUE;
	}
	JSString* jssQuery = JSVAL_TO_STRING(argv[0]);
	JSString* jssParams = NULL;
	if(argc >= 2){
		if(!JSVAL_IS_STRING(argv[1])){
			JS_ReportError(cx, "Second argument of sendQuery method must be a string");
			return JS_TRUE;
		}
		jssParams = JSVAL_TO_STRING(argv[1]);
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpComMan == NULL){
		JS_ReportError(cx, "Communication manager is not set");
		return JS_TRUE;
	}
	GPSM(cx)->SetServerCallbacks();
	wchar_t* jscQuery = wr_JS_GetStringChars(jssQuery);
	wchar_t* jscParams = jssParams == NULL ? NULL : wr_JS_GetStringChars(jssParams);
	//int iErrorCode = 0;
	int iID = pSM->GetContext()->mpComMan->SendQuery(jscQuery, jscParams);
	JSObject* jsoRequest = mlHTTPRequest::newJSObject(cx);
	if(/*iErrorCode > 0 ||*/ iID < 0){
		//TraceServerError(cx, iErrorCode);
		jsval vStatus = INT_TO_JSVAL(0);
		JS_SetProperty(cx, jsoRequest, "status", &vStatus);
		//jsval vErrorCode = INT_TO_JSVAL(iErrorCode);
		jsval vErrorCode = INT_TO_JSVAL(1);
		JS_SetProperty(cx, jsoRequest, "errorCode", &vErrorCode);
		const wchar_t* pwcError = L"Unknown server error";
	/*	switch(iErrorCode){
			// ??
		}*/
		jsval vError = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwcError));
		JS_SetProperty(cx, jsoRequest, "errorCode", &vErrorCode);
	}else{
		// сохраним ссылку на объект, пока ему не придет ответ с сервера или не возникнет ошибка
		SAVE_FROM_GC(cx, pSM->GetPlServerGO(), jsoRequest)
		mlHTTPRequest* pRequest = (mlHTTPRequest*)JS_GetPrivate(cx, jsoRequest);
		pRequest->msQuery = jscQuery; // save debug info
		pRequest->SetID(iID);
		pRequest->SetRequestID(GPSM(cx)->GetRequestList()->GetNextServiceIDRequest()); 
		GPSM(cx)->GetRequestList()->AddRequest(pRequest);
		jsval vStatus = INT_TO_JSVAL(1);
		JS_SetProperty(cx, jsoRequest, "status", &vStatus);
		//mlTrace(cx, "sendQuery ID = %d, pointer=%x, php=%S\n", iID, pRequest, pRequest->msQuery.c_str());
	}
	*rval = OBJECT_TO_JSVAL(jsoRequest);
	return JS_TRUE;
}

// Отослать запрос на сервер на получение статуса объекта
// после получения вызвать у apObjectLink onStatusReceived(auObjectID, auBornRealityID, aiStatus)
int mlSceneManager::SendRequestForObjectStatus(unsigned int auObjectID, unsigned int auBornRealityID, mlSynchLinkJSO* apObjectLink){

	// TODO Obsolete
	assert( false);

	if(GetContext()->mpSyncMan == NULL)
		return -1;
	// ?? если для объекта есть описание в кэше синхронизируемых свойств
	// ?? значит объект просто еще не загрузился (вернуть статус SLS_NOT_LOADED_YET 3)
	// сохраниям ссылку на объект Link, чтобы потом он точно был жив и у него можно было бы вызвать onStatusReceived
	SAVE_FROM_GC(cx, GetPlServerGO(), apObjectLink->mjso);
	// ?? помещаем ссылку в map-у, чтобы потом ее найти по objectID и вызвать у него onStatusReceived
	// ??
	mMapCallbackManager->SetCallbacks(cx);
	mSyncCallbackManager->SetCallbacks(cx);
	mVoipCallbackManager->SetCallbacks(cx);
	// запрашиваем статус и у SyncServer-а и у MapServer-а
	if(OMS_FAILED(GetContext()->mpSyncMan->GetObjectStatus(auObjectID, auBornRealityID)))
		return -1;

	/*if(OMS_FAILED(GetContext()->mpMapMan->GetObjectStatus(auObjectID, auBornRealityID, (void*)apObjectLink->mjso)))
		return -1;*/
	return 1;
}

void mlSyncCallbackManager::onReceivedObjectStatus(unsigned int auObjectID, unsigned int auBornRealityID, int aiStatus){
	mlSynchData oData;
	oData << auObjectID;
	oData << auBornRealityID;
	oData << aiStatus;
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnObjectStatusReceived, (unsigned char*)oData.data(), oData.size());
}

// Получен статус объекта с SyncServer-а
void mlSyncCallbackManager::onReceivedObjectStatusInternal(mlSynchData& aData){
	JSContext* cx = mpSM->GetJSContext();
	unsigned int uiObjectID = 0;
	aData >> uiObjectID;
	unsigned int uiBornRealityID = 0;
	aData >> uiBornRealityID;
	int iStatus = 0;
	aData >> iStatus;
	// mlSynchLinkJSO* pObjectLink = NULL;
	// ?? находим в map-е ссылки на Link-и по objectID
	// ?? и вызываем у них onStatusReceived
	// ??
	// FREE_FOR_GC(cx, mpSM->GetPlServerGO(), pObjectLink->mjso);
}

#define MAKE_COMPLEX_ID(OID, BRID) ((unsigned __int64)(((unsigned __int64)BRID << 32) + OID))


// Получен статус объекта с MapServer-а
void mlMapCallbackManager::onReceivedObjectStatusInernal(mlSynchData& aData){
	unsigned int uiObjectID;
	aData >> uiObjectID;
	unsigned int uiBornRealityID;
	aData >> uiBornRealityID;
	int iStatus;
	aData >> iStatus;
	unsigned int uiUserData;
	aData >> (unsigned int)uiUserData;
	mpSM->onReceivedMapServerObjectStatus(uiObjectID, uiBornRealityID, iStatus, uiUserData);
}

void mlServiceCallbackManager::onGetService(unsigned int rmmlID, const wchar_t* apwcServiceName, const wchar_t* apwcServiceDescription, int* apiErrorCode ){
	// получить rmml-й объект по описанию
	// заполнить ему description
		// так как возможно этот вызов происхордит в другом потоке
		// то просто запоминаем все в вектор, чтобы потом создать объекты-обертки (mlLogRecordJSO)
		mlSynchData serviceData;
		serviceData<<rmmlID;
		serviceData<<apwcServiceDescription;
		if (apiErrorCode==NULL)
			serviceData<<0;
		else
			serviceData<<*apiErrorCode;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_serviceOnGetService, (unsigned char*)serviceData.data(), serviceData.size());
}


void mlServiceCallbackManager::onConnectRestored()
{
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_serviceOnConnectRestored, NULL, NULL);
}

void mlServiceCallbackManager::onConnectRestoredInternal(mlSynchData& aData)
{
	JSObject* mjsoServer = mpSM->GetPlServerGO();
	JSContext *cx = mpSM->GetJSContext();
	// все JS-объекты JSON-сервисов
	JSIdArray* pGPropIDArr = JS_Enumerate(cx, mjsoServer);
	for(int ikl=0; ikl<pGPropIDArr->length; ikl++){
		jsid id=pGPropIDArr->vector[ikl];
		jsval v;
		if(!JS_IdToValue(cx,id,&v)) continue;
		jsval vProp = JSVAL_NULL;
		jschar* jsc = NULL;
		if(JSVAL_IS_STRING(v)){
			jsc = JS_GetStringChars(JSVAL_TO_STRING(v));
			unsigned int attrs;
			JSBool found;
			if (JS_GetUCPropertyAttributes(cx, mjsoServer, jsc, -1, &attrs, &found) &&
				found == JS_TRUE && (attrs & JSPROP_READONLY)
				)
				continue;
			JS_GetUCProperty(cx, mjsoServer, jsc, -1, &vProp);
		}else 
			continue;
		if(!JSVAL_IS_REAL_OBJECT(vProp)) continue;
		JSObject* jso = JSVAL_TO_OBJECT(vProp);
		JSClass* pClass = JS_GET_CLASS(cx,jso);
		if(mlJSONRPCJSO::IsInstance(cx, jso)){
			if(JS_GetStringLength(JSVAL_TO_STRING(v)) != 2)
				continue;
			mlJSONRPCJSO* pJSON=(mlJSONRPCJSO*)JS_GetPrivate(cx,jso);
			if(pJSON->service.name.empty())
				continue;
			pJSON->QueryService();
		}
	}
}

void mlServiceCallbackManager::SetCallbacks(JSContext *cx){
	omsContext* pOMSContext = mpSM->GetContext();
	if(pOMSContext == NULL) return;
	if(!mbCallbacksWSMIsSet) 
	{
		service::IServiceWorldManager* pServiceManWorld = pOMSContext->mpServiceWorldMan;
		if (pServiceManWorld)
			mbCallbacksWSMIsSet = pServiceManWorld->SetCallbacks(this);

	}

	if(!mbCallbacksSMIsSet) 
	{
		service::IServiceManager* pServiceMan = pOMSContext->mpServiceMan;
		if (pServiceMan)
			mbCallbacksSMIsSet = pServiceMan->SetCallbacks(this);
	}


}

void mlSceneManager::onReceivedMapServerObjectStatus(unsigned int auObjectID, unsigned int auBornRealityID, int aiStatus, unsigned int uiUserData){
	assert(false);
	// ?? реально зону, в которой находится динамический объект знает SyncServer, а не MapServer
	// ?? поэтому status, полученный здесь от MapServer-а может быть не валидным
	// ?? надо будет дождаться, когда придет ответ с SyncServer-а и уже потом уведомлять Link-а
	jsval v = JSVAL_NULL;
	GET_SAVED_FROM_GC(cx, GetPlServerGO(), (uiUserData), (&v));
	if(!JSVAL_IS_REAL_OBJECT(v)){
		return;
	}
	JSObject* jsoLink = JSVAL_TO_OBJECT(v);
	mlSynchLinkJSO* pObjectLink = (mlSynchLinkJSO*)JS_GetPrivate(cx, jsoLink);

	pObjectLink->onStatusReceived(auObjectID, auBornRealityID, aiStatus);

	FREE_FOR_GC(cx, GetPlServerGO(), pObjectLink->mjso);

	// находим в map-е ссылки на Link-и по objectID
	//unsigned __int64 complexID = MAKE_COMPLEX_ID(auObjectID, auBornRealityID);
	//mlMapLinks::iterator mi = mmWaitingStatusLinks.find( complexID);
	//if(mi != mmWaitingStatusLinks.end()){
	//	mlJSOList& vLinks = mi->second;
	//	mlJSOList::iterator vi = vLinks.begin();
	//	for(; vi != vLinks.end(); vi++){
	//		// ?? пока не будем дожидаться ответа от SyncServer-а
	//		JSObject* jsoLink = *vi;
	//		mlSynchLinkJSO* pObjectLink = (mlSynchLinkJSO*)JS_GetPrivate(cx, jsoLink);
	//		pObjectLink
	//		// ?? и вызываем у них onStatusReceived
	//		// ??
	//		FREE_FOR_GC(cx, GetPlServerGO(), pObjectLink->mjso);
	//	}
	//	mmWaitingStatusLinks.erase(mi);
	//}
}

}

void mlServiceCallbackManager::onNotify(mlSynchData& aData){
	int iID;
	wchar_t* pwcResponse = NULL;
	int iErrorCode = 0;

	aData >> iID;
	aData >> pwcResponse;

	mlString sResult(L"[");
	sResult = sResult + pwcResponse + L"]";
	jsval v = JSVAL_NULL;
	JSObject* jso = (JSObject*)iID;
	GET_SAVED_FROM_GC((mpSM->GetJSContext()), mpSM->GetPlServerGO(), jso, &v);
	if(JSVAL_IS_REAL_OBJECT(v)){
		JSObject* jsoJSONService = JSVAL_TO_OBJECT(v);
		mlJSONRPCJSO* pJSONService = (mlJSONRPCJSO*)JS_GetPrivate((mpSM->GetJSContext()), jsoJSONService);
			if(pJSONService != NULL)
				pJSONService->Notify(mpSM->GetJSContext(),sResult);
	}
}

void mlServiceCallbackManager::onReceivedMethodResultInternal(mlSynchData& aData){
	int iRequestID;
	int iRmmlID;
	bool isService;
	wchar_t* pwcResponse = NULL;
	int iErrorCode = -1;

	aData >> iRequestID;
	aData >> iRmmlID;
	aData >> isService;
	aData >> pwcResponse;
	aData >> iErrorCode;

	mlHTTPRequest* pRequest = mpSM->GetRequestList()->GetRequestByID(iRequestID, iRequestID);
	if(pRequest == NULL)
		return;
	if(pRequest->pResultHandler!=NULL)
	{
		pRequest->pResultHandler->onCompleteFunction(mpSM, pwcResponse, &iErrorCode);
	}
	pRequest->onQueryComplete(isService, pwcResponse, &iErrorCode);
}


void mlServiceCallbackManager::onGetServiceInternal(mlSynchData& aData){
	unsigned int uID;
	aData >> uID;
	wchar_t* sParam = L"";
	aData >> sParam;
	mlString sDescription(L"[");
	sDescription = sDescription + sParam + L"]";
	int iError;
	aData >> iError;

	jsval v = JSVAL_NULL;
	JSObject* jso = (JSObject*)uID;
	GET_SAVED_FROM_GC((mpSM->GetJSContext()), mpSM->GetPlServerGO(), jso, &v);
	if(JSVAL_IS_REAL_OBJECT(v)){
		JSObject* jsoJSONService = JSVAL_TO_OBJECT(v);
		mlJSONRPCJSO* pJSONService = (mlJSONRPCJSO*)JS_GetPrivate((mpSM->GetJSContext()), jsoJSONService);
		if(pJSONService != NULL){
			if (iError!=0)
				pJSONService->SetErrorCode(iError);
			pJSONService->Initialize(mpSM->GetJSContext(),sDescription);
		}
	}
}

void mlSyncCallbackManager::onObjectCreationResult(unsigned int auiErrorCode, unsigned int auiObjectID, unsigned int auiBornRealityID, unsigned int auiRmmlHandlerID)
{
	mlSynchData data;
	data << auiErrorCode;
	data << auiObjectID;
	data << auiBornRealityID;
	data << auiRmmlHandlerID;

	mpSM->OnObjectCreationResult( auiErrorCode, auiObjectID, auiBornRealityID, auiRmmlHandlerID);
}

void mlSyncCallbackManager::onServerErrorReceived(unsigned int errorCode)
{
	mlSynchData data;
	data << errorCode;

	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnServerError, (unsigned char*)data.data(), data.size() );
}


void mlSyncCallbackManager::onErrorCreateTempLocation( unsigned int aErrorCode)
{
	mlSynchData oData;
	oData << aErrorCode;		
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onErrorCreateTempLocation, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::OnErrorCreateTempLocationInternal(mlSynchData& aData)
{	
	unsigned int errorCode = 0;	

	aData >> errorCode;	
	jsval vErrorCode = INT_TO_JSVAL( errorCode);
	
	JS_SetProperty( cx, GPSM(cx)->GetEventGO(),"locationErrorCode",&vErrorCode);
	NotifyServerListeners( EVID_onErrorCreateTempLocation);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "locationErrorCode");
}

void mlSyncCallbackManager::onInfoMessage(unsigned int auMsgCode, const wchar_t *apMessage){
	mlSynchData oData;
	oData << auMsgCode;
	oData << apMessage;
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnInfoMessage, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::OnInfoMessageInternal(mlSynchData& aData)
{
	unsigned int iMsgCode = 0;
	wchar_t *sMessage = NULL;
	aData >> iMsgCode;	
	jsval jsv = INT_TO_JSVAL(iMsgCode);
	aData >> sMessage;

	JS_SetProperty(cx, GetEventGO(), "msgCode", &jsv);

	mlString sResult(L"[");
	sResult = sResult + sMessage + L"]";
	jsval vJSONProps = NULL;
	JSBool bR = JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), (const jschar*)sResult.c_str(), sResult.size(), "messageObj", 1, &vJSONProps);
	if (JSVAL_IS_REAL_OBJECT(vJSONProps))
	{	
		JSObject *obj=JSVAL_TO_OBJECT(vJSONProps);
		JS_GetElement(cx,obj,0,&vJSONProps);
		if (JSVAL_IS_REAL_OBJECT(vJSONProps))
		{
			JS_SetProperty(cx, GetEventGO(), "messageObj", &vJSONProps);
		}
	}
	NotifyServerListeners(EVID_onSyncNotify);
	JS_DeleteProperty(cx, GetEventGO(), "messageObj");
	JS_DeleteProperty(cx, GetEventGO(), "msgCode");
}

void mlSceneManager::ResetPlayerServer(){
	// освободить все JS-объекты JSON-сервисов
	JSIdArray* pGPropIDArr = JS_Enumerate(cx, mjsoServer);
	for(int ikl=0; ikl<pGPropIDArr->length; ikl++){
		jsid id=pGPropIDArr->vector[ikl];
		jsval v;
		if(!JS_IdToValue(cx,id,&v)) continue;
		jsval vProp = JSVAL_NULL;
		jschar* jsc = NULL;
		if(JSVAL_IS_STRING(v)){
			jsc = JS_GetStringChars(JSVAL_TO_STRING(v));
			unsigned int attrs;
			JSBool found;
			if (JS_GetUCPropertyAttributes(cx, mjsoServer, jsc, -1, &attrs, &found) &&
				found == JS_TRUE && (attrs & JSPROP_READONLY)
				)
				continue;
			JS_GetUCProperty(cx, mjsoServer, jsc, -1, &vProp);
		}else 
			continue;
		if(!JSVAL_IS_REAL_OBJECT(vProp)) continue;
		JSObject* jso = JSVAL_TO_OBJECT(vProp);
JSClass* pClass = JS_GET_CLASS(cx,jso);
		if(mlJSONRPCJSO::IsInstance(cx, jso)){
			if(JS_GetStringLength(JSVAL_TO_STRING(v)) != 2)
				continue;
			FREE_FOR_GC(cx, mjsoServer, jso);
		}	}}
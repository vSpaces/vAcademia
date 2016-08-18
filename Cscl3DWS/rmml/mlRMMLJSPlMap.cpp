#include "mlRMML.h"
#include "config/oms_config.h"
#include <sstream>
#include "config/prolog.h"
#include "ILogWriter.h"

namespace rmml {

/**
 * Реализация подобъекта "Map" глобального JS-объекта "Player"
 */

JSBool JSMapGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool JSMapSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	
JSClass JSRMMLMapClass = {
	"_Map", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, 
	JSMapGetProperty, JSMapSetProperty,
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL,
	NULL, NULL, NULL
};

enum {
	JSPROP_creating3DOAnywareAllowed=1,
	TIDE_mlRMMLPlMap
};

//enum { EVID_mlRMMLPlMap=TIDE_mlRMMLPlMap-1,

enum{
	EVID_onMyAvatarCreated=0,
	EVID_onObjectCreated,
	EVID_onBeforeTeleport,
	EVID_onTeleported,
	EVID_onTeleportError,
	EVID_onShowZone,
	EVID_onBeforeCreatingOfObjects,	// Начало загрузки объектов
	EVID_onAfterCreatingOfObjects,	// Загружены и созданы объекты воспроизводимой записи
	EVID_onURLChanged,
	EVID_onUserSleep,
	EVID_onUserWakeUp,
};

enum {
};

JSPropertySpec _JSMapPropertySpec[] = {
	JSPROP_RO(creating3DOAnywareAllowed)
	{ 0, 0, 0, 0, 0 }
};

JSBool JSMapSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){
	if(JSVAL_IS_INT(id)){
		mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
		int iID=JSVAL_TO_INT(id);
/*		switch(iID){

		case JSPROP_src:
			// ??
			break;

		}*/
		// уведомляем слушателей
		char chEvID=iID-1;
//		pSM->NotifyMapListeners(chEvID);
	}
	return JS_TRUE;
}

JSBool JSMapGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){
	if(JSVAL_IS_INT(id)){
		mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
		int iID=JSVAL_TO_INT(id);
		switch(iID){
		case JSPROP_creating3DOAnywareAllowed:
//			*vp = JSVAL_TRUE;	// FOR DEBUG PURPOSE ONLY!!!
			break;
		}
	}
	return JS_TRUE;
}

JSFUNC_DECL(teleport)
JSFUNC_DECL(createObject)
JSFUNC_DECL(duplicateObject)
JSFUNC_DECL(deleteObject)
JSFUNC_DECL(findFreePlace)
JSFUNC_DECL(setWorldScene)
JSFUNC_DECL(isPointInActiveZones)
JSFUNC_DECL(isLocationCanBeCreated)
JSFUNC_DECL(getVisibleLocations)
JSFUNC_DECL(getVisibleCommunicationAreas)
JSFUNC_DECL(getLocationByPosition)
JSFUNC_DECL(getLocationParams)
JSFUNC_DECL(getNewPositionObjectOnBoundLocation)
JSFUNC_DECL(setLocationEnteringPermissionForUser)

///// JavaScript Function Table
JSFunctionSpec _JSMapFunctionSpec[] = {
	JSFUNC(teleport,1) // телепортировать аватара в другое место
	JSFUNC(createObject, 1) // создать в мире объект по его описанию
	JSFUNC(duplicateObject, 1) // создать в мире дубликат объекта
	JSFUNC(deleteObject, 1) // удалить из мира объект
	JSFUNC(findFreePlace, 1) // найти свободное место вблизи указанного
	JSFUNC(setWorldScene, 1) // назначить 3D сцену для объектов мира
	JSFUNC(isPointInActiveZones, 1) // проверить, лежит ли точка в видимых зонах
	JSFUNC(isLocationCanBeCreated, 1) // проверить может быть временная локация создана. Передаются 2 краевые точки
	JSFUNC(getVisibleLocations, 1) // получить список видимых локаций
	JSFUNC(getVisibleCommunicationAreas, 1) // получить список видимых локаций общений
	JSFUNC(getLocationByPosition, 1) // получить описание локации в указанной точке 3D-мира
	JSFUNC(getLocationParams, 1) // получить параметры локации в виде строки (это значение из objects_map.params)
	JSFUNC(getNewPositionObjectOnBoundLocation, 3) // получить новые координаты перемещаемого объекта в на границе локации локации
	JSFUNC(setLocationEnteringPermissionForUser, 3) // разрешить/запретить пользователю посещать определенную локацию в определенной реальности
	{ 0, 0, 0, 0, 0 }
};

void CreateMapJSObject(JSContext *cx, JSObject *ajsoPlayer){

    JSObject* jsoMap = JS_DefineObject(cx, ajsoPlayer, GJSONM_MAP, &JSRMMLMapClass, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
	JSBool bR;
	bR=JS_DefineProperties(cx, jsoMap, _JSMapPropertySpec);
	bR=JS_DefineFunctions(cx, jsoMap, _JSMapFunctionSpec);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, ajsoPlayer);
	JS_SetPrivate(cx,jsoMap,pSM);
	pSM->mjsoMap=jsoMap;
}

char GetMapEventID(const wchar_t* apwcEventName){
	if(isEqual(apwcEventName,L"onMyAvatarCreated")){
		return (char)EVID_onMyAvatarCreated;
	}else if(isEqual(apwcEventName,L"onObjectCreated")){
		return (char)EVID_onObjectCreated;
	}else if(isEqual(apwcEventName,L"onBeforeTeleport")){
		return (char)EVID_onBeforeTeleport;
	}else if(isEqual(apwcEventName,L"onTeleported")){
		return (char)EVID_onTeleported;
	}else if(isEqual(apwcEventName,L"onTeleportError")){
		return (char)EVID_onTeleportError;
	}else if(isEqual(apwcEventName,L"onShowZone")){
		return (char)EVID_onShowZone;
	}else if(isEqual(apwcEventName,L"onBeforeCreatingOfObjects")){
		return (char)EVID_onBeforeCreatingOfObjects;
	}else if(isEqual(apwcEventName,L"onAfterCreatingOfObjects")){
		return (char)EVID_onAfterCreatingOfObjects;
	}else if(isEqual(apwcEventName,L"onURLChanged")){
		return (char)EVID_onURLChanged;	
	}else if(isEqual(apwcEventName,L"onUserSleep")){
		return (char)EVID_onUserSleep;	
	}else if(isEqual(apwcEventName,L"onUserWakeUp")){
		return (char)EVID_onUserWakeUp;	
	}

	return -1;
}

wchar_t* GetMapEventName(char aidEvent){
	switch(aidEvent){
	case (char)EVID_onMyAvatarCreated:
		return L"onMyAvatarCreated";
	case (char)EVID_onObjectCreated:
		return L"onObjectCreated";
	case (char)EVID_onBeforeTeleport:
		return L"onBeforeTeleport";
	case (char)EVID_onTeleported:
		return L"onTeleported";
	case (char)EVID_onTeleportError:
		return L"onTeleportError";
	case (char)EVID_onShowZone:
		return L"onShowZone";
	case (char)EVID_onAfterCreatingOfObjects:
		return L"onAfterCreatingOfObjects";
	case (char)EVID_onBeforeCreatingOfObjects:
		return L"onBeforeCreatingOfObjects";
	case (char)EVID_onURLChanged:
		return L"onURLChanged";
	}
	return L"???";
}

__forceinline int GetMapEventIdx(char aidEvent){
	switch(aidEvent){
	case EVID_onMyAvatarCreated:
	case EVID_onObjectCreated:
	case EVID_onBeforeTeleport:
	case EVID_onTeleported:
	case EVID_onTeleportError:
	case EVID_onURLChanged:
	case EVID_onShowZone :
	case EVID_onAfterCreatingOfObjects:
	case EVID_onBeforeCreatingOfObjects:
		return aidEvent;
	}
	return 0;
}

bool mlSceneManager::addMapListener(char aidEvent, mlRMMLElement* apMLEl){
	if(apMLEl == NULL) return false;
	char chEvID=aidEvent;
	if(chEvID >= ML_MAP_EVENTS_COUNT) return false;
	v_elems* pv=&(maMapListeners[chEvID]);
	v_elems::const_iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl) return true;
	}
	pv->push_back(apMLEl);
	return true;
}

void mlSceneManager::NotifyMapListeners(char chEvID){
	if(chEvID < ML_MAP_EVENTS_COUNT){
		v_elems* pv=&(maMapListeners[chEvID]);
		v_elems::const_iterator vi;
		for(vi=pv->begin(); vi != pv->end(); vi++ ){
			mlRMMLElement* pMLEl=*vi;
			pMLEl->EventNotify(chEvID,(mlRMMLElement*)GOID_Map);
		}
	}
}

bool mlSceneManager::removeMapListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	if(aidEvent==-1){
		// и удалим его из списка слушателей всех событий
		for(int ii=0; ii<ML_MAP_EVENTS_COUNT; ii++){
			v_elems* pv=&maMapListeners[ii];
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
	v_elems* pv=&(maMapListeners[aidEvent-1]);
	v_elems::iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl){
			pv->erase(vi);
			return true;
		}
	}
	return true;
}

void mlSceneManager::ResetMap(){
	for(int ii=0; ii<ML_MAP_EVENTS_COUNT; ii++){
		maMapListeners[ii].clear();
	}
}

void mlSceneManager::NotifyMapOnMyAvatarCreatedListeners(){

	jsval vAvatar = JSVAL_NULL;
	vAvatar = OBJECT_TO_JSVAL(mpMyAvatar->mjso);
	JS_SetProperty(cx, GetEventGO(), "avatar", &vAvatar);

	NotifyMapListeners(EVID_onMyAvatarCreated);

	JS_DeleteProperty( cx, GetEventGO(), "avatar");
	
}

// запускает процесс телепортации аватара в другое место
// bool Player.map.teleport(sURL | obj)
JSBool JSFUNC_teleport(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	omsContext* pContext = pSM->GetContext();
	if(argc == 0){
		JS_ReportError(cx, "teleport-method must get one argument at least");
		return JS_TRUE;
	}
	mlString sURL;
	jsval vArg = argv[0];
	if(JSVAL_IS_STRING(vArg)){
		sURL = (const wchar_t*)JS_GetStringChars(JSVAL_TO_STRING(vArg));
	}else if(JSVAL_IS_REAL_OBJECT(vArg)){
		JSObject* jsoArg = JSVAL_TO_OBJECT(vArg);
		if(sURL == L""){
			JS_ReportError(cx, "teleport-method got an empty location");
			return JS_TRUE;
		}
	}else{
		JS_ReportError(cx, "teleport-method must get a string or an object");
		return JS_TRUE;
	}
/*
mlString sTeleportLogMessage = L"Player.map.teleport(";
sTeleportLogMessage += sURL;
sTeleportLogMessage += L") called from ";
sTeleportLogMessage += pSM->GetJSCallStack();pContext->mpLogWriter->WriteLn((const char*)cLPCSTR(sTeleportLogMessage.c_str()));
*/
	if(pContext->mpSyncMan == NULL){
		JS_ReportError(cx,"SyncManager is not set");
		return JS_TRUE;
	}else{
		JSString* jssURL = JS_NewUCStringCopyZ(cx, (const jschar*)sURL.c_str());
		jsval vURL = STRING_TO_JSVAL(jssURL);
		JS_SetProperty(cx, GPSM(cx)->GetEventGO(), "URL", &vURL);
		JS_SetProperty(cx, GPSM(cx)->GetEventGO(), "url", &vURL);
		pSM->NotifyMapListeners(EVID_onBeforeTeleport);
		JS_DeleteProperty(cx, GPSM(cx)->GetEventGO(), "URL");
		JS_DeleteProperty(cx, GPSM(cx)->GetEventGO(), "url");
		//
		pSM->SuspendSynchStatesSending();
		pSM->SetMapCallbacks();
		bool bRet = pContext->mpSyncMan->PreciseURL(sURL.c_str());

		*rval = BOOLEAN_TO_JSVAL(bRet);
	}
	return JS_TRUE;
}

// создать в мире объект по его описанию
// в качестве параметра дается объект со свойствами:
//  string type - тип объекта ('object', 'group')
//  string name - имя объекта (необязательный параметр)
//  string _class - имя класса
//  string src - путь к ресурсному файлу
//  reference parent - ссылка на объект-родитель (если отсутствует или null, то будет создан в сцене)
//  string userProps - дополнительные свойства объекта в формате JSON (будет создано свойство userProps с массивом со ссылкой на объект)
//  Position3D position - позиция (абсолютная) (обязательный параметр)
//	Rotation3D rotation - поворот (по умолчанию - (0, 0, 0))
//  Scale3D scale - масштаб (по умолчанию - (1, 1, 1))
//  function onCreated - ссылка на фнкцию, которая будет вызвана, когда объект будет реально создан на сервере и на клиенте
//  function onError - ссылка на функцию, которая будет вызвана в случае, если во время создания объекта произойдет ошибка

JSBool JSFUNC_createObject(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_NULL;
	if(argc == 0){
		JS_ReportWarning(cx, "createObject-method must get an argument");
		return JS_TRUE;
	}
	mlObjectInfo4OnServerCreation oOI4OSC;
	jsval vArg = argv[0];
	if(!JSVAL_IS_REAL_OBJECT(vArg)){
		JS_ReportWarning(cx, "createObject-method must get a object");
		return JS_TRUE;
	}
	JSObject* jsoObj = JSVAL_TO_OBJECT(vArg);
	oOI4OSC.musType = MLMT_OBJECT;
	// определяем тип (unsigned short musType;	// RMML-тип объекта (обычно MLMT_OBJECT или MLMT_GROUP))
	jsval v = JSVAL_NULL;
	if(JS_GetProperty(cx, jsoObj, "type", &v) && (v != JSVAL_VOID)){
		if(!JSVAL_IS_STRING(v)){
			JS_ReportWarning(cx, "createObject argument object must get a string 'type' property");
			return JS_TRUE;
		}
		JSString* jssType = JSVAL_TO_STRING(v);
		mlString sType = (const wchar_t*)JS_GetStringChars(jssType);
		wchar_t* pwcType = (wchar_t*)sType.c_str();
		_wcslwr_s(pwcType, sType.length()+1);
		if(isEqual(sType, L"object")){
			oOI4OSC.musType = MLMT_OBJECT;
		}else if(isEqual(sType, L"group")){
			oOI4OSC.musType = MLMT_GROUP;
		}else if(isEqual(sType, L"communication_area")){
			oOI4OSC.musType = MLMT_COMMINICATION_AREA;
		}else if(isEqual(sType, L"character")){
			oOI4OSC.musType = MLMT_CHARACTER;
		}else if(isEqual(sType, L"particles")){
			oOI4OSC.musType = MLMT_PARTICLES;
		}else{
			JS_ReportWarning(cx, "Unknown 'type' property value '%s'", cLPCSTR(sType.c_str()));
			return JS_TRUE;
		}
	}
	// извлекаем имя(const char* mszName;	// имя объекта)
	std::string sName;
	if(JS_GetProperty(cx, jsoObj, "name", &v) && (v != JSVAL_VOID)){
		if(!JSVAL_IS_STRING(v)){
			JS_ReportWarning(cx, "createObject argument object must get a string 'name' property");
			return JS_TRUE;
		}
		JSString* jss = JSVAL_TO_STRING(v);
		mlString str = (const wchar_t*)JS_GetStringChars(jss);
		sName = cLPCSTR(str.c_str());
	}
	oOI4OSC.mszName = sName.c_str();
	// определим класс (const char* mszClassName;	// имя класса объекта)
	std::string sClassName;
	if(JS_GetProperty(cx, jsoObj, "_class", &v) && (v != JSVAL_VOID)){
		if(!JSVAL_IS_STRING(v)){
			JS_ReportWarning(cx, "createObject argument object must get a string 'class' property");
			return JS_TRUE;
		}
		JSString* jss = JSVAL_TO_STRING(v);
		mlString str = (const wchar_t*)JS_GetStringChars(jss);
		sClassName = cLPCSTR(str.c_str());
	}
	oOI4OSC.mszClassName = sClassName.c_str();
	//const wchar_t* mwcSrc;	// Путь к ресурсам
	mlString sSrc;
	if(JS_GetProperty(cx, jsoObj, "src", &v) && (v != JSVAL_VOID)){
		if(!JSVAL_IS_STRING(v)){
			JS_ReportWarning(cx, "createObject argument object must get a string 'src' property");
			return JS_TRUE;
		}
		JSString* jss = JSVAL_TO_STRING(v);
		sSrc = (const wchar_t*)JS_GetStringChars(jss);
	}
	oOI4OSC.mwcSrc = sSrc.c_str();
	//const mlMedia* mpParent;	// родительская 3D-сцена или 3D-группа
	oOI4OSC.mpParent = NULL;
	if(JS_GetProperty(cx, jsoObj, "parent", &v) && (v != JSVAL_VOID)){
		if(!JSVAL_IS_MLEL(cx, v)){
			JS_ReportWarning(cx, "'parent' property must be a reference to real RMML element object");
			return JS_TRUE;
		}
		mlRMMLElement* pMLEl = JSVAL_TO_MLEL(cx, v);
		if(!(pMLEl->mRMMLType == MLMT_SCENE3D || pMLEl->mRMMLType == MLMT_OBJECT || pMLEl->mRMMLType == MLMT_GROUP)){
			JS_ReportWarning(cx, "'parent' property must be a reference to real 3D-scene, 3D-object or 3D-group");
			return JS_TRUE;
		}
		oOI4OSC.mpParent = pMLEl;
	}
	//const wchar_t* mwcParams;	// скрипт инициализации (userProps)
	mlString sUserProps;
	if(JS_GetProperty(cx, jsoObj, "userProps", &v) && (v != JSVAL_VOID)){
		if(!JSVAL_IS_STRING(v)){
			JS_ReportWarning(cx, "createObject argument object must get a string 'userProps' property");
			return JS_TRUE;
		}
		JSString* jss = JSVAL_TO_STRING(v);
		sUserProps = (const wchar_t*)JS_GetStringChars(jss);
	}
	oOI4OSC.mwcParams = sUserProps.c_str();
	// position
	if((JS_GetProperty(cx, jsoObj, "position", &v) && (v != JSVAL_VOID)) || (JS_GetProperty(cx, jsoObj, "pos", &v) && (v != JSVAL_VOID))){
		if(!JSVAL_IS_REAL_OBJECT(v) || !mlPosition3D::IsInstance(cx, JSVAL_TO_OBJECT(v))){
			JS_ReportWarning(cx, "createObject argument object 'position' property must be a Position3D-object");
			return JS_TRUE;
		}
		mlPosition3D* pPos3D = (mlPosition3D*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(v));
		oOI4OSC.mPos = pPos3D->GetPos();
	}else{
		JS_ReportWarning(cx, "createObject argument object 'position' property must be set");
		return JS_TRUE;
	}
	// rotation
	if(JS_GetProperty(cx, jsoObj, "rotation", &v) && (v != JSVAL_VOID)){
		if(!JSVAL_IS_REAL_OBJECT(v) || !mlRotation3D::IsInstance(cx, JSVAL_TO_OBJECT(v))){
			JS_ReportWarning(cx, "createObject argument object 'rotation' property must be a Rotation3D-object");
			return JS_TRUE;
		}
		mlRotation3D* pRot3D = (mlRotation3D*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(v));
		oOI4OSC.mRot = pRot3D->GetRot();
	}
	// scale
	if(JS_GetProperty(cx, jsoObj, "scale", &v) && (v != JSVAL_VOID)){
		if(!JSVAL_IS_REAL_OBJECT(v) || !mlScale3D::IsInstance(cx, JSVAL_TO_OBJECT(v))){
			JS_ReportWarning(cx, "createObject argument object 'scale' property must be a Scale3D-object");
			return JS_TRUE;
		}
		mlScale3D* pScl3D = (mlScale3D*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(v));
		oOI4OSC.mScl = pScl3D->GetScl();
	}
	// bounds
	if(JS_GetProperty(cx, jsoObj, "bounds", &v) && (v != JSVAL_VOID)){
		if(!JSVAL_IS_REAL_OBJECT(v)){
			JS_ReportWarning(cx, "createObject argument object 'bounds' property must be an object");
			return JS_TRUE;
		}
		ml3DScale oBounds; ML_INIT_3DSCALE(oBounds);
		JSObject* jsoBounds = JSVAL_TO_OBJECT(v);
		jsval vp = JSVAL_NULL;
		if(JS_GetProperty(cx, jsoBounds, "x", &vp) && JSVAL_IS_NUMBER(vp)){
			jsdouble dVal = 0;
			if(JS_ValueToNumber(cx, vp, &dVal))
				oBounds.x = dVal;
		}
		if(JS_GetProperty(cx, jsoBounds, "y", &vp) && JSVAL_IS_NUMBER(vp)){
			jsdouble dVal = 0;
			if(JS_ValueToNumber(cx, vp, &dVal))
				oBounds.y = dVal;
		}
		if(JS_GetProperty(cx, jsoBounds, "z", &vp) && JSVAL_IS_NUMBER(vp)){
			jsdouble dVal = 0;
			if(JS_ValueToNumber(cx, vp, &dVal))
				oBounds.z = dVal;
		}
		if(oBounds.x > 0 && oBounds.y > 0 && oBounds.z > 0){
			oOI4OSC.mBounds = oBounds;
		}
	}
	// level
	if(JS_GetProperty(cx, jsoObj, "level", &v) && (v != JSVAL_VOID)){
		jsdouble dVal = 0;
		if(!JS_ValueToNumber(cx, v, &dVal)){
			JS_ReportWarning(cx, "createObject argument object 'level' property must be a number");
			return JS_TRUE;
		}
		oOI4OSC.miLevel = (int)dVal;
	}
	// location
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	omsContext* pContext = pSM->GetContext();
	std::string sLocationID;
	if(pContext->mpMapMan != NULL){
		jsval vCreating3DOAnywareAllowed = JSVAL_VOID;
		JS_GetProperty(cx, obj, "creating3DOAnywareAllowed", &vCreating3DOAnywareAllowed);
		if(vCreating3DOAnywareAllowed == JSVAL_TRUE){
			sLocationID = "creating3DOAnywareAllowed";
			oOI4OSC.mszLocation = sLocationID.c_str();
		}else{
			ml3DLocation oLocation;
			if(pContext->mpMapMan->GetLocationByXYZ(oOI4OSC.mPos.x, oOI4OSC.mPos.y, oOI4OSC.mPos.z, oLocation)){
				if(oLocation.ID.mstr.size() > 0){
					sLocationID = cLPCSTR(oLocation.ID.mstr.c_str());
					oOI4OSC.mszLocation = sLocationID.c_str();
				}
			}
		}
	}

	// защищаем объект от удаления GC, чтобы потом вызвать у него onCreated или onError
//	SAVE_FROM_GC(cx, obj, jsoObj);
//	oOI4OSC.mCallback.mpParams = (void*)jsoObj; // MapManager вернет его при уведомлении о том, что объект (не)создан
	JSObject* jsoHandler = JS_NewObject(cx,NULL,NULL,NULL);
	SAVE_FROM_GC(cx, obj, jsoHandler);
	oOI4OSC.mCallback.mpParams = (void*)jsoHandler; // MapManager вернет его при уведомлении о том, что объект (не)создан
	if(pContext->mpMapMan == NULL){
		JS_ReportError(cx,"MapManager is not set");
		return JS_TRUE;
	}else{
		pSM->SetMapCallbacks();

		bool bRet = pContext->mpMapMan->CreateObject(&oOI4OSC, (unsigned int)jsoHandler);
		if(bRet){
			*rval = OBJECT_TO_JSVAL(jsoHandler);
			//*rval = BOOLEAN_TO_JSVAL(bRet);
		}
	}
	return JS_TRUE;
}

mlString JSValToJSON(JSContext* cx, jsval av){
	mlString wsVal;
	if(av == JSVAL_VOID) return wsVal;
	if(JSVAL_IS_INT(av)){
		int iVal=JSVAL_TO_INT(av);
		bool bMinus=false;
		if(iVal < 0){
			bMinus=true;
			iVal=-iVal;
		}
		if(iVal==0){
			wsVal=L"0";
		}else{
			wsVal = Int2MlString(iVal);
			if(bMinus) wsVal.insert(0, L"-");
		}
		return wsVal;
	}else if(JSVAL_IS_BOOLEAN(av)){
		bool bVal=JSVAL_TO_BOOLEAN(av);
		if(bVal) wsVal=L"true";
		else wsVal=L"false";
		return wsVal;
	}else if(JSVAL_IS_NULL(av)){
		return L"null";
	}else if(JSVAL_IS_STRING(av)){
		JSString* jssVal = JSVAL_TO_STRING(av);
		wsVal = L"\"";
		wsVal += (const wchar_t*)JS_GetStringChars(jssVal);
		wsVal += L"\"";
		return wsVal;
	}else if(JSVAL_IS_DOUBLE(av)){
		jsdouble* dVal=JSVAL_TO_DOUBLE(av);
		wostringstream oss;
		oss << *dVal;
		return oss.rdbuf()->str();
	}else if(JSVAL_IS_OBJECT(av)){
		if(JS_TypeOfValue(cx, av) == JSTYPE_FUNCTION){
			JSObject* jsoFun = JSVAL_TO_OBJECT(av);
			JSClass* pClass=ML_JS_GETCLASS(cx, jsoFun);
			if(pClass->flags == 1){
				return L"function (){ /*native*/ }";
			}else{
				JSFunction* jsf = JS_ValueToFunction(cx, av);
				if(jsf){
					JSString* jssFun = JS_DecompileFunction(cx, jsf, 0);
					if(jssFun == NULL)
						return L"";
					jschar* jsc = JS_GetStringChars(jssFun);
					return (const wchar_t*)jsc;
				}
				return L"function (){ /*not decompiled*/ }";
			}
		}else{
			wsVal = L"{";
			JSObject* jsoVal = JSVAL_TO_OBJECT(av);
			JSIdArray* idArr = JS_Enumerate(cx, jsoVal);
			for(int ij = 0; ij < idArr->length; ij++){
				jsid id = idArr->vector[ij]; 
				jsval v;
				if(!JS_IdToValue(cx, id, &v)) continue;
				if(!JSVAL_IS_STRING(v)) continue;
				jschar* pjcPName = JS_GetStringChars(JSVAL_TO_STRING(v));
				JS_GetUCProperty(cx, jsoVal, pjcPName, -1, &v);
				mlString wsPropVal = JSValToJSON(cx, v);
				if(wsPropVal.empty())
					continue;
				if(wsVal != L"{")
					wsVal += L',';
				wsVal += (const wchar_t*)pjcPName;
				wsVal += L':';
				wsVal += wsPropVal;
			}
			JS_DestroyIdArray(cx, idArr);
			wsVal += L"}";
			return wsVal;
		}
	}else{
	}
	return wsVal;
}

// преобразовать массив userProps в JSON-строку
mlString UserPropsToJSON(JSContext* cx, JSObject* obj){
	mlString wsUserProps;
	if(JS_IsArrayObject(cx, obj)){
		jsuint uiLen=0;
		if(JS_GetArrayLength(cx, obj, &uiLen)){
			for(unsigned int jj=0; jj<uiLen; jj++){
				jsval v;
				if(JS_GetElement(cx, obj, jj, &v)){
					if(JSVAL_IS_REAL_OBJECT(v)){
						mlString wsUserProps2 = JSValToJSON(cx, v);
						if(!wsUserProps2.empty()){
							if(!wsUserProps.empty())
								wsUserProps += L",";
							wsUserProps += wsUserProps2;
						}
					}
				}
			}
		}
		return wsUserProps;
	}
	return JSValToJSON(cx, OBJECT_TO_JSVAL(obj));
}

// создать в мире дубликат объекта
// в качестве параметра дается 3D-объект или 3D-группа и объект со свойствами:
//  string name - имя объекта (необязательный параметр)
//  reference parent - ссылка на объект-родитель (если отсутствует, то тот же, который у копируемого объекта)
//  string userProps - дополнительные свойства объекта в формате JSON (будет создано свойство userProps с массивом со ссылкой на объект)
//  Position3D position - позиция (абсолютная) (обязательный параметр)
//	Rotation3D rotation - поворот (по умолчанию - (0, 0, 0))
//  Scale3D scale - масштаб (по умолчанию - (1, 1, 1))
JSBool JSFUNC_duplicateObject(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_NULL;
	if(argc < 2){
		JS_ReportWarning(cx, "duplicateObject-method must get two arguments");
		return JS_TRUE;
	}
	jsval vArg = argv[0];
	if(!JSVAL_IS_REAL_OBJECT(vArg)){
		JS_ReportWarning(cx, "duplicateObject-method must get objects");
		return JS_TRUE;
	}
	JSObject* jsoSrc = JSVAL_TO_OBJECT(vArg);
	if(!JSO_IS_MLEL(cx, jsoSrc)){
		JS_ReportWarning(cx, "First argument of duplicateObject-method must be a 3D-object or 3D-group");
		return JS_TRUE;
	}
	mlRMMLElement* pMLEl = GET_RMMLEL(cx, jsoSrc);
	if(pMLEl->mRMMLType != MLMT_OBJECT && pMLEl->mRMMLType != MLMT_GROUP){
		JS_ReportWarning(cx, "First argument of duplicateObject-method must be a 3D-object or 3D-group");
		return JS_TRUE;
	}
	mlObjectInfo4OnServerCreation oOI4OSC;
	// забираем что можно из элемента, копию с которого делаем
	oOI4OSC.musType = pMLEl->mRMMLType;
	// (const char* mszClassName;	// имя класса объекта)
	std::string sClassName;
	if(pMLEl->GetClass() != NULL){
		const wchar_t* pwcClassName = pMLEl->GetClass()->GetName();
		if(pwcClassName != NULL && *pwcClassName != L'\0'){
			sClassName = cLPCSTR(pwcClassName);
			oOI4OSC.mszClassName = sClassName.c_str();
		}
	}
	// (const wchar_t* mwcSrc;	// Путь к ресурсам)
	const wchar_t* pwcSrc = pMLEl->GetLoadable()->GetSrc();
	mlString wsSrc;
	if(pwcSrc != NULL && *pwcSrc != L'\0'){
		wsSrc = pwcSrc;
		oOI4OSC.mwcSrc = wsSrc.c_str();
	}
	// (const mlMedia* mpParent;	// родительская 3D-сцена или 3D-группа)
	oOI4OSC.mpParent = pMLEl->mpParent;
	//
	mlString sUserProps;
	jsval v;
	if(JS_GetProperty(cx, jsoSrc, "userProps", &v) && (v != JSVAL_VOID)){
		if(JSVAL_IS_REAL_OBJECT(v)){
			sUserProps = UserPropsToJSON(cx, JSVAL_TO_OBJECT(v));
			if(!sUserProps.empty())
				oOI4OSC.mwcParams = sUserProps.c_str();
		}
	}
	// (ml3DRotation mRot;	// поворот объекта)
	oOI4OSC.mRot = pMLEl->Get3DObject()->GetRotation();
	oOI4OSC.mScl = pMLEl->Get3DObject()->GetScale();
	
	////
	// остальное извлекаем из второго аргумента метода
	vArg = argv[1];
	if(!JSVAL_IS_REAL_OBJECT(vArg)){
		JS_ReportWarning(cx, "duplicateObject-method must get objects");
		return JS_TRUE;
	}
	JSObject* jsoObj = JSVAL_TO_OBJECT(vArg);
	// извлекаем имя(const char* mszName;	// имя объекта)
	std::string sName;
	if(JS_GetProperty(cx, jsoObj, "name", &v) && (v != JSVAL_VOID)){
		if(!JSVAL_IS_STRING(v)){
			JS_ReportWarning(cx, "createObject argument object must get a string 'name' property");
			return JS_TRUE;
		}
		JSString* jss = JSVAL_TO_STRING(v);
		mlString str = (const wchar_t*)JS_GetStringChars(jss);
		sName = cLPCSTR(str.c_str());
	}
	oOI4OSC.mszName = sName.c_str();

	if(JS_GetProperty(cx, jsoObj, "parent", &v) && (v != JSVAL_VOID)){
		if(!JSVAL_IS_MLEL(cx, v)){
			JS_ReportWarning(cx, "'parent' property must be a reference to real RMML element object");
			return JS_TRUE;
		}
		mlRMMLElement* pMLEl = JSVAL_TO_MLEL(cx, v);
		if(!(pMLEl->mRMMLType == MLMT_SCENE3D || pMLEl->mRMMLType == MLMT_OBJECT || pMLEl->mRMMLType == MLMT_GROUP)){
			JS_ReportWarning(cx, "'parent' property must be a reference to real 3D-scene, 3D-object or 3D-group");
			return JS_TRUE;
		}
		oOI4OSC.mpParent = pMLEl;
	}
	//const wchar_t* mwcParams;	// скрипт инициализации (userProps)
	//sUserProps;
	if(JS_GetProperty(cx, jsoObj, "userProps", &v) && (v != JSVAL_VOID)){
		if(!JSVAL_IS_STRING(v)){
			JS_ReportWarning(cx, "createObject argument object must get a string 'userProps' property");
			return JS_TRUE;
		}
		JSString* jss = JSVAL_TO_STRING(v);
		sUserProps = (const wchar_t*)JS_GetStringChars(jss);
//		#pragma message("!!!!!!!!!! UNCOMMENT oOI4OSC.mwcParams = sUserProps.c_str(); !!!!!!!!!!!")
		oOI4OSC.mwcParams = sUserProps.c_str(); // !!! временно закомментировано, чтобы у доски положение камеры не копировалось
	}
	// position
	if((JS_GetProperty(cx, jsoObj, "position", &v) && (v != JSVAL_VOID)) || (JS_GetProperty(cx, jsoObj, "pos", &v) && (v != JSVAL_VOID))){
		if(!JSVAL_IS_REAL_OBJECT(v) || !mlPosition3D::IsInstance(cx, JSVAL_TO_OBJECT(v))){
			JS_ReportWarning(cx, "createObject argument object 'position' property must be a Position3D-object");
			return JS_TRUE;
		}
		mlPosition3D* pPos3D = (mlPosition3D*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(v));
		oOI4OSC.mPos = pPos3D->GetPos();
	}else{
		JS_ReportWarning(cx, "duplicateObject argument object 'position' property must be set");
		return JS_TRUE;
	}
	// rotation
	if(JS_GetProperty(cx, jsoObj, "rotation", &v) && (v != JSVAL_VOID)){
		if(!JSVAL_IS_REAL_OBJECT(v) || !mlRotation3D::IsInstance(cx, JSVAL_TO_OBJECT(v))){
			JS_ReportWarning(cx, "createObject argument object 'rotation' property must be a Rotation3D-object");
			return JS_TRUE;
		}
		mlRotation3D* pRot3D = (mlRotation3D*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(v));
		oOI4OSC.mRot = pRot3D->GetRot();
	}
	// scale
	if(JS_GetProperty(cx, jsoObj, "scale", &v) && (v != JSVAL_VOID)){
		if(!JSVAL_IS_REAL_OBJECT(v) || !mlScale3D::IsInstance(cx, JSVAL_TO_OBJECT(v))){
			JS_ReportWarning(cx, "createObject argument object 'scale' property must be a Scale3D-object");
			return JS_TRUE;
		}
		mlScale3D* pScl3D = (mlScale3D*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(v));
		oOI4OSC.mScl = pScl3D->GetScl();
	}
	// location
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	omsContext* pContext = pSM->GetContext();
	std::string sLocationID;
	if(pContext->mpMapMan != NULL){
		jsval vCreating3DOAnywareAllowed = JSVAL_VOID;
		JS_GetProperty(cx, obj, "creating3DOAnywareAllowed", &vCreating3DOAnywareAllowed);
		if(vCreating3DOAnywareAllowed == JSVAL_TRUE){
			sLocationID = "creating3DOAnywareAllowed";
			oOI4OSC.mszLocation = sLocationID.c_str();
		}else{
			ml3DLocation oLocation;
			if(pContext->mpMapMan->GetLocationByXYZ(oOI4OSC.mPos.x, oOI4OSC.mPos.y, oOI4OSC.mPos.z, oLocation)){
				if(oLocation.ID.mstr.size() > 0){
					sLocationID = cLPCSTR(oLocation.ID.mstr.c_str());
					oOI4OSC.mszLocation = sLocationID.c_str();
				}
			}
		}
	}

	// защищаем объект от удаления GC, чтобы потом вызвать у него onCreated или onError
//	SAVE_FROM_GC(cx, obj, jsoObj);
//	oOI4OSC.mCallback.mpParams = (void*)jsoObj; // MapManager вернет его при уведомлении о том, что объект (не)создан
	JSObject* jsoHandler = JS_NewObject(cx,NULL,NULL,NULL);
	SAVE_FROM_GC(cx, obj, jsoHandler);
	oOI4OSC.mCallback.mpParams = (void*)jsoHandler; // MapManager вернет его при уведомлении о том, что объект (не)создан
	if(pContext->mpMapMan == NULL){
		JS_ReportError(cx,"MapManager is not set");
		return JS_TRUE;
	}else{
		pSM->SetMapCallbacks();
		bool bRet = pContext->mpMapMan->CreateObject(&oOI4OSC, (unsigned int)jsoHandler);
		if(bRet){
			*rval = OBJECT_TO_JSVAL(jsoHandler);
			//*rval = BOOLEAN_TO_JSVAL(bRet);
		}
	}
	return JS_TRUE;
}

// удалить из мира объект
JSBool JSFUNC_deleteObject(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	if(argc == 0){
		JS_ReportWarning(cx, "deleteObject-method must get an argument");
		return JS_TRUE;
	}
	jsval vArg = argv[0];
	if(!JSVAL_IS_MLEL(cx, vArg)){
		JS_ReportWarning(cx, "deleteObject-method must get a RMML element object");
		return JS_TRUE;
	}
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	omsContext* pContext = pSM->GetContext();
	mlRMMLElement* pMLEl = JSVAL_TO_MLEL(cx, vArg);
	if(pContext->mpMapMan == NULL){
		JS_ReportError(cx,"MapManager is not set");
		return JS_TRUE;
	}else{
		pMLEl->DontSynchAwhile(); // Приостановить синхронизацию объекта
		pSM->SetMapCallbacks();
		bool bRet = pContext->mpMapMan->Remove(pMLEl);
		*rval = BOOLEAN_TO_JSVAL(bRet);
	}
	return JS_TRUE;
}

#define GET_BOUNDS_VAL(PROPNAME) { \
	jsval v; JS_GetProperty(cx, jsoBounds, #PROPNAME, &v); \
	jsdouble dVal = 0.0; \
	if(!JS_ValueToNumber(cx, v, &dVal)){ \
		JS_ReportWarning(cx, "findFreePlace second argument (bounds) '%s' property must be a number", #PROPNAME); \
	}else \
		bounds.PROPNAME = dVal; \
	}

// найти свободное место вблизи указанного
// Position3D Player.map.findFreePlace(Position3D pos, bounds, boundsRotation, maxDistance, location);
//   pos - точка, в которую хотелось бы поместить область
//   bounds - границы прямоугольной области (например, объекта), которую надо поместить
//   boundsRotation - поворот границ прямоугольной области
//   maxDistance - максимальное расстояние от pos до которого искать свободное место
//   location - идентификатор локации, в пределах которой возможно размещать объект
JSBool JSFUNC_findFreePlace(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc == 0){
		JS_ReportWarning(cx, "findFreePlace-method must get an argument");
		return JS_TRUE;
	}
	// pos
	ml3DPosition pos; ML_INIT_3DPOSITION(pos);
	jsval vPos = argv[0];
	if(!JSVAL_IS_REAL_OBJECT(vPos) || !mlPosition3D::IsInstance(cx, JSVAL_TO_OBJECT(vPos))){
		JS_ReportWarning(cx, "findFreePlace first argument (position) must be a Position3D-object");
		return JS_TRUE;
	}
	JSObject* jsoPos = JSVAL_TO_OBJECT(vPos);
	mlPosition3D* pPos3D = (mlPosition3D*)JS_GetPrivate(cx, jsoPos);
	pos = pPos3D->GetPos();
	ml3DBounds bounds; ML_INIT_3DBOUNDS(bounds);
	if(argc > 1){
		jsval vBounds = argv[1];
		if(!JSVAL_IS_REAL_OBJECT(vBounds)){
			JS_ReportWarning(cx, "findFreePlace second argument (bounds) must be an object");
			return JS_TRUE;
		}
		JSObject* jsoBounds = JSVAL_TO_OBJECT(vBounds);
		GET_BOUNDS_VAL(xMin);
		GET_BOUNDS_VAL(xMax);
		GET_BOUNDS_VAL(yMin);
		GET_BOUNDS_VAL(yMax);
		GET_BOUNDS_VAL(zMin);
		GET_BOUNDS_VAL(zMax);
	}
	//
	ml3DRotation rot; ML_INIT_3DROTATION(rot);
	if(argc > 2){
		jsval vRot = argv[2];
		if(!JSVAL_IS_REAL_OBJECT(vRot) || !mlRotation3D::IsInstance(cx, JSVAL_TO_OBJECT(vRot))){
			JS_ReportWarning(cx, "findFreePlace third argument (rotation) must be a Rotation3D-object");
			return JS_TRUE;
		}
		JSObject* jsoRot = JSVAL_TO_OBJECT(vRot);
		mlRotation3D* pRot3D = (mlRotation3D*)JS_GetPrivate(cx, jsoRot);
		rot = pRot3D->GetRot();
	}
	int maxDist = -1;
	if(argc > 3){
		jsval vMaxDist = argv[3];
		if(!JSVAL_IS_INT(vMaxDist)){
			JS_ReportWarning(cx, "findFreePlace fourth argument (maxDistance) must be an integer");
			return JS_TRUE;
		}
		maxDist = JSVAL_TO_INT(vMaxDist);
	}
	mlString wsLocationID;
	if(argc > 4){
		jsval vLocation = argv[4];
		if(!JSVAL_IS_STRING(vLocation)){
			JS_ReportWarning(cx, "findFreePlace fifth argument (location) must be a string");
			return JS_TRUE;
		}
		JSString* jssLocation = JSVAL_TO_STRING(vLocation);
		wsLocationID = (const wchar_t*)JS_GetStringChars(jssLocation);
	}
	//
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	omsContext* pContext = pSM->GetContext();
	ml3DPosition posRes; ML_INIT_3DPOSITION(posRes);

	if( !pContext->mpRM->FindFreePlace(pos, bounds, rot, maxDist, posRes, wsLocationID.c_str()))
	{
		// если ничего не нашли вернем начальную позицию
		posRes = pos;
	}

	JSObject* jsoNewP3D = mlPosition3D::newJSObject(cx);
	mlPosition3D* pNewPos = (mlPosition3D*)JS_GetPrivate(cx, jsoNewP3D);
	pNewPos->SetPos(posRes);
	*rval = OBJECT_TO_JSVAL(jsoNewP3D);

	return JS_TRUE;
}


JSBool JSFUNC_getVisibleLocations(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	omsContext* pContext = pSM->GetContext();
	*rval = JSVAL_FALSE;
	
	if (pContext!=NULL && pContext->mpMapMan!=NULL)
	{	std::vector<rmml::ml3DBounds> locations = pContext->mpMapMan->GetVisibleLocations();
		std::vector<rmml::ml3DBounds>::iterator it = locations.begin();
		std::vector<rmml::ml3DBounds>::iterator itEnd = locations.end();
		
		JSObject* jsoBounds = NULL;
		if (locations.size() != 0)
		{
			jsval* pjsvals = MP_NEW_ARR( jsval, locations.size());
			int i = 0;

			for ( ; it != itEnd; it++, i++)
			{
				rmml::ml3DBounds location = (rmml::ml3DBounds)(*it);
				JSObject* jsoBounds = JS_NewObject(cx,NULL,NULL,NULL);
				jsval jsv = JSVAL_NULL;
				mlJS_NewDoubleValue(cx,location.xMin,&jsv);
				JS_SetProperty(cx, jsoBounds, "xMin", &jsv);
			
				mlJS_NewDoubleValue(cx,location.xMax,&jsv);
				JS_SetProperty(cx, jsoBounds, "xMax", &jsv);
			
				mlJS_NewDoubleValue(cx,location.yMin,&jsv);
				JS_SetProperty(cx, jsoBounds, "yMin", &jsv);
			
				mlJS_NewDoubleValue(cx,location.yMax,&jsv);
				JS_SetProperty(cx, jsoBounds, "yMax", &jsv);

				mlJS_NewDoubleValue(cx,location.zMin,&jsv);
				JS_SetProperty(cx, jsoBounds, "zMin", &jsv);

				mlJS_NewDoubleValue(cx,location.zMax,&jsv);
				JS_SetProperty(cx, jsoBounds, "zMax", &jsv);

				pjsvals[i] = OBJECT_TO_JSVAL(jsoBounds);
			}
		
			jsoBounds = JS_NewArrayObject(cx,locations.size(),pjsvals);
		}
		else
		{
			jsoBounds = JS_NewArrayObject(cx, 0, NULL);
		}
		*rval = OBJECT_TO_JSVAL(jsoBounds);
		return JS_TRUE;

	}
	return JS_FALSE;
}

JSBool JSFUNC_getVisibleCommunicationAreas(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	omsContext* pContext = pSM->GetContext();
	*rval = JSVAL_FALSE;

	if (pContext!=NULL && pContext->mpMapMan!=NULL)
	{	std::vector<rmml::ml3DBounds> locations = pContext->mpMapMan->GetVisibleCommunicationAreas();
	std::vector<rmml::ml3DBounds>::iterator it = locations.begin();
	std::vector<rmml::ml3DBounds>::iterator itEnd = locations.end();

	JSObject* jsoBounds = NULL;
	if (locations.size() != 0)
	{
		jsval* pjsvals = MP_NEW_ARR( jsval, locations.size());
		int i = 0;

		for ( ; it != itEnd; it++, i++)
		{
			rmml::ml3DBounds location = (rmml::ml3DBounds)(*it);
			JSObject* jsoBounds = JS_NewObject(cx,NULL,NULL,NULL);
			jsval jsv = JSVAL_NULL;
			mlJS_NewDoubleValue(cx,location.xMin,&jsv);
			JS_SetProperty(cx, jsoBounds, "xMin", &jsv);

			mlJS_NewDoubleValue(cx,location.xMax,&jsv);
			JS_SetProperty(cx, jsoBounds, "xMax", &jsv);

			mlJS_NewDoubleValue(cx,location.yMin,&jsv);
			JS_SetProperty(cx, jsoBounds, "yMin", &jsv);

			mlJS_NewDoubleValue(cx,location.yMax,&jsv);
			JS_SetProperty(cx, jsoBounds, "yMax", &jsv);

			mlJS_NewDoubleValue(cx,location.zMin,&jsv);
			JS_SetProperty(cx, jsoBounds, "zMin", &jsv);

			mlJS_NewDoubleValue(cx,location.zMax,&jsv);
			JS_SetProperty(cx, jsoBounds, "zMax", &jsv);

			pjsvals[i] = OBJECT_TO_JSVAL(jsoBounds);
		}

		jsoBounds = JS_NewArrayObject(cx,locations.size(),pjsvals);
	}
	else
	{
		jsoBounds = JS_NewArrayObject(cx, 0, NULL);
	}
	*rval = OBJECT_TO_JSVAL(jsoBounds);
	return JS_TRUE;

	}
	return JS_FALSE;
}

JSBool JSFUNC_isLocationCanBeCreated(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	omsContext* pContext = pSM->GetContext();
	*rval = JSVAL_FALSE;
	if(argc != 2){
		JS_ReportWarning(cx, "JSFUNC_isLocationCanBeCreated-method must get two arguments");
		return JS_TRUE;
	}

	ml3DPosition leftPoint;
	ml3DPosition rightPoint;

	if(!JSVAL_IS_REAL_OBJECT(argv[0])){
		JS_ReportError(cx,"First parameter of isLocationCanBeCreated(..) must be a 3D-point object");
		return JS_TRUE;
	}
		
	JSObject* jsoLeftPoint = JSVAL_TO_OBJECT(argv[0]);
	if(!mlPosition3D::IsInstance(cx, jsoLeftPoint)){
		JS_ReportError(cx,"First parameter of isLocationCanBeCreated(..) must be a 3D-point object");
		return JS_TRUE;
	}
		
	mlPosition3D* pLeftPoint = (mlPosition3D*)JS_GetPrivate(cx, jsoLeftPoint);
	leftPoint = pLeftPoint->GetPos();

	if(!JSVAL_IS_REAL_OBJECT(argv[1])){
		JS_ReportError(cx,"Secound parameter of isLocationCanBeCreated(..) must be a 3D-point object");
		return JS_TRUE;
	}

	JSObject* jsoRightPoint = JSVAL_TO_OBJECT(argv[1]);
	if(!mlPosition3D::IsInstance(cx, jsoRightPoint)){
		JS_ReportError(cx,"Secound parameter of isLocationCanBeCreated(..) must be a 3D-point object");
		return JS_TRUE;
	}

	mlPosition3D* pRightPoint = (mlPosition3D*)JS_GetPrivate(cx, jsoRightPoint);
	rightPoint = pRightPoint->GetPos();

	// Возвратим код ошибки
	*rval = INT_TO_JSVAL( pContext->mpMapMan->IsLocationCanBeCreated( &leftPoint, &rightPoint));
	return JS_TRUE;
}

// проверить, лежит ли точка в видимых зонах
// Position3D Player.map.isPointInActiveZones( x, y);
JSBool JSFUNC_isPointInActiveZones(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	if(argc != 2){
		JS_ReportWarning(cx, "JSFUNC_isPointInActiveZones-method must get two arguments");
		return JS_TRUE;
	}
	
	jsdouble jsx;
	jsdouble jsy;
	double x = 0.0;
	double y = 0.0;
	if(!JS_ValueToNumber(cx, argv[0], &jsx))
	{
		JS_ReportError(cx,"First parameter of isPointInActiveZones(..) must be a double (x) coordinate)");
		return JS_FALSE;
	}
	x = jsx;

	if(!JS_ValueToNumber(cx, argv[1], &jsy))
	{
		JS_ReportError(cx,"Second parameter of isPointInActiveZones(..) must be a double (y) coordinate)");
		return JS_FALSE;
	}
	y = jsy;

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	omsContext* pContext = pSM->GetContext();

	*rval = BOOLEAN_TO_JSVAL( pContext->mpMapMan->IsPointInActiveZones( x, y));

	return JS_TRUE;
}

// назначить 3D сцену для объектов мира
// Player.map.setWorldScene(Scene3D scene);
JSBool JSFUNC_setWorldScene(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	if(argc == 0){
		JS_ReportWarning(cx, "setWorldScene-method must get an argument");
		return JS_TRUE;
	}
	// scene
	jsval vScene = argv[0];
	if(!JSVAL_IS_REAL_OBJECT(vScene) || !mlRMMLScene3D::IsInstance(cx, JSVAL_TO_OBJECT(vScene)))
	{
		JS_ReportWarning(cx, "setWorldScene first argument (scene) must be a Scene3D-object");
		return JS_TRUE;
	}

	JSObject* jsoScene = JSVAL_TO_OBJECT(vScene);
	mlRMMLScene3D* pWorldScene = (mlRMMLScene3D*)JS_GetPrivate(cx, jsoScene);

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetWorldScene( pWorldScene);

	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

// получить описание локации в точке с заданными координатами
// object getLocationByPosition(x,y[,z])
JSBool JSFUNC_getLocationByPosition(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_NULL; // вне локации
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	omsContext* pContext = pSM->GetContext();
	double dX = 0.0, dY = 0.0, dZ = 0.0;

	ml3DPosition pos3dParam; ML_INIT_3DPOSITION(pos3dParam);

	bool bError = true;
	for(;;){
		jsval vObj = argv[0];
		if(!JSVAL_IS_REAL_OBJECT(vObj)){
			break;
		}
		JSObject* jsoObj = JSVAL_TO_OBJECT(vObj);
		if(mlPosition3D::IsInstance(cx, jsoObj)){
			mlPosition3D* pPosObj = (mlPosition3D*)JS_GetPrivate(cx, jsoObj);
			pos3dParam = pPosObj->GetPos();
			bError = false;
			break;
		}else if(cx, JSVAL_IS_MLEL(cx, vObj)){
			mlRMMLElement* pMLEL = JSVAL_TO_MLEL(cx, vObj);
			mlRMML3DObject* p3dObj = pMLEL->Get3DObject();
			if(p3dObj != NULL){
				pos3dParam = p3dObj->GetPos();
				bError = false;
				break;
			}
		}
		break;
	}
	if(!bError){
		dX = pos3dParam.x;
		dY = pos3dParam.y;
		dZ = pos3dParam.z;
	}
	if(bError && argc < 2){
		JS_ReportError(cx,"getLocationByPosition must get two numbers at least");
		return JS_TRUE;
	}
	if(bError && argc > 0){
		if(argc >= 1){
			jsval v = argv[0];
			if(!JSVAL_IS_NUMBER(v)){
				JS_ReportError(cx,"Arguments of getLocationByPosition must be numbers");
				return JS_TRUE;
			}
			JS_ValueToNumber(cx, v, &dX);
		}
		if(argc >= 2){
			jsval v = argv[1];
			if(!JSVAL_IS_NUMBER(v)){
				JS_ReportError(cx,"Arguments of getLocationByPosition must be numbers");
				return JS_TRUE;
			}
			JS_ValueToNumber(cx, v, &dY);
		}
		if(argc >= 3){
			jsval v = argv[2];
			if(!JSVAL_IS_NUMBER(v)){
				JS_ReportError(cx,"Arguments of getLocationByPosition must be numbers");
				return JS_TRUE;
			}
			JS_ValueToNumber(cx, v, &dZ);
		}
		bError = false;
	}
	if(bError){
		JS_ReportError(cx,"Argument of getLocationByPosition(..) must be a 3D-position object or a 3D-object");
		return JS_TRUE;
	}

	ml3DLocation oLocation;
	if(pContext->mpMapMan->GetLocationByXYZ( dX, dY, dZ, oLocation)){
		JSObject* jsoLocation = pSM->LocationToJSObject(oLocation, true);
		*rval = OBJECT_TO_JSVAL(jsoLocation);
	}
	return JS_TRUE;
}

// получить параметры локации в виде строки (это значение из objects_map.params)
// object getLocationParams(locID)
JSBool JSFUNC_getLocationParams(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){

	wchar_t		pwcValue[3000] = L"";
	*rval = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)pwcValue)); // нет параметров

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	omsContext* pContext = pSM->GetContext();

	if( argc != 1){
		JS_ReportError(cx,"getLocationParams(..) must take one string parameter");
		return JS_TRUE;
	}

	if(!JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx,"Parameter of getLocationParams(..) must be a string");
		return JS_TRUE;
	}

	JSString*	jssLocationID = JSVAL_TO_STRING(argv[0]);
	wchar_t*	pwcLocationID = (wchar_t*)JS_GetStringChars(jssLocationID);

	if(pContext->mpMapMan->GetLocationParams( pwcLocationID, pwcValue, sizeof(pwcValue)/sizeof(pwcValue[0]) - 1))
	{
		*rval = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)pwcValue));
	}
	return JS_TRUE;
}

// разрешить/запретить пользователю посещать определенную локацию в определенной реальности
JSBool JSFUNC_setLocationEnteringPermissionForUser(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	omsContext* pContext = pSM->GetContext();

	if( argc != 4){
		JS_ReportError(cx,"setLocationEnteringPermissionForUser must take three arguments (user_login, location_id, reality_id, permission_value)");
		return JS_TRUE;
	}

	// 1 param
	if(!JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx,"First parameter for setLocationEnteringPermissionForUser must be a string");
		return JS_TRUE;
	}
	JSString*	jssUserLogin = JSVAL_TO_STRING(argv[0]);
	wchar_t*	pwcUserLogin = (wchar_t*)JS_GetStringChars(jssUserLogin);

	// 2 param
	if(!JSVAL_IS_STRING(argv[1]))
	{
		JS_ReportError(cx,"Second parameter for setLocationEnteringPermissionForUser must be a string");
		return JS_TRUE;
	}
	JSString*	jssLocationID = JSVAL_TO_STRING(argv[1]);
	wchar_t*	pwcLocationID = (wchar_t*)JS_GetStringChars(jssLocationID);

	// 3 param
	if(!JSVAL_IS_NUMBER(argv[2]))
	{
		JS_ReportError(cx,"Third parameter for setLocationEnteringPermissionForUser must be a number");
		return JS_TRUE;
	}
	int			realityID = JSVAL_TO_INT(argv[2]);

	// 4 param
	if(!JSVAL_IS_BOOLEAN(argv[3]))
	{
		JS_ReportError(cx,"Forth parameter for setLocationEnteringPermissionForUser must be a boolean");
		return JS_TRUE;
	}
	bool	permissionValue = JSVAL_TO_BOOLEAN(argv[3]);
	
	pContext->mpSyncMan->SetLocationEnteringPermissionForUser(pwcUserLogin, pwcLocationID, realityID, permissionValue);
	return JS_TRUE;
}

// получить новые координаты перемещаемого объекта в на границе локации локации
JSBool JSFUNC_getNewPositionObjectOnBoundLocation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	omsContext* pContext = pSM->GetContext();

	if( argc != 3){
		JS_ReportError(cx,"LocationID must take one string parameter");
		return JS_TRUE;
	}

	if(!JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx,"Parameter of LocationID must be a string");
		return JS_TRUE;
	}

	JSString*	jssLocationID = JSVAL_TO_STRING(argv[0]);
	wchar_t*	pwcLocationID = (wchar_t*)JS_GetStringChars(jssLocationID);

	ml3DPosition oAvatarPos;
	jsval vAvatarPos = argv[1];
	if(!JSVAL_IS_REAL_OBJECT(vAvatarPos)){
		JS_ReportError(cx,"Second parameter must be a 3D-position object");
		return JS_TRUE;
	}
	JSObject* jsoAvatarPos = JSVAL_TO_OBJECT(vAvatarPos);
	if(!mlPosition3D::IsInstance(cx, jsoAvatarPos)){
		JS_ReportError(cx,"Second parameter must be a 3D-position object");
		return JS_TRUE;
	}
	mlPosition3D* pAvatarPos = (mlPosition3D*)JS_GetPrivate(cx, jsoAvatarPos);
	oAvatarPos = pAvatarPos->GetPos();

	ml3DPosition oObjectPos;
	jsval vDestPos = argv[2];
	if(!JSVAL_IS_REAL_OBJECT(vDestPos)){
		JS_ReportError(cx,"Second parameter must be a 3D-position object");
		return JS_TRUE;
	}
	JSObject*jsoDestPos = JSVAL_TO_OBJECT(vDestPos);
	if(!mlPosition3D::IsInstance(cx, jsoDestPos)){
		JS_ReportError(cx,"Second parameter must be a 3D-position object");
		return JS_TRUE;
	}
	mlPosition3D*pDestPos = (mlPosition3D*)JS_GetPrivate(cx, jsoDestPos);
	oObjectPos = pDestPos->GetPos();

	ml3DPosition ptObject;
	//const wchar_t *apwcLocationID, float xMyAvatar, float yMyAvatar, float x, float y, CPoint &ptObjNew
	if(pContext->mpMapMan->GetNewPositionObjectOnBoundLocation(pwcLocationID, oAvatarPos, oObjectPos, ptObject))
	{
		/*JSObject* jsoLocation = pSM->LocationToJSObject(oLocation, true);
			*rval = OBJECT_TO_JSVAL(jsoLocation);
		}*/
		JSObject* jsoNewP3D = mlPosition3D::newJSObject(cx);
		mlPosition3D* pNewPos = (mlPosition3D*)JS_GetPrivate(cx, jsoNewP3D);
		pNewPos->SetPos(ptObject);
		*rval = OBJECT_TO_JSVAL(jsoNewP3D);
	}
	else
		*rval = OBJECT_TO_JSVAL(jsoDestPos);
	return JS_TRUE;
}

mlMapCallbackManager::mlMapCallbackManager(mlSceneManager* apSM){
	mpSM = apSM;
	mbCallbacksIsSet = false;
}

void mlMapCallbackManager::SetCallbacks(JSContext *cx){
	if(mbCallbacksIsSet) return;
	omsContext* pOMSContext = mpSM->GetContext();
	if(pOMSContext == NULL) return;
	if(pOMSContext->mpMapMan == NULL)
		return;
	mbCallbacksIsSet = pOMSContext->mpMapMan->SetCallbacks(this);
}

void mlMapCallbackManager::onTeleported( cm::cmTeleportInfo* aTeleportInfo){
	// Поместить ответ в очередь событий
	omsContext* pOMSContext = mpSM->GetContext();
	if(pOMSContext == NULL) return;
	mpSM->SetCurrentRealityID( aTeleportInfo->uRealityID);
	mlSynchData oData;
	oData << aTeleportInfo->x;
	oData << aTeleportInfo->y;
	oData << aTeleportInfo->z;
	oData << aTeleportInfo->rx;
	oData << aTeleportInfo->ry;
	oData << aTeleportInfo->rz;
	oData << aTeleportInfo->ra;
	oData << aTeleportInfo->uRealityID;
	pOMSContext->mpInput->AddCustomEvent(mlSceneManager::CEID_mapTeleported, (unsigned char*)oData.data(), oData.size());
}

void mlMapCallbackManager::onTeleportedInternal(mlSynchData& aData){
	// После телепортации продолжаем посылать на сервер состояния
	mpSM->ResumeSynchStatesSending();

	cm::cmTeleportInfo teleportInfo;
	aData >> teleportInfo.x;
	aData >> teleportInfo.y;
	aData >> teleportInfo.z;
	aData >> teleportInfo.rx;
	aData >> teleportInfo.ry;
	aData >> teleportInfo.rz;
	aData >> teleportInfo.ra;
	aData >> teleportInfo.uRealityID;

	// ?? поместить errorCode и error в Event
	JSContext* cx = mpSM->GetJSContext();
	JSObject* jsoPos = mlPosition3D::newJSObject(cx);
	mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, jsoPos);
	ml3DPosition pos; 
	pos.x = teleportInfo.x;
	pos.y = teleportInfo.y;
	pos.z = teleportInfo.z;
	pPos->SetPos(pos);
	jsval vPos = OBJECT_TO_JSVAL(jsoPos);
	JS_SetProperty(cx, GPSM(cx)->GetEventGO(), "position", &vPos);
	//
	JSObject* jsoRot = mlRotation3D::newJSObject(cx);
	mlRotation3D* pRot = (mlRotation3D*)JS_GetPrivate(cx, jsoRot);
	ml3DRotation rot; 
	rot.x = teleportInfo.rx;
	rot.y = teleportInfo.ry;
	rot.z = teleportInfo.rz;
	rot.a = teleportInfo.ra;
	pRot->SetRot(rot);
	jsval vRot = OBJECT_TO_JSVAL(jsoRot);
	JS_SetProperty(cx, GPSM(cx)->GetEventGO(), "rotation", &vRot);
	//
	mpSM->NotifyMapListeners(EVID_onTeleported);

	JS_DeleteProperty(cx, GPSM(cx)->GetEventGO(), "position");
	JS_DeleteProperty(cx, GPSM(cx)->GetEventGO(), "rotation");
}

void mlMapCallbackManager::onTeleportError(int aiError, const wchar_t* apwcError, const wchar_t* apwcEventInfo){
	// Поместить ответ в очередь событий
	omsContext* pOMSContext = mpSM->GetContext();
	if(pOMSContext == NULL) return;
	mlSynchData oData;
	oData << aiError;
	if(apwcError != NULL){
		oData << apwcError;
	}else{
		oData << L"";
	}
	if(apwcEventInfo != NULL){
		oData << apwcEventInfo;
	}else{
		oData << L"";
	}
	pOMSContext->mpInput->AddCustomEvent(mlSceneManager::CEID_mapTeleportError, (unsigned char*)oData.data(), oData.size());
}

void mlMapCallbackManager::onTeleportErrorInternal(mlSynchData& aData){
	int iErrorCode;
	wchar_t* pwcError = L"";
	std::wstring wsError;
	wchar_t* pwcEventInfo = L"";
	std::wstring wsEventInfo;
	aData >> iErrorCode;
	aData >> pwcError;
	if( pwcError)
		wsError = pwcError;
	aData >> pwcEventInfo;
	if( pwcEventInfo)
		wsEventInfo = pwcEventInfo;

	mlString sDescription(L"[");
	sDescription = sDescription + wsEventInfo + L"]";

	// ?? поместить errorCode и error в Event
	JSContext* cx = mpSM->GetJSContext();
	jsval v = INT_TO_JSVAL( iErrorCode);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"teleportErrorCode",&v);

	unsigned int uLength = sDescription.size();
	JSContext *mcx = (mpSM->GetJSContext());
	bool abEventInfoWaSet = false;
	JSBool bR = JS_EvaluateUCScript(mcx, JS_GetGlobalObject(mcx), (const jschar*)sDescription.c_str(), uLength, "teleportEventInfo", 1, &v);
	if (JSVAL_IS_REAL_OBJECT(v))
	{	
		JSObject *obj=JSVAL_TO_OBJECT(v);
		JS_GetElement(mcx,obj,0,&v);
		if (JSVAL_IS_REAL_OBJECT(v))
		{
			abEventInfoWaSet = true;
			JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "teleportEventInfo", &v);
		}
	}
	// При ошибке телепортации продолжаем посылать на сервер состояния
	mpSM->ResumeSynchStatesSending();
	mpSM->NotifyMapListeners(EVID_onTeleportError);

	if( abEventInfoWaSet)
	{
		JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "teleportEventInfo");
	}
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "teleportErrorCode");
}

void mlMapCallbackManager::onURLChangedInternal(mlSynchData& aData)
{
	wchar_t *pURL = 0;
	aData >> pURL;
	jsval vParam = JSVAL_NULL;
	JSContext* cx = mpSM->GetJSContext();
	JSString *ssURL = JS_NewUCStringCopyZ(cx, (const jschar*)pURL);
	vParam = STRING_TO_JSVAL(ssURL);
	JS_SetProperty(cx, GPSM(cx)->GetEventGO(), "url", &vParam );
	mpSM->NotifyMapListeners(EVID_onURLChanged);
	JS_DeleteProperty(cx, GPSM(cx)->GetEventGO(), "url");
}

void mlMapCallbackManager::onShowZoneInternal(mlSynchData& aData){
	bool bVal;
	aData >> bVal;
	JSContext* cx = mpSM->GetJSContext();
	jsval v = BOOLEAN_TO_JSVAL( bVal);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"bShowZone",&v);
	mpSM->NotifyMapListeners(EVID_onShowZone);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "bShowZone");
}


void mlMapCallbackManager::OnUserSleepInternal()
{
	mpSM->NotifyMapListeners(EVID_onUserSleep);
}

void mlMapCallbackManager::OnUserWakeUpInternal()
{
	mpSM->NotifyMapListeners(EVID_onUserWakeUp);
}


// объект был создан по запросу из Player.map.CreateObject()
// (aiUserData - ссылка на объект, переданный в CreateObject)
void mlMapCallbackManager::onObjectCreated(int aiUserData, unsigned int auiObjectID, unsigned int auiBornRealityID, int aiError, const wchar_t* apwcError){
	omsContext* pOMSContext = mpSM->GetContext();
	mlSynchData oData;
	oData << aiUserData;
	oData << auiObjectID;
	oData << auiBornRealityID;
	oData << aiError;
	if(apwcError != NULL)
		oData << apwcError;	
	else
		oData << L"";
	pOMSContext->mpInput->AddCustomEvent(mlSceneManager::CEID_mapObjectCreated, (unsigned char*)oData.data(), oData.size());
}


void mlMapCallbackManager::onObjectCreatedInternal(mlSynchData& aData){
	int iUserData = 0;
	aData >> iUserData;
	unsigned int uiObjectID;
	aData >> uiObjectID;
	unsigned int uiBornRealityID;
	aData >> uiBornRealityID;
	int iError;
	aData >> iError;
	wchar_t* pwcError;
	aData >> pwcError;
	mlString sError = pwcError;
	if(iUserData == 0)
		return;
	JSContext* cx = mpSM->GetJSContext();
	JSObject* jsoMap = mpSM->mjsoMap;
	JSObject* jsoUserData = (JSObject*)iUserData;
	jsval v;
	GET_SAVED_FROM_GC(cx, jsoMap, jsoUserData, &v);
	if(!JSVAL_IS_REAL_OBJECT(v)){
		mlTrace(mpSM->GetJSContext(), "3D-object creating event handler object not found\n");
		return;
	}
	JSObject* jso = JSVAL_TO_OBJECT(v);
	FREE_FOR_GC(cx, jsoMap, jso);
	// получить ссылку на метод и вызвать его
	const char* pszEvent = "onCreated";
	bool bError = false;
	if(iError != 0){
		jsval vErrorCode = INT_TO_JSVAL(iError);
		JS_SetProperty(cx, jso, "errorCode", &vErrorCode);
		JSString* jssError = JS_NewUCStringCopyZ(cx, (const jschar*)pwcError);
		jsval vError = STRING_TO_JSVAL(jssError);
		JS_SetProperty(cx, jso, "error", &vError);
		bError = true;
		pszEvent = "onError";
	}else{
		// найти объект и ссылку на него поместить в это объект
		mlRMMLElement* pMLEl = GPSM(cx)->GetScriptedObjectByID(uiObjectID, uiBornRealityID);
		if(pMLEl != NULL){
			jsval vObj = OBJECT_TO_JSVAL(pMLEl->mjso);
			JS_SetProperty(cx, jso, "ref", &vObj);
			pMLEl->ForceWholeStateSending(); // регистрим объект в SynchServer-е
		}
	}
	// если есть свойство onXXX
	jsval vFunc;
	if(JS_GetProperty(cx, jso, pszEvent, &vFunc) == JS_FALSE)
		return;
	if(!JSVAL_IS_STRING(vFunc)){
		if(!JSVAL_IS_REAL_OBJECT(vFunc))
			return;
		if(!JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(vFunc)))
			return;
	}
	if(vFunc != JSVAL_NULL){
		// вызываем функцию
		jsval v;
		if(JSVAL_IS_STRING(vFunc)){
			JSString* jss = JSVAL_TO_STRING(vFunc);
			JS_EvaluateUCScript(cx, jso, JS_GetStringChars(jss), JS_GetStringLength(jss), 
				pszEvent, 1, &v);
		}else{
			JS_CallFunctionValue(cx, jso, vFunc, 0, NULL, &v);
		}
	}
}



void mlMapCallbackManager::onReceivedObjectStatus(unsigned int auObjectID, unsigned int auBornRealityID, int aiStatus, void* apUserData){
	omsContext* pOMSContext = mpSM->GetContext();
	mlSynchData oData;
	oData << auObjectID;
	oData << auBornRealityID;
	oData << aiStatus;
	oData << (unsigned int)apUserData;
	pOMSContext->mpInput->AddCustomEvent(mlSceneManager::CEID_mapObjectStatusReceived, (unsigned char*)oData.data(), oData.size());
}

// Сгенерировать событие Player.server.recorder.onRecordAfterCreatingOfObjects
// когда загрузилась и создалась часть объектов воспроизводимой записи
// aiAfterCreatingOfObjects - сколько объектов загружено из aiObjectCount ожидаемых
void mlSceneManager::NotifyAfterCreatingOfObjects(int aiObjectCreated, int aiObjectCount){
	jsval jsvObjectsLoaded = INT_TO_JSVAL(aiObjectCreated);
	JS_SetProperty(cx, GPSM(cx)->GetEventGO(), "objectCreated", &jsvObjectsLoaded);
	jsval jsvObjectCount = INT_TO_JSVAL(aiObjectCount);
	JS_SetProperty(cx, GPSM(cx)->GetEventGO(), "objectCount", &jsvObjectCount);
	NotifyMapListeners(EVID_onAfterCreatingOfObjects);
	JS_DeleteProperty(cx, GPSM(cx)->GetEventGO(), "objectCreated");
	JS_DeleteProperty(cx, GPSM(cx)->GetEventGO(), "objectCount");
}

void mlSceneManager::NotifyBeforeCreatingOfObjects(){
	NotifyMapListeners(EVID_onBeforeCreatingOfObjects);
}

}

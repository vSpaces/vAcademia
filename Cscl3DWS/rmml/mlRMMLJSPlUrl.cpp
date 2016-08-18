#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"
#include "mlRMMLJSPlUrl.h"

namespace rmml {

	UrlProperties	urlStorage;
/**
 * Реализация глобального JS-объекта "Url"
 */

JSBool JSUrlGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool JSUrlSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	
JSClass JSRMMLUrlClass = {
	"_Url", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, 
	JSUrlGetProperty, JSUrlSetProperty,
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL,
	NULL, NULL, NULL
};

enum {
	JSPROP_location=1,
	JSPROP_place,
	JSPROP_x,
	JSPROP_y,
	JSPROP_z,
	JSPROP_zoneId,
	JSPROP_action,
	JSPROP_actionParam,
	TIDE_mlRMMLUrl
};

enum {
	EVID_onUrlSet = 0,
	TEVIDE_mlRMMLUrl
};

JSPropertySpec _JSUrlPropertySpec[] = {
	JSPROP_RO(location)	// 
	JSPROP_RO(place)	// 
	JSPROP_RO(x)		// 
	JSPROP_RO(y)		// 
	JSPROP_RO(z)		// 
	JSPROP_RO(zoneId)	// 
	JSPROP_RO(action)	// 
	JSPROP_RO(actionParam)	// 
	{ 0, 0, 0, 0, 0 }
};

// Function declare
// JSFUNC_DECL(..)


///// JavaScript Function Table
JSFunctionSpec _JSUrlFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

void CreateUrlJSObject(JSContext *cx, JSObject *ajsoPlayer){
    JSObject* jsoUrl = JS_DefineObject(cx, ajsoPlayer, GJSONM_URL, &JSRMMLUrlClass, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
	JSBool bR;
	bR=JS_DefineProperties(cx, jsoUrl, _JSUrlPropertySpec);
	bR=JS_DefineFunctions(cx, jsoUrl, _JSUrlFunctionSpec);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, ajsoPlayer);
	JS_SetPrivate(cx,jsoUrl,pSM);
	pSM->mjsoUrl = jsoUrl;
	//
}

char GetUrlEventID(const wchar_t* apwcEventName){
	if(isEqual(apwcEventName,L"onSet"))
		return (char)EVID_onUrlSet;
	return -1;
}

wchar_t* GetUrlEventName(char aidEvent){
	switch(aidEvent){
	case (char)EVID_onUrlSet:
		return L"onSet";
	}
	return L"???";
}

__forceinline int GetUrlEventIdx(char aidEvent){
	switch(aidEvent){
	case EVID_onUrlSet:
		return aidEvent;
	}
	return 0;
}

bool mlSceneManager::addUrlListener(char aidEvent, mlRMMLElement* apMLEl){
	if(apMLEl == NULL) return false;
	char chEvID=aidEvent;
	if(chEvID >= ML_URL_EVENTS_COUNT) return false;
	v_elems* pv=&(maUrlListeners[chEvID]);
	v_elems::const_iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl) return true;
	}
	pv->push_back(apMLEl);
	return true;
}

void mlSceneManager::NotifyUrlListeners(char chEvID){
	if(chEvID < ML_URL_EVENTS_COUNT){
		v_elems* pv=&(maUrlListeners[chEvID]);
		v_elems::const_iterator vi;
		for(vi=pv->begin(); vi != pv->end(); vi++ ){
			mlRMMLElement* pMLEl=*vi;
			pMLEl->EventNotify(chEvID,(mlRMMLElement*)GOID_Url);
		}
	}
}

bool mlSceneManager::removeUrlListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	if(aidEvent==-1){
		// и удалим его из списка слушателей всех событий
		for(int ii=0; ii<ML_URL_EVENTS_COUNT; ii++){
			v_elems* pv=&maUrlListeners[ii];
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
	v_elems* pv=&(maUrlListeners[aidEvent-1]);
	v_elems::iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl){
			pv->erase(vi);
			return true;
		}
	}
	return true;
}

void mlSceneManager::NotifyUrlOnSet()
{
	NotifyUrlListeners(EVID_onUrlSet);
}

void mlSceneManager::ResetUrl(){
	for(int ii=0; ii<ML_URL_EVENTS_COUNT; ii++){
		maUrlListeners[ii].clear();
	}
}

JSBool JSUrlGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){ 
	if(JSVAL_IS_INT(id)){
		mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
		int iID=JSVAL_TO_INT(id);
		switch(iID){
		case JSPROP_location:{
			if(urlStorage.wcLocation[0] == 0){
				*vp = JS_GetEmptyStringValue(cx);
			}else{
				*vp = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)urlStorage.wcLocation));
			}
		}break;
		case JSPROP_place:{
			if(urlStorage.wcPlace[0] == 0){
				*vp = JS_GetEmptyStringValue(cx);
			}else{
				*vp = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)urlStorage.wcPlace));
			}
		}break;
		case JSPROP_action:{
			if(urlStorage.wcAction[0] == 0){
				*vp = JS_GetEmptyStringValue(cx);
			}else{
				*vp = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)urlStorage.wcAction));
			}
		}break;
		case JSPROP_actionParam:{
			if(urlStorage.wcActionParam[0] == 0){
				*vp = JS_GetEmptyStringValue(cx);
			}else{
				*vp = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)urlStorage.wcActionParam));
			}
		}break;
		case JSPROP_x:{
			jsdouble* jsdbl=JS_NewDouble(cx, urlStorage.x);
			*vp = DOUBLE_TO_JSVAL(jsdbl);
		}break;
		case JSPROP_y:{
			jsdouble* jsdbl=JS_NewDouble(cx, urlStorage.y);
			*vp = DOUBLE_TO_JSVAL(jsdbl);
		}break;
		case JSPROP_z:{
			jsdouble* jsdbl=JS_NewDouble(cx, urlStorage.z);
			*vp = DOUBLE_TO_JSVAL(jsdbl);
		}break;
		case JSPROP_zoneId:{
			*vp = INT_TO_JSVAL(urlStorage.zoneID);
		}break;
		}
	}
	return JS_TRUE; 
}

JSBool JSUrlSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){ 
	return JS_TRUE; 
}

}


#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"
#include "mlRMMLJSPlVideoCapture.h"
#include "ILogWriter.h"

namespace rmml {

/**
 * Реализация глобального JS-объекта "VideoCapture"
 */
JSBool JSVideoCaptureGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool JSVideoCaptureSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	
JSClass JSRMMLVideoCaptureClass = {
	"_VideoCapture", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, 
	JSVideoCaptureGetProperty, JSVideoCaptureSetProperty,
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL,
	NULL, NULL, NULL
};

enum {
	TIDE_mlRMMLVideoCapture
};

enum {
	EVID_onCaptureError = 0,
	TEVIDE_mlRMMLVideoCapture
};

JSPropertySpec _JSVideoCapturePropertySpec[] = {
	{ 0, 0, 0, 0, 0 }
};

// Function declare
JSFUNC_DECL(start)
JSFUNC_DECL(end)
JSFUNC_DECL(getVideoURL)
JSFUNC_DECL(canVideoBeStarted)
JSFUNC_DECL(setVideoSize)

///// JavaScript Function Table
JSFunctionSpec _JSVideoCaptureFunctionSpec[] = {
	JSFUNC(start,1)
	JSFUNC(end,0)
	JSFUNC(getVideoURL,0)	
	JSFUNC(canVideoBeStarted,0)	
	JSFUNC(setVideoSize,2)	
	{ 0, 0, 0, 0, 0 }
};

void CreateVideoCaptureJSObject(JSContext *cx, JSObject *ajsoPlayer){
    JSObject* jsoVideoCapture = JS_DefineObject(cx, ajsoPlayer, GJSONM_VIDEOCAPTURE, &JSRMMLVideoCaptureClass, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
	JSBool bR;
	bR=JS_DefineProperties(cx, jsoVideoCapture, _JSVideoCapturePropertySpec);
	bR=JS_DefineFunctions(cx, jsoVideoCapture, _JSVideoCaptureFunctionSpec);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, ajsoPlayer);
	JS_SetPrivate(cx,jsoVideoCapture,pSM);
	pSM->mjsoVideoCapture = jsoVideoCapture;
	//
}

char GetVideoCaptureEventID(const wchar_t* apwcEventName){
	if(isEqual(apwcEventName,L"onCaptureError"))
		return (char)EVID_onCaptureError;
	return -1;
}

wchar_t* GetVideoCaptureEventName(char aidEvent){
	switch(aidEvent){
	case (char)EVID_onCaptureError:
		return L"onCaptureError";
	}
	return L"???";
}

bool mlSceneManager::addVideoCaptureListener(char aidEvent, mlRMMLElement* apMLEl){
	if(apMLEl == NULL) return false;
	char chEvID=aidEvent;
	if(chEvID >= ML_VIDEOCAPTURE_EVENTS_COUNT) return false;
	v_elems* pv=&(maVideoCaptureListeners[chEvID]);
	v_elems::const_iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl) return true; 
	}
	pv->push_back(apMLEl);
	return true;
}

void mlSceneManager::NotifyVideoCaptureListeners(char chEvID){
	if(chEvID < ML_VIDEOCAPTURE_EVENTS_COUNT){ 
		v_elems* pv=&(maVideoCaptureListeners[chEvID]);
		v_elems::const_iterator vi;
		for(vi=pv->begin(); vi != pv->end(); vi++ ){
			mlRMMLElement* pMLEl=*vi;
			pMLEl->EventNotify(chEvID,(mlRMMLElement*)GOID_Voice);
		}
	}
}

bool mlSceneManager::removeVideoCaptureListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	if(aidEvent==-1){
		// и удалим его из списка слушателей всех событий
		for(int ii=0; ii<ML_VIDEOCAPTURE_EVENTS_COUNT; ii++){
			v_elems* pv=&maVideoCaptureListeners[ii];
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
	v_elems* pv=&(maVideoCaptureListeners[aidEvent-1]);
	v_elems::iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl){
			pv->erase(vi);
			return true;
		}
	}
	return true;
}

void mlSceneManager::OnVideoCaptureErrorInternal(mlSynchData& aData)
{
	unsigned int errorCode = 0;
	aData >> errorCode;

	jsval v = INT_TO_JSVAL( errorCode);
	JS_SetProperty(cx, GPSM(cx)->GetEventGO(), "errorCode", &v);
	
	NotifyVideoCaptureListeners(EVID_onCaptureError);

	JS_DeleteProperty(cx, GPSM(cx)->GetEventGO(), "errorCode");
}

void mlSceneManager::NotifyVideoCaptureError()
{
	NotifyVoiceListeners(EVID_onCaptureError);
}

void mlSceneManager::ResetVideoCapture(){
	for(int ii=0; ii<ML_VIDEOCAPTURE_EVENTS_COUNT; ii++){
		maVideoCaptureListeners[ii].clear();
	}
}

JSBool JSVideoCaptureGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{ 
	return JS_TRUE; 
}

JSBool JSVideoCaptureSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{ 
	return JS_TRUE; 
}

JSBool JSFUNC_start( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlSceneManager* pSM = GPSM(cx);

	if ( argc!=1)
	{
		JS_ReportError(cx, "Player.videoCapture.start must get one int argument");
		return JS_TRUE;
	}

	jsdouble eventID;

	if (JS_ValueToNumber(cx, argv[0], &eventID) == JS_FALSE)
	{ 
		JS_ReportError(cx, "Player.videoCapture.start must get one int argument");
		return JS_TRUE;
	}

	if( pSM->GetContext() && pSM->GetContext()->mpRM)
	{
		bool res = pSM->GetContext()->mpRM->GetVideoCaptureSupport()->StartCapture((unsigned int)eventID);
		*rval = BOOLEAN_TO_JSVAL(res);
	}

	return JS_TRUE;
}

JSBool JSFUNC_setVideoSize( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlSceneManager* pSM = GPSM(cx);

	if ( argc!=2)
	{
		JS_ReportError(cx, "Player.videoCapture.setVideoSize must get 2 int arguments");
		return JS_TRUE;
	}

	jsdouble width, height;

	if (JS_ValueToNumber(cx, argv[0], &width) == JS_FALSE)
	{ 
		JS_ReportError(cx, "Player.videoCapture.setVideoSize must get 2 int arguments");
		return JS_TRUE;
	}

	if (JS_ValueToNumber(cx, argv[1], &height) == JS_FALSE)
	{ 
		JS_ReportError(cx, "Player.videoCapture.setVideoSize must get 2 int arguments");
		return JS_TRUE;
	}

	if( pSM->GetContext() && pSM->GetContext()->mpRM)
		pSM->GetContext()->mpRM->GetVideoCaptureSupport()->SetVideoSize((int)width, (int)height);

	return JS_TRUE;
}

JSBool JSFUNC_end( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlSceneManager* pSM = GPSM(cx);

	if( pSM->GetContext() && pSM->GetContext()->mpRM)
		pSM->GetContext()->mpRM->GetVideoCaptureSupport()->EndCapture();

	return JS_TRUE;
}

JSBool JSFUNC_getVideoURL( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlSceneManager* pSM = GPSM(cx);

	std::string url;

	if( pSM->GetContext() && pSM->GetContext()->mpRM)
		url = pSM->GetContext()->mpRM->GetVideoCaptureSupport()->GetVideoURL();

	*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, cLPWCSTR(url.c_str())));

	return JS_TRUE;
}

JSBool JSFUNC_canVideoBeStarted( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlSceneManager* pSM = GPSM(cx);

	bool canBeStarted = false;

	if( pSM->GetContext() && pSM->GetContext()->mpRM)
		canBeStarted = pSM->GetContext()->mpRM->GetVideoCaptureSupport()->CanVideoBeStarted();

	*rval = BOOLEAN_TO_JSVAL(canBeStarted);

	return JS_TRUE;
}

void mlSceneManager::OnVideoCaptureError(const unsigned int errorCode)
{
	mlSynchData oData;
	oData << errorCode;	
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onVideoCaptureError, (unsigned char*)oData.data(), oData.size());
}

}


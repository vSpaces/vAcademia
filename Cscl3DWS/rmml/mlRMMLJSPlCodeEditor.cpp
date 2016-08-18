#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"
#include "mlRMMLJSPlCodeEditor.h"
#include "ILogWriter.h"

namespace rmml {

/**
 * Реализация глобального JS-объекта "CodeEditor"
 */
JSBool JSCodeEditorGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool JSCodeEditorSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	
JSClass JSRMMLCodeEditorClass = {
	"_CodeEditor", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, 
	JSCodeEditorGetProperty, JSCodeEditorSetProperty,
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL,
	NULL, NULL, NULL
};

enum {
	TIDE_mlRMMLCodeEditor
};

enum {
	EVID_onCodeEditorClosed = 0,
	TEVIDE_mlRMMLCodeEditor
};

JSPropertySpec _JSCodeEditorPropertySpec[] = {
	{ 0, 0, 0, 0, 0 }
};

// Function declare
JSFUNC_DECL(start)
JSFUNC_DECL(close)

///// JavaScript Function Table
JSFunctionSpec _JSCodeEditorFunctionSpec[] = {
	JSFUNC(start,1)
	JSFUNC(close,0)
	{ 0, 0, 0, 0, 0 }
};

void CreateCodeEditorJSObject(JSContext *cx, JSObject *ajsoPlayer){
    JSObject* jsoCodeEditor = JS_DefineObject(cx, ajsoPlayer, GJSONM_CODEEDITOR, &JSRMMLCodeEditorClass, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
	JSBool bR;
	bR=JS_DefineProperties(cx, jsoCodeEditor, _JSCodeEditorPropertySpec);
	bR=JS_DefineFunctions(cx, jsoCodeEditor, _JSCodeEditorFunctionSpec);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, ajsoPlayer);
	JS_SetPrivate(cx,jsoCodeEditor,pSM);
	pSM->mjsoCodeEditor = jsoCodeEditor;
	//
}

char GetCodeEditorEventID(const wchar_t* apwcEventName){
	if(isEqual(apwcEventName,L"onCodeEditorClosed"))
		return (char)EVID_onCodeEditorClosed;
	return -1;
}

wchar_t* GetCodeEditorEventName(char aidEvent){
	switch(aidEvent){
	case (char)EVID_onCodeEditorClosed:
		return L"onCodeEditorClosed";
	}
	return L"???";
}

bool mlSceneManager::addCodeEditorListener(char aidEvent, mlRMMLElement* apMLEl){
	if(apMLEl == NULL) return false;
	char chEvID=aidEvent;
	if(chEvID >= ML_CODEEDITOR_EVENTS_COUNT) return false;
	v_elems* pv=&(maCodeEditorListeners[chEvID]);
	v_elems::const_iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl) return true; 
	}
	pv->push_back(apMLEl);
	return true;
}

void mlSceneManager::NotifyCodeEditorListeners(char chEvID){
	if(chEvID < ML_CODEEDITOR_EVENTS_COUNT){ 
		v_elems* pv=&(maCodeEditorListeners[chEvID]);
		v_elems::const_iterator vi;
		for(vi=pv->begin(); vi != pv->end(); vi++ ){
			mlRMMLElement* pMLEl=*vi;
			pMLEl->EventNotify(chEvID,(mlRMMLElement*)GOID_Voice);
		}
	}
}

bool mlSceneManager::removeCodeEditorListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	if(aidEvent==-1){
		// и удалим его из списка слушателей всех событий
		for(int ii=0; ii<ML_CODEEDITOR_EVENTS_COUNT; ii++){
			v_elems* pv=&maCodeEditorListeners[ii];
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
	v_elems* pv=&(maCodeEditorListeners[aidEvent-1]);
	v_elems::iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl){
			pv->erase(vi);
			return true;
		}
	}
	return true;
}

void mlSceneManager::OnCodeEditorClosedInternal(mlSynchData& aData)
{
	wchar_t* str;
	if (aData.readSafely(str))
	{
		jsval v = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, str));
		JS_SetProperty(cx, GPSM(cx)->GetEventGO(), "programText", &v);
	
		NotifyCodeEditorListeners(EVID_onCodeEditorClosed);

		JS_DeleteProperty(cx, GPSM(cx)->GetEventGO(), "programText");
	}
}

void mlSceneManager::NotifyCodeEditorClosed()
{
	NotifyCodeEditorListeners(EVID_onCodeEditorClosed);
}

void mlSceneManager::ResetCodeEditor(){
	for(int ii=0; ii<ML_CODEEDITOR_EVENTS_COUNT; ii++){
		maCodeEditorListeners[ii].clear();
	}
}

JSBool JSCodeEditorGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{ 
	return JS_TRUE; 
}

JSBool JSCodeEditorSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{ 
	return JS_TRUE; 
}

JSBool JSFUNC_start( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlSceneManager* pSM = GPSM(cx);

	if ( argc!=1)
	{
		JS_ReportError(cx, "Player.codeEditor.start must get one string argument");
		return JS_TRUE;
	}

	mlString sText;	
	if(!JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx, "Player.codeEditor.start must get one string argument");
		return JS_TRUE;
	}

	sText = (const wchar_t*)JS_GetStringChars(JSVAL_TO_STRING(argv[0]));

	if (pSM->GetContext() && pSM->GetContext()->mpRM)
	{
		pSM->GetContext()->mpRM->GetCodeEditorSupport()->Start(sText);
	}

	return JS_TRUE;
}

JSBool JSFUNC_close( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlSceneManager* pSM = GPSM(cx);

	if (pSM->GetContext() && pSM->GetContext()->mpRM)
	{
		pSM->GetContext()->mpRM->GetCodeEditorSupport()->Close();
	}

	return JS_TRUE;
}

void mlSceneManager::OnCodeEditorClose(const wstring& text)
{
	mlSynchData oData;
	oData << text.c_str();	
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onCodeEditorClosed, (unsigned char*)oData.data(), oData.size());
}

}


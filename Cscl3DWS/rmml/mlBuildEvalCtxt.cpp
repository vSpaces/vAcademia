
#include "mlRMML.h"
#include "mlBuildEvalCtxt.h"
#include "mlErrors.h"

namespace rmml{

static JSBool reportWarnings = JS_TRUE;

static void mlEvalErrorReporter(JSContext *cx, const char *message, JSErrorReport *report){
	JSString* sError = mlFormatJSError(cx, message, report, reportWarnings);
	if(sError == (JSString*)1){
		mlTraceError(cx, "eval error:\'%s\'\n",message);
	}else{
		mlTraceError(cx, "eval error:\'%s\'\n",JS_GetStringBytes(sError));
	}
}

static JSBool
ev_global_enumerate(JSContext *cx, JSObject *obj)
{
    return JS_EnumerateStandardClasses(cx, obj);
}

static JSBool
ev_global_resolve(JSContext *cx, JSObject *obj, jsval id, uintN flags,
               JSObject **objp)
{
    if ((flags & JSRESOLVE_ASSIGNING) == 0) {
        JSBool resolved;

        if (!JS_ResolveStandardClass(cx, obj, id, &resolved))
            return JS_FALSE;
        if (resolved) {
            *objp = obj;
            return JS_TRUE;
        }
    }

    return JS_TRUE;
}

JSClass ev_global_class = {
    "global", JSCLASS_NEW_RESOLVE | JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,  JS_PropertyStub,
    JS_PropertyStub,  JS_PropertyStub,
    ev_global_enumerate, (JSResolveOp) ev_global_resolve,
    JS_ConvertStub,   JS_FinalizeStub
};

JSBool mlBuildEvalCtxt::evGetFullPath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	JSString* jssResPath = NULL;
	jschar* jscResPath = NULL;
	if(argc == 0 || !JSVAL_IS_STRING(argv[0])){
	}else{
		JSString* jssResPath = JSVAL_TO_STRING(argv[0]);
		jscResPath = JS_GetStringChars(jssResPath);
		// если начинаяется с ':', то уже полный путь
		if(*jscResPath == L':'){
			if(jssResPath == NULL)
				*rval = JSVAL_NULL;
			else
				*rval = STRING_TO_JSVAL(jssResPath);
			return JS_TRUE;
		}
	}
	//
	mlBuildEvalCtxt* pmBuildEvalCtxt = (mlBuildEvalCtxt*)JS_GetPrivate(cx, JS_GetGlobalObject(cx));
	std::wstring sFullResPath = (const wchar_t*)jscResPath;
	if(pmBuildEvalCtxt->mpRefineResPathFunc != NULL){
		sFullResPath = pmBuildEvalCtxt->mpRefineResPathFunc(pmBuildEvalCtxt->mwsXMLFullPath, sFullResPath.c_str());
	}
	*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, sFullResPath.c_str()));
	return JS_TRUE;
}

JSFunctionSpec ev_functions[] = {
	{"getFullPath", mlBuildEvalCtxt::evGetFullPath, 1},
	{0,0,0}
};

mlBuildEvalCtxt::mlBuildEvalCtxt(JSRuntime* rt):
	MP_WSTRING_INIT(mwsRes),
	MP_WSTRING_INIT(mwsXMLFullPath)
{
	cx = NULL;
	glob = NULL;

//	rt = JS_NewRuntime(8L * 1024L * 1024L);
//	if (rt == NULL) return;

	// establish a context
	cx = JS_NewContext(rt, 8192);
	if (cx == NULL)	return;

	JS_SetErrorReporter(cx, mlEvalErrorReporter);

	GUARD_JS_CALLS(cx)

	// initialize the built-in JS objects and the global object
	glob = JS_NewObject(cx, &ev_global_class, NULL, NULL);
	if (glob == NULL) return;
//	JS_InitStandardClasses(cx, glob);
	JS_DefineFunctions(cx, glob, ev_functions);

//	jsval v;
//	JSBool bR=JS_EvaluateUCScript(cx, glob, L"0", 1, "initizlize", 1, &v);
	JS_SetPrivate(cx, glob, this);
}

void mlBuildEvalCtxt::DestroyCtxt(){
	if(cx != NULL){
//		JS_DestroyContext(cx);
//		cx = NULL;
//		JS_DestroyRuntime(rt);
//		rt = NULL;
	}
	mpRefineResPathFunc = NULL;
}

mlBuildEvalCtxt::~mlBuildEvalCtxt(){
	DestroyCtxt();
}

void mlBuildEvalCtxt::DefineConstant(const wchar_t* apwcName, const wchar_t* apwcValue){
	GUARD_JS_CALLS(cx);
	if(apwcName == NULL || *apwcName == L'\0')
		return;
	if(apwcValue == NULL || *apwcValue == L'\0'){
		jsval v = JSVAL_NULL;
		wr_JS_SetUCProperty(cx, glob, apwcName, -1, &v);
		return;
	}
	jsval v = JSVAL_NULL;
	JSBool bR=wr_JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), apwcValue, wcslen(apwcValue), "define", 1, &v);
	if(!bR){
		wr_JS_SetUCProperty(cx, glob, apwcName, -1, &v);
		return;
	}
	wr_JS_DefineUCProperty(cx, glob, apwcName, -1, v, NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
}

const wchar_t* mlBuildEvalCtxt::Eval(const wchar_t* apwcExpr){
	GUARD_JS_CALLS(cx);
	mwsRes.erase();
	if(apwcExpr == NULL)
		return mwsRes.c_str();
	jsval v;
	JSBool bR=wr_JS_EvaluateUCScript(cx, glob, apwcExpr, wcslen(apwcExpr), "eval", 1, &v);
	if(!bR)
		return mwsRes.c_str();
	JSString* jssRes = JS_ValueToString(cx, v);
	if(jssRes == NULL)
		return mwsRes.c_str();
	mwsRes = wr_JS_GetStringChars(jssRes);
	return mwsRes.c_str();
}

jsval mlBuildEvalCtxt::Eval(JSContext* aDestCtx, const wchar_t* apwcExpr){
	GUARD_JS_CALLS(cx);
	jsval vRes = JSVAL_VOID;
	JSBool bR=wr_JS_EvaluateUCScript(cx, glob, apwcExpr, wcslen(apwcExpr), "eval", 1, &vRes);
	if(!bR) 
		return JSVAL_VOID;
	if(JSVAL_IS_DOUBLE(vRes)){
		double dVal = *JSVAL_TO_DOUBLE(vRes);
		mlJS_NewDoubleValue(aDestCtx, dVal, &vRes);
	}else if(JSVAL_IS_STRING(vRes)){
		JSString* jss = JSVAL_TO_STRING(vRes);
		jss = JS_NewUCStringCopyZ(aDestCtx, JS_GetStringChars(jss));
		vRes = STRING_TO_JSVAL(jss);
	}else if(JSVAL_IS_OBJECT(vRes)){
		vRes = JSVAL_VOID;
	}
	return vRes;
}

void mlBuildEvalCtxt::SetXMLFullPath(const wchar_t* apwcXMLFullPath){
	mwsXMLFullPath = apwcXMLFullPath;
}

void mlBuildEvalCtxt::Clear(){
	GUARD_JS_CALLS(cx);

	JSObject* obj = JS_GetGlobalObject(cx);
	JSIdArray* pPropIDArr = JS_Enumerate(cx, obj);
	for(int i = 0; i < pPropIDArr->length; i++){
		jsval v;
		if(!JS_IdToValue(cx, pPropIDArr->vector[i], &v)) continue;
		if(JSVAL_IS_STRING(v)){
			jschar* jsc = JS_GetStringChars(JSVAL_TO_STRING(v));
			int len = wcslen((const wchar_t*)jsc);

			char* pcPropName = MP_NEW_ARR( char, len+2);
			size_t numConverted = 0;
			wcstombs_s(&numConverted, pcPropName, len+2, (const wchar_t*)jsc, len);
			pcPropName[len] = '\0';
			JS_DeleteProperty(cx, obj, pcPropName);
			MP_DELETE_ARR( pcPropName);
		}else if(JSVAL_IS_INT(v)){
			int iId = JSVAL_TO_INT(v);
			JS_DeleteElement(cx, obj, iId);
		}else continue;
	}
	JS_DestroyIdArray(cx, pPropIDArr);
}

void mlBuildEvalCtxt::CopyAllConstantsTo(JSContext* aCtx){
	GUARD_JS_CALLS(cx);
	JSIdArray* pPropIDArr = JS_Enumerate(cx, glob);
	for(int i = 0; i < pPropIDArr->length; i++){
		jsval v;
		if(!JS_IdToValue(cx, pPropIDArr->vector[i], &v)) continue;
		if(!JSVAL_IS_STRING(v)) continue;
		jschar* jsc = JS_GetStringChars(JSVAL_TO_STRING(v));
		jsval vProp = JSVAL_VOID;
		JS_GetUCProperty(cx,glob,jsc,-1,&vProp);
		if(JSVAL_IS_DOUBLE(vProp)){
			double dVal = *JSVAL_TO_DOUBLE(vProp);
			mlJS_NewDoubleValue(aCtx, dVal, &vProp);
		}else if(JSVAL_IS_STRING(vProp)){
			JSString* jssVal = JSVAL_TO_STRING(vProp);
			JS_NewUCStringCopyZ(aCtx, JS_GetStringChars(jssVal));
		}else if(JSVAL_IS_OBJECT(vProp)){
			vProp = JSVAL_VOID;
		}
		if(vProp != JSVAL_VOID)
			JS_SetUCProperty(aCtx, JS_GetGlobalObject(aCtx), jsc, -1, &vProp);
	}
	JS_DestroyIdArray(cx, pPropIDArr);
}

}

#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml {

mlRMMLLoadable::mlRMMLLoadable(void):
	MP_WSTRING_INIT(msSrcFull)
{
	src = NULL;
	miSrcLoaded = LST_SRC_NOT_SET;
	miLoadedBytes = 0;
	miTotalBytes = 1;
	mbStreaming = false;
	mLoadingPriority = ELP_UI;
}

mlRMMLLoadable::~mlRMMLLoadable(void)
{
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLLoadable::_JSPropertySpec[] = {
	JSPROP_RW(src)	
	//JSPROP_RO(bytesLoaded)
	//JSPROP_RO(bytesTotal)
	JSPROP_RW(streaming)

	JSPROP_EV(onLoad)
	JSPROP_EV(onLoading)
	JSPROP_EV(onLoadAll)
	JSPROP_EV(onUnload)
	JSPROP_EV(onLoadError)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLLoadable::_JSFunctionSpec[] = {
	JSFUNC(getBytesLoaded, 0)
	JSFUNC(getBytesTotal, 0)
	JSFUNC(getLoadingPriority, 0)
	JSFUNC(setLoadingPriority, 1)
	JSFUNC(reload, 0)
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlRMMLLoadable::_EventSpec[] = {
	MLEVENT(onLoad)
	MLEVENT(onLoading)
	MLEVENT(onLoadAll)
	MLEVENT(onUnload)
	MLEVENT(onLoadError)
	{0, 0, 0}
};

ADDPROPFUNCSPECS_IMPL(mlRMMLLoadable)

///// JavaScript Get Property Wrapper
JSBool mlRMMLLoadable::GetJSProperty(int id, jsval *vp) {
	switch(id) {
		case JSPROP_src:
			if(src==NULL) *vp=JSVAL_NULL;
			else *vp=STRING_TO_JSVAL(src);
			break;
//		case JSPROP_bytesLoaded:
//			*vp = INT_TO_JSVAL(miLoadedBytes);
//			break;
//		case JSPROP_bytesTotal:
//			*vp = INT_TO_JSVAL(miTotalBytes);
//			break;
		case JSPROP_streaming:
			*vp = BOOLEAN_TO_JSVAL(mbStreaming);
			break;			
		case EVID_onLoad:
		case EVID_onLoading:
		case EVID_onLoadAll:
		case EVID_onUnload:
		case EVID_onLoadError:
			{
			mlRMMLElement* pMLEl = GetElem_mlRMMLLoadable();
			int iRes = GPSM(pMLEl->mcx)->CheckEvent(id, pMLEl, vp);
			if(iRes >= 0)
				*vp = BOOLEAN_TO_JSVAL(iRes);
			}break;
	}
	return JS_TRUE;
}

JSBool mlRMMLLoadable::SetJSProperty(JSContext* cx, int id, jsval *vp) {
	switch(id) {
		case JSPROP_src:
			if( JSVAL_IS_STRING(*vp))
			{
				src=JSVAL_TO_STRING(*vp);				
				mlRMMLElement* pMLElThis = GetElem_mlRMMLLoadable();
				if(pMLElThis->miSrcFilesIdx < 0){
					//	wchar_t* pwcSrc=JS_GetStringChars(src);
if(pMLElThis->mRMMLType == MLMT_AUDIO)
int hh=0;
					JSScript* script = NULL;
					JSStackFrame* fp = NULL;
					fp = JS_FrameIterator(cx, &fp);
					for(;fp != NULL; fp = JS_FrameIterator(cx, &fp)){
						if(JS_GetFrameFunction(cx, fp) != NULL){
							JSFunction* fun = JS_GetFrameFunction(cx, fp);
					//JSString* jssFun = JS_DecompileFunction(cx, fun, 0);
					//jschar* jscFun = JS_GetStringChars(jssFun);
							script = JS_GetFunctionScript(cx, fun);
							break;
						}
						if(JS_GetFrameScript(cx, fp) != NULL){
							script = JS_GetFrameScript(cx, fp);
							break;
						}
					}
					if(script){
						const char* pszScriptFilename = JS_GetScriptFilename(cx, script);
						pMLElThis->SetSrcFileIdx(GPSM(cx)->mSrcFileColl.Find(cLPWCSTR(pszScriptFilename)), pMLElThis->miSrcLine);
					}
				}
				SrcChanged();
			}
			break;
		case JSPROP_streaming:
			ML_JSVAL_TO_BOOL( mbStreaming, *vp);
			break;		
//			*vp = __float_TO_JSVal(cx,priv->getX());
	}
	return JS_TRUE;
}

void mlRMMLLoadable::Set_Event_state(mlRMMLElement* apMLEl, int aiState, wchar_t *aUserErrorString, wchar_t *aWarningText){
	jsval v = INT_TO_JSVAL(aiState);
	wr_JS_SetUCProperty(apMLEl->mcx, GPSM(apMLEl->mcx)->GetEventGO(), L"state", -1, &v);
	if (aUserErrorString)
	{
		jsval v3 = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(apMLEl->mcx, aUserErrorString));
		wr_JS_SetUCProperty(apMLEl->mcx, GPSM(apMLEl->mcx)->GetEventGO(), L"userErrorString", -1, &v3);
	}
	if(aWarningText != NULL){
		v = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(apMLEl->mcx, aWarningText));
		wr_JS_SetUCProperty(apMLEl->mcx, GPSM(apMLEl->mcx)->GetEventGO(), L"warningText", -1, &v);
	}else{
		JS_DeleteProperty(apMLEl->mcx, GPSM(apMLEl->mcx)->GetEventGO(), "warningText");
	}
}

void mlRMMLLoadable::Set_Event_errorCode(mlRMMLElement* apMLEl, int aiErrorCode, int aiUserErrorCode, wchar_t *aUserErrorString){
	jsval v = INT_TO_JSVAL(aiErrorCode);
	wr_JS_SetUCProperty(apMLEl->mcx, GPSM(apMLEl->mcx)->GetEventGO(), L"errorCode", -1, &v);

	jsval v2 = INT_TO_JSVAL(aiUserErrorCode);
	wr_JS_SetUCProperty(apMLEl->mcx, GPSM(apMLEl->mcx)->GetEventGO(), L"userErrorCode", -1, &v2);
	if (aUserErrorString != NULL)
	{
		jsval v3 = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(apMLEl->mcx, aUserErrorString));
		wr_JS_SetUCProperty(apMLEl->mcx, GPSM(apMLEl->mcx)->GetEventGO(), L"userErrorString", -1, &v3);
	}
}

bool mlRMMLLoadable::Lod_SetEventData(char aidEvent, mlSynchData &Data){
//if(GetElem_mlRMMLLoadable()->_name != NULL && isEqual(GetElem_mlRMMLLoadable()->mpName,L"test123"))
//miLoadedBytes = 2;
	switch(aidEvent){
	case EVID_onLoad:{
		int iState = 0;
		wchar_t *userErrorString = NULL;
		if(!Data.inTheEnd())
			Data >> iState;
		wchar_t *warningText = NULL;
		//if(!Data.inTheEnd())
		//	Data >> warningText;
		Set_Event_state(GetElem_mlRMMLLoadable(), iState, userErrorString, warningText);
		}return true;
	case EVID_onLoadError:{
		unsigned int iErrorCode = 0;
		unsigned int iUserErrorCode = 0;
		wchar_t *userErrorString = NULL;
		Data >> iErrorCode;
		Data >> iUserErrorCode;
		Data >> userErrorString;
		Set_Event_errorCode(GetElem_mlRMMLLoadable(), iErrorCode, iUserErrorCode, userErrorString);
		}return true;
	}
	return true;
}

JSBool mlRMMLLoadable::JSFUNC_getBytesLoaded(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMMLLoadable* pLoadable = pMLEl->GetLoadable();
	*rval = INT_TO_JSVAL(pLoadable->miLoadedBytes);
	return JS_TRUE;
}

JSBool mlRMMLLoadable::JSFUNC_getBytesTotal(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMMLLoadable* pLoadable = pMLEl->GetLoadable();
	*rval = INT_TO_JSVAL(pLoadable->miTotalBytes);
	return JS_TRUE;
}

JSBool mlRMMLLoadable::JSFUNC_getLoadingPriority(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMMLLoadable* pLoadable = pMLEl->GetLoadable();
	*rval = INT_TO_JSVAL( (int)pLoadable->mLoadingPriority);
	return JS_TRUE;
}

JSBool mlRMMLLoadable::JSFUNC_setLoadingPriority(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc != 1){
		JS_ReportWarning(cx,"setLoadingPriority should take only one int argument.\nExpected format: setLoadingPriority(priority=[0,3])");
		*rval=JSVAL_FALSE;
		return JS_TRUE;
	}

	if(!JSVAL_IS_INT(argv[0])){
		JS_ReportError(cx,"setLoadingPriority-method parameter must be an integer");
		return JS_TRUE;
	}

    int iPriority = JSVAL_TO_INT(argv[0]);
	if( iPriority < ELP_UI || iPriority >= ELP_Last){
		JS_ReportError(cx,"setLoadingPriority-method parameter should be in range [0-3]");
		return JS_TRUE;
	}

	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMMLLoadable* pLoadable = pMLEl->GetLoadable();
	pLoadable->LoadingPriorityChanged( (ELoadingPriority)iPriority);

	return JS_TRUE;
}

JSBool mlRMMLLoadable::JSFUNC_reload(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMMLLoadable* pLoadable = pMLEl->GetLoadable();
	pLoadable->Reload();
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

/**
 * Реализация mlILoadable
 */

const wchar_t* mlRMMLLoadable::GetRawSrc() const {
	if(src==NULL) return NULL;
	return wr_JS_GetStringChars(src);
}

const wchar_t* mlRMMLLoadable::GetSrc(){
	if(src==NULL) return NULL;
	wchar_t* pwcSrc=wr_JS_GetStringChars(src);
	if(*pwcSrc == L'\0') return NULL;
	if(*(pwcSrc+1) == L'\0' && (*pwcSrc == L'\\' || *pwcSrc == L'/')) return NULL;
	mlRMMLElement* pMLEl = GetElem_mlRMMLLoadable();
	msSrcFull = GPSM(pMLEl->mcx)->RefineResPathEl(pMLEl, pwcSrc);
	return msSrcFull.c_str();
}

ELoadingPriority mlRMMLLoadable::GetLoadingPriority() const
{
	return mLoadingPriority;
}

bool mlRMMLLoadable::IsStreaming(){
	return mbStreaming;
}

void mlRMMLLoadable::SetLoaded(){
	if (miSrcLoaded != LST_LOADING)
		miSrcLoaded  = LST_LOADED;
}

bool mlRMMLLoadable::IsLoading()
{
	if ( miSrcLoaded == LST_SRC_NOT_SET || miSrcLoaded == LST_LOADED)
		return false;

	return true;
}

//MLEVENT_IMPL(mlRMMLLoadable,onLoad);
void mlRMMLLoadable::onLoad(int aiState){ 
	// если в моем update-е, 
//	if(gpSM->Updating()){
//		// то выполняем сразу же
//		GetElem_mlRMMLLoadable()->CallListeners(EVID_onLoad);
//	}else{

		mlRMMLElement* pMLEl = GetElem_mlRMMLLoadable();

		if (aiState == RMML_LOAD_STATE_SRC_LOADED || aiState == RMML_LOAD_STATE_ALL_RESOURCES_LOADED)
			miSrcLoaded = LST_RES_LOADED;
		else if (aiState == RMML_LOAD_STATE_SRC_LOADING)
			miSrcLoaded = LST_LOADING;
		else
			miSrcLoaded = LST_LOAD_ERROR;

		miLoadedBytes = miTotalBytes;
		
		//// иначе ставим в очередь событий
		// сохраняем внутренний путь до элемента 
		mlSynchData Data;
		GPSM(pMLEl->mcx)->SaveInternalPath(Data, pMLEl);
		Data << aiState;
		// ставим в очередь событий
		unsigned char ucEventCode = EVID_onLoad;
		if(aiState == RMML_LOAD_STATE_LOADERROR){		
			ucEventCode = EVID_onLoadError;
			Data << 0;
		}else if(aiState == RMML_LOAD_STATE_SRC_LOADING){
			ucEventCode = EVID_onLoading;		
		}else if(aiState == RMML_LOAD_STATE_ALL_RESOURCES_LOADED){
			ucEventCode = EVID_onLoadAll;
		}
		GPSM(pMLEl->mcx)->GetContext()->mpInput->AddInternalEvent(ucEventCode, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(pMLEl->mcx)->GetMode() == smmInitilization);

		GPSM(pMLEl->mcx)->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onLoadStateChanged,(unsigned char*)Data.data(), Data.size());

//	}
}

void mlRMMLLoadable::onLoading(int amiLoadedBytes, int amiTotalBytes)
{
	miLoadedBytes = amiLoadedBytes;
	miTotalBytes = amiTotalBytes;
}

//MLEVENT_IMPL(mlRMMLLoadable,onUnload);
void mlRMMLLoadable::onUnload(){
	mlRMMLElement* pMLEl = GetElem_mlRMMLLoadable();
	GPSM(pMLEl->mcx)->mbDontSendSynchEvents = false;
	pMLEl->CallListeners(EVID_onUnload);
}

moILoadable* mlRMMLLoadable::GetmoILoadable(){
	moMedia* pMO=GetElem_mlRMMLLoadable()->GetMO();
	if(PMO_IS_NULL(pMO)) return NULL;
	return pMO->GetILoadable();
}

#define MOILOAD_EVENT_CALL(E) \
	moILoadable* pmoILoadable=GetmoILoadable(); \
	if(pmoILoadable) return pmoILoadable->##E(); \
	return false;

bool mlRMMLLoadable::SrcChanged(){ 
	// если какой-нибудь ресурс уже был загружен
	if(miSrcLoaded != LST_SRC_NOT_SET){
		// то уведомить о том, что он выгружается
		mlRMMLElement* pMLEl = GetElem_mlRMMLLoadable();
		pMLEl->CallListeners(EVID_onUnload);
	}

	if(GetSrc() == NULL)
	{
		miSrcLoaded = LST_SRC_NOT_SET;
	}
	else
	{
		miSrcLoaded = LST_NOT_LOADED;
	}

	GetElem_mlRMMLLoadable()->VisualRepresentationChanged();
/*
unsigned long ulBeforeScrChangedTime = 0;
mlString sSrc = GetSrc()!=NULL?GetSrc():L"";
if(GetElem_mlRMMLLoadable()->mRMMLType == MLMT_OBJECT && sSrc.find(L'#') != mlString::npos)
ulBeforeScrChangedTime = GPSM(GetElem_mlRMMLLoadable()->mcx)->GetContext()->mpApp->GetTimeStamp();
	//MOILOAD_EVENT_CALL(SrcChanged); 
	bool bRes = false;
	moILoadable* pmoILoadable=GetmoILoadable();
	if(pmoILoadable) bRes = pmoILoadable->SrcChanged();
if(ulBeforeScrChangedTime > 0){
mlTrace(GetElem_mlRMMLLoadable()->mcx, "SrcChanged (%d) src=%s target=%s \n", 
	GPSM(GetElem_mlRMMLLoadable()->mcx)->GetContext()->mpApp->GetTimeStamp() - ulBeforeScrChangedTime, 
	cLPCSTR(GetSrc()!=NULL?GetSrc():L""), 
	cLPCSTR(GetElem_mlRMMLLoadable()->GetFullPath().c_str())
);
}
	return bRes;
	*/
	MOILOAD_EVENT_CALL(SrcChanged); 
}
bool mlRMMLLoadable::Reload() 
{
	MOILOAD_EVENT_CALL(Reload);
}

void mlRMMLLoadable::LoadingPriorityChanged( ELoadingPriority aPriority)
{
	mLoadingPriority = aPriority;

	moILoadable* pmoILoadable=GetmoILoadable();
	if(pmoILoadable)
		pmoILoadable->LoadingPriorityChanged( mLoadingPriority);
}

}

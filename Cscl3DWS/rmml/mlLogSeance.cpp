
#include "mlRMML.h"
#include "mlLogSeance.h"
#include <time.h>

namespace rmml {

mlLogSeanceJSO::mlLogSeanceJSO():
	MP_WSTRING_INIT(msName),
	MP_WSTRING_INIT(msAuthor),
	MP_WSTRING_INIT(msMembers),
	MP_WSTRING_INIT(msDate),
	MP_WSTRING_INIT(msURL)
{
	mode = 0;
	errorCode = 0;
	mbPlaying = false;
}

///// JavaScript Variable Table
JSPropertySpec mlLogSeanceJSO::_JSPropertySpec[] = {
	JSPROP_RW(name)
	JSPROP_RW(members)
	JSPROP_RO(author)
	JSPROP_RO(mode)
	JSPROP_RO(URL)
	JSPROP_RW(date)
	JSPROP_RO(recordID)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlLogSeanceJSO::_JSFunctionSpec[] = {
	JSFUNC(queryFullInfo,0) // получить список сеансов воспроизведения для записи лога
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Seance,mlLogSeanceJSO,0)
	MLJSCLASS_ADDPROPFUNC
//	MLJSCLASS_ADDPROPENUMOP
//	MLJSCLASS_ADDALTERCONSTR(AlterJSConstructor)
MLJSCLASS_IMPL_END(mlLogSeanceJSO)


JSBool mlLogSeanceJSO::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN2(mlLogSeanceJSO);
		switch(iID){
		case JSPROP_name:
			if(JSVAL_IS_STRING(*vp)){
				priv->msName = (const wchar_t*)JS_GetStringChars(JSVAL_TO_STRING(*vp));
			}else{
				JS_ReportError(cx,"'name' property value must be a string");
				priv->msName = L"";
			}
			priv->NameChanged();
			break;
		case JSPROP_members:
			if(JSVAL_IS_STRING(*vp)){
				priv->msMembers = (const wchar_t*)JS_GetStringChars(JSVAL_TO_STRING(*vp));
			}else{
				JS_ReportError(cx,"'members' property value must be a string");
				priv->msMembers = L"";
			}
			priv->MembersChanged();
			break;
		case JSPROP_date:
			if(JSVAL_IS_STRING(*vp)){
				priv->msDate= (const wchar_t*)JS_GetStringChars(JSVAL_TO_STRING(*vp));
			}else{
				JS_ReportError(cx,"'date' property value must be a string");
				priv->msDate = L"";
			}
			priv->DateChanged();
			break;
		}
	SET_PROPS_END2;
	return JS_TRUE;
}

JSBool mlLogSeanceJSO::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN2(mlLogSeanceJSO);
		switch(iID){
		case JSPROP_name:
			if(priv->msName.empty()){
				*vp = JS_GetEmptyStringValue(cx);
			}else{
				*vp = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)priv->msName.c_str()));
			}
			break;
		case JSPROP_members:
			if(priv->msMembers.empty()){
				*vp = JS_GetEmptyStringValue(cx);
			}else{
				*vp = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)priv->msMembers.c_str()));
			}
			break;
		case JSPROP_date:
			if(priv->msDate.empty()){
				*vp = JS_GetEmptyStringValue(cx);
			}else{
				*vp = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)priv->msDate.c_str()));
			}
			break;
		case JSPROP_author:
			if(priv->msAuthor.empty()){
				*vp = JS_GetEmptyStringValue(cx);
			}else{
				*vp = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)priv->msAuthor.c_str()));
			}
			break;
		case JSPROP_URL:
			if(priv->msURL.empty()){
				*vp = JS_GetEmptyStringValue(cx);
			}else{
				*vp = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)priv->msURL.c_str()));
			}
			break;
		case JSPROP_mode:
			*vp = INT_TO_JSVAL(priv->mode);
			break;
		case JSPROP_recordID:
			*vp = INT_TO_JSVAL(priv->recordID);
			break;
		}
	GET_PROPS_END2;
	return JS_TRUE;
}


void mlLogSeanceJSO::NameChanged(){
	miFlags |= MLLSI_NAME;
}

void mlLogSeanceJSO::MembersChanged(){
	miFlags |= MLLSI_MEMBERS;
}

void mlLogSeanceJSO::DateChanged(){
	miFlags |= MLLSI_DATE;
}

JSBool mlLogSeanceJSO::JSFUNC_queryFullInfo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlLogSeanceJSO* pSeance = (mlLogSeanceJSO*)JS_GetPrivate(cx, obj);
	SAVE_FROM_GC(cx,GPSM(cx)->GetSRecorderGO(),obj);
	omsresult omsres = GPSM(cx)->GetContext()->mpSyncMan->QueryFullSeanceInfo((unsigned int)obj, pSeance->GetSeanceID());
	return JS_TRUE;
}

void mlLogSeanceJSO::Init(mlLogSeanceInfo& aLRI){
	miSeanceID  = aLRI.muSeanceID;
	miFlags = aLRI.miFlags;
	msName = L"";
	if(aLRI.miFlags & MLLSI_NAME) {	
		msName = aLRI.msName;
	}
	if(aLRI.miFlags & MLLSI_MEMBERS){
		msMembers = aLRI.msMembers;
	}
	if(aLRI.miFlags & MLLSI_AUTHOR) {
		msAuthor = aLRI.msAuthor;
	}
	if(aLRI.miFlags & MLLSI_URL) {
		msURL = aLRI.msURL;
	}
	
	mode = aLRI.mlMode;

	if(aLRI.miFlags & MLLSI_DATE) {
		msDate = aLRI.msDate;
	}

	if(aLRI.miFlags & MLLSI_RECORD_ID) {
		recordID = aLRI.mlRecordID;
	}
}

bool mlLogSeanceJSO::FillRecordInfo(syncSeanceInfo& aSeanceInfo){
	aSeanceInfo.muSeanceID = miSeanceID;

	aSeanceInfo.mpwcName.clear();
	if (!msName.empty())
		aSeanceInfo.mpwcName = msName.c_str();
	aSeanceInfo.mpwcMembers.clear();
	if (!msMembers.empty())
		aSeanceInfo.mpwcMembers = msMembers.c_str();
	aSeanceInfo.mpwcAuthor.clear(); // RO
	if(!msAuthor.empty())
		aSeanceInfo.mpwcAuthor = msAuthor.c_str();
	aSeanceInfo.mpwcURL.clear();
	if(!msURL.empty())
	{
		aSeanceInfo.mpwcURL = msURL.c_str();
	}
	aSeanceInfo.mlRecordID = recordID;

// Доделать 5.04.08
	aSeanceInfo.mlDate = 0;
/*	if(date != NULL)
		aSeanceInfo.mpwcDate = JS_GetStringChars(date);
*/
	return true;
}

void mlLogSeanceJSO::SetSeanceID(unsigned int auID)
{
	miSeanceID  = auID;
}

////////////////////////////////////////////
// mlLogRecordsJSO
////////////////////

mlLogSeancesJSO::mlLogSeancesJSO(void):
	MP_VECTOR_INIT(mvSeanceInfo)
{
	errorCode = 0;
	length = 0;
}

mlLogSeancesJSO::~mlLogSeancesJSO(void){
}

///// JavaScript Variable Table
JSPropertySpec mlLogSeancesJSO::_JSPropertySpec[] = {
//	JSPROP_RO(errorCode)
//	JSPROP_RO(length)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlLogSeancesJSO::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Seances,mlLogSeancesJSO,0)
	MLJSCLASS_ADDPROPFUNC
//	MLJSCLASS_ADDPROPENUMOP
//	MLJSCLASS_ADDALTERCONSTR(AlterJSConstructor)
MLJSCLASS_IMPL_END(mlLogSeancesJSO)

JSBool mlLogSeancesJSO::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN2(mlLogSeancesJSO);
//	switch(iID){
//	}
	SET_PROPS_END2;
	return JS_TRUE;
}

JSBool mlLogSeancesJSO::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	if (JSVAL_IS_STRING(id)) {
		mlLogSeancesJSO* priv=(mlLogSeancesJSO*)JS_GetPrivate(cx, obj);
		if (priv==NULL){ *vp = JSVAL_NULL; return JS_TRUE; }
		JSString* jssPropName = JSVAL_TO_STRING(id);
		jschar* jscPropName = JS_GetStringChars(jssPropName);
		if(isEqual((const wchar_t*)jscPropName, L"length")){
			*vp = INT_TO_JSVAL(priv->length);
		}else if(isEqual((const wchar_t*)jscPropName, L"errorCode")){
			*vp = INT_TO_JSVAL(priv->errorCode);
		}
		return JS_TRUE;
	}
	GET_PROPS_BEGIN2(mlLogSeancesJSO);
/*
	switch(iID){
		case JSPROP_errorCode:
			*vp = INT_TO_JSVAL(priv->errorCode);
			break;
		case JSPROP_length:
			*vp = INT_TO_JSVAL(priv->length);
			break;
	}
*/
	GET_PROPS_END2;
	return JS_TRUE;
}

mlLogSeanceInfo::mlLogSeanceInfo(syncSeanceInfo & aSeanceInfo):
	MP_WSTRING_INIT(msName),
	MP_WSTRING_INIT(msAuthor),	
	MP_WSTRING_INIT(msURL),
	MP_WSTRING_INIT(msMembers),
	MP_WSTRING_INIT(msDate)
{
	// флаги, определяющие какая информация действительна 
	miFlags = 0;
	// (пока будет без флагов. Если == NULL, значит не определена. Если == "", значит пустая строка)
	// идентификатор записи 
	muSeanceID  = aSeanceInfo.muSeanceID;
	// название записи
	if(!aSeanceInfo.mpwcName.empty()){
		msName = aSeanceInfo.mpwcName;
		miFlags |= MLLSI_NAME;
	}
	// описание записи
	if(!aSeanceInfo.mpwcMembers.empty()){
		msMembers = aSeanceInfo.mpwcMembers;
		miFlags |= MLLSI_MEMBERS;
	}
	// автор (RO)
	if(!aSeanceInfo.mpwcAuthor.empty()){
		msAuthor = aSeanceInfo.mpwcAuthor;
		miFlags |= MLLSI_AUTHOR;
	}
	// URL (RO)
	if(!aSeanceInfo.mpwcURL.empty()){
		msURL = aSeanceInfo.mpwcURL;
		miFlags |= MLLSI_AUTHOR;
	}
	// Режим (RO)
		mlMode = aSeanceInfo.mlMode;
		miFlags |= MLLSI_MODE;

		mlRecordID = aSeanceInfo.mlRecordID;
		miFlags |= MLLSI_RECORD_ID;
	// дата сеанса
	if(aSeanceInfo.mlDate != 0){
		
		struct tm *seanceTime = NULL;
		_gmtime64_s(seanceTime, (const __int64 *)&aSeanceInfo.mlDate);
		wchar_t buff[100]; 
		_wasctime_s(buff, 100, seanceTime  );
		msDate = buff;
		miFlags |= MLLSI_DATE;
	}
}

mlLogSeanceInfo::mlLogSeanceInfo():
	MP_WSTRING_INIT(msName),
	MP_WSTRING_INIT(msAuthor),	
	MP_WSTRING_INIT(msURL),
	MP_WSTRING_INIT(msMembers),
	MP_WSTRING_INIT(msDate)
{
}

mlLogSeanceInfo::mlLogSeanceInfo(const mlLogSeanceInfo& aSrc):
	MP_WSTRING_INIT(msName),
	MP_WSTRING_INIT(msAuthor),	
	MP_WSTRING_INIT(msURL),
	MP_WSTRING_INIT(msMembers),
	MP_WSTRING_INIT(msDate)
{
	// флаги, определяющие какая информация действительна 
	miFlags = aSrc.miFlags;
	// идентификатор записи 
	muSeanceID = aSrc.muSeanceID;
	// название записи
	msName = aSrc.msName;
	// осписок участников
	msMembers = aSrc.msMembers;
	// автор (RO)
	msAuthor = aSrc.msAuthor;
	// URL (RO)
	msURL = aSrc.msURL;
	// дата
	msDate = aSrc.msDate;
	mlDate = aSrc.mlDate;
	mlMode = aSrc.mlMode;
	mlRecordID = aSrc.mlRecordID;
}

void mlLogSeancesJSO::SaveSeanceInfo(syncSeanceInfo** appSeancesInfo){
	if(appSeancesInfo == NULL)
		return;
	while(*appSeancesInfo != NULL){
		mvSeanceInfo.push_back(mlLogSeanceInfo((syncSeanceInfo&)(**appSeancesInfo)));
		appSeancesInfo++;
	}
}

void mlLogSeancesJSO::SaveSeanceInfo(mlLogSeanceInfo seanceInfo){
	mvSeanceInfo.push_back(mlLogSeanceInfo(seanceInfo));
}

// сохраняем URL
void mlLogSeanceJSO::SetSeanceURL(const wchar_t* aURL)
{
	msURL = aURL;
//	URL = JS_NewUCStringCopyZ(mcx, aURL);
//	jsval v;
//	JS_GetProperty(mcx,mjso,"URL",&v);

}
// Сохраняем автора сеанса
void mlLogSeanceJSO::SetSeanceAuthor(const wchar_t* aAuthor)
{
	msAuthor = aAuthor;
}

// Сохраняем автора сеанса
void mlLogSeanceJSO::SetSeanceName(const wchar_t* aName)
{
	msName = aName;
}

void mlLogSeanceJSO::OnSeanceCreated()
{
	if(errorCode > 0){
		JSObject* jsoEvent = GPSM(mcx)->GetEventGO();
		jsval vErrorCode = INT_TO_JSVAL(errorCode);
		JS_SetProperty(mcx, jsoEvent, "errorCode", &vErrorCode);
		CallListeners(EVNM_onError);
		return;
	}

	CallListeners(EVNM_onCreated);
}

void mlLogSeanceJSO::OnSeanceClosed()
{
	if(errorCode > 0){
		JSObject* jsoEvent = GPSM(mcx)->GetEventGO();
		jsval vErrorCode = INT_TO_JSVAL(errorCode);
		JS_SetProperty(mcx, jsoEvent, "errorCode", &vErrorCode);
		CallListeners(EVNM_onError);
		return;
	}

	CallListeners(EVNM_onClosed);
}

/*
void mlLogSeancesJSO::CallListeners(const wchar_t* apwcEventName)
{
	jsval vFunc = JSVAL_NULL;
	if(apwcEventName == NULL) 
		return;
	// если есть свойство onXXX
	if(JS_GetUCProperty(mcx, mjso, apwcEventName, wcslen(apwcEventName), &vFunc) == JS_FALSE)
		return;
	if(!JSVAL_IS_STRING(vFunc)){
		if(!JSVAL_IS_REAL_OBJECT(vFunc))
			return;
		if(!JS_ObjectIsFunction(mcx, JSVAL_TO_OBJECT(vFunc)))
			return;
	}
	if(vFunc != JSVAL_NULL){
		// вызываем функцию
		jsval v;
		if(JSVAL_IS_STRING(vFunc)){
			JSString* jss = JSVAL_TO_STRING(vFunc);
			JS_EvaluateUCScript(mcx, mjso, JS_GetStringChars(jss), JS_GetStringLength(jss), 
				cLPCSTRq(apwcEventName), 1, &v);
		}else{
			JS_CallFunctionValue(mcx, mjso, vFunc, 0, NULL, &v);
		}
	}
}
*/

void mlLogSeancesJSO::OnReceivedSeancesInfo(){
	if(errorCode > 0){
		JSObject* jsoEvent = GPSM(mcx)->GetEventGO();
		jsval vErrorCode = INT_TO_JSVAL(errorCode);
		JS_SetProperty(mcx, jsoEvent, "errorCode", &vErrorCode);
		CallListeners(EVNM_onError);
		return;
	}
	// перебрать все записи в mvRecordInfo и создать по ним обертки (mlLogRecordJSO)
	std::vector<mlLogSeanceInfo>::iterator vi = mvSeanceInfo.begin();
	int iIdx;
	for(iIdx = 0; vi != mvSeanceInfo.end(); vi++, iIdx++){
		JSObject* jsoSeance = mlLogSeanceJSO::newJSObject(mcx);
		mlLogSeanceJSO* pSeance = (mlLogSeanceJSO*)JS_GetPrivate(mcx, jsoSeance);
		pSeance->Init(*vi);
		jsval v = OBJECT_TO_JSVAL(jsoSeance);
		JS_SetElement(mcx, mjso, iIdx, &v);
		SAVE_FROM_GC(mcx,mjso,jsoSeance);
	}
	length = iIdx;
	CallListeners(EVNM_onReceived);
}

void mlLogSeanceJSO::OnReceivedFullInfo()
{
	if(errorCode > 0){
		JSObject* jsoEvent = GPSM(mcx)->GetEventGO();
		jsval vErrorCode = INT_TO_JSVAL(errorCode);
		JS_SetProperty(mcx, jsoEvent, "errorCode", &vErrorCode);
		CallListeners(EVNM_onError);
		return;
	}

	CallListeners(EVNM_onReceivedFullInfo);
}

bool mlLogSeanceJSO::isPlaying()
{
	return mbPlaying;
}

void mlLogSeanceJSO::setPlaying(bool playing)
{
	mbPlaying = playing;
}
/*
void mlLogSeanceJSO::CallListeners(const wchar_t* apwcEventName)
{
	jsval vFunc = JSVAL_NULL;
	if(apwcEventName == NULL) 
		return;
	// если есть свойство onXXX
	if(JS_GetUCProperty(mcx, mjso, apwcEventName, wcslen(apwcEventName), &vFunc) == JS_FALSE)
		return;
	if(!JSVAL_IS_STRING(vFunc)){
		if(!JSVAL_IS_REAL_OBJECT(vFunc))
			return;
		if(!JS_ObjectIsFunction(mcx, JSVAL_TO_OBJECT(vFunc)))
			return;
	}
	if(vFunc != JSVAL_NULL){
		// вызываем функцию
		jsval v;
		if(JSVAL_IS_STRING(vFunc)){
			JSString* jss = JSVAL_TO_STRING(vFunc);
			JS_EvaluateUCScript(mcx, mjso, JS_GetStringChars(jss), JS_GetStringLength(jss), 
				cLPCSTRq(apwcEventName), 1, &v);
		}else{
			JS_CallFunctionValue(mcx, mjso, vFunc, 0, NULL, &v);
		}
	}
}
*/

}

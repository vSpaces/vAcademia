#include "mlRMML.h"
#include "config/oms_config.h"
#include <float.h>
#include <time.h>
#include "ILogWriter.h"
#include "mlSuperUser.h"
//#define _DEBUG_SM3

namespace rmml {

// таблица символов, разрешенных для использования в имени
// свойства или объекта
const bool g_charTable[256] = {
	false, false, false, false, false, false, false, false, // 000
	false, false, false, false, false, false, false, false, // 008
	false, false, false, false, false, false, false, false, // 016
	false, false, false, false, false, false, false, false, // 024
	false, false, false, false, true , false, false, false, // 032
	false, false, false, false, false, false, true , false, // 040
	true , true , true , true , true , true , true , true , // 048
	true , true , false, false, false, false, false, false, // 056
	true , true , true , true , true , true , true , true , // 064
	true , true , true , true , false, true , true , true , // 072
	true , true , true , true , true , true , true , true , // 080
	true , true , true , false, false, false, false, true , // 088
	false, true , true , true , true , true , true , true , // 096
	true , true , true , true , true , true , true , true , // 104
	true , true , true , true , true , true , true , true , // 112
	true , true , true , false, false, false, false, false, // 120
	false, false, false, false, false, false, false, false, // 128
	false, false, false, false, false, false, false, false, // 136
	false, false, false, false, false, false, false, false, // 144
	false, false, false, false, false, false, false, false, // 152
	false, false, false, false, false, false, false, false, // 160
	false, false, false, false, false, false, false, false, // 168
	false, false, false, false, false, false, false, false, // 176
	false, false, false, false, false, false, false, false, // 184
	false, false, false, false, false, false, false, false, // 192
	false, false, false, false, false, false, false, false, // 200
	false, false, false, false, false, false, false, false, // 208
	false, false, false, false, false, false, false, false, // 216
	false, false, false, false, false, false, false, false, // 224
	false, false, false, false, false, false, false, false, // 232
	false, false, false, false, false, false, false, false, // 240
	false, false, false, false, false, false, false, false};// 248

bool isPropertyOrObjectNameValid(unsigned char* str)
{
	for (unsigned int i = 0; i < strlen((char*)str); i++)
	if (!g_charTable[str[i]])
	{
		return false;
	}

	return true;
}

void mlPropsToSynch::AddProp(const wchar_t* apwcName){
	if(apwcName == NULL) return;
	// если имя в спске уже есть, то не добавляем
	baseClass::iterator vi;
	for(vi = baseClass::begin(); vi != baseClass::end(); vi++){
		mlPropToSynch& oProp = *vi;
		if(isEqual(oProp.msName.c_str(), apwcName))
			return;
	}
	baseClass::push_back(mlPropToSynch(apwcName));
}



mlSynch::mlSynch(void):
	MP_WSTRING_INIT(msName)
{
	mpParentElem = NULL;
	mpParent = NULL;
	isPaused = false;

	mpSynchedChildren = NULL;
	mbHashEvaluating = false;
	mbWaitForLoading = false;

	Reset();
}

JSBool mlSynch::AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(mlSynch::JSConstructor(cx, obj, argc, argv, rval) == JS_FALSE)
		return JS_FALSE;
	// ??
	return JS_TRUE;
}

v_elems* mlSynch::CreateSynchedChildren(){
	if(mpSynchedChildren != NULL && mpSynchedChildren != (v_elems*)1){
		mpSynchedChildren->clear();
	}else{
		MP_NEW_VECTOR_V(mpSynchedChildren, v_elems);
	}
	return mpSynchedChildren;
}

mlSynch::~mlSynch(void){
	if(mpSynchedChildren != NULL && mpSynchedChildren != (v_elems*)1)
		MP_DELETE( mpSynchedChildren);
}

JSPropertySpec mlSynch::_JSPropertySpec[] = {
	JSPROP_RO(serverTime)
	{ 0, 0, 0, 0, 0 }
};

#define ML_EMPLT_BIN_STATE_MAX_SIZE 6

///// JavaScript Function Table
JSFunctionSpec mlSynch::_JSFunctionSpec[] = {
	JSFUNC(capture,0)
	JSFUNC(free,0)
	JSFUNC(isMyOwn,0)
	JSFUNC(isFree,0)
	JSFUNC(mayChange,0)
	//JSFUNC(fixPos,1)
	JSFUNC(remove,1)
	JSFUNC(settingStart,0)
	JSFUNC(settingEnd,0)
//	JSFUNC(restoreObject,1)
	JSFUNC(sendMessage,1)
	JSFUNC(getOwner,0)	// возвращает идентификатор пользователя-собственника
	JSFUNC(getLocation,0)	// возвращает идентификатор локации
	JSFUNC(getDeleteTime,1) // возвращает время, когда объект будет удален сервером
	JSFUNC(setDeleteTime,1) // задать время, когда серверу надо будет удалить объект
	JSFUNC(pause,1)
	JSFUNC(resume,1)
	JSFUNC(getCommunicationArea,0)	// возвращает идентификатор зоны общения
	JSFUNC(isEmptyBinState,0)	// возвращает состояние binState пустой или нет
	{ 0, 0, 0, 0, 0 }
};

//EventSpec mlSynch::_EventSpec[] = {
//	MLEVENT(onOwnerChange)	
//};

MLJSCLASS_IMPL_BEGIN(Synch,mlSynch,0)
	MLJSCLASS_ADDPROPFUNC
//	MLJSCLASS_ADDALTERCONSTR(AlterJSConstructor)
//	MLJSCLASS_ADDPROPENUMOP
//	MLJSCLASS_ADDEVENTS
MLJSCLASS_IMPL_END(mlSynch)

/*
__flags – (целое) специальные флаги
0-й флаг – если установлен, то это объект с назначаемым сервером собственником
устанавливается через свойство __type=2 в атрибуте synch
1-й флаг – если установлен, то свойства может менять только собственник, если он установлен (если нет, то никто не может менять)
устанавливается через свойство __type=1 в атрибуте synch
__owner – (строка) собственник объекта (только он может изменять свойства)
Пока логин пользователя.
Может назначаться при создании через __owner=1 в атрибуте synch (если у объекта собственник еще не установлен, то автоматически собственником будет назначен клиент, создавший объект)
*/

// установить флаг изменения системного свойства
void mlSynch::SetChanged(int aiMask){
	if(mpParentElem == NULL)
		return;
	miChanged |= aiMask;
	// уведомляем родительский RMML-объект об изменении свойства
	if( !mbHashEvaluating)
	{
		mpParentElem->SetSynchChanged();
	}
}

JSBool mlSynch::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	if(JSVAL_IS_STRING(id)){
		JSString* jssPropName = JSVAL_TO_STRING(id);
		wchar_t* pwcPropName = (wchar_t*)JS_GetStringChars(jssPropName);
		mlSynch* priv = (mlSynch*)JS_GetPrivate(cx, obj);
		// если предположительно системное свойство
		if(pwcPropName[0] == L'_' && pwcPropName[1] == L'_'){
			if(priv->mbInitialization){
				if(isEqual(pwcPropName, L"__type")){
					if(JSVAL_IS_INT(*vp)){
						int iVal = JSVAL_TO_INT(*vp);
						if(iVal == 1){
							priv->mSSPs.miFlags |= MLSYNCH_OWNER_ONLY_SYNCHR_MASK;
							priv->SetChanged(MLSYNCH_FLAGS_SET_MASK);
						}else if(iVal == 2){
							priv->mSSPs.miFlags |= MLSYNCH_SERVER_OWNER_ASSIGNATION_MASK;
							priv->SetChanged(MLSYNCH_FLAGS_SET_MASK);
						}else if(iVal == 4){
							priv->mSSPs.miFlags |= MLSYNCH_EXCLUSIVE_OWN_MASK;
							priv->SetChanged(MLSYNCH_FLAGS_SET_MASK);
						}
					}
					return JS_TRUE;
				}else if(isEqual(pwcPropName, L"__owner")){
					if(JSVAL_IS_INT(*vp)){
						int iVal = JSVAL_TO_INT(*vp);
						if(iVal == 1){
							priv->mSSPs.miFlags |= MLSYNCH_OWNER_ONLY_SYNCHR_MASK;
							priv->SetChanged(MLSYNCH_FLAGS_SET_MASK);
							priv->Capture();
						}
					}
					return JS_TRUE;
				}
			}
		}
		if(!priv->mbSynchronizing && priv->GetTopParent()->MayChange()!=MCERROR_NOERROR){
			JS_GetUCProperty(cx, obj, (const jschar*)pwcPropName, -1, vp);
			return JS_TRUE;
		}

		bool bPrevSetting = priv->mbSetting;
		priv->mbSetting = true;
		priv->mpwcPropName = pwcPropName;
		// если устанавливается свойство-подобъект, 
		if(JSVAL_IS_REAL_OBJECT(*vp)){
			JSObject* jsoProp = JSVAL_TO_OBJECT(*vp);
			if(mlSynchLinkJSO::IsInstance(cx, jsoProp)){
				// это синхронизируемая ссылка
				mlSynchLinkJSO* pSrcSynchLink = (mlSynchLinkJSO*)JS_GetPrivate(cx, jsoProp);
				JSObject* jsoSynchLink = mlSynchLinkJSO::newJSObject(cx);
				mlSynchLinkJSO* pSynchLink = (mlSynchLinkJSO*)JS_GetPrivate(cx, jsoSynchLink);
				pSynchLink->Init(pSrcSynchLink);
				pSynchLink->SetParents(priv->mpParentElem, priv);
				*vp = OBJECT_TO_JSVAL(jsoSynchLink);
			}else{
				// если это синхронизируемый RMML-объект (есть ID), то создать под него mlSynchLinkJSO, а не mlSynch
				mlRMMLElement* pMLEl = JSVAL_TO_MLEL(cx, *vp);
				unsigned int uiBornRealityID = 0;
				if(pMLEl != NULL && pMLEl->GetSynchID(uiBornRealityID, false) != OBJ_ID_UNDEF){
					JSObject* jsoSynchLink = mlSynchLinkJSO::newJSObject(cx);
					mlSynchLinkJSO* pSynchLink = (mlSynchLinkJSO*)JS_GetPrivate(cx, jsoSynchLink);
					pSynchLink->muObjectID = pMLEl->GetSynchID(uiBornRealityID, false);
					pSynchLink->muBornRealityID = uiBornRealityID;
					pSynchLink->SetParents(priv->mpParentElem, priv);
					*vp = OBJECT_TO_JSVAL(jsoSynchLink);
				}else{
					// то создать новый mlSynch объект и переприсвоить ему все свойства
					JSObject* jsoSubSynch = mlSynch::newJSObject(cx);
					mlSynch* pSubSynch = (mlSynch*)JS_GetPrivate(cx, jsoSubSynch);
					pSubSynch->SetParents(priv->mpParentElem, priv);
					pSubSynch->msName = pwcPropName;
					SAVE_FROM_GC(cx, JS_GetGlobalObject(cx), jsoSubSynch);	// Tandy: 14.01.2011
					*vp = OBJECT_TO_JSVAL(jsoSubSynch);
					priv->mjsvProp = *vp;
					//
					JSIdArray* idArr = JS_Enumerate(cx, jsoProp);
					for(int ij = 0; ij < idArr->length; ij++){
						jsval v;
						if(!JS_IdToValue(cx,idArr->vector[ij],&v))
							continue;
						if(!JSVAL_IS_STRING(v))
							continue;
						JSString* jssPName = JSVAL_TO_STRING(v);
						jschar* pjcPName = JS_GetStringChars(jssPName);
						jsval vProp;
						pSubSynch->mbSynchronizing = priv->mbSynchronizing;
						JS_GetUCProperty(cx, jsoProp, pjcPName, -1, &vProp);
						// если присваивается значение undefined, то заменим его на null, так как на остальных клиентах тоже будет null

						// Проверка нужна для случая, если в synch добавляют undefined свойство.
						// Иначе на компе owner`а оно будет undefined, а на всех остальных null.
						// Проще чтобы на всех null была.
						if( JSVAL_IS_VOID(vProp))
							vProp = JSVAL_NULL;
						// при 3м подрежиме не надо записывать, что изменилось это свойство.
						// достаточно будет записи об изменении всего подобъекта
						pSubSynch->mbSubPropSetting = true;
						JS_SetUCProperty(cx, jsoSubSynch, pjcPName, -1, &vProp);
						pSubSynch->mbSubPropSetting = false;
						pSubSynch->mbSynchronizing = false;
					}
					FREE_FOR_GC(cx, JS_GetGlobalObject(cx), jsoSubSynch);	// Tandy: 14.01.2011
				}
			}
		}
		if(*vp == JSVAL_VOID)
			*vp = JSVAL_NULL;
		priv->mjsvProp = *vp;
		priv->SetChanged(pwcPropName);
		priv->mbSetting = bPrevSetting;
	}

	return JS_TRUE;
}

JSBool mlSynch::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	mlSynch* priv = (mlSynch*)JS_GetPrivate(cx, obj);
	if(JSVAL_IS_STRING(id)){
		if(priv != NULL && priv->mbSetting){
			// значит значение надо выдавать новое
			JSString* jssPropName = JSVAL_TO_STRING(id);
			wchar_t* pwcPropName = (wchar_t*)JS_GetStringChars(jssPropName);
			if(isEqual(pwcPropName, priv->mpwcPropName)){
				*vp = priv->mjsvProp;
			}
		}
	}
	else
		// Серверное время
		if (JSVAL_IS_INT(id))
		{
			 int iID = JSVAL_TO_INT(id);
			 if (iID == JSPROP_serverTime)
			 {
				jsdouble jsdArg = (double)priv->GetServerTime();
				mlJS_NewDoubleValue(cx, jsdArg, vp);
			 }			
		}
	return JS_TRUE;
}

unsigned __int64 mlSynch::GetServerTime(){
	omsContext* pContext = GPSM(mcx)->GetContext();
	unsigned __int64 deltaTime = pContext->mpApp->GetTimeStamp() - mSetPropLocalTime;

//				 mlTrace(cx, "%S       serverTime: %I64d \n", mpParentElem->mpName,  mSSPs.serverTime);
//				 mlTrace(cx, "%S       mSetPropLocalTime: %I64d \n", mpParentElem->mpName,  mSetPropLocalTime);
//				 mlTrace(cx, "%S       deltaTime: %I64d \n", mpParentElem->mpName,  deltaTime);
//				 unsigned __int64 intTime = (unsigned __int64)((mSSPs.serverTime / 10000) + deltaTime);
//				 mlTrace(cx, "%S       intTime: %I64d \n", mpParentElem->mpName,  intTime);

	return (unsigned __int64)((mSSPs.serverTime / 10000) + deltaTime);
}

// Исправление #477
void mlSynch::Reset()
{
	if (mpParentElem != NULL)
	{
		moIBinSynch* pmoBinSynch = GetIBinSynch();
		if (pmoBinSynch != NULL)
		{
			pmoBinSynch->Reset();
		}
		// 
		JSIdArray* idArr = JS_Enumerate(mcx, mjso);
		for(int ij = 0; ij < idArr->length; ij++){
			jsval v;
			if(!JS_IdToValue(mcx,idArr->vector[ij],&v))
				continue;
			if(!JSVAL_IS_STRING(v))
				continue;
			JSString* jssPName = JSVAL_TO_STRING(v);
			jschar* pjcPName = JS_GetStringChars(jssPName);
			jsval rv;
			JS_DeleteUCProperty2(mcx, mjso, pjcPName, JS_GetStringLength(jssPName), &rv);
		}

		mSSPs.Reset();
		mSSPs.mBinState.ApllyUpdating(BSSCC_REPLACE_ALL, 0, NULL, 0);
	}

	mwServerVersion = 0;
	mwClientVersion = 0;

	mbSynchronizing = false;
	mbSetting = false;
	mpwcPropName = NULL;
	mjsvProp = JSVAL_VOID;
	isSynchInitialized = false;
	//
	miChanged = 0;
	//
	mbInitialization = false;
	mbSubPropSetting = false;
	//
	mOutLocationID = 0;
	mSetPropLocalTime = 0;
	fullStateQueryTime = 0;
	mOutCommunicationAreaID = 0;
	//
	mbDontSynch = false;
	nextSendingStateType = syncObjectState::ST_UPDATE;

	if (mpParentElem != NULL)
	{
		FireOnReset();
	}
}
// Конец #477

void mlSynch::ResetState()
{
	if (mpParentElem != NULL)
	{
		moIBinSynch* pmoBinSynch = GetIBinSynch();
		if (pmoBinSynch != NULL)
		{
			pmoBinSynch->Reset();
		}
		// 
		JSIdArray* idArr = JS_Enumerate(mcx, mjso);
		for(int ij = 0; ij < idArr->length; ij++){
			jsval v;
			if(!JS_IdToValue(mcx,idArr->vector[ij],&v))
				continue;
			if(!JSVAL_IS_STRING(v))
				continue;
			JSString* jssPName = JSVAL_TO_STRING(v);
			jschar* pjcPName = JS_GetStringChars(jssPName);
			jsval rv;
			JS_DeleteUCProperty2(mcx, mjso, pjcPName, JS_GetStringLength(jssPName), &rv);
		}

		mSSPs.mBinState.ApllyUpdating(BSSCC_REPLACE_ALL, 0, NULL, 0);
	}
	//
	miChanged = 0;
	//
	mbDontSynch = false;
	nextSendingStateType = syncObjectState::ST_UPDATE;
}

// надо запоминать какие свойства изменились не только в Synch-е верхнего уровня, но и в подобъектах.
// Запоминаем в том же Synch-е верхнего уровня
void mlSynch::SetChanged(const wchar_t* apwcPropName){
	// получаем указатель на synch
	// заодно генерируем "путь" к свойству
	mlSynch* pSynch = this;
	mlString sPropPath = apwcPropName;
	while(pSynch->mpParent != NULL){
		if(pSynch->mpParent != NULL){ // сам объект synch не считается
			sPropPath.insert(0, L".");
			sPropPath.insert(0, pSynch->msName);
		}
		pSynch = pSynch->mpParent;
	}
	
	mlString sFullPropPath = sPropPath;

	// вызываем обработчики watch-ей на весь путь
	if (!mbWaitForLoading)
	for(;;){
		if( !mbHashEvaluating)
			pSynch->CallListeners2(sPropPath.c_str());
		mlString::size_type dotPos = sPropPath.rfind(L".");
		if(dotPos == mlString::npos)
			break;
		sPropPath.erase(dotPos);
	};
	// если идет установка свойств в значение с сервера, 
	if(pSynch->mbSynchronizing) 
		return; // то не делать запись
	// или объект только в процессе создания
	if(pSynch->mpParentElem == NULL || pSynch->mpParentElem->mpMO == NULL)
		return; // то не делать запись
	// или менеджер сцен не в нормальном режиме
	if(GPSM(mcx)->GetMode() != smmNormal)
		return; // то не делать запись
	// ?? если изменение свойства произошло не в результате работы обработчика watch
	// ?? то тоже не делать запись
	
	// если идет установка свойства при установке целого подобъекта
	if(mbSubPropSetting)
		return; // то не делать запись
	// сохраняем запись об изменении свойства для синхронизации
	pSynch->mPropsToSynch.AddProp(sFullPropPath.c_str());

	// уведомляем родительский RMML-объект об изменении свойства
	if( !mbHashEvaluating)
		mpParentElem->SetSynchChanged();
}

void mlSynch::DontSynchLastChanges(){
	// локацию надо посылать по любому, если она отличается от той, что получена с сервера
	int iLocationMask = miChanged & MLSYNCH_LOCATION_CHANGED_MASK;
	miChanged = 0;
	if(iLocationMask != 0){
		SetChanged(MLSYNCH_LOCATION_CHANGED_MASK);
	}
	int iCommunicationAreaMask = miChanged & MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK;
	miChanged = 0;
	if(iCommunicationAreaMask != 0){
		SetChanged(MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK);
	}
	mPropsToSynch.Clear();
}

void mlSynchPropListeners::NotifySenderDestroyed(mlRMMLElement* apMLElSender){
	iterListeners vi = mvListeners.begin();
	for(; vi != mvListeners.end(); vi++){
		mlSynchPropListener& lis = *vi;
		v_elems_iter vi2 = lis.vListeners.begin();
		for(; vi2 != lis.vListeners.end(); vi2++){
			mlRMMLElement* pMLEl = *vi2;
			pMLEl->EventSenderDestroyed(apMLElSender);
		}
	}
}

pv_elems mlSynchPropListeners::Get(const wchar_t* apwcSynchProp){
	iterListeners vi = mvListeners.begin();
	for(; vi != mvListeners.end(); vi++){
		mlSynchPropListener& lis = *vi;
		if(isEqual(lis.sProp.c_str(), apwcSynchProp))
			return &(lis.vListeners);
	}
	return NULL;
}

void mlSynchPropListeners::Set(const wchar_t* apwcSynchProp, mlRMMLElement* apMLElListener){
	iterListeners vi = mvListeners.begin();
	for(; vi != mvListeners.end(); vi++){
		mlSynchPropListener& lis = *vi;
		if(isEqual(lis.sProp.c_str(), apwcSynchProp)){
			lis.vListeners.push_back(apMLElListener);
			return;
		}
	}
	mvListeners.push_back(mlSynchPropListener(apwcSynchProp, apMLElListener));
	mbChanged=true;
}

void mlSynchPropListeners::Remove(const wchar_t* apwcSynchProp, mlRMMLElement* apMLElListener){
	for(iterListeners vi = mvListeners.begin(); vi != mvListeners.end(); vi++){
		mlSynchPropListener& lis = *vi;
		if(isEqual(lis.sProp.c_str(), apwcSynchProp)){
			v_elems_iter vi2 = lis.vListeners.begin();
			for(; vi2 != lis.vListeners.end(); vi2++){
				mlRMMLElement* pMLEl = *vi2;
				lis.vListeners.erase(vi2);
				if(lis.vListeners.size() == 0)
					mvListeners.erase(vi);
				mbChanged=true;
				return;
			}
		}
	}
}

void mlSynch::CallListeners2(const wchar_t* apwcSynchProp)
{

	pv_elems pv=NULL;

	pv = mListeners.Get(apwcSynchProp);
	if(pv != NULL && pv->size() == 0) pv = NULL;
	if(pv == NULL) 
		return;

	mListeners.ClearChanged();
	v_elems::iterator vi = pv->begin();
	int iELCnt = pv->size();
	if(iELCnt <= 0) return;
	// сделать копию списка слушателей и идти по нему сверяясь с наличием слешателя в 
	// исходном списке, если он изменился
	mlRMMLElement** paListeners = MP_NEW_ARR(mlRMMLElement*, (iELCnt+1)*sizeof(mlRMMLElement*));
#if _MSC_VER >= 1600
	mlRMMLElement** paListenersSrc = vi._Ptr;
#elif _MSC_VER > 1200
	mlRMMLElement** paListenersSrc = vi._Myptr;
#else
	mlRMMLElement** paListenersSrc = vi;
#endif
	memcpy(paListeners, paListenersSrc, iELCnt*sizeof(mlRMMLElement*));
	paListeners[iELCnt] = NULL;
	for(;; paListeners++){
		mlRMMLElement* pMLEl=*paListeners;
		if(pMLEl == NULL) break;
		// если во время обработки события изменился список слушателей,
		if(mListeners.IsChanged()){
			// то проверяем его наличие в изменившемся списке
			pv = mListeners.Get(apwcSynchProp);
			if(pv!=NULL && pv->size()==0) pv=NULL;
			if(pv == NULL) break;
			v_elems::iterator vi;
			for(vi=pv->begin(); vi!=pv->end(); vi++){
				if(*vi == pMLEl) break;
			}
			if(vi==pv->end()) continue;
		}
		if(pMLEl->EventNotify(0, mpParentElem)){
			// надо удалить его из списка, 
			// сохранив флаг измененности списка неизменным
			bool bWasChanged = mListeners.IsChanged();
			mListeners.Remove(apwcSynchProp, pMLEl);
			if(!bWasChanged)
				mListeners.ClearChanged();
		}
	}
}

mlString JSVal2String(JSContext* mcx, jsval vProp)
{
//	mlString sRetVal;
	if(JSVAL_IS_INT(vProp)){
		int iVal = JSVAL_TO_INT(vProp);
		wchar_t pwcBuf[25];
		swprintf_s(pwcBuf, 25, L"%d", iVal);
		return pwcBuf;
	}else if(JSVAL_IS_BOOLEAN(vProp)){
		bool bVal=JSVAL_TO_BOOLEAN(vProp);
		if(bVal){
			return L"true";
		}else
			return L"false";
	}else if(JSVAL_IS_STRING(vProp)){
		jschar* jsc = JS_GetStringChars(JSVAL_TO_STRING(vProp));
		mlString sRetVal;
		sRetVal += L"'";
		sRetVal += (wchar_t*)jsc;
		sRetVal += L"'";
		return sRetVal;
	}else if(JSVAL_IS_NULL(vProp)){
		return L"null";
	}else if(JSVAL_IS_VOID(vProp)){
		return L"undefined";
	}else if(JSVAL_IS_DOUBLE(vProp)){
		double dVal = *JSVAL_TO_DOUBLE(vProp);
		wchar_t pwcBuf[45];
		swprintf_s(pwcBuf, 45, L"%f", dVal);
		return pwcBuf;
	}else if(JSVAL_IS_OBJECT(vProp)){
		if(JS_TypeOfValue(mcx,vProp)==JSTYPE_FUNCTION){
			JSObject* jsoFun = JSVAL_TO_OBJECT(vProp);
			JSClass* pClass = ML_JS_GETCLASS(mcx, jsoFun);
			////JSString * JS_DecompileScript(JSContext *cx, JSScript *script,
			////    const char *name, uintN indent);
			//if(pClass->flags==1 || meJSOutMode == JSOM_STATE || meJSOutMode == JSOM_STATE2){
			//	// Script
			//	*this << PJSVT_NO;
			//}else{
			JSFunction* jsf = JS_ValueToFunction(mcx, vProp);
			if(jsf){
				JSString* jssFun=JS_DecompileFunction(mcx, jsf, 0);
				jschar* jsc=JS_GetStringChars(jssFun);
				return (const wchar_t*)jsc;
			}else{
				return L"?function?";
			}
			//}
		}else{
			JSObject* jsoP = JSVAL_TO_OBJECT(vProp);
			JSClass* pClass = ML_JS_GETCLASS(mcx, jsoP);
			if(isEqual(pClass->name, "Link") && mlSynchLinkJSO::IsInstance(mcx, jsoP)){
				mlSynchLinkJSO* pSynchLink = (mlSynchLinkJSO*)JS_GetPrivate(mcx, jsoP);
				mlString sRetVal = L"Link(";
				JSObject* jsoRef = pSynchLink->GetRef();
				if(jsoRef == NULL || !(JSO_IS_MLEL(mcx, jsoRef))){
					wchar_t pwcBuf[45];
					unsigned int uiBornRealityID = 0; 
					unsigned int uiID = pSynchLink->GetObjectID(uiBornRealityID);
					swprintf_s(pwcBuf, 45, L"%d:%d", uiBornRealityID, uiID);
					sRetVal += pwcBuf;
				}else{
					mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(mcx, jsoRef);
					mlString sStringID = pMLEl->GetStringID();
					sRetVal += sStringID;
				}
				sRetVal += L")";
				return sRetVal;
			}
			bool bArray = false;
			mlString sRetVal = L"{";
			if(pClass != NULL){
				if(isEqual(pClass->name, "Array"))
					bArray = true;
				if(bArray){
					sRetVal = L"[";
				}else{
					sRetVal += L"(";
					sRetVal += cLPWCSTRq(pClass->name);
					sRetVal += L") ";
				}
			}
			JSIdArray* pGPropIDArr = JS_Enumerate(mcx, jsoP);
			int iLen = pGPropIDArr->length;
			for(int ii = 0; ii < iLen; ii++){
				jsid id = pGPropIDArr->vector[ii];
				jsval v;
				if(!JS_IdToValue(mcx, id, &v)){					
				}
				jschar* jscProp = NULL;
				jsval vProp2;
				if(JSVAL_IS_STRING(v)){
					jscProp=JS_GetStringChars(JSVAL_TO_STRING(v));
					sRetVal += (wchar_t*)jscProp;
					sRetVal += L":";
					JS_GetUCProperty(mcx, jsoP, jscProp, -1, &vProp2);
				}else if(JSVAL_IS_INT(v)){
					int iIdx = JSVAL_TO_INT(v);
					JS_GetElement(mcx, jsoP, JSVAL_TO_INT(v), &vProp2);
				}
				sRetVal += JSVal2String(mcx, vProp2);
				if(ii < (iLen - 1))
					sRetVal += L", ";
			}
			if(bArray){
				sRetVal += L"]";
			}else
				sRetVal += L"}";
			return sRetVal;
		}
	}else{ // unknown jsval type
		JSType jst=JS_TypeOfValue(mcx,vProp);
	}
	return L"??";
	// return sRetVal;
}

//iLen = pGPropIDArr->length;
//for(int ikl=0; ikl<iLen; ikl++){
//	jsid id=pGPropIDArr->vector[ikl];
//	jsval v;
//	if(!JS_IdToValue(mcx,id,&v)){
//		raiseCriticalException();
//	}
//	jschar* jscProp = NULL;
//	jsval vProp;
//	if(JSVAL_IS_STRING(v)){
//		jscProp=JS_GetStringChars(JSVAL_TO_STRING(v));

__forceinline void SaveProp(mlSynchData& oData, const char* apcName, jsval ajsvVal){
	oData << apcName;
	MLWORD wValueLength = 0;	
	int iValueLenPos = oData.getPos();
	oData << wValueLength;
	int iValueBeginPos = oData.getPos();
	oData << (long)ajsvVal;
	wValueLength = oData.getPos() - iValueBeginPos;
	oData.setPos(iValueLenPos); 
	oData << wValueLength;
}

void mlSynch::SetPropToSynchedJSO(JSObject* ajsoSynched, const char* apcPropName){
	// по 3й субмодели надо записывать в synched и все родительские подобъекты
	std::string sPropPath = apcPropName;
	int iPropNameBegin = 0;
	JSObject* pParentJSO = ajsoSynched;
	for(;;){
		std::string::size_type dotPos = sPropPath.find('.', iPropNameBegin);
		std::string sPropName;
		// если это само свойство
		if(dotPos == std::string::npos){
			sPropName = sPropPath.substr(iPropNameBegin);
			// то просто устанавливаем его в true и заканчиваем
			jsval jsv = JSVAL_TRUE;
			JS_SetProperty(mcx, pParentJSO, sPropName.c_str(), &jsv);
			break;
		}
		sPropName = sPropPath.substr(iPropNameBegin, dotPos - iPropNameBegin);
		jsval vProp = JSVAL_NULL;
		JS_GetProperty(mcx, pParentJSO, sPropName.c_str(), &vProp);
		// если подобъекта еще не было или было, но со значением true
		if(vProp == JSVAL_NULL || vProp == JSVAL_VOID){
			// то надо создать подобъект
			JSObject* jsoNewObj = JS_NewObject(mcx, NULL, NULL, NULL);
			jsval vNewObj = OBJECT_TO_JSVAL(jsoNewObj);
			JS_SetProperty(mcx, pParentJSO, sPropName.c_str(), &vNewObj);
			pParentJSO = jsoNewObj;
		}else{
			// а если уже был, то проверить, действительно ли он - объект (на всякий случай)
			if(!JSVAL_IS_REAL_OBJECT(vProp))
				break;
			pParentJSO = JSVAL_TO_OBJECT(vProp);
		}
		iPropNameBegin = dotPos + 1;
	};
}

inline std::string to_ascii_lazy(const std::wstring& str){
	const wchar_t* pwc = str.c_str();
	size_t len = str.size();
	char* pchbegin = (char*)_alloca(len + 1);
	char* pch = pchbegin;
	for(; len > 0; len--, pwc++, pch++)
		*pch = (char)(*pwc);
	*pch = '\0';
	return pchbegin;
}

/*
Размер	| Назване		| Данные
4		| objId 		Идентификатор Объекта
2		| version		Версия объекта
1		| sysPropCount 	Количество передаваемых системных свойств
sysPropCount раз { 	
 1		| sysPropID 	Идентификатор системного свойства
 n		| sysPropValue 	Значение системного свойства (см.ниже)
} 	
2		| propCount 	Количество измененных в объекте свойств
propCount раз { 	
 2		| propNameLength 		Длина имени свойства
 propNameLength | nameLength 	Название свойства
 2		| propValueLength 		Длина значения свойства
 propValueLength | propValue 	Значение свойства
}
*/

bool mlSynch::IsSynchChanged()
{
	if(mpParentElem == NULL)
		return false;
	if(mPropsToSynch.size() == 0 && !miChanged) 
		return false;
	return true;
}

bool mlSynch::GetSynchProps(syncObjectState& aState, JSObject* &jsoSynched)
{
	if(!IsSynchChanged()) 
		return false;
	if(mbDontSynch)
		return false;

	omsContext* pContext = GPSM(mcx)->GetContext();

	// получить идентификатор объекта в системе
	unsigned int uiID = mpParentElem->GetID();
	unsigned int uiBornRealityID = mpParentElem->GetBornRealityID();
	unsigned int uiRealityID = mpParentElem->GetRealityID();
	if (uiID != OBJ_ID_UNDEF)
	{
		if (nextSendingStateType == syncObjectState::ST_FULL)
			aState.type = syncObjectState::ST_FULL;
		else
			aState.type = syncObjectState::ST_UPDATE;
		aState.uiID =  uiID;
		aState.uiBornRealityID = uiBornRealityID;
		aState.uiRealityID = uiRealityID;
	}
	nextSendingStateType = syncObjectState::ST_UPDATE;

	if(!mbHashEvaluating)
		mwClientVersion++;

#ifdef _DEBUG_SM3
	if(!mbHashEvaluating)
		mlTraceSyncMode3(mcx, "SEND2 prop changes (objID: %u, bornRealityID: %u, name: %s, serverVersion: %u, clientVersion: %u) :{\n", uiID, uiBornRealityID, cLPCSTR( mpParentElem->GetName()), mwClientVersion, mwServerVersion);
#endif

	aState.mwClientVersion = mwClientVersion;
	aState.mwServerVersion = mwServerVersion;

	// сначала записываем системные свойства
	aState.sys_props.miSet = 0;
	if(miChanged){
// 1. Flags
		if(miChanged & MLSYNCH_FLAGS_SET_MASK){
			aState.sys_props.SetFlags( mSSPs.miFlags);
#ifdef _DEBUG_SM3
			if(!mbHashEvaluating)
				mlTraceSyncMode3(mcx, "  __flags=%d\n", mSSPs.miFlags);
#endif
		}
// 2. Owner
		if(miChanged & MLSYNCH_OWNER_CHANGED_MASK){
			aState.sys_props.SetOwner( mSSPs.msOwner);
#ifdef _DEBUG_SM3
			if(!mbHashEvaluating)
				mlTraceSyncMode3(mcx,"  __owner= %S \n",mSSPs.msOwner.c_str());
#endif
		}
// 3. Zone
		if(miChanged & MLSYNCH_ZONE_CHANGED_MASK){
			aState.sys_props.SetZone( mSSPs.miZone);
#ifdef _DEBUG_SM3
			if(!mbHashEvaluating)
				mlTraceSyncMode3(mcx, "  __zone=%d\n", mSSPs.miZone);
#endif
		}
// 4. Reality
		if(miChanged & MLSYNCH_REALITY_SET_MASK){
//			aState.sys_props.SetReality( mSSPs.muReality);
#ifdef _DEBUG_SM3
			if(!mbHashEvaluating)
				mlTraceSyncMode3(mcx, "  __reality=%d\n", mSSPs.muReality);
#endif
		}
// 5. CoordinateZ
		if ((miChanged & MLSYNCH_COORDINATE_SET_MASK) != 0 && (mSSPs.miSet & MLSYNCH_COORDINATE_SET_MASK) != 0){
			aState.sys_props.SetCoordinateZ(  mSSPs.mfCoordinateZ);
#ifdef _DEBUG_SM3
			if(!mbHashEvaluating)
				mlTraceSyncMode3(mcx, "  __coordinateZ=%f\n", mSSPs.mfCoordinateZ);
#endif
		}
// 6. Coordinates XY
		if ((miChanged & MLSYNCH_COORDINATE_XY_SET_MASK) != 0 && (mSSPs.miSet & MLSYNCH_COORDINATE_XY_SET_MASK) != 0){
			aState.sys_props.SetCoordinateXY( mSSPs.mfCoordinateX, mSSPs.mfCoordinateY);
#ifdef _DEBUG_SM3
			if(!mbHashEvaluating)
			{
				mlTraceSyncMode3(mcx, "  __coordinateX=%f\n", mSSPs.mfCoordinateX);
				mlTraceSyncMode3(mcx, "  __coordinateY=%f\n", mSSPs.mfCoordinateY);
			}
#endif
		}
// 7 Server Time
		if(miChanged & MLSYNCH_SERVERTIME_SET_MASK){
			aState.sys_props.SetServerTime( mSSPs.serverTime);
#ifdef _DEBUG_SM3
			if(!mbHashEvaluating)
				mlTraceSyncMode3(mcx,"  __serverTime= %u \n" ,mSSPs.serverTime); 
#endif
		}
// 8. Bin State
		if ((miChanged & MLSYNCH_BIN_STATE_CHANGED_MASK)/* && binStateForSending.GetSize() != 0*/){	// условие закоментил Alex, иначе нельзя совсем обнулить BinState
			aState.sys_props.SetBinState( binStateForSending);
#ifdef _DEBUG_SM3
			if(!mbHashEvaluating)
				mlTraceSyncMode3(mcx, "  __binState - %d bytes\n", binStateForSending.GetSize());
#endif
			binStateForSending.ApllyUpdating(BSSCC_CLEAR, mSSPs.mBinState.GetVersion(), NULL, 0);
		}
// 9. Location
		if(miChanged & MLSYNCH_LOCATION_CHANGED_MASK){
			aState.sys_props.SetLocation( mSSPs.msLocation);
#ifdef _DEBUG_SM3
			if(!mbHashEvaluating)
				mlTraceSyncMode3(mcx, "  __location='%S'\n", mSSPs.msLocation.c_str());
#endif
		}
		// 10. CommunicationArea
		if(miChanged & MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK){
			aState.sys_props.SetCommunicationArea( mSSPs.msCommunicationArea);
#ifdef _DEBUG_SM3
			if(!mbHashEvaluating)
				mlTraceSyncMode3(mcx, "  __communicationArea='%S'\n", mSSPs.msCommunicationArea.c_str());
#endif
		}
// 11.  MP_DELETE( time
		if(miChanged & MLSYNCH_DELETETIME_CHANGED_MASK){
			aState.sys_props.SetDeleteTime( mSSPs.mlDeleteTime);
#ifdef _DEBUG_SM3
			if(!mbHashEvaluating)
			{
				if(mlIsSynchTracesEnabled()){
					std::string sTime = FormatServerTime(mSSPs.mlDeleteTime);
					mlTraceSyncMode3(mcx, "  __deleteTime=%s\n", sTime.c_str());
				}
			}
#endif
		}
	}
	miChanged = 0;

	// складываем в aState.props свойства
	// все объекты "разворачиваем" в набор свойств с проверкой, не попали ли они уже в aState.props
	// теперь сохраняем собственно свойства
	if(mPropsToSynch.size() > 0 ){
		jsoSynched = JS_NewObject(mcx, NULL, NULL, NULL);
		SAVE_FROM_GC(mcx, mjso, jsoSynched);
	}

	mlPropsToSynch::iterator vi = mPropsToSynch.begin();
	for (;  vi != mPropsToSynch.end();  vi++)
	{
		syncObjectProp currentProp;
		mlPropToSynch& oProp = *vi;
		jsval vProp = OBJECT_TO_JSVAL(mjso);
		std::wstring sPropPath = oProp.msName;
		std::wstring sPropName;
		int iPropNameBegin = 0;
		for(;;)
		{
			bool bLastName = false; 
			std::wstring::size_type dotPos = sPropPath.find(L'.', iPropNameBegin);
			if(dotPos == std::wstring::npos)
			{
				sPropName = sPropPath.substr(iPropNameBegin);
				bLastName = true; // sPropName содержит последнее имя в пути (т.е. имя непосредственного самого свойства)
			}
			else
			{
				sPropName = sPropPath.substr(iPropNameBegin, dotPos - iPropNameBegin);
			}
			if(!JS_GetUCProperty(mcx, JSVAL_TO_OBJECT(vProp), (const jschar*)sPropName.c_str(), -1, &vProp))
				break;
			if(bLastName)
				break;
			if(!JSVAL_IS_REAL_OBJECT(vProp)) // такого не должно быть, но на всякий случай...
				break;
			
			iPropNameBegin = dotPos + 1;
		};
		currentProp.name = to_ascii_lazy(sPropPath.c_str());

		if (jsoSynched == NULL){
			jsoSynched = JS_NewObject(mcx, NULL, NULL, NULL);
			SAVE_FROM_GC(mcx, mjso, jsoSynched);
		}
		SetPropToSynchedJSO(jsoSynched, currentProp.name.c_str());

#ifdef _DEBUG_SM3
		if(!mbHashEvaluating)
		{
			if(mlIsSynchTracesEnabled()){
				mlTraceSyncMode3(mcx, "  %s=%s\n", (char*)cLPCSTRq(oProp.msName.c_str()), (const char*)cLPCSTR(JSVal2String(mcx, vProp).c_str()));
			}
		}
#endif

		mlSynchData data;
		data.setJSContext(mcx);
		data << (long)vProp;
		currentProp.binValue.assign( (unsigned char*)data.data(), (unsigned char*)data.data() + data.size());
		aState.props.push_back(currentProp);
	}

	// чистим список свойств на синхронизацию
	mPropsToSynch.Clear();

#ifdef _DEBUG_SM3
	if(!mbHashEvaluating)
		mlTraceSyncMode3(mcx, "}\n");
#endif
	if(jsoSynched != NULL){
		FREE_FOR_GC(mcx, mjso, jsoSynched);
	}

	return true;
}


moIBinSynch* mlSynch::GetIBinSynch()
{
	moMedia* pmoMedia = mpParentElem->GetMO();
	if (PMO_IS_NULL(pmoMedia))
		return NULL;
	moIBinSynch* pmoBinSynch = pmoMedia->GetIBinSynch();
	return pmoBinSynch;
}

// форсировать отправку на сервер полного состояния
void mlSynch::ForceWholeStateSending(){
	nextSendingStateType = syncObjectState::ST_FULL;
	
	// ? возможно и не надо отсылать все системные синхронизируемые свойства
	SetChanged(MLSYNCH_FULL_STATE_MASK);

	if (mSSPs.miSet & MLSYNCH_BIN_STATE_CHANGED_MASK)
	{
		binStateForSending = mSSPs.mBinState;
	}
	// 
	JSIdArray* idArr = JS_Enumerate(mcx, mjso);
	for(int ij = 0; ij < idArr->length; ij++){
		jsval v;
		if(!JS_IdToValue(mcx,idArr->vector[ij],&v))
			continue;
		if(!JSVAL_IS_STRING(v))
			continue;
		JSString* jssPName = JSVAL_TO_STRING(v);
		jschar* pjcPName = JS_GetStringChars(jssPName);
		SetChanged((const wchar_t*)pjcPName);
	}
}

void mlSynch::SimulateFullStateCome(JSObject* jsoSynched){

	// устанавливаем все свойства
	JSIdArray* idArr = JS_Enumerate(mcx, mjso);
	for(int ij = 0; ij < idArr->length; ij++){
		jsval v;
		if(!JS_IdToValue(mcx,idArr->vector[ij],&v))
			continue;
		if(!JSVAL_IS_STRING(v))
			continue;
		JSString* jssPName = JSVAL_TO_STRING(v);
		jschar* pjcPName = JS_GetStringChars(jssPName);

		jsval jsvTrue = JSVAL_TRUE;
		JS_SetUCProperty(mcx, jsoSynched, pjcPName, -1, &jsvTrue);
	}

	mpParentElem->FireOnSynched(jsoSynched, true);
}

ESynchStateError mlSynch::SetSynchProps( const syncObjectState& aState, JSObject* &jsoSynched, bool& aSynchedEmpty)
{
	if( isPaused)
	{
		return SSE_NO_ERRORS;
	}
	if (mpParentElem->GetID() != OBJ_ID_UNDEF)
	{
		if( aState.uiID < 4000000000)
		{
			// все реджект стейты применяем без исключения
			if ( !aState.IsForceState())
			{
				// FullState применяем только если он больше или равен текущему
				if( aState.IsFullState())
				{
					if( !IsApplyableFullState( aState))
					{
						return SSE_OBSOLETE;
					}
				}
				else
				{
					// UpdateState применяем только если он больше на 1 чем наш
					if (aState.mwServerVersion != mwServerVersion + 1)
					{
						return SSE_REJECTED;
					}
				}
			}
		}
	}

	aSynchedEmpty = true;
	bool bDontTrace = false;
	if (mpParentElem->GetID() == OBJ_ID_UNDEF){
		mlTraceSyncMode3(mcx, "  child: (name: %s) :{\n", cLPCSTR( mpParentElem->GetName()));
	}else{
		// если пришло только серверное время или только координата Z, то не стоит выводить
		if(aState.props.size() == 0 && aState.sub_objects.size() == 0 
			&& ((aState.sys_props.miSet & ~(MLSYNCH_SERVERTIME_SET_MASK | MLSYNCH_COORDINATE_SET_MASK)) == 0)
		){
			bDontTrace = true;
		}else{
			unsigned int uiID = mpParentElem->GetID();
			unsigned int uiBornRealityID = mpParentElem->GetBornRealityID();
			mlTraceSyncMode3(mcx, "RECEIVED2 prop changes (objID: %u, bornRealityID: %u, name: %s) :{\n", uiID, uiBornRealityID, cLPCSTR( mpParentElem->GetName()));
		}
	}

	ESynchStateError error = SetSysSynchProps( aState.sys_props, bDontTrace);

	if (error != SSE_NO_ERRORS)
		return error;

	syncObjectPropVector::const_iterator cvi = aState.props.begin();
	for (;  cvi != aState.props.end();  cvi++)
	{
		const syncObjectProp& oProp = *cvi;
		SetPropToSynchedJSO(jsoSynched, oProp.name.c_str());
		SetSynchProp( oProp);
		aSynchedEmpty = false;
	}

	mwServerVersion = aState.mwServerVersion;
	if (aState.IsFullState())
		mwClientVersion = mwServerVersion;
	else if (mwServerVersion > mwClientVersion)
		mwClientVersion = mwServerVersion;

	return SSE_NO_ERRORS;
}

bool mlSynch::SetSynchProp( const syncObjectProp& aProp)
{
	if (aProp.name.size() == 0) 
		return false;


	mbSynchronizing = true;

	jsval vProp;
#if _MSC_VER >= 1600
	mlSynchData propData( aProp.binValue.begin()._Ptr, aProp.binValue.size());
#else
	mlSynchData propData( aProp.binValue.begin()._Myptr, aProp.binValue.size());
#endif
	propData.setJSContext(mcx);

//mlString wsAllocComment = L"mlSynch::SetSynchProp ";
//wsAllocComment += cLPWCSTR(aProp.name.c_str());
	GPSM(mcx)->SetAllocComment(L"mlSynch::SetSynchProp propData.readSafely(vProp)");

	if (!propData.readSafely(vProp)){	// читаем jsval с проверкой на успешность --nap
		GPSM(mcx)->SetAllocComment(L"");
		return false;
	}

	SAVE_VAL_FROM_GC(mcx, mjso, vProp);
	jsval jsvSavedFromGC = vProp;

	GPSM(mcx)->SetAllocComment(L"mlSynch::SetSynchProp");
	std::string sPropPath = aProp.name;
	if(mlIsSynchTracesEnabled()){
		mlTraceSyncMode3(mcx, "  %s=%s\n", sPropPath.c_str(), (const char*)cLPCSTR(JSVal2String(mcx, vProp).c_str()));
	}
	JSObject* jsoSynch = mjso;
	int iPropNameBegin = 0;
	for (;;)
	{
		std::string::size_type dotPos = sPropPath.find(L'.', iPropNameBegin);
		std::string sPropName;
		if (dotPos == std::string::npos)
		{
			sPropName = sPropPath.substr(iPropNameBegin);
			// то просто устанавливаем его значение и все
			mlSynch* pSynch = (mlSynch*)JS_GetPrivate(mcx, jsoSynch);
			if(pSynch != this)
				pSynch->mbSynchronizing = true;
			JS_SetProperty(mcx, jsoSynch, sPropName.c_str(), &vProp);
			if(pSynch != this)
				pSynch->mbSynchronizing = false;
			break;
		}

		// иначе берем нужный подобъект (если его нет, создаем)
		sPropName = sPropPath.substr(iPropNameBegin, dotPos - iPropNameBegin);
		jsval vSubObj = JSVAL_NULL;
		JS_GetProperty(mcx, jsoSynch, sPropName.c_str(), &vSubObj);
		// если объекта нет
		if (vSubObj == JSVAL_NULL || vSubObj == JSVAL_VOID)
		{
			// то создаем
			JSObject* jsoSubSynch = mlSynch::newJSObject(mcx);
			mlSynch* pSubSynch = (mlSynch*)JS_GetPrivate(mcx, jsoSubSynch);
			mlSynch* pSynch = (mlSynch*)JS_GetPrivate(mcx, jsoSynch);
			pSubSynch->SetParents(mpParentElem, pSynch);
			pSubSynch->msName = cLPWCSTRq(sPropName.c_str());
			jsoSynch = jsoSubSynch;
		}
		else
		{
			if(!JSVAL_IS_REAL_OBJECT(vSubObj)) // вообще-то такого не должно быть, но на всякий...
				break;
			jsoSynch = JSVAL_TO_OBJECT(vSubObj);
		}
		iPropNameBegin = dotPos + 1;
	}

	FREE_VAL_FOR_GC(mcx, mjso, jsvSavedFromGC);

	mbSynchronizing = false;
	GPSM(mcx)->SetAllocComment(L"");
	return true;
}

ESynchStateError mlSynch::SetSysSynchProps(const mlSysSynchProps& aSysProps, bool abDontTrace)
{
	int iAllowedSet;
	if (mpParentElem == NULL)
		iAllowedSet = 0;
	else if (ParentElementIsWithID())
		iAllowedSet = 0xFFFFFFFF;
	else
		iAllowedSet = MLSYNCH_BIN_STATE_CHANGED_MASK;

	int iSet = mSSPs.update(aSysProps, iAllowedSet);

	// если объект выпал из мира
	if ((iSet & MLSYNCH_CONNECTED_CHANGED_MASK) && !mSSPs.mbConnected)
	{
		// то надо его удалить 
		mlTraceSyncMode3(mcx, "  __connected=%d\n", mSSPs.mbConnected);
		CLogValue logValue( "SSE_NEED_ DELETE 2");
		GPSM(mcx)->GetContext()->mpLogWriter->WriteLnLPCSTR( logValue.GetData());
		return SSE_DELETE_DISCONECTED;
	}

	// TODO Надо определять локацию на сервере
	if (iSet & MLSYNCH_LOCATION_CHANGED_MASK)
	{
		mOutLocationID = 0;
		FireEvent(EVNM_onLocationChange);
	}

	if (iSet & MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK)
	{
		mOutCommunicationAreaID = 0;
		FireEvent(EVNM_onCommunicationAreaChange);
	}

	if(iSet & MLSYNCH_OWNER_CHANGED_MASK)
		FireEvent(EVNM_onOwnerChange);

	if(iSet & MLSYNCH_BIN_STATE_CHANGED_MASK)
	{
		ApplyBinStateUpdating(aSysProps.mBinState);
	}
	else if (iSet & MLSYNCH_FLAGS_SET_MASK)
	{
		moIBinSynch* pmoBinSynch = GetIBinSynch();
		if (pmoBinSynch != NULL)
			pmoBinSynch->UpdateState( 0, BSSCC_CLEAR, NULL, 0);
	}

	if (iSet & MLSYNCH_SERVERTIME_SET_MASK)
	{
		if(!abDontTrace)
			mlTraceSyncMode3(mcx, "  __serverTime=%d\n", mSSPs.serverTime);
		omsContext* pContext = GPSM(mcx)->GetContext();
		mSetPropLocalTime = pContext->mpApp->GetTimeStamp();
		jsval vEmpty;
		JS_GetProperty(mcx, mjso, "serverTime", &vEmpty);
	}

	if(iSet & MLSYNCH_PAUSE_LEVEL_CHANGED_MASK)
	{
		mlTraceSyncMode3(mcx, "  __pauseLevel=%u\n", aSysProps.muPauseLevel);
		if( mpParentElem != NULL)
		{
			if( aSysProps.muPauseLevel != 0)
				mpParentElem->Freeze();
			else
				mpParentElem->Unfreeze();
		}
	}

	if (iSet & MLSYNCH_REALITY_SET_MASK)
	{
		mlTraceSyncMode3(mcx, "  __reality=%d\n", aSysProps.muReality);
		omsContext* pContext = GPSM(mcx)->GetContext();

		// #876 Созданный в записи стул в динамической локации исчезает при проигрывании
		// Нужно удалять объект только в том случае, если его новая реальность не совпадает с реальностью, к которой я нахожусь
		if( pContext->mpMapMan->GetCurrentRealityID() != aSysProps.muReality)
		{
			// end of #876
			// Удаляем объект, перешедший в другую реальность
			CLogValue logValue( "SSE_NEED_ DELETE 3", pContext->mpMapMan->GetCurrentRealityID(), " ", aSysProps.muReality);
			GPSM(mcx)->GetContext()->mpLogWriter->WriteLnLPCSTR( logValue.GetData());

			return SSE_DELETE_CHANGED_REALITY_AVATAR;
		}
	}
	if (iSet & MLSYNCH_DELETETIME_CHANGED_MASK)
	{
		if(mlIsSynchTracesEnabled()){
			std::string sTime = FormatServerTime(mSSPs.mlDeleteTime);
			mlTraceSyncMode3(mcx, "  __deleteTime=%s\n", sTime.c_str());
		}
	}
	return SSE_NO_ERRORS;
}

// Перевод значения из серверного времени в UTC в миллисекундах
unsigned __int64 mlSynch::ServerTimeToLocalUTC(unsigned __int64 aulServerTime){
	__int64 lTZDelta = GetTimeZoneDelta();
	return (unsigned __int64)((__int64)aulServerTime - (__int64)62135596800000 + lTZDelta);
}

// форматирование серверного времени в читабельный вид
std::string mlSynch::FormatServerTime(unsigned __int64 aulServerTime){
	if(aulServerTime == 0)
		return "0";
	unsigned __int64 ulLocalTime = ServerTimeToLocalUTC(aulServerTime);
	time_t ltime = (time_t)(ulLocalTime / 1000);
	char timebuf[100];
	int res = ctime_s(timebuf, 100, &ltime);
	if(res != 0)
		return "(time format error)";
	std::string sTime = timebuf;
	std::string::size_type pos = 0;
	while((pos = sTime.find("\n",pos)) != std::string::npos){
		sTime.replace(pos, 1, "");
	}
	return sTime;
}

void mlSynch::ApplyBinStateUpdating( const syncObjectBinState& aBinStateUpdating)
{
	moMedia* pmoMedia = mpParentElem->GetMO();
	moIBinSynch* pmoBinSynch = NULL;
	if (!PMO_IS_NULL(pmoMedia))
		pmoBinSynch = pmoMedia->GetIBinSynch();

	bool bIsSet = false;
	if (pmoBinSynch != NULL)
	{
		BSSCCode iCode;
		unsigned int iVersion;
		BYTE* binStateData;
		int binStateDataSize;
		aBinStateUpdating.GetFullState(iCode, iVersion, binStateData, binStateDataSize);

		if (iCode == BSSCC_REPLACE_ALL)
			bIsSet = pmoBinSynch->SetFullState( iVersion, binStateData, binStateDataSize);
		else if (iCode == BSSCC_APPEND)
			bIsSet = pmoBinSynch->UpdateState( iVersion, BSSCC_APPEND, binStateData, binStateDataSize);
		else if (iCode == BSSCC_CLEAR)
			bIsSet = pmoBinSynch->UpdateState( iVersion, BSSCC_CLEAR, NULL, 0);
	}
}

void mlSynch::ApplyFullBinState()
{
	ApplyBinStateUpdating( mSSPs.mBinState);
}

JSBool mlSynch::JSFUNC_capture(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 0)
		JS_ReportWarning(cx, "synch.capture method has no arguments");
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	bool bRet = pThis->Capture();
	*rval = bRet?JSVAL_TRUE:JSVAL_FALSE; 
	return JS_TRUE;
}

bool mlSynch::Capture(){
	mlSynch* pSynch;
	if (!GetSynchOfParentElementWithID( pSynch))
		return false;
	if (pSynch != this)
		return pSynch->Capture();

	// если собственник уже установлен
	if(!mSSPs.msOwner.empty())
		return IsMyOwn();
	if(IsSOAObject())
		return false;
	if(IsOOSObject())
		return false;

	mSSPs.msOwner = GPSM(mcx)->GetClientStringID();
	if(mSSPs.msOwner.empty())
		return false;
	SetChanged(MLSYNCH_OWNER_CHANGED_MASK);
	return true; // получилось послать запрос на захват объекта (можно изменять свойства)
}

JSBool mlSynch::JSFUNC_free(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 0)
		JS_ReportWarning(cx, "synch.free method has no arguments");
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	bool bRet = pThis->Free();
	*rval = bRet?JSVAL_TRUE:JSVAL_FALSE; 
	return JS_TRUE;
}

bool mlSynch::Free(){
	mlSynch* pSynch;
	if (!GetSynchOfParentElementWithID( pSynch))
		return false;
	if (pSynch != this)
		return pSynch->Free();

	if(!IsMyOwn())
		return false;
	if(!mSSPs.msOwner.empty()){
		mSSPs.msOwner.clear();
		SetChanged(MLSYNCH_OWNER_CHANGED_MASK);
	}	
	
	return true;
}

JSBool mlSynch::JSFUNC_isMyOwn(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 0)
		JS_ReportWarning(cx, "synch.isMyOwn method has no arguments");
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	bool bRet = pThis->IsMyOwn();
	*rval = bRet?JSVAL_TRUE:JSVAL_FALSE; 
	return JS_TRUE;
}

bool mlSynch::IsMyOwn(){
	mlSynch* pSynch;
	if (!GetSynchOfParentElementWithID( pSynch))
		return false;
	if (pSynch != this)
		return pSynch->IsMyOwn();

	if(mSSPs.msOwner.empty())
		return false;
	if(isEqual(mSSPs.msOwner.c_str(), GPSM(mcx)->GetClientStringID()))
		return true;
	return false;
}

JSBool mlSynch::JSFUNC_getOwner(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	if(pThis->mSSPs.msOwner.empty())
		*rval = JSVAL_NULL;
	JSString* jssOwner = JS_NewUCStringCopyZ(cx, (const jschar*)pThis->mSSPs.msOwner.c_str());
	*rval = STRING_TO_JSVAL(jssOwner);
	return JS_TRUE;
}

JSBool mlSynch::JSFUNC_getLocation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	mlSynch* pSynch;
	if (!pThis->GetSynchOfParentElementWithID( pSynch))
	{
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}
	if(pSynch->mSSPs.msLocation.empty())
		*rval = JSVAL_NULL;
	JSString* jssLocation = JS_NewUCStringCopyZ(cx, (const jschar*)pSynch->mSSPs.msLocation.c_str());
	*rval = STRING_TO_JSVAL(jssLocation);
	return JS_TRUE;
}

JSBool mlSynch::JSFUNC_getCommunicationArea(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	mlSynch* pSynch;
	if (!pThis->GetSynchOfParentElementWithID( pSynch))
	{
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}
	if(pSynch->mSSPs.msCommunicationArea.empty())
		*rval = JSVAL_NULL;
	JSString* jssCommunicationArea = JS_NewUCStringCopyZ(cx, (const jschar*)pSynch->mSSPs.msCommunicationArea.c_str());
	*rval = STRING_TO_JSVAL(jssCommunicationArea);
	return JS_TRUE;
}

JSBool mlSynch::JSFUNC_isFree(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 0)
		JS_ReportWarning(cx, "synch.isFree method has no arguments");
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	bool bRet = pThis->IsFree();
	*rval = bRet?JSVAL_TRUE:JSVAL_FALSE; 
	return JS_TRUE;
}

JSBool mlSynch::JSFUNC_settingStart(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 0)
		JS_ReportWarning(cx, "synch.settingStart method has no arguments");
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	//mlTrace(cx,"JSFUNC_settingStart\n");
	// Выставляем флаг что состояние обрабатывается
	return JS_TRUE;
}	

JSBool mlSynch::JSFUNC_settingEnd(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 0)
		JS_ReportWarning(cx, "synch.settingEnd method has no arguments");
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	// Выставляем флаг что состояние обрабатывается
	if (GPSM(cx)->mbObjetcsUnfreezenAfterFreeze)
		GPSM(cx)->mbObjetcsFreezeUnfreezenObjects = true;
	mlTrace(cx,"JSFUNC_settingEnd\n");
	return JS_TRUE;
}

JSBool mlSynch::JSFUNC_isEmptyBinState(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	//JS_ReportWarning(cx, "isEmptyBinState::pThis->mSSPs.mBinState.GetSize()==%d"+pThis->mSSPs.mBinState.GetSize());
	if(pThis->mSSPs.mBinState.GetSize() > ML_EMPLT_BIN_STATE_MAX_SIZE)
		*rval = JSVAL_FALSE;
	else
		*rval = JSVAL_TRUE;	
	return JS_TRUE;
}

bool mlSynch::IsRecorded()
{
	mlSynch* pSynch;
	if (!GetSynchOfParentElementWithID( pSynch))
		return false;
	if (pSynch != this)
		return pSynch->IsRecorded();

	if( IsFree())
		return false;

	if(mSSPs.msOwner == L"@@server")
		return true;

	return false;
}

bool mlSynch::IsFree(){
	mlSynch* pSynch;
	if (!GetSynchOfParentElementWithID( pSynch))
		return false;
	if (pSynch != this)
		return pSynch->IsFree();

	if(mSSPs.miFlags & MLSYNCH_SERVER_OWNER_ASSIGNATION_MASK)
		return false;
	if(mSSPs.msOwner.empty())
		return true;
	
	static int isSuperAdmin = -1;
	if (-1 == isSuperAdmin)
	{
		mlSuperUser superUserChecker;		
		isSuperAdmin = (int)superUserChecker.IsCurrentUserSuperUser(GPSM(mcx)->GetClientStringID());
	}

	return false;
}

/** Возвращает системные разрешения на изменения данного объекта кем - то.
\param JSContext * cx -
\param JSObject * obj -
\param uintN argc -
\param jsval * argv -
\param jsval * rval -
\return int
0 - может править;
1 - не может править, потому что занят;
2 - не может править, потому что записан;
3 - не может править, потому что это объект не вошедший в запись (например экран вне локации);
\details
*/
JSBool mlSynch::JSFUNC_mayChange(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 0)
		JS_ReportWarning(cx, "synch.mayChange method has no arguments");
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	int iRet = pThis->MayChange();
	*rval = INT_TO_JSVAL( iRet);
	return JS_TRUE;
}

int mlSynch::MayChange()
{
	if (mpParentElem == NULL)
		return MCERROR_NOERROR;

	mlSynch* pParentSynch = NULL;
	if (!GetSynchOfParentElementWithID( pParentSynch))
	{
		//assert( false);
		//return MCERROR_WRONGSYNCOBJECT;
		// ругался при размещении сидячих мест у пользовательского объекта, когда создавали копию аватара, которого нет на сервере
		// Tandy разрешил убрать assert и вернуть  MCERROR_NOERROR,
		// так что на его совести)
		return MCERROR_NOERROR;
	}

	if (pParentSynch != this)
		return pParentSynch->MayChange();

	// если собственник, то точно можно
	if( IsMyOwn())
		return MCERROR_NOERROR;

	// если это записанный объект, то нельзя
	if( IsRecorded())
		return MCERROR_OBJECTISRECORDED;

	// если не свободен
	if( !IsFree())
		return MCERROR_OBJECTISCAPTURED;

	// если по флагам разрешено менять только хозяину ( которым я уже не являюсь)
	if(IsOOSObject())
		return MCERROR_OBJECTISCAPTURED;

	return MCERROR_NOERROR;
}

mlSynch* mlSynch::GetTopParent(){
	mlSynch* pParent = this;
	while(pParent->mpParent != NULL)
		pParent = pParent->mpParent;
	return pParent;
}

void mlSynch::ZoneChanged(int aiZone)
{
	// если номер зоны действительно сменился и я могу менять свойства у этого объекта (собственник или он свободен)
	if (aiZone != mSSPs.miZone && (MayChange()==MCERROR_NOERROR))
	{
		// если зона уже была установлена и после этого сменилась, то уведомить об этом сервер
		SetChanged(MLSYNCH_ZONE_CHANGED_MASK);
		mSSPs.miZone = aiZone;
	}
}

void mlSynch::RealityChanged(unsigned int auReality)
{
	// если номер реальности действительно сменился и я могу менять свойства у этого объекта (собственник или он свободен)
	if(auReality != mSSPs.muReality && (MayChange()==MCERROR_NOERROR)){
		// если зона уже была установлена и после этого сменилась, то уведомить об этом сервер
		SetChanged(MLSYNCH_REALITY_SET_MASK);
		mSSPs.muReality = auReality;
	}
}

void mlSynch::SetSynchFlags(unsigned int auiFlags){
	mSSPs.miFlags |= (int)auiFlags;
	SetChanged(MLSYNCH_FLAGS_SET_MASK);
}

unsigned int mlSynch::GetSynchFlags(){
	return mSSPs.miFlags;
}

float mlSynch::GetZ()
{
	return mSSPs.mfCoordinateZ;
}

float mlSynch::GetX()
{
	return mSSPs.mfCoordinateX;
}
float mlSynch::GetY()
{
	return mSSPs.mfCoordinateY;
}

// пришло значение z аватара другого пользователя и надо его установить
bool mlSynch::NeedToSetZ(){
	if(IsMyOwn())
		return false;
	if(mSSPs.mfCoordinateZ == MLSYNCH_UDEF_Z)
		return false;
	return true;
}

void mlSynch::SetZ(float aZ)
{
	if (MayChange()!=MCERROR_NOERROR)
		return;
	mSSPs.SetCoordinateZ( aZ);
	SetChanged(MLSYNCH_COORDINATE_SET_MASK);
}

void mlSynch::SetXY(float aX, float aY)
{
	if (MayChange()!=MCERROR_NOERROR)
		return;
	if (mSSPs.SetCoordinateXY( aX, aY))
		SetChanged(MLSYNCH_COORDINATE_XY_SET_MASK);
}

JSBool mlSynch::JSFUNC_remove(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_FALSE;
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	oms::omsContext* pContext = GPSM(cx)->GetContext();
	if(pContext->mpMapMan == NULL)
		return JS_TRUE;
	bool bRet = pContext->mpMapMan->Remove(pThis->mpParentElem);
	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

// посылает сообщение клиенту-собственнику объекта
JSBool mlSynch::JSFUNC_sendMessage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_FALSE;
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	ML_ASSERTION(cx, (pThis->mpParentElem != NULL), "mlSynch::JSFUNC_sendMessage")
	unsigned int uiObjID = pThis->mpParentElem->GetID();
	unsigned int uiBornRealityID = pThis->mpParentElem->GetBornRealityID();
	if(uiObjID == OBJ_ID_UNDEF){
		JS_ReportError(cx, "Object ID is not valid. Message is not sent.");
		return JS_TRUE;
	}
	// перебираем аргументы и помещаем их в данные
	mlSynchData oData;
	// (! надо проверить, нормально ли сохраняются подобъекты и ссылки на объекты)
	oData << argc;
	for(int ii = 0; ii < (int)argc; ii++){
		oData << (long)argv[ii];
	}
	oms::omsContext* pContext = GPSM(cx)->GetContext();
	if(pContext->mpSyncMan == NULL)
		return JS_TRUE;
	bool bRet = true; pContext->mpSyncMan->SendMessage(uiObjID, uiBornRealityID, (unsigned char*)oData.data(), oData.size());
	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

// возвращает время, когда объект будет удален сервером
//  возвращает null, если время не задано
JSBool mlSynch::JSFUNC_getDeleteTime(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;	// не задано
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	if(pThis->mSSPs.mlDeleteTime == 0)
		return JS_TRUE;
	wchar_t wsScript[255];
	swprintf_s(wsScript,255,L"new Date(%u000)", (unsigned long)(pThis->ServerTimeToLocalUTC(pThis->mSSPs.mlDeleteTime) / 1000));
	jsval val = JSVAL_NULL; 
	JSBool bR = JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), (jschar*)wsScript, wcslen(wsScript), "getDeleteTime", 1, &val);
	if (JSVAL_IS_REAL_OBJECT(val))
		*rval = val;
	return JS_TRUE;
}

// Возвращает разницу временных зон между локальным компьютером и сервером
__int64 mlSynch::GetTimeZoneDelta(){
	omsContext* pContext = GPSM(mcx)->GetContext();
	time_t lTime;	// в секундах (UTC)
	time(&lTime);	
	unsigned __int64 ulLocalTime = ((unsigned __int64)lTime * (unsigned __int64)1000);	// в миллисекундах
	ulLocalTime += 62135596800000; // UTC -> abs time
	unsigned __int64 ulServerTime = GetServerTime();
	__int64 lTZDelta = ((__int64)ulLocalTime - (__int64)ulServerTime);
	lTZDelta /= 5*60*1000; lTZDelta *= 5*60*1000; // округляем до 5 минутнх интервалов
	return lTZDelta;
}

JSBool mlSynch::JSFUNC_pause(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) 
{
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	pThis->isPaused = true;
	*rval = JSVAL_TRUE;	// не получилось
	return JS_TRUE;
}

JSBool mlSynch::JSFUNC_resume(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) 
{
	mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
	pThis->isPaused = false;
	*rval = JSVAL_TRUE;	// не получилось
	return JS_TRUE;
}

// задать время, когда серверу надо будет удалить объект
JSBool mlSynch::JSFUNC_setDeleteTime(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_FALSE;	// не получилось
	unsigned __int64 ulNewDeleteTime = 0; 
	if(argc > 0){
		jsval jsvArg = argv[0];
		if(JSVAL_IS_REAL_OBJECT(jsvArg)){
			JSObject* jsoDate = JSVAL_TO_OBJECT(jsvArg);
			jsval vFunc = JSVAL_NULL;
			JS_GetProperty(cx, jsoDate, "getTime", &vFunc);
			if(vFunc == JSVAL_VOID || !JSVAL_IS_OBJECT(vFunc) || !JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(vFunc))){
				mlTraceWarning(cx, "Argument of setDeleteTime method must be a Date object");
				return JS_TRUE;
			}
			jsval jsvRet = JSVAL_VOID;
			JS_CallFunctionValue(cx, jsoDate, vFunc, 0, NULL, &jsvRet);
			mlSynch* pThis = (mlSynch*)JS_GetPrivate(cx,obj);
			if(JSVAL_IS_NUMBER(jsvRet)){
				jsdouble jsdRet;
				JS_ValueToNumber(cx, jsvRet, &jsdRet);
				unsigned __int64 ulDeleteTime = (unsigned __int64)jsdRet; // iDeleteTime - в миллисекундах (1/1000 секунды)
				bool bRet = pThis->SetDeleteTime(ulDeleteTime);
				*rval = BOOLEAN_TO_JSVAL(bRet);
			}
		}else{
			mlTraceWarning(cx, "Argument of setDeleteTime method must be a Date object");
			return JS_TRUE;
		}
	}
	return JS_TRUE;
}

bool mlSynch::SetDeleteTime(unsigned __int64 aulDeleteTime){
	if(MayChange()!=MCERROR_NOERROR)
		return false;
	unsigned __int64 ulServerTime = GetServerTime();
	// Если aulDeleteTime > 20 лет, то переводим его во временную зону serverTime
	if(aulDeleteTime > (unsigned __int64)20*365*24*60*60*1000){
		aulDeleteTime += 62135596800000; // UTC -> abs time
		// приводим временную зону aulDeleteTime к серверной
		__int64 lTZDelta = GetTimeZoneDelta();
		aulDeleteTime -= lTZDelta;
	}
	// Если aulDeleteTime > serverTime, то устанавливаем как есть
	if(aulDeleteTime > ulServerTime){
	}else{
		// Если aulDeleteTime > 20 лет, то не устанавливаем
		if(aulDeleteTime > (unsigned __int64)20*365*24*60*60*1000)
			return false;
		// иначе добавляем к aulDeleteTime serverTime
		aulDeleteTime = ulServerTime + aulDeleteTime; // (и то и другое тут должно быть в миллисекундах)
	}
	mSSPs.mlDeleteTime = aulDeleteTime;
	SetChanged(MLSYNCH_DELETETIME_CHANGED_MASK);
	return true;
}

// с сервера пришло сообщение от другого клиента
void mlSynch::HandleSynchMessage(const wchar_t* apwcSender, mlSynchData& aData){
	JSObject* jsoEvent = GPSM(mcx)->GetEventGO();
	jsval vSender = STRING_TO_JSVAL(JS_NewUCStringCopyZ(mcx, (const jschar*)apwcSender));
	JS_SetProperty(mcx, jsoEvent, "sender", &vSender);
	unsigned int argc = 0;
	aData >> argc;
	jsval* avArgs = MP_NEW_ARR( jsval, argc + 1);
	for(int ii = 0; ii < (int)argc; ii++){
		jsval vArg = JSVAL_NULL;
		long lArg; aData >> lArg; vArg = (jsval)lArg;
		avArgs[ii] = vArg;
	}
	jsval vArgs = OBJECT_TO_JSVAL(JS_NewArrayObject(mcx, argc, avArgs));
	JS_SetProperty(mcx, jsoEvent, "args", &vArgs);
	 MP_DELETE_ARR( avArgs);
	FireEvent(EVNM_onMessage);
	JS_DeleteProperty(mcx, jsoEvent, "sender");
	JS_DeleteProperty(mcx, jsoEvent, "args");
}

void mlSynch::CallEventFunction(const wchar_t* apwcEventName){
	// если есть свойство onXXX
	jsval vFunc = JSVAL_NULL;
	mlString sEventFunc = L"synch_";
	sEventFunc += apwcEventName;
	if(JS_GetUCProperty(mcx, mpParentElem->mjso, (const jschar*)sEventFunc.c_str(), sEventFunc.length(), &vFunc) == JS_FALSE)
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

void mlSynch::FireEvent(const wchar_t* apwcEventName){
	v_elems* pvSynchedChildren = GetSynchedChildren();
	if(pvSynchedChildren != NULL && pvSynchedChildren != ML_SYNCH_CHILDREN_EMPTY_VAL){
		v_elems::iterator vi;
		for(vi = pvSynchedChildren->begin(); vi != pvSynchedChildren->end(); vi++){
			mlRMMLElement* pSynchChild = *vi;
			pSynchChild->mpSynch->CallEventFunction(apwcEventName);
			pSynchChild->mpSynch->CallListeners2(apwcEventName);
		}
	}

	CallEventFunction(apwcEventName);
	CallListeners2(apwcEventName);
}

void mlSynch::FireOnInitialized(){
	// пока будем его генерировать только у synch-а верхнего уровня
	FireEvent(EVNM_onInitialized);
}

void mlSynch::FireOnReset(){
	// пока будем его генерировать только у synch-а верхнего уровня
	// вызываем только после загрузки объекта
	if (mpParentElem == NULL)
		return;

	if (mbWaitForLoading)
		return;

	FireEvent(EVNM_onReset);
}

bool mlSynch::GetParentElementWithID(mlRMMLElement*& apParentElement)
{
	if (mpParentElem == NULL)
		return false;

	apParentElement = mpParentElem;
	for(; apParentElement != NULL; apParentElement = apParentElement->GetParent()){
		if(apParentElement->GetID() != OBJ_ID_UNDEF)
			break;
	}
	if( apParentElement == NULL)
		return false;
	if (apParentElement->mpSynch == NULL)
		return false;
	return true;
}

bool mlSynch::ParentElementIsWithID()
{
	if (mpParentElem == NULL)
		return false;
	if (mpParentElem->GetID() == OBJ_ID_UNDEF)
		return false;
	return true;
}

bool mlSynch::GetSynchOfParentElementWithID(mlSynch*& apSynch)
{
	mlRMMLElement* pParentElement;
	if (!GetParentElementWithID( pParentElement))
		return false;
	if (pParentElement->mpSynch == NULL)
		return false;
	apSynch = pParentElement->mpSynch;
	return true;
}

bool mlSynch::StateChanged( unsigned int auStateVersion, BSSCCode aCode, void* apData, unsigned long aulSize, 
							unsigned long aulOffset, unsigned long aulBlockSize)
{
	if(MayChange()!=MCERROR_NOERROR)
		return false;
	if (aCode == BSSCC_CLEAR)
	{
		mSSPs.SetBinState( BSSCC_REPLACE_ALL, auStateVersion, NULL, 0);
		binStateForSending.ApllyUpdating(BSSCC_REPLACE_ALL, auStateVersion, NULL, 0);
	}
	else
	{
		mSSPs.SetBinState( aCode, auStateVersion, (BYTE*)apData, aulSize);
		binStateForSending.ApllyUpdating(aCode, auStateVersion, (BYTE*)apData, aulSize);
	}
	SetChanged(MLSYNCH_BIN_STATE_CHANGED_MASK);
	return true; 
}

// 3D-объект сместился и получена его локация (возможно она и не изменилась)
void mlSynch::SetLocationSID(const wchar_t* pszLocationID){
	if(MayChange()!=MCERROR_NOERROR)
		return;

	/*if((int)pszLocationID == mOutLocationID)
		return;*/
	mOutLocationID = (int)pszLocationID;
	// если идентификатор изменился,
//if(mpParentElem->GetID() == 15)
//int hh=0;
	if(pszLocationID != NULL){	
		//if(!mSSPs.msLocation.compare( pszLocationID))
		if(isEqual(mSSPs.msLocation, pszLocationID))
			return;
		mSSPs.msLocation = pszLocationID;
		mSSPs.miSet |= MLSYNCH_LOCATION_CHANGED_MASK;
	}else{
		if(*(mSSPs.msLocation.c_str()) == L'\0')
			return;
		mSSPs.msLocation = L"";
	}
	// то установить флаг для последующей отправки на сервер
	SetChanged(MLSYNCH_LOCATION_CHANGED_MASK);
}

void mlSynch::SetCommunicationAreaSID(const wchar_t* pszCommunicationAreaID){
	if(MayChange()!=MCERROR_NOERROR)
		return;

	/*if((int)pszCommunicationAreaID == mOutCommunicationAreaID)
		return;
	mOutCommunicationAreaID = (int)pszCommunicationAreaID;*/
	// если идентификатор изменился,
	//if(mpParentElem->GetID() == 15)
	//int hh=0;
	if(pszCommunicationAreaID != NULL){
		if(isEqual(mSSPs.msCommunicationArea, pszCommunicationAreaID))
			return;
		mSSPs.msCommunicationArea = pszCommunicationAreaID;
		mSSPs.miSet |= MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK;
	}else{
		if(*(mSSPs.msCommunicationArea.c_str()) == L'\0')
			return;
		mSSPs.msCommunicationArea = L"";
	}
	// то установить флаг для последующей отправки на сервер
	SetChanged(MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK);
}

// Исправление #621
void AddNewLine(mlString& sRetVal, wchar_t* aEndOfLine = NULL)
{
	if (aEndOfLine != NULL)
		sRetVal += aEndOfLine;
	sRetVal += L"\r\n";
}

void AddIndent(mlString& sRetVal, int iIndent, const wchar_t* sIndentStr)
{
	for (int i = 0;  i < iIndent;  i++)
		sRetVal += sIndentStr;
}

mlString mlSynch::GetSysPropertiesJson( int aIndent, const wchar_t* aIdentStr)
{
	wchar_t strBuffer[ 100];

	mlString strSysProps = L"{";

	bool added = false;
	if (mpParentElem->GetID() != OBJ_ID_UNDEF)
	{
		added = true;

		AddNewLine( strSysProps);
		aIndent++;

		AddIndent( strSysProps, aIndent, aIdentStr);
		strSysProps += L"flags: ";
		if ((mSSPs.miFlags & MLSYNCH_SERVER_OWNER_ASSIGNATION_MASK) != 0)
			strSysProps += L"SERVER_OWNER_ASSIGNATION | ";
		if ((mSSPs.miFlags & MLSYNCH_OWNER_ONLY_SYNCHR_MASK) != 0)
			strSysProps += L"OWNER_ONLY_SYNCHR | ";
		if ((mSSPs.miFlags & MLSYNCH_EXCLUSIVE_OWN_MASK) != 0)
			strSysProps += L"EXCLUSIVE_OWN | ";
		if (mSSPs.miFlags != 0)
			strSysProps.erase( strSysProps.size() - 3, 3);
		AddNewLine( strSysProps, L",");

		AddIndent( strSysProps, aIndent, aIdentStr);
		strSysProps += L"owner: '";
		strSysProps += mSSPs.msOwner;
		AddNewLine( strSysProps, L"',");

		AddIndent( strSysProps, aIndent, aIdentStr);
		swprintf_s( strBuffer, 100, L"x: %4.2f, y: %4.2f, z: %4.2f", mSSPs.mfCoordinateX, mSSPs.mfCoordinateY, mSSPs.mfCoordinateZ);
		strSysProps += strBuffer;
		AddNewLine( strSysProps, L",");

		AddIndent( strSysProps, aIndent, aIdentStr);
		strSysProps += L"location: '";
		strSysProps += mSSPs.msLocation;
		AddNewLine( strSysProps, L"',");

		AddIndent( strSysProps, aIndent, aIdentStr);
		swprintf_s( strBuffer, 100, L"zone: %u", mSSPs.miZone);
		strSysProps += strBuffer;
		AddNewLine( strSysProps, L",");

		AddIndent( strSysProps, aIndent, aIdentStr);
		swprintf_s( strBuffer, 100, L"reality: %u", mSSPs.muReality);
		strSysProps += strBuffer;
		AddNewLine( strSysProps, L",");

		AddIndent( strSysProps, aIndent, aIdentStr);
		strSysProps += L"communicationArea: '";
		strSysProps += mSSPs.msCommunicationArea;
		strSysProps += L"'";
	}

	if (mSSPs.miSet & MLSYNCH_BIN_STATE_CHANGED_MASK)
	{
		if (added)
			AddNewLine( strSysProps, L",");
		else
			AddNewLine( strSysProps);
		AddIndent( strSysProps, aIndent, aIdentStr);
		swprintf_s( strBuffer, 100, L"binState: { version: %u, size: %u, checksum: %u}", mSSPs.mBinState.GetVersion(), mSSPs.mBinState.GetSize(), mSSPs.mBinState.GetChecksum());
		strSysProps += strBuffer;
	}

	aIndent--;
	if (added)
	{
		AddNewLine( strSysProps);
		AddIndent( strSysProps, aIndent, aIdentStr);
	}
	strSysProps += L"}";

	return strSysProps;
}
// Конец #621

bool mlSynch::IsApplyableFullState( const syncObjectState& aState)
{
	// если пришла версия 0 и у нас 0, или пришла версия больше нашей то это норм.
	if( mwServerVersion >= aState.mwServerVersion)
	{
		if( aState.mwServerVersion != 0)
			return false;
	}
	return true;
}

void mlSynch::BeginHashEvaluating()
{
	mbHashEvaluating = true;

	mPushPropsToSynch.Clear();
	mPushPropsToSynch = mPropsToSynch;

	if( mpParentElem != NULL && mpParentElem->GetID() == 1000016413)
	{
		int ii =0;
	}

	mPushMiChanged = miChanged;
	mPushNextSendingStateType = nextSendingStateType;

	ForceWholeStateSending();

	// запомним синк состояния дочерних элементов
	v_elems* pvSynchedChildren = GetSynchedChildren();
	if(pvSynchedChildren != NULL && pvSynchedChildren != ML_SYNCH_CHILDREN_EMPTY_VAL){
		v_elems::iterator vi;
		for(vi = pvSynchedChildren->begin(); vi != pvSynchedChildren->end(); vi++){
			mlRMMLElement* pSynchChild = *vi;
			pSynchChild->mpSynch->BeginHashEvaluating();
		}
	}
}

void mlSynch::EndHashEvaluating()
{
	mPropsToSynch.Clear();
	mPropsToSynch = mPushPropsToSynch;

	miChanged = mPushMiChanged;
	nextSendingStateType = mPushNextSendingStateType;

	// восстановим синк состояния дочерних элементов
	v_elems* pvSynchedChildren = GetSynchedChildren();
	if(pvSynchedChildren != NULL && pvSynchedChildren != ML_SYNCH_CHILDREN_EMPTY_VAL){
		v_elems::iterator vi;
		for(vi = pvSynchedChildren->begin(); vi != pvSynchedChildren->end(); vi++){
			mlRMMLElement* pSynchChild = *vi;
			pSynchChild->mpSynch->EndHashEvaluating();
		}
	}

	mbHashEvaluating = false;
}

void mlSynch::ForceVersions()
{
	mwClientVersion++;
	mwServerVersion = mwClientVersion;
}

bool mlSynch::GetBinState( BYTE*& aBinState, int& aBinStateSize)
{
	if (mSSPs.miSet & MLSYNCH_BIN_STATE_CHANGED_MASK)
	{
		BSSCCode code = BSSCC_REPLACE_ALL;
		unsigned int version = 0;
		mSSPs.mBinState.GetFullState( code, version, aBinState, aBinStateSize);		
		return true;
	}
	return false;
}
void mlSynch::SetWaitForLoadingStatus()
{
	mbWaitForLoading = true;
}

bool mlSynch::IsWaitForLoading()
{
	return mbWaitForLoading;
}

void mlSynch::ResetWaitForLoadingStatus()
{
	mbWaitForLoading = false;
}

}
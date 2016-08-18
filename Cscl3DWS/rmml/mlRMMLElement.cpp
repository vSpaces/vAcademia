#include "mlRMML.h"
#include "config/oms_config.h"
#include "ILogWriter.h"

#include <sstream>
#include "mlSPParser.h"
#include "config/prolog.h"

namespace rmml {

#define SYNCHPROPERTIESN L"synchProperties" 
	
mlSynchedObjProps::mlSynchedObjProps():
	MP_VECTOR_INIT(vProps),
	MP_STRING_INIT(sSubobjPath)
{
}

mlSynchedObjProps::mlSynchedObjProps(const mlSynchedObjProps& other):
	MP_VECTOR_INIT(vProps),
	MP_STRING_INIT(sSubobjPath)
{
	uObjID = other.uObjID;
	uBornRealityID = other.uBornRealityID; // идентификатор реальности рождения
	sSubobjPath = other.sSubobjPath;	// путь к подобъекту
	oSysProps = other.oSysProps;
	vProps = other.vProps;
}

mlRMMLElement::mlRMMLElement()
{
	mcx = NULL;
	mjso = NULL;
	mRMMLType=MLMT_UNDEFINED;
	mpvNonJSOChildren=NULL;
	//
	_name=NULL;
	_frozen=false;
	//
	mpClass=NULL;
	mpParent=NULL;
	//
	mbBtnEvListenersIsSet=false;
	//
	miSrcFilesIdx = -1;
	mpSrcFilePath = NULL;
	miSrcLine = -1;
	//
	mchSynchronized = -1;
	muID = OBJ_ID_UNDEF;
	mucSynchFlags4Events = 0;
	mbMayHaveSynchChildren = false;
	mpSynch = NULL;
	mpCreationCallback = NULL;
	//
	muRealityID = 0;
	muBornRealityID = 0;
	muID = 0xFFFFFFFF;

	lLastUpdateTime = 0;

	_updateBinState = false;

	mpChildren = NULL;

	muLastEvaluatedHash = 0;
	mbNeedHashEvaluating = true;

	#ifdef ML_TRACE_ALL_ELEMS
	ElementPrecreated(this);
	#endif
}

void mlRMMLElement::SetJSContext(JSContext* cx){
	mcx = cx;
	// если в режиме редактирования или паузы, то замораживаем
	if(GPSM(mcx)->GetMode() == smmAuthoring || GPSM(mcx)->GetMode() == smmPause)
		_frozen=true;
	GPSM(mcx)->ElementCreating(this);
}

// http://www.cscl69.ru/projects/vks/projects/physics/index.phtml?sessionID=43&groupID=5&roleID=rlMedLadybird
// http://www.csclbd.ru/projects/vks/projects/physics/player.phtml?recID=1
// http://www.csclbd.ru/projects/vks/php/log_clear.php?sessionID=1
// http://www.csclbd.ru/projects/vks/projects/physics/index.phtml?sessionID=1
/* 
	Режим синхронизации mchSynchronized == 1 (полный):
	1) синхронизируются все свойства
	2) синхронизируются все методы
	3) синхронизируются все события

	Режим синхронизации mchSynchronized == 2 (выборочный):
	1) задается установкой у объекта свойства synch в строковое значение, отличное от "true" и "false"
	2) синхронизируются свойства, входящие в список, хранящийся в строковом свойстве объекта synch
	3) синхронизируются методы, у объекта которых есть свойство synch.
	   Свойства объекта, перечисленные в synch синхронизируются вместе с вызовом метода.
	4) события не синхронизируются
	5) для каждого события, порождаемого длительной операцией, запоминается, была запущена длительная 
	   операция в цепочке вызовов 1 или 0, и от этого зависит, синхронизировать вызовы методов или нет (цепочка продолжается)
	   (GPSM(mcx)->mbDontSendSynchEvents, mlRMMLElement::mucSynchFlags4Events)
	// 4) методы, вызываемые из обработчиков событий, которые объявлены в элементах с mchSynchronized == 2
	//   считаются вызовом типа (1 -> 1) и поэтому не синхронизируются
	6) несинхронизируемые методы объекта с mchSynchronized == 2 считаются "серой областью". 
	   Эти методы вызываются всегда (1 -> 1, 0 -> 0 и, соответственно, не синхронизируются никогда).
	7) в режиме инициализации вызовы 1 -> 0 не выполняются. Состояние всей несинхронизируемой области 
	   надо восстанавливать по состоянию синхронизируемой сразу после выхода из режима инициализации
	8)!!! не реализовано еще прерывание длительных операций (из RM-а надо давать соответствующую нотификацию в SM)
	   (???не понятно как синхронизировать прерывание длительных операций)
		Специальные идентификаторы в списке свойств:
		this - синхронизировать медиа-объект и все свойства объекта
		all - синхронизировать все свойства объекта
		media - синхронизировать медиа-объект 

	Режим синхронизации mchSynchronized == 3 (по состояниям):
	1) задается установокй у объекта атрибута/свойства synch в строковое значение инициализации объекта synch
		(присутствуют знаки '=')
	2) создается специальное свойство-подобъект synch, которое инициализируется выполнением строки инициализации.
		Все подобъекты типа Object пересоздаются в подобъекты типа Synch с теми свойствами и подобъектами (тоже в Synch)
	3) все изменения свойств подобъекта Synch сохраняются до получения уведомления о том, что они отправлены на сервер.
	4) на изменения синхрогизируемых свойств можно "повесить" watch
	5) при инициализации состояния объекта по данным с сервера генерируется событие onSynched
	6) при изменении синхронизируемых свойств с сервера будет рассылаться watch (пункт 4) и событие onSynched, при 
		котором в объекте Event будет установлено свойство synched со свойствами, которые изменились (с неопределенным 
		значением)
*/
/*
Cобытия, порождаемые длительной операцией:
mlRMMLLoadable::EVID_onLoad (0x80) 7-й бит
mlRMMLContinuous::EVID_onEnterFrame (0x40) 6
mlRMMLContinuous::EVID_onPlayed (0x20) 5
mlRMML3DObject::EVID_onPassed (0x40) 6
mlRMMLCharacter::EVID_onTurned (0x01) 0
mlRMMLCharacter::EVID_onSaid (0x02) 1
mlRMMLMotion::EVID_onSet (0x01) 0
mlRMMLMotion::EVID_onDone (0x02) 1
mlRMMLSpeech::EVID_onSaid (0x01) 0
*/

mlRMMLElement::~mlRMMLElement(void)
{
//wchar_t* pwcNm = _name?JS_GetStringChars(_name):L"??";
//mlTrace(mcx, "~mlRMMLElement:%s\n", cLPCSTR(pwcNm));
//if(pwcNm != NULL && isEqual(pwcNm, L"seqLevel"))
//int hh=0;
//if(pwcNm != NULL && isEqual(pwcNm, L"voiceLevel"))
//int hh=0;
	mpSynch = NULL; // возможно GC уже успел его убить
	// уведомляем parent-а о своей смерти
	if(IsJSO() && mpParent != NULL)
		mpParent->ChildIsDestroying(this);
	//
	UnlinkFromEventSenders(false);
	if(mpChildren != NULL){
		v_elems::iterator vi;
		for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
			mlRMMLElement* pChild = *vi;
			v_elems::iterator vi;
			if(pChild != NULL && !(pChild->IsJSO()) && mpvNonJSOChildren != NULL){
				for(vi = mpvNonJSOChildren->begin(); vi != mpvNonJSOChildren->end(); vi++){
					if(*vi == pChild){
						pChild->destroy();
						pChild = NULL;
						break;
					}
				}
			}
//if(pChild != NULL && pChild->mpParent != this)
//int hh=0;
			if(pChild != NULL && pChild->mpParent == this)
				pChild->mpParent = NULL;
		}
		mpChildren->clear();
		MP_DELETE(mpChildren);
	}
	if(mpvNonJSOChildren!=NULL)
		 MP_DELETE( mpvNonJSOChildren);
	//
	GPSM(mcx)->ElementDestroying(this);
}

// Когда объект исключается из иерархии сцены (parent=null), то надо, чтобы он перестал обрабатывать и события
void mlRMMLElement::UnlinkFromEventSenders(bool abWithChildren){
	if(mpListeners.get()!=NULL){
		mpListeners->NotifySenderDestroyed(this);
		mpListeners->RemoveAllListeners();
	}
	GPSM(mcx)->RemoveEventListener(this);
	if(IsButton())
		GPSM(mcx)->RemoveFromModalButList(this, true);
	//
	v_elems::iterator vi;
/*	if(mpvNonJSOChildren != NULL){
		for(vi = mpvNonJSOChildren->begin(); vi != mpvNonJSOChildren->end(); vi++){
			mlRMMLElement* pNonJSOChild = *vi;
			if(pNonJSOChild != NULL){
				pNonJSOChild->UnlinkFromEventSenders();
			}
		}
	}
*/
	if(!abWithChildren)
		return;
	GPSM(mcx)->ResetMouse(this);
	if(mpChildren == NULL || mpChildren->empty())
		return;
	for(vi = mpChildren->begin(); vi != mpChildren->end(); vi++){
		mlRMMLElement* pChild = *vi;
		pChild->UnlinkFromEventSenders();
	}
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLElement::_JSPropertySpec[] = {
	{ MLELPN_NAME, JSPROP__name, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0},
	JSPROPA_RW(name, _name)
	{ MLELPN_CLASS, JSPROP__class, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0}, // JSPROP_READONLY
	JSPROPA_RW(class, _class)
	{ MLELPN_PARENT, JSPROP__parent, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0},
	JSPROPA_RW(parent, _parent)
	JSPROP_RW(_synch)
	JSPROPA_RW(synch, _synch)
	JSPROP_RW(_frozen)
	JSPROPA_RW(frozen, _frozen)
	JSPROP_RO(_target)
	JSPROPA_RO(target, _target)
	JSPROP_RW(userEvents)
	JSPROP_RW(scriptProviders)

	JSPROP_EV(onSynched)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLElement::_JSFunctionSpec[] = {
	JSFUNC(duplicate,1)
	JSFUNC(synchronize,1)
	JSFUNC(fireEvent,1)
	JSFUNC(getSubclass,1)
	JSFUNC(getChildren,0)
	//JSFUNC(synch_fixPos,0)
	JSFUNC(synch_remove,0)
	JSFUNC(getBornReality,0)
	JSFUNC(createEventScript,0)
	JSFUNC(applyToChildren,1)	// перебирает все дочерние элементы первого уровня 
								// и вызывает для кождого из них функцию, переданную в качестве аргумента
	JSFUNC(queryFullState,0)
	JSFUNC(getServerVersion,0)
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlRMMLElement::_EventSpec[] = {
	MLEVENT(onSynched)
	{0, 0, 0}
};

ADDPROPFUNCSPECS_IMPL(mlRMMLElement)

bool mlRMMLElement::IsNameValid(JSString* ajssName){
	jschar* jsc=JS_GetStringChars(ajssName);
	int iLen=JS_GetStringLength(ajssName);
	while(iLen--){
		switch(*jsc){
		case L'.':
		case L' ':
		case L',':
		case L':':
		case L';':
		case L'-':
		case L'+':
		case L'*':
		case L'|':
		case L'\'':
		case L'\"':
		case L'(':
		case L')':
		case L'[':
		case L']':
		case L'{':
		case L'}':
		case L'<':
		case L'>':
		case L'=':
		case L'/':
		case L'`':
		case L'~':
		case L'!':
		case L'@':
		case L'#':
		//case L'$':
		case L'%':
		case L'^':
		case L'&':
		case L'\\':
		case L'?':
		case L'':
			return false;
		}
		jsc++;
	}
	return true;
}

bool IsElementRef(JSString* ajssName){
	jschar* jsc=JS_GetStringChars(ajssName);
	int iLen=JS_GetStringLength(ajssName);
	while(iLen--){
		switch(*jsc){
		case L' ':
		case L',':
		case L':':
		case L';':
		case L'-':
		case L'+':
		case L'*':
		case L'|':
		case L'\'':
		case L'\"':
		case L'(':
		case L')':
		case L'[':
		case L']':
		case L'{':
		case L'}':
		case L'<':
		case L'>':
		case L'=':
		case L'/':
		case L'`':
		case L'~':
		case L'!':
		case L'@':
		case L'#':
		case L'$':
		case L'%':
		case L'^':
		case L'&':
		case L'\\':
		case L'?':
		case L'':
			return false;
		}
		jsc++;
	}
	return true;
}

JSString* CombineSynchProperties(JSContext* cx, jsval* pvSP1, jsval* pvSP2){
	JSString* jssSP1 = JSVAL_TO_STRING(*pvSP1);
	mlString sSP1 = wr_JS_GetStringChars(jssSP1);
	JSString* jssSP2 = JSVAL_TO_STRING(*pvSP2);
	mlString sSP2 = wr_JS_GetStringChars(jssSP2);
	// ?? надо объединять с проверкой (если такое свойство уже есть, то не добавлять)
	mlString sSP = sSP1 + L" " + sSP2;
	return wr_JS_NewUCStringCopyZ(cx, sSP.c_str());
}

bool mlRMMLElement::CreateSynchObject(JSString* ajssSynch){
	// создаем сам объект
	JSObject* jsoSynch = mlSynch::newJSObject(mcx);
	mpSynch = (mlSynch*)JS_GetPrivate(mcx, jsoSynch);
	mpSynch->SetParents(this);
	// выполняем в его контексте скрипт инициализации (заданный через атрибут/свойство synch)
	if(ajssSynch != NULL){
		jsval v;
		unsigned int uLength = JS_GetStringLength(ajssSynch);
		wchar_t* pwcInit = wr_JS_GetStringChars(ajssSynch);
		mpSynch->SetInitializationMode(true);
		JSBool bR = wr_JS_EvaluateUCScript(mcx, jsoSynch, pwcInit, uLength, "initSynch", 1, &v);
		mpSynch->SetInitializationMode(false);
	}
	return true;
}

///// JavaScript Set Property Wrapper
JSBool mlRMMLElement::SetJSProperty(JSContext* cx, int id, jsval *vp) {
	switch(id) {
		case JSPROP__name:{
			if(JSVAL_IS_NULL(*vp)){
				if(mpParent!=NULL && _name){
					// то убрать его из свойств родителя
					jsval v;
					JS_DeleteUCProperty2(cx, mpParent->mjso, JS_GetStringChars(_name), 
						JS_GetStringLength(_name), &v);
				}
				_name=NULL;
				return JS_TRUE;
			}
			if(!JSVAL_IS_STRING(*vp)){
				JS_ReportError(cx,"Element name value must be a string");
				return JS_FALSE;
			}
			JSString* jss=JSVAL_TO_STRING(*vp);
			if(!IsNameValid(jss)){
				JS_ReportError(cx,"Element name '%s' is not valid",cLPCSTR(jss));
				return JS_FALSE;
			}
			if(mpParent!=NULL){
				// если элемент уже имел имя
				if(_name){
					// то убрать его из свойств родителя
					jsval v;
					JS_DeleteUCProperty2(cx, mpParent->mjso, JS_GetStringChars(_name), 
						JS_GetStringLength(_name), &v);
				}else{
					FREE_FOR_GC(cx, mpParent->mjso, mjso);
				}
			}
			_name=JSVAL_TO_STRING(*vp);
//#ifdef _DEBUG
mpName=(wchar_t*)JS_GetStringChars(_name);
//if(isEqual(mpName, L"UpSPName"))
//int hh=0;
//#endif
			// устанавливаем связь с родительским элементом
			if(mpParent!=NULL){
				SAVE_FROM_GC(cx, mpParent->mjso, mjso);
				// если у родителя уже есть дочерний элемент с таким именем
				jsval v = JSVAL_NULL;
				JSBool bR = JS_GetUCProperty(cx, mpParent->mjso, JS_GetStringChars(_name), -1, &v);
				if(bR){
					mlRMMLElement* pMLELOldChild = JSVAL_TO_MLEL(cx, v);
					if(pMLELOldChild != NULL){
						// то удалитьего из child-ов
						mpParent->RemoveChild(pMLELOldChild);
//						GPSM(mcx)->CallGC(pMLELOldChild->NeedToClearRefs()?this:NULL);
//						GPSM(mcx)->CallGC();
					}
				}
				// родителю добавляем свойство с именем элемента
				//gd::CString sNewElemName;
				//sNewElemName=JS_GetStringChars(_name);
				//if(sNewElemName.IsEmpty()){
				//	sNewElemName=GenerateRandomName();
				//}
				v=OBJECT_TO_JSVAL(mjso);
				bR=JS_SetUCProperty(cx, mpParent->mjso, JS_GetStringChars(_name), -1, &v);
				//
				FREE_FOR_GC(cx, mpParent->mjso, mjso);
			}
			}break;
		case JSPROP__class:{
bool bDSSE = GPSM(mcx)->mbDontSendSynchEvents;
			if(JSVAL_IS_STRING(*vp)){
				SetClass(JSVAL_TO_STRING(*vp));
			}else if(JSVAL_IS_REAL_OBJECT(*vp)){
				SetClass(JSVAL_TO_OBJECT(*vp));
			}
			}break;
		case JSPROP__parent:{
//if(_name && isEqual(mpName, L"ui_COrderedCompositionBase")){
//	if(JSVAL_IS_REAL_OBJECT(*vp)){
//		JSObject* jso = JSVAL_TO_OBJECT(*vp);
//		if(JSO_IS_MLEL(jso)){
//			mlRMMLElement* pML2 = (mlRMMLElement*)JS_GetPrivate(mcx, jso);
//			int hh=0;
//		}
//	}
//	jsval vClss;
//	JS_GetProperty(mcx, JS_GetGlobalObject(mcx), GJSONM_CLASSES, &vClss);
//	jsval v;
//	JS_GetProperty(mcx, JSVAL_TO_OBJECT(vClss), "ui_COrderedCompositionBase", &v);
//	if(JSVAL_IS_OBJECT(v)){
//		JSObject* jso = JSVAL_TO_OBJECT(v);
//		if(jso == mjso)
//		int hh=0;
//	}
//}
			mlRMMLElement* pNewParent = JSVAL_TO_MLEL(cx, *vp);
bool bDSSE0 = GPSM(mcx)->mbDontSendSynchEvents;
			if(pNewParent != mpParent){
				bool bWasParent = (mpParent != NULL);
				mlRMMLElement* pElemToClearRefsIn = NULL;
				if(mpParent != NULL && pNewParent == NULL){
					pElemToClearRefsIn = GetElemToClearRefsIn();
				}
				// если родитель уже был
				if(mpParent != NULL){
					// то мы больше не его
					mpParent->RemoveChild(this);
				}
				if(pNewParent != NULL){
					pNewParent->AddChild(this);
					if(!bWasParent && GPSM(mcx)->mbParentIsSetFromJS){
						GPSM(mcx)->MayBeAddedToScene(this);
					}
				}else{
					// почистить ссылки на объект (например массив intersections)
//					GPSM(mcx)->CallGC(NeedToClearRefs()?pElemToClearRefsIn:NULL);
					if(bWasParent && GPSM(mcx)->mbParentIsSetFromJS)
						GPSM(mcx)->MayBeRemovedFromScene(this);
					GPSM(mcx)->CallGC();
				}
				// если сменился родитель, то надо уведомить об этом RenderManager
				if(bWasParent && pNewParent != NULL){
bool bDSSE = GPSM(mcx)->mbDontSendSynchEvents;
					NotifyParentChanged();
bool bDSSE2 = GPSM(mcx)->mbDontSendSynchEvents;
int hh = 0;
				}
			}
			}break;
		case JSPROP__synch:
//if(_name != NULL && isEqual(mpName, L"wboardObj"))
//int hh=0;
if(mpSynch != NULL)
int hh=0;
//mpSynch = NULL;
			// ML_JSVAL_TO_BOOL(mchSynchronized,*vp);
			if(JSVAL_IS_STRING(*vp)){
				if(isEqual(wr_JS_GetStringChars(JSVAL_TO_STRING(*vp)),L"true")){
					mchSynchronized = true;
				}else if(isEqual(wr_JS_GetStringChars(JSVAL_TO_STRING(*vp)),L"false")){
					mchSynchronized = false;
				}else{ // synch содержит список свойств, которые надо синхронизировать
					mlString sSynch = wr_JS_GetStringChars(JSVAL_TO_STRING(*vp));
					if(sSynch.find(L"=") == mlString::npos){
						mchSynchronized = 2;
						GPSM(mcx)->SetSynchMode(mlSceneManager::synchMode2);
						jsval vSP = JSVAL_NULL;
						wr_JS_GetUCProperty(cx, mjso, SYNCHPROPERTIESN, -1, &vSP);
						// если список синхронизируемых свойств не был унаследован от класса
						if(vSP == JSVAL_NULL || vSP == JSVAL_VOID){
							// то сохраняем
							wr_JS_SetUCProperty(cx, mjso, SYNCHPROPERTIESN, -1, vp);
						}else{
							// если был унаследован, 
							// то объединяем унаследованный список с полученным
							JSString* jssSP = CombineSynchProperties(cx, &vSP, vp);
							vSP = STRING_TO_JSVAL(jssSP);
							wr_JS_SetUCProperty(cx, mjso, SYNCHPROPERTIESN, -1, &vSP);
						}	
					}else{
						mchSynchronized = 3;
						GPSM(mcx)->SetSynchMode(mlSceneManager::synchMode3);
						// создать объект типа Synch
						CreateSynchObject(JSVAL_TO_STRING(*vp));
						jsval vS = (mpSynch!=NULL)?OBJECT_TO_JSVAL(mpSynch->mjso):JSVAL_NULL;
						*vp = vS;
					}
				}
			}else if(JSVAL_IS_BOOLEAN(*vp)){
				mchSynchronized = JSVAL_TO_BOOLEAN(*vp);
			}else if(JSVAL_IS_REAL_OBJECT(*vp)){ 
				// если это synch, 
				JSObject* jsoSrcSynch = JSVAL_TO_OBJECT(*vp);
				if(mlSynch::IsInstance(cx, jsoSrcSynch)){
					// то создать свой synch 
					mchSynchronized = 3;
					CreateSynchObject(NULL);
					if(mpSynch != NULL){
						// и скопировать туда свойства из того
						JSIdArray* idArr = JS_Enumerate(mcx, jsoSrcSynch);
						for(int ij=0; ij < idArr->length; ij++){
							jsid id = idArr->vector[ij];
							jsval v;
							if(JS_IdToValue(mcx, id, &v)){
								if(JSVAL_IS_STRING(v)){
									JSString* jss=JSVAL_TO_STRING(v);
									jschar* pjsch=JS_GetStringChars(jss);
									jsval vProp;
									size_t jssLen=JS_GetStringLength(jss);
									JS_GetUCProperty(mcx, jsoSrcSynch, pjsch, jssLen, &vProp);
									JS_SetUCProperty(mcx, mpSynch->mjso, pjsch, jssLen, &vProp);
								}
							}
						}
						JS_DestroyIdArray(cx, idArr);
					}
					jsval vS = (mpSynch!=NULL)?OBJECT_TO_JSVAL(mpSynch->mjso):JSVAL_NULL;
					*vp = vS;
				}
			}
			if(mchSynchronized > 0){
				GPSM(mcx)->mbSceneMayHaveSynchEls=true;
				//GPSM(mcx)->SetSynchJSHook();
				mlRMMLElement* pParent = this;
				while(pParent){
					pParent->mbMayHaveSynchChildren = true;
					pParent = pParent->mpParent;
				}
			}
			break;
		case JSPROP__frozen:{
			bool bWasFrozen=_frozen;
			ML_JSVAL_TO_BOOL(_frozen,*vp);
			if(bWasFrozen!=_frozen)
				FrozenChanged();
			}break;
		case JSPROP_userEvents:{
			if(JSVAL_IS_STRING(*vp)){
				JSString* jss = JSVAL_TO_STRING(*vp);
				ParseUserEvents(wr_JS_GetStringChars(jss));
			}else{
				JS_ReportError(cx, "Value of userEvents property must be a string");
			}
			}break;
		case JSPROP_scriptProviders:{
			if(JSVAL_IS_STRING(*vp)){
				JSString* jss = JSVAL_TO_STRING(*vp);
				ParseScriptProviders(wr_JS_GetStringChars(jss));
			}else{
				JS_ReportError(cx, "Value of scriptProviders property must be a string");
			}
			}break;
		default:
			return GetLoadable()->SetJSProperty(cx, id, vp);
	}
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLElement::GetJSProperty(int id, jsval *vp) {
	switch(id) {
		case JSPROP__name:
			if(_name==NULL) *vp=JSVAL_NULL;
			else *vp=STRING_TO_JSVAL(_name);
			break;
		case JSPROP__class:
			if(mpClass==NULL) *vp=JSVAL_NULL;
			else *vp=OBJECT_TO_JSVAL(mpClass->mjso);
			break;
		case JSPROP__parent:
//if(_name && isEqual(mpName, L"ui_COrderedCompositionBase")){
//	if(JSVAL_IS_REAL_OBJECT(*vp)){
//		JSObject* jso = JSVAL_TO_OBJECT(*vp);
//		if(JSO_IS_MLEL(jso)){
//			mlRMMLElement* pML2 = (mlRMMLElement*)JS_GetPrivate(mcx, jso);
//			if(pML2->_name && isEqual(pML2->mpName, L"oChemCalc"))
//				int hh=0;
//		}
//	}
//}
			if(mpParent==NULL) *vp=JSVAL_NULL;
			else *vp=OBJECT_TO_JSVAL(mpParent->mjso);
			break;
//			*vp = __float_TO_JSVal(cx,priv->getX());
		case JSPROP__synch:
//			*vp=BOOLEAN_TO_JSVAL((mchSynchronized > 0));
			*vp = JSVAL_FALSE;
//if(_name != NULL && isEqual(mpName, L"testscomp"))
//int hh=0;
			if(mchSynchronized == 1){
				*vp = JSVAL_TRUE;
			}else if(mchSynchronized == 2){
				wr_JS_GetUCProperty(mcx, mjso, SYNCHPROPERTIESN, -1, vp);
			}else if(mchSynchronized == 3){
				*vp = JSVAL_NULL;
				if(mpSynch != NULL){
					*vp = OBJECT_TO_JSVAL(mpSynch->mjso);
				}
			}
			break;
		case JSPROP__frozen:
			*vp=BOOLEAN_TO_JSVAL(_frozen);
			break;
		case JSPROP__target:{
			JSString* jssTarget = wr_JS_NewUCStringCopyZ(mcx, GetFullPath().c_str());
			*vp = STRING_TO_JSVAL(jssTarget);
			}break;
		case JSPROP_userEvents:{
			JSString* jssUserEvents = wr_JS_NewUCStringCopyZ(mcx, GetUserEvents().c_str());
			*vp = STRING_TO_JSVAL(jssUserEvents);
			}break;
		default:
			return GetLoadable()->GetJSProperty(id,vp);
	}
	return JS_TRUE;
}

///// JavaScript Function Wrappers
//JSBool mlRMMLElement::JSFUNC_move(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	//Vector2d *p = (Vector2d*)JS_GetPrivate(cx, obj);
	//if (argc < 3) return JS_FALSE;
	//if (argc == 3) {
	//	// Function: void approach(float x, float y, float i) 
	//	if (JSVAL_IS_NUMBER(argv[0]) && JSVAL_IS_NUMBER(argv[1]) && JSVAL_IS_NUMBER(argv[2])) {
	//		p->approach(
	//			__JSVal_TO_float(argv[0]),
	//			__JSVal_TO_float(argv[1]),
	//			__JSVal_TO_float(argv[2])
	//		);
	//		return JS_TRUE;
	//	}
	//}
//	return JS_FALSE;
//}

//JSBool mlRMMLElement::JSFUNC_hide(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	// ??
//	return JS_FALSE;
//}

//void mlRMMLElement::SetAttribute(JSContext* cx, const char* asAttrName, wchar_t* asAttrValue){
//	jsval v;
//	JSObject* obj=((mlJSClass*)this)->getJSObject(cx);
//	JSString* sVal=JS_NewUCStringCopyZ(cx,(const jschar*)asAttrValue);
//	v=STRING_TO_JSVAL(sVal);
//	JS_SetProperty(cx, obj, asAttrName, &v);
//}

JSObject* mlRMMLElement::FindClass(const wchar_t* apwcClassName){
//if(isEqual(apwcClassName,L"stlNameNormal"))
//int hh=0;
	// получаем "classes"
	jsval vClss;
	JS_GetProperty(mcx, JS_GetGlobalObject(mcx), GJSONM_CLASSES, &vClss);
	if(!JSVAL_IS_REAL_OBJECT(vClss)) return NULL;
	JSObject* jsoClasses = JSVAL_TO_OBJECT(vClss);
	// получаем объект класса
	jsval vc = JSVAL_NULL;
	wr_JS_GetUCProperty(mcx, jsoClasses, apwcClassName, -1, &vc);
	if((vc==JSVAL_NULL || vc==JSVAL_VOID) && mpParent != NULL){
		std::list<mlRMMLElement*> lParents;
		mlRMMLElement* pParent = mpParent;
		for(; pParent != NULL; pParent = pParent->mpParent)
			lParents.push_front(pParent);
		for(int i = lParents.size(); i > 0; i--){
			mlString sSubClassName;
			std::list<mlRMMLElement*>::iterator li = lParents.begin();
			for(int j = 0; j < i; j++, li++){
				pParent = *li;
				sSubClassName += pParent->GetName();
				sSubClassName += L'_';
			}
			sSubClassName += apwcClassName;
			wr_JS_GetUCProperty(mcx, jsoClasses, sSubClassName.c_str(), -1, &vc);
			if(vc!=JSVAL_NULL && vc!=JSVAL_VOID)
				break;
		}
	}
	// если все-таки не нашли, продолжаем поиски играясь с именем parent-а
	if((vc==JSVAL_NULL || vc==JSVAL_VOID) && mpParent != NULL && mpParent->_name != NULL){
		jschar* jscParentName = JS_GetStringChars(mpParent->_name);
		mlString sParentName = (const wchar_t*)jscParentName;
		mlString::size_type pos;
		while((pos = sParentName.rfind(L'_')) != mlString::npos){
			sParentName.erase(pos);
			mlString sSubClassName = sParentName;
			sSubClassName += L'_';
			sSubClassName += apwcClassName;
			wr_JS_GetUCProperty(mcx, jsoClasses, sSubClassName.c_str(), -1, &vc);
			if(vc!=JSVAL_NULL && vc!=JSVAL_VOID)
				break;
		}
	}
	if(JSVAL_IS_REAL_OBJECT(vc))
		return JSVAL_TO_OBJECT(vc);
	return NULL;
}

mlresult mlRMMLElement::SetClass(JSString* ajssClass){
	// копируем свойства
	if(mpClass!=NULL){
		JS_ReportError(mcx, "Class of \'%s\' already set to \'%s\'", cLPCSTR(GetName()), cLPCSTR(mpClass->GetName()));
		return ML_ERROR_FAILURE;
	}
	JSObject* jsoClass = FindClass(wr_JS_GetStringChars(ajssClass));
	if(jsoClass == NULL){
		JS_ReportError(mcx,"Class '%s' not found",cLPCSTR(ajssClass));
		return ML_ERROR_FAILURE;
	}
	return SetClass(jsoClass);
}

mlresult mlRMMLElement::SetClass(JSObject* ajsoClass){
	mlRMMLElement* pClass = GET_RMMLEL(mcx,ajsoClass);
	if(mpClass == pClass)
		return ML_OK;
	mpClass = pClass;
	//// устанавливаем его в качестве прототипа
//	JS_SetPrototype(cx, mjso, jsoClass);
////JSObject* jsop=JS_GetPrototype(cx, mjso);
////mlRMMLElement* pMLEl=GET_RMMLEL(cx,jsop);
////int hh=0;
	// проверяем соответствие типов
	if(mRMMLType!=mpClass->mRMMLType && 
		!(mRMMLType == MLMT_PARTICLES && mpClass->mRMMLType == MLMT_OBJECT))
	{
		mpClass=NULL;
		JS_ReportError(mcx,"Object and its class must have same type");
		return ML_ERROR_FAILURE;
	}
	SAVE_FROM_GC(mcx,mjso,ajsoClass)
	// проводим инициализацию (копируем из класса все свойства и методы)
	return InitFromClass();
}

void mlRMMLElement::PropIsSet(mlRMMLElement* apThis, char idProp){
	if(idProp == -1) apThis->UnknownPropIsSet();
	if(!PMO_IS_NULL(apThis->mpMO)) apThis->mpMO->PropIsSet();
//if(apThis->_name && isEqual(apThis->mpName, L"loading") && idProp == mlRMMLVisible::JSPROP__visible)
//return;
	if(apThis->mpListeners.get()==NULL) return;
	if(idProp>=TIDB_mlRMMLElement && !aEventIsPW[idProp]) return;
	apThis->CallListeners(idProp,false,true);
}

#define IS_EQUAL(S1, S2) \
	(*(S1) == *(S2) && (*(S1) == 0 || isEqual((S1)+1, (S2)+1)))

__forceinline bool IsParentOrClassPropName(const wchar_t* apwcPropName){
	if(*apwcPropName == L'_') apwcPropName++;
	if(IS_EQUAL(apwcPropName, MLELPNAW_PARENT)) return true;
	if(IS_EQUAL(apwcPropName, MLELPNAW_CLASS)) return true;
	if(IS_EQUAL(apwcPropName, MLELPN_SCRIPTPROVIDERS)) return true;
	return false;
}

__forceinline bool IsClassPropName(const wchar_t* apwcPropName){
	if(*apwcPropName == L'_') apwcPropName++;
	if(isEqual(apwcPropName, MLELPNAW_CLASS)) return true;
	return false;
}

mlresult mlRMMLElement::GetPropsAndChildrenCopyFrom(mlRMMLElement* apMLEl, bool abInitFromClass){
	ML_ASSERTION(mcx, apMLEl!=NULL,"GetPropsFrom");
	// копируем детей 
	if(apMLEl->mpChildren!=NULL){
		for(unsigned int ii=0; ii<apMLEl->mpChildren->size(); ii++){
			mlRMMLElement* pElToCopy=(*(apMLEl->mpChildren))[ii];
#ifdef _DEBUG
if(pElToCopy->_name && isEqual(pElToCopy->mpName, L"stCaptionActive")){
int hh=0;
}
#endif
if(pElToCopy->miSrcFilesIdx == 3 && pElToCopy->miSrcLine == 336)
int hh=0;
			mlRMMLElement* pNewMLEl = pElToCopy->Duplicate(this);
			ML_ASSERTION(mcx, pNewMLEl!=NULL,"mlRMMLElement::GetPropsAndChildrenCopyFrom");
//if(pNewMLEl->mType == MLMT_SCRIPT){
//int hh=0;
//}
			pNewMLEl->SetSrcFileIdx(pElToCopy->miSrcFilesIdx, pElToCopy->miSrcLine);
			pNewMLEl->SetParent(this);
			if(pNewMLEl->IsOnloadScript()){
				if(abInitFromClass && ((mlRMMLScript*)pElToCopy)->mpClass == NULL){
						((mlRMMLScript*)pNewMLEl)->mpClass = apMLEl;
				}else
					((mlRMMLScript*)pNewMLEl)->mpClass = ((mlRMMLScript*)pElToCopy)->mpClass;
//				((mlRMMLScript*)pNewMLEl)->mpClass = (mlRMMLElement*)(((int)((mlRMMLScript*)pElToCopy)->mpClass) + (abInitFromClass?1:0));
			}
		}
	}

	// копируем свойства
	JSIdArray* idArr=JS_Enumerate(mcx, apMLEl->mjso);
	for(int ij=0; ij<idArr->length; ij++){
		jsid id=idArr->vector[ij];
		jsval v;
		if(JS_IdToValue(mcx,id,&v)){
//if(v==JSVAL_NULL)
//int kk=0;
//if(v==JSVAL_VOID)
//int kk=0;
			if(JSVAL_IS_STRING(v)){
				JSString* jss=JSVAL_TO_STRING(v);
				wchar_t* pjsch=wr_JS_GetStringChars(jss);
if(pjsch && isEqual((const wchar_t*)pjsch, L"synch")){
int hh=0;
}
				if(!IsParentOrClassPropName((wchar_t*)pjsch)){
					// копируем свойство класса
					jsval vProp;
					size_t jssLen=JS_GetStringLength(jss);
					wr_JS_GetUCProperty(mcx, apMLEl->mjso, pjsch, jssLen, &vProp);
					bool bDontCopy=false;
					if(vProp!=JSVAL_NULL && JSVAL_IS_OBJECT(vProp)){
						JSObject* jso=JSVAL_TO_OBJECT(vProp);
						if(jssLen==2 && ML_JS_GETCLASS(mcx, jso)->flags==1){ 
							// SAVE_FROM_GC-свойство (script)
							bDontCopy=true;
						}else if(JSO_IS_MLEL(mcx, jso)){
							mlRMMLElement* pMLElChld=GET_RMMLEL(mcx,jso);
							if(pMLElChld!=NULL){
								// если действительно ребенок
								if(pMLElChld->mpParent==apMLEl){
									// то не копируем 
									bDontCopy=true;
								}else{
									// значит ссылка на другой элемент
									mlString wsElemRef=apMLEl->GetRefPropContext((const wchar_t*)pjsch)->GetElemRefName(pMLElChld);
									JSString* jss=wr_JS_NewUCStringCopyZ(mcx,wsElemRef.c_str());
									vProp=STRING_TO_JSVAL(jss);
								} 
							}
						}else{
							// не RMML-элемент
//							if(JS_TypeOfValue(mcx,vProp)==JSTYPE_FUNCTION){
//							 	mlString sSuperName=L"super_";
//								sSuperName+=pjsch;
//								JS_SetUCProperty(mcx, mjso, sSuperName.c_str(), -1, &vProp);
//								bDontCopy=true;
//							}
						}
					}
					if(!bDontCopy)
						wr_JS_SetUCProperty(mcx, mjso, pjsch, jssLen, &vProp);
				}
				else if( IsClassPropName(pjsch))
				{
					mpClass = apMLEl->mpClass;
					if(mpClass != NULL){
						jsval vProp = OBJECT_TO_JSVAL(mpClass->mjso);
						wr_JS_SetUCProperty(mcx, mjso, pjsch, -1, &vProp);
						// устанавливаем classCommon
						jsval jsvClassCommon = JSVAL_VOID;
						JS_GetProperty(mcx, mpClass->mjso, MLELPN_CLASSCOMMON, &jsvClassCommon);
						JS_DefineProperty(mcx, mjso, MLELPN_CLASSCOMMON, jsvClassCommon, NULL, NULL, JSPROP_READONLY);
					}
				}
			}
			if(JSVAL_IS_OBJECT(v)){
int jj=0;
			}
			if(JSVAL_IS_INT(v)){
int jj=0;
			}
		}
//JSProperty prop; prop.id=id;
	}
	JS_DestroyIdArray(mcx, idArr);
	return ML_OK;
}

mlresult mlRMMLElement::InitFromClass(){

	// сохраняем имя
	JSString* jssName=_name;
	// копируем свойства и детей класса
	mlRMMLElement* pClass = mpClass;
	GetPropsAndChildrenCopyFrom(mpClass, true);
	mpClass = pClass;
	// восстанавливаем имя
	jsval v=JSVAL_NULL;
	if(jssName!=NULL)
		v=STRING_TO_JSVAL(jssName);
	JS_SetProperty(mcx, mjso, MLELPN_NAME, &v);
	// устанавливаем classCommon
	jsval jsvClassCommon = JSVAL_VOID;
	JS_GetProperty(mcx, mpClass->mjso, MLELPN_CLASSCOMMON, &jsvClassCommon);
	JS_DefineProperty(mcx, mjso, MLELPN_CLASSCOMMON, jsvClassCommon, NULL, NULL, JSPROP_READONLY);
	return ML_OK;
}

mlresult mlRMMLElement::SetParent(mlRMMLElement* apParent){
	if(mpParent==apParent) return ML_OK;
	GPSM(mcx)->SceneStructChanged();
	if(mpParent!=NULL){
		mpParent->RemoveChild(this);
	}
	if( GetType() == MLMT_SCENE3D)
	{
		int ii = 0;
	}

	mpParent=NULL;
	if(apParent != NULL){
		if(IsJSO()){
			// установить ребенку _parent
			jsval v=JSVAL_NULL;
			if(apParent->IsJSO())
				v=OBJECT_TO_JSVAL(apParent->mjso);
			GPSM(mcx)->mbParentIsSetFromJS = false;
			JS_SetProperty(mcx, mjso, MLELPN_PARENT, &v);
			JS_SetProperty(mcx, mjso, MLELPNA_PARENT, &v);
			GPSM(mcx)->mbParentIsSetFromJS = true;
		}else{
			apParent->AddChild(this);
		}
	}else{
		if(IsJSO()){
			// установить ребенку _parent = null
			jsval v=JSVAL_NULL;
			GPSM(mcx)->mbParentIsSetFromJS = false;
			JS_SetProperty(mcx, mjso, MLELPNA_PARENT, &v);
			GPSM(mcx)->mbParentIsSetFromJS = true;
		}
		// Разрегистрируем синхронизируемый объект
		if(muID != OBJ_ID_UNDEF)
		{
			CLogValue logValue(
				"[RMML.OBJECTS] parent=NULL - ",
				(const char*)cLPCSTR( GetName())
				);
			GPSM(mcx)->GetContext()->mpLogWriter->WriteLn( logValue);
		}
	}
#ifdef TRACE_LOST_ELEMS
	GPSM(mcx)->UpdateNotInScenesElems(this);
#endif
	return ML_OK;
}

void mlRMMLElement::ChildIsDestroying(mlRMMLElement* pChild){
	if(mpChildren==NULL) return;
	v_elems::iterator vi;
	for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++){
		if(*vi==pChild){
			mpChildren->erase(vi);
			return;
		}
	}
}

void mlRMMLElement::RemoveChild(mlRMMLElement* pChild){
	if(mpChildren==NULL) return;
	ChildRemoving(pChild);
	GPSM(mcx)->SceneStructChanged();
	v_elems::iterator vi;

	if(pChild->IsSynchronized() && pChild->muID == OBJ_ID_UNDEF){
		mlRMMLElement* pParentWithID = this;
		for(; pParentWithID != NULL; pParentWithID = pParentWithID->mpParent){
			if(pParentWithID->muID != OBJ_ID_UNDEF)
				break;
		}
		if(pParentWithID != NULL){
			mlSynch* pSynch = pParentWithID->mpSynch;
			if(pSynch != NULL){
				v_elems* pvSynchedChildren = pSynch->GetSynchedChildren();
				if(pvSynchedChildren != NULL && pvSynchedChildren != ML_SYNCH_CHILDREN_EMPTY_VAL){
					for(vi = pvSynchedChildren->begin(); vi != pvSynchedChildren->end(); vi++){
						if(*vi == pChild){
							*vi = NULL;
							break;
						}
					}
				}
			}
		}
	}

	for(vi = mpChildren->begin(); vi != mpChildren->end(); vi++){
		if(*vi == pChild){
			mpChildren->erase(vi);
			// удаляем его из свойств
			if(IsJSO() && pChild->IsJSO()){
				if(pChild->_name!=NULL){
					jsval v;
					JS_DeleteUCProperty2(mcx, mjso, JS_GetStringChars(pChild->_name), 
						JS_GetStringLength(pChild->_name), &v);
				}else{
					// разрешаем сборщику мусора удалить его при следующей сборке
					FREE_FOR_GC(mcx,mjso,pChild->mjso)
				}
			}
			if(!pChild->IsJSO() && mpvNonJSOChildren!=NULL){
				v_elems::iterator vi;
				for(vi=mpvNonJSOChildren->begin(); vi!=mpvNonJSOChildren->end(); vi++){
					if(*vi==pChild) *vi=NULL;
				}
			}
			pChild->mpParent = NULL;
			pChild->ParentNulled();
#ifdef TRACE_LOST_ELEMS
			GPSM(mcx)->UpdateNotInScenesElems(this);
#endif
			return;
		}
	}
	return;
}

void mlRMMLElement::AddChild(mlRMMLElement* pNewChild){
	bool bDSSE = GPSM(mcx)->mbDontSendSynchEvents;
	if(pNewChild==NULL) return;
	if(pNewChild->mpParent!=NULL && pNewChild->mpParent!=this){
		ML_ASSERTION(mcx, false,"Wrong old parent in mlRMMLElement::AddChild");
	}
//if(isEqual(pNewChild->GetName(), L"sqvu_cveto4ek_ekran09"))
//int hh=0;
	GPSM(mcx)->SceneStructChanged();
	pNewChild->mpParent=this;
#ifdef TRACE_LOST_ELEMS
	GPSM(mcx)->UpdateNotInScenesElems(this);
#endif
	if(mpChildren==NULL){ 
		MP_NEW_VECTOR(pv, v_elems);
		mpChildren = pv;
	}
#ifdef _DEBUG
//?? похоже бывает так, что Child 2 раза в mpChildren добавляется
// надо проверять!
v_elems::iterator vi;
for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
mlRMMLElement* pChild = *vi;
ML_ASSERTION(mcx, pChild != pNewChild, "mlRMMLElement::AddChild");
if(pChild == pNewChild)
int hh=0;
}
#endif
	mpChildren->push_back(pNewChild);

//	if(IsJSO() && pNewChild->IsJSO()){
//		JS_SetParent(mcx, pNewChild->mjso, mjso);
//	}
	if(!pNewChild->IsJSO()){
		if(mpvNonJSOChildren==NULL){
			MP_NEW_VECTOR_V(mpvNonJSOChildren, v_elems);
		}
		if(mpvNonJSOChildren->size()>30){
			v_elems::iterator vi;
			for(vi=mpvNonJSOChildren->begin(); vi!=mpvNonJSOChildren->end(); vi++){
				if(*vi==NULL){
					*vi=pNewChild;
					break;
				}
				if(*vi == pNewChild)	// вряд ли такое может быть, но все же...
					break;
			}
			if(vi==mpvNonJSOChildren->end())
				mpvNonJSOChildren->push_back(pNewChild);
		}else
			mpvNonJSOChildren->push_back(pNewChild);
		return;
	}
	if(pNewChild->IsJSO()){
		if(pNewChild->_name!=NULL){
			// если уже был дочерний элемент с таким именем,
			v_elems::iterator vi;
			for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
				mlRMMLElement* pChild = *vi;
				if(pChild == pNewChild) continue;
				if(pChild->_name == NULL) continue;
				if(isEqual(pNewChild->_name, pChild->_name)){
					// то удалить его из списка
					//mpChildren->erase(vi);
					if(pChild->mpParent == this){
					//	pChild->mpParent = NULL;
						RemoveChild(pChild);
					}
					break;
				}
			}
			// добавляем его в свойства
			jsval v=OBJECT_TO_JSVAL(pNewChild->mjso);
			JSBool bR=JS_SetUCProperty(mcx, mjso, JS_GetStringChars(pNewChild->_name), 
				JS_GetStringLength(pNewChild->_name), &v);
		}else{
			// защищаем его от сборщика мусора
//			jsval v=OBJECT_TO_JSVAL(pNewChild->mjso);
//			jschar* pjsch=(jschar*)(void*)(&(pNewChild->mjso));
//			JSBool bR=JS_SetUCProperty(mcx, mjso, pjsch, sizeof(void*)/sizeof(jschar), &v);
			SAVE_FROM_GC(mcx, mjso, pNewChild->mjso);
		}
		if(pNewChild->IsSynchronized() && pNewChild->muID == OBJ_ID_UNDEF){
			mlRMMLElement* pParentWithID = this;
			for(; pParentWithID != NULL; pParentWithID = pParentWithID->mpParent){
				if(pParentWithID->muID != OBJ_ID_UNDEF)
					break;
			}
			if(pParentWithID != NULL){
				mlSynch* pSynch = pParentWithID->mpSynch;
				if(pSynch != NULL){
					v_elems* pvSynchedChildren = pSynch->GetSynchedChildren();
					if(pvSynchedChildren == NULL || pvSynchedChildren == ML_SYNCH_CHILDREN_EMPTY_VAL)
						pvSynchedChildren = mpSynch->CreateSynchedChildren();
					pvSynchedChildren->push_back(pNewChild);
				}
			}
		}
	}
	// если медиа-объекты дочернего элемента еще не загружены
	if(pNewChild->mpMO == NULL){
		// а медиа-объекты сцены уже загружены
		mlRMMLElement* pScene=this;
		while(pScene->mRMMLType!=MLMT_SCENE){
			pScene=pScene->mpParent;
			if(pScene==NULL) break;
		}
		if(pScene!=NULL && pScene->mpMO==PMOV_CREATED){
//if(isEqual(pNewChild->GetName(), L"sqvu_cveto4ek_ekran09"))
//	int hh=0;
unsigned long tmStart = GPSM(mcx)->GetContext()->mpApp->GetTimeStamp();
			// то загрузить медиа-объекты этого элемента
			pNewChild->CreateAndLoadMOs();
//TRACE(mcx, " pNewChild->CreateAndLoadMOs() of %s for %d ms\n", cLPCSTR(GetName()), GPSM(mcx)->GetContext()->mpApp->GetTimeStamp() - tmStart);
//tmStart = GPSM(mcx)->GetContext()->mpApp->GetTimeStamp();
			// и активировать его скрипты
			pNewChild->ActivateScripts();
//TRACE(mcx, " pNewChild->ActivateScripts() of %s for %d ms\n", cLPCSTR(GetName()), GPSM(mcx)->GetContext()->mpApp->GetTimeStamp() - tmStart);
			// 
			if(IsVisible() && pNewChild->IsVisible())
				GetVisible()->InvalidBounds();
			//
			ChildAdded(pNewChild);
		}
	}
}

mlRMMLElement* mlRMMLElement::GetChild(const wchar_t* apwcName, int aiNameLen){
	unsigned int uNameLen;
	if(aiNameLen < 0) uNameLen=wcslen(apwcName);
	else uNameLen=(unsigned int)aiNameLen;
	if(isEqual(apwcName,MLELPNW_PARENT,uNameLen)) return mpParent;
	jsval vProp=JSVAL_NULL;
	if(!JS_GetUCProperty(mcx, mjso, (jschar*)apwcName, uNameLen, &vProp))
		return NULL;
	if(!JSVAL_IS_REAL_OBJECT(vProp)) return NULL;
	JSObject* jsoChild = JSVAL_TO_OBJECT(vProp);
	if(!JSO_IS_MLEL(mcx, jsoChild)) return NULL;
	return GET_RMMLEL(mcx,jsoChild);
}

wchar_t* mlRMMLElement::GetName(){ 
	if(_name!=NULL)
		return wr_JS_GetStringChars(_name); 
	if(!IsJSO()){
		if(mRMMLType==MLMT_SCRIPT){
			return L"[script]";
		}
		return L"??";
	}
	jsval v;
	if(JS_GetUCProperty(mcx,mjso,(jschar*)(void*)(L"__"), sizeof(void*)/sizeof(jschar),&v)
		&& JSVAL_IS_STRING(v)){
		return wr_JS_GetStringChars(JSVAL_TO_STRING(v));
	}
	wchar_t pwcBuf[100];
	JSString* jssHexName = NULL;
	if(mpClass != NULL){
		swprintf_s(pwcBuf,100,L"[%X:", mjso);
		mlString wsHexNameWithClass = pwcBuf;
		wsHexNameWithClass += mpClass->GetName();
		wsHexNameWithClass += L']';
		jssHexName=wr_JS_NewUCStringCopyZ(mcx, wsHexNameWithClass.c_str());
	}else{
		swprintf_s(pwcBuf,100,L"[%X]", mjso);
		jssHexName=wr_JS_NewUCStringCopyZ(mcx, pwcBuf);
	}
	v=STRING_TO_JSVAL(jssHexName);
	JS_DefineUCProperty(mcx, mjso, 
		(jschar*)(void*)(L"__"), sizeof(void*)/sizeof(jschar), 
		v, NULL, NULL, 0); 
	return wr_JS_GetStringChars(jssHexName);
}

mlRMMLElement* mlRMMLElement::FindElemByName(const wchar_t* apwcName){
	if(apwcName==NULL) return NULL;
	const wchar_t* pwcDot=findDot(apwcName);
	if(pwcDot==NULL){
//if(mpChildren!=NULL){
//v_elems::iterator vi;
//for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
//mlRMMLElement* pMLEl=*vi;
//if(pMLEl->mRMMLType==MLMT_VIEWPORT)
//int hh=0;
//}
//}
		return GetChild(apwcName);
	}
	mlRMMLElement* pChild = NULL;
	int iFirstIDLen = pwcDot-apwcName;
	if(iFirstIDLen == CWSLENGTH(GJSOWNM_SCENE) && isEqual(apwcName, GJSOWNM_SCENE, iFirstIDLen))
		pChild = (mlRMMLElement*)GetScene();
	if(pChild == NULL)
		pChild = GetChild(apwcName, iFirstIDLen);
	if(pChild == NULL) return NULL;
	return pChild->FindElemByName(pwcDot+1);
}

mlRMMLElement* mlRMMLElement::FindElemByName2(const wchar_t* apwcName){
	if(mpParent == NULL) return NULL;
	// поищем у родительского объекта 
	mlRMMLElement* pMLEl = mpParent->FindElemByName(apwcName);
	if(pMLEl != NULL) return pMLEl;
	// поищем у себя
	pMLEl = FindElemByName(apwcName);
	if(pMLEl != NULL) return pMLEl;
	// возможно это target в dot-нотации
	const wchar_t* pwcDot = findDot(apwcName);
	if(pwcDot != NULL) return NULL;
	mlRMMLElement* pParent = mpParent->mpParent;
	while(pParent != NULL){
		pMLEl = pParent->GetChild(apwcName);
		if(pMLEl != NULL) return pMLEl;
		pParent = pParent->mpParent;
	}
	// поищем в классах
	jsval vClss;
	JS_GetProperty(mcx, JS_GetGlobalObject(mcx), GJSONM_CLASSES, &vClss);
	if(!JSVAL_IS_OBJECT(vClss)) return NULL;
	// получаем объект класса
	jsval vc;
	wr_JS_GetUCProperty(mcx, JSVAL_TO_OBJECT(vClss), apwcName, -1, &vc);
	if(!JSVAL_IS_OBJECT(vc)) return NULL;
	JSObject* jso = JSVAL_TO_OBJECT(vc);
	// if(!mlRMMLStyles::IsInstance(jso)) return NULL; // ?
	pMLEl = GET_RMMLEL(mcx,jso);
	return pMLEl;
}

mlresult mlRMMLElement::UpdateSynchZ(const __int64 alTime, bool aForce)
{
	// z coordinate synchronization
	if (mpSynch == NULL)
		return ML_ERROR_NOT_IMPLEMENTED;

	mlRMML3DObject* p3DObj = Get3DObject();
	if (p3DObj == NULL)
		return ML_ERROR_NOT_IMPLEMENTED;

	if (!aForce)
		return ML_OK;

	float zShift = fabs( mpSynch->GetZ() - p3DObj->GetPos_z());

	__int64 alTimeDelta = alTime - lLastUpdateTime;

	// 1. изменения более 10 см компенсируем сразу
	// 2. изменения от 5 до 10 см компенсируем каждые 1 секунды
	// 3. изменения от 1 до 5 см компенсируем каждые 5 секунды
	bool applySynchronization = false;
	if( zShift > 10)
	{
		applySynchronization = true;
	}
	else if( zShift > 5)
	{
		if( alTimeDelta > 1*1000000)
			applySynchronization = true;
	}
	else if( zShift > 1)
	{
		if( alTimeDelta > 5*1000000)
			applySynchronization = true;
	}
	if (!applySynchronization)
		return ML_OK;

	lLastUpdateTime = alTime;
	mpSynch->SetZ( p3DObj->GetPos_z());

	return ML_OK;
}

// реализация синхронизации

bool mlRMMLElement::NeedToSynchronize(){
	mlRMMLElement* pMLEl=this;

	if(pMLEl->mchSynchronized==2) 
		return true;

	int whileCount = 0;

	while(pMLEl!=NULL)
	{
		if(pMLEl->mchSynchronized == 0)
			return false;
		if(pMLEl->mchSynchronized == 1) 
			return true;
		pMLEl=pMLEl->mpParent;

		whileCount++;

		if( whileCount == 100)
		{
			if( GPSM(mcx)->GetContext()->mpLogWriter)
			{
				GPSM(mcx)->GetContext()->mpLogWriter->WriteLn( "Seems to be hanged on mlRMMLElement::NeedToSynchronize");

				mlRMMLElement* pMLElTemp=this;
				while(pMLElTemp!=NULL)
				{
					wchar_t* wcName = GetStringProp("name");
					if( wcName)
						GPSM(mcx)->GetContext()->mpLogWriter->WriteLn( "Element name = ", (char*)cLPCSTRq(wcName));
					else
						GPSM(mcx)->GetContext()->mpLogWriter->WriteLn( "Element name == NULL");

					wchar_t* wcTarget = GetStringProp("target");
					if( wcTarget)
						GPSM(mcx)->GetContext()->mpLogWriter->WriteLn( "Element target = ", (char*)cLPCSTRq(wcTarget));
					else
						GPSM(mcx)->GetContext()->mpLogWriter->WriteLn( "Element target == NULL");

					GPSM(mcx)->GetContext()->mpLogWriter->WriteLn( "Try to find parent");

					pMLElTemp=pMLElTemp->mpParent;

					whileCount++;

					Sleep(1);

					if( whileCount == 200)
						break;
				}
				GPSM(mcx)->GetContext()->mpLogWriter->WriteLn( "Done in mlRMMLElement::NeedToSynchronize");
			}
		}
	}
	return false;
}

int mlRMMLElement::GetChildNum(mlRMMLElement* apChild){
	if(mpChildren==NULL) return -1;
	v_elems::iterator vi;
	int iNum=0;
	for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++,iNum++ ){
		mlRMMLElement* pMLEl=*vi;
		if(pMLEl==apChild){
			return iNum;
		}
		
	}
	return -1;
}

mlString mlRMMLElement::GetStringID(){
	mlString sFullStringID;
	if(mpParent!=NULL){
		sFullStringID=mpParent->GetStringID();
		sFullStringID+=L'.';
	}
	if(_name!=NULL){
		wchar_t* pwcName=wr_JS_GetStringChars(_name);
		return sFullStringID+pwcName;
	}else{
		wchar_t pwcName[3];
		pwcName[0]=0xF;
		pwcName[1]=1;
		if(mpParent!=NULL){
			pwcName[1]=mpParent->GetChildNum(this)+1;
		}
		pwcName[2]=0;
		return sFullStringID+pwcName;
	}
}

mlRMMLElement* mlRMMLElement::FindElemByStringID(wchar_t* apwcStringID){
	wchar_t* pwc=apwcStringID;
	if(*pwc==0xF){
		if(mpChildren==NULL) return NULL;
		pwc++;
		if(*pwc==L'\0') return NULL;
		int iChildNum=(*pwc)-1; 
		if(iChildNum >= (int)(mpChildren->size())) return NULL;
		mlRMMLElement* pMLEl=(*mpChildren)[iChildNum];
		pwc++;
		if(*pwc==L'\0') return pMLEl;
		if(*pwc!=L'.') return NULL;
		pwc++;
		return pMLEl->FindElemByStringID(pwc);
	}else{
		while(*pwc!=L'\0' && *pwc!=L'.') pwc++;
		v_elems::iterator vi;
		if(mpChildren == NULL) return NULL;
		for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
			mlRMMLElement* pMLEl=*vi;
			if(pMLEl->_name==NULL) continue;
			wchar_t* pwcChildName=pMLEl->GetName();
			if(isEqual(pwcChildName,apwcStringID,pwc-apwcStringID)){
				if(*pwc==L'\0') return pMLEl;
				if(*pwc!=L'.') return NULL;
				pwc++;
				return pMLEl->FindElemByStringID(pwc);
			}
		}
	}
	return NULL;
}

#define LOC_ID_MASK 0x40000000

unsigned int mlRMMLElement::GetID(bool abGetIfUndefined){
	/* Закомментарил BDima (27.12.2008) Портился muID
	if(muID == OBJ_ID_UNDEF || (muID & LOC_ID_MASK)){
		if(!abGetIfUndefined)
			return muID;
		mlString sStringID = GetStringID();
		mlISinchronizer* pSynch = GPSM(mcx)->GetContext()->mpSynch;
		if(pSynch != NULL){
			pSynch->GetObjectID(sStringID.c_str(), muID);
			if(muID == 0)
				muID = OBJ_ID_UNDEF;
			if(muID != OBJ_ID_UNDEF)
				GPSM(mcx)->RegSynchObject( this);
		}
	}
	*/
	return muID;
}

//

void mlRMMLElement::RelinkClassFunction(jsval &vFunc, const wchar_t* apwcFuncName){
	// если родитель функции - класс
	JSObject* jsoFunParent = JS_GetParent(mcx, JSVAL_TO_OBJECT(vFunc));
	if(JSO_IS_MLEL(mcx, jsoFunParent)){
		mlRMMLElement* pMLElClassChild = (mlRMMLElement*)JS_GetPrivate(mcx, jsoFunParent);
		if(pMLElClassChild->mpMO == NULL){ // дочерний элемент в классе или сам класс
			mlRMMLElement* pMLElFunParent = this;
			if(pMLElClassChild->mpParent == NULL){ // сам класс
				// тогда ищем наследника класса среди родителей	this-а
				while(pMLElFunParent != NULL){
					if(pMLElFunParent->mpClass == pMLElClassChild)
						break;
					pMLElFunParent = pMLElFunParent->mpParent;
				}
			}else{
				// ищем сам класс, запоминая "обратный путь"
				mlRMMLElement* pMLElClass = pMLElClassChild;
				int iPathLen = 0;
				int bufferLength = 100;
				wchar_t** paPath = MP_NEW_ARR( wchar_t*, bufferLength);
				while(pMLElClass->mpParent != NULL){
					ML_ASSERTION(mcx, pMLElClass->_name != NULL, "mlRMMLElement::RecompileClassFunction");
					paPath[iPathLen] = MP_NEW_ARR( wchar_t, JS_GetStringLength(pMLElClass->_name)+1);
					//wcscpy(paPath[iPathLen], JS_GetStringChars(pMLElClass->_name));
					rmmlsafe_wcscpy(paPath[0], bufferLength, iPathLen, wr_JS_GetStringChars(pMLElClass->_name));
					iPathLen++;
					pMLElClass = pMLElClass->mpParent;
				}
				// ищем наследника класса среди родителей
				while(pMLElFunParent != NULL){
					if(pMLElFunParent->mpClass == pMLElClass)
						break;
					pMLElFunParent = pMLElFunParent->mpParent;
				}
				// если нашли
				if(pMLElFunParent != NULL){
					// ищем дочерний элемент, который соответствует pMLElClassChild-у в классе
					for(int ii = iPathLen-1; ii>=0; ii--){
						pMLElFunParent = pMLElFunParent->GetChild(paPath[ii]);
						if(pMLElFunParent == NULL)
							break;
					}
				}
				while(iPathLen > 0){
					MP_DELETE_ARR( paPath[--iPathLen]);
				}
				MP_DELETE_ARR( paPath);
			}
			// если нашли
			if(pMLElFunParent != NULL){
				// создаем новый объект функции
				JSObject* jsoFunCloned = JS_CloneFunctionObject(mcx, JSVAL_TO_OBJECT(vFunc), pMLElFunParent->mjso);
				vFunc = OBJECT_TO_JSVAL(jsoFunCloned);
				wr_JS_SetUCProperty(mcx, pMLElFunParent->mjso, apwcFuncName, -1, &vFunc);
//JS_GetUCProperty(mcx, mjso, apwcFuncName, -1, &vFunc);

//				// перекомпилируем функцию в его контексте
//				JSString* jssFun = JS_DecompileFunctionBody(mcx, jsf, 0);
//				jschar* jscFun = JS_GetStringChars(jssFun);
//				JSFunction* jsf2 = JS_CompileUCFunction(mcx, pMLElFunParent->mjso, cLPCSTR(apwcFuncName), 0, NULL, 
//					jscFun, JS_GetStringLength(jssFun), "_script_", 1);
//				// и заменяем свойство onXXX
//				jsval vFunc = OBJECT_TO_JSVAL(JS_GetFunctionObject(jsf));
//				JS_SetUCProperty(mcx, mjso, apwcFuncName, -1, &vFunc);
			}
		}
	}
}

const wchar_t* mlRMMLElement::GetEventName(char aidEvent){
	const wchar_t* pwcEvName=NULL;
	if(aidEvent < TIDB_mlRMMLElement){
		if(aidEvent < 0){ // user event
			if(mpListeners.get()!=NULL)
				pwcEvName = mpListeners->GetUserEventName(aidEvent);
		}else{
			// specific element event
			EventSpec* pEventSpec=GetEventSpec();
			if(pEventSpec!=NULL){
				while(pEventSpec->name){
					if(pEventSpec->id==aidEvent){
						pwcEvName=pEventSpec->name;
						break;
					}
					pEventSpec++;
				}
			}
		}
	}else{
		// prototype event
//			ML_ASSERTION(aEventIsPW[aidEvent]==0,"mlRMMLElement::CallListeners");
		pwcEvName = aEventNames[aidEvent];
		// если нужно имя свойства в юникоде
		if(aEventIsPW[aidEvent]){
			pwcEvName = aPropNames[aidEvent].c_str();
			// если имя свойства в юникоде запрашивается первый раз,
			if(pwcEvName == NULL || *pwcEvName == L'\0'){
				// то делаем его из неюникодного и сохраняем на следующие запросы
				aPropNames[aidEvent] = cLPWCSTR((const char*)aEventNames[aidEvent]);
				pwcEvName = aPropNames[aidEvent].c_str();
			}
		}
	}
	return pwcEvName;
}

__forceinline jsval mlRMMLElement::GetEventJSVal(char aidEvent){
	const wchar_t* pwcEvName = GetEventName(aidEvent);
	if(pwcEvName == NULL) 
		return JSVAL_NULL;
	// если есть свойство onXXX
	jsval vFunc = JSVAL_NULL;
	if(wr_JS_GetUCProperty(mcx, mjso, pwcEvName, wcslen(pwcEvName), &vFunc) == JS_FALSE)
		return JSVAL_NULL;
	if(JSVAL_IS_STRING(vFunc))
		return vFunc;
	if(JSVAL_IS_REAL_OBJECT(vFunc) && JS_ObjectIsFunction(mcx, JSVAL_TO_OBJECT(vFunc)))
		return vFunc;
	return JSVAL_NULL;
}

void mlRMMLElement::SetEventJSVal(char aidEvent, jsval aValue){
	const wchar_t* pwcEvName = GetEventName(aidEvent);
	if(pwcEvName == NULL) 
		return;
	wr_JS_SetUCProperty(mcx, mjso, pwcEvName, wcslen(pwcEvName), &aValue);
}

int mlRMMLElement::CheckUserEvent(JSString* ajssEventID, mlRMMLElement* apThis, jsval *vp){
	char idEvent = apThis->GetUserEventID(wr_JS_GetStringChars(ajssEventID));
	if(idEvent == 0) return -1; // is not user event
	return GPSM(apThis->mcx)->CheckEvent(idEvent, apThis, vp);
}

void mlRMMLElement::CallListeners(char aidEvent, bool abAllEvents, bool abProp){
	// проверяем: разрешено ли элементу обрабатывать это событие
	mlRMMLVisible* pVis=GetVisible();
	if(pVis!=NULL && !(pVis->CanHandleEvent(aidEvent))) return;
	mlRMML3DObject* pObj=Get3DObject();
	if(pObj!=NULL && !(pObj->CanHandleEvent(aidEvent))) return;
	mlRMMLPButton* pBut=GetButton();
	bool bNeedToHandleButEv=false;
	if(pBut!=NULL && !(pBut->CanHandleEvent(aidEvent,abAllEvents,bNeedToHandleButEv))) return;
	// проверяем на наличие обработчиков 
	jsval vFunc = JSVAL_NULL;
	if(!abProp && IsJSO())
		vFunc = GetEventJSVal(aidEvent);
	pv_elems pv=NULL;
	if(mpListeners.get()!=NULL){
		pv=mpListeners->Get(aidEvent);
		if(pv!=NULL && pv->size()==0) pv=NULL;
	}
	// если обработчиков нет, то выходим
	if(!bNeedToHandleButEv && vFunc == JSVAL_NULL && pv == NULL) return;
	//
	if(GPSM(mcx)->mbSceneMayHaveSynchEls &&
	   !GPSM(mcx)->mbDontSendSynchEvents && 
	   NeedToSynchronize() && 
	   GPSM(mcx)->GetContext()->mpSynch!=NULL){
		// если 2-й режим синхронизации объекта (частичная синхронизация)
		if(mchSynchronized == 2){
			// то обработчики событий не синхронизируются вообще
		}else{
			// если есть обработчики в script-элементах
			mlSynchData oData;
			oData << aidEvent;
			oData << abAllEvents;
			if(pVis!=NULL) pVis->Vis_GetData4Synch(mcx, aidEvent,oData);
			if(pBut!=NULL) pBut->But_GetData4Synch(mcx, aidEvent,oData);
			moMedia* pMO=GetMO();
			if(!PMO_IS_NULL(pMO)) pMO->GetSynchData(oData);
			// ??
/*
			if(pv!=NULL){
				// собираем данные для синхронизации
				for(v_elems::iterator vi=pv->begin(); vi!=pv->end(); vi++){
					mlRMMLElement* pMLEl=*vi;
					if(pMLEl->mRMMLType==MLMT_SCRIPT){
						mlRMMLScript* pScript=(mlRMMLScript*)pMLEl;
//	??					pScript->GetSynchData(vData);
					}
				}
			}
*/
			// посылаем запрос на возможность выполнить обработчик события
			int iID = GetID();
			if(iID != OBJ_ID_UNDEF){
				mlISinchronizer* pSynch = GPSM(mcx)->GetContext()->mpSynch;
				omsresult res=pSynch->CallEvent(iID, (unsigned char*)oData.data(), oData.size());
				if(OMS_SUCCEEDED(res))
					return;
			}
		}
	}
	if(pBut!=NULL && bNeedToHandleButEv) pBut->HandleEvent(aidEvent); // onSetFocus, onKeyDown, onKeyUp
if(aidEvent == mlRMMLContinuous::EVID_onPlayed)
int hh=0;
	// выполняем onXXX
	if(vFunc != JSVAL_NULL){
//		RelinkClassFunction(vFunc, pwcEvName);
		// вызываем функцию
		jsval v;
		if(JSVAL_IS_STRING(vFunc)){
			JSString* jss = JSVAL_TO_STRING(vFunc);
			JS_EvaluateUCScript(mcx, mjso, JS_GetStringChars(jss), JS_GetStringLength(jss), 
				cLPCSTRq(GPSM(mcx)->mSrcFileColl[miSrcFilesIdx]), miSrcLine, &v);
		}else{
//			JS_CallFunction(mcx, JS_GetParent(mcx, JS_GetFunctionObject(jsf)), jsf, 0, NULL, &v);
//			JS_CallFunction(mcx, mjso, jsf, 0, NULL, &v);
			JS_CallFunctionValue(mcx, mjso, vFunc, 0, NULL, &v);
		}
	}
	//
	if(pv==NULL) return;
	//v_elems vc=*pv; pv=&vc;
	mpListeners->ClearChanged();

	if(mpListeners.get()!=NULL){
		pv=mpListeners->Get(aidEvent);
		if(pv!=NULL && pv->size()==0) pv=NULL;
	}

	if(pv==NULL) return;

	v_elems::iterator vi=pv->begin();
	int iELCnt = pv->size();
	if(aidEvent < 0 && vi!=pv->end()){ vi++; iELCnt--; } // skip user event id
	if(iELCnt <= 0) return;
	// сделать копию списка слушателей и идти по нему сверяясь с наличием слешателя в 
	// исходном списке, если он изменился
	mlRMMLElement** paListeners = MP_NEW_ARR(mlRMMLElement*, (iELCnt+1)*sizeof(mlRMMLElement*));
	mlRMMLElement** paListenersIt = paListeners;
#if _MSC_VER > 1200
	mlRMMLElement** paListenersSrc = &(*vi);
#else
	mlRMMLElement** paListenersSrc = vi;
#endif
	memcpy(paListeners, paListenersSrc, iELCnt*sizeof(mlRMMLElement*));
	paListeners[iELCnt] = NULL;
	for(;; paListenersIt++){
		mlRMMLElement* pMLEl=*paListenersIt;
		if(pMLEl == NULL) break;
		// если во время обработки события изменился список слушателей,
		if(mpListeners.get() == NULL)
			break;
		if(mpListeners->IsChanged()){
			// то проверяем его наличие в изменившемся списке
			pv = mpListeners->Get(aidEvent);
			if(pv != NULL && pv->size() == 0) pv = NULL;
			if(pv == NULL) break;
			v_elems::iterator vi;
			for(vi = pv->begin(); vi != pv->end(); vi++){
				if(*vi == pMLEl) break;
			}
			if(vi == pv->end()) continue;
		}
		if(pMLEl->EventNotify(aidEvent,this)){
			// надо удалить его из списка, 
			// сохранив флаг измененности списка неизменным
			bool bWasChanged = mpListeners->IsChanged();
			mpListeners->Remove(aidEvent, pMLEl);
			if(!bWasChanged)
				mpListeners->ClearChanged();
		}
	}
	MP_DELETE_ARR( paListeners);
//	while(vi!=pv->end()){
//		mlRMMLElement* pMLEl=*vi;
//		if(pMLEl->EventNotify(aidEvent,this)){
//			// надо удалить его из списка
//			pv->erase(vi);
//		}else 
//			vi++;
//		// ?? если во время обработки события изменился список слушателей,
//		// то прерываем обработку события, т.к. vi и pv могут оказаться неправильными
//		if(mpListeners->IsChanged()) break; 
//	}
}

EventSender* mlRMMLElement::GetEventSender(const wchar_t* apwcEvent){
//if(isEqual(apwcEvent,L"onResize") && isEqual(mpName, L"edLogin"))
//int hh=0;
	mlRMMLElement* pMLElSender=this;
	ML_ASSERTION(mcx, pMLElSender!=NULL,"mlRMMLElement::GetEventSender");
	wchar_t* pwc=(wchar_t*)apwcEvent;
	wchar_t* pwcDot=NULL;
	while((pwcDot=(wchar_t*)findDot(pwc))!=NULL){
		*pwcDot=L'\0';
		char idGOEvent;
		char idGObj;
		if((pwc==apwcEvent) && GPSM(mcx)->IsGlobalObjectEvent(pwc,pwcDot+1,idGObj,idGOEvent)){
			if(idGOEvent<0) return NULL;
			EventSender *eventSender = NULL;
			MP_NEW_V2( eventSender, EventSender, idGOEvent, (mlRMMLElement*)idGObj);
			return eventSender;
		}
		if((pwc == apwcEvent) && isEqual(pwc, L"this")){ 
			// pMLElSender и так уже =this
		}else if((pwc == apwcEvent) && isEqual(pwc,L"scene")){
			pMLElSender=(mlRMMLElement*)GetScene();
		}else if((pwc == apwcEvent) && isEqual(pwc,L"Player") && isEqual(pwcDot+1, L"UI.", 3) /* &&
			GetScene() == GPSM(mcx)->GetUI() */) // commented 14.02.2003 by Tandy
		{
			pMLElSender=(mlRMMLElement*)GPSM(mcx)->GetUI();
			*pwcDot = L'.';
			pwc = pwcDot + 1 + 3;
			continue;
		}else
			pMLElSender=pMLElSender->GetChild(pwc);
		*pwcDot=L'.';
		if(pMLElSender==NULL) return NULL;
		pwc=pwcDot+1;
	}
	char idEvent=pMLElSender->GetEventID(pwc);
	if(idEvent==0){
		// try to get user event ID
		idEvent = pMLElSender->GetUserEventID(pwc);
		if(idEvent==0)
			return NULL;
	}
	EventSender *eventSender = NULL;
	MP_NEW_V2( eventSender, EventSender, idEvent, pMLElSender);
	return eventSender;
}

void mlRMMLElement::SetEventListener(char aidEvent, mlRMMLElement* apMLElListener){
	if(mpListeners.get()==NULL){
		EventSpec* pEventSpec=GetEventSpec();
		if(pEventSpec==NULL) return;
		mpListeners = std::auto_ptr<mlListeners>(new mlListeners(pEventSpec));
	}
	mpListeners->Set(aidEvent,apMLElListener);
	// если это мышиное событие
	if(aidEvent>=mlRMML3DObject::EVID_onMouseDown && aidEvent<=mlRMML3DObject::EVID_onMouseUp)
		aidEvent=(aidEvent - mlRMML3DObject::EVID_onMouseDown) + mlRMMLVisible::EVID_onMouseDown;
	if((aidEvent>=mlRMMLVisible::EVID_onMouseDown && aidEvent<=mlRMMLVisible::EVID_onMouseWheel)){
		// то зарегистрировать себя как слушателя мышиных событий
		ML_ASSERTION(mcx, IsJSO(),"mlRMMLElement::SetEventListener");
		//jsval vProp;
		//if(!JS_GetProperty(mcx, JS_GetGlobalObject(mcx), GJSONM_MOUSE, &vProp))
		//	return;
		//JSObject* jso=JSVAL_TO_OBJECT(vProp);
		//mlSceneManager* pSM=(mlSceneManager*)JS_GetPrivate(mcx,jso);
		GPSM(mcx)->addMouseListener(aidEvent,this);
	}
	// если это кнопочное событие
	if(aidEvent>=mlRMMLButton::EVID_onPress && aidEvent<=mlRMMLButton::EVID_onRollOut){
		mbBtnEvListenersIsSet=true;
//		mlRMMLElement* pParent=this;
//		while(pParent!=NULL){
//			pParent->mbBtnEvListenersIsSet=true;
//			pParent=pParent->mpParent;
//		}
	}
	if(aidEvent>=mlRMMLButton::EVID_onKeyDown && aidEvent<=mlRMMLButton::EVID_onKeyUp){
		ML_ASSERTION(mcx, IsJSO(),"mlRMMLElement::SetEventListener");
		mlRMMLPButton* pBut = GetButton();
		if(pBut != NULL)
			pBut->KeyEvListenersIsSet();
		GPSM(mcx)->addKeyListener(aidEvent,this);
	}
}

void mlRMMLElement::RemoveEventListener(char aidEvent, mlRMMLElement* apMLElListener){
	if(mpListeners.get()==NULL) return;
	mpListeners->Remove(aidEvent,apMLElListener);
	// если это кнопочное событие
	if(aidEvent>=mlRMMLButton::EVID_onPress && aidEvent<=mlRMMLButton::EVID_onRollOut){
		// и больше слушателей кнопочных событий нет
		mbBtnEvListenersIsSet=false;
		for(char idEv=mlRMMLButton::EVID_onPress; idEv<=mlRMMLButton::EVID_onRollOut; idEv++){
			pv_elems pv=mpListeners->Get(idEv);
			if(pv!=NULL){
				mbBtnEvListenersIsSet=true;
				break;
			}
		}
	}
}

SynchPropEventSender* mlRMMLElement::GetSynchPropEventSender(const wchar_t* apwcEvent)
{
	mlRMMLElement* pMLElSender=this;
	ML_ASSERTION(mcx, pMLElSender!=NULL,"mlRMMLElement::GetEventSender");
	wchar_t* pwc=(wchar_t*)apwcEvent;
	wchar_t* pwcDot=NULL;
	while((pwcDot=(wchar_t*)findDot(pwc))!=NULL){
		*pwcDot=L'\0';
		if(isEqual(pwc,L"synch")){
			*pwcDot=L'.';
			pwc=pwcDot+1;
			break;
		}
		if((pwc==apwcEvent) && isEqual(pwc,L"scene")){
			pMLElSender = (mlRMMLElement*)GetScene();
		}else
			pMLElSender = pMLElSender->GetChild(pwc);
		*pwcDot=L'.';
		if(pMLElSender==NULL) return NULL;
		pwc=pwcDot+1;
	}
	SynchPropEventSender *spEventSender = NULL;
	MP_NEW_V2( spEventSender, SynchPropEventSender, pwc, pMLElSender);
	return spEventSender;
}

void mlRMMLElement::SetSynchPropEventListener(const wchar_t* apwcSynchProp, mlRMMLElement* apMLElListener)
{
	if(mpSynch == NULL)
		CreateSynchObject(NULL);

	if(mpSynch != NULL)
		mpSynch->SetEventListener(apwcSynchProp,apMLElListener);
}

void mlRMMLElement::RemoveSynchPropEventListener(const wchar_t* apwcSynchProp, mlRMMLElement* apMLElListener)
{
	if(mpSynch == NULL)
		return;
	mpSynch->RemoveEventListener(apwcSynchProp,apMLElListener);
}

#define SYNCHED_PROP_NAME "synched"
#define ISFULLSTATE_PROP_NAME "isFullState"

bool mlRMMLElement::IsSynchChanged()
{
	if(mpSynch == NULL)
		return false;
	bool bRet = mpSynch->IsSynchChanged();
	return bRet;
}

bool mlRMMLElement::GetForcedState( syncObjectState& aoState)
{
	if(mpSynch == NULL)
		return false;

	mpSynch->ForceVersions();
	ForceWholeStateSending();

	return GetSynchProps(aoState);
}

bool mlRMMLElement::GetSynchProps( syncObjectState& aState)
{
	if(mpSynch == NULL)
		return false;
	
	if(!mpSynch->IsSynchInitialized()) // BDima 01.06.2010. Было за коментаренно. Откомментарил. Приводит к пропаданию автаров
		return false;

	// В момент телепортации мы не посылаем события на сервер
	//if( GPSM(mcx)->IsSynchUpdatingSuspended())
	//	return false;

	if( !mpSynch->IsHashEvaluating())
	{
		// Исправление #252. 
		if (mpSynch->MayChange()!=MCERROR_NOERROR  && !mpSynch->IsFree())
			return false;
		// Конец #252

		if(mpSynch->DontSynch())
			return false;

		if (mRMMLType == MLMT_CHARACTER)
		{
			mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)this;
			moI3DObject* pmoI3DObject=pCharacter->GetmoI3DObject(); 
			if (!pmoI3DObject || !pmoI3DObject->HasAttachParent())
			{
				UpdateSynchZ( GPSM(mcx)->GetTime(), true);
			}
		}
	}

	bool bIsSubObjSynchChanged = mpSynch->IsSubObjSynchChanged();

	JSObject* jsoSynched = NULL;
	bool bRet = mpSynch->GetSynchProps(aState, jsoSynched);

	if( !mpSynch->IsHashEvaluating())
	{
		if (jsoSynched != NULL && IsRootReadyForSynch())
		{
			FireOnSynched(jsoSynched,  aState.IsFullState());
		}
	}

	if(!bIsSubObjSynchChanged)
	{
		mbNeedHashEvaluating |= bRet;
		return bRet;
	}

	if(mpChildren == NULL || mpChildren->empty())
	{
		mbNeedHashEvaluating |= bRet;
		return bRet;
	}

	v_elems* pvSynchedChildren = GetSynchedChildren();

	if(pvSynchedChildren != NULL)
	{
		aState.sub_objects.reserve(pvSynchedChildren->size());
		v_elems::iterator vi;
		for (vi = pvSynchedChildren->begin();  vi != pvSynchedChildren->end();  vi++ ){
			mlRMMLElement* pChild = *vi;
			if (pChild == NULL)
				continue;
			if(!pChild->IsSynchChanged())
				continue;
			if(pChild->_name == NULL)
				continue;
			// формируем name до parent-a с ID в dot-нотации
			std::string sPath = cLPCSTR(JS_GetStringChars(pChild->_name));
			mlRMMLElement* pParent = pChild->mpParent;
			while(pParent != this && pParent != NULL){
				if(pParent->_name == NULL){
					mlTrace(mcx, "Parents for %s  must be named for synchronization",sPath);;
					break;
				}
				sPath.insert(0, ".");
				sPath.insert(0, cLPCSTR(JS_GetStringChars(pParent->_name)));
				pParent = pParent->mpParent;
			}
			aState.sub_objects.push_back(syncObjectState());
			syncObjectState& subObjState = aState.sub_objects.back();
			subObjState.string_id = sPath;
			pChild->ComleteInitialization(); // раз родительский инициализирован, то можно считать, что и дочерние тоже
			bool bChRet = pChild->GetSynchProps(subObjState);
			bRet = true; // ?
		}
	}

	mbNeedHashEvaluating |= bRet;
	
	return bRet;
}

unsigned int mlRMMLElement::GetObjectStateHash()
{
	if( !mbNeedHashEvaluating)
		return muLastEvaluatedHash;

	omsContext* pOMSContext = GPSM(mcx)->GetContext();

	syncObjectState oStateFull;
	GetFullStateForHash(oStateFull);

#ifdef DEBUG
	bool abSave = false;
	/*if( this->muID == 4000000023)	//Alex avatar
	{
		abSave = true;

		std::wstring ws = pOMSContext->mpSM->GetJsonFormatedProps( GetStringProp("target"));
		CLogValue log("JsOn string: ", (const char*)cLPCSTR(ws.c_str()));
		pOMSContext->mpLogWriter->WriteLn( log);
	}*/
	muLastEvaluatedHash = pOMSContext->mpSyncMan->GetHashObj( oStateFull, abSave);
#else
	muLastEvaluatedHash = pOMSContext->mpSyncMan->GetHashObj( oStateFull, false);
#endif

	
	mbNeedHashEvaluating = false;

	return muLastEvaluatedHash;
}

bool mlRMMLElement::GetFullStateForHash(syncObjectState& aStateFull)
{
	if(mpSynch == NULL)
		return false;

	if(!mpSynch->IsSynchInitialized()) // BDima 01.06.2010. Было за коментаренно. Откомментарил. Приводит к пропаданию аватаров
		return false;

	mpSynch->BeginHashEvaluating();
	
	GetSynchProps( aStateFull);

	aStateFull.uiRealityID = 0;
	aStateFull.mwServerVersion = 0;
	aStateFull.mwClientVersion = 0;
	aStateFull.uiHashObj = 0;
	aStateFull.type = syncObjectState::ST_FULL;
	// Уберем координаты и зону, так как они не влияют на положение (все через скрипт идет),
	// а системные координаты могут немного отличаться за счет локальных погрешностей
	aStateFull.sys_props.miSet &= ~MLSYNCH_COORDINATE_XY_SET_MASK;
	aStateFull.sys_props.miSet &= ~MLSYNCH_COORDINATE_SET_MASK;
	aStateFull.sys_props.miSet &= ~MLSYNCH_ZONE_CHANGED_MASK;

	mpSynch->EndHashEvaluating();
	
	return true;
}

void mlRMMLElement::ResetSynchOnlyFireEvent()
{
	if (IsSynchronized())
	{
		mpSynch->ResetWaitForLoadingStatus();
		mpSynch->FireOnReset();

		if (muID == OBJ_ID_UNDEF)
			return;

		v_elems* pvSynchedChildren = GetSynchedChildren();
		if(pvSynchedChildren != NULL)
		{
			v_elems::iterator vi;
			for (vi = pvSynchedChildren->begin();  vi != pvSynchedChildren->end();  vi++ )
			{
				mlRMMLElement* pChild = *vi;
				if (pChild == NULL)
					continue;
				pChild->ResetSynchOnlyFireEvent();
			}
		}
	}
}

void mlRMMLElement::ResetSynch()
{	
	// Исправление #477
	if (IsSynchronized())
	{
		if (!IsRootReadyForSynch())
		{
			mpSynch->SetWaitForLoadingStatus();
		}

		mpSynch->Reset();
		if (muID == OBJ_ID_UNDEF)
			return;

		v_elems* pvSynchedChildren = GetSynchedChildren();
		if(pvSynchedChildren != NULL)
		{
			v_elems::iterator vi;
			for (vi = pvSynchedChildren->begin();  vi != pvSynchedChildren->end();  vi++ )
			{
				mlRMMLElement* pChild = *vi;
				if (pChild == NULL)
					continue;
				pChild->ResetSynch();
			}
		}
	}
	// Конец #477
}

void mlRMMLElement::ResetSynchedChildren()
{
	v_elems* pvSynchedChildren = GetSynchedChildren();

	if(pvSynchedChildren != NULL)
	{
		v_elems::iterator vi;
		for (vi = pvSynchedChildren->begin();  vi != pvSynchedChildren->end();  vi++ ){
			mlRMMLElement* pChild = *vi;
			if (pChild == NULL)
				continue;
			if(!pChild->IsSynchronized())
				continue;
			pChild->ResetSynch();
		}
	}
}

void mlRMMLElement::ResetSynchState()
{
	if (IsSynchronized())
	{
		mpSynch->ResetState();
		if (muID == OBJ_ID_UNDEF)
			return;

		v_elems* pvSynchedChildren = GetSynchedChildren();
		if(pvSynchedChildren != NULL)
		{
			v_elems::iterator vi;
			for (vi = pvSynchedChildren->begin();  vi != pvSynchedChildren->end();  vi++ )
			{
				mlRMMLElement* pChild = *vi;
				if (pChild == NULL)
					continue;
				pChild->ResetSynchState();
			}
		}
	}
}

void mlRMMLElement::ResetSynchStateChildren()
{
	v_elems* pvSynchedChildren = GetSynchedChildren();

	if(pvSynchedChildren != NULL)
	{
		v_elems::iterator vi;
		for (vi = pvSynchedChildren->begin();  vi != pvSynchedChildren->end();  vi++ ){
			mlRMMLElement* pChild = *vi;
			if (pChild == NULL)
				continue;
			if(!pChild->IsSynchronized())
				continue;
			pChild->ResetSynchState();
		}
	}
}

v_elems* mlRMMLElement::GetSynchedChildren()
{
	if(mpSynch == NULL)
		return NULL;

	if( muID == OBJ_ID_UNDEF)
	{
		// Объекты, не имеющие objectID, не должны иметь массива дочерних синхронизируемых элементов.
		// Вместо того, чтобы хранить их у себя, они должны передавать на них ссылки в самый "верхний" объект иерархии,
		// который имеет objectID
		//assert( false);
		return NULL;
	}

	// перебрать дочерние элементы с synch
	v_elems* pvSynchedChildren = mpSynch->GetSynchedChildren();
	if(pvSynchedChildren == ML_SYNCH_CHILDREN_EMPTY_VAL) // если уже искали, но не нашли, то выходим
		return NULL;

	if(pvSynchedChildren == NULL){
		// сформировать список синхронизируемых дочерних объектов и сложить его в mpSynch в целях экономии памяти
		mlRMMLIterator iter(mcx, this, true);
		mlRMMLElement* pChild = NULL;
		while((pChild=iter.GetNext())!=NULL){
			if(pChild == NULL || pChild == this)
				continue;
			if(!pChild->IsJSO())
				continue;
			if(!pChild->IsSynchronized())
				continue;
			if (pvSynchedChildren == NULL)
				pvSynchedChildren = mpSynch->CreateSynchedChildren();
			pvSynchedChildren->push_back( pChild);
		}
		if(pvSynchedChildren == NULL)
			mpSynch->SetSynchedChildrenEmpty();
	}
	return pvSynchedChildren;
}

void mlRMMLElement::ForceWholeStateSending()
{
	if(mpSynch == NULL)
		return;
	mpSynch->ForceWholeStateSending();
	if (mpChildren != NULL)
	{
		v_elems::iterator vi;
		for(vi = mpChildren->begin(); vi != mpChildren->end(); vi++){
			mlRMMLElement* pChild = *vi;
			pChild->ForceWholeStateSending();
		}
	}
}

void mlRMMLElement::SkipSynchProps(mlSynchData& oData){
	MLWORD wPropsCnt = 0;
	oData >> wPropsCnt;
	for(int jj = 0; jj < wPropsCnt; jj++){
		wchar_t* pwcPropName;
		oData >> pwcPropName;
		MLWORD wValueLength = 0;
		oData >> wValueLength;
		oData.skip(wValueLength);
	}
}

bool mlRMMLElement::IsSynchronized(){
	if (mpSynch != NULL)
		return true;
	else
		return false;
}

bool mlRMMLElement::IsSynchInitialized()
{
	if (mpSynch == NULL)
		return true;
	return mpSynch->IsSynchInitialized();
}

void mlRMMLElement::ComleteInitialization()
{
	if (mpSynch == NULL)
		return;
	mpSynch->ComleteInitialization();
}

bool mlRMMLElement::IsExclusiveOwn(){
	if (mpSynch == NULL)
		return false;
	if ((mpSynch->GetSynchFlags() & MLSYNCH_EXCLUSIVE_OWN_MASK) != 0)
		return true;
	else
		return false;
}

ESynchStateError mlRMMLElement::SetSynchProps( const syncObjectState& aState)
{
	if (mpSynch == NULL)
		return SSE_NOT_SYNCRONIZED;

	mbNeedHashEvaluating = true;

	JSObject* jsoSynched = JS_NewObject(mcx, NULL, NULL, NULL);
	jsval jsv = OBJECT_TO_JSVAL(jsoSynched);

	if (aState.IsNotFound())
	{
		// Исправление #640 
		// Для вновь создаваемых синхронизирумых объектов надо устанавливать текущую реальность.
		mpSynch->RealityChanged(aState.uiRealityID);
	}
	else
	{
		JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), SYNCHED_PROP_NAME);
		SAVE_FROM_GC(mcx, GPSM(mcx)->GetEventGO(), jsoSynched);

		if (aState.IsForceState() || (aState.IsFullState() && mpSynch->IsApplyableFullState(aState)))
		{
			ResetSynchState();
			ResetSynchStateChildren();
		}

		bool synchedEmpty;
		ESynchStateError error = mpSynch->SetSynchProps( aState, jsoSynched, synchedEmpty);
		if (error != SSE_NO_ERRORS)
			return error;

		syncObjectStateVector::const_iterator it = aState.sub_objects.begin();
		for (;  it != aState.sub_objects.end();  it++)
		{
			const syncObjectState& subobjectState = *it;
			mlRMMLElement* pChild = NULL;
			pChild = FindElemByName( cLPWCSTR( subobjectState.string_id.c_str()));
			if(pChild == NULL)
				continue;
			pChild->SetSynchProps( subobjectState);
		}

		if (!IsRootReadyForSynch())
		{
			mpSynch->SetWaitForLoadingStatus();
		}
		else if (!synchedEmpty)
		{
			FireOnSynched(jsoSynched,  aState.IsFullState());
		}
		FREE_FOR_GC(mcx, GPSM(mcx)->GetEventGO(), jsoSynched);
	}

	bool needSendFullState = aState.IsNotFound();
	needSendFullState = needSendFullState && !mpSynch->IsSynchInitialized() && (mpSynch->IsMyOwn() || mpSynch->IsFree());

	FinishInitialization( jsv);

	// Для объектов, которые не захвачены мной, позицию Z берем из синхронизации
	if( GetICharacter())
	{
		GetICharacter()->SynchronizeZPosition();
	}

	/*if (!mpSynch->IsMyOwn())
	{
		mlRMML3DObject* p3DObj = Get3DObject();
		if (p3DObj != NULL && mRMMLType == MLMT_CHARACTER)
		{
			// SetPos_z - просто вызывает установку правитльной позиции из mlRMMLCharacter::PosChanged
			p3DObj->SetPos_z( p3DObj->GetPos_z());	// реальная позиция установится в void mlRMMLCharacter::PosChanged(ml3DPosition &aPos)
		}
	}*/

	if (needSendFullState)
		ForceWholeStateSending();

	return SSE_NO_ERRORS;
}

void mlRMMLElement::FinishInitialization( jsval jsv, bool abForce)
{
	if (mpSynch->IsSynchInitialized() && !abForce)
		return;

	if (mpSynch->IsWaitForLoading())
		return;

	if(mpChildren != NULL)
	{
		JSObject* jsoEmpty = JS_NewObject(mcx, NULL, NULL, NULL);
		jsval jsvEmpty = OBJECT_TO_JSVAL( jsoEmpty);
		JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), SYNCHED_PROP_NAME, &jsv);
		v_elems::iterator vi;
		for (vi = mpChildren->begin();  vi != mpChildren->end();  vi++ )
		{
			mlRMMLElement* pChild = *vi;
			if(pChild == NULL)
				continue;
			if(!pChild->IsJSO())
				continue; // skip scripts
			if(!pChild->IsSynchronized())
				continue;
			pChild->FinishInitialization( jsvEmpty, abForce);
		}
	}

	// сгенерировать synch.onInitialized
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), SYNCHED_PROP_NAME, &jsv);
	FireSynchOnInitialized();
	JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), SYNCHED_PROP_NAME);

	mpSynch->ComleteInitialization();
}

bool mlRMMLElement::Capture()
{
	if (mpSynch == NULL)
		return false;

	return mpSynch->Capture();
}

void mlRMMLElement::SetSynchFlags(unsigned int auiFlags){
	if (mpSynch == NULL)
		return;
	mpSynch->SetSynchFlags(auiFlags);
}

unsigned int mlRMMLElement::GetSynchFlags(){
	if (mpSynch == NULL)
		return 0;
	return mpSynch->GetSynchFlags();
}

//mlresult mlRMMLElement::Render(const __int64 alTime,
//                               mme::RenderDevice &device)
//{
//	if (mpMO)
//    mpMO->Render(alTime, device);
//
//	return ML_OK;
//}

bool mlRMMLElement::IsPointIn(mlPoint &aPnt){
	mlRMMLPButton* pBut=GetButton();
	if(pBut){
		if(!pBut->enabled) return false;
		return pBut->Button_IsPointIn(aPnt);
	}
	mlRMMLVisible* pVis=GetVisible();
	if(pVis){
		return pVis->Visible_IsPointIn(aPnt);
	}
	return false;
}

mlRMMLElement* mlRMMLElement::GetButtonUnderMouse(mlPoint &aMouseXY, mlButtonUnderMouse &aBUM){ 
	//if(!mbBtnEvListenersIsSet) return NULL; // optimization disabled for bug fix by Tandy 11.04.06
	//ML_ASSERTION(mcx, GetVisible()!=NULL,"mlRMMLElement::GetButtonUnderMouse");
	if (!GetVisible()) return NULL;
	if(!(GetVisible()->mbVisible)) return NULL;
	if(IsPointIn(aMouseXY)){
		aBUM.mpVisible = this->GetVisible();
		return this;
	}
	return NULL; 
}

mlMedia* mlRMMLElement::GetScene(){
	if(mRMMLType==MLMT_SCENE) return this;
	mlRMMLElement* pParent=mpParent;
	while(pParent!=NULL){
		if(pParent->mRMMLType==MLMT_SCENE) 
			return pParent;
		pParent=pParent->mpParent;
	}
	return NULL;
}

bool mlRMMLElement::IsAllParentComposLoaded(){
	mlRMMLElement* pParent=this;
	while(pParent!=NULL){
		if(pParent->mpMO == NULL)
			return false;
		if(pParent->mRMMLType==MLMT_SCENE) 
			return true;
		pParent=pParent->mpParent;
	}
	return false;
}

mlRMMLElement* mlRMMLElement::GetChild(unsigned short aRMMLType, unsigned short aNum){
	if(mpChildren!=NULL){
		v_elems::iterator vi;
		unsigned short uNum=0;
		for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++){
			mlRMMLElement* pMLEl=*vi;
			if(pMLEl->mRMMLType==aRMMLType){
				if(uNum==aNum)
					return pMLEl;
				uNum++;
			}
		}
	}
	return NULL;
}

bool mlRMMLElement::IsChild(mlRMMLElement* apMLEl){
	if(mpChildren==NULL) return false;
	v_elems::iterator vi;
	for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++){
		if(apMLEl==*vi) return true;
	}
	return false;
}

mlRMMLElement* mlRMMLElement::GetChildByIdx(unsigned short auNum){
	if(mpChildren==NULL) return NULL;
	if(auNum >= mpChildren->size()) return NULL;
	return (*mpChildren)[auNum];
}

short mlRMMLElement::GetChildIdx(mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return -1;
	if(mpChildren==NULL) return -1;
	v_elems::iterator vi; int ii=0;
	for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++,ii++){
		if(apMLEl==*vi) return ii;
	}
	return -1;
}

// для взятия ID в authoring-е
mlRMMLElement* mlRMMLElement::FindChildByLastNamed(const wchar_t* apwcName, int aiShift){
	if(mpChildren==NULL) return NULL;
	v_elems::reverse_iterator vri = mpChildren->rbegin();
	// если имя указано, то ищем сначала по имени
	if(apwcName != NULL && *apwcName != L'\0'){
		for(; vri != mpChildren->rend(); vri++){
			mlRMMLElement* pMLEl = *vri;
			if(pMLEl->_name == NULL) continue;
			if(isEqual(pMLEl->_name, apwcName)){
				if(aiShift == 0) return pMLEl;
				break;
			}
		}
		// если не нашли, то выходим нисчем
		if (vri == mpChildren->rend())
			return NULL;
	}
	// ищем по смещению
	unsigned short uNum=0;
	for(; vri != mpChildren->rend(); vri++){
		if(uNum == aiShift){
			mlRMMLElement* pMLEl = *vri;
			return pMLEl;
		}
		uNum++;
	}
	return NULL;
}

mlresult mlRMMLElement::CreateAndLoadMOs(mlCreateAndLoadMOsInfo* apCLMOInfo){
	omsContext* pOMSContext = GPSM(mcx)->GetContext();
	unsigned long ulStartTime = 0;
	if(apCLMOInfo != NULL && apCLMOInfo->miTimeQuota > 0){
		ulStartTime = pOMSContext->mpApp->GetTimeStamp();
	}
	mlRMMLIterator Iter(mcx, this, true);
	mlRMMLElement* pElem = NULL;
	while((pElem=Iter.GetNext()) != NULL){
//if(pElem->_name!=NULL){
//JSString* jss=pElem->_name;
//char* pch=JS_GetStringBytes(jss);
//for(int ii=Iter.GetDepth(); ii>0; ii--) TRACE("  ");
//TRACE("%s\n",pch);
//}
		if(!pElem->IsComposition()){
			// создание медиа-объекта
			if(pElem->GetMO() == NULL || pElem->mpMO == PMOV_DESTROYED){
				pElem->mpMO = NULL;
				mlDontSendSynchEvents oDSSE(GPSM(mcx));
				pElem->CreateMedia(pOMSContext);
				// загрузка
				pElem->Load();
				if(pElem->GetMO() == NULL){
					pElem->mpMO = PMOV_CREATED;
				}
				if(apCLMOInfo != NULL){
					apCLMOInfo->miLoaded++;
					if(apCLMOInfo->miTimeQuota > 0){
						unsigned long ulCurTime = pOMSContext->mpApp->GetTimeStamp();
						if((int)(ulCurTime - ulStartTime) >= apCLMOInfo->miTimeQuota){
							if(apCLMOInfo->miLoaded >= apCLMOInfo->miToLoad)
								break;
						}
					}else{
						if(apCLMOInfo->miToLoad >= 0)
							if(--apCLMOInfo->miToLoad <= 0) break;
					}
				}
			}
		}else{
			pElem->mpMO = PMOV_CREATED;
		}
	}
	return ML_OK;
}

mlresult mlRMMLElement::ActivateScripts(){
	mlDontSendSynchEvents oDSSE(GPSM(mcx));
	if(GetScene() == GPSM(mcx)->GetUI())
		GPSM(mcx)->SetActivationUIScriptsFlag(this);
	mlRMMLIterator Iter(mcx,this);
	//Iter.Reset(pScene);
	mlRMMLElement* pElem=NULL;
	while((pElem=Iter.GetNext())!=NULL){
		if(pElem->mRMMLType==MLMT_SCRIPT){ // default 
			mlRMMLScript* pScript=(mlRMMLScript*)pElem;
			if(pElem->IsOnloadScript()){
				if(pElem->mpParent->mRMMLType!=MLMT_SEQUENCER){
					if(pScript->mpMO != PMOV_ACTIVATED){
						pScript->Execute(NULL);
						pScript->mpMO = PMOV_ACTIVATED;
					}
				}
			}else{
				if(pScript->mpMO != PMOV_ACTIVATED){
					pScript->Activate();
					pScript->mpMO = PMOV_ACTIVATED;
				}
			}
		}
	}
	AllParentComposLoaded();
	// если есть callback-функция
	if(mpCreationCallback != NULL && mpCreationCallback->mFunc != NULL)
		mpCreationCallback->mFunc((mlMedia*)this, mpCreationCallback->mpParams);
	GPSM(mcx)->ResetActivationUIScriptsFlag(this);
	return ML_OK;
}

mlresult mlRMMLElement::AllParentComposLoaded(){
	if(!IsAllParentComposLoaded())
		return ML_ERROR_NOT_INITIALIZED;
//	ML_ASSERTION(IsAllParentComposLoaded(),"mlRMMLElement::AllParentComposLoaded(1)")
	mlRMMLIterator Iter(mcx,this);
	mlRMMLElement* pElem=NULL;
	while((pElem=Iter.GetNext())!=NULL){
		if(PMO_IS_NULL(pElem->mpMO))
			continue;

if(!pElem->IsAllParentComposLoaded())
int hh=0;
		ML_ASSERTION(mcx, pElem->IsAllParentComposLoaded(),"mlRMMLElement::AllParentComposLoaded(2)")
		pElem->mpMO->AllParentComposLoaded();
	}
	return ML_OK;
}

void mlRMMLElement::FrozenChanged(){
	if(_frozen){
		Freeze();
	}else{
		Unfreeze();
	}
}

bool mlRMMLElement::FreezeChildren(){
	if(mpChildren==NULL) return false;
	bool bOnePaused=false;
	for(v_elems::iterator vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
		mlRMMLElement* pMLEl=*vi;
		if(pMLEl->Freeze()) bOnePaused=true;
	}
	return bOnePaused;
}

bool mlRMMLElement::UnfreezeChildren(){
	if(mpChildren==NULL) return false;
	bool bOneResumed=false;
	for(v_elems::iterator vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
		mlRMMLElement* pMLEl=*vi;
		if(pMLEl->Unfreeze()) bOneResumed=true;;
	}
	return bOneResumed;
}

//// структуры для передачи свойств
//struct mldPropertiesInfo{
//	short numGroup;
//};
//struct mldPropGroupInfo{
//	short numPropeties;
//	int type;
//};

//#define MLMPT_LOADABLE (1<<(8+0))
//#define MLMPT_VISIBLE (1<<(8+1))
//#define MLMPT_AUDIBLE (1<<(8+2))
//#define MLMPT_CONTINUOUS (1<<(8+3))
//#define MLMPT_BUTTON (1<<(8+4))
//
//	virtual mlRMMLVisible* GetVisible(){ return NULL; }
//	virtual mlRMMLButton* GetButton(){ return NULL; }
//	virtual mlRMML3DObject* Get3DObject(){ return NULL; }

mlRMMLElement* GetRMMLElement2(JSContext* cx, JSObject* jso){
	JSClass* pClass=ML_JS_GETCLASS(cx, jso);
	if(pClass!=NULL && 
	   (pClass->flags & JSCLASS_HAS_PRIVATE) && JSO_IS_MLEL(cx, jso)
	){
		return (mlRMMLElement*)JS_GetPrivate(cx,jso);
	}
	return NULL;
}

mlString mlRMMLElement::GetFullPath(){
	mlString wsPath;
	wchar_t* pwcName=GetName();
	if(pwcName!=NULL) wsPath=pwcName;
	if(mpParent==NULL){
		if(IsScene()){
			if(this == GPSM(mcx)->GetUI())
				return L"Player.UI";
			return GJSOWNM_SCENE; 
		}
		wsPath = L"Classes." + wsPath;
		return wsPath;
	}
	mlRMMLElement* pParent=mpParent;
	while(pParent!=NULL){
		wsPath=L"."+wsPath;
		if(pParent->mpParent==NULL){ // scene
			if(pParent == GPSM(mcx)->GetUI()){
				wsPath = L"Player.UI" + wsPath;
			}else if(pParent == GPSM(mcx)->GetScene()){
				wsPath = (wchar_t*)GJSOWNM_SCENE + wsPath;
			}else{
				wsPath = pParent->GetName() + wsPath;
				wsPath = L"Classes." + wsPath;
			}
		}else
			wsPath=pParent->GetName()+wsPath;
		pParent=pParent->mpParent;
	}
	return wsPath;
}

mlString mlRMMLElement::GetElemRefName(mlRMMLElement* apElem){
	mlString wsPath;
	mlRMMLElement* pMLSender=apElem;
	if(pMLSender==mpParent){
		return pMLSender->GetFullPath();
	}
	std::vector<mlRMMLElement*> vSParents; // предки источника событий
	mlRMMLElement* pParent=pMLSender;
	while(pParent!=NULL){
		vSParents.insert(vSParents.begin(),pParent);
		pParent=pParent->mpParent;
	}
	std::vector<mlRMMLElement*> vLParents; // мои предки
	pParent=this;
	while(pParent!=NULL){
		vLParents.insert(vLParents.begin(),pParent);
		pParent=pParent->mpParent;
	}
	if(vSParents.size()==0 || vLParents.size()==0) 
		return wsPath;
	if(vSParents[0]!=vLParents[0]) 
		return wsPath;
	int iIdx;
	for(iIdx=0; vSParents[iIdx]==vLParents[iIdx]; iIdx++);
	iIdx--;
	pParent=vSParents[iIdx]; // ближайший общий предок
	int iIdx2=vLParents.size()-1;
	while(vLParents[iIdx2] != pParent){
		wsPath+=L"_parent.";
		iIdx2--;
	}
	iIdx++;
	while(iIdx < (int)vSParents.size()){
		wchar_t* wcsName=vSParents[iIdx]->GetName();
		if(wcsName==NULL)
			wsPath+=L"???";
		else
			wsPath+=wcsName;
		iIdx++;
		if(iIdx < (int)vSParents.size())
			wsPath+=L".";
	}
	return wsPath;
}

mlString mlRMMLElement::JSValToString(jsval av, char &acMLType){
	mlString wsVal;
	if(av==JSVAL_NULL) return wsVal;
	if(JSVAL_IS_INT(av)){
		acMLType=MLPT_INT;
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
		acMLType=MLPT_BOOLEAN;
		bool bVal=JSVAL_TO_BOOLEAN(av);
		if(bVal) wsVal=L"true";
		else wsVal=L"false";
		return wsVal;
	}else if(JSVAL_IS_NULL(av)){
		acMLType=MLPT_NULL;
		return wsVal;
	}else if(JSVAL_IS_STRING(av)){
		acMLType=MLPT_STRING;
		JSString* jssVal=JSVAL_TO_STRING(av);
		wsVal=wr_JS_GetStringChars(jssVal);
		return wsVal;
	}else if(JSVAL_IS_DOUBLE(av)){
		acMLType=MLPT_DOUBLE;
		jsdouble* dVal=JSVAL_TO_DOUBLE(av);
		wostringstream oss;
		oss << *dVal;
		return oss.rdbuf()->str();
	}else if(JSVAL_IS_OBJECT(av)){
		if(JS_TypeOfValue(mcx,av)==JSTYPE_FUNCTION){
			acMLType=MLPT_FUNCTION;
			// ??
			JSObject* jsoFun=JSVAL_TO_OBJECT(av);
			JSClass* pClass=ML_JS_GETCLASS(mcx, jsoFun);
//JSString * JS_DecompileScript(JSContext *cx, JSScript *script,
//    const char *name, uintN indent);
			if(pClass->flags==1){
				// Script
//	MLPT_EVENTSCR,
//				*this << PJSVT_NO;
				return L"[Script]";
			}else{
				JSFunction* jsf=JS_ValueToFunction(mcx, av);
				if(jsf){
//					JSString* jssFun=JS_DecompileFunction(mcx, jsf, 0);
//					jschar* jsc=JS_GetStringChars(jssFun);
//					*this << PJSVT_FUNCTION;
//					*this << jsc;
				}
				return L"[Function]";
			}
		}else{
			JSObject* jsoVal=JSVAL_TO_OBJECT(av);
			bool bElemRef=false;
			mlRMMLElement* pMLElProp=NULL;
			if(mpParent && mpParent->mjso == jsoVal){ 
				// _parent
				pMLElProp=mpParent;
				bElemRef=true;
			}else{
				pMLElProp=GetRMMLElement2(mcx, jsoVal);
				if(pMLElProp && pMLElProp->mpParent && pMLElProp->mpParent->mjso != mjso) // not child
					bElemRef=true;
			}
			if(bElemRef){
				acMLType=MLPT_ELEMREF;
				return GetElemRefName(pMLElProp);
			}else{ 
//	MLPT_POINT,
//	MLPT_SIZE,
//	MLPT_RECT,
//	MLPT_3DPOS,
//	MLPT_3DSCL,
//	MLPT_3DROT,
//				JSClass* pClass=JS_GetClass(jsoP);
//				// сохранить класс объекта
//				if(pClass!=NULL){
//					*this << pClass->name;
//				}else{
//					*this << " ";
//				}
//				*this << jsoP;
				if(mlPosition3D::IsInstance(mcx, jsoVal) ||
				   mlScale3D::IsInstance(mcx, jsoVal) ||
				   mlRotation3D::IsInstance(mcx, jsoVal)
				   ){
					JSIdArray* idArr=JS_Enumerate(mcx, jsoVal);
					for(int ij=0; ij<idArr->length; ij++){
						jsid id=idArr->vector[ij]; jsval v;
						if(!JS_IdToValue(mcx,id,&v)) continue;
						if(!JSVAL_IS_STRING(v)) continue;
						wchar_t* pjcPName=wr_JS_GetStringChars(JSVAL_TO_STRING(v));
						if(!wsVal.empty())
							wsVal+=L' ';
						wsVal+=pjcPName;
						wsVal+=L':';
						wr_JS_GetUCProperty(mcx, jsoVal, pjcPName, -1, &v);
						char ch;
						if(JSVAL_IS_STRING(v)){
							wsVal+=L"\"";
							wsVal+=wr_JS_GetStringChars(JSVAL_TO_STRING(v));
							wsVal+=L"\"";
						}else
							wsVal+=JSValToString(v, ch);
					}
					JS_DestroyIdArray(mcx, idArr);
					acMLType=MLPT_JSOREF;
					return wsVal;
				}
				acMLType=MLPT_JSOREF;
				return L"[Object]";
			}
		}
	}else{
		acMLType=MLPT_UNKNOWN;
	}
	return wsVal;
}

int mlRMMLElement::SavePropsInfo(mlSynchData &aData, JSPropertySpec* apPropSpec, mlPropNames &aPropNames, bool abSkipROProps){
	int iSkipProp=0;
	int iPropIdx = 0;
	for(iPropIdx=0; apPropSpec[iPropIdx].name!=NULL; iPropIdx++){
		const char* pszPropName=apPropSpec[iPropIdx].name;
		aPropNames.push_back(pszPropName);
		if(abSkipROProps && (apPropSpec[iPropIdx].flags & JSPROP_READONLY)){
			iSkipProp++;
			continue;
		}
		if( *pszPropName == '_' || 
			isEqual(pszPropName,"xMouse") || 
			isEqual(pszPropName,"yMouse") || 
			isEqual(pszPropName,"parent")
			){
			iSkipProp++;
			continue;
		}
		jsval vProp;
		JS_GetProperty(mcx,mjso,pszPropName,&vProp);
		char cPropType;
		mlString wsVal=JSValToString(vProp,cPropType);
		aData << pszPropName;
		aData << cPropType;
		aData << wsVal.c_str();
	}
	return iPropIdx-iSkipProp;
}

#define SAVE_PROPS_GROUP(SPEC) { \
		int iPGIPos=aData.getPos(); \
		aData.put(&PropGroupInfo,sizeof(mldPropGroupInfo)); \
		PropGroupInfo.numPropeties= \
			SavePropsInfo(aData, SPEC, vPropNames, abSkipROProps); \
		aData.setPos(iPGIPos); \
		aData.put(&PropGroupInfo,sizeof(mldPropGroupInfo)); \
		aData.setPos(); \
		PropsInfo.numGroup++; \
		}

bool mlRMMLElement::GetPropertiesInfo(mlSynchData &aData, bool abSkipROProps){
	mldPropertiesInfo PropsInfo;
	PropsInfo.numGroup = 0;
	int iPropsInfoPos = aData.getPos();
	aData.put(&PropsInfo,sizeof(mldPropertiesInfo));
	if(mRMMLType==MLMT_SCRIPT){
		return true;
	}
	//
	mlPropNames vPropNames;
	mldPropGroupInfo PropGroupInfo;
	PropGroupInfo.type = MLMPT_ELEMENT;
	SAVE_PROPS_GROUP(mlRMMLElement::_JSPropertySpec)
	// enum prototypes
	if(mRMMLType & MLMPT_LOADABLE){
		PropGroupInfo.type=MLMPT_LOADABLE;
		SAVE_PROPS_GROUP(mlRMMLLoadable::_JSPropertySpec)
	}
	if(mRMMLType & MLMPT_VISIBLE){
		PropGroupInfo.type=MLMPT_VISIBLE;
		SAVE_PROPS_GROUP(mlRMMLVisible::_JSPropertySpec)
	}
//	if(mRMMLType & MLMPT_AUDIBLE){
//		PropGroupInfo.type=MLMPT_AUDIBLE;
//		SAVE_PROPS_GROUP(mlRMMLAudible::_JSPropertySpec)
//	}
	if(mRMMLType & MLMPT_CONTINUOUS){
		PropGroupInfo.type=MLMPT_CONTINUOUS;
		SAVE_PROPS_GROUP(mlRMMLContinuous::_JSPropertySpec)
	}
	if(mRMMLType & MLMPT_BUTTON){
		PropGroupInfo.type=MLMPT_BUTTON;
		SAVE_PROPS_GROUP(mlRMMLPButton::_JSPropertySpec)
	}
	if(mRMMLType & MLMPT_3DOBJECT){
		PropGroupInfo.type=MLMPT_3DOBJECT;
		SAVE_PROPS_GROUP(mlRMML3DObject::_JSPropertySpec)
	}
	JSPropertySpec* pPropSpec=GetPropSpec();
	if(pPropSpec!=NULL && pPropSpec[0].name!=NULL){
		PropGroupInfo.type=mRMMLType;
		SAVE_PROPS_GROUP(pPropSpec)
	}
	// перебираем все свойства JS-объекта
	// если какого-то свойства нет, то записываем его в custom-свойства
	// (в отдельную группу надо вынести event-ы и watch-и)
	int iPGIPos=aData.getPos();
	PropGroupInfo.numPropeties=0;
	PropGroupInfo.type=MLMPT_CUSTOM;
	aData.put(&PropGroupInfo,sizeof(mldPropGroupInfo)); 
	JSIdArray* idArr=JS_Enumerate(mcx, mjso);
	for(int ij=0; ij<idArr->length; ij++){
		jsval v;
		if(!JS_IdToValue(mcx,idArr->vector[ij],&v)) continue;
		if(!JSVAL_IS_STRING(v)) continue;
		JSString* jssPName=JSVAL_TO_STRING(v);
		wchar_t* pjcPName=wr_JS_GetStringChars(jssPName);
		if(JS_GetStringLength(jssPName)==2){ 
			// возможно это SAVE_FROM_GC-свойство
			jsval v;
			wr_JS_GetUCProperty(mcx, mjso, pjcPName, -1, &v);
			if(JSVAL_IS_OBJECT(v)){
				JSObject* jsoT=JSVAL_TO_OBJECT(v);
				JSClass* pClass=ML_JS_GETCLASS(mcx, jsoT);
				if(pClass->flags==1) continue; // script
			}else if(JSVAL_IS_STRING(v)){
				JSString* jssT=JSVAL_TO_STRING(v);
				jschar* pjc=JS_GetStringChars(jssT);
				int hh=0;
			}
		}
		bool bNative=false;
		for(mlPropNamesIter vi=vPropNames.begin(); vi != vPropNames.end(); vi++ ){
			if(isEqual(pjcPName,cLPWCSTR(*vi))){
				bNative=true;
				break;
			}
		}
		if(bNative) continue;
		if(mpChildren!=NULL){
			bool bChild=false;
			for(v_elems_iter vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
				mlRMMLElement* pChild=*vi;
				wchar_t* pwcChildName=pChild->GetName();
				if(pwcChildName==NULL) continue;
				if(isEqual(pjcPName,pwcChildName)){
					bChild=true;
					break;
				}
			}
			if(bChild) continue;
		}
		wr_JS_GetUCProperty(mcx, mjso, pjcPName, -1, &v);
		char cPropType;
		mlString wsVal=JSValToString(v,cPropType);
		aData << (const char*)cLPCSTR(pjcPName);
		aData << cPropType;
		aData << wsVal.c_str();
		PropGroupInfo.numPropeties++;
	}
	JS_DestroyIdArray(mcx, idArr);
	aData.setPos(iPGIPos); 
	aData.put(&PropGroupInfo,sizeof(mldPropGroupInfo)); 
	aData.setPos(); 
	PropsInfo.numGroup++; 
	// обновляем количество групп
	aData.setPos(iPropsInfoPos);
	aData.put(&PropsInfo,sizeof(mldPropertiesInfo));
	vPropNames.clear();
	return true;
}

int mlRMMLElement::SaveEventsInfo(mlSynchData &aData, EventSpec* apEventSpec, mlEventNames &aEventNames){
	int iSkipEvent = 0;
	int iEventIdx = 0;
	for(iEventIdx = 0; apEventSpec[iEventIdx].name != NULL; iEventIdx++){
		if(apEventSpec[iEventIdx].prop){
			iSkipEvent++;
			continue;
		}
		const wchar_t* pwcEventName = apEventSpec[iEventIdx].name;
		aEventNames.push_back(pwcEventName);
		jsval vProp;
		wr_JS_GetUCProperty(mcx, mjso, pwcEventName, -1, &vProp);
		char cPropType;
		mlString wsVal = JSValToString(vProp, cPropType);
		aData << pwcEventName;
		aData << wsVal.c_str();
	}
	return iEventIdx - iSkipEvent;
}

#define SAVE_EVENTS_GROUP(SPEC) { \
		int iPGIPos=aData.getPos(); \
		aData.put(&EventGroupInfo,sizeof(mldEventsGroupInfo)); \
		EventGroupInfo.numEvents= \
			SaveEventsInfo(aData, SPEC, vEventNames); \
		aData.setPos(iPGIPos); \
		aData.put(&EventGroupInfo,sizeof(mldEventsGroupInfo)); \
		aData.setPos(); \
		EventsInfo.numGroup++; \
		}

bool mlRMMLElement::GetTypeEventsInfo(mlSynchData &aData){
	mldEventsInfo EventsInfo;
	EventsInfo.numGroup = 0;
	int iEventsInfoPos = aData.getPos();
	aData.put(&EventsInfo,sizeof(mldEventsInfo));
	if(mRMMLType==MLMT_SCRIPT){
		return true;
	}
	//
	mlEventNames vEventNames;
	mldEventsGroupInfo EventGroupInfo;
	EventGroupInfo.type=MLMPT_ELEMENT;
	SAVE_EVENTS_GROUP(mlRMMLElement::_EventSpec)
	// enum prototypes
	if(mRMMLType & MLMPT_LOADABLE){
		EventGroupInfo.type=MLMPT_LOADABLE;
		SAVE_EVENTS_GROUP(mlRMMLLoadable::_EventSpec)
	}
	if(mRMMLType & MLMPT_VISIBLE){
		EventGroupInfo.type=MLMPT_VISIBLE;
		SAVE_EVENTS_GROUP(mlRMMLVisible::_EventSpec)
	}
//	if(mRMMLType & MLMPT_AUDIBLE){
//		EventGroupInfo.type=MLMPT_AUDIBLE;
//		SAVE_EVENTS_GROUP(mlRMMLAudible::_EventSpec)
//	}
	if(mRMMLType & MLMPT_CONTINUOUS){
		EventGroupInfo.type=MLMPT_CONTINUOUS;
		SAVE_EVENTS_GROUP(mlRMMLContinuous::_EventSpec)
	}
	if(mRMMLType & MLMPT_BUTTON){
		EventGroupInfo.type=MLMPT_BUTTON;
		SAVE_EVENTS_GROUP(mlRMMLPButton::_EventSpec)
	}
	if(mRMMLType & MLMPT_3DOBJECT){
		EventGroupInfo.type=MLMPT_3DOBJECT;
		SAVE_EVENTS_GROUP(mlRMML3DObject::_EventSpec)
	}
	EventSpec* pEventSpec = GetEventSpec2();
	if(pEventSpec != NULL && pEventSpec[0].name != NULL){
		EventGroupInfo.type = mRMMLType;
		SAVE_EVENTS_GROUP(pEventSpec)
	}
	// перебираем все свойства JS-объекта
	// если какого-то свойства нет, то записываем его в custom-свойства
	// (в отдельную группу надо вынести event-ы и watch-и)
	int iPGIPos=aData.getPos();
	EventGroupInfo.numEvents=0;
	EventGroupInfo.type=MLMPT_CUSTOM;
	aData.put(&EventGroupInfo,sizeof(mldEventsGroupInfo)); 
	// 
	// ??
	aData.setPos(iPGIPos);
	aData.put(&EventGroupInfo,sizeof(mldEventsGroupInfo));
	aData.setPos();
	EventsInfo.numGroup++; 
	// обновляем количество групп
	aData.setPos(iEventsInfoPos);
	aData.put(&EventsInfo,sizeof(mldEventsInfo));
	vEventNames.clear();
	return true;
}

mlPropertyValues::~mlPropertyValues(){
	mlPropValVec::const_iterator vci;
	for(vci = mvPropVals.begin(); vci != mvPropVals.end(); vci++){
		mlPropertyValue* pPropVal = *vci;
		if(pPropVal->copy){
			char* name = (char*)(pPropVal->name);
			MP_DELETE_ARR( name);
		}
		MP_DELETE( pPropVal);
	}
	mvPropVals.clear();
}

mlPropertyValues::mlPropertyValue* mlPropertyValues::AddNew(){
	mlPropertyValue* pNewPropVal = MP_NEW( mlPropertyValue);
	mvPropVals.push_back(pNewPropVal);
	return pNewPropVal;
}

mlPropertyValues::mlPropertyValue* mlPropertyValues::Find(const char* alpcName) const {
	mlPropValVec::const_iterator vci;
	for(vci = mvPropVals.begin(); vci != mvPropVals.end(); vci++){
		if(isEqual((*vci)->name, alpcName)){
			return (*vci);
		}
	}
	return NULL;
}

#define SAVE_PROP_VALS(SPEC) { \
			for(int iPropIdx=0; SPEC[iPropIdx].name!=NULL; iPropIdx++){ \
				mlPropertyValues::mlPropertyValue* pPropVal = avPropVals.AddNew(); \
				pPropVal->copy = false; \
				const char* pszPropName = SPEC[iPropIdx].name; \
				pPropVal->name = pszPropName; \
				JS_GetProperty(mcx,mjso,pszPropName,&(pPropVal->val)); \
			} \
		}

void mlRMMLElement::GetAllProperties(mlPropertyValues &avPropVals){
	SAVE_PROP_VALS(mlRMMLElement::_JSPropertySpec)
	// enum prototypes
	if(mRMMLType & MLMPT_LOADABLE){
		SAVE_PROP_VALS(mlRMMLLoadable::_JSPropertySpec)
	}
	if(mRMMLType & MLMPT_VISIBLE){
		SAVE_PROP_VALS(mlRMMLVisible::_JSPropertySpec)
	}
//	if(mRMMLType & MLMPT_AUDIBLE){
//		SAVE_PROP_VALS(mlRMMLAudible::_JSPropertySpec)
//	}
	if(mRMMLType & MLMPT_CONTINUOUS){
		SAVE_PROP_VALS(mlRMMLContinuous::_JSPropertySpec)
	}
	if(mRMMLType & MLMPT_BUTTON){
		SAVE_PROP_VALS(mlRMMLPButton::_JSPropertySpec)
	}
	if(mRMMLType & MLMPT_3DOBJECT){
		SAVE_PROP_VALS(mlRMML3DObject::_JSPropertySpec)
	}
	JSPropertySpec* pPropSpec=GetPropSpec();
	if(pPropSpec!=NULL && pPropSpec[0].name!=NULL){
		SAVE_PROP_VALS(pPropSpec)
	}
	JSIdArray* idArr=JS_Enumerate(mcx, mjso);
	for(int ij=0; ij<idArr->length; ij++){
		jsval v;
		if(!JS_IdToValue(mcx,idArr->vector[ij],&v)) continue;
		if(!JSVAL_IS_STRING(v)) continue;
		JSString* jssPName = JSVAL_TO_STRING(v);
		wchar_t* pjcPName = wr_JS_GetStringChars(jssPName);
		if(JS_GetStringLength(jssPName)==2){ 
			// возможно это SAVE_FROM_GC-свойство
			jsval v;
			wr_JS_GetUCProperty(mcx, mjso, pjcPName, -1, &v);
			if(JSVAL_IS_OBJECT(v)){
				JSObject* jsoT=JSVAL_TO_OBJECT(v);
				JSClass* pClass=ML_JS_GETCLASS(mcx, jsoT);
				if(pClass->flags==1) continue; // script
			}else if(JSVAL_IS_STRING(v)){
				JSString* jssT=JSVAL_TO_STRING(v);
				jschar* pjc=JS_GetStringChars(jssT);
				continue;
			}
//			mlPropertyValues::mlPropertyValue* pPropVal = avPropVals.AddNew();
//			pPropVal->copy = false;
//			pPropVal->name = "??";
//			pPropVal->val = v;
//			continue;
		}
		char* pcPName = WC2MBq(pjcPName);
		if(avPropVals.Find(pcPName) != NULL){
      	    MP_DELETE_ARR( pcPName);
			continue;
		}
		mlPropertyValues::mlPropertyValue* pPropVal = avPropVals.AddNew();
		pPropVal->copy = true;
		size_t len = strlen(pcPName);
		pPropVal->name = MP_NEW_ARR( char, len + 1);
		//strcpy((char*)(pPropVal->name), pcPName);
		rmmlsafe_strcpy((char*)(pPropVal->name), len + 1, 0, pcPName);
		JS_GetProperty(mcx,mjso,pcPName,&(pPropVal->val));
		MP_DELETE_ARR( pcPName);
	}
	JS_DestroyIdArray(mcx, idArr);
}

bool mlRMMLElement::Get3DPropertiesInfo(mlSynchData &aData){
	if(PMO_IS_NULL(mpMO)) return false;
	moI3DObject* pmoI3D=mpMO->GetI3DObject();
	if(pmoI3D==NULL) return false;
	return pmoI3D->Get3DPropertiesInfo(aData);
}

string mlRMMLElement::Get3DProperty(char* apszName,char* apszSubName){
	if(PMO_IS_NULL(mpMO)) return false;
	moI3DObject* pmoI3D=mpMO->GetI3DObject();
	if(pmoI3D==NULL) return false;
	mlOutString sVal;
	if(pmoI3D->Get3DProperty(apszName,apszSubName,sVal)){
		return (const char*)cLPCSTR(sVal.mstr.c_str());
	}
	return "";
}

mlresult mlRMMLElement::Set3DProperty(char* apszName,char* apszSubName,char* apszVal){
	if(PMO_IS_NULL(mpMO)) return false;
	moI3DObject* pmoI3D=mpMO->GetI3DObject();
	if(pmoI3D==NULL) return false;
	if(!pmoI3D->Set3DProperty(apszName,apszSubName,apszVal))
		return ML_ERROR_FAILURE;
	return ML_OK;
}

mlString mlRMMLElement::GetSrcFileRef(){
	mlString sSrc;
	if(miSrcFilesIdx >= 0){
		sSrc=L"(";
		sSrc+=GPSM(mcx)->mSrcFileColl[miSrcFilesIdx];
		if(miSrcLine >= 0){
			sSrc+=L":";
			mlString sStrN;
			int iStrN=miSrcLine+1;
			if(iStrN==0){
				sStrN=L"0";
			}
			else
			{
				sStrN = Int2MlString(iStrN);
			}
			sSrc+=sStrN;
		}
		sSrc+=L")";
	}
	return sSrc;
}

// реализация mlMedia
	// произвольный доступ к свойствам 
int mlRMMLElement::GetPropType(const char* apszName){
	if(!IsJSO()) return MLPT_UNKNOWN;
	GUARD_JS_CALLS(mcx);
	jsval v;
	JS_GetProperty(mcx,mjso,apszName,&v);
	if(JSVAL_IS_INT(v)) return MLPT_INT;
	if(JSVAL_IS_DOUBLE(v)) return MLPT_DOUBLE;
	if(JSVAL_IS_STRING(v)) return MLPT_STRING;
	if(JSVAL_IS_BOOLEAN(v)) return MLPT_BOOLEAN;
	if(JSVAL_IS_REAL_OBJECT(v)){
		JSObject* jso=JSVAL_TO_OBJECT(v);
		if(JSO_IS_MLEL(mcx, jso))
			return MLPT_ELEMREF;
		return MLPT_JSOREF;
	}
	return MLPT_UNKNOWN;
}

int mlRMMLElement::GetIntProp(const char* apszName){
	if(!IsJSO()) return 0;
	GUARD_JS_CALLS(mcx);
	jsval v;
	JS_GetProperty(mcx,mjso,apszName,&v);
	if(JSVAL_IS_INT(v)) return JSVAL_TO_INT(v);
	return 0;
}

bool mlRMMLElement::GetBooleanProp(const char* apszName){
	if(!IsJSO()) return 0;
	jsval v;
	GUARD_JS_CALLS(mcx);
	JS_GetProperty(mcx,mjso,apszName,&v);
	if(JSVAL_IS_BOOLEAN(v)) return JSVAL_TO_BOOLEAN(v);
	return 0;
}

double mlRMMLElement::GetDoubleProp(const char* apszName){
	if(!IsJSO()) return 0.0;
	GUARD_JS_CALLS(mcx);
	jsval v;
	JS_GetProperty(mcx,mjso,apszName,&v);
	if(JSVAL_IS_DOUBLE(v)) return *JSVAL_TO_DOUBLE(v);
	if(JSVAL_IS_INT(v)) return JSVAL_TO_INT(v);
	return 0.0;
}

wchar_t* mlRMMLElement::GetStringProp(const char* apszName){
	if(!IsJSO()) return NULL;
	GUARD_JS_CALLS(mcx);
	jsval v;
	JS_GetProperty(mcx,mjso,apszName,&v);
	if(JSVAL_IS_STRING(v)) return wr_JS_GetStringChars(JSVAL_TO_STRING(v));
	return NULL;
}

mlMedia* mlRMMLElement::GetRefProp(const char* apszName){
	if(!IsJSO()) return NULL;
	GUARD_JS_CALLS(mcx);
	jsval v;
	JS_GetProperty(mcx,mjso,apszName,&v);
	if(v!=JSVAL_NULL && v!=JSVAL_VOID && JSVAL_IS_OBJECT(v)){
		JSObject* jso=JSVAL_TO_OBJECT(v);
		if(JSO_IS_MLEL(mcx, jso))
			return (mlMedia*)(mlRMMLElement*)JS_GetPrivate(mcx,jso);
		return NULL;
	}
	return NULL;
}

void mlRMMLElement::SetProp(const char* apszName, int aiVal){
	if(!IsJSO()) return;
	GUARD_JS_CALLS(mcx);
	jsval v=INT_TO_JSVAL(aiVal);
	JS_SetProperty(mcx,mjso,apszName,&v);
}

void mlRMMLElement::SetProp(const char* apszName, double adVal){
	if(!IsJSO()) return;
	GUARD_JS_CALLS(mcx);
	jsdouble* jsdbl=JS_NewDouble(mcx, adVal); 
	jsval v=DOUBLE_TO_JSVAL(jsdbl);
	JS_SetProperty(mcx,mjso,apszName,&v);
}

void mlRMMLElement::SetProp(const char* apszName, const char* apszVal){
	if(!IsJSO()) return;
	GUARD_JS_CALLS(mcx);
	JSString* jssVal=JS_NewStringCopyZ(mcx,apszVal);
	jsval v=STRING_TO_JSVAL(jssVal);
	JS_SetProperty(mcx,mjso,apszName,&v);
}

void mlRMMLElement::SetProp(const char* apszName, const wchar_t* apwcVal){
	if(!IsJSO()) return;
	GUARD_JS_CALLS(mcx);
	JSString* jssVal=wr_JS_NewUCStringCopyZ(mcx,apwcVal);
	jsval v=STRING_TO_JSVAL(jssVal);
	JS_SetProperty(mcx,mjso,apszName,&v);
}

int mlRMMLElement::EnumPlugins(std::vector<mlMedia*>& avPlugRefs){
	if(mpChildren==NULL) return 0;
	int iCnt=0;
	v_elems::iterator vi;
	for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
		mlRMMLElement* pMLEl=*vi;
		if(pMLEl->mRMMLType==MLMT_PLUGIN){
			avPlugRefs.push_back((mlMedia*)pMLEl);
			iCnt++;
		}
	}
	return iCnt;
}

unsigned int mlRMMLElement::GetSynchID(unsigned int &auiBornReality, bool abGetIfUndefined){
	auiBornReality = GetBornRealityID();
	return GetID(abGetIfUndefined);
}

bool mlRMMLElement::SetEventData(char aidEvent, mlSynchData &Data){
	switch(aidEvent&~((1<<4)-1)){
	case (0<<4):
	case (1<<4):
		return false;
	case TIDB_mlRMMLElement:
		GetLoadable()->Lod_SetEventData(aidEvent, Data);
		// ??
		break;
	case TIDB_mlRMMLVisible:
//		GetVisible()->Vis_SetEventData(aidEvent, Data);
		break;
	case TIDB_mlRMMLContinuous:
		// ??
		break;
	case TIDB_mlRMMLPButton:
//		GetButton()->But_SetEventData(aidEvent, Data);
		break;
	case TIDB_mlRMML3DObject:
		Get3DObject()->O3d_SetEventData(aidEvent, Data);
		break;
	}
	return true;
}

JSBool mlRMMLElement::JSFUNC_duplicate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if (argc > 1) return JS_FALSE;
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	if(argc==0 || (argc==1 && (argv[0]==JSVAL_VOID || argv[0]==JSVAL_NULL))){
		// сделать безимянный дубликат и выдать ссылку на него 
		// (_parent тот же)
		mlRMMLElement* pNewEl=pMLEl->Duplicate(NULL);
		jsval v=JSVAL_NULL;
		JS_SetProperty(pMLEl->mcx, pNewEl->mjso, MLELPN_NAME, &v);
		pNewEl->SetParent(pMLEl->mpParent);
		pNewEl->DuplicateMedia(pMLEl);
		*rval=OBJECT_TO_JSVAL(pNewEl->mjso);
	}else{
		if(JSVAL_IS_STRING(argv[0])){
			// сделать дубликат с новым именем и выдать ссылку на него
			// (_parent тот же)
			JSString* jssNewName=JSVAL_TO_STRING(argv[0]);
			mlRMMLElement* pNewEl=pMLEl->Duplicate(NULL);
			jsval v=STRING_TO_JSVAL(jssNewName);
			JS_SetProperty(pMLEl->mcx, pNewEl->mjso, MLELPN_NAME, &v);
			pNewEl->SetParent(pMLEl->mpParent);
			pNewEl->DuplicateMedia(pMLEl);
			*rval=OBJECT_TO_JSVAL(pNewEl->mjso);
		}else if(JSVAL_IS_INT(argv[0])){
			int iCount=JSVAL_TO_INT(argv[0]);
			if(iCount<=0) return JS_FALSE;
			// сделать iCount дубликатов без имени и вернуть массив ссылок
			// ??
		}
	}
	return JS_TRUE;
}

bool mlRMMLElement::PropValueIsSynchronizable(jsval vProp){
	if((!JSVAL_IS_NULL(vProp)) && (!JSVAL_IS_VOID(vProp)) && JSVAL_IS_OBJECT(vProp)){
		// если свойство - функция, то не будем его синхронизировать
		if(JS_TypeOfValue(mcx,vProp) == JSTYPE_FUNCTION)
			return false;
		// если свойство - дочерний объект, то не синхронизируем
		JSObject* jso = JSVAL_TO_OBJECT(vProp);
		if(JSO_IS_MLEL(mcx, jso)){
			mlRMMLElement* pMLEl = GET_RMMLEL(mcx,jso);
			if(pMLEl->mpParent == this)
				return false;
		}
	}
	return true;
}

void mlRMMLElement::PutForSynch(mlSynchData &aData, bool abProps, bool abMedia){
	aData << SNCOT_ELEM;
	MLWORD wID = GetID();
	aData << wID;
	moMedia* pMO = GetMO();
	if(!PMO_IS_NULL(pMO) && abMedia){
		aData << (bool)true;
		pMO->GetSynchData(aData);
	}else{
		aData << (bool)false;
	}
	// сохраняем свойства
	JSIdArray* pGPropIDArr = JS_Enumerate(mcx, mjso);
	int iNPropsPos = aData.getPos();
	unsigned short iNProps=0;
	aData << iNProps;
	if(!abProps) return;
	int iLen = pGPropIDArr->length;
	for(int ikl = 0; ikl < iLen; ikl++){
		jsid id = pGPropIDArr->vector[ikl];
		jsval v;
		if(JS_IdToValue(mcx,id,&v)){
			if(JSVAL_IS_STRING(v)){
				wchar_t* jscProp = wr_JS_GetStringChars(JSVAL_TO_STRING(v));
				if(isEqual(jscProp, L"_class") || isEqual(jscProp, L"_parent") || isEqual(jscProp, L"_name"))
					continue;
				jsval vProp;
				wr_JS_GetUCProperty(mcx, mjso, jscProp, -1, &vProp);
				if(!PropValueIsSynchronizable(vProp)) continue;
				aData << jscProp;
				PutJSValForSynch(mcx, vProp, aData);
				iNProps++;
			}
		}
	}
	JS_DestroyIdArray(mcx, pGPropIDArr);
	aData.setPos(iNPropsPos); 
	aData << iNProps;
	aData.setPos(); 
}

void mlRMMLElement::GetAndSynch(mlSynchData &aData){
	bool bMedia;
	aData >> bMedia;
	if(bMedia){
		moMedia* pMO = GetMO();
		ML_ASSERTION(mcx, !PMO_IS_NULL(pMO),"mlRMMLElement::GetAndSynch");
		pMO->SetSynchData(aData);
	}
	// читаем и устанавливаем свойства
	unsigned short iNProps=0;
	aData >> iNProps;
	for(int ikl=0; ikl<iNProps; ikl++){
		wchar_t* jscPropName;
		aData >> jscPropName;
		mlString wsPropName=jscPropName;
		jsval vProp=GetJSValForSynch(mcx, aData);
		if(isEqual(wsPropName.c_str(),L"src")){
			JSString* jssNewSrc=JSVAL_TO_STRING(vProp);
			jsval vSrcProp;
			wr_JS_GetUCProperty(mcx,mjso,wsPropName.c_str(),-1,&vSrcProp);
			JSString* jssSrc=JSVAL_TO_STRING(vSrcProp);
//			const wchar_t* pwcSrc=GetLoadable()->GetSrc();
			if(isEqual(jssNewSrc, jssSrc))
				continue;
		}
		wr_JS_SetUCProperty(mcx,mjso,wsPropName.c_str(),-1,&vProp);
	}
}

#define MLJSVT_NULL		0
#define MLJSVT_BOOL		1
#define MLJSVT_INT		2
#define MLJSVT_STRING	3
#define MLJSVT_DOUBLE	4
#define MLJSVT_OBJECT	5
#define MLJSVT_VOID		6
#define MLJSVT_STRUCT	0x20
#define MLJSVT_POS3D	(1 | MLJSVT_STRUCT)
#define MLJSVT_SCL3D	(2 | MLJSVT_STRUCT)
#define MLJSVT_ROT3D	(3 | MLJSVT_STRUCT)

void mlRMMLElement::PutJSValForSynch(JSContext *cx, jsval vProp, mlSynchData &oData){
	if(JSVAL_IS_NULL(vProp)){
		oData << (char)MLJSVT_NULL;
	}else if(JSVAL_IS_BOOLEAN(vProp)){
		oData << (char)MLJSVT_BOOL;
		bool bArg=JSVAL_TO_BOOLEAN(vProp);
		oData << bArg;
	}else if(JSVAL_IS_INT(vProp)){
		oData << (char)MLJSVT_INT;
		int iArg=JSVAL_TO_INT(vProp);
		oData << iArg;
	}else if(JSVAL_IS_STRING(vProp)){
		oData << (char)MLJSVT_STRING;
		JSString* jssArg=JSVAL_TO_STRING(vProp);
		jschar* jscArg=JS_GetStringChars(jssArg);
		oData << jscArg;
	}else if(JSVAL_IS_DOUBLE(vProp)){
		oData << (char)MLJSVT_DOUBLE;
		jsdouble* pjsdbl=JSVAL_TO_DOUBLE(vProp);
		oData << (*pjsdbl);
	}else if(JSVAL_IS_OBJECT(vProp)){
		JSObject* jsoArg=JSVAL_TO_OBJECT(vProp);
		if(mlPosition3D::IsInstance(cx, jsoArg)){
			oData << (char)MLJSVT_POS3D;
			mlPosition3D* pMLElArg=(mlPosition3D*)JS_GetPrivate(cx,jsoArg);
			ml3DPosition pos=pMLElArg->GetPos();
			oData.put(&pos,sizeof(ml3DPosition));
		}else if(mlScale3D::IsInstance(cx, jsoArg)){
			oData << (char)MLJSVT_SCL3D;
			mlScale3D* pMLElArg=(mlScale3D*)JS_GetPrivate(cx,jsoArg);
			ml3DScale scl=pMLElArg->GetScl();
			oData.put(&scl,sizeof(ml3DScale));
		}else if(mlRotation3D::IsInstance(cx, jsoArg)){
			oData << (char)MLJSVT_ROT3D;
			mlRotation3D* pMLElArg=(mlRotation3D*)JS_GetPrivate(cx,jsoArg);
			ml3DRotation rot=pMLElArg->GetRot();
			oData.put(&rot,sizeof(ml3DRotation));
		}else{
			oData << (char)MLJSVT_OBJECT;
			if(JSO_IS_MLEL(cx, jsoArg)){
				mlRMMLElement* pMLElArg=(mlRMMLElement*)JS_GetPrivate(cx,jsoArg);
				if(pMLElArg==NULL){
					JS_ReportWarning(cx, "PutJSValForSynch (unsupported JS-object argument type)");
					return;
				}
				MLWORD wID=pMLElArg->GetID();
				oData << wID;
			}else{
				// ??
				MLWORD wID=0xFFFF;
				oData << wID;
			}
		}
	}else if(JSVAL_IS_VOID(vProp)){
		oData << (char)MLJSVT_VOID;
	}else{
		JS_ReportWarning(cx, "PutJSValForSynch (unsupported argument type)");
		return;
	}
}

jsval mlRMMLElement::GetJSValForSynch(JSContext *cx, mlSynchData &oData){
	jsval vProp=JSVAL_NULL;
	char chArgType;
	oData >> chArgType;
	switch(chArgType){
	case MLJSVT_NULL: // NULL
		break;
	case MLJSVT_BOOL:{ // bool
		bool bArg;
		oData >> bArg;
		vProp=BOOLEAN_TO_JSVAL(bArg);
		}break;
	case MLJSVT_INT:{ // int
		int iArg;
		oData >> iArg;
		vProp=INT_TO_JSVAL(iArg);
		}break;
	case MLJSVT_STRING:{ // string
		wchar_t* jscArg=NULL;
		oData >> jscArg;
		JSString* jssArg=wr_JS_NewUCStringCopyZ(cx,jscArg);
		vProp=STRING_TO_JSVAL(jssArg);
		}break;
	case MLJSVT_DOUBLE:{ // double
		jsdouble jsdArg=0.0;
		oData >> jsdArg;
		mlJS_NewDoubleValue(cx,jsdArg,&vProp);
		}break;
	case MLJSVT_OBJECT:{ // object
		MLWORD wID=0;
		oData >> wID;
		}break;
	case MLJSVT_VOID:
		vProp=JSVAL_VOID;
		break;
	case MLJSVT_POS3D:{
		JSObject* jso=mlPosition3D::newJSObject(cx);
		mlPosition3D* pMLEl=(mlPosition3D*)JS_GetPrivate(cx,jso);
		ml3DPosition pos;
		oData.get(&pos,sizeof(ml3DPosition));
		pMLEl->SetPos(pos);
		vProp = OBJECT_TO_JSVAL(jso);
		}break;
	case MLJSVT_SCL3D:{
		JSObject* jso=mlScale3D::newJSObject(cx);
		mlScale3D* pMLEl=(mlScale3D*)JS_GetPrivate(cx,jso);
		ml3DScale scl;
		oData.get(&scl,sizeof(ml3DScale));
		pMLEl->SetScl(scl);
		vProp = OBJECT_TO_JSVAL(jso);
		}break;
	case MLJSVT_ROT3D:{
		JSObject* jso=mlRotation3D::newJSObject(cx);
		mlRotation3D* pMLEl=(mlRotation3D*)JS_GetPrivate(cx,jso);
		ml3DRotation rot;
		oData.get(&rot,sizeof(ml3DRotation));
		pMLEl->SetRot(rot);
		vProp = OBJECT_TO_JSVAL(jso);
		}break;
	}
	return vProp;
}
/*
mlString mlRMMLElement::GetSJSValForSynch(JSContext *cx, mlSynchData &oData){
	char chArgType;
	oData >> chArgType;
	switch(chArgType){
	case MLJSVT_NULL: // NULL
		return L"null";
	case MLJSVT_BOOL:{ // boolean
		bool bArg;
		oData >> bArg;
		if(bArg) return L"true";
		return L"false";
		}break;
	case MLJSVT_INT:{ // int
		int iArg;
		oData >> iArg;
		wchar_t wsIVal[100];
		swprintf(wsIVal,L"%d",iArg);
		return wsIVal;
//		vProp=INT_TO_JSVAL(iArg);
		}break;
	case MLJSVT_STRING:{ // string
		jschar* jscArg=NULL;
		oData >> jscArg;
		return jscArg;
		}break;
	case MLJSVT_DOUBLE:{ // double
		jsdouble jsdArg=0.0;
		oData >> jsdArg;
		wchar_t wsDVal[100];
		swprintf(wsDVal,L"%f",jsdArg);
		return wsDVal;
		}break;
	case MLJSVT_OBJECT:{ // object
		MLWORD wID=0;
		oData >> wID;
		mlRMMLElement* pMLElArg=GPSM(mcx)->GetScriptedObjectByID(wID);
		if(pMLElArg!=NULL)
			return pMLElArg->GetStringID();
		return L"null";
		}break;
	case MLJSVT_VOID:
		return L"undefined";
	case MLJSVT_POS3D:{
		ml3DPosition pos;
		oData.get(&pos,sizeof(ml3DPosition));
		wchar_t wsVal[400];
		swprintf(wsVal,L"\"%lf; %lf; %lf\"",pos.x, pos.y, pos.z);
		return wsVal;
		}break;
	case MLJSVT_SCL3D:{
		ml3DScale scl;
		oData.get(&scl,sizeof(ml3DScale));
		wchar_t wsVal[400];
		swprintf(wsVal,L"\"%lf; %lf; %lf\"",scl.x, scl.y, scl.z);
		return wsVal;
		}break;
	case MLJSVT_ROT3D:{
		ml3DRotation rot;
		oData.get(&rot,sizeof(ml3DRotation));
		wchar_t wsVal[400];
		swprintf(wsVal,L"\"%lf; %lf; %lf; %lf\"",rot.x, rot.y, rot.z, rot.a);
		return wsVal;
		}break;
	}
	return L"null";
}
*/

void mlRMMLElement::PutPropsForSynch(mlSynchData &aData, const wchar_t* apwcPropList, unsigned char &aiNObjs){
	const wchar_t* pwcPropName = SkipSpaces(apwcPropList);
	while(*pwcPropName != L'\0'){
		const wchar_t* pwcDelim = GetNextDelimPos(pwcPropName);
		if(isEqual(pwcPropName, L"this", (pwcDelim - pwcPropName))){
			// this - синхронизировать медиа-объект и все свойства объекта
			PutForSynch(aData);
			aiNObjs++;
		}else if(isEqual(pwcPropName, L"all", (pwcDelim - pwcPropName))){
			// all - синхронизировать все свойства объекта
			PutForSynch(aData, true, false);
			aiNObjs++;
		}else if(isEqual(pwcPropName, L"media", (pwcDelim - pwcPropName))){
			// media - синхронизировать медиа-объект 
			PutForSynch(aData, false, true);
			aiNObjs++;
		}else{
			// свойство объекта
			jsval vProp = JSVAL_NULL;
			if((*pwcPropName != L'_' || pwcPropName[1] != L'\0') && 
				wr_JS_GetUCProperty(mcx, mjso, pwcPropName, (pwcDelim - pwcPropName), &vProp))
			{
				if(PropValueIsSynchronizable(vProp)){
					aData << SNCOT_PROP;
					wchar_t wc = *pwcDelim;
					*(wchar_t*)pwcDelim = L'\0';
					aData << pwcPropName;
					*(wchar_t*)pwcDelim = wc;
					PutJSValForSynch(mcx, vProp, aData);
					aiNObjs++;
				}
			}
		}
		if(*pwcDelim == L'\0')
			break;
		pwcPropName = SkipDelims(pwcDelim);
	}
}

JSBool mlRMMLElement::JSFUNC_synchronize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	// сохраняем данные для синхронизации
	mlSynchData oData;
	oData << SNCMAG_SERIES; // 0x7E
	int iNObjsPos=oData.getPos();
	unsigned char iNObjs=0;
	oData << iNObjs;
	if(argc==0){
		// синхронизировать все свои свойства и moMedia
		pMLEl->PutForSynch(oData);
		iNObjs++;
	}else{
		// перебираем все аргументы
		bool bFunc=false;
		for(int iArg = 0; iArg < (int)argc; iArg++){
			if(bFunc){ 
				// значит параметр функции
				PutJSValForSynch(cx, argv[iArg], oData);
				iNObjs++;
				continue;
			}
			// если аргумент - строка, то ищем его и определяем его тип
			if(JSVAL_IS_STRING(argv[iArg])){
				JSString* jssArg=JSVAL_TO_STRING(argv[iArg]);
				wchar_t* pjsc=wr_JS_GetStringChars(jssArg);
				// если это 'this', 
				if(isEqual(pjsc,L"this")){
					// то синхронизируем все свои свойства и moMedia
					pMLEl->PutForSynch(oData);
					iNObjs++;
				}else{
					jsval v=JSVAL_NULL;
					wr_JS_EvaluateUCScript(cx,obj,pjsc,JS_GetStringLength(jssArg),
						cLPCSTRq(GPSM(cx)->mSrcFileColl[pMLEl->miSrcFilesIdx]),pMLEl->miSrcLine,&v);
					if(v!=JSVAL_NULL && v!=JSVAL_VOID){
						if(JSVAL_IS_OBJECT(v)){
							// если это метод, то оставшиеся аргументы - это его параметры
							if(JS_TypeOfValue(cx,v)==JSTYPE_FUNCTION){
								oData << SNCOT_FUN;
								oData << pjsc;
								bFunc=true;
								int uArgc = -1; // кол-во аргументов будет определяться по кол-ву оставшихся объектов
								oData << uArgc;
								iNObjs++;
								continue;
							}else{
								JSObject* jso=JSVAL_TO_OBJECT(v);
								if(JSO_IS_MLEL(cx, jso)){
									mlRMMLElement* pElem=GET_RMMLEL(cx,jso);
									if(pElem!=NULL && pElem->_name!=NULL){
										// если имя элемента совпадает с именем свойства, 
										wchar_t* pwcDot=(wchar_t*)findLastDot(pjsc);
										if(pwcDot==NULL) pwcDot=pjsc; else pwcDot++;
										if(isEqual(pElem->_name,pwcDot)){
											// то синхронизируем все его свойства и moMedia
											pElem->PutForSynch(oData);
											iNObjs++;
											continue;
										}
									}
								}
							}
						}
					}
					// значит может быть это свойство
//					mlRMMLElement* pElem=pMLEl;
//					wchar_t* pwcDot=(wchar_t*)findLastDot(pjsc);
//					if(pwcDot!=NULL){
//					}
					// если это свойство какого-нибудь объекта, то синхронизируем его
					// ??
					oData << SNCOT_PROP;
					oData << pjsc;
					PutJSValForSynch(cx, v, oData);
				}
			}else{
				JS_ReportWarning(cx,"Invalid parameter type ('%s')",cLPCSTR(JS_ValueToString(cx,argv[iArg])));
			}
		}
	}
	oData.setPos(iNObjsPos); 
	oData << iNObjs;
	oData.setPos(); 
	//
	if(GPSM(cx)->mbDontSendSynchEvents || GPSM(cx)->GetMode() != smmNormal){
		char cSNCMAG_SERIES=0;
		oData >> cSNCMAG_SERIES;
		pMLEl->synchronize(oData);
	}else{
		MLWORD wID=pMLEl->GetID();
		if(wID!=0xFFFF){
			mlISinchronizer* pSynch=GPSM(cx)->GetContext()->mpSynch;
			omsresult res=OMS_OK;
			res=pSynch->CallEvent(wID, (unsigned char*)oData.data(), oData.size());
		}
	}
	return JS_TRUE;
}

mlString wsTrace;

void mlRMMLElement::SynchronizeProperty(mlSynchData &aData){
	wchar_t* pjsc=NULL;
	mlString sPropName;
	aData >> pjsc;
	sPropName = pjsc;
	mlString sPropVal; // = GetSJSValForSynch(mcx,aData);
	char chArgType;
	aData >> chArgType;
	switch(chArgType){
	case MLJSVT_NULL: // NULL
		sPropVal = L"null";
	case MLJSVT_BOOL:{ // boolean
		bool bArg;
		aData >> bArg;
		sPropVal = bArg?L"true":L"false";
		}break;
	case MLJSVT_INT:{ // int
		int iArg;
		aData >> iArg;
		wchar_t wsIVal[100];
		swprintf_s(wsIVal,100,L"%d",iArg);
		sPropVal = wsIVal;
		}break;
	case MLJSVT_STRING:{ // string
		wchar_t* jscArg=NULL;
		aData >> jscArg;
		sPropVal = jscArg;
		}break;
	case MLJSVT_DOUBLE:{ // double
		jsdouble jsdArg=0.0;
		aData >> jsdArg;
		wchar_t wsDVal[100];
		swprintf(wsDVal,100,L"%f",jsdArg);
		sPropVal = wsDVal;
		}break;
	case MLJSVT_OBJECT:{ // object
		MLWORD wID=0;
		aData >> wID;
		}break;
	case MLJSVT_VOID:{
		jsval v;
		wr_JS_DeleteUCProperty2(mcx, mjso, sPropName.c_str(), sPropName.length(), &v);
wsTrace += L" DELETE ";
wsTrace += sPropName;
wsTrace += L"; ";
		}break;
	case MLJSVT_POS3D:{
		ml3DPosition pos;
		aData.get(&pos,sizeof(ml3DPosition));
		// взять тип свойства
		jsval vProp;
		wr_JS_GetUCProperty(mcx, mjso, sPropName.c_str(), -1, &vProp);
		JSObject* jsoStructProp = NULL;
		if(JSVAL_IS_REAL_OBJECT(vProp))
			jsoStructProp = JSVAL_TO_OBJECT(vProp);
		// если он не совпадает с этим типом или отсутствует вообще,
		if(jsoStructProp == NULL || !mlPosition3D::IsInstance(mcx, jsoStructProp)){
JSClass* cls = ML_JS_GETCLASS(mcx, jsoStructProp);
			//   то пересоздать его
			jsoStructProp = mlPosition3D::newJSObject(mcx);
			// присвоить значения свойствам сруктуры
			mlPosition3D* pPos3D = (mlPosition3D*)JS_GetPrivate(mcx, jsoStructProp);
			pPos3D->SetPos(pos);
			// установить в новое значение
			vProp = OBJECT_TO_JSVAL(jsoStructProp);
			wr_JS_SetUCProperty(mcx, mjso, sPropName.c_str(), -1, &vProp);
		}else{
			// присвоить значения свойствам сруктуры
			mlPosition3D* pPos3D = (mlPosition3D*)JS_GetPrivate(mcx, jsoStructProp);
			pPos3D->SetPos(pos);
		}
		}break;
	case MLJSVT_SCL3D:{
		ml3DScale scl;
		aData.get(&scl,sizeof(ml3DScale));
		// взять тип свойства
		jsval vProp;
		wr_JS_GetUCProperty(mcx, mjso, sPropName.c_str(), -1, &vProp);
		JSObject* jsoStructProp = NULL;
		if(JSVAL_IS_REAL_OBJECT(vProp))
			jsoStructProp = JSVAL_TO_OBJECT(vProp);
		// если он не совпадает с этим типом или отсутствует вообще,
		if(jsoStructProp == NULL || !mlScale3D::IsInstance(mcx, jsoStructProp)){
JSClass* cls = ML_JS_GETCLASS(mcx, jsoStructProp);
			//   то пересоздать его
			jsoStructProp = mlScale3D::newJSObject(mcx);
			// присвоить значения свойствам сруктуры
			mlScale3D* pScl3D = (mlScale3D*)JS_GetPrivate(mcx, jsoStructProp);
			pScl3D->SetScl(scl);
			// установить в новое значение
			vProp = OBJECT_TO_JSVAL(jsoStructProp);
			wr_JS_SetUCProperty(mcx, mjso, sPropName.c_str(), -1, &vProp);
		}else{
			// присвоить значения свойствам сруктуры
			mlScale3D* pScl3D = (mlScale3D*)JS_GetPrivate(mcx, jsoStructProp);
			pScl3D->SetScl(scl);
		}
		}break;
	case MLJSVT_ROT3D:{
		ml3DRotation rot;
		aData.get(&rot,sizeof(ml3DRotation));
		// взять тип свойства
		jsval vProp;
		wr_JS_GetUCProperty(mcx, mjso, sPropName.c_str(), -1, &vProp);
		JSObject* jsoStructProp = NULL;
		if(JSVAL_IS_REAL_OBJECT(vProp))
			jsoStructProp = JSVAL_TO_OBJECT(vProp);
		// если он не совпадает с этим типом или отсутствует вообще,
		if(jsoStructProp == NULL || !mlRotation3D::IsInstance(mcx, jsoStructProp)){
JSClass* cls = ML_JS_GETCLASS(mcx, jsoStructProp);
			//   то пересоздать его
			jsoStructProp = mlRotation3D::newJSObject(mcx);
			// присвоить значения свойствам сруктуры
			mlRotation3D* pRot3D = (mlRotation3D*)JS_GetPrivate(mcx, jsoStructProp);
			pRot3D->SetRot(rot);
			// установить в новое значение
			vProp = OBJECT_TO_JSVAL(jsoStructProp);
			wr_JS_SetUCProperty(mcx, mjso, sPropName.c_str(), -1, &vProp);
		}else{
			// присвоить значения свойствам сруктуры
			mlRotation3D* pRot3D = (mlRotation3D*)JS_GetPrivate(mcx, jsoStructProp);
			pRot3D->SetRot(rot);
		}
		}break;
	}
	// если это структура, то ее уже заполнили
	if(chArgType & MLJSVT_STRUCT)
		return;
	if(!sPropVal.empty()){
		sPropName += L'=';
		sPropName += sPropVal;
		sPropName += L';';
		jsval v;
		wr_JS_EvaluateUCScript(mcx,mjso,sPropName.c_str(),sPropName.length(),cLPCSTRq(GPSM(mcx)->mSrcFileColl[miSrcFilesIdx]),miSrcLine,&v); 
wsTrace += sPropName;
wsTrace += L' ';
	}
}

void mlRMMLElement::synchronize(mlSynchData &aData){
wsTrace = L"Synch-ed: ";
	unsigned char iNObjs=0;
	aData >> iNObjs;
	bool bFunc=false;
	for(int ii=0; ii<iNObjs; ii++){
		char chType;
		aData >> chType;
		if(chType==SNCOT_ELEM){
			MLWORD wID;
			aData >> wID;
		}else if(chType==SNCOT_FUN){
			wchar_t* pjsc=NULL;
			mlString sFunName;
			aData >> pjsc;
			sFunName=pjsc;
			ii++;
			jsval *aArgv=NULL;
			int uArgc;
			aData >> uArgc;
			if(uArgc < 0)
				uArgc = iNObjs-ii;
			if(uArgc > 0){
				aArgv = MP_NEW_ARR( jsval, uArgc);
			}
wsTrace += L"\n     ";
wsTrace += GetStringID();
wsTrace += L'.';
wsTrace += sFunName;
wsTrace += L'(';
			for(int ij=0; ij<uArgc; ij++){
				aArgv[ij]=GetJSValForSynch(mcx,aData);
if(JSVAL_IS_STRING(aArgv[ij])) wsTrace += L'\'';
wsTrace += wr_JS_GetStringChars(JS_ValueToString(mcx,aArgv[ij]));
if(JSVAL_IS_STRING(aArgv[ij])) wsTrace += L'\'';
if(ij+1<uArgc) wsTrace += L", ";
			}
wsTrace += L')';
mlTrace(mcx, "%S\n",wsTrace.c_str());
wsTrace = L"";
bool bDSSE = GPSM(mcx)->mbDontSendSynchEvents;
			jsval v;
			JSBool retcode=JS_CallFunctionName(mcx,mjso,cLPCSTRq(sFunName.c_str()),uArgc,aArgv,&v);
			if(aArgv!=NULL)
				MP_DELETE_ARR( aArgv);
			break;
		}else if(chType == SNCOT_PROP){
			SynchronizeProperty(aData);
		}else if(chType == SNCOT_MOS){
			unsigned int uiSize = 0;
			aData >> uiSize;
			mlSynchData oData;
			oData.put(aData.getDataInPos(aData.getPos()), uiSize);
			aData.skip(uiSize);
			if(!PMO_IS_NULL(mpMO)){
				mpMO->SetSynchData(oData);
			}
		}
	}
if(wsTrace.length() > 0)
mlTrace(mcx, "%S\n",wsTrace.c_str());
}

bool mlRMMLElement::SynchronizeProp(jsval id, jsval *vp){
	if(mchSynchronized == 2){
		if(!JSVAL_IS_STRING(id)) return false;
		JSString* jssID = JSVAL_TO_STRING(id);
		wchar_t* pjscID = wr_JS_GetStringChars(jssID);
		// если свойства нет в списке synchProperties
		jsval vSP = JSVAL_NULL;
		wr_JS_GetUCProperty(mcx, mjso, SYNCHPROPERTIESN, -1, &vSP);
		if(!JSVAL_IS_STRING(vSP))
			return false;
		JSString* jssSP = JSVAL_TO_STRING(vSP);
		wchar_t* jscSP = wr_JS_GetStringChars(jssSP);
		const wchar_t* pwcPropName = SkipSpaces(jscSP);
		while(*pwcPropName != L'\0'){
			const wchar_t* pwcDelim = GetNextDelimPos(pwcPropName);
			if(isEqual(pwcPropName, pjscID, (pwcDelim - pwcPropName))){
				if(pjscID[pwcDelim - pwcPropName] == L'\0')
					break;
			}
			if(*pwcDelim == L'\0'){
				pwcPropName = pwcDelim;
				break;
			}
			pwcPropName = SkipDelims(pwcDelim);
		}
		if(*pwcPropName == L'\0'){
			// то его не надо синхронизировать
			return false;
		}
	}
	mlSynchData oData;
	oData << SNCMAG_SERIES; // 0x7E
	oData << (unsigned char)1;
	oData << SNCOT_PROP;
	if(JSVAL_IS_STRING(id)){
		JSString* jssID = JSVAL_TO_STRING(id);
		jschar* pjsc = JS_GetStringChars(jssID);
		oData << pjsc;
	}else if(JSVAL_IS_INT(id)){
		int iID = JSVAL_TO_INT(id);
		mlString sName = L"??";
		if(iID < TIDB_mlRMMLElement){
			// specific element property
			EventSpec* pEventSpec=GetEventSpec();
			if(pEventSpec!=NULL){
				while(pEventSpec->name){
					if (pEventSpec->id == iID){
						sName=pEventSpec->name;
						break;
					}
					pEventSpec++;
				}
			}
		}else{
			// prototype property
			sName = cLPWCSTR((char*)aEventNames[iID]);
		}
		oData << sName.c_str();
	}else{
		oData << L"??";
	}
	PutJSValForSynch(mcx, *vp, oData);
	// посылаем запрос на возможность установки свойства
	MLWORD wID=GetID();
	if(wID!=0xFFFF){
		mlISinchronizer* pSynch=GPSM(mcx)->GetContext()->mpSynch;
		omsresult res=pSynch->CallEvent(wID, (unsigned char*)oData.data(), oData.size());
		if(OMS_SUCCEEDED(res))
			return true;
	}
	return false;
}

mlString mlRMMLElement::GetTagName(){
	if(IsJSO()){
		JSClass* pClass=ML_JS_GETCLASS(mcx, mjso);
		locale loc1 ( "Russian" );

		mlString sTagName=cLPWCSTRq(pClass->name);
		if(isEqual(sTagName.c_str(), L"Object3D"))
			sTagName=L"object";
		wchar_t* wstr=(wchar_t*)sTagName.c_str();
//		use_facet<ctype<wchar_t> > ( loc1 , 0,false).tolower
//		  ( &wstr[0], &wstr[wcslen(&wstr[0])-1] );
		_wcslwr_s(wstr, sTagName.length()+1);
		return wstr;
	}
	return L"??";
}

// сиинхронизировать состояние объекта на всех клиентах
// (вызывается из медиа-объекта)
bool mlRMMLElement::synchronize(mlSynchData* apData){
	if(GPSM(mcx)->Updating()){
		// сюда врядли зайдет, но на всякий случай...
		return realSynchronize(apData);
	}
	// сохраняем, чтобы на следующем update-е вызвать 
	mlSynchData* pData = NULL;
	if(apData == NULL)
		apData = pData = MP_NEW( mlSynchData);
	SaveDataForSynch(apData);
	if(pData != NULL)
		 MP_DELETE( pData);
	return true;
}

// synchronize(mlSynchData* apData=NULL) после кэширования
bool mlRMMLElement::realSynchronize(mlSynchData* apData){
	// подготавливаем данные
	mlSynchData oData;
	oData << SNCMAG_SERIES; // 0x7E
	oData << (unsigned char)1;
	oData << SNCOT_MOS;
	oData << (unsigned int)apData->size();
	oData.put(apData->data(), apData->size());
	// отсылаем при необходимости на синхронизацию
	if(GPSM(mcx)->mbDontSendSynchEvents || GPSM(mcx)->GetMode() != smmNormal){
		// если не надо синхронизировать с другими клиентами (режим воспроизведения)
		// то отправляем обратно в медиа-объект	
		char cSNCMAG_SERIES=0;
		oData >> cSNCMAG_SERIES;
		synchronize(oData);
	}else{
		MLWORD wID=GetID();
		if(wID!=0xFFFF){
			mlISinchronizer* pSynch=GPSM(mcx)->GetContext()->mpSynch;
			omsresult res=OMS_OK;
			res=pSynch->CallEvent(wID, (unsigned char*)oData.data(), oData.size());
		}
	}
	return true;
}

bool mlRMMLElement::NeedToSave(int aiPropGroupType,const wchar_t* apwcName,
							   const wchar_t* apwcVal,mlRMMLElement* apTempElem){
	if(*apwcVal==L'\0') return false;
	if(apTempElem != NULL && apTempElem->IsJSO()){
		jsval v;
		wr_JS_GetUCProperty(mcx,apTempElem->mjso,apwcName,wcslen(apwcName),&v);
		wchar_t* jsc=wr_JS_GetStringChars(JS_ValueToString(mcx,v));
		if(isEqual(apwcVal,jsc))
			return false;
	}
	if(aiPropGroupType==MLMPT_ELEMENT){
		if(isEqual(apwcName,L"_parent")) return false;
		return true;
	}
	if(aiPropGroupType==MLMPT_VISIBLE){
		if(isEqual(apwcName,L"_width") || isEqual(apwcName,L"_height")){
			if(PMO_IS_NULL(mpMO)){
				if(IsComposition()) return false;
			}else{
				mlSize size=GetVisible()->GetmoIVisible()->GetMediaSize();
				if(isEqual(apwcName,L"_width")){
					JSString* jssVal=JS_ValueToString(mcx,INT_TO_JSVAL(size.width));
					if(isEqual(apwcVal,wr_JS_GetStringChars(jssVal)))
						return false;
				}
				if(isEqual(apwcName,L"_height")){
					JSString* jssVal=JS_ValueToString(mcx,INT_TO_JSVAL(size.height));
					if(isEqual(apwcVal,wr_JS_GetStringChars(jssVal)))
						return false;
				}
			}
		}
		if(isEqual(apwcName,L"_xmouse") || isEqual(apwcName,L"xMouse") || 
			isEqual(apwcName,L"_ymouse") || isEqual(apwcName,L"yMouse")
		)
			return false;
		return true;
	}
	return true;
}

char mlRMMLElement::mlListeners::GetUserEventID(const wchar_t* apwcIDName){
	for(int ii=0; ii<miSize; ii++){
		if(maIDs[ii] < 0){
			const wchar_t* pwcEventID = (wchar_t*)(*maListeners[ii]->begin());
			if(isEqual(apwcIDName, pwcEventID))
				return maIDs[ii];
		}
	}
	return 0;
}

const wchar_t* mlRMMLElement::mlListeners::GetUserEventName(char aidEvent){
	for(int ii=0; ii<miSize; ii++){
		if(maIDs[ii] < 0 && maIDs[ii] == aidEvent)
			return (const wchar_t*)(*maListeners[ii]->begin());
	}
	return NULL;
}

char mlRMMLElement::mlListeners::AddUserEvent(const wchar_t* apwcUserEvent){
	if(apwcUserEvent == NULL || *apwcUserEvent == L'\0')
		return 0;
	// grow maIDs
	char* aIDsNew = MP_NEW_ARR( char, miSize+1);
	if(miSize > 0)
		memcpy(aIDsNew, maIDs, miSize);
	// grow maListeners
	pv_elems* aListenersNew = MP_NEW_ARR( pv_elems, miSize+1);
	if(miSize > 0)
		memcpy(aListenersNew, maListeners, sizeof(pv_elems*)*miSize);
	// count user event code
	unsigned char ucUserEventCode = 0x81;
	if(miSize > 0){
		char cLastEventCode = maIDs[miSize-1];
		if(cLastEventCode < 0)
			ucUserEventCode = ((unsigned char)cLastEventCode)+1;
	}
	// 
	if(maIDs != NULL)  MP_DELETE_ARR( maIDs);
	maIDs = aIDsNew; 
	maIDs[miSize] = (char)ucUserEventCode;
	if(maListeners != NULL)  MP_DELETE_ARR( maListeners);
	maListeners = aListenersNew;
	MP_NEW_VECTOR(pv, v_elems);
	maListeners[miSize] = pv;
	int bufferSize = wcslen(apwcUserEvent)+1;
	wchar_t* pwcUserEventIDCopy = MP_NEW_ARR( wchar_t, bufferSize);
	//wcscpy(pwcUserEventIDCopy, apwcUserEvent);
	rmmlsafe_wcscpy(pwcUserEventIDCopy, bufferSize, 0, apwcUserEvent);
	pv->push_back((mlRMMLElement*)pwcUserEventIDCopy);
	miSize++;
	mbChanged = true;
	return (char)ucUserEventCode;
}

void mlRMMLElement::ParseUserEvents(const wchar_t* apwcValue){
	if(apwcValue == NULL || *apwcValue==L'\0') return;
	mlString sUserEvent;
	const wchar_t* pwcStart = SkipDelims(apwcValue);
	const wchar_t* pwc = pwcStart;
	for(;;){
		pwc = GetNextDelimPos(pwc);
		sUserEvent.assign(pwcStart, pwc-pwcStart);
		if(mpListeners.get()==NULL){
			EventSpec* pEventSpec=GetEventSpec();
			if(pEventSpec==NULL) return;
			mpListeners = std::auto_ptr<mlListeners>(new mlListeners(pEventSpec));
		}
		mpListeners->AddUserEvent(sUserEvent.c_str());
		if(*pwc == L'\0')
			break;
		pwcStart = pwc = SkipDelims(pwc);
	}
}

mlString mlRMMLElement::mlListeners::GetUserEvents(){
	mlString userEvents;
	for(int ii=0; ii<miSize; ii++){
		pv_elems pv=maListeners[ii];
		if(pv!=NULL){
			if(maIDs[ii] < 0){
				if(!userEvents.empty())
					userEvents += L", ";
				userEvents += (wchar_t*)(*pv->begin());
			}
		}
	}
	return userEvents;
}

mlString mlRMMLElement::GetUserEvents(){
	if(mpListeners.get()==NULL)
		return L"";
	return mpListeners->GetUserEvents();
}

JSBool mlRMMLElement::JSFUNC_fireEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	if(argc == 0 || !JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx, "fireEvent method must get a string argument");
		return JS_FALSE;
	}
	if(argc > 1)
		JS_ReportWarning(cx, "fireEvent method must get a string argument");
	JSString* jssEventID = JSVAL_TO_STRING(argv[0]);
	wchar_t* jscEventID = wr_JS_GetStringChars(jssEventID);
//if( isEqual(jscEventID, L"onResize") && isEqual(pMLEl->mpName, L"edLogin"))
//int jkghkh=0;
	EventSender* pEventSender = pMLEl->GetEventSender(jscEventID);
bool bDSSE = GPSM(cx)->mbDontSendSynchEvents;
	if(pEventSender == NULL){
		JS_ReportError(cx, "Event %s not found in fireEvent method", cLPCSTR(jscEventID));
		return JS_TRUE;
	}
	pEventSender->pSender->CallListeners(pEventSender->idEvent);
	MP_DELETE( pEventSender);
	return JS_TRUE;
}

mlresult mlRMMLElement::CallFunction(mlSynchData &aData){
	wchar_t* pwcFuncName = NULL;
	aData >> pwcFuncName;
	bool bEvent;
	aData >> bEvent;
	if(pwcFuncName == NULL) return ML_ERROR_FAILURE;
	// если есть такая функция
	jsval vFunc=JSVAL_NULL;
	wr_JS_GetUCProperty(mcx, mjso, pwcFuncName, -1, &vFunc);
	if(!JSVAL_IS_REAL_OBJECT(vFunc) || !JS_ObjectIsFunction(mcx,JSVAL_TO_OBJECT(vFunc))){
		if(bEvent) return ML_OK;
		mlTraceError(mcx, "Function %S not found\n", pwcFuncName);
		return ML_ERROR_NOT_IMPLEMENTED;
	}
//	RelinkClassFunction(vFunc, pwcFuncName);
	// то вызвать ее
	jsval* vArgs = NULL;
	unsigned char ucArgC;
	aData >> ucArgC;
	if(ucArgC & 0x80){
		ucArgC &= 0x7F;
		if(ucArgC > 0){
			vArgs = MP_NEW_ARR( jsval, ucArgC);
			for(int ij = 0; ij < ucArgC; ij++){
				char chType = MLPT_UNKNOWN;
				aData >> chType;
				jsval vArg = JSVAL_NULL;
				switch(chType){
				case MLPT_INT:{
					int iVal;
					aData >> iVal;
					vArg = INT_TO_JSVAL(iVal);
					}break;
				case MLPT_DOUBLE:{
					double dVal;
					aData >> dVal;
					mlJS_NewDoubleValue(mcx, dVal, &vArg);
					}break;
				case MLPT_STRING:{
					wchar_t* pwcVal = NULL;
					aData >> pwcVal;
					vArg = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(mcx,pwcVal));
					}break;
				}
				vArgs[ij] = vArg;
			}
		}
	}else{
		if(ucArgC > 0){
			vArgs = MP_NEW_ARR( jsval, ucArgC);
			for(int ij = 0; ij < ucArgC; ij++){
				aData >> vArgs[ij];
			}
		}
	}
	jsval v;
	JS_CallFunctionValue(mcx, mjso, vFunc, ucArgC, vArgs, &v);
//	JS_CallFunction(mcx, JS_GetParent(mcx, JS_GetFunctionObject(jsf)), jsf, ucArgC, vArgs, &v);
//	JS_CallFunction(mcx, mjso, jsf, ucArgC, vArgs, &v);
	if(vArgs!=NULL)  MP_DELETE_ARR( vArgs);
	return ML_OK;
}

void MakePropsEnumerable(JSContext* cx, JSObject* jso, JSPropertySpec* apPropSpec){
	for(int ii=0; ; ii++){
		JSPropertySpec* pPropSpec=&(apPropSpec[ii]);
		if(pPropSpec->name==NULL) return;
		if(!(pPropSpec->flags & JSPROP_ENUMERATE))
			continue;
		if( isEqual(pPropSpec->name,"_xmouse") ||
			isEqual(pPropSpec->name,"_ymouse") ||
			isEqual(pPropSpec->name,"_parent") ||
			isEqual(pPropSpec->name,"_class")
		){
			continue;
		}
		jsval vProp;
		JS_GetProperty(cx,jso,pPropSpec->name,&vProp);
		JS_SetProperty(cx,jso,pPropSpec->name,&vProp);
	}
}

void mlRMMLElement::MakeAllPropsEnumerable(bool abRecurse){
	if(!IsJSO()) return;
	MakePropsEnumerable(mcx, mjso, mlRMMLElement::_JSPropertySpec);
	// enum prototypes
	if(mRMMLType & MLMPT_LOADABLE)
		MakePropsEnumerable(mcx, mjso, mlRMMLLoadable::_JSPropertySpec);
	if(mRMMLType & MLMPT_VISIBLE)
		MakePropsEnumerable(mcx, mjso, mlRMMLVisible::_JSPropertySpec);
//	if(mRMMLType & MLMPT_AUDIBLE)
//		MakePropsEnumerable(mcx, mjso, mlRMMLAudible::_JSPropertySpec);
	if(mRMMLType & MLMPT_CONTINUOUS)
		MakePropsEnumerable(mcx, mjso, mlRMMLContinuous::_JSPropertySpec);
	if(mRMMLType & MLMPT_BUTTON)
		MakePropsEnumerable(mcx, mjso, mlRMMLPButton::_JSPropertySpec);
	if(mRMMLType & MLMPT_3DOBJECT)
		MakePropsEnumerable(mcx, mjso, mlRMML3DObject::_JSPropertySpec);
	MakePropsEnumerable(mcx, mjso, GetPropSpec());
	if(!abRecurse) return;
	if(mpChildren==NULL) return;
	v_elems::iterator vi;
	for(vi = mpChildren->begin(); vi != mpChildren->end(); vi++){
		mlRMMLElement* pChild=*vi;
		if(pChild->IsJSO()){
			pChild->MakeAllPropsEnumerable(true);
		}
	}
}

typedef mlMedia* mlMediaPtr;

mlMedia** mlRMMLElement::GetChildren(){
	if(mpChildren == NULL) return NULL;
	mlMedia** paChildren = MP_NEW_ARR( mlMediaPtr, mpChildren->size()+1);
	int i = 0;
	v_elems::iterator vi;
	for(vi=mpChildren->begin(); vi != mpChildren->end(); vi++ ){
		mlRMMLElement* pChild=*vi;
		if(pChild == NULL) continue;
		if(!(pChild->IsJSO())) continue; // skip scripts
		paChildren[i++] = pChild;
	}
	if (i == 0) return NULL;
	paChildren[i++] = NULL;
	return paChildren;
}

mlMedia** mlRMMLElement::GetAllVisibleChildren(){
	if(IsComposition()){
		typedef std::vector<mlRMMLElement*> v_elems_s;
		v_elems_s vVisibles;
		mlRMMLIterator iter(mcx, this);
		mlRMMLElement* pElem = NULL;
		while((pElem = iter.GetNext()) != NULL){
			if(pElem->GetMO() == NULL){
				// JS_ReportWarning(mcx,"composition is not completely loaded yet");
				return NULL;
			}
			if(pElem->IsVisible() && !PMO_IS_NULL(pElem->GetMO()))
				vVisibles.push_back(pElem);
		}
		if(vVisibles.size() > 0){
			mlMedia** paChildren = MP_NEW_ARR( mlMediaPtr, vVisibles.size()+1);
			memcpy(paChildren, (void*)&vVisibles[0], vVisibles.size() * sizeof(mlMedia*));
			int i = 0;
			paChildren[vVisibles.size()] = NULL;
			return paChildren;
		}
	}
	return NULL;
}

void mlRMMLElement::FreeChildrenArray(mlMedia** apaChildren){
	if(apaChildren != NULL)
		 MP_DELETE_ARR( apaChildren);
}

void mlRMMLElement::ParseScriptProviders(const wchar_t* apwcValue){
	if(apwcValue == NULL || *apwcValue==L'\0') return;
	mlString sScriptProvider;
	const wchar_t* pwcStart = SkipDelims(apwcValue);
	const wchar_t* pwc = pwcStart;
	for(;;){
		pwc = GetNextDelimPos(pwc);
		sScriptProvider.assign(pwcStart, pwc-pwcStart);
		ImportScriptFromProvider(sScriptProvider.c_str());
		if(*pwc == L'\0')
			break;
		pwcStart = pwc = SkipDelims(pwc);
	}
}

void mlRMMLElement::ImportScriptFromProvider(const wchar_t* apwcClassName){
	JSObject* jsoClass = FindClass(apwcClassName);
	if(jsoClass == NULL){
		JS_ReportError(mcx,"Script provider class '%s' not found", cLPCSTR(apwcClassName));
		return;
	}
	if(!JSO_IS_MLEL(mcx, jsoClass)){
		JS_ReportError(mcx,"Script provider class object '%s' is not a RMML object", cLPCSTR(apwcClassName));
		return;
	}
	SAVE_FROM_GC(mcx, mjso, jsoClass);
	mlRMMLElement* pMLElClass = GET_RMMLEL(mcx, jsoClass);
	// копируем скрипты 
	if(pMLElClass->mpChildren == NULL) return;
	for(unsigned int ii=0; ii<pMLElClass->mpChildren->size(); ii++){
		mlRMMLElement* pElToCopy=(*(pMLElClass->mpChildren))[ii];
		if(pElToCopy->mRMMLType != MLMT_SCRIPT)
			continue;
		mlRMMLElement* pNewMLEl=pElToCopy->Duplicate(this);
		ML_ASSERTION(mcx, pNewMLEl != NULL, "mlRMMLElement::ImportScriptFromProvider");
		pNewMLEl->SetSrcFileIdx(pElToCopy->miSrcFilesIdx, pElToCopy->miSrcLine);
		pNewMLEl->SetParent(this);
		if(pNewMLEl->IsOnloadScript()){
			if(((mlRMMLScript*)pElToCopy)->mpClass == NULL){
				((mlRMMLScript*)pNewMLEl)->mpClass = pMLElClass;
			}else
				((mlRMMLScript*)pNewMLEl)->mpClass = ((mlRMMLScript*)pElToCopy)->mpClass;
		}
	}
	if(!PMO_IS_NULL(mpMO))
		ActivateScripts();
}

void mlRMMLElement::NotifyParentChanged(){
	// если видимый, то...
	mlRMMLVisible* pVis = GetVisible();
	if(pVis != NULL){
		pVis->Visible_NotifyParentChanged();
	}
	mlRMMLPButton* pBut = GetButton();
	if(pBut != NULL){
		pBut->Button_ParentChanged();
	}
	if(!PMO_IS_NULL(mpMO)){
		mpMO->ParentChanged();
	}
	mlRMMLIterator Iter(mcx,this);
	mlRMMLElement* pElem=NULL;
	while((pElem=Iter.GetNext())!=NULL){
		pElem->NotifyParentChanged();
	}
}

__forceinline const wchar_t* SkipJSConstValue(const wchar_t* apwc){
	bool bInString = false;
	for(; *apwc!=L'\0'; apwc++){
		wchar_t wc = *apwc;
		if(bInString){
			if(wc == L'\\'){
				apwc++;
				continue;
			}
			if(wc == L'\''){
				bInString = false;
			}
			continue;
		}
		if(wc == L'\''){
			bInString = true;
			continue;
		}
		if(IsDelim(*apwc))
			break;
	}
	return apwc;
}

mlresult mlRMMLElement::SetUserProperties(const wchar_t* apwcUserProperties){
	if(apwcUserProperties == NULL || *apwcUserProperties==L'\0') return ML_OK;
	mlString sUserProperties(apwcUserProperties);
	const wchar_t* pwcStart = SkipDelims(sUserProperties.c_str());
	const wchar_t* pwc = pwcStart;
	for(;;){
		if(*pwc == L'\0') break;
		pwc = GetNextDelimPos(pwc);
		if(*pwc != L'='){
			mlTraceError(mcx, "Invalid userProperties attribute value %S\n",GetSrcFileRef().c_str());
			return ML_ERROR_INVALID_ARG;
		}
		mlString sPropName;
		sPropName.assign(pwcStart, pwc-pwcStart);
		pwc++;
		if(*pwc == L'\0') break;
		const wchar_t* pwcValue = pwc;
		pwc = SkipJSConstValue(pwc);
		mlString sPropValue;
		sPropValue.assign(pwcValue, pwc-pwcValue);
		//
		{	GUARD_JS_CALLS(mcx);
			jsval vPorpVal = GPSM(mcx)->GetBuildEvalCtxt()->Eval(mcx, sPropValue.c_str());
			wr_JS_SetUCProperty(mcx, mjso, sPropName.c_str(), -1, &vPorpVal);
		}
		//
		if(*pwc == L'\0') break;
		const wchar_t* pwcNext = SkipDelims(pwc);
//		(*(wchar_t*)pwc++) = L';';
//		while(pwc != pwcNext){
//			(*(wchar_t*)pwc++) = L' ';
//		}
		pwcStart = pwc = pwcNext;
	}
//	jsval v;
//	JSBool bR = JS_EvaluateUCScript(mcx,mjso,sUserProperties.c_str(),sUserProperties.length(),
//		cLPCSTRq(GPSM(mcx)->mSrcFileColl[miSrcFilesIdx]),miSrcLine,&v); 
	return ML_OK;
}

//const wchar_t* mlRMMLElement::GetFullClassName(mlRMMLElement* apMLElClass){
//	if(apMLElClass == NULL) return NULL;
//	// получаем "classes"
//	jsval vClss;
//	JS_GetProperty(mcx, JS_GetGlobalObject(mcx), GJSONM_CLASSES, &vClss);
//	if(!JSVAL_IS_REAL_OBJECT(vClss)) return NULL;
//	JSObject* jsoClasses = JSVAL_TO_OBJECT(vClss);
//	//
//	JSIdArray* pGPropIDArr= JS_Enumerate(mcx, jsoClasses);
//	for(int ikl=0; ikl<pGPropIDArr->length; ikl++){
//		jsid id=pGPropIDArr->vector[ikl];
//		jsval v;
//		if(!JS_IdToValue(mcx,id,&v)) continue;
//		if(!JSVAL_IS_STRING(v)) continue;
//		JSString* jss = JSVAL_TO_STRING(v);
//		jschar* jsc=JS_GetStringChars(JSVAL_TO_STRING(v));
//		jsval vProp;
//		JS_GetUCProperty(mcx, jsoClasses, jsc, -1, &vProp);
//		if(!JSVAL_IS_REAL_OBJECT(vProp)) continue;
//		if(JSVAL_TO_OBJECT(vProp) == apMLElClass->mjso)
//			return jsc;
//	}
//	return NULL;
//}

JSBool mlRMMLElement::JSFUNC_getSubclass(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	if(argc == 0 || !JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx, "getSubclass method must get a string argument");
		return JS_FALSE;
	}
	if(argc > 1)
		JS_ReportWarning(cx, "getSubclass method must get a string argument");
	JSString* jssSubclassName = JSVAL_TO_STRING(argv[0]);
	wchar_t* jscSubclassName = wr_JS_GetStringChars(jssSubclassName);
	JSObject* jsoSubClass = NULL;
	if(pMLEl->mpClass != NULL){
		// находим свой класс в Classes
//		jsoSubClass = pMLEl->mpClass->FindClass(jscSubclassName);
//		const wchar_t* pwcFullClassName = pMLEl->GetFullClassName(pMLEl->mpClass);
//		if(pwcFullClassName != NULL){
		mlString sFullSublassName = wr_JS_GetStringChars(pMLEl->mpClass->_name);
		sFullSublassName += L'_';
		sFullSublassName += jscSubclassName;
		jsoSubClass = pMLEl->FindClass(+sFullSublassName.c_str());
//		}
	}
	if(jsoSubClass == NULL) *rval = JSVAL_NULL;
	else *rval = OBJECT_TO_JSVAL(jsoSubClass);
	return JS_TRUE;
}

JSBool mlRMMLElement::JSFUNC_getChildren(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx, obj);
	int iLen = 0;
	jsval* pjsvals;
	if(pMLEl->mpChildren == NULL){
		pjsvals = MP_NEW_ARR( jsval, 1);
	}else{
		pjsvals = MP_NEW_ARR( jsval, pMLEl->mpChildren->size() + 1);
		v_elems::iterator vi;
		for(vi = pMLEl->mpChildren->begin(); vi != pMLEl->mpChildren->end(); vi++){
			mlRMMLElement* pChild = *vi;
			if(pChild->IsJSO()){
				pjsvals[iLen++] = OBJECT_TO_JSVAL(pChild->mjso);
			}
		}
	}
	JSObject* jso=JS_NewArrayObject(cx,iLen,pjsvals);
	MP_DELETE_ARR( pjsvals);
	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

/*JSBool mlRMMLElement::JSFUNC_synch_fixPos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_FALSE;
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	oms::omsContext* pContext = GPSM(cx)->GetContext();
	if(pContext->mpMapMan == NULL)
		return JS_TRUE;
	bool bRet = pContext->mpMapMan->FixPos(pMLEl);
	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}*/

JSBool mlRMMLElement::JSFUNC_synch_remove(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_FALSE;
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	oms::omsContext* pContext = GPSM(cx)->GetContext();
	if(pContext->mpMapMan == NULL)
		return JS_TRUE;
	bool bRet = pContext->mpMapMan->Remove(pMLEl);
	*rval = BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

JSBool mlRMMLElement::JSFUNC_getBornReality(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);

	oms::omsContext* pContext = GPSM(cx)->GetContext();
	if(pContext->mpMapMan == NULL)
		return JS_TRUE;

	JSObject* pRealityObject = GetRealityObject( cx, pContext->mpMapMan, pMLEl->muBornRealityID);
	if (pRealityObject != NULL)
		*rval = OBJECT_TO_JSVAL( pRealityObject);

	return JS_TRUE;
}

// Создает mlRMMLScript, который будет выполняться при возникновении определенного события.
// Аналог декларативного способа <script event="событие">скрипт<script>.
// bool createEventScript(event, script);
JSBool mlRMMLElement::JSFUNC_createEventScript(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_FALSE;
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	if(argc < 2 || !JSVAL_IS_STRING(argv[0]) || !JSVAL_IS_STRING(argv[1])){
		JS_ReportError(cx, "createEventScript method must get two string arguments");
		return JS_TRUE;
	}
	const wchar_t* pwcEvent = wr_JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
	const wchar_t* pwcScript = wr_JS_GetStringChars(JSVAL_TO_STRING(argv[1]));
	if(pwcEvent == NULL || pwcScript == NULL){
		return JS_FALSE;
	}

	mlRMMLScript* pNewScriptElem = MP_NEW( mlRMMLScript);
	pNewScriptElem->mcx = cx;
	pNewScriptElem->SetProperty(L"event", pwcEvent);
	pNewScriptElem->SetParent(pMLEl);
	pNewScriptElem->SetValue((wchar_t*)pwcScript);
	pNewScriptElem->Activate();
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

// перебирает все дочерние элементы первого уровня 
// и вызывает для кождого из них функцию, переданную в качестве аргумента
// applyToChildren(function, arg)
JSBool mlRMMLElement::JSFUNC_applyToChildren(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_FALSE;
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	if(argc < 1){
		JS_ReportError(cx, "applyToChildren method must get one argument at least");
		return JS_TRUE;
	}
	jsval vFunc = argv[0];
	if(JS_TypeOfValue(cx, vFunc) != JSTYPE_FUNCTION){
		JS_ReportWarning(cx, "First argument of applyToChildren method must get a function");
		return JS_TRUE;
	}
	int iArgCount = 0;
	jsval av[10];
	if(argc > 1){
		iArgCount = 1;
		av[0] = argv[1];
	}

	if(pMLEl->mpChildren != NULL){
		v_elems::const_iterator cvi = pMLEl->mpChildren->begin();
		for(; cvi != pMLEl->mpChildren->end(); cvi++){
			mlRMMLElement* pChild = *cvi;
			if(pChild == NULL) continue;
			if(!(pChild->IsJSO())) continue; // skip scripts
			jsval rv;
			JS_CallFunctionValue(cx, pChild->mjso, vFunc, iArgCount, av, &rv);
		}
	}
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

// Запрашивает полное состояние объекта
// queryFullState();
JSBool mlRMMLElement::JSFUNC_queryFullState(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_FALSE;
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	oms::omsContext* pContext = GPSM(cx)->GetContext();
	if(pContext->mpSyncMan == NULL)
		return JS_TRUE;
	pContext->mpSyncMan->QueryObject( pMLEl->GetID(), pMLEl->GetBornRealityID(),
		pContext->mpMapMan->GetCurrentRealityID(), 0, false);
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

JSBool mlRMMLElement::JSFUNC_getServerVersion(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_FALSE;
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	if(pMLEl == NULL)
		return JS_TRUE;
	unsigned int serverVersion = pMLEl->GetServerVersion();
	*rval = INT_TO_JSVAL(serverVersion);
	return JS_TRUE;
}

bool mlRMMLElement::Freeze(){
	_frozen = true;
	mlRMMLContinuous* pCont = GetContinuous();
	if(pCont != NULL) pCont->Freeze();
	if(!PMO_IS_NULL(mpMO)) mpMO->FrozenChanged();
	CallFrozenChangedJSFunc();
	return true; 
}

bool mlRMMLElement::Unfreeze(){
	_frozen = false;
	mlRMMLContinuous* pCont = GetContinuous();
	if(pCont != NULL) pCont->Unfreeze();
	if(!PMO_IS_NULL(mpMO)) mpMO->FrozenChanged();
	CallFrozenChangedJSFunc();
	return true; 
}

static const wchar_t* gpwcFrozenChangedFuncName = L"onFrozenChanged";
static int giFrozenChangedFuncNameLen = 0;

void mlRMMLElement::CallFrozenChangedJSFunc(){
	if(giFrozenChangedFuncNameLen == 0)
		giFrozenChangedFuncNameLen = wcslen(gpwcFrozenChangedFuncName);
	jsval vFunc = JSVAL_NULL;
	if(wr_JS_GetUCProperty(mcx, mjso, gpwcFrozenChangedFuncName, giFrozenChangedFuncNameLen, &vFunc) == JS_FALSE)
		return;
	if(JSVAL_IS_REAL_OBJECT(vFunc) && JS_ObjectIsFunction(mcx, JSVAL_TO_OBJECT(vFunc))){
		jsval v;
		JS_CallFunctionValue(mcx, mjso, vFunc, 0, NULL, &v);
	}
}

void mlRMMLElement::ZoneChanged(int aiZone){
	if(mpSynch == NULL)
		return;
	mpSynch->ZoneChanged(aiZone);
}

void mlRMMLElement::RealityChanged(unsigned int auReality){
	muRealityID = auReality;
}

void mlRMMLElement::ReadyForBinState(){
	if(mpSynch == NULL)
		return;
	mpSynch->ApplyFullBinState();
}

void mlRMMLElement::SetForcedUpdateBinState(bool updateBinState){
	_updateBinState = updateBinState;
}

bool mlRMMLElement::IsForcedUpdateBinState(){
	return _updateBinState;
}

void mlRMMLElement::HandleSynchMessage(const wchar_t* apwcSender, mlSynchData& aData){
	if(mpSynch == NULL)
		return;
	mpSynch->HandleSynchMessage(apwcSender, aData);
}

void mlRMMLElement::DontSynchLastChanges(){
	if(mpSynch == NULL)
		return;
	mpSynch->DontSynchLastChanges();
}

bool mlRMMLElement::IsMyOwner(){
	if(mpSynch == NULL)
		return false;
	return mpSynch->IsMyOwn();
}

void mlRMMLElement::FireSynchOnInitialized(){
	if(mpSynch == NULL)
		return;
	mpSynch->FireOnInitialized();
}

mlresult mlRMMLElement::Update(const __int64 alTime){ 
	mlresult res = ML_ERROR_NOT_IMPLEMENTED;
	mlRMMLVisible* pVis = GetVisible();
	if(pVis != NULL){
		if(pVis->UpdateVisualRepresentationChanged() != ML_ERROR_NOT_IMPLEMENTED)
			res = ML_OK;
	}
	if(!PMO_IS_NULL(mpMO)){
		if(mpMO->Update(alTime))
			res = ML_OK;
	}
	return res;
}

mlRMMLElement* mlRMMLElement::Duplicate(mlRMMLElement* apNewParent){ 
	ML_ASSERTION(mcx, false,"Duplicate") return NULL; 
}

void mlRMMLElement::VisualRepresentationChanged(){
	mlRMMLElement* pParent = this;
	while(pParent){
		mlRMMLVisible* pVis = pParent->GetVisible();
		if(pVis == NULL)
			break;
		pVis->Vis_VisualRepresentationChanged();
		pParent = pParent->mpParent;
	}
}

// объект точно будет удален, поэтому уже гарантировано можно уничтожать его MO (и дочерних элементов)
void mlRMMLElement::ForceMODestroy(v_mos *apMOs){
	if(!PMO_IS_NULL(mpMO)){
		omsContext* pContext = GPSM(mcx)->GetContext();
		if(apMOs != NULL){
			mpMO->mType = mType;
			apMOs->push_back(mpMO);
			mpMO->SetMLMedia(NULL);
			pContext->mpRM->MLMediaDeleting(this);
		}else{
			pContext->mpRM->DestroyMO(this);
		}
		mpMO = PMOV_DESTROYED;
	}
	if(mpChildren == NULL || mpChildren->empty())
		return;
	v_elems::iterator vi;
	for(vi = mpChildren->begin(); vi != mpChildren->end(); vi++){
		mlRMMLElement* pChild = *vi;
		pChild->ForceMODestroy(apMOs);
	}
}

void mlRMMLElement::SetSynchChanged(){
	// если объект дочерний, 
	if(muID == OBJ_ID_UNDEF){
		// то надо уведомить родительский с реальным ID, что его надо синхронизировать
		mlRMMLElement* pParent = mpParent;
		for(; pParent != NULL; pParent = pParent->mpParent){
			if(pParent->muID != OBJ_ID_UNDEF)
				break;
		}
		if(pParent != NULL && pParent->mpSynch != NULL)
			pParent->mpSynch->SetSubobjChanged();
	}
}

void mlRMMLElement::SetLocationSID(const wchar_t* pszLocationID){
	if(mpSynch == NULL)
		return;

	mpSynch->SetLocationSID(pszLocationID);
}

void mlRMMLElement::SetCommunicationAreaSID(const wchar_t* pszCommunicationAreaID){
	if(mpSynch == NULL)
		return;

	mpSynch->SetCommunicationAreaSID(pszCommunicationAreaID);
}

// возвращает true, если элемент является частью класса
bool mlRMMLElement::IsClassChildren(){
	mlRMMLElement* pRootElem = this;
	while(pRootElem->mpParent != NULL){
		pRootElem = pRootElem->mpParent;
	}
	if(pRootElem->muID == OBJ_ID_CLASS)
		return true;
	return false;
}

void mlRMMLElement::SetSrcFileIdx(short aSrcFileIdx, short aiSrcLine){
	miSrcFilesIdx = aSrcFileIdx;
	mpSrcFilePath = GPSM(mcx)->mSrcFileColl[miSrcFilesIdx];
	miSrcLine = aiSrcLine;
}

// Приостановить синхронизацию объекта (но сохранять все изменения до разрешения синхронизировать дальше)
void mlRMMLElement::DontSynchAwhile(){
	if(mpSynch == NULL)
		return;
	mpSynch->DontSynchAwhile();
}
// Возобновить синхронизацию 
void mlRMMLElement::RecommenceSynchronization(){
	if(mpSynch == NULL)
		return;
	mpSynch->RecommenceSynchronization();
}

unsigned __int64 mlRMMLElement::GetSynchChangingTime()
{
	if(mpSynch == NULL)
		return 0;
	return mpSynch->GetLastChangingTime();
}


// получить список всех родительских композиций, у которых надо вызывать кнопочные обработчики
v_elems* mlRMMLElement::GetButtonParents()
{
	v_elems* pvButtonParents = NULL;
	mlRMMLElement* pBut = this;
	while(pBut != NULL){
		if(pBut->mpParent == NULL)
			break;
		if(!pBut->mpParent->IsComposition())
			break;
		mlRMMLComposition* pParentBut = (mlRMMLComposition*)(pBut->mpParent);
		if(pParentBut->mbBtnEvListenersIsSet){
			if(!pParentBut->IsChildButtonsDisabled()){
				if(pvButtonParents == NULL){
					MP_NEW_VECTOR_V(pvButtonParents, v_elems);
				}
				pvButtonParents->push_back(pParentBut);
			}else
				break;	// сюда по идее вообще не должен приходить
		}
		pBut = pParentBut;
	}
	return pvButtonParents;
}

// вызывается, когда на окно плеера перетащили файл
// и при этом мышь была над этим объектом
bool mlRMMLElement::OnFileDrop(const wchar_t* apwcFilePath)
{
	if( GPSM(mcx)->GetContext()->mpLogWriter)
	{
		GPSM(mcx)->GetContext()->mpLogWriter->WriteLn( "[RMML] mlRMMLElement OnFileDrop = ",(char*)cLPCSTRq(apwcFilePath));
		GPSM(mcx)->GetContext()->mpLogWriter->WriteLn( "[RMML] mlRMMLElement OnFileDrop  name = ",(const char*)cLPCSTR(this->GetName()));
	}

	const wchar_t* pwcOnFileDropped = L"onFileDropped";
	jsval vFunc = JSVAL_NULL;
	if(wr_JS_GetUCProperty(mcx, mjso, pwcOnFileDropped, wcslen(pwcOnFileDropped), &vFunc) == JS_FALSE)
	{
		if( GPSM(mcx)->GetContext()->mpLogWriter)
		{
			GPSM(mcx)->GetContext()->mpLogWriter->WriteLn( "[RMML] mlRMMLElement OnFileDrop wr_JS_GetUCProperty == JS_FALSE");
		}
		return  false;
	}


	if(JSVAL_IS_STRING(vFunc))
	{
		if( GPSM(mcx)->GetContext()->mpLogWriter)
		{
			GPSM(mcx)->GetContext()->mpLogWriter->WriteLn( "[RMML] mlRMMLElement OnFileDrop JSVAL_IS_STRING");
		}

		return false;
	}

	if(!(JSVAL_IS_REAL_OBJECT(vFunc) && JS_ObjectIsFunction(mcx, JSVAL_TO_OBJECT(vFunc))))
	{
		if( GPSM(mcx)->GetContext()->mpLogWriter)
		{
			GPSM(mcx)->GetContext()->mpLogWriter->WriteLn( "[RMML] mlRMMLElement OnFileDrop !JSVAL_IS_REAL_OBJECT");
		}
		return false;
	}

	if( GPSM(mcx)->GetContext()->mpLogWriter)
	{
		GPSM(mcx)->GetContext()->mpLogWriter->WriteLn( "[RMML] mlRMMLElement OnFileDrop succes");
	}
	// вызываем функцию 
	JSString* jssFilePath = wr_JS_NewUCStringCopyZ(mcx, apwcFilePath);
	jsval vFilePath = STRING_TO_JSVAL(jssFilePath);
	const char* pcDroppedFilePathPName = "droppedFilePath";
	JS_SetProperty(mcx,GPSM(mcx)->GetEventGO(),pcDroppedFilePathPName,&vFilePath);
	jsval v;
	JS_CallFunctionValue(mcx, mjso, vFunc, 0, NULL, &v);
	JS_DeleteProperty(mcx,GPSM(mcx)->GetEventGO(),pcDroppedFilePathPName);

	return true;
}

void mlRMMLElement::OnLoadStateChanged()
{
	if (mpSynch == NULL)
		return;
	
	mlRMMLLoadable* pLoadable = GetLoadable();

	if (pLoadable != NULL)
	{
		// так как CallListeners(EVID_onSynched) выз-ся сразу, а onLoad ложится в очередь,
		// то элемент считается загруженным, только после вызова метода OnLoadStateChanged
		pLoadable->SetLoaded();
	}

	mlRMMLElement* firstElement = GetFirstElementInSynchHirarchy();

	if (this == firstElement && IsReadyForSynch() && mpSynch->IsWaitForLoading())
	{
		ResetSynchOnlyFireEvent();
		SimulateFullStateCome(true);
	}
}

void mlRMMLElement::SimulateFullStateCome(bool abSynchRoot)
{
	JSObject* jsoSynched = JS_NewObject(mcx, NULL, NULL, NULL);
	jsval jsv = OBJECT_TO_JSVAL(jsoSynched);
	SAVE_FROM_GC(mcx, GPSM(mcx)->GetEventGO(), jsoSynched);

	mpSynch->SimulateFullStateCome(jsoSynched);	

	v_elems::iterator vi;
	v_elems* pvSynchedChildren = mpSynch->GetSynchedChildren();
	if(pvSynchedChildren != NULL && pvSynchedChildren != ML_SYNCH_CHILDREN_EMPTY_VAL){
		for(vi = pvSynchedChildren->begin(); vi != pvSynchedChildren->end(); vi++){
			if(*vi == NULL){
				continue;
			}

			(*vi)->SimulateFullStateCome(false);
		}
	}

	if(abSynchRoot){
		jsval jsv = OBJECT_TO_JSVAL(jsoSynched);
		FinishInitialization(jsv, true);
	}

	FREE_FOR_GC(mcx, GPSM(mcx)->GetEventGO(), jsoSynched);
}

mlRMMLElement* mlRMMLElement::GetFirstElementInSynchHirarchy()
{
	mlRMMLElement* pPrevParent = this;
	mlRMMLElement* pParent = mpParent;

	while(pParent!=NULL && pParent->mRMMLType!=MLMT_SCENE){
		if(pParent->IsSynchronized() &&
			(pParent->mRMMLType == MLMT_CHARACTER || pParent->mRMMLType == MLMT_OBJECT) )
				pPrevParent = pParent;

		pParent = pParent->mpParent;
	}

	return pPrevParent;
}

bool mlRMMLElement::IsReadyForSynch()
{
	mlRMMLLoadable* pLoadable = GetLoadable();

	if (pLoadable == NULL)
			return true;

	if (pLoadable->IsLoading())
			return false;

	return true;
}

bool mlRMMLElement::IsRootReadyForSynch()
{
	// берем верхний элемент в иерархии
	// проверям тип (3D - объект или персонаж) и состояние загружен или нет
	// если да, можно вызывать onReset, onSynched

	mlRMMLElement* firstElement = GetFirstElementInSynchHirarchy();
	return firstElement->IsReadyForSynch();
}

void mlRMMLElement::FireOnSynched(JSObject* jsoSynched, bool bFullState)
{
	if (mpSynch->IsWaitForLoading() == true)
		return;

	jsval jsv = OBJECT_TO_JSVAL(jsoSynched);
	jsval v = BOOLEAN_TO_JSVAL(bFullState);
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(),ISFULLSTATE_PROP_NAME,&v);

	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), SYNCHED_PROP_NAME, &jsv);
	CallListeners(EVID_onSynched);
	JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), SYNCHED_PROP_NAME);

	JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), ISFULLSTATE_PROP_NAME);
}

}

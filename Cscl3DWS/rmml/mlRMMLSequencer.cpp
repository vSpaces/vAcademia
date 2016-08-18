#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml {

mlRMMLSequencer::mlRMMLSequencer(void):
	MP_VECTOR_INIT(mvWaitingEvents),
	MP_VECTOR_INIT(mvFiredEvents)
{
	mType = MLMT_SEQUENCER;
	mRMMLType = MLMT_SEQUENCER;
	//
	mlTimeOfWaitingEnd = -1;
	mlTimeToWaitAfterUnfreez = -1;
	mpEventSender = NULL;
	mpWaitJSExpr = NULL;
//	miCurrentFrameAfterUpdate=currentFrame;
}

mlRMMLSequencer::~mlRMMLSequencer(void){
	if(mpEventSender!=NULL){ 
		// мы ждали события от кого-то, а теперь мы уничтожаемся
		// и поэтому надо ему сказать, что мы его больше не слушаем
		mpEventSender->pSender->RemoveEventListener(mpEventSender->idEvent,this);
		//
		MP_DELETE( mpEventSender);
	}
	JSObject* jso = mjso;
	mjso = NULL;
	FreeWaitJSExpr();
	mjso = jso;
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLSequencer::_JSPropertySpec[] = {
//	{ "x", JSVAR_x, JSPROP_ENUMERATE, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLSequencer::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Sequencer,mlRMMLSequencer,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLContinuous)
MLJSCLASS_IMPL_END(mlRMMLSequencer)

///// JavaScript Set Property Wrapper
JSBool mlRMMLSequencer::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLSequencer);
		SET_PROTO_PROP(mlRMMLElement);
		SET_PROTO_PROP(mlRMMLContinuous);
		default:
			// if(id==??)
			;
	SET_PROPS_END;
	//if(JSVAL_IS_STRING(id)){
	//	if(JS_TypeOfValue(cx,*vp)==JSTYPE_FUNCTION){
	//		// возможно (пере)устанавливается обработчик событий.
	//		// поищем его в карте событий
	//		wchar_t* pwc=JS_GetStringChars(JSVAL_TO_STRING(id));
	//		JSFunction* jsf=JS_ValueToFunction(cx, *vp);
	//		if(jsf!=NULL){
	//			int hh=0;
	//		}
	//	}else if(JSVAL_IS_VOID(*vp)){
	//		// возможно удаляется обработчик событий.
	//		// поищем его в карте событий
	//		int hh=0;
	//	}
	//	// 
	//	int hh=0;
	//}
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLSequencer::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLSequencer);
		GET_PROTO_PROP(mlRMMLElement);
		GET_PROTO_PROP(mlRMMLContinuous);
		default:
			;
	GET_PROPS_END;
	return JS_TRUE;
}

mlRMMLElement* mlRMMLSequencer::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLSequencer* pNewEL=(mlRMMLSequencer*)GET_RMMLEL(mcx, mlRMMLSequencer::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

void mlRMMLSequencer::FreeWaitJSExpr(){
	if(mpWaitJSExpr==NULL) return;
	if(mjso != NULL){
		FREE_STR_FOR_GC(mcx,mjso,mpWaitJSExpr->mjssExpr)
	}
	MP_DELETE( mpWaitJSExpr);
	mpWaitJSExpr=NULL;
	if(mvWaitingEvents.size() > 0){
		std::vector<EventSender>::iterator vi;
		for(vi = mvWaitingEvents.begin(); vi != mvWaitingEvents.end(); vi++){
			if((*vi).pSender < (mlRMMLElement*)0xFF){
				GPSM(mcx)->RemoveEventListener((char)((*vi).pSender),this);
			}else{
				(*vi).pSender->RemoveEventListener((*vi).idEvent,this);
			}
		}
		mvWaitingEvents.clear();
	}
}

class mlCheckEventsMode
{
	mlSceneManager* mpSM;
public:
	mlCheckEventsMode(std::vector<EventSender>* pvFiredEvents, mlRMMLElement* apSELMLEl){
		mpSM = GPSM(apSELMLEl->mcx);
		mpSM->mpvFiredEvents = pvFiredEvents;
		mpSM->mpSELMLEl = apSELMLEl;
		mpSM->mbCheckingEvents = true;
	}
	~mlCheckEventsMode(){
		mpSM->RestoreEventJSVals();
		mpSM->mbCheckingEvents = false;
		mpSM->mpvFiredEvents = NULL;
		mpSM->mpSELMLEl = NULL;
	}
};

bool mlRMMLSequencer::CheckWaitJSExpr(){
	if(mpWaitJSExpr==NULL) return true;
	mlCheckEventsMode oCheckEventMode(&mvFiredEvents, this);
	wchar_t* pwcWaitExpr=wr_JS_GetStringChars(mpWaitJSExpr->mjssExpr);
	size_t iLen=JS_GetStringLength(mpWaitJSExpr->mjssExpr);
	jsval v;
	JSBool bR=wr_JS_EvaluateUCScript(mcx, mpParent->mjso, pwcWaitExpr, iLen, mpWaitJSExpr->msSrcFileName.c_str(), mpWaitJSExpr->miSrcLine, &v);
	if(!bR){
		_playing=false;
		mlTraceError(mcx, "Sequencer \'%S\' cannot evaluate \'%S\'\n",GetName(),pwcWaitExpr);
		return false;
	}
	if(v==JSVAL_VOID || v==JSVAL_NULL) return false;
	if(JSVAL_IS_BOOLEAN(v)){
		if(v==JSVAL_FALSE) return false;
	}else if(JSVAL_IS_INT(v)){
		int iVal=JSVAL_TO_INT(v);
		if(iVal==0) return false;
	}else if(JSVAL_IS_STRING(v)){
		JSString* jss=JSVAL_TO_STRING(v);
		if(JS_GetStringLength(jss)==0) return false;
	}else if(JSVAL_IS_DOUBLE(v)){
		jsdouble dVal=(*JSVAL_TO_DOUBLE(v));
		if(dVal==0.0) return false;
	}
	FreeWaitJSExpr();
	return true; // may execute next script
}

class mlGetWaitingEventsMode
{
	mlSceneManager* mpSM;
public:
	mlGetWaitingEventsMode(mlRMMLElement* apSELMLEl){
		mpSM = GPSM(apSELMLEl->mcx);
		mpSM->mpvFiredEvents = NULL;
		mpSM->mpSELMLEl = apSELMLEl;
		mpSM->mbCheckingEvents = true;
	}
	~mlGetWaitingEventsMode(){
		mpSM->RestoreEventJSVals();
		mpSM->mbCheckingEvents = false;
		mpSM->mpSELMLEl = NULL;
	}
};

void mlRMMLSequencer::GetWaitingEvents(){
	if(mpWaitJSExpr==NULL) return;
	mvWaitingEvents.clear();
	mlString sExpr(wr_JS_GetStringChars(mpWaitJSExpr->mjssExpr));
	// вектор нужен только если есть операция AND (похоже неправда 26.01.06)
	mlString::size_type andpos = mlString::npos;
	// if((andpos = sExpr.find(L"&&")) == mlString::npos) return; // commented by Tandy 26.01.06
	while((andpos = sExpr.find(L"&&",andpos)) != mlString::npos){
		sExpr.replace(andpos, 2, L"||");
	}
	mlGetWaitingEventsMode oCWEM(this);
	jsval v;
	wr_JS_EvaluateUCScript(mcx, mpParent->mjso, sExpr.c_str(), sExpr.length(), 
		mpWaitJSExpr->msSrcFileName.c_str(), mpWaitJSExpr->miSrcLine, &v);
}

void mlRMMLSequencer::Reset(){
	FreeWaitJSExpr();
	if(mpEventSender!=NULL){
		mpEventSender->pSender->RemoveEventListener(mpEventSender->idEvent,this);
		MP_DELETE( mpEventSender); mpEventSender=NULL;
	}
	mlTimeOfWaitingEnd=-1;
}

//class nlKeepCurrentFrameAfterUpdate{
//	mlRMMLSequencer* mpSeq;
//public:
//	nlKeepCurrentFrameAfterUpdate(mlRMMLSequencer* apSeq):mpSeq(apSeq){}
//	~nlKeepCurrentFrameAfterUpdate(){
//		mpSeq->miCurrentFrameAfterUpdate=mpSeq->currentFrame;
//	}
//};

// изменился режим работы сцены, изменилось глобальное время
void mlRMMLSequencer::GlobalTimeShifted(const __int64 alTime, const __int64 alTimeShift, mlEModes aePrevMode){
	if(mlTimeOfWaitingEnd > 0){
		mlTimeOfWaitingEnd += alTimeShift;
	}
}

mlresult mlRMMLSequencer::Update(const __int64 alTime){
	if(GPSM(mcx)->GetMode() == smmAuthoring) return ML_OK;	
	if(this->GetFrozen())
		return ML_OK;
	if(mpChildren==NULL) return ML_ERROR_NOT_IMPLEMENTED;
//	if(currentFrame != miCurrentFrameAfterUpdate){
//		Reset();
//	}
	if(!_playing){
		if(mpWaitJSExpr!=NULL)
			FreeWaitJSExpr();
		return ML_ERROR_NOT_IMPLEMENTED; // исключаем из списка обновляемых объектов
	}
//mlTrace("mlRMMLSequencer(%s)::Update(%I64d)\n",cLPCSTR(GetName()),alTime);
	////// 	currentFrame = текущий скрипт
	// если ждем 
	if(mpWaitJSExpr!=NULL){
		if(!CheckWaitJSExpr()){
			// еще не дождались выполнения условия
			return ML_OK;
		}else{
			// будем выполнять следующий скрипт
			Reset();
			currentFrame++;
		}
	}
	if(mpEventSender!=NULL){
		// еще не дождались события
		return ML_OK;
	}
	if(mlTimeOfWaitingEnd > 0){
		// и еще не дождались, то выходим
//if((mlTimeOfWaitingEnd - alTime )/1000 > 10000)
//int hh=0;
//TRACE("!!!!!!!!!!!!waiting %s: %d\n", cLPCSTR(mpName), (mlTimeOfWaitingEnd - alTime )/1000);
		if(mlTimeOfWaitingEnd > alTime) 
			return ML_OK;
		// а если дождались, то выполняем следующий скрипт
		currentFrame++;
	}
if(_name==NULL && currentFrame==1)
int hh=0;
//	nlKeepCurrentFrameAfterUpdate kcau(this);
	// выполняем скрипты 
	while(1){
		// если все скрипты выполнили
		if(currentFrame >= 0 && currentFrame >= (int)mpChildren->size()){
			Reset();
			// , то говорим, что все
			currentFrame=0;
			GetIContinuous()->onPlayed();
			//if(!_loop) _playing=false;
			
			// и вываливаемся
			return ML_OK;
		}
		if(currentFrame < 0) currentFrame=0;
		// выполняем текущий скрипт
		mlRMMLScript* pScript=NULL;
		int iFrmNum=0;
		for(int ii=0; ; ii++){
			if(ii > (int)(mpChildren->size())){
				pScript=NULL;
				break;
			}
			pScript=(mlRMMLScript*)(*mpChildren)[ii];
//			if(pScript->mType!=MLMT_SCRIPT) continue;
			if(pScript->mRMMLType!=MLMT_SCRIPT) continue;
			if(iFrmNum==currentFrame) break;
			iFrmNum++;
		}
		int iCurrentFrameBefore=currentFrame;
		ML_ASSERTION(mcx, pScript!=NULL,"mlRMMLSequencer::Update");
		// если sequencer в области синхронизации, 
		bool bDontSendSynchEventsWas = GPSM(mcx)->mbDontSendSynchEvents;
		if(NeedToSynchronize()){
			// то не синхронизировать подвызовы функций
			GPSM(mcx)->mbDontSendSynchEvents = true;
		}
		jsval jsvRet=JSVAL_NULL;
		if(ML_FAILED(pScript->Execute(&jsvRet))){
			// при выполнении скрипта возникла ошибка 
			// (JS engine должен был вывести соответствующее сообщение)
			GPSM(mcx)->mbDontSendSynchEvents = bDontSendSynchEventsWas;
			_playing=false;
			return ML_OK;
		}
		GPSM(mcx)->mbDontSendSynchEvents = bDontSendSynchEventsWas;
		if(currentFrame != iCurrentFrameBefore){
			Reset();
			return ML_OK;
		}
		// если выполнили последний скрипт и _loop==false, то...
		// if((!_loop) && (currentFrame+1) == (int)mpChildren->size()){ currentFrame++; continue; }
		mlTimeOfWaitingEnd=-1;
		if(JSVAL_IS_NUMBER(jsvRet)){
			int iWait=0;
			if(JSVAL_IS_INT(jsvRet)){
				iWait=JSVAL_TO_INT(jsvRet);
			}else if(JSVAL_IS_DOUBLE(jsvRet)){
				jsdouble* pjsdbl=JSVAL_TO_DOUBLE(jsvRet);
				iWait=(int)(*pjsdbl);
			}
			mlTimeOfWaitingEnd = alTime + ((__int64)iWait)*1000;
			GPSM(mcx)->UpdateNextUpdateDeltaTime(iWait+1);
//TRACE("seq %s wait: %d ms\n", cLPCSTR(mpName), iWait);
			return ML_OK;
		}else if(JSVAL_IS_STRING(jsvRet)){
			// разбираем условие окончания ожидания
			JSString* jssWaitExpr=JSVAL_TO_STRING(jsvRet);
			if(IsElementRef(jssWaitExpr)){
				// наверное это событие
				wchar_t* pwcWaitExpr=wr_JS_GetStringChars(jssWaitExpr);
				if(SetMeAsEventListener(pwcWaitExpr)!=ML_OK){
					_playing=false;
					mlTraceError(mcx, "Sequencer \'%S\' cannot find event \'%S\'\n",GetName(),pwcWaitExpr);
					return ML_ERROR_FAILURE;
				}
				return ML_OK;
			}else{
				// выражение, которое надо ждать, пока оно не станет =true || !="" // !=0
				mpWaitJSExpr = MP_NEW( WaitJSExpr);
				mpWaitJSExpr->mjssExpr = jssWaitExpr;
				SAVE_STR_FROM_GC(mcx,mjso,mpWaitJSExpr->mjssExpr)
				const wchar_t* pwcSrcFileName = GPSM(mcx)->mSrcFileColl[miSrcFilesIdx];
				mlString sSrcFileName;
				if(pwcSrcFileName != NULL)
					sSrcFileName = pwcSrcFileName;				
				mpWaitJSExpr->msSrcFileName = cLPCSTR(sSrcFileName.c_str());
				mpWaitJSExpr->miSrcLine = pScript->miSrcLine;
				GetWaitingEvents();
				mvFiredEvents.clear();
				if(!CheckWaitJSExpr()){
					return ML_OK;
				}else{
					FreeWaitJSExpr();
				}
			}
		}else if(JSVAL_IS_BOOLEAN(jsvRet)){
			// если false, то паузим
			JSBool bRV=JSVAL_TO_BOOLEAN(jsvRet);
			if(!bRV){
				_playing=false;
				return ML_OK;
			}
		}
		// будем выполнять следующий скрипт
		currentFrame++;
		if(!_playing)
			return ML_OK;
	}
	return ML_OK;
}

bool mlRMMLSequencer::EventNotify(char aidEvent, mlRMMLElement* apSender){
//	ML_ASSERTION(mpEventSender!=NULL,"mlRMMLSequencer::EventNotify");
	if(mpEventSender!=NULL){
		ML_ASSERTION(mcx, mpEventSender->idEvent==aidEvent,"mlRMMLSequencer::EventNotify");
		ML_ASSERTION(mcx, mpEventSender->pSender==apSender,"mlRMMLSequencer::EventNotify");
		mpEventSender->pSender->RemoveEventListener(mpEventSender->idEvent,this); // его сразу EventNotify() после не удалят
		MP_DELETE( mpEventSender); mpEventSender=NULL;
		currentFrame++;
//		miCurrentFrameAfterUpdate=currentFrame;
//		jsval vTime;
//		JS_GetProperty(mcx, JS_GetGlobalObject(mcx),"_time",&vTime);
//		Update(((__int64)(JSVAL_TO_INT(vTime)))*1000);
		Update(GPSM(mcx)->GetTime());
		return false; // не удалять из списка слушателей
	}
	if(mpWaitJSExpr != NULL){
		mvFiredEvents.push_back(EventSender(aidEvent, apSender));
		Update(GPSM(mcx)->GetTime());
		return true; // удалить из списка слушателей
	}
	return false;
}

void mlRMMLSequencer::EventSenderDestroyed(mlRMMLElement* apSender){
	if(mpEventSender!=NULL && mpEventSender->pSender==apSender){
		// мы его ждали, а он уничтожается :)
		// ??
		MP_DELETE( mpEventSender); mpEventSender=NULL;
	}
}

mlresult mlRMMLSequencer::SetMeAsEventListener(wchar_t* apwcEvent){
	ML_ASSERTION(mcx, mpParent!=NULL,"mlRMMLSequencer::SetMeAsEventListener");
	EventSender* pEventSender=mpParent->GetEventSender(apwcEvent);
	if(pEventSender==NULL) return ML_ERROR_INVALID_ARG;
	pEventSender->pSender->SetEventListener(pEventSender->idEvent,this);
	if(mpEventSender)  MP_DELETE( mpEventSender);
	mpEventSender=pEventSender;
	return ML_OK;
}

void mlRMMLSequencer::YouAreSetAsEventListener(char aidEvent, mlRMMLElement* apSender){
	if(GPSM(mcx)->mpSELMLEl != NULL && GPSM(mcx)->mpvFiredEvents == NULL)
		mvWaitingEvents.push_back(EventSender(aidEvent, apSender));
}

bool mlRMMLSequencer::Freeze(){
	if(!mlRMMLElement::Freeze()) return false; 
	// запоминаем время последнего Update'а
	__int64 lCurTime = GPSM(mcx)->GetTime();
	if(mlTimeOfWaitingEnd > lCurTime){
		mlTimeToWaitAfterUnfreez = mlTimeOfWaitingEnd - lCurTime;
	}else 
		mlTimeToWaitAfterUnfreez = 0L;
	return true;
}

bool mlRMMLSequencer::Unfreeze(){
	if(!mlRMMLElement::Unfreeze()) return false; 
	// продолжаем работу, но с учетем времени последнего Update'а при приостановке
	if(mlTimeToWaitAfterUnfreez > 0)
		mlTimeOfWaitingEnd = GPSM(mcx)->GetTime() + mlTimeToWaitAfterUnfreez;
	return false;
}

void mlRMMLSequencer::PlayingChanged(){
//if(isEqual(GetName(),L"hide_last_screen"))
//currentFrame=0;
	if(!_playing){
		Reset();
	}else{
		GPSM(mcx)->AddUpdatableElement(this, true);
	}
}

void mlRMMLSequencer::FrameChanged(){
	Reset();
}

void mlRMMLSequencer::PosChanged(){
	Reset();
}

int mlRMMLSequencer::GetFrameByName(const wchar_t* apwcName){
	mlRMMLScript* pScript=NULL;
	int iFrmNum=0;
	for(int ii=0; ; ii++){
		if(ii > (int)(mpChildren->size())){
			pScript=NULL;
			break;
		}
		pScript=(mlRMMLScript*)(*mpChildren)[ii];
//			if(pScript->mType!=MLMT_SCRIPT) continue;
		if(pScript->mRMMLType!=MLMT_SCRIPT) continue;
		if(isEqual(pScript->GetScriptName().c_str(), apwcName)){
				return iFrmNum;
		}
		iFrmNum++;
	}
	return -1;
}

}

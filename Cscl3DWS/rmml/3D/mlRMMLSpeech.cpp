#include "mlRMML.h"

namespace rmml {

mlRMMLSpeech::mlRMMLSpeech(void)
{
	mRMMLType=MLMT_SPEECH;
	mType=MLMT_SPEECH;
}

mlRMMLSpeech::~mlRMMLSpeech(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpSndM!=NULL,"mlRMMLSpeech::~mlRMMLSpeech");
	pContext->mpSndM->DestroyMO(this);
	//
	if(mpSEListeners.get()!=NULL){
		v_elems::iterator vi;
		for(vi=mpSEListeners->mvListeners.begin(); vi != mpSEListeners->mvListeners.end(); vi++ ){
			mlRMMLElement* pMLEl=*vi;
			pMLEl->EventSenderDestroyed(this);
		}
		mpSEListeners->mvListeners.clear();
	}
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLSpeech::_JSPropertySpec[] = {
	JSPROP_RW(actions)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLSpeech::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlRMMLSpeech::_EventSpec[] = {
	MLEVENT(onSaid)
	{0, 0, 0}
};

MLJSCLASS_IMPL_BEGIN(Speech,mlRMMLSpeech,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDEVENTS
MLJSCLASS_IMPL_END(mlRMMLSpeech)


///// JavaScript Set Property Wrapper
JSBool mlRMMLSpeech::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLSpeech);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		default:{
			switch(iID){
			case JSPROP_actions:
				// читаем скрипты из xml-ки
				// ??
				break;
			}
		}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLSpeech::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLSpeech);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		default:{
			switch(iID){
			case EVID_onSaid:
				{
				int iRes = GPSM(cx)->CheckEvent(iID, priv, vp);
				if(iRes >= 0)
					*vp = BOOLEAN_TO_JSVAL(iRes);
				}break;
			}
		}
	GET_PROPS_END;
	return JS_TRUE;
}


// Реализация mlRMMLElement
mlresult mlRMMLSpeech::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpSndM!=NULL,"mlRMMLSpeech::CreateMedia");
	amcx->mpSndM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLSpeech::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded.
		return ML_ERROR_FAILURE;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLSpeech::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLSpeech* pNewEL=(mlRMMLSpeech*)GET_RMMLEL(mcx, mlRMMLSpeech::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

//MLEVENT_IMPL(mlRMMLSpeech,onSaid);
void mlRMMLSpeech::onSaid(){
	PerformCurActions();
	if(mpSEListeners.get()!=NULL){
		mpSEListeners->Reset();
	}
	bool bDontSendSynchEvents = (mucSynchFlags4Events & EVSM_onSaid)?true:false;
	// если в моем update-е, 
	if(GPSM(mcx)->Updating()){
		// то выполняем сразу же
		if(mpParent!=NULL && mpParent->mRMMLType==MLMT_CHARACTER)
			((mlRMMLCharacter*)mpParent)->Said(this);
		GPSM(mcx)->mbDontSendSynchEvents = bDontSendSynchEvents;
		CallListeners(EVID_onSaid);
	}else{
		//// иначе ставим в очередь событий
		// сохраняем внуттренний путь до элемента 
		mlSynchData Data;
		GPSM(mcx)->SaveInternalPath(Data,this);
		Data << bDontSendSynchEvents;
		// ставим в очередь событий (в режиме инициализации - в начало)
		GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onSaid, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(mcx)->GetMode() == smmInitilization);
	}
}

__forceinline bool PosLessEqual(JSContext* cx, long alPos1, SynchEventSender::VType aType1, long alPos2, SynchEventSender::VType aType2){
	if(aType1==aType2) return (alPos1 <= alPos2);
	if(aType1==SynchEventSender::ms && aType2==SynchEventSender::sec)
		return (alPos1 <= alPos2*1000);
	if(aType1==SynchEventSender::sec && aType2==SynchEventSender::ms)
		return (alPos1*1000 <= alPos2);
	mlTraceError(cx, "Incompatible position value types in mlRMMLSpeech::PosLess\n");
	return true;
}

void mlRMMLSpeech::ResetActions(){
	if(mpSEListeners.get()==NULL) return;
	mpSEListeners->Reset();
}

void mlRMMLSpeech::PerformCurActions(){
	if(PMO_IS_NULL(mpMO)) return;
	if(mpSEListeners.get()==NULL) return;
	moISpeech* pISpeech=mpMO->GetISpeech();
	if(pISpeech==NULL) return;
	// если есть скрипты на pos
	long lCurPos=pISpeech->GetCurrentPos();
	if(lCurPos < 0) return; // звук не проигрывается или возникла какая-нибудь ошибка
if(lCurPos == 0)
int hh=0;
//mlTrace("%d\n",(int)ulCurPos);
//	mpSEListeners->mvi = mpSEListeners->mvListeners.begin();
	for(; mpSEListeners->mvi != mpSEListeners->mvListeners.end(); mpSEListeners->mvi++ ){
		mlRMMLElement* pMLEl=*(mpSEListeners->mvi);
		SynchEventSender* pSESS=pMLEl->GetSynchEventSenderStruct();
		ML_ASSERTION(mcx, pSESS!=NULL,"mlRMMLSpeech::PerformCurActions");
		if(PosLessEqual(mcx, pSESS->lValue,pSESS->eValueType,lCurPos,SynchEventSender::ms)){
			mlRMMLScript* pScript=(mlRMMLScript*)pMLEl;
			pScript->SynchEventNotify();
		}else
			break;
	}
	mpSEListeners->muLastPos = lCurPos;
}

void mlRMMLSpeech::SetSynchEventListener(mlRMMLElement* apMLElListener, SynchEventSender* apSEEventSender){
	ML_ASSERTION(mcx, apSEEventSender!=NULL,"mlRMMLSpeech::SetSynchEventListener");
	if(apMLElListener->mRMMLType!=MLMT_SCRIPT){
		mlTraceError(mcx, "Speech synchronizing event listener must be a script");
		return;
	}
	if(mpSEListeners.get()==NULL){ mpSEListeners = std::auto_ptr<SynchEvListeners>(new SynchEvListeners); }
	v_elems::iterator vi;
	for(vi=mpSEListeners->mvListeners.begin(); vi != mpSEListeners->mvListeners.end(); vi++ ){
		mlRMMLElement* pMLEl=*vi;
		SynchEventSender* pSESS=pMLEl->GetSynchEventSenderStruct();
		ML_ASSERTION(mcx, pSESS!=NULL,"mlRMMLSpeech::SetSynchEventListener");
		if(PosLessEqual(mcx, apSEEventSender->lValue,apSEEventSender->eValueType,pSESS->lValue,pSESS->eValueType)) break;
	}
	mpSEListeners->mvListeners.insert(vi,apMLElListener);
	mpSEListeners->Reset();
}

void mlRMMLSpeech::RemoveSynchEventListener(mlRMMLElement* apMLElListener){
	if(mpSEListeners.get()==NULL){ return; }
	v_elems::iterator vi;
	for(vi=mpSEListeners->mvListeners.begin(); vi != mpSEListeners->mvListeners.end(); vi++ ){
		if(*vi==apMLElListener){
			mpSEListeners->mvListeners.erase(vi);
			break;
		}
	}
}

}

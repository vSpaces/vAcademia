#include "mlRMML.h"

namespace rmml {

mlRMMLMotion::mlRMMLMotion(void)
{
	mRMMLType=MLMT_MOTION;
	mType=MLMT_MOTION;
	inverse=false;
	duration=0;
	primary=false;
	tracks = NULL;
}

mlRMMLMotion::~mlRMMLMotion(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLMotion::~mlRMMLMotion");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLMotion::_JSPropertySpec[] = {
	JSPROP_RW(inverse)
	JSPROP_RW(duration)
	JSPROP_RW(primary)
	JSPROP_RO(tracks)

	JSPROP_EV(onSet)
	JSPROP_EV(onDone)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLMotion::_JSFunctionSpec[] = {
	JSFUNC(moveTo, 1)
	JSFUNC(rotateTo, 1)
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlRMMLMotion::_EventSpec[] = {
	MLEVENT(onSet)
	MLEVENT(onDone)
	{0, 0, 0}
};

MLJSCLASS_IMPL_BEGIN(Motion,mlRMMLMotion,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDEVENTS
MLJSCLASS_IMPL_END(mlRMMLMotion)


///// JavaScript Set Property Wrapper
JSBool mlRMMLMotion::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLMotion);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		default:
			switch(iID){
				case JSPROP_inverse:
					ML_JSVAL_TO_BOOL(priv->inverse,*vp);
					priv->InverseChanged();
					break;
				case JSPROP_duration:
					ML_JSVAL_TO_INT(priv->duration,*vp);
					priv->DurationChanged();
					break;
				case JSPROP_primary:
					ML_JSVAL_TO_BOOL(priv->primary,*vp);
					priv->PrimaryChanged();
					break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLMotion::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLMotion);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		default:
			switch(iID){
				case JSPROP_inverse:
					*vp=BOOLEAN_TO_JSVAL(priv->inverse);
					break;
				case JSPROP_duration:
					*vp=INT_TO_JSVAL(priv->duration);
					break;
				case JSPROP_primary:
					*vp=BOOLEAN_TO_JSVAL(priv->primary);
					break;
				case JSPROP_tracks:
					if(priv->tracks == NULL){
						JSObject* jsoTracks = mlMotionTracks::newJSObject(cx);
						priv->tracks = (mlMotionTracks*)JS_GetPrivate(cx, jsoTracks);
						SAVE_FROM_GC(cx, obj, jsoTracks)
						priv->tracks->mpMotion = priv;
					}
					*vp=OBJECT_TO_JSVAL(priv->tracks->mjso);
					break;
				case EVID_onSet:
				case EVID_onDone:
					{
					int iRes = GPSM(cx)->CheckEvent(iID, priv, vp);
					if(iRes >= 0)
						*vp = BOOLEAN_TO_JSVAL(iRes);
					}break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}


// Реализация mlRMMLElement
mlresult mlRMMLMotion::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLMotion::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLMotion::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded.
		return ML_ERROR_FAILURE;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLMotion::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLMotion* pNewEL=(mlRMMLMotion*)GET_RMMLEL(mcx, mlRMMLMotion::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

JSBool mlRMMLMotion::JSFUNC_moveTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc <= 0){
		JS_ReportError(cx,"Method 'moveTo' must get one argument at least");
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMMLMotion* pMotion = (mlRMMLMotion*)pMLEl;
	jsval vDestPos = argv[0];
	if(!JSVAL_IS_REAL_OBJECT(vDestPos)){
		JS_ReportError(cx,"First parameter of moveTo(..) must be a 3D-position object");
		return JS_TRUE;
	}
	JSObject* jsoDestPos = JSVAL_TO_OBJECT(vDestPos);
	if(!mlPosition3D::IsInstance(cx, jsoDestPos)){
		JS_ReportError(cx,"First parameter of moveTo(..) must be a 3D-position object");
		return JS_TRUE;
	}
	mlPosition3D* pDestPos = (mlPosition3D*)JS_GetPrivate(cx, jsoDestPos);
	pMotion->moveTo(pDestPos->GetPos());
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

JSBool mlRMMLMotion::JSFUNC_rotateTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc <= 0){
		JS_ReportError(cx,"Method 'rotateTo' must get one argument at least");
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}
	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, obj);
	mlRMMLMotion* pMotion = (mlRMMLMotion*)pMLEl;
	jsval vDestRot = argv[0];
	if(!JSVAL_IS_REAL_OBJECT(vDestRot)){
		JS_ReportError(cx,"First parameter of rotateTo(..) must be a 3D-rotation object");
		return JS_TRUE;
	}
	JSObject* jsoDestRot = JSVAL_TO_OBJECT(vDestRot);
	if(!mlRotation3D::IsInstance(cx, jsoDestRot)){
		JS_ReportError(cx,"First parameter of rotateTo(..) must be a 3D-rotation object");
		return JS_TRUE;
	}
	mlRotation3D* pDestRot = (mlRotation3D*)JS_GetPrivate(cx, jsoDestRot);
	pMotion->rotateTo(pDestRot->GetRot());
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

#define MOIMO_METH_BODY_IMPL \
	if(PMO_IS_NULL(mpMO)) return false; \
	return mpMO->GetIMotion()

bool mlRMMLMotion::moveTo(ml3DPosition& aPos3D){
	MOIMO_METH_BODY_IMPL->moveTo(aPos3D);
}

bool mlRMMLMotion::rotateTo(ml3DRotation& aRot3D){
	MOIMO_METH_BODY_IMPL->rotateTo(aRot3D);
}

void mlRMMLMotion::SetDuration(unsigned int aiDuration)
{
	ML_ASSERTION( mcx, aiDuration > 0, "mlRMMLMotion::SetDuration");
	if( aiDuration > 0)
		duration = aiDuration;
}

//MLEVENT_IMPL(mlRMMLMotion,onSet);
void mlRMMLMotion::onSet(){
/*#ifdef MLD_TRACE_INIT_EVENTS
if(GPSM(mcx)->GetMode()==smmInitilization){
mlTrace(mcx, "---SynchInitInfo--- %s.onSet", 
	cLPCSTR(GetStringID().c_str())
);
}
#endif*/
	bool bDontSendSynchEvents = (mucSynchFlags4Events & EVSM_onSet)?true:false;
	// если в моем update-е, 
/*	if(GPSM(mcx)->Updating()){
		// то выполняем сразу же
		GPSM(mcx)->mbDontSendSynchEvents = bDontSendSynchEvents;
#ifdef MLD_TRACE_INIT_EVENTS
if(GPSM(mcx)->GetMode()==smmInitilization){
TRACE(mcx, " handled\n");
}
#endif
		CallListeners(EVID_onSet);
	}else{*/
		//// иначе ставим в очередь событий
		// сохраняем внуттренний путь до элемента 
		mlSynchData Data;
		GPSM(mcx)->SaveInternalPath(Data,this);
		Data << bDontSendSynchEvents;
		// ставим в очередь событий
		GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onSet, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(mcx)->GetMode() == smmInitilization);
/*#ifdef MLD_TRACE_INIT_EVENTS
if(GPSM(mcx)->GetMode()==smmInitilization){
TRACE(mcx, " queued\n");
}
#endif
	}*/
}
//MLEVENT_IMPL(mlRMMLMotion,onDone);
void mlRMMLMotion::onDone(){
/*#ifdef MLD_TRACE_INIT_EVENTS
if(GPSM(mcx)->GetMode()==smmInitilization){
mlTrace(mcx, "---SynchInitInfo--- %s.onDone", 
	cLPCSTR(GetStringID().c_str())
);
}
#endif*/
	bool bDontSendSynchEvents = (mucSynchFlags4Events & EVSM_onDone)?true:false;
	/*// если в моем update-е, 
	if(GPSM(mcx)->Updating()){
		// то выполняем сразу же
		GPSM(mcx)->mbDontSendSynchEvents = bDontSendSynchEvents;
#ifdef MLD_TRACE_INIT_EVENTS
if(GPSM(mcx)->GetMode()==smmInitilization){
TRACE(mcx, " handled\n");
}
#endif
		CallListeners(EVID_onDone);
	}else{*/
		//// иначе ставим в очередь событий
		// сохраняем внуттренний путь до элемента 
		mlSynchData Data;
		GPSM(mcx)->SaveInternalPath(Data,this);
		Data << bDontSendSynchEvents;
		// ставим в очередь событий
		GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(EVID_onDone, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(mcx)->GetMode() == smmInitilization);
/*#ifdef MLD_TRACE_INIT_EVENTS
if(GPSM(mcx)->GetMode()==smmInitilization){
TRACE(mcx, " queued\n");
}
#endif
	}*/
}

// реализация  moIMotion
#define MLIMOT_PROP_CH(N) \
void mlRMMLMotion::N(){ \
	if(PMO_IS_NULL(mpMO)) return; \
	mpMO->GetIMotion()->N(); \
}

MLIMOT_PROP_CH(DurationChanged)
MLIMOT_PROP_CH(InverseChanged)
MLIMOT_PROP_CH(PrimaryChanged)

ml3DPosition mlRMMLMotion::GetFramePos(const wchar_t* apwcTrack, unsigned int auFrame){
	if(PMO_IS_NULL(mpMO)){ ml3DPosition pos; ML_INIT_3DPOSITION(pos); return pos; }
	return mpMO->GetIMotion()->GetFramePos(apwcTrack, auFrame);
}
void mlRMMLMotion::SetFramePos(const wchar_t* apwcTrack, unsigned int auFrame, ml3DPosition &pos){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIMotion()->SetFramePos(apwcTrack, auFrame, pos);
}
ml3DScale mlRMMLMotion::GetFrameScale(const wchar_t* apwcTrack, unsigned int auFrame){
	if(PMO_IS_NULL(mpMO)){ ml3DScale scl; ML_INIT_3DSCALE(scl); return scl; }
	return mpMO->GetIMotion()->GetFrameScale(apwcTrack, auFrame);
}
void mlRMMLMotion::SetFrameScale(const wchar_t* apwcTrack, unsigned int auFrame, ml3DScale &scl){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIMotion()->SetFrameScale(apwcTrack, auFrame, scl);
}
ml3DRotation mlRMMLMotion::GetFrameRotation(const wchar_t* apwcTrack, unsigned int auFrame){
	if(PMO_IS_NULL(mpMO)){ ml3DRotation rot; ML_INIT_3DROTATION(rot); return rot; }
	return mpMO->GetIMotion()->GetFrameRotation(apwcTrack, auFrame);
}
void mlRMMLMotion::SetFrameRotation(const wchar_t* apwcTrack, unsigned int auFrame, ml3DRotation &rot){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIMotion()->SetFrameRotation(apwcTrack, auFrame, rot);
}

ml3DPosition mlRMMLMotion::GetFramePos(int aiTrack, unsigned int auFrame){
	if(PMO_IS_NULL(mpMO)){ ml3DPosition pos; ML_INIT_3DPOSITION(pos); return pos; }
	return mpMO->GetIMotion()->GetFramePos(aiTrack, auFrame);
}
void mlRMMLMotion::SetFramePos(int aiTrack, unsigned int auFrame, ml3DPosition &pos){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIMotion()->SetFramePos(aiTrack, auFrame, pos);
}
ml3DScale mlRMMLMotion::GetFrameScale(int aiTrack, unsigned int auFrame){
	if(PMO_IS_NULL(mpMO)){ ml3DScale scl; ML_INIT_3DSCALE(scl); return scl; }
	return mpMO->GetIMotion()->GetFrameScale(aiTrack, auFrame);
}
void mlRMMLMotion::SetFrameScale(int aiTrack, unsigned int auFrame, ml3DScale &scl){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIMotion()->SetFrameScale(aiTrack, auFrame, scl);
}
ml3DRotation mlRMMLMotion::GetFrameRotation(int aiTrack, unsigned int auFrame){
	if(PMO_IS_NULL(mpMO)){ ml3DRotation rot; ML_INIT_3DROTATION(rot); return rot; }
	return mpMO->GetIMotion()->GetFrameRotation(aiTrack, auFrame);
}
void mlRMMLMotion::SetFrameRotation(int aiTrack, unsigned int auFrame, ml3DRotation &rot){
	if(PMO_IS_NULL(mpMO)) return;
	mpMO->GetIMotion()->SetFrameRotation(aiTrack, auFrame, rot);
}

}

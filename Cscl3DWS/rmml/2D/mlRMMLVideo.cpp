#include "mlRMML.h"
#include "config/oms_config.h"
#include <sstream>
#include "config/prolog.h"

namespace rmml {

mlRMMLVideo::mlRMMLVideo(void)
{
	mType = MLMT_VIDEO;
	mRMMLType = MLMT_VIDEO;
	windowed = true;
}

mlRMMLVideo::~mlRMMLVideo(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLVideo::~mlRMMLVideo");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLVideo::_JSPropertySpec[] = {
	JSPROP_RW(windowed)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLVideo::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Video,mlRMMLVideo,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMMLVisible)
	MLJSCLASS_ADDPROTO(mlRMMLContinuous)
	MLJSCLASS_ADDPROTO(mlRMMLPButton)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLVideo)

///// JavaScript Set Property Wrapper
JSBool mlRMMLVideo::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLVideo);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		SET_PROTO_PROP(mlRMMLVisible);
		SET_PROTO_PROP(mlRMMLContinuous);
		SET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID){
				case JSPROP_windowed:{
					bool bWasWindowed=priv->windowed;
					ML_JSVAL_TO_BOOL(priv->windowed,*vp);
					if(bWasWindowed!=priv->windowed)
						priv->WindowedChanged();
				}break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLVideo::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLVideo);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		GET_PROTO_PROP(mlRMMLVisible);
		GET_PROTO_PROP(mlRMMLContinuous);
		GET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID){
				case JSPROP_windowed:{
					*vp=BOOLEAN_TO_JSVAL(priv->windowed);
				}break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

// реализация mlRMMLElement
mlresult mlRMMLVideo::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLVideo::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
//	bool	bplaying = _playing;
	mlRMMLContinuous::FrameChangedDontStop(); // currentFrame attribute added 21.02.06 (patched 06.05.06)
	mlRMMLContinuous::PosChangedDontStop(); // currentPos attribute added 21.02.06 (patched 06.05.06)
//	_playing = bplaying;
	return ML_OK;
}

mlresult mlRMMLVideo::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;

	int iOldType=mType;
	mType=MLMT_VIDEO;

	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. Restore
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLVideo::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLVideo* pNewEL=(mlRMMLVideo*)GET_RMMLEL(mcx, mlRMMLVideo::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

bool mlRMMLVideo::Freeze(){
	if(!mlRMMLElement::Freeze()) return false; 
	// ??
	return true;
}

bool mlRMMLVideo::Unfreeze(){
	if(!mlRMMLElement::Unfreeze()) return false; 
	// ??
	return false;
}

mlresult mlRMMLVideo::Update(const __int64 alTime){
#ifdef CSCL
return ML_ERROR_NOT_IMPLEMENTED;
	if(!_playing) return ML_OK;
		// ??
#else
	if(_playing && !PMO_IS_NULL(mpMO))
		mpMO->Update(alTime);
#endif
	return ML_OK;
}

// реализация moIText
#define MOIVIDEOEVENT_IMPL(M) \
void mlRMMLVideo::M(){ \
	if(PMO_IS_NULL(mpMO)) return; \
	mpMO->GetIVideo()->M(); \
}
MOIVIDEOEVENT_IMPL(WindowedChanged);

}

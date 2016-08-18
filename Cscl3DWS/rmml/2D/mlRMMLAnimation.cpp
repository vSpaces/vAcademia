// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $RCSfile: mlRMMLAnimation.cpp,v $
// $Revision: 1.12 $
// $State: Exp $
// $Locker:  $
//
// last change: $Date: 2009/05/12 12:07:44 $
//              $Author: tandy $ 
//
//////////////////////////////////////////////////////////////////////
#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml {

mlRMMLAnimation::mlRMMLAnimation(void)
{
	mType=MLMT_ANIMATION;
	mRMMLType=MLMT_ANIMATION;
}

mlRMMLAnimation::~mlRMMLAnimation(void){
	omsContext* pContext=GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLAnimation::~mlRMMLAnimation");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLAnimation::_JSPropertySpec[] = {
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLAnimation::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Animation,mlRMMLAnimation,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMMLVisible)
	MLJSCLASS_ADDPROTO(mlRMMLContinuous)
	MLJSCLASS_ADDPROTO(mlRMMLPButton)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLAnimation)

///// JavaScript Set Property Wrapper
JSBool mlRMMLAnimation::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLAnimation);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		SET_PROTO_PROP(mlRMMLVisible);
		SET_PROTO_PROP(mlRMMLContinuous);
		SET_PROTO_PROP(mlRMMLPButton);
		default:;
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLAnimation::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLAnimation);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		GET_PROTO_PROP(mlRMMLVisible);
		GET_PROTO_PROP(mlRMMLContinuous);
		GET_PROTO_PROP(mlRMMLPButton);
		default:;
	GET_PROPS_END;
	return JS_TRUE;
}

// реализация mlRMMLElement
mlresult mlRMMLAnimation::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLAnimation::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	mlRMMLContinuous::FrameChangedDontStop(); // currentFrame attribute added 21.02.06 (patched 06.05.06)
	mlRMMLContinuous::PosChangedDontStop(); // currentPos attribute added 21.02.06 (patched 06.05.06)
	return ML_OK;
}

mlresult mlRMMLAnimation::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	int iOldType=mType;
	mType=MLMT_ANIMATION;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. Restore
		mType=iOldType;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLAnimation::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLAnimation* pNewEL=(mlRMMLAnimation*)GET_RMMLEL(mcx, mlRMMLAnimation::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

bool mlRMMLAnimation::Freeze(){
	if(!mlRMMLElement::Freeze()) return false; 
	// ??
	return true;
}

bool mlRMMLAnimation::Unfreeze(){
	if(!mlRMMLElement::Unfreeze()) return false; 
	// ??
	return false;
}

mlresult mlRMMLAnimation::Update(const __int64 alTime){
return ML_ERROR_NOT_IMPLEMENTED;
	if(!_playing) return ML_OK;
	// ??
	return ML_OK;
}

}

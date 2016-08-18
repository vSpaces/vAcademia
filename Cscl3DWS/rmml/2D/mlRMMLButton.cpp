// Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2
//
// $Id: mlRMMLButton.cpp,v 1.5 2009/05/06 13:24:36 tandy Exp $ 
// $Locker:  $
//
// last change: $Date: 2009/05/06 13:24:36 $ $Author: tandy $ 
//
//////////////////////////////////////////////////////////////////////
#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml {

mlRMMLButton::mlRMMLButton(void)
{
	mType=MLMT_BUTTON;
	mRMMLType=MLMT_BUTTON;
}

mlRMMLButton::~mlRMMLButton(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLButton::~mlRMMLButton");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

/*
JSClass mlRMMLButton::_jsClass={
	"Image", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub,
	mlRMMLButton::JSGetProperty, mlRMMLButton::JSSetProperty,
	JS_EnumerateStub, JS_ResolveStub,
	JS_ConvertStub, mlRMMLButton::JSDestructor,
	0, 0, 0, 0, 
	0, 0, 0, 0
};
*/

///// JavaScript Variable Table
JSPropertySpec mlRMMLButton::_JSPropertySpec[] = {
//	{ "x", JSVAR_x, JSPROP_ENUMERATE, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLButton::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Button,mlRMMLButton,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMMLVisible)
	MLJSCLASS_ADDPROTO(mlRMMLPButton)
MLJSCLASS_IMPL_END(mlRMMLButton)

///// JavaScript Set Property Wrapper
JSBool mlRMMLButton::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLButton);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		SET_PROTO_PROP(mlRMMLVisible);
		SET_PROTO_PROP(mlRMMLPButton);
		default:;
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLButton::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLButton);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		GET_PROTO_PROP(mlRMMLVisible);
		GET_PROTO_PROP(mlRMMLPButton);
		default:;
	GET_PROPS_END;
	return JS_TRUE;
}

// реализация mlRMMLElement
mlresult mlRMMLButton::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLButton::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLButton::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	int iOldType=mType;
	mType=MLMT_IMAGE;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. Restore
		mType=iOldType;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLButton::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLButton* pNewEL=(mlRMMLButton*)GET_RMMLEL(mcx, mlRMMLButton::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

}

#include "mlRMML.h"

namespace rmml {

mlRMMLIdles::mlRMMLIdles(void)
{
	mRMMLType=mType=MLMT_IDLES;
}

mlRMMLIdles::~mlRMMLIdles(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLIdles::~mlRMMLIdles");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLIdles::_JSPropertySpec[] = {
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLIdles::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Idles,mlRMMLIdles,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
MLJSCLASS_IMPL_END(mlRMMLIdles)


///// JavaScript Set Property Wrapper
JSBool mlRMMLIdles::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLIdles);
		SET_PROTO_PROP(mlRMMLElement);
//		default:
//			switch(iID){
//			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLIdles::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLIdles);
		GET_PROTO_PROP(mlRMMLElement);
//		default:
//			switch(iID){
//			}
	GET_PROPS_END;
	return JS_TRUE;
}


// Реализация mlRMMLElement
mlresult mlRMMLIdles::CreateMedia(omsContext* amcx){
//	ML_ASSERTION(amcx->mpRM!=NULL,"mlRMMLIdles::CreateMedia");
//	amcx->mpRM->CreateMO(this);
//	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
//	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlRMMLElement* mlRMMLIdles::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLIdles* pNewEL=(mlRMMLIdles*)GET_RMMLEL(mcx, mlRMMLIdles::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

// Свойства
bool mlRMMLIdles::SrcChanged(){
	// вызвать у родителя-character-а IdlesSrcChanged
	if(mpParent == NULL || mpParent->mRMMLType != MLMT_CHARACTER)
		return false;
	mlRMMLCharacter* pCharacter = (mlRMMLCharacter*)mpParent;
	pCharacter->IdlesSrcChanged(GetName(), GetSrc());
	return true;
}

}

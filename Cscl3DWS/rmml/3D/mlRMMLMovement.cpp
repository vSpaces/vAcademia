#include "mlRMML.h"

namespace rmml {

mlRMMLMovement::mlRMMLMovement(void)
{
	mRMMLType=mType=MLMT_MOVEMENT;
	step_length=0;
	rot_speed=0;
}

mlRMMLMovement::~mlRMMLMovement(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLMovement::~mlRMMLMovement");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLMovement::_JSPropertySpec[] = {
	JSPROP_RW(stepLength)
	JSPROP_RW(rotSpeed)
//	{ "_camera", JSPROP__camera, JSPROP_ENUMERATE, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLMovement::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Movement,mlRMMLMovement,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLMovement)


///// JavaScript Set Property Wrapper
JSBool mlRMMLMovement::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLMovement);
		SET_PROTO_PROP(mlRMMLElement);
		default:
			switch(iID){
				case JSPROP_stepLength:{
					jsdouble jsdbl;
					if(JS_ValueToNumber(cx, *vp, &jsdbl)==JS_TRUE){ 
						priv->step_length=jsdbl;
					}else{
						return JS_FALSE;
					} 
					}break;
				case JSPROP_rotSpeed:{
					jsdouble jsdbl;
					if(JS_ValueToNumber(cx, *vp, &jsdbl)==JS_TRUE){ 
						priv->rot_speed=jsdbl;
					}else{
						return JS_FALSE;
					} 
					}break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLMovement::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLMovement);
		GET_PROTO_PROP(mlRMMLElement);
		default:
			switch(iID){
				case JSPROP_stepLength:{
					jsdouble* jsdbl=JS_NewDouble(cx, priv->step_length); 
					*vp=DOUBLE_TO_JSVAL(jsdbl); 
					}break;
				case JSPROP_rotSpeed:{
					jsdouble* jsdbl=JS_NewDouble(cx, priv->rot_speed); 
					*vp=DOUBLE_TO_JSVAL(jsdbl); 
					}break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}


// Реализация mlRMMLElement
mlresult mlRMMLMovement::CreateMedia(omsContext* amcx){
//	ML_ASSERTION(amcx->mpRM!=NULL,"mlRMMLMovement::CreateMedia");
//	amcx->mpRM->CreateMO(this);
//	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
//	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlRMMLElement* mlRMMLMovement::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLMovement* pNewEL=(mlRMMLMovement*)GET_RMMLEL(mcx, mlRMMLMovement::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

}

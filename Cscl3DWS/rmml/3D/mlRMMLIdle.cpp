#include "mlRMML.h"

namespace rmml {

mlRMMLIdle::mlRMMLIdle(void)
{
	mRMMLType=mType=MLMT_IDLE;
	mbChildrenMLElsCreated=false;
    mjssMotion=NULL;
	motion=NULL;
	between=1000;
	freq=1.0;
}

mlRMMLIdle::~mlRMMLIdle(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLIdle::~mlRMMLIdle");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLIdle::_JSPropertySpec[] = {
	JSPROP_RW(motion)
	JSPROP_RW(between)
	JSPROP_RW(freq)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLIdle::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Idle,mlRMMLIdle,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLIdle)

///// JavaScript Set Property Wrapper
JSBool mlRMMLIdle::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLIdle);
		SET_PROTO_PROP(mlRMMLElement);
		default:
			switch(iID){
				case JSPROP_motion:{
//					SET_REF_JSPROP(priv->motion,priv->mjssMotion,mlRMMLMotion,MLMT_MOTION);
					if(JSVAL_IS_NULL(*vp)){
						priv->motion=NULL;
					}else if(JSVAL_IS_OBJECT(*vp)){
						JSObject* jso=JSVAL_TO_OBJECT(*vp);
						if(mlRMMLMotion::IsInstance(cx, jso)){
							priv->motion=(mlRMMLMotion*)JS_GetPrivate(cx,jso);
							if(priv->motion->GetParent()!=priv){
								JS_ReportWarning(cx,"motion-object is of another character-object");
							}
						}else{
							JS_ReportError(cx, "Parameter is not a motion-object");
							return JS_FALSE;
						}
					}else if(JSVAL_IS_STRING(*vp)){
						JSString* jss=JSVAL_TO_STRING(*vp);
						if(!priv->mbChildrenMLElsCreated){
							priv->mjssMotion=jss;
							SAVE_STR_FROM_GC(cx,obj,priv->mjssMotion);
							return JS_TRUE;
						}
						mlRMMLElement* pMLEl=NULL; 
						mlRMMLElement* pParent=priv->GetParent();
						if(pParent!=NULL){
							pParent=pParent->GetParent();
						}
						if(pParent==NULL){
							JS_ReportError(cx, "_parent of '%s' is null",cLPCSTR(priv->GetName()));
							return JS_FALSE;
						}
						pMLEl=pParent->GetChild(jss);
						if(pMLEl==NULL){
							JS_ReportError(cx, "motion-object '%s' for '%s' not found",JS_GetStringBytes(jss),cLPCSTR(priv->GetName()));
							return JS_FALSE;
						}
						if(pMLEl->mRMMLType!=MLMT_MOTION){
							JS_ReportError(cx, "Named object is not a motion");
							return JS_FALSE;
						}
						priv->motion=(mlRMMLMotion*)pMLEl;
					}
					}break;
				case JSPROP_between:{
					jsdouble jsdbl;
					if(JS_ValueToNumber(cx, *vp, &jsdbl)==JS_TRUE){ 
						priv->between=(int)jsdbl;
					}else{
						return JS_FALSE;
					} 
					}break;
				case JSPROP_freq:{
					jsdouble jsdbl;
					if(JS_ValueToNumber(cx, *vp, &jsdbl)==JS_TRUE){ 
						priv->freq=jsdbl;
					}else{
						return JS_FALSE;
					} 
					}break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLIdle::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLIdle);
		GET_PROTO_PROP(mlRMMLElement);
		default:
			switch(iID){
				case JSPROP_motion:{
					if(priv->motion!=NULL){
						*vp=OBJECT_TO_JSVAL(priv->motion->mjso);
						break;
					}
					if(priv->mjssMotion!=NULL){
						*vp=STRING_TO_JSVAL(priv->mjssMotion);
						break;
					}
					*vp=JSVAL_NULL; 
					}break;
				case JSPROP_between:{
					*vp=INT_TO_JSVAL(priv->between); 
					}break;
				case JSPROP_freq:{
					jsdouble* jsdbl=JS_NewDouble(cx, priv->freq);
					*vp=DOUBLE_TO_JSVAL(jsdbl); 
					}break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

#define MLCH_SET_REF(jss,var,MLELPN) \
if(jss!=NULL && var==NULL){ \
		jsval v=STRING_TO_JSVAL(jss); \
		JS_SetProperty(mcx,mjso,MLELPN,&v); \
		FREE_STR_FOR_GC(mcx,mjso,jss); \
		jss=NULL; \
	}

// Реализация mlRMMLElement
mlresult mlRMMLIdle::CreateMedia(omsContext* amcx){
//	ML_ASSERTION(amcx->mpRM!=NULL,"mlRMMLIdle::CreateMedia");
//	amcx->mpRM->CreateMO(this);
//	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
//	mpMO->SetMLMedia(this);
	mbChildrenMLElsCreated=true;
	MLCH_SET_REF(mjssMotion,motion,MLELPN_MOTION);
	return ML_OK;
}

mlRMMLElement* mlRMMLIdle::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLIdle* pNewEL=(mlRMMLIdle*)GET_RMMLEL(mcx, mlRMMLIdle::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

}

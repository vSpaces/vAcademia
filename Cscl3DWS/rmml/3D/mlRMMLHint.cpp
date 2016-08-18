#include "mlRMML.h"

namespace rmml {

mlRMMLHint::mlRMMLHint(void)
{
	mRMMLType=MLMT_HINT;
	mType=MLMT_HINT;
	mjsoStyles=NULL;
	style=NULL;
	mbStylesRef=false;
	delayBefore=250;
	showDelay=5000;
}

mlRMMLHint::~mlRMMLHint(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLHint::~mlRMMLHint");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLHint::_JSPropertySpec[] = {
	JSPROP_RW(style)
	JSPROP_RW(delayBefore)
	JSPROP_RW(showDelay)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLHint::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Hint,mlRMMLHint,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLHint)


///// JavaScript Set Property Wrapper
JSBool mlRMMLHint::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLHint);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		default:
			switch(iID){
			case JSPROP_style: // точно как в mlRMMLText
				if(JSVAL_IS_NULL(*vp)){
					priv->style=NULL;
					priv->mjsoStyles=NULL;
					priv->mbStylesRef=false;
				}else if(JSVAL_IS_OBJECT(*vp)){ 
					JSObject* jso=JSVAL_TO_OBJECT(*vp);
					if(mlRMMLStyles::IsInstance(cx, jso)){
						priv->mjsoStyles=jso;
						priv->mbStylesRef=true;
						priv->style=NULL;
//						priv->StylesChanged();
					}else{
						JS_ReportError(cx, "Parameter is not a styles-object");
						return JS_FALSE;
					}
				}else if(JSVAL_IS_STRING(*vp)){ 
					JSString* jss=JSVAL_TO_STRING(*vp);
					if(IsElementRef(jss)){
						if(priv->GetMO()==NULL){ 
							// CreateMedia wasn't called
							priv->style=jss;
						}else{
							if(priv->mpParent==NULL){
								priv->style=jss;
							}else{
								mlRMMLElement* pMLEl=priv->FindStyles(jss);
								if(pMLEl==NULL){
									JS_ReportError(cx, "Styles-object not found");
									return JS_FALSE;
								}
								if(pMLEl->mRMMLType!=MLMT_STYLES){
									JS_ReportError(cx, "Named object is not a styles");
									return JS_FALSE;
								} 
								priv->mjsoStyles=pMLEl->mjso;
								SAVE_FROM_GC(cx,priv->mjso,priv->mjsoStyles)
								priv->mbStylesRef=true;
								priv->style=NULL;
//								priv->StylesChanged();
							}
						}
					}else{
						// create own styles-object
						// ??
						// and init it with jss value
						// ??
					}
				}
				break;
			case JSPROP_delayBefore:
				if(JSVAL_IS_NUMBER(*vp))
					JS_ValueToInt32(cx,*vp,&(priv->delayBefore));
				break;
			case JSPROP_showDelay:
				if(JSVAL_IS_NUMBER(*vp))
					JS_ValueToInt32(cx,*vp,&(priv->showDelay));
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLHint::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLHint);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		default:
			switch(iID){
			case JSPROP_style:
				// ??
				break;
			case JSPROP_delayBefore:
				*vp=INT_TO_JSVAL(priv->delayBefore);
				break;
			case JSPROP_showDelay:
				*vp=INT_TO_JSVAL(priv->showDelay);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}


// Реализация mlRMMLElement
mlresult mlRMMLHint::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLHint::CreateMedia");
	// set styles
	if(style!=NULL && mjsoStyles==NULL){ 
		mpMO=PMOV_CREATED;
		jsval v=STRING_TO_JSVAL(style); 
		JS_SetProperty(mcx,mjso,"style",&v); 
//		FREE_STR_FOR_GC(mcx,mjso,style); 
		mpMO=NULL;
	}
	//
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLHint::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded.
		return ML_ERROR_FAILURE;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLHint::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLHint* pNewEL=(mlRMMLHint*)GET_RMMLEL(mcx, mlRMMLHint::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

// точно как в mlRMMLText
mlRMMLElement* mlRMMLHint::FindStyles(JSString* ajssStylesName){
	if(mpParent==NULL) return NULL;
	mlRMMLElement* pMLEl=mpParent->FindElemByName(ajssStylesName);
	if(pMLEl!=NULL) return pMLEl;
	wchar_t* jsc=wr_JS_GetStringChars(ajssStylesName);
	int iLen=JS_GetStringLength(ajssStylesName);
	const wchar_t* pwcDot=findDot(jsc);
	if(pwcDot!=NULL) return NULL;
	mlRMMLElement* pParent=mpParent->mpParent;
	while(pParent!=NULL){
		pMLEl=pParent->GetChild(jsc,iLen);
		if(pMLEl!=NULL) return pMLEl;
		pParent=pParent->mpParent;
	}
	// поищем в классах
	jsval vClss;
	JS_GetProperty(mcx, JS_GetGlobalObject(mcx),GJSONM_CLASSES, &vClss);
	if(!JSVAL_IS_OBJECT(vClss)) return NULL;
	// получаем объект класса
	jsval vc;
	wr_JS_GetUCProperty(mcx, JSVAL_TO_OBJECT(vClss),jsc,iLen,&vc);
	if(!JSVAL_IS_OBJECT(vc)) return NULL;
	JSObject* jso=JSVAL_TO_OBJECT(vc);
	if(!mlRMMLStyles::IsInstance(mcx, jso)) return NULL;
	pMLEl=GET_RMMLEL(mcx,jso);
	return NULL;
}

// реализация mlIHint
mlStyle* mlRMMLHint::GetStyle(const char* apszTag){ // точно как в mlRMMLText
	if(mjsoStyles==NULL && style!=NULL){ 
		jsval v=STRING_TO_JSVAL(style); 
		JS_SetProperty(mcx,mjso,"style",&v); 
	}
	if(mjsoStyles==NULL) return NULL;
	mlRMMLStyles* pStyles=(mlRMMLStyles*)JS_GetPrivate(mcx,mjsoStyles);
	ML_ASSERTION(mcx, pStyles!=NULL,"mlRMMLHint::GetStyle");
	return pStyles->GetStyle(apszTag);
}

}

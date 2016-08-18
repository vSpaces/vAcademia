#include "mlRMML.h"

namespace rmml {

mlRMMLGroup::mlRMMLGroup(void)
{
	mRMMLType=MLMT_GROUP;
	mType=MLMT_GROUP;

	scripted = false;
}

mlRMMLGroup::~mlRMMLGroup(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLGroup::~mlRMMLGroup");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLGroup::_JSPropertySpec[] = {
//	{ "_camera", JSPROP__camera, JSPROP_ENUMERATE, 0, 0},
	JSPROP_RW(gravitation)
	JSPROP_RW(intangible)
	JSPROP_RW(scripted)
	JSPROP_RW(color)
	JSPROP_RW(opacity)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLGroup::_JSFunctionSpec[] = {
//	{ "approach", JSFUNC_approach, 3, 0, 0 },
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Group,mlRMMLGroup,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMML3DObject)
	MLJSCLASS_ADDPROTO(mlRMMLPButton)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLGroup)


///// JavaScript Set Property Wrapper
JSBool mlRMMLGroup::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
//if(JSVAL_IS_STRING(id)){
//JSString* jssPropName = JSVAL_TO_STRING(id);
//mlRMMLGroup* pGroup = (mlRMMLGroup*)JS_GetPrivate(cx, obj);
//if( pGroup->GetName()!=NULL && isEqual(pGroup->GetName(), L"bulb1") && 
//   isEqual(JS_GetStringChars(jssPropName),L"setmotion"))
//int hh=0;
//}
	SET_PROPS_BEGIN(mlRMMLGroup);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		SET_PROTO_PROP(mlRMML3DObject);
		SET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID){
				case JSPROP_gravitation:{
					ML_JSVAL_TO_BOOL(priv->gravitation,*vp);
					priv->GravitationChanged();
					}break;
				case JSPROP_intangible:{
					ML_JSVAL_TO_BOOL(priv->intangible,*vp);
					priv->IntangibleChanged();
					}break;
				case JSPROP_scripted:{
					ML_JSVAL_TO_BOOL(priv->scripted,*vp);
					priv->ScriptedChanged();
					}break;
				case JSPROP_color:{
					priv->Get3DObject()->SetColorJSProp(cx, *vp);
					}break;
				case JSPROP_opacity:{
					priv->Get3DObject()->SetOpacityJSProp(cx, *vp);
					}break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLGroup::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLGroup);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		GET_PROTO_PROP(mlRMML3DObject);
		GET_PROTO_PROP(mlRMMLPButton);
		default:
			switch(iID){
				case JSPROP_gravitation:{
					*vp = BOOLEAN_TO_JSVAL(priv->gravitation);
					}break;
				case JSPROP_intangible:{
					*vp = BOOLEAN_TO_JSVAL(priv->intangible);
					}break;
				case JSPROP_scripted:{
					*vp = BOOLEAN_TO_JSVAL(priv->scripted);
					}break;
				case JSPROP_color:{
					priv->GetColorJSProp(cx, vp);
					}break;
				case JSPROP_opacity:{
					priv->GetOpacityJSProp(cx, vp);
					}break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}


// Реализация mlRMMLElement
mlresult mlRMMLGroup::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLGroup::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLGroup::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. 
		return ML_ERROR_FAILURE;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLGroup::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLGroup* pNewEL=(mlRMMLGroup*)GET_RMMLEL(mcx, mlRMMLGroup::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

// moI3DGroup
moI3DGroup* mlRMMLGroup::GetmoI3DGroup(){
	if(PMO_IS_NULL(mpMO)) return NULL;
	return mpMO->GetIGroup3D();
}

void mlRMMLGroup::ChildAdded(mlRMMLElement* apNewChild){
	moI3DGroup*	pmoI3DGroup = GetmoI3DGroup();
	if(pmoI3DGroup==NULL) return;
	pmoI3DGroup->ChildAdded(apNewChild);
}

void mlRMMLGroup::ChildRemoving(mlRMMLElement* apChild){
	moI3DGroup*	pmoI3DGroup = GetmoI3DGroup();
	if(pmoI3DGroup==NULL) return;
	pmoI3DGroup->ChildRemoving(apChild);
}

void mlRMMLGroup::ScriptedChanged(){
	moI3DGroup*	pmoI3DGroup = GetmoI3DGroup();
	if(pmoI3DGroup==NULL) return;
	pmoI3DGroup->ScriptedChanged();
}

}

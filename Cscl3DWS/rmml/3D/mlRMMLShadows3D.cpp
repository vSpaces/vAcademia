
#include "mlRMML.h"

namespace rmml {

mlRMMLShadows3D::mlRMMLShadows3D(void):
	MP_VECTOR_INIT(mvChangeListeners)
{
	mType=mRMMLType=MLMT_SHADOWS3D;
}

mlRMMLShadows3D::~mlRMMLShadows3D(void){
	v_elems::iterator vi;
	for(vi=mvChangeListeners.begin(); vi != mvChangeListeners.end(); vi++ ){
		mlRMML3DObject* pML3DO=(mlRMML3DObject*)*vi;
		pML3DO->NotifyShadowsElemDestroyed(this);
	}
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLShadows3D::_JSPropertySpec[] = {
    JSPROP_RW(type)
    JSPROP_RW(plane)
    JSPROP_RW(color)
    JSPROP_RW(light)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLShadows3D::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Shadows3D,mlRMMLShadows3D,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLShadows3D)

///// JavaScript Set Property Wrapper
JSBool mlRMMLShadows3D::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLShadows3D);
		SET_PROTO_PROP(mlRMMLElement);
		default:
			priv->ml3DShadow::SetJSProperty(cx, iID, vp);
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLShadows3D::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLShadows3D);
		GET_PROTO_PROP(mlRMMLElement);
		default:
			priv->ml3DShadow::GetJSProperty(cx, iID, vp);
	GET_PROPS_END;
	return JS_TRUE;
}

void mlRMMLShadows3D::NotifyChangeListeners(){
	v_elems::iterator vi;
	for(vi=mvChangeListeners.begin(); vi != mvChangeListeners.end(); vi++ ){
		mlRMML3DObject* pML3DO=(mlRMML3DObject*)*vi;
		pML3DO->ShadowChanged();
	}
}

void mlRMMLShadows3D::AddChangeListener(mlRMML3DObject* ap3DO){
	mvChangeListeners.push_back((mlRMMLElement*)ap3DO);
}

void mlRMMLShadows3D::RemoveChangeListener(mlRMML3DObject* ap3DO){
	v_elems::iterator vi;
	for(vi=mvChangeListeners.begin(); vi != mvChangeListeners.end(); vi++ ){
		if(*vi==(mlRMMLElement*)ap3DO){
			mvChangeListeners.erase(vi);
			break;
		}
	}
}

// Реализация mlRMMLElement
mlRMMLElement* mlRMMLShadows3D::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLShadows3D* pNewEL=(mlRMMLShadows3D*)GET_RMMLEL(mcx, mlRMMLShadows3D::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

// Реализация mlIShadows
mlMedia* mlRMMLShadows3D::GetShadow(int aiNum){
	return GetChildByIdx(aiNum);
}

}

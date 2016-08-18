#include "mlRMML.h"

namespace rmml {

mlRMMLPath3D::mlRMMLPath3D(void)
{
	mRMMLType=MLMT_PATH3D;
	mType=MLMT_PATH3D;
}

mlRMMLPath3D::~mlRMMLPath3D(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLPath3D::~mlRMMLPath3D");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLPath3D::_JSPropertySpec[] = {
//	{ "_camera", JSPROP__camera, JSPROP_ENUMERATE, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLPath3D::_JSFunctionSpec[] = {
	JSFUNC(getPosition,1)
	JSFUNC(getNormal,1)
	JSFUNC(getTangent,1)
	JSFUNC(getNormalByLinearPos,1)
	JSFUNC(getTangentByLinearPos,1)
	JSFUNC(getLinearPosition,1)
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Path3D,mlRMMLPath3D,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLPath3D)


///// JavaScript Set Property Wrapper
JSBool mlRMMLPath3D::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLPath3D);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		default:;
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLPath3D::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLPath3D);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		default:;
	GET_PROPS_END;
	return JS_TRUE;
}


// Реализация mlRMMLElement
mlresult mlRMMLPath3D::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLPath3D::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLPath3D::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded.
		return ML_ERROR_FAILURE;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLPath3D::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLPath3D* pNewEL=(mlRMMLPath3D*)GET_RMMLEL(mcx, mlRMMLPath3D::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

JSBool JSVal2Koef(JSContext *cx, jsval val, jsdouble& dKoef, const char* funcName){
	if(!JS_ValueToNumber(cx, val, &dKoef)){
		JS_ReportError(cx,"Path3D.%s method must get one float argument", funcName);
		return JS_FALSE;
	}
	if(dKoef < 0 || dKoef > 1.0){
		JS_ReportError(cx,"Path3D.%s argument must be in range from 0 to 1", funcName);
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool mlRMMLPath3D::JSFUNC_getPosition(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLPath3D* pMLEl = (mlRMMLPath3D*)JS_GetPrivate(cx, obj);
	if(argc == 0 || !JSVAL_IS_NUMBER(argv[0])){
		JS_ReportError(cx,"Path3D.getPosition method must get one float argument");
		return JS_FALSE;
	}
	jsdouble dKoef;
	if(!JSVal2Koef(cx, argv[0], dKoef, "getPosition")){
		return JS_FALSE;
	}
	if(argc > 1){
		JS_ReportWarning(cx,"Path3D.getPosition method must get only one float argument");
	}
	ml3DPosition pos = pMLEl->getPosition(dKoef);
	JSObject* jsoNew = mlPosition3D::newJSObject(cx);
	mlPosition3D* pNewPos = (mlPosition3D*)JS_GetPrivate(cx,jsoNew);
	pNewPos->SetPos(pos);
	*rval = OBJECT_TO_JSVAL(jsoNew);
	return JS_TRUE;
}

JSBool mlRMMLPath3D::JSFUNC_getLinearPosition(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLPath3D* pMLEl = (mlRMMLPath3D*)JS_GetPrivate(cx, obj);
	if(argc == 0 || !JSVAL_IS_NUMBER(argv[0])){
		JS_ReportError(cx,"Path3D.getLinearPosition method must get one float argument");
		return JS_FALSE;
	}
	jsdouble dKoef;
	if(!JSVal2Koef(cx, argv[0], dKoef, "getPosition")){
		return JS_FALSE;
	}
	if(argc > 1){
		JS_ReportWarning(cx,"Path3D.getLinearPosition method must get only one float argument");
	}
	ml3DPosition pos = pMLEl->getLinearPosition(dKoef);
	JSObject* jsoNew = mlPosition3D::newJSObject(cx);
	mlPosition3D* pNewPos = (mlPosition3D*)JS_GetPrivate(cx,jsoNew);
	pNewPos->SetPos(pos);
	*rval = OBJECT_TO_JSVAL(jsoNew);
	return JS_TRUE;
}

JSBool mlRMMLPath3D::JSFUNC_getNormal(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLPath3D* pMLEl = (mlRMMLPath3D*)JS_GetPrivate(cx, obj);
	if(argc == 0 || !JSVAL_IS_NUMBER(argv[0])){
		JS_ReportError(cx,"Path3D.getNormal method must get one float argument");
		return JS_FALSE;
	}
	jsdouble dKoef;
	if(!JSVal2Koef(cx, argv[0], dKoef, "getNormal")){
		return JS_FALSE;
	}
	if(argc > 1){
		JS_ReportWarning(cx,"Path3D.getNormal method must get only one float argument");
	}
	ml3DPosition pos = pMLEl->getNormal(dKoef);
	JSObject* jsoNew = mlPosition3D::newJSObject(cx);
	mlPosition3D* pNewPos = (mlPosition3D*)JS_GetPrivate(cx,jsoNew);
	pNewPos->SetPos(pos);
	*rval = OBJECT_TO_JSVAL(jsoNew);
	return JS_TRUE;
}

// получить касательную к определенной позиции (0..1) на пути
JSBool mlRMMLPath3D::JSFUNC_getTangent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLPath3D* pMLEl = (mlRMMLPath3D*)JS_GetPrivate(cx, obj);
	if(argc == 0 || !JSVAL_IS_NUMBER(argv[0])){
		JS_ReportError(cx,"Path3D.getTangent method must get one float argument");
		return JS_FALSE;
	}
	jsdouble dKoef;
	if(!JSVal2Koef(cx, argv[0], dKoef, "getTangent")){
		return JS_FALSE;
	}
	if(argc > 1){
		JS_ReportWarning(cx,"Path3D.getTangent method must get only one float argument");
	}
	ml3DPosition pos = pMLEl->getTangent(dKoef);
	JSObject* jsoNew = mlPosition3D::newJSObject(cx);
	mlPosition3D* pNewPos = (mlPosition3D*)JS_GetPrivate(cx,jsoNew);
	pNewPos->SetPos(pos);
	*rval = OBJECT_TO_JSVAL(jsoNew);
	return JS_TRUE;
}

// получить нормаль к определенной линейной позиции (0..1) на пути
JSBool mlRMMLPath3D::JSFUNC_getNormalByLinearPos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLPath3D* pMLEl = (mlRMMLPath3D*)JS_GetPrivate(cx, obj);
	if(argc == 0 || !JSVAL_IS_NUMBER(argv[0])){
		JS_ReportError(cx,"Path3D.getNormalByLinearPos method must get one float argument");
		return JS_FALSE;
	}
	jsdouble dKoef;
	if(!JSVal2Koef(cx, argv[0], dKoef, "getNormalByLinearPos")){
		return JS_FALSE;
	}
	if(argc > 1){
		JS_ReportWarning(cx,"Path3D.getNormalByLinearPos method must get only one float argument");
	}
	ml3DPosition pos = pMLEl->getNormalByLinearPos(dKoef);
	JSObject* jsoNew = mlPosition3D::newJSObject(cx);
	mlPosition3D* pNewPos = (mlPosition3D*)JS_GetPrivate(cx,jsoNew);
	pNewPos->SetPos(pos);
	*rval = OBJECT_TO_JSVAL(jsoNew);
	return JS_TRUE;
}

// получить касательную к определенной линейной позиции (0..1) на пути
JSBool mlRMMLPath3D::JSFUNC_getTangentByLinearPos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlRMMLPath3D* pMLEl = (mlRMMLPath3D*)JS_GetPrivate(cx, obj);
	if(argc == 0 || !JSVAL_IS_NUMBER(argv[0])){
		JS_ReportError(cx,"Path3D.getTangentByLinearPos method must get one float argument");
		return JS_FALSE;
	}
	jsdouble dKoef;
	if(!JSVal2Koef(cx, argv[0], dKoef, "getTangentByLinearPos")){
		return JS_FALSE;
	}
	if(argc > 1){
		JS_ReportWarning(cx,"Path3D.getTangentByLinearPos method must get only one float argument");
	}
	ml3DPosition pos = pMLEl->getTangentByLinearPos(dKoef);
	JSObject* jsoNew = mlPosition3D::newJSObject(cx);
	mlPosition3D* pNewPos = (mlPosition3D*)JS_GetPrivate(cx,jsoNew);
	pNewPos->SetPos(pos);
	*rval = OBJECT_TO_JSVAL(jsoNew);
	return JS_TRUE;
}

#define ML_PATH3D_CALL_MO_FUNC1(FUNC) \
ml3DPosition mlRMMLPath3D::FUNC(double adKoef) { \
	if(PMO_IS_NULL(mpMO)){ \
		ml3DPosition pos; ML_INIT_3DPOSITION(pos); \
		return pos; \
	} \
	moIPath3D* pPath3D = mpMO->GetIPath3D(); \
	ML_ASSERTION(mcx, pPath3D!=NULL ,"mlRMMLPath3D::" #FUNC) \
	return pPath3D->FUNC(adKoef); \
}

ML_PATH3D_CALL_MO_FUNC1(getPosition)
ML_PATH3D_CALL_MO_FUNC1(getLinearPosition)
ML_PATH3D_CALL_MO_FUNC1(getNormal)
ML_PATH3D_CALL_MO_FUNC1(getTangent)
ML_PATH3D_CALL_MO_FUNC1(getNormalByLinearPos)
ML_PATH3D_CALL_MO_FUNC1(getTangentByLinearPos)

//ml3DPosition mlRMMLPath3D::getPosition(double adKoef) {
//	if(PMO_IS_NULL(mpMO)){
//		ml3DPosition pos; ML_INIT_3DPOSITION(pos);
//		return pos;
//	}
//	moIPath3D* pPath3D = mpMO->GetIPath3D();
//	ML_ASSERTION(mcx, pPath3D!=NULL ,"mlRMMLPath3D::getPosition")
//	return pPath3D->getPosition(adKoef);
//}

}

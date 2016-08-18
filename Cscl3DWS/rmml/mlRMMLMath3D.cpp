
#include "mlRMML.h"

namespace rmml {

/**
 * Реализация глобального JS-объекта "Math3D"
 */

JSBool JSMath3DGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){
	return JS_TRUE;
}

JSBool JSMath3DSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){
	return JS_TRUE;
}
	
JSClass JSRMMLMath3DClass = {
	"_Math3D", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, 
	JSMath3DGetProperty, JSMath3DSetProperty,
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL,
	NULL, NULL, NULL
};

JSPropertySpec _JSMath3DPropertySpec[] = {
	{ 0, 0, 0, 0, 0 }
};

JSFUNC_DECL(add)
JSFUNC_DECL(sub)
JSFUNC_DECL(mult)
JSFUNC_DECL(div)

///// JavaScript Function Table
JSFunctionSpec _JSMath3DFunctionSpec[] = {
	JSFUNC(add,2)	// сложение векторов (Position3D)
	JSFUNC(sub,2)	// вычитание векторов (Position3D)
	JSFUNC(mult,2)	// умножение вектора на число (Position3D)
					// или дополнительный поворот (Rotation3D)
	JSFUNC(div,2)	// деление вектора на число (Position3D)
	{ 0, 0, 0, 0, 0 }
};

void InitMath3DJSObject(JSContext *cx, JSObject *ajsoMath3D){
	bool bR;
	bR=JS_DefineProperties(cx, ajsoMath3D, _JSMath3DPropertySpec);
	bR=JS_DefineFunctions(cx, ajsoMath3D, _JSMath3DFunctionSpec);
}

__forceinline JSObject* CreatePosition3DJSO(JSContext *cx, ml3DPosition &pos){
	JSObject* jsoNew = mlPosition3D::newJSObject(cx);
	mlPosition3D* pNewPos = (mlPosition3D*)JS_GetPrivate(cx,jsoNew);
	pNewPos->SetPos(pos);
	return jsoNew;
}

__forceinline JSObject* CreateRotation3DJSO(JSContext *cx, ml3DRotation &rot){
	JSObject* jsoNew = mlRotation3D::newJSObject(cx);
	mlRotation3D* pNewRot = (mlRotation3D*)JS_GetPrivate(cx,jsoNew);
	pNewRot->SetRot(rot);
	return jsoNew;
}

JSBool JSFUNC_add(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	bool bError = true;
	for(;;){
		if(argc < 2 || 
			!JSVAL_IS_OBJECT(argv[0]) || JSVAL_IS_NULL(argv[0]) || JSVAL_IS_VOID(argv[0]) || 
			!JSVAL_IS_OBJECT(argv[1]) || JSVAL_IS_NULL(argv[1]) || JSVAL_IS_VOID(argv[1]))
			break;
		JSObject* jsoArgPos1 = JSVAL_TO_OBJECT(argv[0]);
		JSObject* jsoArgPos2 = JSVAL_TO_OBJECT(argv[1]);
		if(!mlPosition3D::IsInstance(cx, jsoArgPos1) || !mlPosition3D::IsInstance(cx, jsoArgPos2))
			break;
		if(argc > 2)
			JS_ReportWarning(cx, "Math3D.add method must get two Position3D arguments");
		mlPosition3D* pArgPos1 = (mlPosition3D*)JS_GetPrivate(cx, jsoArgPos1);
		mlPosition3D* pArgPos2 = (mlPosition3D*)JS_GetPrivate(cx, jsoArgPos2);
		ml3DPosition pos1 = pArgPos1->GetPos();
		ml3DPosition pos2 = pArgPos2->GetPos();
		ml3DPosition posRes;
		posRes.x = pos1.x + pos2.x;
		posRes.y = pos1.y + pos2.y;
		posRes.z = pos1.z + pos2.z;
		*rval = OBJECT_TO_JSVAL(CreatePosition3DJSO(cx, posRes));
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Math3D.add method must get two Position3D arguments");
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool JSFUNC_sub(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	bool bError = true;
	for(;;){
		if(argc < 2 || 
			!JSVAL_IS_OBJECT(argv[0]) || JSVAL_IS_NULL(argv[0]) || JSVAL_IS_VOID(argv[0]) || 
			!JSVAL_IS_OBJECT(argv[1]) || JSVAL_IS_NULL(argv[1]) || JSVAL_IS_VOID(argv[1]))
			break;
		JSObject* jsoArgPos1 = JSVAL_TO_OBJECT(argv[0]);
		JSObject* jsoArgPos2 = JSVAL_TO_OBJECT(argv[1]);
		if(!mlPosition3D::IsInstance(cx, jsoArgPos1) || !mlPosition3D::IsInstance(cx, jsoArgPos2))
			break;
		if(argc > 2)
			JS_ReportWarning(cx, "Math3D.sub method must get two Position3D arguments");
		mlPosition3D* pArgPos1 = (mlPosition3D*)JS_GetPrivate(cx, jsoArgPos1);
		mlPosition3D* pArgPos2 = (mlPosition3D*)JS_GetPrivate(cx, jsoArgPos2);
		ml3DPosition pos1 = pArgPos1->GetPos();
		ml3DPosition pos2 = pArgPos2->GetPos();
		ml3DPosition posRes;
		posRes.x = pos1.x - pos2.x;
		posRes.y = pos1.y - pos2.y;
		posRes.z = pos1.z - pos2.z;
		*rval = OBJECT_TO_JSVAL(CreatePosition3DJSO(cx, posRes));
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Math3D.sub method must get two Position3D arguments");
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool JSFUNC_mult(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	bool bError = true;
	for(;;){
		if(argc < 2 || 
			!JSVAL_IS_OBJECT(argv[0]) || JSVAL_IS_NULL(argv[0]) || JSVAL_IS_VOID(argv[0]))
			break;
		JSObject* jsoArg1 = JSVAL_TO_OBJECT(argv[0]);
		if(mlPosition3D::IsInstance(cx, jsoArg1)){
			jsdouble dArg;
			if(!JS_ValueToNumber(cx, argv[1], &dArg))
				break;
			if(argc > 2)
				JS_ReportWarning(cx, "Math3D.mult method must get one Position3D and one float argument");
			mlPosition3D* pArgPos1 = (mlPosition3D*)JS_GetPrivate(cx, jsoArg1);
			ml3DPosition pos = pArgPos1->GetPos();
			pos.x *= dArg;
			pos.y *= dArg;
			pos.z *= dArg;
			*rval = OBJECT_TO_JSVAL(CreatePosition3DJSO(cx, pos));
		}else if(mlRotation3D::IsInstance(cx, jsoArg1)){
			if(!JSVAL_IS_OBJECT(argv[1]) || JSVAL_IS_NULL(argv[1]) || JSVAL_IS_VOID(argv[1]))
				break;
			JSObject* jsoArgRot2 = JSVAL_TO_OBJECT(argv[1]);
			if(!mlRotation3D::IsInstance(cx, jsoArgRot2))
				break;
			if(argc > 2)
				JS_ReportWarning(cx, "Math3D.mult method must get two Rotation3D arguments");
			mlRotation3D* pArgRot1 = (mlRotation3D*)JS_GetPrivate(cx, jsoArg1);
			mlRotation3D* pArgRot2 = (mlRotation3D*)JS_GetPrivate(cx, jsoArgRot2);
			ml3DRotation rot1 = pArgRot1->GetRot();
			ml3DRotation rot2 = pArgRot2->GetRot();
			ml3DRotation rotRes;
			rm::IMath3DRMML*	pmath3d = GPSM(cx)->GetContext()->mpRM->GetMath3D();
			if( pmath3d)
				pmath3d->quatMult(rot1, rot2, rotRes);
			*rval = OBJECT_TO_JSVAL(CreateRotation3DJSO(cx, rotRes));
		}else 
			break;
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Math3D.mult method must get one Position3D and one float argument or two Rotation3D arguments");
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool JSFUNC_div(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	bool bError = true;
	for(;;){
		if(argc < 2 || 
			!JSVAL_IS_OBJECT(argv[0]) || JSVAL_IS_NULL(argv[0]) || JSVAL_IS_VOID(argv[0]))
			break;
		JSObject* jsoArg1 = JSVAL_TO_OBJECT(argv[0]);
		if(!mlPosition3D::IsInstance(cx, jsoArg1))
			break;
		jsdouble dArg;
		if(!JS_ValueToNumber(cx, argv[1], &dArg))
			break;
		if(argc > 2)
			JS_ReportWarning(cx, "Math3D.div method must get one Position3D and one float argument");
		mlPosition3D* pArgPos1 = (mlPosition3D*)JS_GetPrivate(cx, jsoArg1);
		ml3DPosition pos = pArgPos1->GetPos();
		pos.x /= dArg;
		pos.y /= dArg;
		pos.z /= dArg;
		*rval = OBJECT_TO_JSVAL(CreatePosition3DJSO(cx, pos));
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Math3D.div method must get one Position3D and one float argument");
		return JS_FALSE;
	}
	return JS_TRUE;
}

}
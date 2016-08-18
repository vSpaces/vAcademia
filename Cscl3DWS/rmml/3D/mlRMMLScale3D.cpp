
#include "mlRMML.h"

namespace rmml {

mlScale3D::mlScale3D(void)
{
	pScl=NULL;
	bRef=false;
}

JSBool mlScale3D::AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(mlScale3D::JSConstructor(cx, obj, argc, argv, rval) == JS_FALSE)
		return JS_FALSE;
	if(argc > 3){
		JS_ReportError(cx,"Too many arguments for Scale3D constructor");
		return JS_FALSE;
	}
	if(argc > 0){
		mlScale3D* pThis=(mlScale3D*)JS_GetPrivate(cx,obj);
		pThis->CreateScl();
		if(argc == 1 && JSVAL_IS_OBJECT(argv[0])){
			if(argv[0] != JSVAL_NULL && argv[0] != JSVAL_VOID){
				JSObject* jsoArg = JSVAL_TO_OBJECT(argv[0]);
				if(mlScale3D::IsInstance(cx, jsoArg)){
					mlScale3D* pArgPos=(mlScale3D*)JS_GetPrivate(cx,jsoArg);
					pThis->SetScl(pArgPos->GetScl());
				}
			}
		}else{
			if(argc >= 1){
				jsval v=argv[0];
				if(!JSVAL_IS_NUMBER(v)){
					JS_ReportError(cx,"Arguments of Scale3D constructor must be a numbers");
					return JS_FALSE;
				}
				JS_ValueToNumber(cx,v,&(pThis->pScl->x));
			}
			if(argc >= 2){
				jsval v=argv[1];
				if(!JSVAL_IS_NUMBER(v)){
					JS_ReportError(cx,"Arguments of Scale3D constructor must be a numbers");
					return JS_FALSE;
				}
				JS_ValueToNumber(cx,v,&(pThis->pScl->y));
			}
			if(argc >= 3){
				jsval v=argv[2];
				if(!JSVAL_IS_NUMBER(v)){
					JS_ReportError(cx,"Arguments of Scale3D constructor must be a numbers");
					return JS_FALSE;
				}
				JS_ValueToNumber(cx,v,&(pThis->pScl->z));
			}
		}
	}
	return JS_TRUE;
}

mlScale3D::~mlScale3D(void){
	if(!bRef && pScl!=NULL) MP_DELETE( pScl);
}

///// JavaScript Variable Table
JSPropertySpec mlScale3D::_JSPropertySpec[] = {
	{ "x", JSPROP_x, JSPROP_ENUMERATE, 0, 0},
	{ "y", JSPROP_y, JSPROP_ENUMERATE, 0, 0},
	{ "z", JSPROP_z, JSPROP_ENUMERATE, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlScale3D::_JSFunctionSpec[] = {
	JSFUNC(clone,0)
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Scale3D,mlScale3D,0)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDPROPENUMOP
	MLJSCLASS_ADDALTERCONSTR(AlterJSConstructor)
MLJSCLASS_IMPL_END(mlScale3D)

#define SET_SCL_V(C) \
				{ jsdouble jsdbl; \
				if(JS_ValueToNumber(cx, *vp, &jsdbl)==JS_TRUE){ \
					if(priv->bRef){ \
						priv->mpI3DScl->SetScl_##C(jsdbl); \
					}else{ \
						priv->CreateScl(); \
						priv->pScl->C=jsdbl; \
					} \
				} \
				}

///// JavaScript Set Property Wrapper
JSBool mlScale3D::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlScale3D);
		default:
			switch(iID){
			case JSPROP_x:
				SET_SCL_V(x);
				break;
			case JSPROP_y:
				SET_SCL_V(y);
				break;
			case JSPROP_z:
				SET_SCL_V(z);
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

#define GET_SCL_V(C) \
				{ double dVal; \
					if(priv->bRef){ \
						dVal=priv->mpI3DScl->GetScl().C; \
					}else{ \
						priv->CreateScl(); \
						dVal=priv->pScl->C;\
					} \
					jsdouble* jsdbl=JS_NewDouble(cx, dVal); \
					*vp=DOUBLE_TO_JSVAL(jsdbl); \
				} 

///// JavaScript Get Property Wrapper
JSBool mlScale3D::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlScale3D);
		default:
			switch(iID){
			case JSPROP_x:
				GET_SCL_V(x);
				break;
			case JSPROP_y:
				GET_SCL_V(y);
				break;
			case JSPROP_z:
				GET_SCL_V(z);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

ml3DScale mlScale3D::GetScl(){
	if(!bRef){
		CreateScl();
		return *pScl;
	}else{
		return mpI3DScl->GetScl();
	}
}

void mlScale3D::SetScl(ml3DScale &scl){
	if(bRef){
		mpI3DScl->SetScl_x(scl.x);
		mpI3DScl->SetScl_y(scl.y);
		mpI3DScl->SetScl_z(scl.z);
	}else{
		CreateScl();
		*pScl=scl;
	}
}

__forceinline JSObject* CreateScale3DJSO(JSContext *cx, ml3DScale &scl){
	JSObject* jsoNew = mlPosition3D::newJSObject(cx);
	mlScale3D* pNewScl = (mlScale3D*)JS_GetPrivate(cx,jsoNew);
	pNewScl->SetScl(scl);
	return jsoNew;
}

JSBool mlScale3D::JSFUNC_clone(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 0)
		JS_ReportWarning(cx, "Scale3D.clone method has no arguments");
	mlScale3D* pScale = (mlScale3D*)JS_GetPrivate(cx,obj);
	*rval = OBJECT_TO_JSVAL(CreateScale3DJSO(cx, pScale->GetScl()));
	return JS_TRUE;
}

}

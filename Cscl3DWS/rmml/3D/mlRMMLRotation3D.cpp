
#include "mlRMML.h"
#include <float.h>

namespace rmml {

mlRotation3D::mlRotation3D(void)
{
	pRot = NULL;
	mpRotVA = NULL; // ось поворота (если утеряна в quaternion-е)
	bRef = false;
}

JSBool mlRotation3D::AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(mlRotation3D::JSConstructor(cx, obj, argc, argv, rval) == JS_FALSE)
		return JS_FALSE;
	if(argc > 4){
		JS_ReportError(cx,"Too many arguments for Rotation3D constructor");
		return JS_FALSE;
	}
	if(argc > 0){
		mlRotation3D* pThis=(mlRotation3D*)JS_GetPrivate(cx,obj);
		pThis->CreateRot();
		if(argc == 1 && JSVAL_IS_OBJECT(argv[0])){
			if(argv[0] != JSVAL_NULL && argv[0] != JSVAL_VOID){
				JSObject* jsoArg = JSVAL_TO_OBJECT(argv[0]);
				if(mlRotation3D::IsInstance(cx, jsoArg)){
					mlRotation3D* pArgPos=(mlRotation3D*)JS_GetPrivate(cx,jsoArg);
					pThis->SetRot(pArgPos->GetRot());
				}
			}
		}else{
			ml3DRotationVA rotva;
			ML_INIT_3DROTATION(rotva)
			if(argc >= 1){
				jsval v=argv[0];
				if(!JSVAL_IS_NUMBER(v)){
					JS_ReportError(cx,"Arguments of Rotation3D constructor must be a numbers");
					return JS_FALSE;
				}
				JS_ValueToNumber(cx,v,&(rotva.x));
				if(_isnan(rotva.x))
					rotva.x = 0;
			}
			if(argc >= 2){
				jsval v=argv[1];
				if(!JSVAL_IS_NUMBER(v)){
					JS_ReportError(cx,"Arguments of Rotation3D constructor must be a numbers");
					return JS_FALSE;
				}
				JS_ValueToNumber(cx,v,&(rotva.y));
				if(_isnan(rotva.y))
					rotva.y = 0;
			}
			if(argc >= 3){
				jsval v=argv[2];
				if(!JSVAL_IS_NUMBER(v)){
					JS_ReportError(cx,"Arguments of Rotation3D constructor must be a numbers");
					return JS_FALSE;
				}
				JS_ValueToNumber(cx,v,&(rotva.z));
				if(_isnan(rotva.z))
					rotva.z = 0;
			}
			if(argc >= 4){
				jsval v=argv[3];
				if(!JSVAL_IS_NUMBER(v)){
					JS_ReportError(cx,"Arguments of Rotation3D constructor must be a numbers");
					return JS_FALSE;
				}
				JS_ValueToNumber(cx,v,&(rotva.a));
				if(_isnan(rotva.a))
					rotva.a = 0;
//				if( rotva.a == 90)
//				{
//						int aaa=0;
//				}
				/*if ((fmodf(rotva.a, 360.0) > 0) && (fmodf(rotva.a, 360.0) < 1.0))
				{
					rotva.a += 1.0 - fmodf(rotva.a, 360.0);
				}*/
				rotva.a = (rotva.a*PI/180.0); // в радианы
			}
			*(pThis->pRot) = rotva.GetQuat();
			pThis->mpRotVA = MP_NEW( ml3DRotationVA);
			*(pThis->mpRotVA) = rotva;
		}
	}
	return JS_TRUE;
}

mlRotation3D::~mlRotation3D(void){
	if(!bRef && pRot!=NULL) MP_DELETE( pRot);
	if(mpRotVA != NULL) MP_DELETE( mpRotVA);
}

///// JavaScript Variable Table
JSPropertySpec mlRotation3D::_JSPropertySpec[] = {
	{ "x", JSPROP_x, JSPROP_ENUMERATE, 0, 0},
	{ "y", JSPROP_y, JSPROP_ENUMERATE, 0, 0},
	{ "z", JSPROP_z, JSPROP_ENUMERATE, 0, 0},
	{ "a", JSPROP_a, JSPROP_ENUMERATE, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRotation3D::_JSFunctionSpec[] = {
	JSFUNC(clone,0)
	JSFUNC(mult,1)		// на Rotation3D - дополнительный поворот
	JSFUNC(conjugate,0)	// обратный поворот
	JSFUNC(blend,2)		// интерполяция
	JSFUNC(normalize,0)	// нормализовать
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Rotation3D,mlRotation3D,0)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDPROPENUMOP
	MLJSCLASS_ADDALTERCONSTR(AlterJSConstructor)
MLJSCLASS_IMPL_END(mlRotation3D)

//#define SET_ROT_V(C) \
//				{ jsdouble jsdbl; \
//				if(JS_ValueToNumber(cx, *vp, &jsdbl)==JS_TRUE){ \
//					if(priv->bRef){ \
//						priv->mpI3DRot->SetRot_##C(jsdbl); \
//					}else{ \
//						priv->CreateRot(); \
//						priv->pRot->C=jsdbl; \
//					} \
//				} \
//				}

#define SET_ROT_V(C) \
				{ jsdouble jsdbl; \
				if(JS_ValueToNumber(cx, *vp, &jsdbl)==JS_TRUE){ \
					ml3DRotationVA rotva(priv->GetRot()); \
					if(priv->mpRotVA == NULL){ \
						priv->mpRotVA = MP_NEW( ml3DRotationVA); \
						*(priv->mpRotVA) = rotva; \
					} \
					priv->mpRotVA->C = jsdbl; \
					rotva.C = jsdbl; \
					ml3DRotation quat = rotva.GetQuat(); \
					if((quat.x == 0) && (quat.y == 0) && (quat.z == 0) && (quat.a == 1)) \
						quat = priv->mpRotVA->GetQuat(); \
					if(priv->bRef){ \
						priv->mpI3DRot->SetRot(quat); \
					}else{ \
						priv->CreateRot(); \
						*(priv->pRot) = quat; \
					} \
				} \
				}


///// JavaScript Set Property Wrapper
JSBool mlRotation3D::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRotation3D);
		default:
			switch(iID){
			case JSPROP_x:
				SET_ROT_V(x);
				break;
			case JSPROP_y:
				SET_ROT_V(y);
				break;
			case JSPROP_z:
				SET_ROT_V(z);
				break;
			case JSPROP_a:
				{ jsdouble jsdbl;
					if(JS_ValueToNumber(cx, *vp, &jsdbl)==JS_TRUE){
						ml3DRotationVA rotva(priv->GetRot());
						if(priv->mpRotVA == NULL){
							priv->mpRotVA = MP_NEW( ml3DRotationVA);
							*(priv->mpRotVA) = rotva;
						}
						priv->mpRotVA->a = rotva.a = jsdbl*PI/180;
						ml3DRotation quat = rotva.GetQuat();
						if((quat.x == 0) && (quat.y == 0) && (quat.z == 0) && (quat.a == 1))
							quat = priv->mpRotVA->GetQuat();
						if(priv->bRef){
							priv->mpI3DRot->SetRot(quat);
						}else{
							priv->CreateRot();
							*(priv->pRot) = quat;
						}
					} 
				}
//				SET_ROT_V(a);
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

//#define GET_ROT_V(C) \
//				{ double dVal; \
//					if(priv->bRef){ \
//						dVal=priv->mpI3DRot->GetRot().C; \
//					}else{ \
//						priv->CreateRot(); \
//						dVal=priv->pRot->C;\
//					} \
//					jsdouble* jsdbl=JS_NewDouble(cx, dVal); \
//					*vp=DOUBLE_TO_JSVAL(jsdbl); \
//				} 

#define GET_ROT_V(C) \
				{ \
					ml3DRotationVA rotva(priv->GetRot()); \
					if(priv->mpRotVA != NULL && (rotva.x == 0) && (rotva.y == 0) && (rotva.z == 0)) \
						rotva = *(priv->mpRotVA); \
					jsdouble* jsdbl=JS_NewDouble(cx, rotva.C); \
					*vp=DOUBLE_TO_JSVAL(jsdbl); \
				} 

///// JavaScript Get Property Wrapper
JSBool mlRotation3D::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRotation3D);
		default:
			switch(iID){
			case JSPROP_x:
				GET_ROT_V(x);
				break;
			case JSPROP_y:
				GET_ROT_V(y);
				break;
			case JSPROP_z:
				GET_ROT_V(z);
				break;
			case JSPROP_a:
				ml3DRotationVA rotva(priv->GetRot());
				jsdouble* jsdbl=JS_NewDouble(cx, rotva.a*180/PI);
				*vp=DOUBLE_TO_JSVAL(jsdbl);
//				GET_ROT_V(a);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

ml3DRotation mlRotation3D::GetRot(){
	if(!bRef){
		CreateRot();
		return *pRot;
	}else{
		return mpI3DRot->GetRot();
	}
}

void mlRotation3D::SetRot(ml3DRotation &aRot){
	if(!bRef){
		CreateRot();
		*pRot=aRot;
	}else{
		mpI3DRot->SetRot(aRot);
	}
}

__forceinline JSObject* CreateRotation3DJSO(JSContext *cx, ml3DRotation &rot){
	JSObject* jsoNew = mlRotation3D::newJSObject(cx);
	mlRotation3D* pNewRot = (mlRotation3D*)JS_GetPrivate(cx,jsoNew);
	pNewRot->SetRot(rot);
	return jsoNew;
}

JSBool mlRotation3D::JSFUNC_clone(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 0)
		JS_ReportWarning(cx, "Rotation3D.clone method has no arguments");
	mlRotation3D* pRot = (mlRotation3D*)JS_GetPrivate(cx,obj);
	*rval = OBJECT_TO_JSVAL(CreateRotation3DJSO(cx, pRot->GetRot()));
	return JS_TRUE;
}

JSBool mlRotation3D::JSFUNC_mult(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	bool bError = true;
	for(;;){
		if(argc == 0 || !JSVAL_IS_OBJECT(argv[0]) || JSVAL_IS_NULL(argv[0]) || JSVAL_IS_VOID(argv[0]))
			break;
		JSObject* jsoArgRot = JSVAL_TO_OBJECT(argv[0]);
		if(!mlRotation3D::IsInstance(cx, jsoArgRot))
			break;
		if(argc > 1)
			JS_ReportWarning(cx, "Rotation3D.mult method must get one Rotation3D argument");
		mlRotation3D* pRot = (mlRotation3D*)JS_GetPrivate(cx, obj);
		mlRotation3D* pArgRot = (mlRotation3D*)JS_GetPrivate(cx, jsoArgRot);
		ml3DRotation rot = pRot->GetRot();
		ml3DRotation rotArg = pArgRot->GetRot();
		ml3DRotation rotRes;
		rm::IMath3DRMML* pmath3d = GPSM(cx)->GetContext()->mpRM->GetMath3D();
		if( pmath3d)
			pmath3d->quatMult(rot, rotArg, rotRes);
		pRot->SetRot(rotRes);
//		*rval = OBJECT_TO_JSVAL(CreateRotation3DJSO(cx, rotRes));
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Rotation3D.mult method must get one Rotation3D argument");
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool mlRotation3D::JSFUNC_conjugate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 0)
		JS_ReportWarning(cx, "Rotation3D.conjugate method has no arguments");
	mlRotation3D* pRot = (mlRotation3D*)JS_GetPrivate(cx, obj);
	ml3DRotation rot = pRot->GetRot();
	rot.x = -rot.x;
	rot.y = -rot.y;
	rot.z = -rot.z;
	pRot->SetRot(rot);
//	*rval = OBJECT_TO_JSVAL(CreateRotation3DJSO(cx, rot));
	return JS_TRUE;
}

JSBool mlRotation3D::JSFUNC_blend(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	bool bError = true;
	for(;;){
		if(argc <= 1 || !JSVAL_IS_OBJECT(argv[0]) || JSVAL_IS_NULL(argv[0]) || JSVAL_IS_VOID(argv[0]))
			break;
		JSObject* jsoArgRot = JSVAL_TO_OBJECT(argv[0]);
		if(!mlRotation3D::IsInstance(cx, jsoArgRot))
			break;
		jsdouble dKoef;
		if(!JS_ValueToNumber(cx, argv[1], &dKoef))
			break;
		if(dKoef < 0.0 || dKoef > 1.0)
			break;
		if(argc > 2)
			JS_ReportWarning(cx, "Rotation3D.blend method must get Rotation3D and float arguments");
		mlRotation3D* pRot = (mlRotation3D*)JS_GetPrivate(cx, obj);
		mlRotation3D* pArgRot = (mlRotation3D*)JS_GetPrivate(cx, jsoArgRot);
		ml3DRotation rot = pRot->GetRot();
		ml3DRotation rotArg = pArgRot->GetRot();
		ml3DRotation rotRes;
// ??
		rm::IMath3DRMML*	pmath3d = GPSM(cx)->GetContext()->mpRM->GetMath3D();
		if( pmath3d)
			pmath3d->quatBlend(rot, rotArg, dKoef, rotRes);

		pRot->SetRot(rotRes);
//		*rval = OBJECT_TO_JSVAL(CreateRotation3DJSO(cx, rotRes));
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Rotation3D.blend method must get Rotation3D and float (0..1) arguments");
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool mlRotation3D::JSFUNC_normalize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 0)
		JS_ReportWarning(cx, "Rotation3D.normalize method has no arguments");
	mlRotation3D* pRot = (mlRotation3D*)JS_GetPrivate(cx, obj);
	ml3DRotation rot = pRot->GetRot();
	double dLength = sqrt(rot.x*rot.x + rot.y*rot.y + rot.z*rot.z + rot.a*rot.a);
	if( dLength != 0.0)
	{
		rot.x /= dLength;
		rot.y /= dLength;
		rot.z /= dLength;
		rot.a /= dLength;
	}
	pRot->SetRot(rot);
//	*rval = OBJECT_TO_JSVAL(CreateRotation3DJSO(cx, rot));
	return JS_TRUE;
}

void mlRotation3D::Normalize(){
	ml3DRotation rot = GetRot();
	double dLength = sqrt(rot.x*rot.x + rot.y*rot.y + rot.z*rot.z + rot.a*rot.a);
	if( dLength != 0.0)
	{
		rot.x /= dLength;
		rot.y /= dLength;
		rot.z /= dLength;
		rot.a /= dLength;
	}
	SetRot(rot);
}

}

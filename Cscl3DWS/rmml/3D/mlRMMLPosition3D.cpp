
#include "mlRMML.h"
#include <math.h>

namespace rmml {

mlPosition3D::mlPosition3D(void)
{
//	ML_INIT_3DPOSITION(pos);
	pPos=NULL;
	bRef=false;
}

JSBool mlPosition3D::AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(mlPosition3D::JSConstructor(cx, obj, argc, argv, rval) == JS_FALSE)
		return JS_FALSE;
	if(argc > 3){
		JS_ReportError(cx,"Too many arguments for Position3D constructor");
		return JS_FALSE;
	}
	if(argc > 0){
		mlPosition3D* pThis=(mlPosition3D*)JS_GetPrivate(cx,obj);
		pThis->CreatePos();
		if(argc == 1 && JSVAL_IS_OBJECT(argv[0])){
			if(argv[0] != JSVAL_NULL && argv[0] != JSVAL_VOID){
				JSObject* jsoArg = JSVAL_TO_OBJECT(argv[0]);
				if(mlPosition3D::IsInstance(cx, jsoArg)){
					mlPosition3D* pArgPos=(mlPosition3D*)JS_GetPrivate(cx,jsoArg);
					pThis->SetPos(pArgPos->GetPos());
				}
			}
		}else{
			if(argc >= 1){
				jsval v=argv[0];
				if(!JSVAL_IS_NUMBER(v)){
					JS_ReportError(cx,"Arguments of Position3D constructor must be a numbers");
					return JS_FALSE;
				}
				JS_ValueToNumber(cx,v,&(pThis->pPos->x));
			}
			if(argc >= 2){
				jsval v=argv[1];
				if(!JSVAL_IS_NUMBER(v)){
					JS_ReportError(cx,"Arguments of Position3D constructor must be a numbers");
					return JS_FALSE;
				}
				JS_ValueToNumber(cx,v,&(pThis->pPos->y));
			}
			if(argc >= 3){
				jsval v=argv[2];
				if(!JSVAL_IS_NUMBER(v)){
					JS_ReportError(cx,"Arguments of Position3D constructor must be a numbers");
					return JS_FALSE;
				}
				JS_ValueToNumber(cx,v,&(pThis->pPos->z));
			}
		}
	}
	return JS_TRUE;
}

mlPosition3D::~mlPosition3D(void){
	if(!bRef && pPos!=NULL) MP_DELETE( pPos);
}

///// JavaScript Variable Table
JSPropertySpec mlPosition3D::_JSPropertySpec[] = {
	{ "x", JSPROP_x, JSPROP_ENUMERATE, 0, 0},
	{ "y", JSPROP_y, JSPROP_ENUMERATE, 0, 0},
	{ "z", JSPROP_z, JSPROP_ENUMERATE, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlPosition3D::_JSFunctionSpec[] = {
	JSFUNC(clone,0)
	JSFUNC(add,1)
	JSFUNC(sub,1)
	JSFUNC(mult,1)	// на число или Rotation3D
	JSFUNC(div,1)
	JSFUNC(dot,1)	// скалярное произведение
	JSFUNC(cross,1)	// векторное произведение
	JSFUNC(blend,2)	// интерполяция
	JSFUNC(normalize,0)	// нормализовать
	JSFUNC(length,0)	// длина
	JSFUNC(angle,1)	// возвращает поворот в градусах между этим вектором и данным в качестве аргумента
	JSFUNC(angleSigned,2) // возвращает полный поворот в градусах относительно вектора up (по умолчанию (0, 0, 1)) между этим вектором и данным в качестве аргумента 
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Position3D,mlPosition3D,0)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDPROPENUMOP
	MLJSCLASS_ADDALTERCONSTR(AlterJSConstructor)
MLJSCLASS_IMPL_END(mlPosition3D)

#define SET_POS_V(C) \
				{ jsdouble jsdbl; \
				if(JS_ValueToNumber(cx, *vp, &jsdbl)==JS_TRUE){ \
					if(priv->bRef){ \
						priv->mpI3DPos->SetPos_##C(jsdbl); \
					}else{ \
						priv->CreatePos(); \
						priv->pPos->C=jsdbl; \
					} \
				} \
				}

///// JavaScript Set Property Wrapper
JSBool mlPosition3D::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlPosition3D);
		default:
			switch(iID){
			case JSPROP_x:
				SET_POS_V(x);
				break;
			case JSPROP_y:
				SET_POS_V(y);
				break;
			case JSPROP_z:
				SET_POS_V(z);
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

#define GET_POS_V(C) \
				{ double dVal; \
					if(priv->bRef){ \
						dVal=priv->mpI3DPos->GetPos().C; \
					}else{ \
						priv->CreatePos(); \
						dVal=priv->pPos->C;\
					} \
					jsdouble* jsdbl=JS_NewDouble(cx, dVal); \
					*vp=DOUBLE_TO_JSVAL(jsdbl); \
				} 

///// JavaScript Get Property Wrapper
JSBool mlPosition3D::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlPosition3D);
		default:
			switch(iID){
			case JSPROP_x:
				GET_POS_V(x);
				break;
			case JSPROP_y:
				GET_POS_V(y);
				break;
			case JSPROP_z:
				GET_POS_V(z);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

ml3DPosition mlPosition3D::GetPos(){
	if(!bRef){
		CreatePos();
		return *pPos;
	}else{
		return mpI3DPos->GetPos();
	}
}

void mlPosition3D::SetPos(ml3DPosition &pos){
	if(bRef){
		mpI3DPos->SetPos_x(pos.x);
		mpI3DPos->SetPos_y(pos.y);
		mpI3DPos->SetPos_z(pos.z);
	}else{
		CreatePos();
		*pPos=pos;
	}
}

//	JSFUNC(clone,0)
//	JSFUNC(add,1)
//	JSFUNC(sub,1)
//	JSFUNC(mult,1)	// на число или Rotation3D
//	JSFUNC(div,1)
//	JSFUNC(dot,1)	// скалярное произведение
//	JSFUNC(cross,1)	// векторное произведение
//	JSFUNC(blend,2)	// интерполяция
//	JSFUNC(normalize,0)	// нормализовать
//	JSFUNC(length,0)	// длина

__forceinline JSObject* CreatePosition3DJSO(JSContext *cx, ml3DPosition &pos){
	JSObject* jsoNew = mlPosition3D::newJSObject(cx);
	mlPosition3D* pNewPos = (mlPosition3D*)JS_GetPrivate(cx,jsoNew);
	pNewPos->SetPos(pos);
	return jsoNew;
}

JSBool mlPosition3D::JSFUNC_clone(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 0)
		JS_ReportWarning(cx, "Position3D.clone method has no arguments");
	mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx,obj);
	*rval = OBJECT_TO_JSVAL(CreatePosition3DJSO(cx, pPos->GetPos()));
	return JS_TRUE;
}

JSBool mlPosition3D::JSFUNC_add(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	bool bError = true;
	for(;;){
		if(argc == 0 || !JSVAL_IS_OBJECT(argv[0]) || JSVAL_IS_NULL(argv[0]) || JSVAL_IS_VOID(argv[0]))
			break;
		JSObject* jsoArgPos = JSVAL_TO_OBJECT(argv[0]);
		if(!mlPosition3D::IsInstance(cx, jsoArgPos))
			break;
		if(argc > 1)
			JS_ReportWarning(cx, "Position3D.add method must get one Position3D argument");
		mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, obj);
		mlPosition3D* pArgPos = (mlPosition3D*)JS_GetPrivate(cx, jsoArgPos);
		ml3DPosition pos = pPos->GetPos();
		ml3DPosition posArg = pArgPos->GetPos();
		pos.x += posArg.x;
		pos.y += posArg.y;
		pos.z += posArg.z;
		pPos->SetPos(pos);
//		*rval = OBJECT_TO_JSVAL(CreatePosition3DJSO(cx, pos));
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Position3D.add method must get one Position3D argument");
		return JS_FALSE;
	}
	return JS_TRUE;
}

// Проводит вычисления над текущим объектом
// a = new Position3D(3, 3, 3);
// b = new Position3D(2, 2, 2);
// a.sub(b)
// a == new Position3D(1, 1, 1)
JSBool mlPosition3D::JSFUNC_sub(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	bool bError = true;
	for(;;){
		if(argc == 0 || !JSVAL_IS_OBJECT(argv[0]) || JSVAL_IS_NULL(argv[0]) || JSVAL_IS_VOID(argv[0]))
			break;
		JSObject* jsoArgPos = JSVAL_TO_OBJECT(argv[0]);
		if(!mlPosition3D::IsInstance(cx, jsoArgPos))
			break;
		if(argc > 1)
			JS_ReportWarning(cx, "Position3D.sub method must get one Position3D argument");
		mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, obj);
		mlPosition3D* pArgPos = (mlPosition3D*)JS_GetPrivate(cx, jsoArgPos);
		ml3DPosition pos = pPos->GetPos();
		ml3DPosition posArg = pArgPos->GetPos();
		pos.x -= posArg.x;
		pos.y -= posArg.y;
		pos.z -= posArg.z;
		pPos->SetPos(pos);
//		*rval = OBJECT_TO_JSVAL(CreatePosition3DJSO(cx, pos));
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Position3D.sub method must get one Position3D argument");
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool mlPosition3D::JSFUNC_mult(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	bool bError = true;
	for(;;){
		if(argc == 0)
			break;
		if(argc > 1)
			JS_ReportWarning(cx, "Position3D.mult method must get one numerical or one Rotation3D argument");
		if(JSVAL_IS_NULL(argv[0]) || JSVAL_IS_VOID(argv[0]))
			break;
		mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, obj);
		ml3DPosition pos = pPos->GetPos();
		if(JSVAL_IS_OBJECT(argv[0])){
			JSObject* jsoArgRot = JSVAL_TO_OBJECT(argv[0]);
			if(!mlRotation3D::IsInstance(cx, jsoArgRot))
				break;
			mlRotation3D* pRot = (mlRotation3D*)JS_GetPrivate(cx, jsoArgRot);
			ml3DRotation rot = pRot->GetRot();
			ml3DPosition rotRes;
			rm::IMath3DRMML* pmath3d = GPSM(cx)->GetContext()->mpRM->GetMath3D();
			if( pmath3d)
				pmath3d->vec3MultQuat(pos, rot, rotRes);
			pPos->SetPos(rotRes);
		}else{
			jsdouble dArg;
			if(!JS_ValueToNumber(cx, argv[0], &dArg))
				break;
			pos.x *= dArg;
			pos.y *= dArg;
			pos.z *= dArg;
			pPos->SetPos(pos);
		}
//		*rval = OBJECT_TO_JSVAL(CreatePosition3DJSO(cx, pos));
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Position3D.mult method must get one numerical or one Rotation3D argument");
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool mlPosition3D::JSFUNC_div(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	bool bError = true;
	for(;;){
		if(argc == 0)
			break;
		jsdouble dArg;
		if(!JS_ValueToNumber(cx, argv[0], &dArg))
			break;
		if(argc > 1)
			JS_ReportWarning(cx, "Position3D.mult method must get one numerical argument");
		mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, obj);
		ml3DPosition pos = pPos->GetPos();
		pos.x /= dArg;
		pos.y /= dArg;
		pos.z /= dArg;
		pPos->SetPos(pos);
//		*rval = OBJECT_TO_JSVAL(CreatePosition3DJSO(cx, pos));
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Position3D.mult method must get one numerical argument");
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool mlPosition3D::JSFUNC_dot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	bool bError = true;
	for(;;){
		if(argc == 0 || !JSVAL_IS_OBJECT(argv[0]) || JSVAL_IS_NULL(argv[0]) || JSVAL_IS_VOID(argv[0]))
			break;
		JSObject* jsoArgPos = JSVAL_TO_OBJECT(argv[0]);
		if(!mlPosition3D::IsInstance(cx, jsoArgPos))
			break;
		if(argc > 1)
			JS_ReportWarning(cx, "Position3D.dot method must get one Position3D argument");
		mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, obj);
		mlPosition3D* pArgPos = (mlPosition3D*)JS_GetPrivate(cx, jsoArgPos);
		ml3DPosition pos = pPos->GetPos();
		ml3DPosition posArg = pArgPos->GetPos();
		//pos.x *= posArg.x;
		//pos.y *= posArg.y;
		//pos.z *= posArg.z;
		//pPos->SetPos(pos);
		double	dot = Dot(pos, posArg);
		jsdouble* jsdbl=JS_NewDouble(cx, dot);
		*rval = DOUBLE_TO_JSVAL(jsdbl);
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Position3D.dot method must get one Position3D argument");
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool mlPosition3D::JSFUNC_cross(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	bool bError = true;
	for(;;){
		if(argc == 0 || !JSVAL_IS_OBJECT(argv[0]) || JSVAL_IS_NULL(argv[0]) || JSVAL_IS_VOID(argv[0]))
			break;
		JSObject* jsoArgPos = JSVAL_TO_OBJECT(argv[0]);
		if(!mlPosition3D::IsInstance(cx, jsoArgPos))
			break;
		if(argc > 1)
			JS_ReportWarning(cx, "Position3D.cross method must get one Position3D argument");
		mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, obj);
		mlPosition3D* pArgPos = (mlPosition3D*)JS_GetPrivate(cx, jsoArgPos);
		ml3DPosition pos = pPos->GetPos();
		ml3DPosition posArg = pArgPos->GetPos();
		ml3DPosition posRes;
		Cross(pos, posArg, posRes);
		pPos->SetPos(posRes);
//		*rval = OBJECT_TO_JSVAL(CreatePosition3DJSO(cx, posRes));
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Position3D.cross method must get one Position3D argument");
		return JS_FALSE;
	}
	return JS_TRUE;
}

JSBool mlPosition3D::JSFUNC_blend(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	bool bError = true;
	for(;;){
		if(argc <= 1 || !JSVAL_IS_OBJECT(argv[0]) || JSVAL_IS_NULL(argv[0]) || JSVAL_IS_VOID(argv[0]))
			break;
		JSObject* jsoArgPos = JSVAL_TO_OBJECT(argv[0]);
		if(!mlPosition3D::IsInstance(cx, jsoArgPos))
			break;
		jsdouble dKoef;
		if(!JS_ValueToNumber(cx, argv[1], &dKoef))
			break;
		if(dKoef < 0.0 || dKoef > 1.0)
			break;
		if(argc > 2)
			JS_ReportWarning(cx, "Position3D.blend method must get Position3D and float arguments");
		mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, obj);
		mlPosition3D* pArgPos = (mlPosition3D*)JS_GetPrivate(cx, jsoArgPos);
		ml3DPosition pos1 = pPos->GetPos();
		ml3DPosition pos2 = pArgPos->GetPos();
		ml3DPosition posRes;

		posRes.x = pos1.x + dKoef * (pos2.x - pos1.x);
		posRes.y = pos1.y + dKoef * (pos2.y - pos1.y);
		posRes.z = pos1.z + dKoef * (pos2.z - pos1.z);
		pPos->SetPos(posRes);
//		*rval = OBJECT_TO_JSVAL(CreatePosition3DJSO(cx, posRes));
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Position3D.blend method must get Position3D and float (0..1) arguments");
		return JS_FALSE;
	}
	return JS_TRUE;
}

// возвращает нормализованный объект
// var n = new Position3D(10,0,0)
// var n1 = n.normalize();
// n1 == new Position3D(1,0,0)
JSBool mlPosition3D::JSFUNC_normalize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 0)
		JS_ReportWarning(cx, "Position3D.normalize method has no arguments");
	mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, obj);
	ml3DPosition pos = pPos->GetPos();
	double dLength = sqrt(pos.x*pos.x + pos.y*pos.y + pos.z*pos.z);
	if( dLength != 0.0)
	{
		pos.x /= dLength;
		pos.y /= dLength;
		pos.z /= dLength;
	}
//	pPos->SetPos(pos);
	JSObject* jsoNew = mlPosition3D::newJSObject(cx);
	mlPosition3D* pNewPos = (mlPosition3D*)JS_GetPrivate(cx,jsoNew);
	pNewPos->SetPos(pos);
	*rval = OBJECT_TO_JSVAL(jsoNew);
	return JS_TRUE;
}

double mlPosition3D::GetLength(){
	ml3DPosition pos = GetPos();
	return sqrt(pos.x*pos.x + pos.y*pos.y + pos.z*pos.z);
}

JSBool mlPosition3D::JSFUNC_length(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc > 0)
		JS_ReportWarning(cx, "Position3D.length method has no arguments");
	mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, obj);
	double dLength = pPos->GetLength();
	jsdouble* jsdbl=JS_NewDouble(cx, dLength);
	*rval = DOUBLE_TO_JSVAL(jsdbl);
	return JS_TRUE;
}

JSBool mlPosition3D::JSFUNC_angle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	bool bError = true;
	for(;;){
		if(argc == 0 || !JSVAL_IS_OBJECT(argv[0]) || JSVAL_IS_NULL(argv[0]) || JSVAL_IS_VOID(argv[0]))
			break;
		JSObject* jsoArgPos = JSVAL_TO_OBJECT(argv[0]);
		if(!mlPosition3D::IsInstance(cx, jsoArgPos))
			break;
		if(argc > 1)
			JS_ReportWarning(cx, "Position3D.angle method must get one Position3D argument");
		mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, obj);
		mlPosition3D* pArgPos = (mlPosition3D*)JS_GetPrivate(cx, jsoArgPos);

		double dAngle = 0.0;
		if(pPos->GetLength() != 0.0 && pArgPos->GetLength() != 0.0){
			ml3DPosition pos = pPos->GetPos();
			ml3DPosition posArg = pArgPos->GetPos();
			double arg = (pos.x * posArg.x + pos.y * posArg.y + pos.z * posArg.z) / (pPos->GetLength() * pArgPos->GetLength());
			if(arg > 0.9999999){
				dAngle = 0.0;
			}else if(arg < -0.9999999){
				dAngle = 180.0;
			}else{
				dAngle = acos(arg) * 180 / PI;
			}
		}
		jsdouble* jsdbl=JS_NewDouble(cx, dAngle);
		*rval = DOUBLE_TO_JSVAL(jsdbl);
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Position3D.angle method must get one Position3D argument");
		return JS_FALSE;
	}
	return JS_TRUE;
}


/*
function angleSigned(v1, v2, n) {
    if (typeof n == "undefined") {
        n = new Position3D(0, 0, 1);
    }
    var vcross = v1.clone();
    vcross.cross(v2);
    return Math.atan2(n.dot(vcross), v1.dot(v2)) * 57.29577951308232;
}
*/
JSBool mlPosition3D::JSFUNC_angleSigned(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	bool bError = true;
	for(;;){
		if(argc == 0 || !JSVAL_IS_OBJECT(argv[0]) || JSVAL_IS_NULL(argv[0]) || JSVAL_IS_VOID(argv[0]))
			break;
		JSObject* jsoArgPos = JSVAL_TO_OBJECT(argv[0]);
		if(!mlPosition3D::IsInstance(cx, jsoArgPos))
			break;
		mlPosition3D* pPos = (mlPosition3D*)JS_GetPrivate(cx, obj);
		mlPosition3D* pArgPos = (mlPosition3D*)JS_GetPrivate(cx, jsoArgPos);
		mlPosition3D* pArgUp = NULL;
		if (argc > 1) {
			JSObject* jsoArgUp = JSVAL_TO_OBJECT(argv[1]);
			if(!mlPosition3D::IsInstance(cx, jsoArgUp))
				break;
			pArgUp = (mlPosition3D*)JS_GetPrivate(cx, jsoArgUp);
		}
		if(argc > 3)
			JS_ReportWarning(cx, "Position3D.angleSigned method must get one or two Position3D argument");		

		double dAngle = 0.0;
		if(pPos->GetLength() != 0.0 && pArgPos->GetLength() != 0.0 && (!pArgUp || pArgUp->GetLength() != 0.0)){
			ml3DPosition pos = pPos->GetPos();
			ml3DPosition posArg = pArgPos->GetPos();
			ml3DPosition posUp;
			if (pArgUp) {
				posUp = pArgUp->GetPos();
			} else {
				posUp.x = 0;
				posUp.y = 0;
				posUp.z = 1;
			}
			
			ml3DPosition vCross;
			Cross(pos, posArg, vCross);
			dAngle = atan2(Dot(posUp, vCross), Dot(pos, posArg)) * 57.29577951308232;
		}
		jsdouble* jsdbl=JS_NewDouble(cx, dAngle);
		*rval = DOUBLE_TO_JSVAL(jsdbl);
		bError = false;
		break;
	}
	if(bError){
		JS_ReportError(cx, "Position3D.angleSigned method must get one or two Position3D argument");
		return JS_FALSE;
	}
	return JS_TRUE;
}

void mlPosition3D::Cross(ml3DPosition const& v1, ml3DPosition const& v2, ml3DPosition & ret)
{	
	ret.x = v1.y * v2.z - v1.z * v2.y;
	ret.y = v1.z * v2.x - v1.x * v2.z;
	ret.z = v1.x * v2.y - v1.y * v2.x;
}

double mlPosition3D::Dot(ml3DPosition const& v1, ml3DPosition const& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

}

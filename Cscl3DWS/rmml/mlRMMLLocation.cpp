
#include "mlRMML.h"

namespace rmml {

mlLocation3D::mlLocation3D(void)
{
	// ??
}

JSBool mlLocation3D::AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(mlLocation3D::JSConstructor(cx, obj, argc, argv, rval) == JS_FALSE)
		return JS_FALSE;
	// ??
	return JS_TRUE;
}

mlLocation3D::~mlLocation3D(void){
	// ??
}

///// JavaScript Variable Table
JSPropertySpec mlLocation3D::_JSPropertySpec[] = {
//	{ "x", JSPROP_x, JSPROP_ENUMERATE, 0, 0},
//	{ "y", JSPROP_y, JSPROP_ENUMERATE, 0, 0},
//	{ "z", JSPROP_z, JSPROP_ENUMERATE, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlLocation3D::_JSFunctionSpec[] = {
//	JSFUNC(clone,0)
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Location,mlLocation3D,0)
	MLJSCLASS_ADDPROPFUNC
//	MLJSCLASS_ADDPROPENUMOP
	MLJSCLASS_ADDALTERCONSTR(AlterJSConstructor)
MLJSCLASS_IMPL_END(mlLocation3D)

///// JavaScript Set Property Wrapper
JSBool mlLocation3D::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlLocation3D);
		default:
/*			switch(iID){

			case JSPROP_x:
				SET_POS_V(x);
				break;
			case JSPROP_y:
				SET_POS_V(y);
				break;
			case JSPROP_z:
				SET_POS_V(z);
				break;

			}*/
			break;
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlLocation3D::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlLocation3D);
		default:
/*			switch(iID){

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
			*/
			break;
	GET_PROPS_END;
	return JS_TRUE;
}

const wchar_t* mlLocation3D::makeURL(){
	// ??
	return L"";
}

// выдает положение единым URL-ом
const wchar_t* mlLocation3D::GetLocationURL(){
	return makeURL();
}

}

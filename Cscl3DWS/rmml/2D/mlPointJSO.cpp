
#include "mlRMML.h"
#include "mlSPParser.h"
#include "mlPointJSO.h"
#include "config/prolog.h"

namespace rmml {

JSBool mlPointJSO::AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(mlPointJSO::JSConstructor(cx, obj, argc, argv, rval) == JS_FALSE)
		return JS_FALSE;
	if(argc == 0)
		return JS_TRUE;
	if(argc > 2){
		JS_ReportError(cx,"Too many arguments for Point constructor");
		return JS_FALSE;
	}
	mlPointJSO* pThis=(mlPointJSO*)JS_GetPrivate(cx,obj);
	if(argc == 1 && JSVAL_IS_OBJECT(argv[0])){
		if(argv[0] != JSVAL_NULL && argv[0] != JSVAL_VOID){
			JSObject* jsoArg = JSVAL_TO_OBJECT(argv[0]);
			if(mlPointJSO::IsInstance(cx, jsoArg)){
				mlPointJSO* pArgColor=(mlPointJSO*)JS_GetPrivate(cx,jsoArg);
				pThis->mPoint = pArgColor->mPoint;
			}
		}
	}else if(argc == 1 && JSVAL_IS_STRING(argv[0])){
		JSString* jss = JSVAL_TO_STRING(argv[0]);
		wchar_t* jsc = wr_JS_GetStringChars(jss);
		mlPoint Point;
		mlPropParseInfo mlPointParseInfo[]={
			{L"x",MLPT_INT,&(Point.x)},
			{L"y",MLPT_INT,&(Point.y)},
			{L"",MLPT_UNKNOWN,NULL}
		};
		if(ParseStruct(cx, (const wchar_t *&) jsc, mlPointParseInfo))
			pThis->mPoint = Point;
	}else{
		jsdouble adArgs[2];
		int iValidArgs = 0;
		if(argc >= 1){
			jsval v=argv[0];
			if(!JS_ValueToNumber(cx,v,&adArgs[0])){
				JS_ReportError(cx,"Arguments of Point constructor must be a numbers");
				return JS_FALSE;
			}
			iValidArgs++;
		}
		if(argc >= 2){
			jsval v=argv[1];
			if(!JS_ValueToNumber(cx,v,&adArgs[1])){
				JS_ReportError(cx,"Arguments of Point constructor must be a numbers");
				return JS_FALSE;
			}
			iValidArgs++;
		}
		for(int ii=0; ii<iValidArgs; ii++){
			int iVal = adArgs[ii];
			switch(ii){
			case 0:	pThis->mPoint.x = iVal; break;
			case 1:	pThis->mPoint.y = iVal; break;
			}
		}
	}
	return JS_TRUE;
}

///// JavaScript Variable Table
JSPropertySpec mlPointJSO::_JSPropertySpec[] = {
	{ "x", JSPROP_x, JSPROP_ENUMERATE, 0, 0},
	{ "y", JSPROP_y, JSPROP_ENUMERATE, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlPointJSO::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Point,mlPointJSO,0)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDPROPENUMOP
	MLJSCLASS_ADDALTERCONSTR(AlterJSConstructor)
MLJSCLASS_IMPL_END(mlPointJSO)

#define SET_POINT_V(C) \
				{ jsdouble jsdbl; \
				if(JS_ValueToNumber(cx, *vp, &jsdbl)==JS_TRUE){ \
					priv->NotifyChangeListener(); \
					priv->mpPoint->C = (int)jsdbl; \
				} \
				}

///// JavaScript Set Property Wrapper
JSBool mlPointJSO::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlPointJSO);
		default:
			switch(iID){
			case JSPROP_x:
				SET_POINT_V(x);
				break;
			case JSPROP_y:
				SET_POINT_V(y);
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

#define GET_POINT_V(C) \
				{ \
					*vp=INT_TO_JSVAL(priv->mpPoint->C); \
				} 

JSBool mlPointJSO::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlPointJSO);
		default:
			switch(iID){
			case JSPROP_x:
				GET_POINT_V(x);
				break;
			case JSPROP_y:
				GET_POINT_V(y);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

}

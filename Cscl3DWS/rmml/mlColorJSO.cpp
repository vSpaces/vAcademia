
#include "mlRMML.h"
#include "mlSPParser.h"
#include "mlColorJSO.h"

namespace rmml {

JSBool mlColorJSO::AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(mlColorJSO::JSConstructor(cx, obj, argc, argv, rval)==JS_FALSE)
		return JS_FALSE;
	if(argc == 0)
		return JS_TRUE;
	if(argc > 4){
		JS_ReportError(cx,"Too many arguments for Color constructor");
		return JS_FALSE;
	}
	mlColorJSO* pThis=(mlColorJSO*)JS_GetPrivate(cx,obj);
	if(argc == 1 && JSVAL_IS_OBJECT(argv[0])){
		if(argv[0] != JSVAL_NULL && argv[0] != JSVAL_VOID){
			JSObject* jsoArg = JSVAL_TO_OBJECT(argv[0]);
			if(mlColorJSO::IsInstance(cx, jsoArg)){
				mlColorJSO* pArgColor=(mlColorJSO*)JS_GetPrivate(cx,jsoArg);
				pThis->mColor = pArgColor->mColor;
			}
		}
	}else if(argc == 1 && JSVAL_IS_STRING(argv[0])){
		JSString* jss = JSVAL_TO_STRING(argv[0]);
		jschar* jsc = JS_GetStringChars(jss);
		mlColor color;
		if(ParseColor(cx, (const wchar_t*&)jsc, color))
			pThis->mColor = color;
	}else{
		jsdouble adArgs[4];
		int iValidArgs = 0;
		if(argc >= 1){
			jsval v=argv[0];
			if(!JS_ValueToNumber(cx,v,&adArgs[0])){
				JS_ReportError(cx,"Arguments of Color constructor must be a numbers");
				return JS_FALSE;
			}
			iValidArgs++;
		}
		if(argc >= 2){
			jsval v=argv[1];
			if(!JS_ValueToNumber(cx,v,&adArgs[1])){
				JS_ReportError(cx,"Arguments of Color constructor must be a numbers");
				return JS_FALSE;
			}
			iValidArgs++;
		}
		if(argc >= 3){
			jsval v=argv[2];
			if(!JS_ValueToNumber(cx,v,&adArgs[2])){
				JS_ReportError(cx,"Arguments of Color constructor must be a numbers");
				return JS_FALSE;
			}
			iValidArgs++;
		}
		if(argc >= 4){
			jsval v=argv[3];
			if(!JS_ValueToNumber(cx,v,&adArgs[3])){
				JS_ReportError(cx,"Arguments of Color constructor must be a numbers");
				return JS_FALSE;
			}
			iValidArgs++;
		}
		// если есть аргумент, который > 1, значит [0..255]
		bool bFloat = true;
		int ii;
		for(ii=0; ii<iValidArgs; ii++){
			if(adArgs[ii] > 1.0){
				bFloat = false;
				break;
			}
		}
		for(ii=0; ii<iValidArgs; ii++){
			unsigned char ucVal;
			if(bFloat) // [0.0 .. 1.0]
				ucVal = (unsigned char)(adArgs[ii]*255.0);
			else
				ucVal = (unsigned char)(adArgs[ii]);
			switch(ii){
			case 0:	pThis->mColor.r = ucVal; break;
			case 1:	pThis->mColor.g = ucVal; break;
			case 2:	pThis->mColor.b = ucVal; break;
			case 3:	pThis->mColor.a = ucVal; break;
			}
		}
	}
	return JS_TRUE;
}

///// JavaScript Variable Table
JSPropertySpec mlColorJSO::_JSPropertySpec[] = {
	{ "r", JSPROP_r, JSPROP_ENUMERATE, 0, 0},
	{ "g", JSPROP_g, JSPROP_ENUMERATE, 0, 0},
	{ "b", JSPROP_b, JSPROP_ENUMERATE, 0, 0},
	{ "a", JSPROP_a, JSPROP_ENUMERATE, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlColorJSO::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Color,mlColorJSO,0)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDPROPENUMOP
	MLJSCLASS_ADDALTERCONSTR(AlterJSConstructor)
MLJSCLASS_IMPL_END(mlColorJSO)

#define SET_COLOR_V(C) \
				{ jsdouble jsdbl; \
				if(JS_ValueToNumber(cx, *vp, &jsdbl)==JS_TRUE){ \
					priv->NotifyChangeListener(); \
					priv->mpColor->C = (unsigned char)jsdbl; \
				} \
				}

///// JavaScript Set Property Wrapper
JSBool mlColorJSO::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlColorJSO);
		default:
			switch(iID){
			case JSPROP_r:
				SET_COLOR_V(r);
				break;
			case JSPROP_g:
				SET_COLOR_V(g);
				break;
			case JSPROP_b:
				SET_COLOR_V(b);
				break;
			case JSPROP_a:
				SET_COLOR_V(a);
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

#define GET_COLOR_V(C) \
				{ \
					*vp=INT_TO_JSVAL(priv->mpColor->C); \
				} 

JSBool mlColorJSO::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlColorJSO);
		default:
			switch(iID){
			case JSPROP_r:
				GET_COLOR_V(r);
				break;
			case JSPROP_g:
				GET_COLOR_V(g);
				break;
			case JSPROP_b:
				GET_COLOR_V(b);
				break;
			case JSPROP_a:
				GET_COLOR_V(a);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

mlColor mlColorJSO::GetColor(){
	return *mpColor;
}

void mlColorJSO::SetColor(mlColor &aColor){
	mColor = aColor;
}

}

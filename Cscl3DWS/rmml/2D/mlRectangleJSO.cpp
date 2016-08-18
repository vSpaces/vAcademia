
#include "mlRMML.h"
#include "mlSPParser.h"
#include "mlRectangleJSO.h"
#include "config/prolog.h"

namespace rmml {

JSBool mlRectangleJSO::AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(mlRectangleJSO::JSConstructor(cx, obj, argc, argv, rval) == JS_FALSE)
		return JS_FALSE;
	if(argc == 0)
		return JS_TRUE;
	if(argc > 4){
		JS_ReportError(cx,"Too many arguments for Rectangle constructor");
		return JS_FALSE;
	}
	mlRectangleJSO* pThis=(mlRectangleJSO*)JS_GetPrivate(cx,obj);
	if(argc == 1 && JSVAL_IS_OBJECT(argv[0])){
		if(argv[0] != JSVAL_NULL && argv[0] != JSVAL_VOID){
			JSObject* jsoArg = JSVAL_TO_OBJECT(argv[0]);
			if(mlRectangleJSO::IsInstance(cx, jsoArg)){
				mlRectangleJSO* pArgColor=(mlRectangleJSO*)JS_GetPrivate(cx,jsoArg);
				pThis->mRect = pArgColor->mRect;
			}
		}
	}else if(argc == 1 && JSVAL_IS_STRING(argv[0])){
		JSString* jss = JSVAL_TO_STRING(argv[0]);
		wchar_t* jsc = wr_JS_GetStringChars(jss);
		mlRect rect;
		mlPropParseInfo mlRectParseInfo[]={
			{L"left",MLPT_INT,&(rect.left)},
			{L"top",MLPT_INT,&(rect.top)},
			{L"right",MLPT_INT,&(rect.right)},
			{L"bottom",MLPT_INT,&(rect.bottom)},
			{L"",MLPT_UNKNOWN,NULL}
		};
		if(ParseStruct(cx, (const wchar_t *&) jsc, mlRectParseInfo))
			pThis->mRect = rect;
	}else{
		jsdouble adArgs[4];
		int iValidArgs = 0;
		if(argc >= 1){
			jsval v=argv[0];
			if(!JS_ValueToNumber(cx,v,&adArgs[0])){
				JS_ReportError(cx,"Arguments of Rectangle constructor must be a numbers");
				return JS_FALSE;
			}
			iValidArgs++;
		}
		if(argc >= 2){
			jsval v=argv[1];
			if(!JS_ValueToNumber(cx,v,&adArgs[1])){
				JS_ReportError(cx,"Arguments of Rectangle constructor must be a numbers");
				return JS_FALSE;
			}
			iValidArgs++;
		}
		if(argc >= 3){
			jsval v=argv[2];
			if(!JS_ValueToNumber(cx,v,&adArgs[2])){
				JS_ReportError(cx,"Arguments of Rectangle constructor must be a numbers");
				return JS_FALSE;
			}
			iValidArgs++;
		}
		if(argc >= 4){
			jsval v=argv[3];
			if(!JS_ValueToNumber(cx,v,&adArgs[3])){
				JS_ReportError(cx,"Arguments of Rectangle constructor must be a numbers");
				return JS_FALSE;
			}
			iValidArgs++;
		}
		for(int ii=0; ii<iValidArgs; ii++){
			int iVal = adArgs[ii];
			switch(ii){
			case 0:	pThis->mRect.left = iVal; break;
			case 1:	pThis->mRect.top = iVal; break;
			case 2:	pThis->mRect.right = iVal; break;
			case 3:	pThis->mRect.bottom = iVal; break;
			}
		}
	}
	return JS_TRUE;
}

///// JavaScript Variable Table
JSPropertySpec mlRectangleJSO::_JSPropertySpec[] = {
	{ "left", JSPROP_left, JSPROP_ENUMERATE, 0, 0},
	{ "top", JSPROP_top, JSPROP_ENUMERATE, 0, 0},
	{ "right", JSPROP_right, JSPROP_ENUMERATE, 0, 0},
	{ "bottom", JSPROP_bottom, JSPROP_ENUMERATE, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRectangleJSO::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Rectangle,mlRectangleJSO,0)
	MLJSCLASS_ADDPROPFUNC
	MLJSCLASS_ADDPROPENUMOP
	MLJSCLASS_ADDALTERCONSTR(AlterJSConstructor)
MLJSCLASS_IMPL_END(mlRectangleJSO)

#define SET_RECT_V(C) \
				{ jsdouble jsdbl; \
				if(JS_ValueToNumber(cx, *vp, &jsdbl)==JS_TRUE){ \
					priv->NotifyChangeListener(); \
					priv->mpRect->C = (int)jsdbl; \
				} \
				}

///// JavaScript Set Property Wrapper
JSBool mlRectangleJSO::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRectangleJSO);
		default:
			switch(iID){
			case JSPROP_left:
				SET_RECT_V(left);
				break;
			case JSPROP_top:
				SET_RECT_V(top);
				break;
			case JSPROP_right:
				SET_RECT_V(right);
				break;
			case JSPROP_bottom:
				SET_RECT_V(bottom);
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

#define GET_RECT_V(C) \
				{ \
					*vp=INT_TO_JSVAL(priv->mpRect->C); \
				} 

JSBool mlRectangleJSO::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRectangleJSO);
		default:
			switch(iID){
			case JSPROP_left:
				GET_RECT_V(left);
				break;
			case JSPROP_top:
				GET_RECT_V(top);
				break;
			case JSPROP_right:
				GET_RECT_V(right);
				break;
			case JSPROP_bottom:
				GET_RECT_V(bottom);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

}

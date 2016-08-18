#include "mlRMML.h"
#include "jsapi.h"
#include "jsapi_wrappers.h"

JSBool wr_JS_EvaluateUCScript(JSContext *cx, JSObject *obj,
					const wchar_t *chars, uintN length,
					const char *filename, uintN lineno,
					jsval *rval)
{
	return JS_EvaluateUCScript(cx, obj, (const jschar*)chars, length, filename, lineno, rval);
}

wchar_t* wr_JS_GetStringChars(JSString *str)
{
	return (wchar_t*)JS_GetStringChars(str);
}

JSString* wr_JS_NewUCStringCopyZ(JSContext *cx, const wchar_t *s)
{
	return JS_NewUCStringCopyZ( cx, (const jschar *) s);
}

JSString* wr_JS_NewUCStringCopyN(JSContext *cx, const wchar_t *s, size_t n)
{
	return JS_NewUCStringCopyN(cx, (const jschar *) s, n);
}

JSBool wr_JS_DefineUCProperty(JSContext *cx, JSObject *obj,
					const wchar_t *name, size_t namelen, jsval value,
					JSPropertyOp getter, JSPropertyOp setter,
					uintN attrs)
{
	return JS_DefineUCProperty(cx, obj, (const jschar *) name, namelen, value, getter, setter, attrs);
}

JSBool wr_JS_GetUCProperty(JSContext *cx, JSObject *obj,
				 const wchar_t *name, size_t namelen,
				 jsval *vp)
{
	return JS_GetUCProperty(cx, obj, (const jschar *) name, namelen, vp);
}

JSBool wr_JS_SetUCProperty(JSContext *cx, JSObject *obj,
				 const wchar_t *name, size_t namelen,
				 jsval *vp)
{
	return JS_SetUCProperty(cx, obj, (const jschar *) name, namelen, vp);
}

JSBool wr_JS_DeleteUCProperty2(JSContext *cx, JSObject *obj,
					 const wchar_t *name, size_t namelen,
					 jsval *rval)
{
	return JS_DeleteUCProperty2(cx, obj, (const jschar *) name, namelen, rval);}
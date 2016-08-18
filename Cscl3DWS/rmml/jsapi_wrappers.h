#ifndef __JSAPI_WRAPPERS_H_
#define __JSAPI_WRAPPERS_H_

JSBool wr_JS_EvaluateUCScript(JSContext *cx, JSObject *obj,
					const wchar_t *chars, uintN length,
					const char *filename, uintN lineno,
					jsval *rval);

wchar_t* wr_JS_GetStringChars(JSString *str);

JSBool	wr_JS_SetUCProperty(JSContext *cx, JSObject *obj,
				 const wchar_t* name, size_t namelen,
				 jsval *vp);

JSString* wr_JS_NewUCStringCopyZ(JSContext *cx, const wchar_t *s);

JSBool wr_JS_GetUCProperty(JSContext *cx, JSObject *obj,
				 const wchar_t *name, size_t namelen,
				 jsval *vp);

JSBool wr_JS_DefineUCProperty(JSContext *cx, JSObject *obj,
					const wchar_t *name, size_t namelen, jsval value,
					JSPropertyOp getter, JSPropertyOp setter,
					uintN attrs);

JSString* wr_JS_NewUCStringCopyN(JSContext *cx, const wchar_t *s, size_t n);

JSBool wr_JS_DeleteUCProperty2(JSContext *cx, JSObject *obj,
					 const wchar_t *name, size_t namelen,
					 jsval *rval);

#endif
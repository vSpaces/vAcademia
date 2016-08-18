#pragma once

#include "jsapi.h"
#include <string>

namespace rmml{

typedef std::wstring (*Func_RefineResPath)(std::wstring aXPath,const wchar_t* apwcResPath);

class mlBuildEvalCtxt
{
//	JSRuntime *rt;
	JSContext *cx;
	JSObject *glob;
	MP_WSTRING mwsRes;
	MP_WSTRING mwsXMLFullPath;
	Func_RefineResPath mpRefineResPathFunc;
public:
	mlBuildEvalCtxt(JSRuntime* rt);
	~mlBuildEvalCtxt();
	void SetRefineResPathFunc(Func_RefineResPath apRefineResPathFunc){ 
		mpRefineResPathFunc = apRefineResPathFunc;
	}
	static JSBool mlBuildEvalCtxt::evGetFullPath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

	void DefineConstant(const wchar_t* apwcName, const wchar_t* apwcValue);
	const wchar_t* Eval(const wchar_t* apwcExpr);
	jsval Eval(JSContext* aDestCtx, const wchar_t* apwcExpr);
	void SetXMLFullPath(const wchar_t* apwcXMLFullPath);
	void CopyAllConstantsTo(JSContext* aCtx);
	void DestroyCtxt();
	void Clear();
};

}

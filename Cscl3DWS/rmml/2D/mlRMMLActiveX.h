#pragma once

namespace rmml {

/**
 * Класс элмента RMML, загружающего и отображающего 
 * ActiveX Control
 */

omsresult CallExternal(mlRMMLElement* apMLEl, const wchar_t* apProcName, int aiArgC, 
	ML_VARIANT* pvarArgs, ML_VARIANT* varRes, bool abEvent);

class mlRMMLActiveX :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlRMMLVisible,
					public mlIActiveX
{
friend class mlRMMLBrowser;
friend class mlRMMLFlash;
friend class mlRMMLQT;
	typedef std::vector<mlSynchData*> VSynchData;
	typedef std::vector<mlSynchData*>::iterator VISynchData;

	VSynchData mvSynchData;
public:
	mlRMMLActiveX(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLActiveX(void);
MLJSCLASS_DECL
private:
	JSFUNC_DECL(call)
	JSFUNC_DECL(set)
	JSFUNC_DECL(get)

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	mlresult Update(const __int64 alTime);
	void SaveDataForSynch(mlSynchData* apData);
	mlSynchData* GetDataForSynch();

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация  mlIVisible
MLIVISIBLE_IMPL

// реализация  mlIActiveX
public:
	mlIActiveX* GetIActiveX(){ return this; }
	omsresult CallExternal(const wchar_t* apProcName, int aiArgC, 
						   ML_VARIANT* pvarArgs, ML_VARIANT* varRes, bool abEvent);
};


__forceinline void VariantToJSVal(JSContext *cx, ML_VARIANT &avIn, jsval *rval){
	if(rval == NULL) return;
	switch(avIn.vt){
	case MLVT_UI1: *rval = BOOLEAN_TO_JSVAL(avIn.bVal); break;
	case MLVT_I2: *rval = INT_TO_JSVAL(avIn.iVal); break;
	case MLVT_I4: *rval = INT_TO_JSVAL(avIn.lVal); break;
	case MLVT_R4: JS_NewDoubleValue(cx, avIn.fltVal, rval); break;
	case MLVT_R8: JS_NewDoubleValue(cx, avIn.dblVal, rval); break;
	case MLVT_BOOL: *rval = BOOLEAN_TO_JSVAL(avIn.boolVal); break;
		//	case MLVT_ERROR: scode
		//	case MLVT_CY: cyVal
		//	case MLVT_DATE: date
	case MLVT_BSTR: *rval = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)avIn.bstrVal)); break;
		//	case VT_UNKNOWN. punkVal
		//	case VT_DISPATCH. pdispVal
		//	case VT_ARRAY | *. parray
		//	case VT_BYREF | VT_UI1. pbVal
		//	case VT_BYREF | VT_I2. piVal
		//	case VT_BYREF | VT_I4. plVal
		//	case VT_BYREF | VT_R4. pfltVal 
		//	case VT_BYREF | VT_R8. pdblVal 
		//	case VT_BYREF | VT_BOOL. pboolVal 
		//	case VT_BYREF | VT_ERROR. pscode 
		//	case VT_BYREF | VT_CY. pcyVal 
		//	case VT_BYREF | VT_DATE. pdate 
		//	case VT_BYREF | VT_BSTR. pbstrVal 
		//	case VT_BYREF | VT_UNKNOWN. ppunkVal 
		//	case VT_BYREF | VT_DISPATCH. ppdispVal 
		//	case VT_ARRAY | *. pparray  
		//	case VT_BYREF | VT_VARIANT. pvarVal 
	}
}

__forceinline void JSValToMLVariant(ML_VARIANT* pArg, jsval vArg){
	if (JSVAL_IS_INT(vArg)){
		pArg->vt = MLVT_INT;
		pArg->intVal = JSVAL_TO_INT(vArg);
	}else if (JSVAL_IS_BOOLEAN(vArg)){
		pArg->vt = MLVT_BOOL;
		pArg->boolVal = JSVAL_TO_BOOLEAN(vArg);
	}else if (JSVAL_IS_DOUBLE(vArg)){
		pArg->vt = MLVT_R8;
		pArg->dblVal = *JSVAL_TO_DOUBLE(vArg);
	}else if (JSVAL_IS_STRING(vArg)){
		pArg->vt = MLVT_BSTR;
		pArg->bstrVal = (wchar_t*)JS_GetStringChars(JSVAL_TO_STRING(vArg));
	}else{
		pArg->vt = MLVT_EMPTY;
	}
}

}
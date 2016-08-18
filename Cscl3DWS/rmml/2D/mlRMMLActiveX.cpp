#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml {

mlRMMLActiveX::mlRMMLActiveX()
{
	mType=MLMT_ACTIVEX;
	mRMMLType=MLMT_ACTIVEX;
}

mlRMMLActiveX::~mlRMMLActiveX(void){
	omsContext* pContext=GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLActiveX::~mlRMMLActiveX");
	if(!PMO_IS_NULL(mpMO))
		pContext->mpRM->DestroyMO(this);
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLActiveX::_JSPropertySpec[] = {
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLActiveX::_JSFunctionSpec[] = {
	JSFUNC(call,10)
	JSFUNC(set,2)
	JSFUNC(get,2)
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(ActiveX,mlRMMLActiveX,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROTO(mlRMMLVisible)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLActiveX)

///// JavaScript Set Property Wrapper
JSBool mlRMMLActiveX::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLActiveX);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		SET_PROTO_PROP(mlRMMLVisible);
		default:;
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLActiveX::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLActiveX);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		GET_PROTO_PROP(mlRMMLVisible);
		default:;
	GET_PROPS_END;
	return JS_TRUE;
}

// реализация mlRMMLElement
mlresult mlRMMLActiveX::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, mpMO==NULL,"mlRMMLActiveX::CreateMedia");
	// set styles
	//
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLActiveX::CreateMedia");

#ifndef CSCL
	// определим что же за тип ActiveX мы будем создавать
	mlRMMLLoadable *pLoadable = GetLoadable();
	if (pLoadable){
		// получить рписание источника данных
		// и по расширению определить тип ActiveX объекта
		const wchar_t *src = pLoadable->GetSrc();

		wchar_t drive[_MAX_DRIVE] = L"",
				dir[_MAX_DIR]     = L"",
				fname[_MAX_FNAME] = L"",
				ext[_MAX_EXT]     = L"";

		_wsplitpath(src, drive, dir, fname, ext);

		if (wstring(L".swf") == ext
			|| wstring(L".mfp") == ext
			|| wstring(L".spl") == ext)
			mType = MLMT_FLASH;
		else if (wstring(L".html") == ext
			|| wstring(L".htm") == ext
			|| wstring(L".shtml") == ext)
			mType = MLMT_BROWSER;
	}
#endif

	amcx->mpRM->CreateMO(this);
	if(mpMO == NULL) 
		return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLActiveX::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. 
		// ??
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLActiveX::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLActiveX* pNewEL=(mlRMMLActiveX*)GET_RMMLEL(mcx, mlRMMLActiveX::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}

/*
inline void VariantToJSVal(JSContext *cx, ML_VARIANT &avIn, jsval *rval){
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
	case MLVT_BSTR: *rval = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, avIn.bstrVal)); break;
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

inline void JSValToMLVariant(ML_VARIANT* pArg, jsval vArg){
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
		pArg->bstrVal = JS_GetStringChars(JSVAL_TO_STRING(vArg));
	}else{
		pArg->vt = MLVT_EMPTY;
	}
}
*/

JSBool mlRMMLActiveX::JSFUNC_call(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if (argc < 1) return JS_FALSE;
	if(!JSVAL_IS_STRING(argv[0])) return JS_FALSE;
	mlRMMLActiveX* pActiveX=(mlRMMLActiveX*)JS_GetPrivate(cx, obj);
	if(PMO_IS_NULL(pActiveX->mpMO)) return JS_FALSE;
	moIActiveX* pmoIActiveX=pActiveX->mpMO->GetIActiveX();
	wchar_t* jscProcName=wr_JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
	int iArgC=argc-1;
	ML_VARIANT* pvarArgs=NULL;
	if(iArgC > 0){
		pvarArgs = MP_NEW_ARR( ML_VARIANT, iArgC);
		for(int ii=0; ii<iArgC; ii++){
			jsval vArg=argv[1+ii];
			ML_VARIANT* pArg=&pvarArgs[ii];
			JSValToMLVariant(pArg, vArg);
		}
	}
	ML_VARIANT varRes;
	omsresult res=pmoIActiveX->Call(jscProcName,iArgC,pvarArgs,&varRes);
	if(OMS_FAILED(res)){
		JS_ReportError(cx,"ActiveX procedure (%S) call failed",jscProcName);
	}
	if(pvarArgs!=NULL){
		// ??
		MP_DELETE_ARR( pvarArgs);
	}
	VariantToJSVal(cx, varRes, rval);
	return JS_TRUE;
}

JSBool mlRMMLActiveX::JSFUNC_set(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if (argc < 2) return JS_FALSE;
	if(!JSVAL_IS_STRING(argv[0])) return JS_FALSE;
	mlRMMLActiveX* pActiveX=(mlRMMLActiveX*)JS_GetPrivate(cx, obj);
	if(PMO_IS_NULL(pActiveX->mpMO)) return JS_FALSE;
	moIActiveX* pmoIActiveX=pActiveX->mpMO->GetIActiveX();
	if (pmoIActiveX == NULL) return JS_FALSE;

	wchar_t* jscPropName=wr_JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
	ML_VARIANT vVar;
	JSValToMLVariant(&vVar, argv[1]);
	omsresult res=pmoIActiveX->Set(jscPropName,&vVar);
	if(OMS_FAILED(res)){
		JS_ReportError(cx,"ActiveX proprty (%s) set failed",cLPCSTR(jscPropName));
	}
	return JS_TRUE;
}

JSBool mlRMMLActiveX::JSFUNC_get(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	CHECK_DONT_IMPL
	if (argc < 1) return JS_FALSE;
	if(!JSVAL_IS_STRING(argv[0])) return JS_FALSE;
	mlRMMLActiveX* pActiveX=(mlRMMLActiveX*)JS_GetPrivate(cx, obj);
	if(PMO_IS_NULL(pActiveX->mpMO)) return JS_FALSE;
	moIActiveX* pmoIActiveX=pActiveX->mpMO->GetIActiveX();
	if (pmoIActiveX == NULL) return JS_FALSE;

	wchar_t* jscPropName=wr_JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
	ML_VARIANT varRes;
	omsresult res=pmoIActiveX->Get(jscPropName,&varRes);
	if(OMS_FAILED(res)){
		JS_ReportError(cx,"ActiveX proprty (%s) get failed",cLPCSTR(jscPropName));
	}
	VariantToJSVal(cx, varRes, rval);
	return JS_TRUE;
}

omsresult mlRMMLActiveX::CallExternal(const wchar_t* apProcName, int aiArgC, 
									  ML_VARIANT* pvarArgs, ML_VARIANT* varRes, bool abEvent){
	return rmml::CallExternal(this, apProcName, aiArgC, pvarArgs, varRes, abEvent);
}

omsresult CallExternal(mlRMMLElement* apMLEl, const wchar_t* apProcName, int aiArgC, 
									  ML_VARIANT* pvarArgs, ML_VARIANT* varRes, bool abEvent){
	if(apProcName==NULL) return OMS_ERROR_INVALID_ARG;
	if(varRes == NULL){ //  && !GPSM(apMLEl->mcx)->Updating()  // Tandy: комментарий в mlRMMLBrowser::CallExternal (OMS)
		// помещаем вызов в очередь
		mlSynchData Data;
		Data.setJSContext(apMLEl->mcx);
		GPSM((apMLEl->mcx))->SaveInternalPath(Data, apMLEl);
		Data << apProcName;
		Data << abEvent;
		Data << (unsigned char)aiArgC;
		if(aiArgC>0){
			for(int ij=0; ij<aiArgC; ij++){
				jsval vArg = JSVAL_NULL;
				ML_VARIANT* pArg=&pvarArgs[aiArgC-1-ij]; // параметры приходят в обратном порядке
				if(pArg != NULL){
					VariantToJSVal(apMLEl->mcx, *pArg, &vArg);
				}
				Data << vArg;
			}
		}
		GPSM(apMLEl->mcx)->GetContext()->mpInput->AddInternalEvent(0x7F, true, (unsigned char*)Data.data(), Data.size());
		return OMS_OK;
	}
	// если есть такая функция
	JSFunction* jsf=NULL;
	jsval vFunc=JSVAL_NULL;
	wr_JS_GetUCProperty(apMLEl->mcx, apMLEl->mjso, apProcName, -1, &vFunc);
	if(!JSVAL_IS_REAL_OBJECT(vFunc) || !JS_ObjectIsFunction(apMLEl->mcx, JSVAL_TO_OBJECT(vFunc))){
		if(abEvent) return OMS_OK;
		mlTraceError(apMLEl->mcx, "ActiveX function %S not found\n", apProcName);
		return OMS_ERROR_NOT_IMPLEMENTED;
	}
	// то вызвать ее
	jsval* vArgs=NULL;
	if(aiArgC>0){
		vArgs = MP_NEW_ARR( jsval, aiArgC);
		for(int ij=0; ij<aiArgC; ij++){
			jsval* pvArg=&vArgs[ij];
			*pvArg=JSVAL_NULL;
			ML_VARIANT* pArg=&pvarArgs[aiArgC-1-ij]; // параметры приходят в обратном порядке
			if(pArg==NULL) continue;
			VariantToJSVal(apMLEl->mcx, *pArg, pvArg);
		}
	}
	jsval vRes = JSVAL_VOID;
//	JS_CallFunction(mcx, JS_GetParent(mcx, JS_GetFunctionObject(jsf)), jsf, aiArgC, vArgs, &v);
//	JS_CallFunction(mcx, mjso, jsf, aiArgC, vArgs, &v);
	JS_CallFunctionValue(apMLEl->mcx, apMLEl->mjso, vFunc, aiArgC, vArgs, &vRes);
	if(vArgs!=NULL)  MP_DELETE_ARR( vArgs);
	if(varRes != NULL){
		varRes->vt = MLVT_VOID;
		JSValToMLVariant(varRes, vRes);
	}
	return OMS_OK;
}

void mlRMMLActiveX::SaveDataForSynch(mlSynchData* apData){
	mlSynchData* pData = MP_NEW( mlSynchData);
	if(apData != NULL)
		pData->put(apData->data(), apData->size());
	mvSynchData.push_back(pData);
}

mlSynchData* mlRMMLActiveX::GetDataForSynch(){
	if(mvSynchData.size() == 0) return NULL;
	mlSynchData* pData = mvSynchData.front();
	mvSynchData.pop_back();
	return NULL;
}

mlresult mlRMMLActiveX::Update(const __int64 alTime){
	if(GPSM(mcx)->GetMode() == smmAuthoring) return ML_OK;
	if(mvSynchData.size() == 0) return ML_OK;
	mlSynchData* pData = GetDataForSynch();
	realSynchronize(pData);
	MP_DELETE( pData);
	return ML_OK;
}

}

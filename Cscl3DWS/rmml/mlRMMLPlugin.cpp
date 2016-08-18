#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml {

mlRMMLPlugin::mlRMMLPlugin(void):
	MP_WSTRING_INIT(uid)
{
	mType=MLMT_PLUGIN;
	mRMMLType=MLMT_PLUGIN;

	mbInMyCall = false;
}

mlRMMLPlugin::~mlRMMLPlugin(void){
	omsContext* pContext = GPSM(mcx)->GetContext();
	ML_ASSERTION(mcx, pContext->mpRM!=NULL,"mlRMMLPlugin::~mlRMMLPlugin");
	if(!PMO_IS_NULL(mpMO)){
		if(GPSM(mcx)->IsDestroying()){
			GPSM(mcx)->DestroyPluginLater(mpMO);
			mpMO = PMOV_DESTROYED;
		}else
			pContext->mpRM->DestroyMO(this);
	}
}

///// JavaScript Variable Table
JSPropertySpec mlRMMLPlugin::_JSPropertySpec[] = {
	JSPROP_RW(uid)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlRMMLPlugin::_JSFunctionSpec[] = {
	JSFUNC(call,10)
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Plugin,mlRMMLPlugin,1)
	MLJSCLASS_ADDPROTO(mlRMMLElement)
	MLJSCLASS_ADDPROTO(mlRMMLLoadable)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlRMMLPlugin)

///// JavaScript Set Property Wrapper
JSBool mlRMMLPlugin::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlRMMLPlugin);
		SET_PROTO_PROP(mlRMMLElement);
//		SET_PROTO_PROP(mlRMMLLoadable);
		default:
			switch(iID){
			case JSPROP_uid:
				if(!JSVAL_IS_STRING(*vp)){
					JS_ReportError(cx,"Plugin uid  must be a string");
					return JS_TRUE;
				}
				JSString* jssUID = JSVAL_TO_STRING(*vp);
				priv->uid = wr_JS_GetStringChars(jssUID);
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlRMMLPlugin::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlRMMLPlugin);
		GET_PROTO_PROP(mlRMMLElement);
//		GET_PROTO_PROP(mlRMMLLoadable);
		default:
			switch(iID){
			case JSPROP_uid:
				if(priv->uid.length() == 0){
					*vp = JS_GetEmptyStringValue(cx);
				}else{
					*vp = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, priv->uid.c_str()));
				}
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

// реализация mlRMMLElement
mlresult mlRMMLPlugin::CreateMedia(omsContext* amcx){
	ML_ASSERTION(mcx, mpMO==NULL,"mlRMMLPlugin::CreateMedia");
	// set styles
	//
	ML_ASSERTION(mcx, amcx->mpRM!=NULL,"mlRMMLPlugin::CreateMedia");
	amcx->mpRM->CreateMO(this);
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	mpMO->SetMLMedia(this);
	return ML_OK;
}

mlresult mlRMMLPlugin::Load(){
	if(mpMO==NULL) return ML_ERROR_NOT_INITIALIZED;
	if(!(mpMO->GetILoadable()->SrcChanged())){
		// not loaded. 
		mlTraceError(mcx, "Cannot load plugin %S\n", GetSrc());
		return ML_ERROR_FAILURE;
	}
	return ML_OK;
}

mlRMMLElement* mlRMMLPlugin::Duplicate(mlRMMLElement* apNewParent){
	mlRMMLPlugin* pNewEL=(mlRMMLPlugin*)GET_RMMLEL(mcx, mlRMMLPlugin::newJSObject(mcx));
	pNewEL->SetParent(apNewParent);
	pNewEL->GetPropsAndChildrenCopyFrom(this);
	return pNewEL;
}
/*
	va_list args=new char[20]; // char* 
	// put int
	va_list ap=args;
	*(int*)ap=10;
	ap+=_INTSIZEOF(int);
	// put double
	*(double*)ap=12.2;
	ap+=_INTSIZEOF(double);
	// put str
	*(char**)ap="stt";
	ap+=_INTSIZEOF(char*);

	ap=args;
	int iI=va_arg(ap,int);
	double dblD=va_arg(ap,double);
	char* pszStr=va_arg(ap,char*);

//	char* ff=args;
//#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
//#define va_start(ap,v)  ( ap = (va_list)&v + _INTSIZEOF(v) )
//#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
//#define va_end(ap)      ( ap = (va_list)0 )
//mlTrace("%d%s",10,"str");
*/

JSBool mlRMMLPlugin::JSFUNC_call(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if (argc < 1) return JS_FALSE;
	if(!JSVAL_IS_STRING(argv[0])) return JS_FALSE;
	mlRMMLPlugin* pPlugin=(mlRMMLPlugin*)JS_GetPrivate(cx, obj);
	if(PMO_IS_NULL(pPlugin->mpMO)) return JS_FALSE;
	moIPlugin* pmoIPlugin=pPlugin->mpMO->GetIPlugin();
	jschar* jscProcName=JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
	int iArgC=argc-1;
	va_list args=NULL;
	if(iArgC > 0){
		args = MP_NEW_ARR( char, iArgC*_INTSIZEOF(double)+2);
		va_list ap=args;
		for(int ii=0; ii<iArgC; ii++){
			jsval vArg=argv[1+ii];
			if(JSVAL_IS_INT(vArg)){
				*(int*)ap=JSVAL_TO_INT(vArg);
				ap+=_INTSIZEOF(int);
			}else if(JSVAL_IS_DOUBLE(vArg)){
				*(double*)ap=*JSVAL_TO_DOUBLE(vArg);
				ap+=_INTSIZEOF(double);
			}else if(JSVAL_IS_STRING(vArg)){
				*(wchar_t**)ap=wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));;
				ap+=_INTSIZEOF(wchar_t*);
			}else{
				JS_ReportWarning(cx,"Wrong argument type for plugin call-function");
				MP_DELETE_ARR( args);
				*rval=JSVAL_FALSE;
				return JS_TRUE;
			}
		}
	}
	MLDWORD dwRes;
	CInMyCall IMC(pPlugin);
	omsresult res=pmoIPlugin->Call(cLPCSTRq(jscProcName),iArgC,args,dwRes);
	if(OMS_FAILED(res)){
		JS_ReportWarning(cx,"ActiveX procedure (%s) call failed",cLPCSTRq(jscProcName));
	}
	*rval=INT_TO_JSVAL(dwRes);
	if(args!=NULL){
		 MP_DELETE_ARR( args);
	}
	return JS_TRUE;
}

omsresult mlRMMLPlugin::Call(char* apProcName, char* aArgTypes, va_list aArgsV){
	if(apProcName==NULL) return OMS_ERROR_INVALID_ARG;
	if(mpParent==NULL) return OMS_ERROR_FAILURE;
	if(!IsInMyCall()){
		// помещаем вызов в очередь
		mlSynchData Data;
		Data.setJSContext(mcx);
		GPSM(mcx)->SaveInternalPath(Data, GetParent());
		std::wstring sProcName = cLPWCSTR(apProcName);
		Data << sProcName.c_str();
		Data << false;
		int iArgC = aArgTypes?strlen(aArgTypes):0;
		Data << (((unsigned char)iArgC) | 0x80);
		if(iArgC>0){
			// JS_Lock(JS_GetRuntime(mcx));
			for(int ij=0; ij<iArgC; ij++){
				jsval vArg;
				vArg = JSVAL_NULL;
				switch(aArgTypes[ij]){
				case 'i':
					Data << (char)MLPT_INT;
					Data << (int)(va_arg(aArgsV,int));
					// vArg=INT_TO_JSVAL(va_arg(aArgsV,int));
					break;
				case 'f':
					Data << (char)MLPT_DOUBLE;
					Data << (double)(va_arg(aArgsV,double));
					// JS_NewDoubleValue(mcx,va_arg(aArgsV,double),&vArg);
					break;
				case 's':
					Data << (char)MLPT_STRING;
					Data << (wchar_t*)(va_arg(aArgsV,wchar_t*));
					// vArg = STRING_TO_JSVAL(JS_NewUCStringCopyZ(mcx,va_arg(aArgsV,wchar_t*)));
//{
//JSString *jss = JSVAL_TO_STRING(*pvArg);
//jschar* jsc = JS_GetStringChars(jss);
//int hh=0;
//}
					break;
				default:
					// JS_Unlock(JS_GetRuntime(mcx));
					mlTraceError(mcx, "Unsupported argument type character value \'%c\'",aArgTypes[ij]);
					return OMS_ERROR_INVALID_ARG;
				}
				// Data << vArg;
			}
			// JS_Unlock(JS_GetRuntime(mcx));
		}
		GPSM(mcx)->GetContext()->mpInput->AddInternalEvent(0x7F, true, (unsigned char*)Data.data(), Data.size());
		return OMS_OK;
	}
	// если есть такая функция
	JSFunction* jsf=NULL;
	jsval vFunc=JSVAL_NULL;
	wr_JS_GetUCProperty(mcx, mpParent->mjso, cLPWCSTRq(apProcName), -1, &vFunc);
	if(vFunc==JSVAL_VOID || !JSVAL_IS_OBJECT(vFunc) || !JS_ObjectIsFunction(mcx, JSVAL_TO_OBJECT(vFunc))){
		mlTraceError(mcx, "Plugin function %S not found\n", apProcName);
		return OMS_ERROR_NOT_IMPLEMENTED;
	}
	// то вызвать ее
	jsval* vArgs=NULL;
	int aiArgC=aArgTypes?strlen(aArgTypes):0;
	if(aiArgC>0){
		vArgs = MP_NEW_ARR( jsval, aiArgC);
		for(int ij=0; ij<aiArgC; ij++){
			jsval* pvArg=&vArgs[ij];
			*pvArg=JSVAL_NULL;
			switch(aArgTypes[ij]){
			case 'i':
				*pvArg=INT_TO_JSVAL(va_arg(aArgsV,int));
				break;
			case 'f':
				mlJS_NewDoubleValue(mcx,va_arg(aArgsV,double),pvArg);
				break;
			case 's':
				*pvArg = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(mcx,va_arg(aArgsV,wchar_t*)));
//{
//JSString *jss = JSVAL_TO_STRING(*pvArg);
//jschar* jsc = JS_GetStringChars(jss);
//int hh=0;
//}
				break;
			default:
				mlTraceError(mcx, "Unsupported argument type character value \'%c\'",aArgTypes[ij]);
				 MP_DELETE_ARR( vArgs);
				return OMS_ERROR_INVALID_ARG;
			}
		}
	}
	jsval v;
	//JS_CallFunction(mcx, JS_GetParent(mcx, JS_GetFunctionObject(jsf)), jsf, aiArgC, vArgs, &v);
	//JS_CallFunction(mcx, mpParent->mjso, jsf, aiArgC, vArgs, &v);
	JS_CallFunctionValue(mcx, mpParent->mjso, vFunc, aiArgC, vArgs, &v);
	if(vArgs!=NULL)  MP_DELETE_ARR( vArgs);
	return OMS_OK;
}

/**
* Перекрываем метод mlILoadable для того чтобы не коверкать скриптовый путь. Плагины грузятся с папки движка
*/
const wchar_t* mlRMMLPlugin::GetSrc(){
	if(src==NULL) return NULL;
	wchar_t* pwcSrc=wr_JS_GetStringChars(src);
	if(*pwcSrc == L'\0') return NULL;
	if(*(pwcSrc+1) == L'\0' && (*pwcSrc == L'\\' || *pwcSrc == L'/')) return NULL;

	msSrcFull = pwcSrc;
	return msSrcFull.c_str();
}

}

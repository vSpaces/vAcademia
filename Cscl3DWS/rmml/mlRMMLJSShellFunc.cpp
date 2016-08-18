#include "mlRMML.h"
#include "config/oms_config.h"
#include "jsdbgapi.h"
#include "config/prolog.h"
#include "ILogWriter.h"
#include "..\InfoMan\InfoMan.h"
#include "md5.h"

namespace rmml {

#pragma warning( disable : 4996 )

char* WC2MB(const wchar_t* apwc){
	int iSLen=wcslen(apwc);
	char* pch = MP_NEW_ARR( char, iSLen*MB_CUR_MAX+1);
	wcstombs(pch, apwc, iSLen);
//	memset(&pch[0], 0, iSLen+1);
//	mbstate_t gds_state=0;
//	std::locale gds_loc("Russian");//English_Britain");//German_Germany
//	char* pszNext;
//	wchar_t* pwszNext;
//	std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >(gds_loc, (std::codecvt<wchar_t, char, mbstate_t> *)0, true).out( 
//		gds_state, 
//		apwc, &apwc[iSLen], pwszNext, 
//		pch, &pch[iSLen], pszNext);
	pch[iSLen]=0;
	return pch;
}

char* WC2MBq(const wchar_t* apwc){
	int iSLen=wcslen(apwc);
	char* pch = MP_NEW_ARR( char, iSLen+1);
	pch[iSLen]=0;
	char* pc=pch;
	while(iSLen--){
		*pc++=(char)*apwc++;
	}
	return pch;
}

wchar_t* MB2WC(const char* apch){
	int iSLen=strlen(apch);
	wchar_t* wsAttrValue = MP_NEW_ARR( wchar_t, iSLen+1);
	mbstowcs(wsAttrValue, apch, iSLen);
//	memset(&wsAttrValue[0], 0, (sizeof(wchar_t))*(iSLen+1));
//	mbstate_t gds_state=0;
//	std::locale gds_loc("Russian");//English_Britain");//German_Germany
//	char* gds_pszNext;
//	wchar_t* gds_pwszNext;
//	std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >(gds_loc, (std::codecvt<wchar_t, char, mbstate_t> *)0, true).in( 
//		gds_state, 
//		apch, &apch[iSLen], gds_pszNext, 
//		wsAttrValue, &wsAttrValue[iSLen], gds_pwszNext);
	wsAttrValue[iSLen]=0;
	return wsAttrValue;
}

wchar_t* MB2WCq(const char* apch){
	int iSLen=strlen(apch);
	wchar_t* ws = MP_NEW_ARR( wchar_t, iSLen+1);
	memset(&ws[0], 0, (sizeof(wchar_t))*(iSLen+1));
	ws[iSLen]=0;
	wchar_t* pwc=ws;
	while(iSLen--){
		*pwc++=(wchar_t)*apch++;
	}
	return ws;
}

static JSBool Alert(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
//    jsval result;
	if(argc < 1) return JS_FALSE;
	jsval v0=argv[0];
	JSString *str;
JSType type=JS_TypeOfValue(cx, v0);
	//if(JSVAL_IS_STRING(v0)){
	//	str = JSVAL_TO_STRING(v0);
	//}else{
		str = JS_ValueToString(cx, v0);
	//}
	wchar_t* jscStr=wr_JS_GetStringChars(str);
	{	char* pchStr=WC2MB(jscStr);
TRACE(cx, "js-alert:\'%s\'\n",pchStr);
		GPSM(cx)->GetContext()->mpApp->ShowMessageBox((const wchar_t*)jscStr, L"RMML Alert");
		 MP_DELETE_ARR( pchStr);
	}
	return JS_TRUE;
}

static JSBool Trace(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc < 1) return JS_FALSE;
	JSString *str = JS_ValueToString(cx, argv[0]);
	if(str==NULL) return JS_FALSE;
wchar_t* pwsz=wr_JS_GetStringChars(str);
//if(isEqual(str, L"hhhhhhhhhhhhhhhhhj"))
//int hh=0;
	mlTrace(cx, "%s\n",cLPCSTR(str));
	return JS_TRUE;
}

static JSBool TraceNew(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc < 1) return JS_FALSE;
	JSString *str = JS_ValueToString(cx, argv[0]);
	if(str==NULL) return JS_FALSE;
wchar_t* pwsz=wr_JS_GetStringChars(str);
//if(isEqual(str, L"hhhhhhhhhhhhhhhhhj"))
//int hh=0;
	mlTraceNew(cx, "%s\n",cLPCSTR(str));
	return JS_TRUE;
}

std::vector<JSObject*> vLooked2;
std::vector<wchar_t*> vsRefs;

//#define JSO_IS_MLEL(JSO) (JS_GetClass(JSO)->spare==1)

bool FindJSORefs(JSContext* cx,JSObject* obj,JSObject* objToFind){
	if(obj==JS_GetGlobalObject(cx)){
		vLooked2.clear();
		vsRefs.clear();
	}
	int iPropCnt = 0;
	mlPropertyValues vPropVals;
	JSIdArray* pGPropIDArr = NULL;
mlRMMLElement* pMLElTmp = NULL;
	if(JSO_IS_MLEL(cx, obj)){
		mlRMMLElement* pMLEl = GET_RMMLEL(cx, obj);
pMLElTmp = pMLEl;
//		pMLEl->GetAllProperties(vPropVals);
//		iPropCnt = vPropVals.mvPropVals.size();
	}
	if(iPropCnt == 0){
		pGPropIDArr = JS_Enumerate(cx, obj);
		iPropCnt = pGPropIDArr->length;
	}
	for(int ikl=0; ikl<iPropCnt; ikl++){
		mlString sPropName;
		jsval vProp;
		if(pGPropIDArr != NULL){
			jsid id=pGPropIDArr->vector[ikl];
			jsval v;
			if(!JS_IdToValue(cx,id,&v)) continue;
			if(JSVAL_IS_STRING(v)){
				wchar_t* jsc=wr_JS_GetStringChars(JSVAL_TO_STRING(v));
				wr_JS_GetUCProperty(cx,obj,jsc,-1,&vProp);
				sPropName = jsc;
			}else if(JSVAL_IS_INT(v)){
				JS_GetElement(cx, obj, JSVAL_TO_INT(v), &vProp);
				wchar_t pwcNum[20];
				swprintf(pwcNum, L"[%d]", JSVAL_TO_INT(v));
				sPropName = pwcNum;
			}else
				continue;
		}else{
			mlPropertyValues::mlPropertyValue* pPropVal = vPropVals.mvPropVals[ikl];
			sPropName = cLPWCSTR(pPropVal->name);
			vProp = pPropVal->val;
		}
		if(!JSVAL_IS_REAL_OBJECT(vProp)) continue;
if(isEqual(sPropName.c_str(), L"__DefMats_"))
continue; // а то выпадает часто на попытке взять класс у diffuse_map.source
		JSObject* jsoProp=JSVAL_TO_OBJECT(vProp);
		bool bLooked=false;
		std::vector<JSObject*>::iterator vi;
		for(vi=vLooked2.begin(); vi!=vLooked2.end(); vi++){
			if(jsoProp==*vi){
				bLooked=true;
				break;
			}
		}
		if(bLooked) continue;
		if(jsoProp==objToFind){
			// формируем путь к объекту, в свойствах 
			// которого есть ссылка на objToFind
			mlString wsPath;
			if(obj==JS_GetGlobalObject(cx)){
				wsPath = sPropName;
			}else if(JSO_IS_MLEL(cx, obj)){
				mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,obj);
				wsPath=pMLEl->GetFullPath();
				wsPath+=L'.';
				wsPath+=sPropName;
			}else{
				wchar_t pwcBuf[50];
				JSClass* objclass = ML_JS_GETCLASS(cx, obj);
				if( objclass)
					swprintf(pwcBuf,L"...[%X:%s]",obj, cLPWCSTR(objclass->name));
				else
					swprintf(pwcBuf,L"...[%X]",obj);
				wsPath=pwcBuf;
				wsPath+=L'.';
				wsPath+=sPropName;
			}
			if(!wsPath.empty()){
				int bufferLength = wsPath.length()+1;
				wchar_t* pwc = MP_NEW_ARR( wchar_t, bufferLength);
				//wcscpy(pwc,wsPath.data());
				rmmlsafe_wcscpy(pwc, bufferLength, 0, wsPath.data());
				vsRefs.push_back(pwc);
			}
			return false;
		}
		vLooked2.push_back(jsoProp);
		if(FindJSORefs(cx,jsoProp,objToFind)){
			return false;
		}
	}
	if(pGPropIDArr != NULL)
		JS_DestroyIdArray(cx, pGPropIDArr);
	return false;
}

static JSBool TraceRefs(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc < 1) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[0])) return JS_TRUE; 
	vLooked2.clear();
	FindJSORefs(cx,JS_GetGlobalObject(cx),JSVAL_TO_OBJECT(argv[0]));
	std::vector<wchar_t*>::iterator vi;
	for(vi=vsRefs.begin(); vi!=vsRefs.end(); vi++){
		mlTrace(cx, "%S\n",*vi);
		 MP_DELETE( *vi);
	}
	vLooked2.clear();
	vsRefs.clear();
	return JS_TRUE;
}

JSBool TraceRefs(JSContext *cx, JSObject *obj, JSObject* objToFind){
	vLooked2.clear();
	FindJSORefs(cx, JS_GetGlobalObject(cx), objToFind);
	std::vector<wchar_t*>::iterator vi;
	for(vi=vsRefs.begin(); vi!=vsRefs.end(); vi++){
		mlTrace(cx, "%S\n",*vi);
		 MP_DELETE( *vi);
	}
	vLooked2.clear();
	vsRefs.clear();
	return JS_TRUE;
}

JSBool LogRefs(JSContext *cx, JSObject *obj, JSObject* objToFind){
	vLooked2.clear();
	FindJSORefs(cx, JS_GetGlobalObject(cx), objToFind);
	std::vector<wchar_t*>::iterator vi;
	ILogWriter* pLogWriter = GPSM( cx)->GetContext()->mpLogWriter;
	mlRMMLElement* pElem = (mlRMMLElement*)JS_GetPrivate( cx, objToFind);
	for(vi=vsRefs.begin(); vi!=vsRefs.end(); vi++){
		CLogValue logValue(
			"[RMML]: Reference on element ", (LPCSTR)cLPCSTR( pElem->GetName()), ": ", (LPCSTR)cLPCSTR( *vi)

		);
		pLogWriter->WriteLnLPCSTR( logValue.GetData());
		 MP_DELETE( *vi);
	}
	
	vLooked2.clear();
	vsRefs.clear();
	return JS_TRUE;
}

static JSBool GetCurrentScriptLineIndex(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlSceneManager* mpSM =  GPSM(cx);

	*rval = INT_TO_JSVAL(mpSM->GetCurrentExecutingScriptLine());

	return JS_TRUE;
}

static JSBool Trim(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc<1 || argc>3) return JS_FALSE;
	wchar_t* jscSpaceChars=L" \t\n\r";
	if(argc==3){
		if(!JSVAL_IS_STRING(argv[2])) return JS_FALSE;
		JSString* jssSpaceChars=JSVAL_TO_STRING(argv[2]);
		jscSpaceChars=wr_JS_GetStringChars(jssSpaceChars);
	}
	if(!JSVAL_IS_STRING(argv[0])) return JS_FALSE;
	JSString* jssIn=JSVAL_TO_STRING(argv[0]);
	int iAlign=3; // left(1)+right(2)
	if(argc==2){
		if(!JSVAL_IS_INT(argv[1])) return JS_FALSE;
		int iA=JSVAL_TO_INT(argv[1]);
		if(iA<0 || iA>3) return JS_FALSE;
		if(iA==0){
			*rval=STRING_TO_JSVAL(jssIn);
			return JS_TRUE;
		}
		iAlign=iA;
	}
	jschar* jscInLeft=JS_GetStringChars(jssIn);
	jschar* jscInRight=jscInLeft+JS_GetStringLength(jssIn)-1;
	if(iAlign & 1){ // trim left
		while(jscInLeft < jscInRight){
			wchar_t* jscSC=jscSpaceChars;
			while(*jscSC!=L'\0'){
				if(*jscInLeft==*jscSC) break;
				jscSC++;
			}
			if(*jscSC==L'\0') break;
			jscInLeft++;
		}
	}
	if(iAlign & 2){ // trim right
		while(jscInLeft < jscInRight){
			wchar_t* jscSC=jscSpaceChars;
			while(*jscSC!=L'\0'){
				if(*jscInRight==*jscSC) break;
				jscSC++;
			}
			if(*jscSC==L'\0') break;
			jscInRight--;
		}
	}
	JSString* jssOut;
	if(jscInRight==jscInLeft){
		jscInRight--;
	}
	jssOut=JS_NewUCStringCopyN(cx,jscInLeft,jscInRight-jscInLeft+1);
	*rval=STRING_TO_JSVAL(jssOut);
	return JS_TRUE;
}

static JSBool SetFocus(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc < 1) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[0])) return JS_FALSE;
	JSObject* jsoMLEl=JSVAL_TO_OBJECT(argv[0]);
	mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(cx,jsoMLEl);
	mlIButton* pBut=pMLEl->GetIButton();
	if(pBut==NULL){
		mlTrace(cx, "Cannot set focus to no button RMML-element %S",pMLEl->GetName());
		return JS_FALSE;
	}
	if(!pBut->GetAbsEnabled()){
		mlTrace(cx, "Cannot set focus to disabled button RMML-element %S",pMLEl->GetName());
		return JS_FALSE;
	}
	GPSM(cx)->SetFocus(pMLEl);
	return JS_TRUE;
}

static JSBool GetFocus(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc != 0) return JS_FALSE;
	mlRMMLElement* pMLEl=GPSM(cx)->GetFocus();
	if(pMLEl==NULL) *rval=JSVAL_NULL;
	else *rval=OBJECT_TO_JSVAL(pMLEl->mjso);
	return JS_TRUE;
}

static JSBool TraceTestResult(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc < 1) return JS_FALSE;
	JSString *str = JS_ValueToString(cx, argv[0]);
	if(str==NULL) return JS_FALSE;
//	mlTrace(cx, "%s\n",cLPCSTR(str));
	char* mpchStr=WC2MB(wr_JS_GetStringChars(str));
	GPSM(cx)->GetContext()->mpInfoMan->TraceTestResult(mpchStr);
	return JS_TRUE;
}

static JSBool TestEnded(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if(argc < 2) return JS_FALSE;
	int32 iCompleteTest = 0;
	JS_ValueToInt32(cx, argv[0], &iCompleteTest);
	int32 iFailTest = 0;
	JS_ValueToInt32(cx, argv[1], &iFailTest);
	//	mlTrace(cx, "%s\n",cLPCSTR(str));
	//char* mpchStr=WC2MB(wr_JS_GetStringChars(str));
	GPSM(cx)->GetContext()->mpInfoMan->TestEnded( iCompleteTest, iFailTest);
	return JS_TRUE;
}


static JSBool ExternCommand(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc<1 || argc>2){
		JS_ReportError(cx,"externCommand must get one or two string parameters");
		return JS_FALSE;
	}
	if(!JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx,"first parameter of externCommand must be a string");
		return JS_FALSE;
	}
	wchar_t* pwcCmd=wr_JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
	wchar_t* pwcPar=NULL;
	if(argc==2){
		if(!JSVAL_IS_STRING(argv[1])){
			JS_ReportError(cx,"second parameter of externCommand must be a string");
			return JS_FALSE;
		}
		pwcPar=wr_JS_GetStringChars(JSVAL_TO_STRING(argv[1]));
	}
	if(GPSM(cx)->GetContext()->mpApp!=NULL){
		if((isEqual(pwcCmd,L"TextLogEvent") || isEqual(pwcCmd,L"AddLogEvent")) && pwcPar!=NULL){
			int bufferLength = wcslen(pwcPar)+sizeof(unsigned short)/sizeof(wchar_t)+1;
			wchar_t* pwcPar2 = MP_NEW_ARR( wchar_t, bufferLength);
			*pwcPar2=(wchar_t)GPSM(cx)->muExternEventID;
			//wcscpy(pwcPar2+sizeof(unsigned short)/sizeof(wchar_t),pwcPar);
			rmmlsafe_wcscpy(pwcPar2, bufferLength, sizeof(unsigned short)/sizeof(wchar_t), pwcPar);
			GPSM(cx)->GetContext()->mpApp->ExternCommand(L"_i!TLE", pwcPar2);
			MP_DELETE_ARR( pwcPar2);
		}else
			GPSM(cx)->GetContext()->mpApp->ExternCommand(pwcCmd, pwcPar);
	}
		
	return JS_TRUE;
}

static JSBool ResourceExists(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc!=1) return JS_FALSE;
	if(!JSVAL_IS_STRING(argv[0])) return JS_FALSE;
	JSString* jss=JSVAL_TO_STRING(argv[0]);
	omsContext* pOMSCont=GPSM(cx)->GetContext();
	wchar_t* pwcPath=wr_JS_GetStringChars(jss);
	mlString sPath=GPSM(cx)->RefineResPathEl(GPSM(cx)->GetScene(), pwcPath);
	unsigned int codeError = 0;
	bool bRet=pOMSCont->mpResM->ResourceExists(sPath.data(), codeError);
	*rval=BOOLEAN_TO_JSVAL(bRet);
	return JS_TRUE;
}

static JSBool Exit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	GPSM(cx)->mbWasExitFromScript=true;
	if(argc > 0){
		*rval=argv[0];
		GPSM(cx)->mjsvalExit=argv[0];
	}
	return JS_FALSE;
}

static JSBool MultiplyRotations(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	ml3DRotation rot1;
	ml3DRotation rot2;
	int iGood=0;
	if(argc >= 2){
		jsval v=argv[0];
		if(JSVAL_IS_OBJECT(v) && v!=JSVAL_NULL && v!=JSVAL_VOID){
			JSObject* jso=JSVAL_TO_OBJECT(v);
			if(mlRotation3D::IsInstance(cx, jso)){
				mlRotation3D* pRMMLRot=(mlRotation3D*)JS_GetPrivate(cx,jso);
				rot1=pRMMLRot->GetRot();
				iGood++;
			}
		}
		v=argv[1];
		if(JSVAL_IS_OBJECT(v) && v!=JSVAL_NULL && v!=JSVAL_VOID){
			JSObject* jso=JSVAL_TO_OBJECT(v);
			if(mlRotation3D::IsInstance(cx, jso)){
				mlRotation3D* pRMMLRot=(mlRotation3D*)JS_GetPrivate(cx,jso);
				rot2=pRMMLRot->GetRot();
				iGood++;
			}
		}
	}
	if(iGood < 2){
		JS_ReportError(cx,"multiplyRotations must get two Rotation3D objects");
		return JS_FALSE;
	}
	*rval=JSVAL_NULL;
	rm::rmIRenderManager* pRM=GPSM(cx)->GetContext()->mpRM;
	if(pRM!=NULL){
		ml3DRotation rotres=pRM->multiplyRotations(rot1, rot2);
		JSObject* jsoRot=mlRotation3D::newJSObject(cx);
		mlRotation3D* pRot=(mlRotation3D*)JS_GetPrivate(cx,jsoRot);
		pRot->SetRot(rotres);
		*rval=OBJECT_TO_JSVAL(jsoRot);
	}
	// ??
	return JS_TRUE;
}

static JSBool MakeAllPropsEnumerable(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlRMMLElement* pMLEl = (mlRMMLElement*)GPSM(cx)->GetActiveScene();
	if(argc > 0){
		jsval vArg = argv[0];
		if(!JSVAL_IS_REAL_OBJECT(vArg)){
			JS_ReportError(cx,"First argument of makeAllPropsEnumerable function must be a RMML-object");
			return JS_TRUE;
		}
		JSObject* jso = JSVAL_TO_OBJECT(vArg);
		if(!JSO_IS_MLEL(cx, jso)){
			JS_ReportError(cx,"First argument of makeAllPropsEnumerable function must be a RMML-object");
			return JS_TRUE;
		}
		pMLEl = GET_RMMLEL(cx, jso);
	}
	bool bRecurse = false;
	if(argc > 1){
		jsval vArg = argv[1];
		if(!JSVAL_IS_BOOLEAN(vArg)){
			JS_ReportError(cx,"Second argument of makeAllPropsEnumerable function must be a boolean");
			return JS_TRUE;
		}
		bRecurse = JSVAL_TO_BOOLEAN(vArg);
	}
	if(pMLEl != NULL){
		pMLEl->MakeAllPropsEnumerable(bRecurse);
	}
	return JS_TRUE;
}

static JSBool GetFullPath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc == 0 || !JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx,"First argument of getFullPath function must be a string");
		return JS_TRUE;
	}
	JSString* jssResPath = JSVAL_TO_STRING(argv[0]);
	wchar_t* jscResPath = wr_JS_GetStringChars(jssResPath);
	// если начинаяется с ':', то уже полный путь
	if(*jscResPath == L':'){
		*rval = STRING_TO_JSVAL(jssResPath);
		return JS_TRUE;
	}
	//
	mlString sFullResPath = GPSM(cx)->GetFullResPath(jscResPath);
	*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, sFullResPath.c_str()));
	return JS_TRUE;
}

static JSBool GetFileSize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc == 0 || !JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx,"First argument of getFileSize function must be a string");
		return JS_TRUE;
	}
	JSString* jssResPath = JSVAL_TO_STRING(argv[0]);
	wchar_t* jscResPath = wr_JS_GetStringChars(jssResPath);
	mlString sFullResPath;
	// если начинается с ':', то уже полный путь
	if ( *jscResPath == L':')
		sFullResPath = jscResPath;	
	else
		sFullResPath = GPSM(cx)->GetFullResPath( jscResPath);
	omsContext* pContext = GPSM(cx)->GetContext();	
	res::resIResource *pResource = NULL;
	omsresult ires = 0;
	if ( OMS_FAILED( ires = pContext->mpResM->OpenResource( sFullResPath.c_str(), pResource, RES_REMOTE | RES_LOCAL | RES_FILE_CACHE | RES_MEM_CACHE))){		
		JS_ReportError(cx, "File '%s' is not exist", WC2MB( sFullResPath.c_str()));
		*rval = INT_TO_JSVAL( 0);
		return JS_TRUE;
	}

	int isize = pResource->GetSize();
	*rval = INT_TO_JSVAL( isize);
	pResource->Close();
	return JS_TRUE;
}

static JSBool CheckBug(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	GPSM(cx)->CheckBug();
	return JS_TRUE;
}

static JSBool GetCallStack(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	mlString sCallStack = GPSM(cx)->GetJSCallStack();
	JSString* jssCS = wr_JS_NewUCStringCopyZ(cx, sCallStack.c_str());
	*rval = STRING_TO_JSVAL(jssCS);
	return JS_TRUE;
}

static JSBool SetSynchTracesEnabled(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	bool abEnabled = false;
	unsigned int auLevel = 0;

	if(argc == 0 || !JSVAL_IS_BOOLEAN(argv[0]))
	{
		JS_ReportError(cx,"First argument of setSynchTracesEnabled function must be a boolean");
		return JS_TRUE;
	}
	abEnabled = JSVAL_TO_BOOLEAN(argv[0]);

	if(argc > 1 && JSVAL_IS_INT(argv[1]))
		auLevel = JSVAL_TO_INT(argv[1]);

	mlSetSynchTracesEnabled( abEnabled, auLevel);
	return JS_TRUE;
}

static JSBool EvalMD5Hash(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc == 0 || !JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx,"Argument of evalMd5Hash function must be a string");
		return JS_TRUE;
	}
	JSString* jssStr = JSVAL_TO_STRING(argv[0]);
	wchar_t* jscStr = wr_JS_GetStringChars(jssStr);
	char hash[33];
	GetMD5( (BYTE*)jscStr, wcslen(jscStr) * 2, &hash[0], 33);
	*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, &hash[0]));
	return JS_TRUE;
}

static JSBool TraceLeakedElements(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	GPSM(cx)->TraceLeakedElements();
	return JS_TRUE;
}

static JSBool TraceUpdatableElements(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	GPSM(cx)->TraceUpdatableElements();
	return JS_TRUE;
}

static JSBool FreeClass(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc == 0 || !JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx,"Argument of freeClass function must be a string");
		return JS_TRUE;
	}
	JSString* jssClassName = JSVAL_TO_STRING(argv[0]);
	mlString wsClassName = (wchar_t*)JS_GetStringChars(jssClassName);
	*rval = GPSM(cx)->FreeClass(wsClassName);
	return JS_TRUE;
}

JSFunctionSpec shell_functions[] = {
	{"alert",        Alert,       1},
	{"trace",        Trace,       1},
	{"traceNew",     TraceNew,       1},
	{"traceRefs",    TraceRefs,   1},
	{"trim",         Trim,        2},
	{"setFocus",     SetFocus,    1},
	{"getFocus",     GetFocus,    0},
	{"externCommand",ExternCommand,2},
	{"resourceExists",ResourceExists,1},
	{"exit",         Exit,1}, // exit script
	{"multiplyRotations", MultiplyRotations,2},
	{"makeAllPropsEnumerable",    MakeAllPropsEnumerable,   2},
	{"getFullPath",  GetFullPath, 1},
	{"getFileSize",  GetFileSize, 1},
	{"checkBug",  CheckBug, 0},
	{"getCallStack",  GetCallStack, 0},	// получить JS call stack в виде строки
	{"setSynchTracesEnabled",  SetSynchTracesEnabled, 0},	// включить/выключить логи свойств
	{"traceTestResult", TraceTestResult, 1},
	{"testEnded", TestEnded, 2},
	{"evalMD5Hash", EvalMD5Hash, 1},
	{"getCurrentScriptLineIndex", GetCurrentScriptLineIndex, 1},
	{"traceLeakedElements", TraceLeakedElements, 1}, // вывести список RMML-элементов, которые не в иерархии сцены, но JS-ссылка на них имеется (сейчас выводится содержимое mvNewElements)
	{"traceUpdatableElements", TraceUpdatableElements, 1}, // вывести список обновляемых RMML-элементов (у которых вызывается Update() на каждый update sceneManager-а)
	{"freeClass", FreeClass, 1}, // удяляет указанный класс из Classes
	{0,0,0}
};

}

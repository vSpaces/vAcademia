#include "mlRMML.h"
#include "mlSPParser.h"
#include "ServiceMan.h"
#include "mlJSONRPCJSO.h"
#include "config/prolog.h"
#include <string.h>
#include <stdio.h>


namespace rmml {

mlJSONRPCJSO::mlJSONRPCJSO():
	MP_WSTRING_INIT(serviceName)
{ 
	iID = 0; errorCode = 0; /*requestID = 0;*/ bInitialiazed = false;
}
void mlJSONRPCJSO::SetManager(service::IBaseServiceManager* aManager)
{
	manager = aManager;
}

void mlJSONRPCJSO::SetServiceName(wchar_t* aServiceName)
{
	serviceName = aServiceName;
}

mlJSONRPCJSO::~mlJSONRPCJSO(void){
/*	
	destroy HERE
	*/
}
#define JSPN_XMLDocument L"XMLDocument"
	
JSPropertySpec mlJSONRPCJSO::_JSPropertySpec[] = {
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlJSONRPCJSO::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(JSONRPCService,mlJSONRPCJSO,0)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlJSONRPCJSO)

JSBool mlJSONRPCJSO::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlJSONRPCJSO);
	default:
		break;
	SET_PROPS_END;
	return JS_TRUE;
}

JSBool mlJSONRPCJSO::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlJSONRPCJSO);
	default:
		break;
  GET_PROPS_END;
	return JS_TRUE;
}

struct WsdlType{
	char * type;
	char * val;
};

int mlJSONRPCJSO::SendQuery(const wchar_t* wchQuery, const wchar_t* wchParam)
{
	mlSceneManager* pSM = GPSM(mcx);
	wchar_t wParam[1024];
	int j = swprintf_s(wParam, 1024, L"servname=");
	swprintf_s(wParam+j, 1024-j, wchParam);
	return pSM->GetContext()->mpComMan->SendQuery(wchQuery, wParam);
}

void PrepareJSONParam(mlString& smlArg){
	int sLength = smlArg.length();
	// Written by Vladimir
	//assert( sLength > 0);
	if ( sLength == 0)
		return;

	if (smlArg[sLength-1]==10 && smlArg[sLength-2]==13 )
		smlArg[sLength -2] = '\0';
	if (smlArg[sLength-1]==13 )
		smlArg[sLength-1] = '\0';
	mlString::size_type pos = 0;
	while((pos = smlArg.find(L"'",pos)) != mlString::npos){
		smlArg.replace(pos, 1, L"\"");
		pos += 1;
	}

	// Нужно заменить все вхождения \ на \\ 
	int nPos = 0;
	while( (nPos = smlArg.find(L"\\", nPos)) != -1)
	{
		smlArg.replace(nPos, 1, L"\\\\");
		nPos += 2;
	}
}

mlString mlJSONRPCJSO::ObjectToJSON(JSContext *cx, JSObject* ajso){
	mlString sJSON;
	if(JSO_IS_MLEL(cx, ajso)){
		mlRMMLElement* pElem = GET_RMMLEL(cx, ajso);
		if(pElem == NULL)
			return L"{}";
		sJSON = L"{";
		unsigned int uiID = pElem->GetID();
		sJSON += L"name:'";
		sJSON += pElem->GetName();
		sJSON += L"',";
		if(uiID != OBJ_ID_UNDEF){
			sJSON += L"id:";
			wchar_t wsID[30];
			swprintf_s(wsID, 30, L"%d", uiID);
			sJSON += wsID;
			sJSON += L",";
			unsigned int uiBornRealityID = pElem->GetBornRealityID();
			sJSON += L"bornRealityID:";
			swprintf_s(wsID, 30, L"%d", uiBornRealityID);
			sJSON += wsID;
			sJSON += L",";
			int iCurrentRealityID = 0;
			if(GPSM(cx)->GetContext()->mpMapMan != NULL)
				iCurrentRealityID = GPSM(cx)->GetContext()->mpMapMan->GetCurrentRealityID();
			sJSON += L"realityID:";
			swprintf_s(wsID, 30, L"%d", iCurrentRealityID);
			sJSON += wsID;
			sJSON += L"";
		}else{
			sJSON += L" error: 'object has no server ID' ";
		}
		sJSON += L"}";
	}else if(mlSynchLinkJSO::IsInstance(cx, ajso)){
#pragma message("mlJSONRPCJSO::ObjectToJSON: not tested")
		mlSynchLinkJSO* pLink = (mlSynchLinkJSO*)JS_GetPrivate(cx, ajso);
		sJSON = L"{ link:true, ";
		JSObject* jso = pLink->GetRef();
		if(jso != NULL){
			mlRMMLElement* pElem = GET_RMMLEL(cx, jso);
			if(pElem != NULL){
				sJSON += L"name:'";
				sJSON += pElem->GetName();
				sJSON += L"',";
			}
		}
		unsigned int uiBornRealityID = 0;
		unsigned int uiID = pLink->GetObjectID(uiBornRealityID);
		if(uiID != OBJ_ID_UNDEF){
			sJSON += L"id:";
			wchar_t wsID[30];
			swprintf_s(wsID, 30, L"%d", uiID);
			sJSON += wsID;
			sJSON += L",";
			sJSON += L"bornRealityID:";
			swprintf_s(wsID, 30, L"%d", uiBornRealityID);
			sJSON += wsID;
			sJSON += L",";
			int iCurrentRealityID = 0;
			if(GPSM(cx)->GetContext()->mpMapMan != NULL)
				iCurrentRealityID = GPSM(cx)->GetContext()->mpMapMan->GetCurrentRealityID();
			sJSON += L"realityID:";
			swprintf_s(wsID, 30, L"%d", iCurrentRealityID);
			sJSON += wsID;
		}
		sJSON += L"}";
	}else{
#pragma message("mlJSONRPCJSO::ObjectToJSON: not tested")
		// считаем что обычный JS-объект: вывести его свойства
		sJSON = L"{";
		JSIdArray* pGPropIDArr= JS_Enumerate(cx, ajso);
		for(int ikl=0; ikl<pGPropIDArr->length; ikl++){
			jsid id=pGPropIDArr->vector[ikl];
			jsval v;
			if(!JS_IdToValue(cx,id,&v)) continue;
			jschar* jsc = NULL;
			jsval vProp;
			if(JSVAL_IS_STRING(v)){
				jsc = JS_GetStringChars(JSVAL_TO_STRING(v));
				JS_GetUCProperty(cx,ajso,jsc,-1,&vProp);
				sJSON += (const wchar_t*)jsc;
				sJSON += L":";
			}else if(JSVAL_IS_INT(v)){
				int iId = JSVAL_TO_INT(v);
				JS_GetElement(cx, ajso, iId, &vProp);
				jsc = (jschar*)iId;
			}			
			else continue;

			if(JSVAL_IS_OBJECT(vProp)){
				JSObject* jsoArg = JSVAL_TO_OBJECT(vProp);
				if( jsoArg != NULL)
				{
					if(JS_IsArrayObject(cx, jsoArg)){				
						sJSON += L"[";
						bool bFirstElem = true;
						JSObject* jsoArr = jsoArg;//JSVAL_TO_OBJECT(vArg);
						jsuint uiLen=0;
						if(JS_GetArrayLength(cx,jsoArr,&uiLen)){
							for(unsigned int jj=0; jj<uiLen; jj++){
								jsval v;
								if(JS_GetElement(cx, jsoArr, jj, &v)){
									if(JSVAL_IS_NUMBER(v)){
										JSString* jss = JS_ValueToString(cx, v);
										if(!bFirstElem) sJSON += L","; bFirstElem = false;
										sJSON += (const wchar_t*)JS_GetStringChars(jss);
									}else if(JSVAL_IS_STRING(v)){
										JSString* jss = JSVAL_TO_STRING(v);
										mlString sVal = (const wchar_t*)JS_GetStringChars(jss);
										PrepareJSONParam(sVal);
										if(!bFirstElem) sJSON += L","; bFirstElem = false;
										sJSON += L'\'';
										sJSON += sVal.c_str();
										sJSON += L'\'';
									}else if(JSVAL_IS_OBJECT(v)){
										if(!bFirstElem) sJSON += L","; bFirstElem = false;
										sJSON += (wchar_t *)(this->ObjectToJSON(cx, JSVAL_TO_OBJECT(v))).c_str();
									}
								}
							}
						}
						sJSON += L"]";
					}
					else
					{
						sJSON += (wchar_t *)(this->ObjectToJSON(cx, jsoArg)).c_str();
					}
				}
			}
			else
			{
				JSString* jssVal = JS_ValueToString(cx, vProp);
				if(jssVal != NULL){
	//				jschar* jscVal = JS_GetStringChars(jssVal);
					mlString sVal = (const wchar_t*)JS_GetStringChars(jssVal);
					PrepareJSONParam(sVal);
					if(JSVAL_IS_STRING(vProp))
						sJSON += L"'";
	//				sJSON += jscVal;
					sJSON += sVal;
					if(JSVAL_IS_STRING(vProp))
						sJSON += L"'";
				}
			}
			sJSON += L",";
		}
		JS_DestroyIdArray(cx, pGPropIDArr);
		sJSON += L"}";
	}
	return sJSON;
}

const char* GetJSOnParameterType( int iRequiredType)
{
	const char* lpcRequired = NULL;
	switch( iRequiredType)
	{
		case mlJSONRPCJSO::JSON_STR:
			lpcRequired = "Строка";
			break;
		case mlJSONRPCJSO::JSON_NUMBER: // оборачивается в '' и в итоге приходит на сервер как строка (оставлено для совместимости) (тип в описании сервиса - integer)
			lpcRequired = "Число обернутое в кавычки";
			break;
		case mlJSONRPCJSO::JSON_ARRAY:
			lpcRequired = "Массив";
			break;
		case mlJSONRPCJSO::JSON_NUM:
			lpcRequired = "Число";
			break;
		case mlJSONRPCJSO::JSON_OBJECT:
			lpcRequired = "Объект";
			break;
		case mlJSONRPCJSO::JSON_UNRECOGNIZE:
			lpcRequired = "Неопознанный";
			break;
		default:
			lpcRequired = "Вообще непонятно";
			break;
	}
	return lpcRequired;
}

JSBool mlJSONRPCJSO::JSFUNC_JSONMethod(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlJSONRPCJSO* pJSON =(mlJSONRPCJSO*)JS_GetPrivate(cx,obj);

	JSStackFrame* fp = NULL;
	fp = JS_FrameIterator(cx, &fp);

	mlSceneManager* pSM = GPSM(cx);
	JSObject* jsoMethodObject = NULL;

	for(;fp != NULL; fp = JS_FrameIterator(cx, &fp))
	{
		if(JS_GetFrameFunction(cx, fp) != NULL)
		{
			JSFunction* fun = JS_GetFrameFunction(cx, fp);
			JSString* jssFun = JS_GetFunctionId(fun);
			if(jssFun == NULL) return NULL;
			jschar* jscFunctionName = JS_GetStringChars(jssFun);;
			gd::CString sJSONMethodDescription = L"{ 'method': '";
			//sJSONMethodDescription = sJSONMethodDescription;
			sJSONMethodDescription += (const wchar_t*)jscFunctionName;
			sJSONMethodDescription += L"', 'params': [";
		 
			JSONService *s=&pJSON->service;
			MethodList test =  s->methods;

			for (MethodList ::iterator item = test.begin(); item != test.end(); item++) 
			{
				// Метод найден. Берем его параметры
				if(isEqual((const wchar_t*)jscFunctionName, (*item).name))
				{
					if(isEqual((const wchar_t*)jscFunctionName, L"QueryLectureStateSet"))
					{
						int h = 0;
						int g = h;
						h = g + 1;
					}
					int i = 0;
					bool paramsAreCorrect = false;
					ParamsList params=(*item).params;

					if( params.size()==0 && argc==0)
						paramsAreCorrect = true;
					if (argc != params.size()){
						JS_ReportWarning(cx, "Service method '%s' must get exactly %d arguments", cLPCSTR((wchar_t*)jscFunctionName), params.size());
						return JS_TRUE;
					}
					for (ParamsList::iterator paramItem = params.begin(); paramItem != params.end() && !paramsAreCorrect; paramItem++)
					{
						jsval vArg = argv[i];
						bool bArrayArg = false;
						bool bObjectArg = false;
						if(JSVAL_IS_REAL_OBJECT(vArg)){
							JSObject* jsoArg = JSVAL_TO_OBJECT(vArg);
							if(JS_IsArrayObject(cx, jsoArg)){
								bArrayArg = true;
							}else{
								bObjectArg = true;
							}
						}
						// Тип аргумента совпал с типом параметра функции
						if ((JSVAL_IS_STRING(vArg) && (*paramItem).type == JSON_STR)
							|| (JSVAL_IS_NUMBER(vArg) && ((*paramItem).type == JSON_NUMBER || (*paramItem).type == JSON_NUM))
							|| (bArrayArg && (*paramItem).type ==JSON_ARRAY)
							|| (bObjectArg && (*paramItem).type == JSON_OBJECT)
							|| (bObjectArg && (*paramItem).type == JSON_STR)
						){
							//TRACE(cx, "Argument type is OK\n",(*item).name);
							if (i!=0) 
								sJSONMethodDescription += L", ";
							sJSONMethodDescription += L"{ 'param': ";

							if(bArrayArg){
								sJSONMethodDescription += L"[";
								bool bFirstElem = true;
								JSObject* jsoArr = JSVAL_TO_OBJECT(vArg);
								jsuint uiLen=0;
								if(JS_GetArrayLength(cx,jsoArr,&uiLen)){
									for(unsigned int jj=0; jj<uiLen; jj++){
										jsval v;
										if(JS_GetElement(cx, jsoArr, jj, &v)){
											if(JSVAL_IS_NUMBER(v)){
												JSString* jss = JS_ValueToString(cx, v);
												if(!bFirstElem) sJSONMethodDescription += L","; bFirstElem = false;
												sJSONMethodDescription += (const wchar_t*)JS_GetStringChars(jss);
											}else if(JSVAL_IS_STRING(v)){
												JSString* jss = JSVAL_TO_STRING(v);
												mlString sVal = (const wchar_t*)JS_GetStringChars(jss);
												PrepareJSONParam(sVal);
												if(!bFirstElem) sJSONMethodDescription += L","; bFirstElem = false;
												sJSONMethodDescription += L'\'';
												sJSONMethodDescription += sVal.c_str();
												sJSONMethodDescription += L'\'';
											}
											
										}
									}
								}
								sJSONMethodDescription += L"]";
							}else if(bObjectArg && (*paramItem).type == JSON_OBJECT){
								sJSONMethodDescription += (wchar_t *)(pJSON->ObjectToJSON(cx, JSVAL_TO_OBJECT(argv[i]))).c_str();
							}else{
								mlString smlArg;
								if(bObjectArg){
									smlArg = pJSON->ObjectToJSON(cx, JSVAL_TO_OBJECT(argv[i]));
								}else{
									JSString* sArg;
									if (JSVAL_IS_STRING(argv[i]) || JSVAL_IS_NUMBER(argv[i]))
										sArg = JS_ValueToString(cx, argv[i]);

									jschar* scArg = JS_GetStringChars(sArg);
									smlArg = (const wchar_t*)scArg;
								}
								PrepareJSONParam(smlArg);

								if((*paramItem).type == JSON_NUM){
									sJSONMethodDescription += (wchar_t *)smlArg.c_str();
								}else
									sJSONMethodDescription = sJSONMethodDescription + L"'"+ (wchar_t *)smlArg.c_str() + L"'";
							}
							sJSONMethodDescription += L"}";
							
							// проверяем на конец списка аргументов
							if (i==(argc-1)||(argc==0))
							{
								paramItem++;
								// Параметры в списке аргументов и в шаблоне функции закончились
								if (paramItem==params.end())
								{
									paramsAreCorrect = true;
									break;
								}
							}
							i++;
						}
						else
						{
							int iRequiredType = (*paramItem).type;

							int iActualType = -1;
							if(JSVAL_IS_STRING(vArg)){
								iActualType = JSON_STR;
							}else if(JSVAL_IS_NUMBER(vArg)){
								iActualType = JSON_NUM;
							}else if(bArrayArg){
								iActualType = JSON_ARRAY;
							}else if(bObjectArg){
								iActualType = JSON_OBJECT;
							}

							const char* lpcRequired = GetJSOnParameterType( iRequiredType);
							const char* lpcCurrent = GetJSOnParameterType( iActualType);

							TRACE(cx, "Ошибка типа в вызове параметра %d функции %s\n",i+1,cLPCSTR((*item).name));
							TRACE(cx, "Ожидаемый тип %s, пришел тип %s\n", lpcRequired, lpcCurrent);
							return JS_FALSE;

						}
					}
					if( paramsAreCorrect)
					{
						// Все нормально. Нужно передать все параметры в serviceMan
						//TRACE(cx, "That's all right\n");
						sJSONMethodDescription =sJSONMethodDescription + L"] }";
						JSObject* jsoRequest = mlHTTPRequest::newJSObject(cx);


						//TRACE(cx, "That's all right pJSON->iID=%d\n",pJSON->iID);
						mlHTTPRequest* pRequest = (mlHTTPRequest*)JS_GetPrivate(cx, jsoRequest);
						//pRequest->SetID(pJSON->requestID); 
						int id = GPSM(cx)->GetRequestList()->GetNextServiceIDRequest();
						pRequest->SetID(id); 
						pRequest->SetRequestID(id); 
						GPSM(cx)->GetRequestList()->AddRequest(pRequest);
						jsval vStatus = INT_TO_JSVAL(1);
						JS_SetProperty(cx, jsoRequest, "status", &vStatus);

						SAVE_FROM_GC(cx, pSM->GetPlServerGO(), jsoRequest) // раньше ссылка сохранялась в jsoSRecorder зачем-то %)
						if (pJSON->manager != NULL)
							pJSON->manager->ExecMethod((int)pRequest->GetID(), (unsigned int)pJSON->mjso, sJSONMethodDescription.GetBuffer()); 

						//pJSON->requestID++;
						*rval = OBJECT_TO_JSVAL(jsoRequest);
						return JS_TRUE;
					}
				}
			}
			return JS_FALSE;
		}
		break;
	}
	
	// получаем имя метода js-объекта(mymethod)

	// вызываем соответствующий метод сервиса на сервере
	return JS_TRUE;
}

bool mlJSONRPCJSO::getMethodParams(JSContext *cx, JSObject *obj, JSONMethod *method)
{
	jsval vJSONProps;
	int paramIndex=0;
	while (true)
	{
		JSONPair param;
		JS_GetElement(cx,obj,paramIndex,&vJSONProps);
		if (vJSONProps==JSVAL_VOID)
			break;
		JSObject *objParam=JSVAL_TO_OBJECT(vJSONProps);

		jsval vName;
		JS_GetProperty(cx, objParam, "name", &vName );
		if (!JSVAL_IS_STRING(vName))
			return false;
		JSString* jss=JSVAL_TO_STRING(vName);
		param.name = (wchar_t*)JS_GetStringChars(jss);

		jsval vType;
		JS_GetProperty(cx, objParam, "type", &vType );
		if (!JSVAL_IS_STRING(vType))
			return false;
		jss=JSVAL_TO_STRING(vType);
		wchar_t *type = (wchar_t*)JS_GetStringChars(jss);
		JSData paramType;
		if (_wcsicmp(type,L"STRING")==0) paramType = JSON_STR;
		else{ 
			if ((_wcsicmp(type,L"double")==0)||(_wcsicmp(type,L"integer")==0)||(_wcsicmp(type,L"decimal")==0) ){
				paramType = JSON_NUMBER;
			}else if(_wcsicmp(type,L"num")==0 || _wcsicmp(type,L"number")==0){ 
				paramType = JSON_NUM;
			}else{
				if (_wcsicmp(type,L"Array")==0){
					paramType = JSON_ARRAY;
				}else if (_wcsicmp(type,L"object")==0){
					paramType = JSON_OBJECT;
				}else{
					paramType = JSON_UNRECOGNIZE;
				}
			}
		};

		param.type =  paramType;

		method->params.push_back(param);
		paramIndex++;
	}
	return true;
}

// Получаем методы сервиса
bool mlJSONRPCJSO::getServiceMethods(JSContext *cx, JSObject *obj)
{
	jsval vJSONProps;
	int methodIndex=0;
	while (true)
	{
		JSONMethod method;
		JS_GetElement(cx,obj,methodIndex,&vJSONProps);
		if (vJSONProps==JSVAL_VOID)
			break;
		JSObject *objMethod=JSVAL_TO_OBJECT(vJSONProps);

		jsval vName;
		JS_GetProperty(cx, objMethod, "name", &vName );
		if (!JSVAL_IS_STRING(vName))
			return false;
		JSString* jss=JSVAL_TO_STRING(vName);
		method.name = (wchar_t*)JS_GetStringChars(jss);
		

		jsval vParams;
		JS_GetProperty(cx, objMethod, "parameters", &vParams );
		JSObject *objParams=JSVAL_TO_OBJECT(vParams);
		if (!getMethodParams(cx,objParams,&method))
			return false;

		service.methods.push_back(method);
		methodIndex++;
	}
	return true;
}

// Пришла нотификация с сервака
bool mlJSONRPCJSO::Notify(JSContext *cx, mlString sMethodResult, bool isService){
	jsval vJSONProps = NULL;
	JSBool bR = JS_EvaluateUCScript(cx, JS_GetGlobalObject(mcx), (const jschar*)sMethodResult.c_str(), sMethodResult.size(), "result", 1, &vJSONProps);
	if (JSVAL_IS_REAL_OBJECT(vJSONProps))
	{	JSObject *obj=JSVAL_TO_OBJECT(vJSONProps);
		JS_GetElement(mcx,obj,0,&vJSONProps);
		if (JSVAL_IS_REAL_OBJECT(vJSONProps))
		{
			JS_SetProperty(mcx, mjso, "result", &vJSONProps);
			OnEventNotifyReceived();
		}
	}
	return true;
}

// json specification
bool mlJSONRPCJSO::Initialize(JSContext *cx, mlString sGetServiceQueryResult ){
//	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);

	if (bInitialiazed)
	{
		CallListeners(EVNM_onConnectionRestored);
		return true;
	}

	bInitialiazed = true;

	if(errorCode > 0){
		JSObject* jsoEvent = GPSM(mcx)->GetEventGO();
		jsval vErrorCode = INT_TO_JSVAL(errorCode);
		JS_SetProperty(mcx, jsoEvent, "errorCode", &vErrorCode);
		CallListeners(EVNM_onError);
		return false;
	}

	jsval vJSONProps = NULL;
	JSBool bR = JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), (const jschar*)sGetServiceQueryResult.c_str(), sGetServiceQueryResult.size(), "responce", 1, &vJSONProps);
	if (JSVAL_IS_REAL_OBJECT(vJSONProps))
	{	JSObject *obj=JSVAL_TO_OBJECT(vJSONProps);
		JS_GetElement(cx,obj,0,&vJSONProps);
		if (JSVAL_IS_REAL_OBJECT(vJSONProps))
		{
			JSObject *obj=JSVAL_TO_OBJECT(vJSONProps);
			jsval vObjectName = NULL;
			JS_GetProperty(cx, obj, "objectName", &vObjectName );
			if (JSVAL_IS_STRING(vObjectName) )
			{
				JSString* jss=JSVAL_TO_STRING(vObjectName);
				const wchar_t *sName = (const wchar_t*)JS_GetStringChars(jss);;
				service.name = sName;
				jsval vMethods = NULL;
				JS_GetProperty(cx, obj, "methods", &vMethods );
				obj=JSVAL_TO_OBJECT(vMethods);
				getServiceMethods(cx, obj);
			}
			else 
			{   SetErrorCode(0xFFFFF);
				JSObject* jsoEvent = GPSM(mcx)->GetEventGO();
				jsval vErrorCode = INT_TO_JSVAL(errorCode);
				JS_SetProperty(mcx, jsoEvent, "errorCode", &vErrorCode);
				CallListeners(EVNM_onError);
				return false;
			}
		}
	}

	MethodList methods =  service.methods;
	for (MethodList::iterator iter = methods.begin();
		iter != methods.end();
		iter++) {
			JSFunction *f = JS_DefineFunction(cx, mjso, cLPCSTR((*iter).name), JSFUNC_JSONMethod, 0, 0);
			int i=9;
			i=4;
		}	

	CallListeners(EVNM_onReceived);

	return true;

}

/*
void mlJSONRPCJSO::CallListeners(const wchar_t* apwcEventName)
{
	jsval vFunc = JSVAL_NULL;
	if(apwcEventName == NULL) 
		return;
	// если есть свойство onXXX
	if(JS_GetUCProperty(mcx, mjso, apwcEventName, wcslen(apwcEventName), &vFunc) == JS_FALSE)
		return;
	if(!JSVAL_IS_STRING(vFunc)){
		if(!JSVAL_IS_REAL_OBJECT(vFunc))
			return;
		if(!JS_ObjectIsFunction(mcx, JSVAL_TO_OBJECT(vFunc)))
			return;
	}
	if(vFunc != JSVAL_NULL){
		// вызываем функцию
		jsval v;
		if(JSVAL_IS_STRING(vFunc)){
			JSString* jss = JSVAL_TO_STRING(vFunc);
			JS_EvaluateUCScript(mcx, mjso, JS_GetStringChars(jss), JS_GetStringLength(jss), 
				cLPCSTRq(apwcEventName), 1, &v);
		}else{
			JS_CallFunctionValue(mcx, mjso, vFunc, 0, NULL, &v);
		}
	}
}
*/

void mlJSONRPCJSO::OnEventNotifyReceived()
{
	if(errorCode > 0){
		JSObject* jsoEvent = GPSM(mcx)->GetEventGO();
		jsval vErrorCode = INT_TO_JSVAL(errorCode);
		JS_SetProperty(mcx, jsoEvent, "errorCode", &vErrorCode);
		CallListeners(EVNM_onError);
		return;
	}

	CallListeners(EVNM_onNotified);
}

void mlJSONRPCJSO::QueryService()
{
	if (manager == NULL)
	{
		JS_ReportError(mcx, "Service manager is not set");
		return;
	}
	{ 
		// tandy: временно делаем их енумерабельными, чтобы можно было их очистить в ResetPlayerServer
		// предполагается, что по timeout-у по любому будут приходить уведомления о том, что сервис не создался 
		// и объект после этого освобождаться
		jsval v=OBJECT_TO_JSVAL(mjso); 
		JS_DefineUCProperty(mcx, GPSM(mcx)->GetPlServerGO(), 
			(jschar*)(void*)(&(mjso)), sizeof(void*)/sizeof(jschar), 
			v, NULL, NULL, JSPROP_ENUMERATE); 
	}
	if (OMS_FAILED(manager->GetJSONService((unsigned int)mjso, serviceName.c_str())))
	{
		FREE_FOR_GC(mcx, GPSM(mcx)->GetPlServerGO(),mjso);
		SetErrorCode(0xFFFF); // ?? tandy: возможно надо какой-то более правильный код устанавливать
	}
}

}

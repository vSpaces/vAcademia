#include "mlRMML.h"
#include "mlSPParser.h"
#include "mlSoapJSO.h"
#include "config/prolog.h"
#include "rmmlsafe.h"
#include <string.h>
#include <stdio.h>

namespace rmml {

mlSoapJSO::mlSoapJSO(){}
mlSoapJSO::~mlSoapJSO(void){
/*	
	destroy HERE
	*/
}
#define JSPN_XMLDocument L"XMLDocument"
	
JSPropertySpec mlSoapJSO::_JSPropertySpec[] = {
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlSoapJSO::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Soap,mlSoapJSO,0)
	MLJSCLASS_ADDPROPFUNC
//	MLJSCLASS_ADDPROPENUMOP
MLJSCLASS_IMPL_END(mlSoapJSO)

JSBool mlSoapJSO::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlSoapJSO);
	default:
		break;
	SET_PROPS_END;
	return JS_TRUE;
}

JSBool mlSoapJSO::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlSoapJSO);
	default:
		break;
  GET_PROPS_END;
	return JS_TRUE;
}

struct WsdlType{
	char * type;
	char * val;
};


WsdlType * ConvertJS(jsval jval){
	WsdlType * wt = MP_NEW( WsdlType);
	if (JSVAL_IS_STRING(jval)){
		JSString * jssP = JSVAL_TO_STRING(jval);
		jschar* jscP = JS_GetStringChars(jssP); //wchar array
		size_t requiredSize2 = 0;
		size_t converted = 0;
		wcstombs_s(&requiredSize2, NULL, 0, (const wchar_t*)jscP, 0); // C4996
		unsigned char * pwc2 = MP_NEW_ARR( unsigned char, requiredSize2 + 1);
		wcstombs_s(&converted, (char *) pwc2, requiredSize2+1, (const wchar_t*)jscP, requiredSize2 ); 
		char * pwc3 = (char *)pwc2;
		
		wt->val = pwc3;
		wt->type = "xsd:string";
		return wt;
	}
	if (JSVAL_IS_NUMBER(jval)){
		char bfr[20];
		if (JSVAL_IS_INT(jval)){ 
			jsint jsn = JSVAL_TO_INT(jval);
			_itoa_s((int)jsn,bfr,20,10);
			
		}
		if (JSVAL_IS_DOUBLE(jval))
		{	
			jsdouble * jsd = JSVAL_TO_DOUBLE(jval);
			//sprintf(bfr, "%g", *jsd);
			rmmlsafe_sprintf(bfr, sizeof(bfr), 0, "%g", *jsd);
		}	
		wt->val = bfr;
		wt->type = "xsd:decimal";
		return wt;
	}
	if (JSVAL_IS_BOOLEAN(jval)){
		JSBool jsb = JSVAL_TO_BOOLEAN(jval);
		if (jsb==JS_TRUE) wt->val = "1";
		if (jsb==JS_FALSE) wt->val = "0";
		wt->type = "xsd:boolean";
		return wt;
	}
	return wt;
}


JSBool mlSoapJSO::JSFUNC_soapMethod(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlSoapJSO* pSoap =(mlSoapJSO*)JS_GetPrivate(cx,obj);
	
	JSStackFrame* fp = NULL;
	fp = JS_FrameIterator(cx, &fp);
	
	for(;fp != NULL; fp = JS_FrameIterator(cx, &fp)){
	if(JS_GetFrameFunction(cx, fp) != NULL){
		JSFunction* fun = JS_GetFrameFunction(cx, fp);
		JSString* jssFun = JS_GetFunctionId(fun);

		jschar* jscP = JS_GetStringChars(jssFun); //wchar array
		size_t requiredSize2 = 0;
		size_t converted = 0;
		wcstombs_s(&requiredSize2, NULL, 0, (const wchar_t*)jscP, 0); // C4996
		unsigned char * pwc2 = MP_NEW_ARR( unsigned char, requiredSize2 + 1);
		wcstombs_s(&converted, (char *) pwc2, requiredSize2 + 1, (const wchar_t*)jscP, requiredSize2); 
		TRACE(cx,"Funk_name =%s ",(char *)pwc2);
		MP_DELETE_ARR(pwc2);
		
		if(jssFun == NULL) return NULL;
		jschar* jscFun = JS_GetStringChars(jssFun);
		SoapService  *s;
		s = &pSoap->serv;
		TRACE(cx,s->name);
		OperationList test =  s->operations;
		for (OperationList::iterator oli = test.begin();
			oli != test.end();
			oli++) {
				if(isEqual((const wchar_t*)jscFun, cLPWCSTR((*oli).name))){
					//создать wsdl-запрос на сервер
					TRACE(cx, "SOAPMETH!!! = %s\n",(*oli).name);
					SoapMessage inp = (*oli).input;

					int i =0;
					for (PartList::iterator pli = inp.parts.begin();
						pli != inp.parts.end();
						pli++) {

								//TRACE(cx, "part %s type %d\n",(*pli).name,(*pli).type);
								//TRACE(cx, "arg_type %d\n",JSVAL_IS_NUMBER(argv[i]));
								
								JSObject*  jsv = NULL;
								JSIntn jsin = JS_FALSE;
								if(JSVAL_IS_OBJECT(argv[i])){
									TRACE(cx,"OBJ");
									JSObject* jsv = JSVAL_TO_OBJECT(argv[i]);
									JSIntn jsin = JS_IsArrayObject(cx,jsv);
								}
							
							if ((JSVAL_IS_STRING(argv[i])&&(*pli).type ==SOAP_STR)
								||(JSVAL_IS_NUMBER(argv[i])&&(*pli).type ==SOAP_DECIMAL)
								||(JSVAL_IS_BOOLEAN(argv[i])&&(*pli).type ==SOAP_BOOLEAN)
								||(jsin==JS_TRUE&&(*pli).type ==SOAP_ARRAY)
								||((argc==0)&&(*pli).type ==SOAP_VOID))
							{
								
								
								TRACE(cx, "TIP OK\n",(*oli).name);
								//вызываем след, иначе 
								if (i==(argc-1)||(argc==0)) {
									TRACE(cx, "ARGC END\n");
									pli++;
									if (pli==inp.parts.end()) {
										TRACE(cx, "That's allright\n");
										//создаем wsdl
										const char * test = "str_param=<?xml version='1.0' ?><SOAP-ENV:Envelope " ;
										
										TRACE(cx,"strdef= %s\n",s->definitions);
										char string[1024];
										strcpy_s(string,1024,test);
										strcat_s(string,s->definitions);
									    strcat_s(string,">");
									//	TRACE(cx,"string= %s",string);
										//uid
										strcat_s(string,"<SOAP-ENV:Header><ns1:uid>");
										strcat_s(string,s->uid);									
										strcat_s(string,"</ns1:uid></SOAP-ENV:Header>");
										
										strcat_s(string,"<SOAP-ENV:Body><ns1:");
										strcat_s(string,(*oli).name);
										strcat_s(string,">");
											TRACE(cx,"string= %s",string);
										int j=0;
										for (pli = inp.parts.begin();
											pli != inp.parts.end();
											pli++) {
											strcat_s(string,"<");
											strcat_s(string,(*pli).name);
											//array
											
											strcat_s(string," xsi:type=");
												WsdlType * mtype;	
											if ((*pli).type ==SOAP_ARRAY){	strcat_s(string,"\"SOAP-ENC:Array\">");
											
												
											
											if (JS_IsArrayObject(cx,jsv)==JS_TRUE){	
												unsigned long lengthp;
												JS_GetArrayLength(cx, jsv, &lengthp);
												int i;
												for (i = 0; i!=lengthp; i++)
												{
													strcat_s(string,"<item xsi:type=\"");
													jsval *vp = MP_NEW( jsval);
													JS_GetElement(cx, jsv, i, vp);

													mtype = ConvertJS(*vp);
													strcat_s(string,mtype->type);
													strcat_s(string,"\">");
													strcat_s(string,mtype->val);
													strcat_s(string,"</item>");
													TRACE(cx,"TEST %s\n",string);
													
												}
															
											}
											}//if ((*pli).type ==SOAP_ARRAY)
											else{
												if (argc==0){
												strcat_s(string,"ns1:void\">VOID");
												}else{
												
												jsval * vp = MP_NEW( jsval);
												*vp = argv[j];
												mtype = ConvertJS(*vp);
												strcat_s(string,mtype->type);
												strcat_s(string,"\">");
												strcat_s(string,mtype->val);
												}
											}
											j++;//next argv[]
											strcat_s(string,"</");
											strcat_s(string,(*pli).name);
											strcat_s(string,">");
											
										}
										//</ns1:operationname></SOAP-ENV:Body></SOAP-ENV:Envelope>
										strcat_s(string,"</ns1:");
										strcat_s(string,(*oli).name);
										strcat_s(string,"></SOAP-ENV:Body></SOAP-ENV:Envelope>");
										

										
										//string to wchar array pwc			
										const char    *pmbhello = string;
	 									size_t requiredSize = 0;
										mbstowcs_s(&requiredSize, NULL, 0, pmbhello, 0);
										wchar_t * pwc = MP_NEW_ARR( wchar_t, requiredSize + 1);
										if (! pwc)
										{
											TRACE(cx,"Memory allocation failure.\n");
											return 1;
										}
										
										size_t size = 0;
										mbstowcs_s(&size, pwc, requiredSize + 1, pmbhello, requiredSize); // C4996
										if (size == (size_t) (-1))
										{
												TRACE(cx,"Couldn't convert string--invalid multibyte character.\n");
										}
						             	//end string to wchar array pwc		
										
										mlSceneManager* pSM = GPSM(cx);
							    		cm::cmICommunicationManager* pComMan = pSM->GetContext()->mpComMan;
										if(pComMan == NULL){
											JS_ReportError(cx, "Communication manager is not set");
										return JS_TRUE;
										}
									
										const wchar_t* wchQwery = L"soap/redirect.php";
										const wchar_t* wchParam = pwc; 
										//int iErrorCode = 0;
										//TRACE(cx,"before query %d\n", iErrorCode);
										GPSM(cx)->SetServerCallbacks();
										int iID= pSM->GetContext()->mpComMan->SendQuery(wchQwery, wchParam);
										
										//new
										//TRACE(cx,"after query iId=%d iErrorCode = %d\n", iID, iErrorCode);
										TRACE(cx,"after query iId=%d \n", iID);
										JSObject* jsoRequest = mlHTTPRequest::newJSObject(cx);
										if(/*iErrorCode > 0 ||*/ iID < 0){
										//TRACE(cx,"ErrorCode %d", iErrorCode);
											jsval vStatus = INT_TO_JSVAL(0);
											JS_SetProperty(cx, jsoRequest, "status", &vStatus);
											//jsval vErrorCode = INT_TO_JSVAL(iErrorCode);
											jsval vErrorCode = INT_TO_JSVAL(1);
											JS_SetProperty(cx, jsoRequest, "errorCode", &vErrorCode);
											const wchar_t* pwcError = L"Unknown server error";
										/*	switch(iErrorCode){
												// ??
											}*/
											jsval vError = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)pwcError));
											JS_SetProperty(cx, jsoRequest, "errorCode", &vErrorCode);
										}else{
											mlHTTPRequest* pRequest = (mlHTTPRequest*)JS_GetPrivate(cx, jsoRequest);
											pRequest->SetRequestID(GPSM(cx)->GetRequestList()->GetNextServiceIDRequest()); 
											pRequest->SetID(iID);
											GPSM(cx)->GetRequestList()->AddRequest(pRequest);
											jsval vStatus = INT_TO_JSVAL(1);
											JS_SetProperty(cx, jsoRequest, "status", &vStatus);
										}
										*rval = OBJECT_TO_JSVAL(jsoRequest);
										
										return JS_TRUE;
										}else {
										TRACE(cx, "Недостаточно параметров в вызове ф-ции  %s\n",(*oli).name);	
										return JS_FALSE;
											
									}
								}else i++;
							}else
							{
								TRACE(cx, "ошибка типа в вызове параметра %d функции %s\n",i+1,(*oli).name);	
								return JS_FALSE;
							}
							
						}
				}
				}return JS_FALSE;

	}
		break;
	}
	
	// получаем имя метода js-объекта(my method)

	// вызываем соответствующий метод сервиса на сервере
	return JS_TRUE;
}

mlresult mlSoapJSO::ParseWSDL(mlXMLBuf* apXMLBuf){
return ML_OK;
}

struct ContextBuf{
	wchar_t * str;
	int pos;
};

int InputReadCallbackSoap(void * context,char * buffer, int len){
	ContextBuf * pXBuf = (ContextBuf*)context;

	if ( (pXBuf->pos+len) > (int)(wcslen(pXBuf->str)) )
		len = wcslen(pXBuf->str)- pXBuf->pos;
	size_t converted = 0;
	wcstombs_s(&converted, buffer, len, pXBuf->str+pXBuf->pos, len);

	pXBuf->pos+=len;
	return len;
	
}

int InputCloseCallbackSoap(void * context){
	return 0;
}

void mlSoapGenericErrorFunc	(void * ctx, const char * msg, ...){
    va_list args;
    va_start(args, msg);
	char sErr[5000]="";
    //vsprintf(sErr, msg, args);
	rmmlsafe_vsprintf(sErr, sizeof(sErr), 0, msg, args);
    va_end(args);
int hh=0;
}


const char* mlNextReadetTypeEl(mlSceneManager::mlLibXml2Funcs* pLibXml2Funcs, xmlTextReaderPtr reader)
{
	int ret = pLibXml2Funcs->xmlTextReaderRead(reader);
	if(ret != 1)
		return NULL;
	int iNodeType = pLibXml2Funcs->xmlTextReaderNodeType(reader);
			
	if(iNodeType == XML_READER_TYPE_ELEMENT) 
	{
		const char* pcTagName = (const char*)pLibXml2Funcs->xmlTextReaderName(reader);
		return pcTagName;
	}
	if(mlNextReadetTypeEl(pLibXml2Funcs,reader)==NULL) return NULL;
	return NULL; // ?? Tandy
}

const char* mlGetLocalName(const char* Fname){
	const char * str = strstr(Fname,":");
	const char * localname;
	if (str!=NULL)
		localname = str+1;
	else localname = Fname;
	return localname;
}

const char* mlGetTagAttrValue(mlSceneManager::mlLibXml2Funcs* pLibXml2Funcs, xmlTextReaderPtr reader,const char * Nattr){
	
	
	while(pLibXml2Funcs->xmlTextReaderMoveToNextAttribute(reader)){
		const char* pcName = (const char*)pLibXml2Funcs->xmlTextReaderName(reader);		
		if (_stricmp(pcName,Nattr)==0){
			const char * type = (const char*)pLibXml2Funcs->xmlTextReaderValue(reader);
			return (const char*)pLibXml2Funcs->xmlTextReaderValue(reader); 
		}
	}	
	
	return NULL;
}
//mstr - wsdl conf from server
bool mlSoapJSO::Initialize(wchar_t* name,JSContext *cx, JSObject *obj,wchar_t* mstr,wchar_t* uid){
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	
	size_t converted = 0;
	size_t requiredSize = 0;
	wcstombs_s(&requiredSize, NULL, 0, uid, 0); // C4996
	unsigned char * pwc;
	pwc = MP_NEW_ARR( unsigned char, requiredSize + 1);
	wcstombs_s(&converted, (char *) pwc, requiredSize + 1, uid, requiredSize); 
	const char* err = strstr((const char *)pwc,"-#-#-");
		if (err==NULL){
			serv.uid = (const char *)pwc;
			TRACE(cx,"uid = %s", serv.uid);
		}else{
			mlTraceError(cx, "Error service initialazing %s",(const char *)pwc);
			return false;
		}
	MessageList mslst;
	OperationList oplst;

	mlSceneManager::mlLibXml2Funcs* pLibXml2Funcs = GPSM(mcx)->GetLibXml2Funcs();
	pLibXml2Funcs->xmlSetGenericErrorFunc(NULL, mlSoapGenericErrorFunc);

	xmlParserInputBufferPtr inputBuffer = NULL;

	

	
	ContextBuf * sRes = MP_NEW( ContextBuf);
	sRes->str = mstr;

	sRes->pos = 0;

	inputBuffer = pLibXml2Funcs->xmlAllocParserInputBuffer(XML_CHAR_ENCODING_NONE);
	if (inputBuffer == NULL){
		mlTrace(cx, "Cannot allocate XML parser input buffer");
		return false;
	}
	//new inputBuffer->context = psRes; // ctxt;
	inputBuffer->context = sRes; 
	//old inputBuffer->context = pRes; 
	inputBuffer->readcallback = InputReadCallbackSoap;
	inputBuffer->closecallback = InputCloseCallbackSoap;

	xmlTextReaderPtr reader;
	reader = GPSM(cx)->GetLibXml2Funcs()->xmlNewTextReader(inputBuffer, (const char *)L"muwsdl.wsdl");
	
	std::vector<JSObject*> vParents;
	if(reader != NULL){
		JSObject* jsoXMLDocument = mlXMLDocument::newJSObject(cx);
		jsval v = OBJECT_TO_JSVAL(jsoXMLDocument);
		JS_DefineUCProperty(cx, mjso, (const jschar*)JSPN_XMLDocument, -1, v, NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY);	
		
		mlXMLDocument* pXMLDoc=(mlXMLDocument*)JS_GetPrivate(cx,jsoXMLDocument);
		std::vector<JSObject*> vParents;
		vParents.push_back(jsoXMLDocument);
		const char* pcTagName = mlNextReadetTypeEl(pLibXml2Funcs,reader); int NeedDepth;
		for(;;){
		
			if(pcTagName == NULL) break;
else TRACE(cx," %s =MUtag\n", pcTagName);
				JSString* jssTagName = JS_NewStringCopyZ(mcx, pcTagName);

				if (_stricmp(pcTagName,"definitions")== 0){
					if(pLibXml2Funcs->xmlTextReaderHasAttributes(reader)){
						char def_str[1024];
						strcpy_s(def_str,1024,"\0");
						while(pLibXml2Funcs->xmlTextReaderMoveToNextAttribute(reader)){
							char* pcName = (char*)pLibXml2Funcs->xmlTextReaderName(reader);
							char* pcCont= (char*)pLibXml2Funcs->xmlTextReaderValue(reader);
							strcat_s(def_str,pcName);
							strcat_s(def_str,"=\"");
							strcat_s(def_str,pcCont);
							strcat_s(def_str,"\" ");
							TRACE(cx,"definitions name=%s content=%s\n",pcName,pcCont);
						}
						serv.definitions = MP_NEW_ARR( char, strlen(def_str)+1);
						strcpy_s((char *)serv.definitions, strlen(def_str)+1, def_str);
						
						TRACE(cx,"definitions1 %s\n, length %d\n ",def_str,strlen(def_str));
						TRACE(cx,"definitions2 %s\n, length %d\n ",serv.definitions,strlen(serv.definitions));
					}
				}
				if (_stricmp(pcTagName,"message")== 0)
				{
					SoapMessage msg;
					NeedDepth = pLibXml2Funcs->xmlTextReaderDepth(reader);
					if(pLibXml2Funcs->xmlTextReaderHasAttributes(reader)){
						while(pLibXml2Funcs->xmlTextReaderMoveToNextAttribute(reader)){
						const char* pcName = (const char*)pLibXml2Funcs->xmlTextReaderName(reader);		
						if (_stricmp(pcName,"name")==0)
							msg.name = (const char*)pLibXml2Funcs->xmlTextReaderValue(reader); 
						}		
					}else TRACE(cx," Message need to have an attribute");
					if (msg.name==NULL) TRACE(cx," Message need to have a name");
					
					
					
//TRACE(cx," inside msg");
//TRACE(cx," msg depth =%d\n",iNodeDepth);
					pcTagName = mlNextReadetTypeEl(pLibXml2Funcs,reader);
					
					int depth = pLibXml2Funcs->xmlTextReaderDepth(reader);
					while (depth > NeedDepth)
					{
						if (_stricmp(pcTagName,"part")== 0){
							const char* type;					
							SoapMessagePart part;
							bool fl = false;
							
							if(pLibXml2Funcs->xmlTextReaderHasAttributes(reader)){
							while(pLibXml2Funcs->xmlTextReaderMoveToNextAttribute(reader)){
							
							const char* pcName = (const char*)pLibXml2Funcs->xmlTextReaderName(reader);		
							if (_stricmp(pcName,"name")==0){
								part.name = (const char*)pLibXml2Funcs->xmlTextReaderValue(reader);
								
								fl = true;
							}
							if (_stricmp(pcName,"type")==0){
								type = (const char*)pLibXml2Funcs->xmlTextReaderValue(reader); 
								type = mlGetLocalName(type);
								JSData TPart;
								if (_stricmp(type,"string")==0) TPart = SOAP_STR;
								else{ 
									if ((_stricmp(type,"double")==0)||(_stricmp(type,"integer")==0)||(_stricmp(type,"int")==0)||(_stricmp(type,"decimal")==0) )TPart = SOAP_DECIMAL;
									else{ 
										if (_stricmp(type,"boolean")==0) TPart = SOAP_BOOLEAN;
										else{
											if (_stricmp(type,"Array")==0) TPart = SOAP_ARRAY;
											else{
												if (_stricmp(type,"void")==0) TPart = SOAP_VOID;
												else TPart = SOAP_UNRECOGNIZE;
											}
										}
									}
								};
								
								part.type = TPart;
								fl= true;
							}
							}
							if (fl) msg.parts.push_back(part);
							//по std::list
							//TRACE(cx," part name =%s part type=%d \n",msg.parts.pop_back());	
				
							}	

		
						}else TRACE(cx," message need to have only a part");
							
						pcTagName = mlNextReadetTypeEl(pLibXml2Funcs,reader);
						depth = pLibXml2Funcs->xmlTextReaderDepth(reader);
					}
					mslst.push_back(msg);
				}else 
				{
				if (_stricmp(pcTagName,"portType")== 0) 
				{
					NeedDepth = pLibXml2Funcs->xmlTextReaderDepth(reader);
					
					pcTagName = mlNextReadetTypeEl(pLibXml2Funcs,reader);
					int depthOp = pLibXml2Funcs->xmlTextReaderDepth(reader);
					while (depthOp > NeedDepth)
					{
						if (_stricmp(pcTagName,"operation")== 0){
						int fl =0;
						SoapOperation op;	
							
							if(pLibXml2Funcs->xmlTextReaderHasAttributes(reader)){
							while(pLibXml2Funcs->xmlTextReaderMoveToNextAttribute(reader)){
								const char* pcName = (const char*)pLibXml2Funcs->xmlTextReaderName(reader);		
								if (_stricmp(pcName,"name")==0)
									op.name = (const char*)pLibXml2Funcs->xmlTextReaderValue(reader);
TRACE(cx," op.name = %s \n", op.name);
								}
							}else TRACE(cx," Operation need to have an attribute=name\n");
		

							
						pcTagName = mlNextReadetTypeEl(pLibXml2Funcs,reader);
						int depthMsg = pLibXml2Funcs->xmlTextReaderDepth(reader);
						while (depthMsg > depthOp )
						{
						int muloop; 
						if (_stricmp(pcTagName,"input")== 0){muloop=1;}else
						if (_stricmp(pcTagName,"output")== 0){muloop=2;}else{muloop=0;}
						if ((muloop== 1)||(muloop== 2)){
							if(pLibXml2Funcs->xmlTextReaderHasAttributes(reader)){
							while(pLibXml2Funcs->xmlTextReaderMoveToNextAttribute(reader)){
								const char* pcName = (const char*)pLibXml2Funcs->xmlTextReaderName(reader);		
								if (_stricmp(pcName,"message")==0){
									const char* PTname = (const char*)pLibXml2Funcs->xmlTextReaderValue(reader);
									PTname = mlGetLocalName(PTname);
								
									for (MessageList::iterator mli = mslst.begin();
										mli != mslst.end();
										mli++) {
											
											if(_stricmp((*mli).name,PTname)==0)	{
												if (muloop==1){ fl++; op.input = (*mli);
												}
												if (muloop==2) {fl++; op.output = (*mli);
												}
//TRACE(cx," mlin =%s \n",(*mli).name);
											}		
											
									}
								}//endif 
										
							}if (fl==2) {oplst.push_back(op);}

									
							}else TRACE(cx," Input need to have an attribute=name");
							
							}
							pcTagName = mlNextReadetTypeEl(pLibXml2Funcs,reader);
							depthMsg = pLibXml2Funcs->xmlTextReaderDepth(reader);
						}//while (depthMsg > depthOp )
						}

					depthOp = pLibXml2Funcs->xmlTextReaderDepth(reader);
					}
					
					
				}else 
				{
					
					if (_stricmp(pcTagName,"service")== 0)  
					{
					if(pLibXml2Funcs->xmlTextReaderHasAttributes(reader)){
					while(pLibXml2Funcs->xmlTextReaderMoveToNextAttribute(reader)){
					const char* pcName = (const char*)pLibXml2Funcs->xmlTextReaderName(reader);		
					if (_stricmp(pcName,"name")==0)
						serv.name = (const char*)pLibXml2Funcs->xmlTextReaderValue(reader); 
					}		
					}	

					if (serv.name==NULL) TRACE(cx," Service need to have a name");
					serv.operations = oplst;
					TRACE(cx," SERVICE name =%s \n",serv.name);	
					OperationList test =  serv.operations;
					for (OperationList::iterator oli = test.begin();
						oli != test.end();
						oli++) {
							TRACE(cx," OPERATION name =%s \n",(*oli).name);	
							SoapMessage inp = (*oli).input;
							TRACE(cx," MESSAGE inp name =%s \n",inp.name);	
							for (PartList::iterator pli = inp.parts.begin();
							pli != inp.parts.end();
							pli++) {
								
								TRACE(cx," part name =%s part type=%d \n",(*pli).name,(*pli).type);	
							}
							SoapMessage otp = (*oli).output;
							TRACE(cx," MESSAGE otp name =%s \n",otp.name);	
							for (PartList::iterator pli1 = otp.parts.begin();
							pli1 != otp.parts.end();
							pli1++) {
								
								TRACE(cx," part name =%s part type=%d \n",(*pli1).name,(*pli1).type);	
							}
						}	
					
					pcTagName = mlNextReadetTypeEl(pLibXml2Funcs,reader);
					} 
					else pcTagName = mlNextReadetTypeEl(pLibXml2Funcs,reader);
				}
				}
		}// for(;;)
	} 
	
  //	mlTraceError(cx, (char*)str);
 //	pRes->Close();

	pLibXml2Funcs->xmlSetGenericErrorFunc(NULL, NULL);

	// получить с сервера wsdl с описанием сервиса с именами функций
	//??
	//разбираем wsdl и для каждого метода создем метод Js-объекта
	OperationList test =  serv.operations;
	for (OperationList::iterator oli = test.begin();
		oli != test.end();
		oli++) {
TRACE(cx," OPERATION name =%s \n",(*oli).name);	
			JS_DefineFunction(cx, mjso, (*oli).name, JSFUNC_soapMethod, 0, 0);
			SoapMessage inp = (*oli).input;
TRACE(cx," MESSAGE inp name =%s \n",inp.name);	
			for (PartList::iterator pli = inp.parts.begin();
				pli != inp.parts.end();
				pli++) {
TRACE(cx," part name =%s part type=%d \n",(*pli).name,(*pli).type);	
				}
				SoapMessage otp = (*oli).output;
TRACE(cx," MESSAGE otp name =%s \n",otp.name);	
				for (PartList::iterator pli1 = otp.parts.begin();
					pli1 != otp.parts.end();
					pli1++) {
TRACE(cx," part name =%s part type=%d \n",(*pli1).name,(*pli1).type);	
							}
	}	
//destroy
//	JS_DefineFunction(cx, mjso, "Destroy", JSFUNC_soapMethod, 0, 0);
return true;

}

}

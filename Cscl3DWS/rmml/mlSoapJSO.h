#pragma once

#include <string>

namespace rmml {
class mlSoapJSO:
					public mlJSClass
{
public:
	mlSoapJSO(void);
	void destroy() { MP_DELETE_THIS; }
	~mlSoapJSO(void);
MLJSCLASS_DECL2(mlSoapJSO)
private:
	static JSBool JSFUNC_soapMethod(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	static mlresult mlSoapJSO::ParseWSDL(mlXMLBuf* apXMLBuf);
	
public:
	//struct soap
	
	enum JSData{
		SOAP_STR,
		SOAP_DECIMAL,
		SOAP_ARRAY,
		SOAP_BOOLEAN,
		SOAP_VOID,
		SOAP_UNRECOGNIZE //не поддерживается
	};
	typedef struct{
		const char * name;
		jsval type;
	} SoapMessagePart;
	
	typedef std::list<SoapMessagePart> PartList;
	
	typedef struct {
		const char * name;
		PartList parts;
	}SoapMessage;
	
	typedef std::list<SoapMessage> MessageList;

	typedef struct {
		const char * name;
		SoapMessage input;
		SoapMessage output;
	}SoapOperation;
	
	typedef std::list<SoapOperation> OperationList;

	typedef struct {
		const char * name;
		const char * uid;
		OperationList operations;
		const char * definitions;
	}SoapService;
	
	SoapService serv;
	bool Initialize(wchar_t* name, JSContext *cx, JSObject *obj,wchar_t* mstr,wchar_t* uid);
	

};



}
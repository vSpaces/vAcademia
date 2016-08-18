#pragma once

#include <string>

namespace service {
	struct IBaseServiceManager;
}

namespace rmml {

#define EVNM_onStatus L"onStatus"
#define EVNM_onError L"onError"
#define EVNM_onNotified  L"onNotified"
#define EVNM_onReceived L"onReceived"
#define EVNM_onConnectionRestored L"onConnectionRestored"

class mlJSONRPCJSO:
					public mlJSClass
{
public:
	mlJSONRPCJSO(void);
	void destroy() { MP_DELETE_THIS; }
	~mlJSONRPCJSO(void);
	void SetServiceName(wchar_t* aServiceName);
	void SetManager(service::IBaseServiceManager* aManager);

	MLJSCLASS_DECL2(mlJSONRPCJSO)

private:
	static JSBool JSFUNC_JSONMethod(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	int SendQuery(const wchar_t* wchQuery, const wchar_t* wchParam);
	int errorCode;
	service::IBaseServiceManager* manager;
	MP_WSTRING serviceName;
	mlString ObjectToJSON(JSContext *cx, JSObject* ajso);
	
public:
	enum JSData{
		JSON_STR,
		JSON_NUMBER, // оборачивается в '' и в итоге приходит на сервер как строка (оставлено для совместимости) (тип в описании сервиса - integer)
		JSON_ARRAY,
		JSON_NUM,	// передеается как и положено числом (тип в описании сервиса - num || number)
		JSON_OBJECT,
		JSON_UNRECOGNIZE //не поддерживается
	};


	typedef struct{
		wchar_t * name;
		jsval type;
	} JSONPair;
	
	typedef MP_LIST<JSONPair> JSONMemberList;
	typedef MP_LIST<JSONMemberList> JSONObjectList;
	
	typedef MP_LIST<JSONPair> ParamsList;

	typedef struct _JSONMethod{
		wchar_t * name;
		ParamsList params;

		_JSONMethod():
			MP_LIST_INIT(params)
		{
			name = NULL;
		}

		_JSONMethod(const _JSONMethod& other):
			MP_LIST_INIT(params)
		{
			name = other.name;
			params = other.params;
		}
	} JSONMethod;
	
	typedef MP_LIST<JSONMethod> MethodList;

	typedef struct _JSONService{
		MP_WSTRING name;
		MethodList methods;

		_JSONService():
			MP_LIST_INIT(methods),
			MP_WSTRING_INIT(name)
		{
		}

		_JSONService(const _JSONService& other):
			MP_LIST_INIT(methods),
			MP_WSTRING_INIT(name)
		{
			name = other.name;
			methods = other.methods;
		}
	}JSONService;
	
	JSONService service;
	int iID;
	bool bInitialiazed;
	bool Initialize(JSContext *cx, mlString sGetServiceQueryResult );
	bool Notify(JSContext *cx, mlString sMethodResult, bool isService = false );
	bool getServiceMethods(JSContext *cx, JSObject *obj);
	bool getMethodParams(JSContext *cx, JSObject *obj, JSONMethod *method);
	void SetErrorCode(int aiErrorCode){ errorCode = aiErrorCode; }

	void OnEventNotifyReceived();

	void QueryService();
};



}
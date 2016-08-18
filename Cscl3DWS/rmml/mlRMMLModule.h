#pragma once

#include "res.h"
using namespace res;

namespace rmml {

#define EVNM_onOpened L"onOpened"
#define EVNM_onCreated L"onCreated"
#define EVNM_onError L"onError"

/**
 * JS-класс для создания модулей ООММС
 */
class mlRMMLModule : public mlJSClass, public res::resIModuleNotify
{
	mlRMMLXML* mpManifest;
	mlRMMLXML* mpTechData;
	JSString* mjssModuleID;
	MP_WSTRING msModuleID;
	// mlString msModuleID;
	MP_WSTRING msAlias;
	IAsyncModuleImpl* pThreadModule;
public:
	mlRMMLModule(void);
	void destroy() { MP_DELETE_THIS; }
	~mlRMMLModule(void);
	static JSBool AlterJSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
MLJSCLASS_DECL2(mlRMMLModule)
private:
	enum {
		JSPROP_identifier,
		JSPROP_manifest,
		JSPROP_techData,
		JSPROP_MS_NOT_EXISTS,
		JSPROP_MS_CREATION_FAILED,
		EVID_onOpened,		// модуль успешно открыт
		EVID_onCreated,		// модуль успешно создан
		EVID_onError,		// возникла ошибка при создании или открытии модуля
	};
	JSFUNC_DECL(create)
	JSFUNC_DECL(open)
	JSFUNC_DECL(close)
	JSFUNC_DECL(readText)
	JSFUNC_DECL(writeText)
	JSFUNC_DECL(saveMedia)
	JSFUNC_DECL(copy)
	JSFUNC_DECL(saveResource)
	void loadMetadata();
	bool moduleCreateOpen( bool aCreate, uintN argc, jsval *argv, jsval *rval);
	bool moduleClose();

	// реализация res::resIModuleNotify
public:
	void OnModuleOpened( const wchar_t* lpcModuleName);
	void OnModuleCreated( const wchar_t* lpcModuleName, bool* abIsRemote);
	void OnModuleError( const wchar_t* lpcModuleName, unsigned int aNotifyCode);
	void SetAsyncModule( IAsyncModuleImpl* aThreadModule);

public:
	static void OnModuleEventInternal(JSContext* acx, mlSynchData& aData, unsigned short ausEventID);

private:
	jsval notifyFunctionIsExists( const wchar_t* lpcFunctionName);
	void callNotifyFunction( const wchar_t* lpcFunctionName);
	void OnModuleOpenedInternal(mlSynchData& aData);
	void OnModuleCreatedInternal(mlSynchData& aData);
	void OnModuleErrorInternal(mlSynchData& aData);
};

}
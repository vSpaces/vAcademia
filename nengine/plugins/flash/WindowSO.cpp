
#include "stdafx.h"
#include "WindowSO.h"
#include "npruntime.h"
#include <string>

/*
struct NPClass
{
	uint32_t structVersion;
	NPAllocateFunctionPtr allocate;
	NPDeallocateFunctionPtr deallocate;
	NPInvalidateFunctionPtr invalidate;
	NPHasMethodFunctionPtr hasMethod;
	NPInvokeFunctionPtr invoke;
	NPInvokeDefaultFunctionPtr invokeDefault;
	NPHasPropertyFunctionPtr hasProperty;
	NPGetPropertyFunctionPtr getProperty;
	NPSetPropertyFunctionPtr setProperty;
	NPRemovePropertyFunctionPtr removeProperty;
	NPEnumerationFunctionPtr enumerate;
	NPConstructFunctionPtr construct;
};
*/
/*
typedef NPObject *(*NPAllocateFunctionPtr)(NPP npp, NPClass *aClass);
typedef void (*NPDeallocateFunctionPtr)(NPObject *obj);
typedef void (*NPInvalidateFunctionPtr)(NPObject *obj);
typedef bool (*NPHasMethodFunctionPtr)(NPObject *obj, NPIdentifier name);
typedef bool (*NPInvokeFunctionPtr)(NPObject *obj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
typedef bool (*NPInvokeDefaultFunctionPtr)(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
typedef bool (*NPHasPropertyFunctionPtr)(NPObject *obj, NPIdentifier name);
typedef bool (*NPGetPropertyFunctionPtr)(NPObject *obj, NPIdentifier name, NPVariant *result);
typedef bool (*NPSetPropertyFunctionPtr)(NPObject *obj, NPIdentifier name, const NPVariant *value);
typedef bool (*NPRemovePropertyFunctionPtr)(NPObject *npobj, NPIdentifier name);
typedef bool (*NPEnumerationFunctionPtr)(NPObject *npobj, NPIdentifier **value, uint32_t *count);
typedef bool (*NPConstructFunctionPtr)(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
*/

NPObject* NPAllocate(NPP npp, NPClass *aClass);
void NPDeallocate(NPObject *obj);
void NPInvalidate(NPObject *obj);
bool NPHasMethod(NPObject *obj, NPIdentifier name);
bool NPInvoke(NPObject *obj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool NPInvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool NPHasProperty(NPObject *obj, NPIdentifier name);
bool NPGetProperty(NPObject *obj, NPIdentifier name, NPVariant *result);
bool NPSetProperty(NPObject *obj, NPIdentifier name, const NPVariant *value);
bool NPRemoveProperty(NPObject *npobj, NPIdentifier name);
bool NPEnumeration(NPObject *npobj, NPIdentifier **value, uint32_t *count);
bool NPConstruct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);


//static NPClass javascriptClass = { 1, jsAllocate, jsDeallocate, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static NPClass noScriptClass = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static NPClass windowClass = {
	NP_CLASS_STRUCT_VERSION,
	NPAllocate,
	NPDeallocate,
	NPInvalidate,
	NPHasMethod,
	NPInvoke,
	NPInvokeDefault,
	NPHasProperty,
	NPGetProperty,
	NPSetProperty,
	NPRemoveProperty,
	NPEnumeration,
	NPConstruct
};

static NPClass locationClass = {
	NP_CLASS_STRUCT_VERSION,
	NPAllocate,
	NPDeallocate,
	NPInvalidate,
	NPHasMethod,
	NPInvoke,
	NPInvokeDefault,
	NPHasProperty,
	NPGetProperty,
	NPSetProperty,
	NPRemoveProperty,
	NPEnumeration,
	NPConstruct
};

NPObject* NPAllocate(NPP npp, NPClass *aClass){
	return NULL;
//	return new ;
}
void NPDeallocate(NPObject *obj){
	// ??
}
void NPInvalidate(NPObject *obj){
	// ??
}
bool NPHasMethod(NPObject *obj, NPIdentifier name){
	return true;
}
bool NPInvoke(NPObject *obj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result){
	PrivateIdentifier* id = (PrivateIdentifier*)name;
	if(obj->_class == &windowClass){
		int hh=0;
	}else if(obj->_class == &locationClass){
		if(id->isString && strcmp(id->value.string, "toString") == 0){
			std::string sURL = "file:///C:/vu2008/ui/k01_2.swf"; // "http://www.ru";
			NPString npsURL = { (const NPUTF8*)sURL.c_str(), static_cast<uint32_t>(sURL.size()) };
			NPN_InitializeVariantWithStringCopy(result, &npsURL);
			int jj=0;
			return true;
		}
	}
	return false;
}
bool NPInvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result){
	return true;
}
bool NPHasProperty(NPObject *obj, NPIdentifier name){
	PrivateIdentifier* id = (PrivateIdentifier*)name;
	if(obj->_class == &windowClass){
		// window object
		CWindowSO* window = (CWindowSO*)obj;
		if(id->isString && strcmp(id->value.string, "location") == 0){
			return true;
		}
		if(id->isString && strcmp(id->value.string, "top") == 0){
			return true;
		}
	}
	return false;
}
bool NPGetProperty(NPObject *obj, NPIdentifier name, NPVariant *result){
	PrivateIdentifier* id = (PrivateIdentifier*)name;
	if(obj->_class == &windowClass){
		// window object
		CWindowSO* window = (CWindowSO*)obj;
		if(id->isString && strcmp(id->value.string, "location") == 0){
			result->type = NPVariantType_Object;
			result->value.objectValue = &(window->m_location);
			return true;
		}
		if(id->isString && strcmp(id->value.string, "top") == 0){
			result->type = NPVariantType_Int32;
			result->value.intValue = 0;
			return true;
		}
	}
	return false;
}
bool NPSetProperty(NPObject *obj, NPIdentifier name, const NPVariant *value){
	return true;
}
bool NPRemoveProperty(NPObject *npobj, NPIdentifier name){
	return true;
}
bool NPEnumeration(NPObject *npobj, NPIdentifier **value, uint32_t *count){
	return true;
}
bool NPConstruct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result){
	return true;
}

CWindowSO::CWindowSO(){
/*
	m_class.structVersion = NP_CLASS_STRUCT_VERSION;
	m_class.allocate = NPAllocate;
	m_class.deallocate = NPDeallocate;
	m_class.invalidate = NPInvalidate;
	m_class.hasMethod = NPHasMethod;
	m_class.invoke = NPInvoke;
	m_class.invokeDefault = NPInvokeDefault;
	m_class.hasProperty = NPHasProperty;
	m_class.getProperty = NPGetProperty;
	m_class.setProperty = NPSetProperty;
	m_class.removeProperty = NPRemoveProperty;
	m_class.enumerate = NPEnumeration;
	m_class.construct = NPConstruct;
*/
	_class = &windowClass;
//	_class = &noScriptClass;
	referenceCount = 1;

	m_location._class = &locationClass;
	m_location.referenceCount = 1;

}

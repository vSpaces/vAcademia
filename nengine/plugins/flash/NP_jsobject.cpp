#include "stdafx.h"
#include "NP_jsobject.h"

#include "npruntime_impl.h"
//#include "npruntime_priv.h"

NPObject* jsAllocate(NPP npp, NPClass *aClass){
	return NULL;
	//	return new ;
}
void jsDeallocate(NPObject *obj){
	// ??
}

static NPClass javascriptClass = { 1, jsAllocate, jsDeallocate, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static NPClass noScriptClass = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

NPClass* NPScriptObjectClass = &javascriptClass;
static NPClass* NPNoScriptObjectClass = &noScriptClass;
/*
NPObject* _NPN_CreateScriptObject(NPP npp, JSObject* imp, PassRefPtr<RootObject> rootObject)
{
    JavaScriptObject* obj = reinterpret_cast<JavaScriptObject*>(_NPN_CreateObject(npp, NPScriptObjectClass));

    obj->rootObject = rootObject.releaseRef();

    if (obj->rootObject)
        obj->rootObject->gcProtect(imp);
    obj->imp = imp;

    return reinterpret_cast<NPObject*>(obj);
}
*/
NPObject* _NPN_CreateNoScriptObject(void)
{
    return _NPN_CreateObject(0, NPNoScriptObjectClass);
}

bool _NPN_InvokeDefault(NPP, NPObject* o, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    if (o->_class == NPScriptObjectClass) {
		// ??
            return false;
    }

    if (o->_class->invokeDefault)
        return o->_class->invokeDefault(o, args, argCount, result);    
    VOID_TO_NPVARIANT(*result);
    return true;
}

bool _NPN_Invoke(NPP npp, NPObject* o, NPIdentifier methodName, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    if (o->_class == NPScriptObjectClass) {
		// ??
            return false;
    }

    if (o->_class->invoke)
        return o->_class->invoke(o, methodName, args, argCount, result);
    
    VOID_TO_NPVARIANT(*result);
    return true;
}

bool _NPN_Evaluate(NPP, NPObject* o, NPString* s, NPVariant* variant)
{
    if (o->_class == NPScriptObjectClass) {
		// ??
            return false;
    }

    VOID_TO_NPVARIANT(*variant);
    return false;
}

bool _NPN_GetProperty(NPP, NPObject* o, NPIdentifier propertyName, NPVariant* variant)
{
    if (o->_class == NPScriptObjectClass) {
		// ??
        return false;
    }

    if (o->_class->hasProperty && o->_class->getProperty) {
        if (o->_class->hasProperty(o, propertyName))
            return o->_class->getProperty(o, propertyName, variant);
        return false;
    }

    VOID_TO_NPVARIANT(*variant);
    return false;
}

bool _NPN_SetProperty(NPP, NPObject* o, NPIdentifier propertyName, const NPVariant* variant)
{
    if (o->_class == NPScriptObjectClass) {
		// ??
            return false;

    }

    if (o->_class->setProperty)
        return o->_class->setProperty(o, propertyName, variant);

    return false;
}

bool _NPN_RemoveProperty(NPP, NPObject* o, NPIdentifier propertyName)
{
    if (o->_class == NPScriptObjectClass) {
		// ??
        return false;
    }
    return false;
}

bool _NPN_HasProperty(NPP, NPObject* o, NPIdentifier propertyName)
{
    if (o->_class == NPScriptObjectClass) {
		// ??
            return false;
    }

    if (o->_class->hasProperty)
        return o->_class->hasProperty(o, propertyName);

    return false;
}

bool _NPN_HasMethod(NPP, NPObject* o, NPIdentifier methodName)
{
    if (o->_class == NPScriptObjectClass) {
		// ??
            return false;
    }
    
    if (o->_class->hasMethod)
        return o->_class->hasMethod(o, methodName);
    
    return false;
}

void _NPN_SetException(NPObject*, const NPUTF8*)
{
    // FIXME:
    // Bug 19888: Implement _NPN_SetException() correctly
    // <https://bugs.webkit.org/show_bug.cgi?id=19888>
}

bool _NPN_Enumerate(NPP, NPObject* o, NPIdentifier** identifier, uint32_t* count)
{
    if (o->_class == NPScriptObjectClass) {
		// ??
        return false;
    }
    
    if (NP_CLASS_STRUCT_VERSION_HAS_ENUM(o->_class) && o->_class->enumerate)
        return o->_class->enumerate(o, identifier, count);
    
    return false;
}

bool _NPN_Construct(NPP npp, NPObject* o, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    if (o->_class == NPScriptObjectClass) {
		// ??
        return false;
    }
    
    if (NP_CLASS_STRUCT_VERSION_HAS_CTOR(o->_class) && o->_class->construct)
        return o->_class->construct(o, args, argCount, result);
    
    return false;
}


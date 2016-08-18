#pragma once

#include <map>

namespace rmml {

class mlRMMLElement;

struct EventSpec{
	bool prop; // Event is property 'watch'
    const wchar_t *name;
    char id;
};
typedef std::auto_ptr<EventSpec> MapEventID; 

/**
 * Ѕазовый класс дл€ всех RMML-классов (Scene, Composition, Image, ...) дл€ JavaScript
 * 
 */
class RMML_NO_VTABLE mlJSClass
{
public:
	mlJSClass(void);
	virtual void destroy() = 0;
	virtual ~mlJSClass(void);

//public:
//	virtual mlRMMLElement* GetRMMLElement()=0;
protected:

public:
//	JSObject *getJSObject(JSContext *cx);
	static void SetInstanceContext(JSContext *cx, JSObject* obj);
	static void Init();
	static mlresult CreatePropFuncSpecs(JSPropertySpec* &pJSPropertySpec,JSFunctionSpec* &pJSFunctionSpec);
	static mlresult DestroyPropFuncSpecs(JSPropertySpec* &pJSPropertySpec,JSFunctionSpec* &pJSFunctionSpec);
	static mlresult AddPropFuncSpecs(JSPropertySpec* &pJSPropertySpec,JSFunctionSpec* &pJSFunctionSpec,
			JSPropertySpec* apJSPropSpec, int anProps,
			JSFunctionSpec* apJSFuncSpec, int anFuncs);
	static mlresult AddEventSpecs(EventSpec* &pEventSpec, EventSpec* apEventSpec, int anEvents);
	static mlresult InitEventIDMap(MapEventID &mMapEventID, EventSpec* pEventSpec, JSPropertySpec* apJSPropertySpec); 
	char GetEventID(MapEventID &amMapEventID, wchar_t* apwcIDName);
//	char* GetEventID(MapEventID &amMapEventID, char acID);
	wchar_t* GetEventName(MapEventID &amMapEventID, char acID);
	// вызов обработчиков событий (функций) дл€ объектов, которые не €вл€ютс€ RMML-элементами
	static void CallListenersStatic(JSContext* mcx, JSObject* mjso, const wchar_t* apwcEventName);
};
/*
	private: \
		struct _JSinternalStruct { \
			JSObject *o; \
			_JSinternalStruct() : o(NULL) {}; \
			~_JSinternalStruct() { if (o) JS_SetPrivate(NULL,o, NULL); }; \
		} _JSinternal; \
		JSObject *getJSObject(JSContext *cx); \
*/
//		mlRMMLElement* GetRMMLElement(); 

#define MLJSCLASS_DECL \
	private: \
		static JSClass _jsClass; \
	public: \
		static mlresult InitJSClass(JSContext* cx , JSObject* obj); \
		static JSObject *newJSObject(JSContext *cx); \
		static bool IsInstance(JSContext *cx, JSObject* obj); \
	private: \
		static JSPropertySpec _JSPropertySpec[]; \
		static JSBool JSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval); \
		static void JSDestructor(JSContext *cx, JSObject *obj); \
		static JSBool JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp); \
		static JSBool JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp); \
		static JSBool JSOPEnumOp(JSContext *cx, JSObject *obj); \
		static JSFunctionSpec _JSFunctionSpec[]; \
	private: \
		static EventSpec _EventSpec[]; \
		static MapEventID mMapEventID; \
	public: \
		char GetEventID(wchar_t* apwcIDName); \
		wchar_t* GetEventName(char acID); \
		EventSpec* GetEventSpec();

#define MLJSCLASS_DECL2(CN) \
	JSContext* mcx; \
	JSObject* mjso; \
	static void PropIsSet(CN* apThis, char idProp){} \
	static int CheckUserEvent(JSString* ajssEventID, CN* apThis, jsval *vp){ return -1; } \
	void CallListeners(const wchar_t* apwcEventName){ mlJSClass::CallListenersStatic(mcx, mjso, apwcEventName); } \
	MLJSCLASS_DECL


#if JS_VERSION >= 170
#define MLJSCLASS_IMPL_BEGIN(N,CN,T)                                                    \
	JSClass CN::_jsClass = {                                                              \
	#N, JSCLASS_HAS_PRIVATE,                                                            \
	JS_PropertyStub, JS_PropertyStub,                                                   \
	CN::JSGetProperty, CN::JSSetProperty,                                               \
	JS_EnumerateStub, JS_ResolveStub,                                                   \
	JS_ConvertStub, CN::JSDestructor,                                                   \
	0, 0, 0, CN::JSConstructor,                                                         \
	(JSXDRObjectOp)T, 0, 0, 0                                                                          \
	};                                                                                    \
	\
	JSObject*                                                                             \
	CN::newJSObject(JSContext *cx)                                                        \
{                                                                                     \
	JSObject* obj=JS_NewObject(cx, &CN::_jsClass, NULL, NULL);                          \
	jsval v;                                                                            \
	CN::JSConstructor(cx, obj, 0, NULL, &v);                                            \
	return obj;                                                                         \
}                                                                                     \
	\
	bool CN::IsInstance(JSContext* cx,JSObject* obj){ \
		if(obj==NULL) return false; \
		return ML_JS_GETCLASS(cx, obj)==&CN::_jsClass; \
		} \
	\
	JSBool                                                                                \
	CN::JSConstructor(JSContext *cx, JSObject *obj,                                       \
	uintN /*argc*/, jsval */*argv*/, jsval *rval)                       \
{                                                                                     \
	if(ML_JS_GETCLASS(cx, obj) == &global_class)                                               \
{                                                                                   \
	JS_ReportWarning(cx, "Instance of %s class must be created by operator new", #N); \
	return JS_FALSE;                                                                  \
}                                                                                   \
	\
	CN *p = NULL;                                                                       \
	p = MP_NEW(CN);                                                                         \
	if (!p || !JS_SetPrivate(cx, obj, p))                                               \
	return JS_FALSE;                                                                  \
	p->mcx=cx;                                                                          \
	p->mjso=obj;                                                                        \
	mlJSClass::SetInstanceContext(cx, obj); \
	*rval = OBJECT_TO_JSVAL(obj);                                                       \
	return JS_TRUE;                                                                     \
}                                                                                   \
	\
void                                                                                \
CN::JSDestructor(JSContext *cx, JSObject *obj)                                      \
{                                                                                   \
	CN *p = (CN*)JS_GetPrivate(cx, obj);                                              \
	if (p){  \
		if(cx != p->mcx){ \
			GPSM(p->mcx)->AddObjToDeleteLater(p, T); \
		}else	\
			 MP_DELETE( p);  \
	} \
}                                                                                   \
	\
JSBool                                                                              \
CN::JSOPEnumOp(JSContext *cx, JSObject *obj)                                        \
{                                                                                   \
	for(int ii=0; ; ii++)                                                             \
{                                                                                 \
	JSPropertySpec* pPropSpec=&_JSPropertySpec[ii];                                 \
	if(pPropSpec->name==NULL)                                                       \
	return JS_TRUE;                                                               \
	jsval vProp;                                                                    \
	JS_GetProperty(cx,obj,pPropSpec->name,&vProp);                                  \
	JS_SetProperty(cx,obj,pPropSpec->name,&vProp);                                  \
}                                                                                 \
	return JS_TRUE;                                                                   \
}                                                                                   \
	\
	mlresult                                                                            \
	CN::InitJSClass(JSContext* cx , JSObject* obj)                                      \
{                                                                                   \
	JSPropertySpec* pJSPropertySpec=NULL;                                             \
	JSFunctionSpec* pJSFunctionSpec=NULL;                                             \
	mlJSClass::CreatePropFuncSpecs(pJSPropertySpec,pJSFunctionSpec);                  \
	EventSpec* pEventSpec = MP_NEW_ARR( EventSpec, 1); \
	memset(pEventSpec,0,sizeof(EventSpec));
	/*                                                                                      \
EventSpec* pEventSpec=new EventSpec[1];                                           \
memset(pEventSpec,0,sizeof(EventSpec));
*/ 

#else
	#define MLJSCLASS_IMPL_BEGIN(N,CN,T) \
	JSClass CN::_jsClass={ \
		#N, JSCLASS_HAS_PRIVATE, \
		JS_PropertyStub, JS_PropertyStub, \
		CN::JSGetProperty, CN::JSSetProperty, \
		JS_EnumerateStub, JS_ResolveStub, \
		JS_ConvertStub, CN::JSDestructor, \
		0, 0, 0, CN::JSConstructor, \
		0, 0, 0, T \
	}; \
	JSObject* CN::newJSObject(JSContext *cx) { \
		JSObject* obj=JS_NewObject(cx, &CN::_jsClass, NULL, NULL); \
		jsval v; \
		CN::JSConstructor(cx, obj, 0, NULL, &v); \
		return obj; \
	} \
	bool CN::IsInstance(JSContext* cx,JSObject* obj){ \
		if(obj==NULL) return false; \
		return ML_JS_GETCLASS(cx, obj)==&CN::_jsClass; \
	} \
	JSBool CN::JSConstructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) { \
		if(ML_JS_GETCLASS(cx, obj) == &global_class){ \
			JS_ReportError(cx, "Instance of %s class must be created by operator new", #N); \
			return JS_FALSE;\
		} \
		CN *p = NULL; \
		p = MP_NEW( CN); \
		if (!p || !JS_SetPrivate(cx, obj, p)) return JS_FALSE; \
		p->mjso=obj; \
		p->mcx=cx; \
		mlJSClass::SetInstanceContext(cx, obj); \
		*rval = OBJECT_TO_JSVAL(obj); \
		return JS_TRUE; \
	} \
	void CN::JSDestructor(JSContext *cx, JSObject *obj) { \
		CN *p = (CN*)JS_GetPrivate(cx, obj); \
		if (p){  \
			if(cx != p->mcx) \
				?? GPSM(p->mcx)->AddElemToDeleteLater(p); \
			else	\
				 MP_DELETE( p);  \
		} \
	} \
	JSBool CN::JSOPEnumOp(JSContext *cx, JSObject *obj){ \
		for(int ii=0; ; ii++){ \
			JSPropertySpec* pPropSpec=&_JSPropertySpec[ii]; \
			if(pPropSpec->name==NULL) return JS_TRUE; \
			jsval vProp; \
			JS_GetProperty(cx,obj,pPropSpec->name,&vProp); \
			JS_SetProperty(cx,obj,pPropSpec->name,&vProp); \
		} \
		return JS_TRUE; \
	} \
	mlresult CN::InitJSClass(JSContext* cx , JSObject* obj){ \
		JSPropertySpec* pJSPropertySpec=NULL; \
		JSFunctionSpec* pJSFunctionSpec=NULL; \
		mlJSClass::CreatePropFuncSpecs(pJSPropertySpec,pJSFunctionSpec); \
		EventSpec* pEventSpec = MP_NEW_ARR( EventSpec, 1); \
		memset(pEventSpec,0,sizeof(EventSpec));
#endif

#define MLJSCLASS_ADDPROTO(PCN) \
	PCN::AddPropFuncSpecs(pJSPropertySpec,pJSFunctionSpec); \
	PCN::AddEventSpecs(pEventSpec);

#define MLJSCLASS_ADDPROPFUNC \
	mlJSClass::AddPropFuncSpecs(pJSPropertySpec, pJSFunctionSpec, \
										_JSPropertySpec, sizeof(_JSPropertySpec)/sizeof(JSPropertySpec)-1, \
										_JSFunctionSpec, sizeof(_JSFunctionSpec)/sizeof(JSFunctionSpec)-1);
#define MLJSCLASS_ADDEVENTS \
	mlJSClass::AddEventSpecs(pEventSpec, \
									_EventSpec, sizeof(_EventSpec)/sizeof(EventSpec)-1);

#define MLJSCLASS_ADDPROPENUMOP \
	_jsClass.enumerate=JSOPEnumOp;

#define MLJSCLASS_ADDALTERCONSTR(ALTC) \
	_jsClass.construct=ALTC;

#define MLJSCLASS_IMPL_END(CN) \
	JS_InitClass(cx, obj, NULL, &_jsClass, \
		_jsClass.construct, 0, \
		pJSPropertySpec, pJSFunctionSpec, \
		NULL, NULL); \
	mlJSClass::InitEventIDMap(CN::mMapEventID,pEventSpec,pJSPropertySpec); \
	mlJSClass::DestroyPropFuncSpecs(pJSPropertySpec,pJSFunctionSpec); \
	return ML_OK; \
} \
MapEventID CN::mMapEventID; \
char CN::GetEventID(wchar_t* apwcIDName){ \
	return mlJSClass::GetEventID(CN::mMapEventID,apwcIDName); \
} \
wchar_t* CN::GetEventName(char acID){ \
	return mlJSClass::GetEventName(CN::mMapEventID,acID); \
} \
EventSpec* CN::GetEventSpec(){ \
	return CN::mMapEventID.get(); \
}

#define SAVE_FROM_GC(CX,PARENT,JSO) \
	{	 \
		jsval v=OBJECT_TO_JSVAL(JSO); \
		JS_DefineUCProperty(CX, PARENT, \
			(jschar*)(void*)(&(JSO)), sizeof(void*)/sizeof(jschar), \
			v, NULL, NULL, 0); \
	}

#define GET_SAVED_FROM_GC(CX,PARENT,JSO,VP) \
	JS_GetUCProperty(CX, PARENT, (jschar*)(void*)(&(JSO)), sizeof(void*)/sizeof(jschar), VP);

//		JS_SetUCProperty(CX, PARENT, (jschar*)(void*)(&(JSO)), sizeof(void*)/sizeof(jschar), &v); 

#define FREE_FOR_GC(CX,PARENT,JSO) \
	{	 \
		jsval v=OBJECT_TO_JSVAL(JSO); \
		JS_DeleteUCProperty2(CX, PARENT, (jschar*)(void*)(&(JSO)), sizeof(void*)/sizeof(jschar), &v); \
	}

#define SAVE_STR_FROM_GC(CX,PARENT,JSS) \
	{	 \
		jsval v=STRING_TO_JSVAL(JSS); \
		JS_DefineUCProperty(CX, PARENT, \
			(jschar*)(void*)(&(JSS)), sizeof(void*)/sizeof(jschar), \
			v, NULL, NULL, 0); \
	}

//		JS_SetUCProperty(CX, PARENT, (jschar*)(void*)(&(JSS)), sizeof(void*)/sizeof(jschar), &v); 

#define FREE_STR_FOR_GC(CX,PARENT,JSS) \
	{	 \
		jsval v=STRING_TO_JSVAL(JSS); \
		JS_DeleteUCProperty2(CX, PARENT, (jschar*)(void*)(&(JSS)), sizeof(void*)/sizeof(jschar), &v); \
	}

#define SAVE_VAL_FROM_GC(CX,PARENT,JSVL) \
	{	 \
		JS_DefineUCProperty(CX, PARENT, \
			(jschar*)(void*)(&(JSVL)), sizeof(void*)/sizeof(jschar), \
			JSVL, NULL, NULL, 0); \
	}

#define FREE_VAL_FOR_GC(CX,PARENT,JSVL) \
	{	 \
		jsval v; \
		JS_DeleteUCProperty2(CX, PARENT, (jschar*)(void*)(&(JSVL)), sizeof(void*)/sizeof(jschar), &v); \
	}

#define JSFUNC(N,A) \
	{ #N, JSFUNC_##N, A, 0, 0},

#define JSPROP_RW(N) \
	{ #N, (int8)JSPROP_##N, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0},
#define JSPROP_RO(N) \
	{ #N, (int8)JSPROP_##N, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT, 0, 0},

#define JSPROPSC_ID_MASK 0x40
#define JSPROP_SC(NM, CD) \
	{ #NM, CD | JSPROPSC_ID_MASK, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT, 0, 0},

// property aliases
#define JSPROPA_RW(N, ID) \
	{ #N, (int8)JSPROP_##ID, JSPROP_PERMANENT, 0},
#define JSPROPA_RO(N, ID) \
	{ #N, (int8)JSPROP_##ID, JSPROP_READONLY | JSPROP_PERMANENT, 0, 0},

#define JSPROP_EV(N) \
	{ #N, (int8)EVID_##N, 0, 0, 0},

#define JSFUNC(N,A) \
	{ #N, JSFUNC_##N, A, 0, 0},

#define JSFUNC_DECL(N)\
	static JSBool JSFUNC_##N(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

//	JSObject* pImageProto=JS_InitClass(cx, obj, NULL, &_jsClass, \

//mlRMMLElement* CN::GetRMMLElement(){ 
//	return (mlRMMLElement*)this; 
//} 

extern const wchar_t* aEventNames[256];
extern bool aEventIsPW[256];
extern mlString aPropNames[256];

}


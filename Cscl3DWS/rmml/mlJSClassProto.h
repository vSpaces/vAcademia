#pragma once

#if _MSC_VER <= 1200
double __cdecl _wtof(const wchar_t *ptr);
#endif

namespace rmml {

// 0..127 
// можно сделать только еще один прототип (7<<4)
enum {
	TIDB_mlRMMLElement=		(2<<4),
//	TIDB_mlRMMLLoadable=	(2<<4),
	TIDB_mlRMMLVisible=		(3<<4),
	TIDB_mlRMMLContinuous=	(4<<4),
	TIDB_mlRMMLPButton=		(5<<4),
	TIDB_mlRMML3DObject=	(6<<4)
};

/**
 * Базовый класс для всех подклассов RMML (Element, Loadable, Visible, ...)
 * для "множественного наследования" в RMML-классах
 * (не путать с прототипами JavaScript!)
 */
/*
class mlJSClassProto
{
public:
	mlJSClassProto(void);
	~mlJSClassProto(void);
};
*/
#define MLJSPROTO_DECL \
	protected: \
		static JSPropertySpec _JSPropertySpec[]; \
		JSBool GetJSProperty(int id, jsval *vp); \
		JSBool SetJSProperty(JSContext* cx, int id, jsval *vp); \
		static JSFunctionSpec _JSFunctionSpec[]; \
		static EventSpec _EventSpec[]; \
	public: \
		static mlresult AddPropFuncSpecs(JSPropertySpec* &pJSPropertySpec,JSFunctionSpec* &pJSFunctionSpec); \
		static mlresult AddEventSpecs(EventSpec* &pEventSpec); 

#define ADDPROPFUNCSPECS_IMPL(CN) \
mlresult CN::AddPropFuncSpecs(JSPropertySpec* &pJSPropertySpec,JSFunctionSpec* &pJSFunctionSpec){ \
	return mlJSClass::AddPropFuncSpecs(pJSPropertySpec, pJSFunctionSpec, \
										_JSPropertySpec, sizeof(_JSPropertySpec)/sizeof(JSPropertySpec)-1, \
										_JSFunctionSpec, sizeof(_JSFunctionSpec)/sizeof(JSFunctionSpec)-1); \
} \
mlresult CN::AddEventSpecs(EventSpec* &pEventSpec){ \
	return mlJSClass::AddEventSpecs(pEventSpec, \
									_EventSpec, sizeof(_EventSpec)/sizeof(EventSpec)-1); \
}

#define SET_PROPS_BEGIN(EL) \
	EL* priv=(EL*)JS_GetPrivate(cx, obj); \
	if (priv==NULL){ return JS_TRUE; } \
	if(GPSM(cx)->mbSceneMayHaveSynchEls && \
	   !GPSM(cx)->mbDontSendSynchEvents && \
	   GPSM(cx)->GetMode() == smmNormal && \
	   JSO_IS_MLEL(cx, obj)) \
	{ \
		mlRMMLElement* pMLEl = (mlRMMLElement*)priv; \
		if(pMLEl->NeedToSynchronize() && GPSM(cx)->GetContext()->mpSynch!=NULL && pMLEl->mpParent != NULL) \
			if (pMLEl->SynchronizeProp(id, vp)) \
				return JS_TRUE; \
	} \
	if (JSVAL_IS_INT(id)) { \
		int iID=JSVAL_TO_INT(id); \
		switch(iID&~((1<<4)-1)){

#define SET_PROTO_PROP(PROT) \
		case TIDB_##PROT: \
			priv->##PROT::SetJSProperty(cx,iID,vp); \
			break;

#define SET_PROPS_END \
		} \
		PropIsSet(priv, iID); \
	}else PropIsSet(priv, -1);

#define SET_PROPS_BEGIN2(EL) \
	EL* priv=(EL*)JS_GetPrivate(cx, obj); \
	if (priv==NULL){ return JS_TRUE; } \
	if (JSVAL_IS_INT(id)) { \
		int iID=JSVAL_TO_INT(id); 

#define SET_PROPS_END2 \
	} 

#define GET_PROPS_BEGIN(EL) \
	if(JSVAL_IS_STRING(id)){ \
		if(GPSM(cx)->IsCheckingEvents()){ \
			EL* priv=(EL*)JS_GetPrivate(cx, obj); \
			int iRes = CheckUserEvent(JSVAL_TO_STRING(id), priv, vp); \
			if(iRes >= 0) *vp = BOOLEAN_TO_JSVAL(iRes); \
		} \
	}else if (JSVAL_IS_INT(id)) { \
		EL* priv=(EL*)JS_GetPrivate(cx, obj); \
		if (priv==NULL){ *vp = JSVAL_NULL; return JS_TRUE; } \
		int iID=JSVAL_TO_INT(id); \
		switch(iID&~((1<<4)-1)){

#define GET_PROTO_PROP(PROT) \
		case TIDB_##PROT: \
			return priv->##PROT::GetJSProperty(iID,vp);

#define GET_PROPS_END \
		} \
	}

#define GET_PROPS_BEGIN2(EL) \
	if (JSVAL_IS_INT(id)) { \
		EL* priv=(EL*)JS_GetPrivate(cx, obj); \
		if (priv==NULL){ *vp = JSVAL_NULL; return JS_TRUE; } \
		int iID=JSVAL_TO_INT(id); 

#define GET_PROPS_END2 \
	}

#define ML_JSVAL_TO_INT(X,V) \
			if(JSVAL_IS_STRING(V)){ \
				X=_wtoi((const wchar_t*)wr_JS_GetStringChars(JSVAL_TO_STRING(V))); \
			}else if(JSVAL_IS_INT(V)){ \
				X=JSVAL_TO_INT(V); \
			}else if(JSVAL_IS_DOUBLE(V)){ \
				X=*JSVAL_TO_DOUBLE(V); \
			}

#define ML_JSVAL_TO_DOUBLE(X,V) \
			if(JSVAL_IS_STRING(V)){ \
				X=WStringToDouble((const wchar_t*)wr_JS_GetStringChars(JSVAL_TO_STRING(V))); \
			}else if(JSVAL_IS_INT(V)){ \
				X=JSVAL_TO_INT(V); \
			}else if(JSVAL_IS_DOUBLE(V)){ \
				X=*JSVAL_TO_DOUBLE(V); \
			}

#define ML_JSVAL_TO_BOOL(X,V) \
			if(JSVAL_IS_STRING(V)){ \
				if(isEqual((const wchar_t *)wr_JS_GetStringChars(JSVAL_TO_STRING(V)),(const wchar_t*)L"true")) \
					X=true; \
				else X=false; \
			}else if(JSVAL_IS_BOOLEAN(V)){ \
				X=JSVAL_TO_BOOLEAN(V); \
			} 

// with true default value
#define ML_JSVAL_TO_BOOL_TDF(X,V) \
			if(JSVAL_IS_STRING(V)){ \
				wchar_t* jsc = wr_JS_GetStringChars(JSVAL_TO_STRING(V)); \
				if(*jsc == 0 || isEqual(jsc,(const wchar_t*)L"true")) \
					X=true; \
				else X=false; \
			}else if(JSVAL_IS_BOOLEAN(V)){ \
				X=JSVAL_TO_BOOLEAN(V); \
			} 

#define MLEVENT(E) {false, EVNM_##E, EVID_##E},

#define MLEVENT_IMPL(CN,EV) \
void CN::EV(){ \
	GPSM(mcx)->mbDontSendSynchEvents = false; \
	GetElem_##CN()->CallListeners(EVID_##EV); \
}

}
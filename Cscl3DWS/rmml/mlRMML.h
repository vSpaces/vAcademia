
#ifndef __RMML_H_
#define __RMML_H_

//#define ENABLE_OBJECT_STAT
// вести полный список созданных элементов и выдавать статистику по ним после каждого вызова GC
// ТОЛЬКО ДЛЯ ОТЛАДКИ!
//#define ML_TRACE_ALL_ELEMS

#include "std_define.h"
#define RMML_BUILD		1
#include "..\..\common\memprofiling.h"
#include "..\..\rtl\rtl.h"

#include "jsapi.h"
#include "jsapi_wrappers.h"

#ifdef CSCL
#include "..\oms\oms_context.h"
#include "rmml.h"
#include "..\rm\rm.h"
#include "..\res\res.h"
#include "..\sm\sm.h"
#ifdef _DEBUG
#include <crtdbg.h>
//#define new new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#endif // _DEBUG
#ifdef JS_THREADSAFE
	#define GPSM(CX) ((mlSceneManager*)JS_GetContextPrivate(CX))
#else // JS_THREADSAFE
	#define GPSM(CX) (gpSM)
#endif // JS_THREADSAFE
#else // not CSCL
#include "config/oms_config.h"
#include "oms_context.h"
#include "rmml.h"
#include "mi.h"
#include "rm\rm.h"
#include "res\res.h"
#include "sm\sm.h"
#include "rtl/oms_allocated.h"
#include <strsafe.h>
#include <exception>

namespace oms{
  struct omsUserEvent;
}
#endif // CSCL
#define RMML_USE_LIBXML2
#ifdef RMML_USE_LIBXML2
#include <libxml\SAX2.h>
#include <libxml\xmlreader.h>
namespace rmml{
bool UTF82String(const char* autf8Str, int anLen, mlString &asStr);
}
#endif // RMML_USE_LIBXML2

#define RMML_DEBUG	// чтобы mldRMMLDebugger имел доступ к частным свойствам других классов
namespace rmmld{
	class mldRMMLDebugger;
}

#define RMML_VERSION_MAJOR 1
#define RMML_VERSION_MINOR 2

// Debug
// выводить в trace дополнительную отладочную инфу в режиме инициализации
#define MLD_TRACE_INIT_EVENTS
// end Debug
// режим отладки 3-го режима синхронизации
#define _DEBUG_SM3

#ifdef JS_THREADSAFE
#define ML_JS_GETCLASS(CX,JSO) JS_GetClass(CX, JSO)
#else
#define ML_JS_GETCLASS(CX,JSO) JS_GetClass(JSO)
#endif

#ifndef JSXDRObjectOp
typedef JSBool
(* JSXDRObjectOp)(JSXDRState *xdr, JSObject **objp);
#endif

#define PMOV_CREATED (moMedia*)1
#define PMOV_ACTIVATED (moMedia*)2
#define PMOV_DESTROYED (moMedia*)3
#define PMO_IS_NULL(PMO) ((((unsigned)PMO)&~3)==0)
#define IS_READY(PMLEL) (!PMO_IS_NULL((PMLEL)->mpMO))
#if JS_VERSION >= 170
#define JSO_IS_MLEL(CX, JSO) (JSO != NULL && ML_JS_GETCLASS(CX, JSO)->xdrObject == (JSXDRObjectOp)1)
#else
#define JSO_IS_MLEL(CX, JSO) (JSO != NULL && ML_JS_GETCLASS(CX, JSO)->spare == 1)
#endif
#define JSVAL_IS_REAL_OBJECT(JSV) (!JSVAL_IS_NULL(JSV) && !JSVAL_IS_VOID(JSV) && JSVAL_IS_OBJECT(JSV))
#define JSVAL_IS_MLEL(CX,JSV) (JSVAL_IS_REAL_OBJECT(JSV) && JSO_IS_MLEL(CX,JSVAL_TO_OBJECT(JSV)))
#define JSVAL_TO_MLEL(CX,JSV) (JSVAL_IS_MLEL(CX,JSV)?GET_RMMLEL(CX,JSVAL_TO_OBJECT(JSV)):NULL)
#define MLIAUDIBLE_IMPL \
public: \
	mlIAudible* GetIAudible(){ return this; }

#include "mlErrors.h"
namespace rmml { extern JSClass global_class; }
#include "mlJSClassProto.h"
#include "mlJSClass.h"
#include <vector>
#include <set>
namespace rmml { void sortchars (char *lo, char *hi); }
#include "mlRMMLElement.h"
#include "mlRMMLLoadable.h"
#include "mlRMMLVisible.h"
#include "mlRMMLContinuous.h"
#include "mlRMMLPButton.h"

// 2D-элементы
#include "2D\mlRMMLImage.h"
#include "2D\mlRMMLAnimation.h"
#include "2D\mlRMMLVideo.h"
#include "2D\mlRMMLStyles.h"
#include "2D\mlRMMLText.h"
#include "2D\mlRMMLInput.h"
#include "2D\mlRMMLActiveX.h"
#include "2D\mlRMMLButton.h"
#include "2D\mlRMMLFlash.h"
#include "2D\mlRMMLBrowser.h"
#include "2D\mlRMMLQT.h"
#include "2D\mlRMMLImageFrame.h"
#include "2D\mlRMMLLine.h"
// 3D-элементы
#include "3D\mlRMML3DObject.h"
#include "3D\mlRMMLParticles.h"
#include "3D\mlRMMLViewport.h"
#include "3D\mlRMMLScene3D.h"
#include "3D\mlRMMLCamera.h"
#include "3D\mlRMMLLight.h"
#include "3D\mlRMMLObject.h"
#include "3D\mlRMMLMotion.h"
#include "3D\mlRMMLGroup.h"
#include "3D\mlRMMLCloud.h"
#include "3D\mlRMMLCharacter.h"
#include "3D\mlRMMLSpeech.h"
#include "3D\mlRMMLMovement.h"
#include "3D\mlRMMLVisemes.h"
#include "3D\mlRMMLPath3D.h"
#include "3D\mlRMMLHint.h"
#include "3D\mlRMMLIdles.h"
#include "3D\mlRMMLIdle.h"
#include "3D\mlRMMLShadow3D.h"
#include "3D\mlRMMLShadows3D.h"
#include "3D\mlRMMLMap.h"
#include "3D\mlRMMLMaterial.h"
#include "3D\mlRMMLMaterials.h"

#include "mlRMMLScript.h"
#include "mlRMMLSequencer.h"
#include "mlRMMLComposition.h"
#include "mlRMMLIterator.h"
// прочие элементы
#include "mlRMMLAudio.h"
#include "mlRMMLPlugin.h"
#include "XML\mlRMMLXML.h"
#include "mlRMMLResource.h"
#include "mlSoapJSO.h"
#include "mlRMMLLocation.h"
#include "mlSynchLink.h"
//#include "mlRMMLSchema.h"

namespace rmml {

// имена тэгов RMML
#define RMMLTN_RMML 		L"rmml"
#define RMMLTN_INCLUDE 		L"include"
#define RMMLTN_Scene 		L"scene"
#define RMMLTN_Composition 	L"composition"
#define RMMLTN_Image 		L"image"
#define RMMLTN_Animation 	L"animation"
#define RMMLTN_Video 		L"video"
#define RMMLTN_Text 		L"text"
#define RMMLTN_Input 		L"input"
#define RMMLTN_ActiveX 		L"activex"
#define RMMLTN_QT			L"quicktime"
#define RMMLTN_Plugin 		L"plugin"
#define RMMLTN_Styles 		L"styles"
#define RMMLTN_Button 		L"button"
#define RMMLTN_Audio 		L"audio"
#define RMMLTN_Sequencer 	L"sequencer"
#define RMMLTN_XML 		L"xml"
#define RMMLTN_Script 		L"script"
#define RMMLTN_Viewport 	L"viewport"
#define RMMLTN_Scene3D 		L"scene3d"
#define RMMLTN_Viewport 	L"viewport"
#define RMMLTN_Camera 		L"camera"
#define RMMLTN_Light 		L"light"
#define RMMLTN_Object 		L"object"
#define RMMLTN_Object3D 	L"object3d"
#define RMMLTN_Motion 		L"motion"
#define RMMLTN_Character 	L"character"
#define RMMLTN_Speech 		L"speech"
#define RMMLTN_Group 		L"group"
#define RMMLTN_Movement 	L"movement"
#define RMMLTN_Cloud 		L"cloud"
#define RMMLTN_Visemes 		L"visemes"
#define RMMLTN_Hint 		L"hint"
#define RMMLTN_Path3D 		L"path3d"
#define RMMLTN_Idles 		L"idles"
#define RMMLTN_Idle 		L"idle"
#define RMMLTN_Shadow 		L"shadow"
#define RMMLTN_Shadows 		L"shadows"
#define RMMLTN_Map 		L"map"
#define RMMLTN_Material 	L"material"
#define RMMLTN_Materials 	L"materials"
#define RMMLTN_Resource 	L"resource"
#define RMMLTN_ImageFrame 	L"imageframe"
#define RMMLTN_Line 		L"line"

// имена глобальных объектов
#define GJSONM_CLASSES  "Classes"
#define GJSONM_PLAYER   "Player"
#define GJSONMW_PLAYER  L"Player"
#define GJSONM_MODULE   "module"
#define GJSONM_SERVER   "server"
#define GJSONM_SRECORDER "recorder"
#define GJSONM_MAP		"map"
#define GJSONM_REALITY	"reality"
#define GJSONM_URL		"url"
#define GJSONM_VOICE	"voice"
#define GJSONM_VIDEOCAPTURE	"videoCapture"
#define GJSONM_SRCURRENT_SENACE "currentSeance"
#define GJSONM_RECORD_EDITOR   "recordEditor"
#define GJSONM_CODEEDITOR	"codeEditor"


#define GJSONM_MOUSE    "Mouse"
#define GJSONMW_MOUSE   L"Mouse"
#define GJSONM_KEY      "Key"
#define GJSONM_SCENE    "scene"
#define GJSOWNM_SCENE  L"scene"
#define GJSONM_EVENT    "Event"

#define CWSLENGTH(CWS) ((sizeof(CWS) - 1) / sizeof(wchar_t))
#define CSLENGTH(CS) (sizeof(CS) - 1)

#define DONT_IMPL_NM_RVAL ((jsval)0xC0000002)
#define CHECK_DONT_IMPL if(*rval == DONT_IMPL_NM_RVAL) return JS_TRUE;


// (Tandy) иногда случается, что из rmml в js32 приходит значение NaN, 
// в скриптах не проверяется и при выполнении каких-то действий с ним падает.
// Поэтому добавили предварительную проверку на NaN и замену на 0.
inline JSBool mlJS_NewDoubleValue(JSContext *cx, jsdouble d, jsval *rval){
#ifdef WIN32
	if(_isnan(d)) d = 0;
#else
	if(isnan(d)) d = 0;
#endif
	return JS_NewDoubleValue(cx, d, rval);
}

/**
* Класс поддержки RMML-языка
* Используется для инициализации языка (объявления JS-классов)
* 
*/
class mlRMML
{
public:
	mlRMML(void);
	~mlRMML(void);
	static mlresult InitGlobalJSObjects(JSContext* cx, JSObject* obj);
	static JSObject* CreateElement(JSContext* cx, const char* apszClassName, int aiType=-1);
	static std::string GetTypeName(int aiRMMLType);
};

__forceinline bool isEqual(const char* apch1, const char* apch2){
	for(;;){
		if(*apch1!=*apch2) return false;
		if(*apch1=='\0') return true;
		apch1++, apch2++;
	}
}

__forceinline bool isEqual(const char* apch1, const char* apch2, unsigned int auLen){
	for(; auLen>0; auLen--){
		if(*apch1!=*apch2) return false;
		apch1++, apch2++;
	}
	return true;
}

__forceinline bool isEqual(const wchar_t* apch1, const wchar_t* apch2){
	for(;;){
		if(*apch1!=*apch2) return false;
		if(*apch1==L'\0') return true;
		apch1++, apch2++;
	}
}

__forceinline bool isEqual(const wchar_t* apch1, const wchar_t* apch2, unsigned int auLen){
	for(; auLen>0; auLen--){
		if(*apch1!=*apch2) return false;
		apch1++, apch2++;
	}
	return true;
}

__forceinline bool isEqualOld(const wchar_t* apch1, const char* apch2){
	for(;;){
		if(*apch1!=*apch2) return false;
		if(*apch1==L'\0') return true;
		apch1++, apch2++;
	}
}

__forceinline bool isEqual(const JSString* ajss1, const wchar_t* apwc2){
	jschar* ajsc1=JS_GetStringChars((JSString*)ajss1);
	for(;;){
		if(*ajsc1!=*apwc2) return false;
		if(*ajsc1==L'\0') return true;
		ajsc1++; apwc2++;
	}
}

__forceinline bool isEqual(const JSString* ajss1, const JSString* ajss2){
	jschar* ajsc1=JS_GetStringChars((JSString*)ajss1);
	jschar* ajsc2=JS_GetStringChars((JSString*)ajss2);
	for(;;){
		if(*ajsc1!=*ajsc2) return false;
		if(*ajsc1==L'\0') return true;
		ajsc1++; ajsc2++;
	}
}

__forceinline bool isEqual(const mlString as1, const wchar_t* apwc2){
	const wchar_t* ajsc1 = as1.c_str();
	for(;;){
		if(*ajsc1!=*apwc2) return false;
		if(*ajsc1==L'\0') return true;
		ajsc1++; apwc2++;
	}
}

}

#ifdef CSCL
#include "gd\gd_XMLArchive.h"
#else
#include "..\gd\gd_XMLArchive.h"
#endif
#define AUTH_SCENE_FILE_NAME L"auth:auth.xml"
#include "mlSceneManager.h"
#include "mlRMMLBuilder.h"

namespace rmml {

double WStringToDouble(const wchar_t *p);

#define ML_POINT_IN_RECT(PNT,RC) \
	(PNT.x >= RC.left && PNT.x <= RC.right && PNT.y >= RC.top && PNT.y <= RC.bottom)

#define ML_RELEASE(P) if((P)!=NULL){ MP_DELETE_ARR( P); (P)=NULL; }

mlString Int2MlString( int iStrN);

class cLPCSTR{
	char* mpchStr;
public:
	cLPCSTR(const wchar_t* apwcStr){
		if(apwcStr==NULL){ mpchStr=NULL; return; }
		mpchStr=WC2MB(apwcStr);
	}
	cLPCSTR(jschar* apwcStr){
		if(apwcStr==NULL){ mpchStr=NULL; return; }
		mpchStr=WC2MB( (const wchar_t*) apwcStr);
	}
	cLPCSTR(JSString* ajssStr){
		if(ajssStr==NULL){ mpchStr=NULL; return; }
		mpchStr=WC2MB((const wchar_t*)JS_GetStringChars(ajssStr));
	}
	operator const char*(){ return mpchStr; }
	~cLPCSTR(){ if(mpchStr) MP_DELETE_ARR( mpchStr); }
};

class cLPCSTRq{
	char* mpchStr;
public:
	cLPCSTRq(wchar_t* apwcStr){
		if(apwcStr==NULL){ mpchStr=NULL; return; }
		mpchStr=WC2MBq(apwcStr);
	}
	cLPCSTRq(const wchar_t* apwcStr){
		if(apwcStr==NULL){ mpchStr=NULL; return; }
		mpchStr=WC2MBq(apwcStr);
	}
#if _MSC_VER >= 1500 //vs2008
	cLPCSTRq(const jschar* apwcStr){
		if(apwcStr==NULL){ mpchStr=NULL; return; }
		mpchStr=WC2MBq((const wchar_t*)apwcStr);
	}
#endif
	cLPCSTRq(JSString* ajssStr){
		if(ajssStr==NULL){ mpchStr=NULL; return; }
		mpchStr=WC2MBq( (const wchar_t *) JS_GetStringChars(ajssStr));
	}
	operator char*(){ return mpchStr; }
	~cLPCSTRq(){ if(mpchStr) MP_DELETE_ARR( mpchStr); }
};

class cLPWCSTR{
	wchar_t* mpwcStr;
public:
	cLPWCSTR(const char* apszStr){
		if(apszStr==NULL){ mpwcStr=NULL; return; }
		mpwcStr=MB2WC(apszStr);
	}
	cLPWCSTR(char* apszStr){
		if(apszStr==NULL){ mpwcStr=NULL; return; }
		mpwcStr=MB2WC(apszStr);
	}
	cLPWCSTR(JSString* ajssStr){
		if(ajssStr==NULL){ mpwcStr=NULL; return; }
		assert(false);
		mpwcStr=(wchar_t*)JS_GetStringChars(ajssStr);
	}
	operator const wchar_t*(){ return mpwcStr; }
	~cLPWCSTR(){ if(mpwcStr)  MP_DELETE_ARR( mpwcStr); }
};

class cLPWCSTRq{
	wchar_t* mpwcStr;
public:
	cLPWCSTRq(const char* apszStr){
		if(apszStr==NULL){ mpwcStr=NULL; return; }
		mpwcStr=MB2WCq(apszStr);
	}
	cLPWCSTRq(char* apszStr){
		if(apszStr==NULL){ mpwcStr=NULL; return; }
		mpwcStr=MB2WCq(apszStr);
	}
	cLPWCSTRq(JSString* ajssStr){
		if(ajssStr==NULL){ mpwcStr=NULL; return; }
		assert(false);	// ниже копия памяти не выделяется
		mpwcStr=(wchar_t*)JS_GetStringChars(ajssStr);
	}
	operator wchar_t*(){ return mpwcStr; }	
	~cLPWCSTRq(){ if(mpwcStr)  MP_DELETE_ARR( mpwcStr); }
};

#ifdef JS_THREADSAFE
	class mlGuardJSCalls
	{
		JSContext* mcx;
	public:
		mlGuardJSCalls(JSContext* cx):mcx(cx){
			JS_BeginRequest(mcx);
		}
		~mlGuardJSCalls(){
			JS_EndRequest(mcx);
		}
	};

	#define GUARD_JS_CALLS(CX) \
		mlGuardJSCalls oGJSC(CX);
#else
	#define GUARD_JS_CALLS(CX)
#endif

#ifdef ML_TRACE_ALL_ELEMS
extern void ElementPrecreated(mlRMMLElement* apElem);
#endif

#pragma warning( disable : 4800 4065 )

}
#endif // __RMML_H_
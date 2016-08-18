#include "mlRMML.h"
#include "config/oms_config.h"
#include "mlRMMLJSPlayer.h"
#include <tchar.h>
#include <algorithm>
#ifndef CSCL
#include "mme/facet/mme_controlled.h"
#include "oms_application.h"
#include "rtl/oms_url.h"
#endif
#include "config/prolog.h"
#include "mlRMMLJSPlUrl.h"
#include "mlRMMLJSPlVoice.h"
#include "mlRMMLModule.h"
#include "ILogWriter.h"
#include "rmmlsafe.h"
#include "..\InfoMan\InfoMan.h"
#include "..\nengine\interfaces\IMouseController.h"
#include "MapManagerStructures.h"
#include "mlSceneManager.h"

#include <malloc.h>
long HeapUsed();

#ifndef CSCL
#ifdef _DEBUG
# pragma comment(lib, "oms_rtl_dbg.lib")
#else
# pragma comment(lib, "oms_rtl.lib")
#endif
#endif
#include "SyncMan.h"
#include "mlObjectsStat.h"

// включить механизм сборки статистики создания объектов для релизной сборки клиента
//#ifndef _DEBUG
#define ENABLE_RELEASE_OBJECT_STAT
//#endif

using namespace cm;

class CComString;

namespace rmml {

// через каждые столько-то минут выводить в tracer кол-во RMML-элементов в сцене и UI
#define ML_TRACE_ELEM_COUNT_TIME 1

// делать ли периодическое полное обновление списков mvUpdatableElements, mvUpdatableUIElements и mvObjectsToSynch?
// ! лучше не делать, потому что из-за этого подтормаживает время от времени !
//#define ML_FULL_LISTS_UPDATE

//#ifndef JS_THREADSAFE
mlSceneManager* gpSM = NULL;
//#endif

typedef JSRuntime* PJSRUNTIME;

class mlJSRunTime{
	JSRuntime* rt;
public:
	mlJSRunTime(){
		rt = JS_NewRuntime(32L * 1024L * 1024L);
	}
	~mlJSRunTime(){
		Destroy();
	}
	void Destroy(){
		if(rt != NULL){
			JS_DestroyRuntime(rt);
			JS_ShutDown();
			rt = NULL;
		}
	}
	operator PJSRUNTIME() const	{ return rt; }
} gJSRunTime;

mlSceneManager::mlSceneManager(omsContext* apContext):mBuildEvalCtxt(gJSRunTime),mCaptureMouseObject(NULL), 
				m_ctrl(nullptr), m_prevButtons(0), m_pressedButton(0), m_XInputButtons(), hGamepadDll(nullptr),
				NewController(nullptr), m_leftThumbState(-1), m_rightThumbState(-1)
#ifdef CSCL
	, bMainModuleLoaded(false)
	, MP_VECTOR_INIT(mvUpdatableElements)
	, MP_VECTOR_INIT(mvUpdatableUIElements)
	, MP_VECTOR_INIT(m_synchObjectsInReality)
	, MP_VECTOR_INIT(mvNewElements)
	, MP_VECTOR_INIT(mvObjectsToSynch)
	, MP_VECTOR_INIT(mvDeletedInUpdate)
	, MP_VECTOR_INIT(mvMOToDeleteOut)
	, MP_VECTOR_INIT(mvPluginsMOs)
	, MP_VECTOR_INIT(mvElemsToDelete)
	, MP_VECTOR_INIT(mvObjectsToDelete)
	, MP_VECTOR_INIT(mvEventsToRestore)
	, MP_VECTOR_INIT(mvlNUEvents)
	, MP_VECTOR_INIT(mvPressedKeys)
	, MP_VECTOR_INIT(mvTrackingLinks)
	, MP_VECTOR_INIT(mvSkipedMouseMoves)
	, MP_WSTRING_INIT(msSceneXML)
	, MP_WSTRING_INIT(msStartURL)
	, MP_WSTRING_INIT(mwsModuleID)
	, MP_WSTRING_INIT(msClientStringID)
	, MP_STRING_INIT(m_currentExecutingScriptSrc)
	, MP_WSTRING_INIT(sSceneTitleDefault)
	, MP_WSTRING_INIT(sScenePathDefault)
	, MP_WSTRING_INIT(sSceneTempTitleDefault)
	, MP_WSTRING_INIT(sSceneTempPathDefault)
#endif
{
	MP_NEW_MEMBER_P(mServerCallbackManager, mlServerCallbackManager, this);
	MP_NEW_MEMBER_P(mServiceCallbackManager, mlServiceCallbackManager, this);
	MP_NEW_MEMBER_P(mMapCallbackManager, mlMapCallbackManager, this);
	MP_NEW_MEMBER_P(mSyncCallbackManager, mlSyncCallbackManager,this);
	MP_NEW_MEMBER_P(mWorldEventManager, mlWorldEventManager, this);
	MP_NEW_MEMBER_P(mVoipCallbackManager, mlVoipCallbackManager, this);
	MP_NEW_MEMBER_P(mDesktopCallbackManager, mlDesktopCallbackManager, this);
	MP_NEW_MEMBER_P(mScriptPreloader, mlScriptPreloader, this);

	int i;
	for(i = 0; i < ML_MOUSE_EVENTS_COUNT; i++)
		MP_VECTOR_DC_INIT(maMouseListeners[i]);
	for(i = 0; i < ML_KEY_EVENTS_COUNT; i++)
		MP_VECTOR_DC_INIT(maKeyListeners[i]);
	for(i = 0; i < ML_PLAYER_EVENTS_COUNT; i++)
		MP_VECTOR_DC_INIT(maPlayerListeners[i]);
	for(i = 0; i < ML_MODULE_EVENTS_COUNT; i++)
		MP_VECTOR_DC_INIT(maModuleListeners[i]);
	for(i = 0; i < ML_SERVER_EVENTS_COUNT; i++)
		MP_VECTOR_DC_INIT(maServerListeners[i]);
	for(i = 0; i < ML_RECORD_EDITOR_EVENTS_COUNT; i++)
		MP_VECTOR_DC_INIT(maRecordEditorListeners[i]);
	for(i = 0; i < ML_RECORDER_EVENTS_COUNT; i++)
		MP_VECTOR_DC_INIT(maRecorderListeners[i]);
	for(i = 0; i < ML_MAP_EVENTS_COUNT; i++)
		MP_VECTOR_DC_INIT(maMapListeners[i]);
	for(i = 0; i < ML_URL_EVENTS_COUNT; i++)
		MP_VECTOR_DC_INIT(maUrlListeners[i]);
	for(i = 0; i < ML_VOICE_EVENTS_COUNT; i++)
		MP_VECTOR_DC_INIT(maVoiceListeners[i]);

	//#ifndef JS_THREADSAFE
	gpSM=this;
	//#endif

	setlocale(LC_ALL, "Russian");

	lastMouseX = -1;
	lastMouseY = -1;

	mjsoClasses=NULL;
	//
	mpvFiredEvents = NULL;
	mpSELMLEl = NULL;
	mbCheckingEvents = false;
	//
	mjsoMouse=NULL;
	ML_INIT_POINT(mMouseXY);
	mpButUnderMouse = NULL;
	//mpVisibleUnderMouse = NULL;
	//mp3DObjectUnderMouse = NULL;
	//mpTextureObjectUnderMouse = NULL;
	//ML_INIT_POINT(mpntTextureObjectMouseXY);
	mbUseHandCursorChanged=false;
	mpButPressed = NULL;
	mpButRPressed = NULL;
	mlTime = 0;
	mbDragging = false;
	mbBigCursor = false;
	m_realityLoadedEventWasGeneratedAlready = false;
	miCursorType = oms::NormalCursor;
	miHandCursorType = oms::ActiveCursor;
	mbForceSetCursor = false;
	m_isMyAvatarCreated = false;
	m_isSynchUpdatesSuspended = false;
	//
	mjsoKey = NULL;
	muKeyCode = 0;
	muKeyStates = 0;
	mpButFocused = NULL;

	m_synchObjectsInRealityCreated = 0;
	m_synchObjectsInRealityCount = 0;
	//
	mjsoPlayer = NULL;
	mjsoModule = NULL;
	mjsoEvent = NULL;
	mjsoSRecorder = NULL;
	//
	mbDontSendSynchEvents=false;
	mbSceneMayHaveSynchEls=false;
	mulSynchEventsGlobalTime = 0;
	miLastUpdateDeltaTime = -1;
	mlLastUpdateTime = 0;
	//mePrevMode = smmInitilization;
	mePrevMode = smmNormal;
	mbNeedToShiftGlobalTime = false;
	mbUIScriptActivatingStartElem = NULL;
	mSynchMode = synchMode1;
	//
	mbWasExitFromScript = false;
	mjsvalExit = JSVAL_NULL;
	//
	mpElemRefToClear = NULL;
	mbNeedToCallGC = false;
	mlLastCallGCTime = 0L;
	mlLastForciblyCallGCTime = 0L;
	mlLastMemSizeCheckTime = 0L;
	//
	mpScene = NULL;
	mpPreviousScene = NULL;
	mpUI = NULL;
	mpAuthScene = NULL;
	mpContext = apContext;
//	mbSceneLoaded=false;
	//meMode = smmInitilization;
	meMode = smmNormal;
	mbPreloadingMode = false;
	mbLoadingMode = false;
	mbUpdating = false;

	miCurrentSceneIndex=-1;

	muElToLoadCnt=0;
	muElLoadedCnt=0;
	mbWasAuthoringMode=false;

	mbNeedToNotifyMouseModalButtons = false;

	mpWorld3DScene = NULL;

	uiCreatedAvatarID = 0xFFFFFFFF;
	muiMyAvatarID = 0;
	mMyAvatarBornRealityID =0;
	mpMyAvatar = NULL;

//	mbLocked=false;
	muExternEventID=0xFFFF;

	DetectLibXml2();

	SceneStructChanged();

	mBuildEvalCtxt.SetRefineResPathFunc(RefineResPath);

	InitJSE();

	SetVoipCallbacks();

	mbObjetcsUnfreezenAfterFreeze = false;
	mbObjetcsFreezeUnfreezenObjects = false;
	mbObjectsFrozen = false;
	miUpdateNumber = 0;

	muiSceneElementCount = 0;
	muiUIElementCount = 0;
	mlLastTraceElemCountTime = 0L;

	mlLastUIUpdatableListBuildingTime = 0; // время последнего обновления списка обновляемых объектов UI
	mlLastUpdatableListBuildingTime = 0; // время последнего обновления списка обновляемых объектов сцены

	mbObjectCreating = false;

	mbUpdatableElementsChanged = true;

	mbDestroying = false;

	mpBRPParents = NULL;
	mpBPParents = NULL;
	mpBUMParents = NULL;

#ifdef JS_THREADSAFE
	// если раскомментарить, то GC будет вызываться в отдельном потоке
//	mpUtilityThread.reset(new mlUtilityThread(gJSRunTime));
	mpScriptTerminationThread.reset(new mlScriptTerminationThread(cx));
#endif

	mlLastOldSyncElementsTime = 0;
	mlLastOldAvatarsTime = 0;
	meTestMode = smNoTest;

	mbNewEventsToNextUpdateMode = false; // false; должно быть по умолчанию false

	mbTraceEvents = true; // false; должно быть по умолчанию false

	mbParentIsSetFromJS = true;

	iSceneWidthDefault = 1024;
	iSceneHeightDefault = 768;
	sSceneTitleDefault = L"scene title"; 
	sScenePathDefault = L"scene.xml";
	sSceneTempTitleDefault = L"temp scene";
	sSceneTempPathDefault = L"temp.xml";

	mbDontUpdateOneTime = false;
	
	//строго соответствует порядку перечисления
	const WORD inputButtons[] = {XINPUT_GAMEPAD_A, XINPUT_GAMEPAD_B, XINPUT_GAMEPAD_X, XINPUT_GAMEPAD_Y, 
							XINPUT_GAMEPAD_DPAD_DOWN, XINPUT_GAMEPAD_DPAD_RIGHT, XINPUT_GAMEPAD_DPAD_UP, 
							XINPUT_GAMEPAD_DPAD_LEFT, XINPUT_GAMEPAD_LEFT_SHOULDER, XINPUT_GAMEPAD_RIGHT_SHOULDER, 
							XINPUT_GAMEPAD_LEFT_THUMB, XINPUT_GAMEPAD_RIGHT_THUMB, XINPUT_GAMEPAD_BACK, XINPUT_GAMEPAD_START };
	m_XInputButtons = std::vector<WORD>(inputButtons, inputButtons + sizeof(inputButtons) / sizeof(WORD));
}

mlSceneManager::~mlSceneManager(void)
{
	mpContext->mpSM = NULL;
	
	if (m_ctrl)
	{
		delete m_ctrl;
	}

	if (hGamepadDll)
	{
		FreeLibrary(hGamepadDll);
		hGamepadDll = nullptr;
	}

	if (mpContext->mpComMan)
		mpContext->mpComMan->SetCallbacks(NULL);
	if (mpContext->mpServiceMan)
		mpContext->mpServiceMan->SetCallbacks(NULL);
	if (mpContext->mpServiceWorldMan)
		mpContext->mpServiceWorldMan->SetCallbacks(NULL);

	{	GUARD_JS_CALLS(cx);
		ResetMouse(NULL); // clear links
		ResetKey(); // clear listener vectors
		ResetModule(); // clear listener vectors
		ResetPlayer(); // clear listener vectors
		ResetMap();
	}
//	mvUIGOListeners.clear();
#ifdef RMML_USE_LIBXML2
	if(mhmLibXml2 != NULL){
//		xmlCleanupEncodingAliases();
//		xmlCleanupCharEncodingHandlers();
		mpContext->mpApp->FreeLibrary(mhmLibXml2);
	}
#endif
	mbDestroying = true;

	DestroyJSE();
	mBuildEvalCtxt.DestroyCtxt();
	mpUtilityThread.reset(NULL);
	gJSRunTime.Destroy();

	v_mos::iterator vi = mvPluginsMOs.begin();
	for(; vi != mvPluginsMOs.end(); vi++){
		mpContext->mpRM->DestroyPlugin(*vi);
	}
	mvPluginsMOs.clear();

	if (mpBUMParents != NULL)
		MP_DELETE(mpBUMParents);

	if (mpBPParents != NULL)
		MP_DELETE(mpBPParents);
	
	if (mpBRPParents != NULL)
		MP_DELETE(mpBRPParents);

	if (mServerCallbackManager)
		MP_DELETE(mServerCallbackManager);

	if (mServiceCallbackManager)
		MP_DELETE(mServiceCallbackManager);
	if (mMapCallbackManager)
		MP_DELETE(mMapCallbackManager);
	if (mSyncCallbackManager)
		MP_DELETE(mSyncCallbackManager);
	if (mWorldEventManager)
		MP_DELETE(mWorldEventManager);
	if (mVoipCallbackManager)
		MP_DELETE(mVoipCallbackManager);
	if (mDesktopCallbackManager)
		MP_DELETE(mDesktopCallbackManager);
	if (mScriptPreloader)
		MP_DELETE(mScriptPreloader);
}

// в деструкторе SM-а уничтожить плагины последними
void mlSceneManager::DestroyPluginLater(moMedia* apMO){
	mvPluginsMOs.push_back(apMO);
}

static JSBool
global_enumerate(JSContext *cx, JSObject *obj)
{
    return JS_EnumerateStandardClasses(cx, obj);
}

static JSBool
global_resolve(JSContext *cx, JSObject *obj, jsval id, uintN flags,
               JSObject **objp)
{
    if ((flags & JSRESOLVE_ASSIGNING) == 0) {
        JSBool resolved;

        if (!JS_ResolveStandardClass(cx, obj, id, &resolved))
            return JS_FALSE;
        if (resolved) {
            *objp = obj;
            return JS_TRUE;
        }
    }

    return JS_TRUE;
}

JSClass global_class = {
    "global", JSCLASS_NEW_RESOLVE,
    JS_PropertyStub,  JS_PropertyStub,
    JS_PropertyStub,  JS_PropertyStub,
    global_enumerate, (JSResolveOp) global_resolve,
    JS_ConvertStub,   JS_FinalizeStub
};


/** JavaScript error report functions
  */

static JSBool reportWarnings = JS_TRUE;

static void smJSErrorReporter(JSContext *cx, const char *message, JSErrorReport *report){
	if(GPSM(cx)->mbWasExitFromScript)
		return;
	JSString* sError = mlFormatJSError(cx, message, report, reportWarnings);
	if(sError == (JSString*)1){
		TRACE(cx, "js-error:\'%s\'\n",message);
	}else{
		TRACE(cx, "js-error:\'%s\'\n",JS_GetStringBytes(sError));
//		::AfxMessageBox(JS_GetStringBytes(sError),MB_OK);
	}
}

JSBool smJSDebugErrorHook(JSContext *cx, const char *message, JSErrorReport *report, void *closure){
	if(GPSM(cx)->mbWasExitFromScript)
		return JS_TRUE;
	JSString* sError = mlFormatJSError(cx, message, report, reportWarnings);
	if(sError == (JSString*)1){
		TRACE(cx, "js-exception:\'%s\'\n", message);
	}else{
		char* pcsError = JS_GetStringBytes(sError);
		TRACE(cx, "js-exception:\'%s\'\n",pcsError);
	}
	return JS_TRUE;
}

static unsigned long gulPreviousGCTime = 0;
static unsigned long gulGCDeltaTime = 0; // Разница по времени между последними срабатываниями GC
static unsigned long gulBetGCObjectCreated = 10000; // Сколько JS-объектов было создано со времени последнего вызова GC
static long glTotalGCObjectCount = 0;	// общее количество созданных JS-объектов
static long glLastTotalGCOCLogged = 0;	// количество созданных JS-объектов, при котором был последний вывод в лог


void DumpJSObjects(JSContext *cx, bool abDontCallGC = false);
#ifdef ML_TRACE_ALL_ELEMS
void LogElemsStatistics(JSContext *cx);	// вывести в лог статистику во всем существующим RMML-элементам
#endif

unsigned long gcStartTime;

JSBool RMMLJSGCCallback(JSContext *cx, JSGCStatus status){
	if(!GPSM(cx) || !GPSM(cx)->GetContext())
		return JS_TRUE;
	if(status == JSGC_BEGIN){
		if(glTotalGCObjectCount > 0)
			mlTrace(cx, "[RMML] Total JS-object count: %d \n", glTotalGCObjectCount);
		mlTrace(cx, "[RMML] JSGC_BEGIN\n");
		gcStartTime = GPSM(cx)->GetContext()->mpApp->GetTimeStamp();
#ifdef _DEBUG
		GPSM(cx)->OptimizeLists();
#endif
	}else if(status == JSGC_END){
		mlTrace(cx, "[RMML] JSGC_END\n");
		if(glTotalGCObjectCount > 0)
			mlTrace(cx, "[RMML] Total JS-object count: %d \n", glTotalGCObjectCount);
		unsigned long ulNow = GPSM(cx)->GetContext()->mpApp->GetTimeStamp();
		//if(gulBetGCObjectCreated < 100000){
			DumpJSObjects(cx, true);
		//}
		#ifdef ML_TRACE_ALL_ELEMS
		LogElemsStatistics(cx);	// вывести в лог статистику во всем существующим RMML-элементам
		#endif
		if (GPSM(cx)->GetContext()->mpLogWriter)
		{
			GPSM(cx)->GetContext()->mpLogWriter->WriteLn("GC ends in ", (int)(ulNow - gcStartTime), " ms");
		}
		gulGCDeltaTime = ulNow - gulPreviousGCTime;
		gulPreviousGCTime = ulNow;
		gulBetGCObjectCreated = 0;
	}
	return JS_TRUE;
}

static bool gbInJSObjectHook = false;
static int giInUpdateNewObjectCount = 0; // количество JS-объектов, созданных в течение текущего Update-а

void CountInUpdateNewObjects(const char* apcClassName, const wchar_t* apcCS){
	giInUpdateNewObjectCount = giInUpdateNewObjectCount;
}

typedef std::map<JSObject*, mlString> CreatedJSObjects;
static CreatedJSObjects mapJSObjects;	// мапка существующих JS-объектов
typedef pair <JSObject*, mlString> CreatedJSObjectPair;

#ifdef ML_TRACE_ALL_ELEMS
// список вообще всех существующих RMML-элементов
typedef std::map<mlRMMLElement*, int> ExistingRMMLElements;
ExistingRMMLElements gmapAllElems;
typedef pair <mlRMMLElement*, int> ExistingRMMLElementPair;
#endif

// функция вызывается на каждое создание JS-объекта
void RMMLJSObjectHook(JSContext *cx, JSObject *obj, JSBool isNew, void *closure){
#ifdef ENABLE_RELEASE_OBJECT_STAT
	if(isNew){
		glTotalGCObjectCount++;
	}else{
		glTotalGCObjectCount--;
	}
	// если общее количество JS-объектов значительно изменилось, 
	// то выводим сообщение в лог
	long lOCDiff = glTotalGCObjectCount - glLastTotalGCOCLogged;
	//if(lOCDiff < 0) lOCDiff = -lOCDiff;
	if(lOCDiff > 5000){
		glLastTotalGCOCLogged = glTotalGCObjectCount;
		mlTrace(cx, "[RMML] Total JS-object count: %d \n", glTotalGCObjectCount);
	}
#endif
#ifdef ENABLE_OBJECT_STAT
	if(!isNew){
		/*CreatedJSObjects::iterator mi = mapJSObjects.find(obj);
		if(mi != mapJSObjects.end())
		{
			mlString wsDescr = mi->second;
			if(wsDescr.find(L"(Function)") != mlString::npos){
				int hh=0;
			}
		}*/
		mapJSObjects.erase(obj);
		return;
	}
	gulBetGCObjectCreated++;
	if(!GPSM(cx) || !GPSM(cx)->GetContext())
		return;
	if(!(GPSM(cx)->GetMyAvatar()))
		return;
	if(gbInJSObjectHook)
		return;
	gbInJSObjectHook = true;
//	if(++giInUpdateNewObjectCount > 1000){
//	if(gulGCDeltaTime > 0 && gulGCDeltaTime < 500){
		JSClass* pClass = ML_JS_GETCLASS(cx, obj);
		mlString wsCS = L"";
		#ifndef _DEBUG
		wsCS = GPSM(cx)->GetJSCallStack();
		#endif
		mlString wsDescr = L"(";
		wsDescr += cLPWCSTR(pClass->name);
		wsDescr += L") ";
		wsDescr += wsCS;
		wsDescr += L" (";
		wsDescr += GPSM(cx)->GetAllocComment();
		wsDescr += L")";		
//#ifdef ENABLE_OBJECT_STAT
		std::string currentExecutingScriptSrc;
		int currentExecutingScriptLine;
		GPSM(cx)->GetCurrentExecutingScriptParams( currentExecutingScriptSrc, currentExecutingScriptLine);
		char buf[30]; 
		_itoa_s( currentExecutingScriptLine, buf, 30, 10);
		
		wsDescr += L" (";
		wsDescr += cLPWCSTR(currentExecutingScriptSrc.c_str());
		wsDescr += L", ";
		wsDescr += cLPWCSTR(buf);
		wsDescr += L")";		
//#endif
		mapJSObjects.insert(CreatedJSObjectPair(obj, wsDescr));
		//mlTrace(cx, "[RMML] new JS-object (%s) (%S)\n", pClass->name, wsCS.c_str());
//		CountInUpdateNewObjects(pClass->name, wsCS.c_str());
//	}
	gbInJSObjectHook = false;
#endif
}

struct JSObjectTypeStat{
	int count;
	mlString descr;
	JSObjectTypeStat(int aCount, mlString aDescr){
		count = aCount;
		descr = aDescr;
	}
	bool operator < (const JSObjectTypeStat* pOther)
	{
		return count < pOther->count;
	}
};

bool OTSSortPredicate(const JSObjectTypeStat* lhs, const JSObjectTypeStat* rhs)
{
	return lhs->count > rhs->count;
}

void mlSceneManager::SetAllocComment(const wchar_t* apwcAllocComment){
	mwsAllocComment = apwcAllocComment;
}

void DumpJSObjects(JSContext *cx, bool abDontCallGC){
	if(!abDontCallGC)
		JS_GC(cx);
	typedef std::map<mlString, int> CreatedJSObjectTypes;
	typedef std::pair<mlString, int> CreatedJSObjectTypesPair;
	// подсчитываем количество "однотипных" объектов
	CreatedJSObjectTypes mapJSObjectsByTypes;
	CreatedJSObjects::iterator mi = mapJSObjects.begin();
	for(; mi != mapJSObjects.end(); mi++){
		JSObject* jso = mi->first;
		mlString wsDescr = mi->second;
		CreatedJSObjectTypes::iterator mi2 = mapJSObjectsByTypes.find(wsDescr);
		if(mi2 == mapJSObjectsByTypes.end()){
			mapJSObjectsByTypes.insert(CreatedJSObjectTypesPair(wsDescr, 1));
		}else{
			mi2->second++;
		}
	}
	// сортируем 
	typedef std::list<JSObjectTypeStat*> SortedVector2;
	SortedVector2 vSorted;
	int iObjectCount = 0;
	CreatedJSObjectTypes::iterator mi2 = mapJSObjectsByTypes.begin();
	for(; mi2 != mapJSObjectsByTypes.end(); mi2++){
		MP_NEW_P2( jsObjectTypeStat, JSObjectTypeStat, mi2->second, mi2->first);
		vSorted.push_back( jsObjectTypeStat);
	}
	vSorted.sort(OTSSortPredicate);
	// выводим в лог
	if(iObjectCount > 0)
		mlTrace(cx, "[RMML] Total JS-object count: %d \n", iObjectCount);
	SortedVector2::iterator li = vSorted.begin();
	for(; li != vSorted.end(); li++){
		/*if((*li)->count < 10)
			continue;*/
		mlTrace(cx, "[RMML] (%d) (%S)\n", (*li)->count, (*li)->descr.c_str());
		 MP_DELETE( *li);
	}
}

#ifdef ML_TRACE_ALL_ELEMS

struct JSObjectTypeCount{
	int count;
	int type;
	JSObjectTypeCount(int aCount, int aType){
		count = aCount;
		type = aType;
	}
	bool operator < (const JSObjectTypeCount* pOther)
	{
		return count < pOther->count;
	}
};

bool OTCSortPredicate(const JSObjectTypeCount* lhs, const JSObjectTypeCount* rhs)
{
	return lhs->count > rhs->count;
}

// Получить имя класса верхнего парента элемента
// abSelf=true, если непосредственно сам элемент является экземпляром класса
mlString GetTopParentClass(mlRMMLElement* apElem, bool& abSelf, mlRMMLElement* apRoot = NULL){
	abSelf = false;
	mlString wsClassName = L"[NO CLASS]";
	// Если элемент в иерархии текущей сцены, то:
	mlRMMLComposition* pScene = (mlRMMLComposition*)apElem->GetScene();
	if(pScene != NULL){
		mlRMMLElement* pParent = apElem;
		mlRMMLElement* pPrevParent = pParent;
		mlRMMLElement* pLastClassedParent = NULL;
		while(pParent != NULL){
			//  если один из родителей - 3D-объект сцены, 
			if(pParent->mType == MLMT_SCENE3D){
				//  то берем класс этого родителя
				mlRMMLElement* pClass = pPrevParent->GetClass();
				if(pClass != NULL){
					if(pPrevParent == apElem && pPrevParent->mRMMLType != MLMT_SCRIPT)
						abSelf = true;
					return pClass->GetName();
				}
			}
			/*
			//  иначе берем класс самого верхнего родителя после сцены
			if(pParent == GPSM(pParent->mcx)->GetScene() 
				|| pParent == (mlRMMLElement*)GPSM(pParent->mcx)->GetUI()
			){
				mlRMMLElement* pClass = pPrevParent->GetClass();
				if(pClass != NULL){
					if(pPrevParent == apElem)
						abSelf = true;
					return pClass->GetName();
				}
			}
			*/
			if(pPrevParent->GetClass() != NULL)
				pLastClassedParent = pPrevParent;
			//
			pPrevParent = pParent;
			pParent = pParent->mpParent;
		}
		// иначе берем класс самого верхнего родителя с классом
		if(pLastClassedParent != NULL){
			mlRMMLElement* pClass = pLastClassedParent->GetClass();
			if(pClass != NULL){
				if(pLastClassedParent == apElem && pLastClassedParent->mRMMLType != MLMT_SCRIPT)
					abSelf = true;
				return pClass->GetName();
			}
		}
		if(pScene == GPSM(apElem->mcx)->GetScene())
			return L"[scene:NO CLASS]";
		if(pScene == (mlRMMLElement*)GPSM(apElem->mcx)->GetUI())
			return L"[UI:NO CLASS]";
	}else{
		// иначе берем имя самого верхнего родителя
		mlRMMLElement* pParent = apElem;
		mlRMMLElement* pPrevParent = pParent;
		while(pParent != NULL){
			pPrevParent = pParent;
			pParent = pParent->mpParent;
			if(apRoot != NULL && pParent == apRoot)
				pParent = NULL;
		}
		if(pPrevParent == apElem && pPrevParent->mRMMLType != MLMT_SCRIPT)
			abSelf = true;
		if(apRoot != NULL){
			mlRMMLElement* pClass = pPrevParent->GetClass();
			if(pClass != NULL && pPrevParent->mRMMLType != MLMT_SCRIPT){
				return mlString(L"") + pClass->GetName();
			}
			return mlString(L"name:") + pPrevParent->GetName();
		}
		return mlString(L"Class:")+pPrevParent->GetName();
	}
	return wsClassName;
}

void LogElementStatistics(JSContext *cx, mlRMMLElement* apMLEl){
	typedef std::map<mlString, int> MapElemsByClass;
	typedef std::pair<mlString, int> MapElemsByClassPair;
	MapElemsByClass mapElemsByClass;	// общее количество элементов экземпляров классов
	MapElemsByClass mapClassInstances;	// общее количество экземпляров классов
	MapElemsByClass mapScriptsByClass;	// общее количество скриптов во всех экземплярах класса

	mlRMMLIterator iter(cx, apMLEl);
	mlRMMLElement* pElem = NULL;
	while((pElem = iter.GetNext()) != NULL){
		bool bSelf = false;
//		mlString wsDescr = GetTopParentClass(pElem, bSelf, pMainClass);
		if(pElem->mRMMLType == MLMT_SCRIPT)
			continue;
		bSelf = true;
		mlString wsDescr;
		mlRMMLElement* pClass = pElem->GetClass();
		if(pClass != NULL && pElem->mRMMLType != MLMT_SCRIPT){
			wsDescr = pClass->GetName();
		}else{
			wsDescr = mlString(L"name:") + pElem->GetName();
		}
		MapElemsByClass::iterator mi2 = mapElemsByClass.find(wsDescr);
		if(mi2 == mapElemsByClass.end()){
			mapElemsByClass.insert(MapElemsByClassPair(wsDescr, 1));
		}else{
			mi2->second++;
		}
		if(bSelf){
			MapElemsByClass::iterator mi4 = mapClassInstances.find(wsDescr);
			if(mi4 == mapClassInstances.end()){
				mapClassInstances.insert(MapElemsByClassPair(wsDescr, 1));
			}else{
				mi4->second++;
			}
		}
		if(pElem->mRMMLType == MLMT_SCRIPT){
			MapElemsByClass::iterator mi6 = mapScriptsByClass.find(wsDescr);
			if(mi6 == mapScriptsByClass.end()){
				mapScriptsByClass.insert(MapElemsByClassPair(wsDescr, 1));
			}else{
				mi6->second++;
			}
		}
	}
	typedef std::list<JSObjectTypeStat*> SortedVector2;
	SortedVector2 vSorted2;
	int iObjectCount = 0;
	MapElemsByClass::iterator mi3 = mapElemsByClass.begin();
	for(; mi3 != mapElemsByClass.end(); mi3++){
		MP_NEW_P2( jsObjectTypeStat, JSObjectTypeStat, mi3->second, mi3->first);
		vSorted2.push_back( jsObjectTypeStat);
		iObjectCount += mi3->second;
	}
	vSorted2.sort(OTSSortPredicate);
	// выводим в лог
	int iAllAnotherElementsCount = 0;
	SortedVector2::iterator li3 = vSorted2.begin();
	for(; li3 != vSorted2.end(); li3++){
		if((*li3)->count < 5){
			iAllAnotherElementsCount++;
			continue;
		}
		mlString wsDescr = (*li3)->descr;
		char pszCounts[100];
/*		int iInstanceCount = 0;
		MapElemsByClass::iterator mi5 = mapClassInstances.find(wsDescr);
		if(mi5 != mapClassInstances.end())
			iInstanceCount = mi5->second; 
		int iScriptsCount = 0;
		MapElemsByClass::iterator mi6 = mapScriptsByClass.find(wsDescr);
		if(mi6 != mapScriptsByClass.end())
			iScriptsCount = mi6->second;
		if(iInstanceCount == 1){
			rmmlsafe_sprintf(pszCounts, 100, 0, "%d s:%d", (*li3)->count, iScriptsCount);
		}else if((*li3)->count == iInstanceCount || iInstanceCount == 0){
			rmmlsafe_sprintf(pszCounts, 100, 0, "%d/%d s:%d", (*li3)->count, iInstanceCount, iScriptsCount);
		}else{
			rmmlsafe_sprintf(pszCounts, 100, 0, "%d/%d=%d s:%d/%d=%d", (*li3)->count, iInstanceCount, (*li3)->count / iInstanceCount, iScriptsCount, iInstanceCount, iScriptsCount / iInstanceCount);
		} */
		rmmlsafe_sprintf(pszCounts, 100, 0, "%d", (*li3)->count);

		mlTrace(cx, "[RMML]   (%s) (%S)\n", pszCounts, (*li3)->descr.c_str());
	}
	mlTrace(cx, "[RMML]   (%d) all other\n", iAllAnotherElementsCount);
	SortedVector2::iterator li4 = vSorted2.begin();
	for(; li4 != vSorted2.end(); li4++){
		MP_DELETE(*li4);
	}
	vSorted2.clear();
}

// посчитать и вывести в лог статистику по указанному классу
void LogClassStatistics(JSContext *cx, const mlString awsClassName){
	mlRMMLElement* pMainClass = NULL;
	jsval vClss;
	JS_GetProperty(cx, JS_GetGlobalObject(cx), GJSONM_CLASSES, &vClss);
	jsval v;
	JS_GetUCProperty(cx, JSVAL_TO_OBJECT(vClss), (const jschar*)awsClassName.c_str(), -1, &v);
	pMainClass = JSVAL_TO_MLEL(cx, v);
	if(pMainClass == NULL)
		return;
	LogElementStatistics(cx, pMainClass);
}

inline std::string replace(std::string text, std::string s, std::string d)
{
	for(unsigned index=0; index=text.find(s, index), index!=std::string::npos;)
	{
		text.replace(index, s.length(), d);
		index+=d.length();
	}
	return text;
}

inline mlString replace(mlString text, mlString s, mlString d)
{
	for(unsigned index=0; index=text.find(s, index), index != mlString::npos;)
	{
		text.replace(index, s.length(), d);
		index+=d.length();
	}
	return text;
}

// вывести в лог статистику во всем существующим RMML-элементам
void LogElemsStatistics(JSContext *cx){
	mlTrace(cx, "[RMML] **** RMML Elements Statistics **** \n", gmapAllElems.size());
	mlTrace(cx, "[RMML] Total number of elements: %d\n", gmapAllElems.size());
	// Считаем и выводим статистику по типам и классам родителей элементов
	typedef std::map<int, int> MapByType;
	typedef pair<int, int> MapByTypePair;
	MapByType mapByType;	// статистика по типар RMML-элементов

	typedef std::map<mlString, int> MapElemsByClass;
	typedef std::pair<mlString, int> MapElemsByClassPair;
	MapElemsByClass mapElemsByClass;	// общее количество элементов экземпляров классов
	MapElemsByClass mapClassInstances;	// общее количество экземпляров классов
	MapElemsByClass mapScriptsByClass;	// общее количество скриптов во всех экземплярах класса
	int iTotalScriptsCount = 0;	// общее количество скриптов
	int iNotCompiledScriptsCount = 0;	// общее количество скриптов
	int iNotCompiledScriptsTotalSize = 0;	// общий объем неоткомпилированных скриптов

	ExistingRMMLElements::const_iterator ci = gmapAllElems.begin();
	for(; ci != gmapAllElems.end(); ci++){
		mlRMMLElement* pElem = ci->first;
//if(pElem->IsComposition() && pElem->GetType() != MLMT_SCRIPT 
//   && pElem->GetClass() != NULL && isEqual(pElem->GetClass()->GetName(), L"CMiniMap"))
//int hh=0;
		int iType = pElem->mType;
		//if(iType & 0xFF == MLMT_SCRIPT){
		//	iType = MLMT_SCRIPT;
		//}
		mapByType[iType]++;
		/// по классам родителей
		// формируем описание класса-родителя
		bool bSelf = false;
		mlString wsDescr = GetTopParentClass(pElem, bSelf);
		MapElemsByClass::iterator mi2 = mapElemsByClass.find(wsDescr);
		if(mi2 == mapElemsByClass.end()){
			mapElemsByClass.insert(MapElemsByClassPair(wsDescr, 1));
		}else{
			mi2->second++;
		}
		if(bSelf){
			MapElemsByClass::iterator mi4 = mapClassInstances.find(wsDescr);
			if(mi4 == mapClassInstances.end()){
				mapClassInstances.insert(MapElemsByClassPair(wsDescr, 1));
			}else{
				mi4->second++;
			}
		}
		if(pElem->mRMMLType == MLMT_SCRIPT){
			iTotalScriptsCount++;
			MapElemsByClass::iterator mi6 = mapScriptsByClass.find(wsDescr);
			if(mi6 == mapScriptsByClass.end()){
				mapScriptsByClass.insert(MapElemsByClassPair(wsDescr, 1));
			}else{
				mi6->second++;
			}
			mlRMMLScript* pScript = (mlRMMLScript*)pElem;
			if(!(pScript->IsCompiled())){
				iNotCompiledScriptsCount++;
				iNotCompiledScriptsTotalSize += pScript->GetNonCompiledScriptSize();
			}
		}
	}
	mlTrace(cx, "[RMML] Total number of scripts: %d\n", iTotalScriptsCount);
	mlTrace(cx, "[RMML] Total number of non-compiled scripts: %d\n", iNotCompiledScriptsCount);
	mlTrace(cx, "[RMML] Total size of non-compiled scripts: %d byte\n", iNotCompiledScriptsTotalSize);
	// сортируем 
	typedef std::list<JSObjectTypeCount*> SortedVector3;
	SortedVector3 vSorted;
	MapByType::const_iterator ci2 = mapByType.begin();
	for(; ci2 != mapByType.end(); ci2++){
		MP_NEW_P2( jsObjectTypeCount, JSObjectTypeCount, ci2->second, ci2->first);
		vSorted.push_back( jsObjectTypeCount);
	}
	vSorted.sort(OTCSortPredicate);
	// выводим в лог
	mlTrace(cx, "[RMML] By type statistics: %d\n", iTotalScriptsCount);
	SortedVector3::iterator vi = vSorted.begin();
	for(; vi != vSorted.end(); vi++){
		if((*vi)->count < 100)
			continue;
		mlTrace(cx, "[RMML]  %s:%d\n", mlRMML::GetTypeName((*vi)->type).c_str(), (*vi)->count);
		MP_DELETE(*vi);
	}
	vSorted.clear();
	/// Выводим статистику по классам
	typedef std::list<JSObjectTypeStat*> SortedVector2;
	SortedVector2 vSorted2;
	int iObjectCount = 0;
	MapElemsByClass::iterator mi3 = mapElemsByClass.begin();
	for(; mi3 != mapElemsByClass.end(); mi3++){
		MP_NEW_P2( jsObjectTypeStat, JSObjectTypeStat, mi3->second, mi3->first);
		vSorted2.push_back( jsObjectTypeStat);
		iObjectCount += mi3->second;
	}
	vSorted2.sort(OTSSortPredicate);
	// выводим в лог
	int iAllAnotherElementsCount = 0;
	for(int iClasses = 0; iClasses < 2; iClasses++){
		if(iClasses == 0){
			mlTrace(cx, "[RMML] Format: ([C]/[n]=[c] s:[S]/[n]=[s]) ([CN])\n");
			mlTrace(cx, "[RMML]   [CN] - class name\n");
			mlTrace(cx, "[RMML]   [n] - number of instances of the class\n");
			mlTrace(cx, "[RMML]   [C] - number of elements (including scripts) in all instances of the class\n");
			mlTrace(cx, "[RMML]   [c] - number of elements in one instance of the class\n");
			mlTrace(cx, "[RMML]   [S] - number of scripts in all instances of the class\n");
			mlTrace(cx, "[RMML]   [s] - number of scripts in one instance of the class\n");
		}else{
			mlTrace(cx, "[RMML] Format: ([C] s:[S]) ([CN])\n");
			mlTrace(cx, "[RMML]   [CN] - class name\n");
			mlTrace(cx, "[RMML]   [C] - number of elements in the class\n");
			mlTrace(cx, "[RMML]   [S] - number of scripts in the class\n");
		}
		iAllAnotherElementsCount = 0;
		SortedVector2::iterator li3 = vSorted2.begin();
		for(; li3 != vSorted2.end(); li3++){
			if((*li3)->count < 100){
				iAllAnotherElementsCount++;
				continue;
			}
			mlString wsDescr = (*li3)->descr;
			bool bClass = (wsDescr.find(L"Class:") == 0);
			if(iClasses == 1){
				// выводим статистику по классам
				if(!bClass)
					continue;
			}else{
				// выводим статистику по экземплярам классов
				if(bClass)
					continue;
			}
			int iInstanceCount = 0;
			MapElemsByClass::iterator mi5 = mapClassInstances.find(wsDescr);
			if(mi5 != mapClassInstances.end())
				iInstanceCount = mi5->second;
			int iScriptsCount = 0;
			MapElemsByClass::iterator mi6 = mapScriptsByClass.find(wsDescr);
			if(mi6 != mapScriptsByClass.end())
				iScriptsCount = mi6->second;
			char pszCounts[100];
			if(iInstanceCount == 1){
				rmmlsafe_sprintf(pszCounts, 100, 0, "%d s:%d", (*li3)->count, iScriptsCount);
			}else if((*li3)->count == iInstanceCount || iInstanceCount == 0){
				rmmlsafe_sprintf(pszCounts, 100, 0, "%d/%d s:%d", (*li3)->count, iInstanceCount, iScriptsCount);
			}else{
				rmmlsafe_sprintf(pszCounts, 100, 0, "%d/%d=%d s:%d/%d=%d", (*li3)->count, iInstanceCount, (*li3)->count / iInstanceCount, iScriptsCount, iInstanceCount, iScriptsCount / iInstanceCount);
			}

			mlTrace(cx, "[RMML] (%s) (%S)\n", pszCounts, (*li3)->descr.c_str());

			if(bClass){
				mlString wsClassName = wsDescr;
				wsClassName = replace(wsDescr, L"Class:", L"");
				LogClassStatistics(cx, wsClassName);
			}else{
				if((*li3)->count > 10000 && iInstanceCount <= 3){
					ExistingRMMLElements::const_iterator ci = gmapAllElems.begin();
					for(; ci != gmapAllElems.end(); ci++){
						mlRMMLElement* pElem = ci->first;
						bool bSelf = false;
						mlString wsClassDescr = GetTopParentClass(pElem, bSelf);
						if(!bSelf)
							continue;
						mlString wsClassName = wsDescr;
						if(wsClassName == wsClassDescr){
							LogElementStatistics(cx, pElem);
						}
					}
				}
			}
		}
	}
	SortedVector2::iterator li3 = vSorted2.begin();
	for(; li3 != vSorted2.end(); li3++){
		MP_DELETE(*li3);
	}
	vSorted2.clear();
	mlTrace(cx, "[RMML] Number of all other elements: %d\n", iAllAnotherElementsCount);
	mlTrace(cx, "[RMML] **** End of RMML Elements Statistics **** \n", gmapAllElems.size());
}
#endif

bool mlSceneManager::InitJSE(void){
	/////////////////////////
	// Initialize JS Engine
	// set up global JS variables, including global and custom objects

//jsword wd = JS_GetCurrentThreadId();
//	rt = JS_NewRuntime(8L * 1024L * 1024L);
	JSRuntime* rt = gJSRunTime;
	if (!rt) return false;

	JS_SetDebugErrorHook(rt, smJSDebugErrorHook, NULL);

	// establish a context
	cx = JS_NewContext(rt, 8192);
	if (cx == NULL)	return false;

	GUARD_JS_CALLS(cx);

	JS_SetErrorReporter(cx, smJSErrorReporter);

	// initialize the built-in JS objects and the global object
	JSObject* glob = JS_NewObject(cx, &global_class, NULL, NULL);
	if (!glob) return false;
	JSBool builtins = JS_InitStandardClasses(cx, glob);
	if (!builtins) return false;
	if (!JS_DefineFunctions(cx, glob, shell_functions))
		return false;

#ifdef JS_THREADSAFE
	JS_SetContextPrivate(cx, this);
#endif

	// инициализируем вспомогательные массивы 
	mlJSClass::Init();

	// initialize extern global object
	mRMML.InitGlobalJSObjects(cx,glob);
	InitGlobalJSObjects(cx,glob);

	JSGCCallback pPrevJSCallback = JS_SetGCCallback(cx, RMMLJSGCCallback);

#if defined(ENABLE_OBJECT_STAT) || defined(ENABLE_RELEASE_OBJECT_STAT)
	JS_SetObjectHook(rt, RMMLJSObjectHook, NULL);
#endif

	return true;
}

void mlSceneManager::DestroyJSE(void)
{
//DumpJSObjects(cx);
	mjsoPlayer = NULL;
	// Before exiting the application, free the JS run time
	mBuildEvalCtxt.DestroyCtxt();
	JS_DestroyContext(cx);
//	JS_DestroyRuntime(rt); // уничтожается в деструкторе mlJSRunTime
//	JS_ShutDown();
}

JSClass mlSceneManager::JSRMMLClassesClass = {
	"_Classes", 0, // JSCLASS_HAS_PRIVATE
	JS_PropertyStub, JS_PropertyStub, 
	JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL,
	NULL, NULL, NULL
};

/**
 * Обработка событий
 */ 

char aVisEvIds[]={
	0, // 0
	mlRMMLVisible::EVID_onMouseMove, // 1
	mlRMMLVisible::EVID_onMouseDown, // 2
	mlRMMLVisible::EVID_onMouseUp, // 3
	0, // 4
	mlRMMLVisible::EVID_onRMouseDown, // 5
	mlRMMLVisible::EVID_onRMouseUp, // 6
	mlRMMLVisible::EVID_onMouseWheel, // 7
	0
};

using oms::omsUserEvent;

void mlSceneManager::CallVisibleListeners(const v_elems* apvElems, int aiEventIdx){
	const v_elems* pv = apvElems;
	v_elems::const_iterator viBegin = pv->begin();
	v_elems::const_iterator viEnd = pv->end();
	v_elems::const_iterator vi = viBegin;
	for(int i = 0; vi != viEnd; vi++, i++){
		mlRMMLElement* pMLEl = *vi;
		if(pMLEl == NULL)
			continue;
		if(pMLEl->GetVisible() != NULL && !pMLEl->GetVisible()->GetAbsVisible())
			continue;
		pMLEl->CallListeners(aVisEvIds[aiEventIdx]);
		// если в процессе обработки события изменился список слушателей
		if(pv->begin() != viBegin || pv->end() != viEnd){
			// значит итераторы уже не валидные
			int j = i;
			viBegin = pv->begin();
			viEnd = pv->end();
			vi = viBegin;
			for(i = 0; vi != viEnd; vi++, i++){
				if(*vi == pMLEl)
					break;
			}
			// если из списка удалился сам элемент, у которого вызывали обработчик
			if(vi == viEnd){
				// то искать будем опираясь на номер элемента в векторе (i)
				vi = viBegin;
				for(i = 0; vi != viEnd; vi++, i++){
					if(i == j)
						break;
				}
			}
			if(vi == viEnd)
				break;	// восстановить итератор не удалось
		}
	}
}

bool mlSceneManager::GetAbsEnabled(mlRMMLVisible* apVisible){
	if(apVisible == NULL)
		return true;
	mlRMMLPButton* pButVisElem = apVisible->GetElem_mlRMMLVisible()->GetButton();
	return pButVisElem->GetAbsEnabled();
}

void mlSceneManager::SetCursorForUnderMouse(bool abForce){
	unsigned int uHandCursor;
	if(    mpButUnderMouse != NULL 
		&& mpButUnderMouse != (mlRMMLElement*)1 
		&& mpButUnderMouse->GetButton() != NULL
		&& mpButUnderMouse->mbBtnEvListenersIsSet 
		&& GetAbsEnabled(mBUM.mpVisible)
		&& (uHandCursor = mpButUnderMouse->GetButton()->UseHandCursor())
	){ 
		SetHandCursor(uHandCursor, abForce);
	}else{
		SetNormalCursor(-1, abForce);
	}
}

mlSceneManager::mlMouseEventParams::mlMouseEventParams(){
	doubleClick = -1;
	leftButton = -1;
	middleButton = -1; 
	rightButton = -1;
	ctrl = -1;
	shift = -1;
}

//void mlSceneManager::mlMouseEventParams::Update(const mlSceneManager::mlMouseEventParams &aMEP){
//	if(aMEP.doubleClick != -1)
//		doubleClick = aMEP.doubleClick;
//	if(aMEP.middleButton != -1)
//		middleButton = aMEP.middleButton;
//	if(aMEP.rightButton != -1)
//		rightButton = aMEP.rightButton;
//	if(aMEP.ctrl != -1)
//		ctrl = aMEP.ctrl;
//	if(aMEP.shift != -1)
//		shift = aMEP.shift;
//}

void mlSceneManager::SetMouseEventBoolProperty(const char* apcName, int aiValue){
	if(aiValue != -1){
		jsval jsv;
		if(aiValue == 0)
			jsv = JSVAL_FALSE;
		else
			jsv = JSVAL_TRUE;
		JS_SetProperty(cx, mjsoEvent, apcName, &jsv);
	}
}

void mlSceneManager::OnVisibilityDistanceKoefChanged(int koef)
{
	mlSynchData oData;
	oData << koef;
	GPSM(cx)->GetContext()->mpInput->AddCustomEvent(CEID_onVisibilityDistanceKoefChanged, (unsigned char*)oData.data(), oData.size());
}

// Установить параметры событий мыши, устанавливаемые в Event при генерации этих событий
// abSetEventObjProps - устанавливать соответствующие свойства в глобальный объект Event или просто запомнить?
void mlSceneManager::SetMouseEventParams(/*const mlMouseEventParams aMEP, bool abSetEventObjProps*/){
	//mMouseEventParams.Update(aMEP);
	//if(abSetEventObjProps){
		DeleteMouseEventParams();
		SetMouseEventBoolProperty("doubleClick", mMouseEventParams.doubleClick);
		SetMouseEventBoolProperty("leftButton", mMouseEventParams.leftButton);
		SetMouseEventBoolProperty("middleButton", mMouseEventParams.middleButton);
		SetMouseEventBoolProperty("rightButton", mMouseEventParams.rightButton);
		SetMouseEventBoolProperty("ctrl", mMouseEventParams.ctrl);
		SetMouseEventBoolProperty("shift", mMouseEventParams.shift);
		//
		SetTextureXYToEventGO();
	//}
}

void mlSceneManager::DeleteMouseEventParams(){
	JS_DeleteProperty(cx, mjsoEvent, "doubleClick");
	JS_DeleteProperty(cx, mjsoEvent, "leftButton");
	JS_DeleteProperty(cx, mjsoEvent, "middleButton");
	JS_DeleteProperty(cx, mjsoEvent, "rightButton");
	JS_DeleteProperty(cx, mjsoEvent, "ctrl");
	JS_DeleteProperty(cx, mjsoEvent, "shift");
	//
	JS_DeleteProperty(cx, mjsoEvent, ML_EVENT_TEXTURE_X_PROP_NAME);
	JS_DeleteProperty(cx, mjsoEvent, ML_EVENT_TEXTURE_Y_PROP_NAME);
}

inline bool IsOneOf(const mlRMMLElement* apElem, const std::auto_ptr<v_elems>& pvElems){
	if(pvElems.get() == NULL || pvElems->size() == 0)
		return false;
	return (std::find(pvElems->begin(), pvElems->end(), apElem) != pvElems->end());
}

inline void CallListenersOfAll(mlRMMLElement* apMLEl, v_elems* pvElems, char aidEvent){
	if(apMLEl != NULL && apMLEl != (mlRMMLElement*)1)
		apMLEl->CallListeners(aidEvent);
	if(pvElems == NULL)
		return;
	v_elems_iter vi = pvElems->begin();
	for(; vi != pvElems->end(); vi++){
		mlRMMLElement* pMLEl = *vi;
		pMLEl->CallListeners(aidEvent);
	}
}

void mlSceneManager::OptimizeLists()
{
	OptimizeList(mvNewElements);
	OptimizeList(mvUpdatableElements);
	OptimizeList(mvUpdatableUIElements);
	OptimizeList(mvObjectsToSynch);
}

void mlSceneManager::HandleMouseEvent(omsUserEvent &aEvent){
//	if(meMode!=smmNormal && meMode!=smmLoading && meMode!=smmAuthoring) return;
//	if(mpScene==NULL) return;

	bool isRealEvent = true; // надо вызывать обработчики в скрипте
	if (aEvent.ev == omsUserEvent::MOUSEMOVE)
	{
		 isRealEvent = ((lastMouseX != aEvent.mousexy.mouse_x) || (lastMouseY != aEvent.mousexy.mouse_y));
		 lastMouseX = aEvent.mousexy.mouse_x;
		 lastMouseY = aEvent.mousexy.mouse_y;
	}

	if(aEvent.ev != omsUserEvent::MOUSEWHEEL){
	mMouseXY.x=aEvent.mousexy.mouse_x;
	mMouseXY.y=aEvent.mousexy.mouse_y;
	}
	if(aEvent.ev == omsUserEvent::MOUSEWHEEL){
		short zDelta=(short)((unsigned short) (((unsigned long) (aEvent.key_code) >> 16) & 0xFFFF));
		JS_DefineProperty(cx,mjsoMouse,"delta",INT_TO_JSVAL(zDelta),0,0,0);
	} 
	if(aEvent.key_code){
if(aEvent.ev==omsUserEvent::LBUTTONDOWN)
int hh=0;
		mBUM.Reset();
		mlRMMLElement* pBut = NULL;
		//mpVisibleUnderMouse = NULL;
		//mp3DObjectUnderMouse = NULL;
		//mpTextureObjectUnderMouse = NULL;
		// mlPoint pntLocalXY; ML_INIT_POINT(pntLocalXY);
		//mlTrace(cx, "#0000FF mouse::mCaptureMouseObject.mpButton %s\n", mCaptureMouseObject.mpButton);
		if(mCaptureMouseObject.mpButton != NULL){
			pBut = mCaptureMouseObject.mpButton;
			// если захвативший мышку объект является 3D-объектом и известен его viewport
			if(mCaptureMouseObject.mpVisible != NULL){
				// то надо получить его текстурный объект и координаты 
				// ??
			}
		}else{
			bool bAllModalsInvisible = true;
			if(!mlModalButs.empty()){
				pBut = GetModalButtonUnderMouse(mMouseXY, bAllModalsInvisible);
			}
			if(bAllModalsInvisible){
				if(meMode == smmAuthoring){
					if(mpAuthScene != NULL)
						pBut=mpAuthScene->GetButtonUnderMouse(mMouseXY, mBUM);
				}else{
					if(mpUI != NULL)
						pBut = mpUI->GetButtonUnderMouse(mMouseXY, mBUM);
					if(pBut == NULL && mpScene != NULL && meMode != smmLoading){
						pBut = mpScene->GetButtonUnderMouse(mMouseXY, mBUM);
					}					
				}
			}
		}
		//if(pBut && !pBut->mbBtnEvListenersIsSet) pBut = NULL; // bug fix by Tandy 29.04.06
		if(pBut != NULL && pBut->mType == MLMT_BROWSER){
			if(pBut->GetMO() && pBut->GetMO()->GetIBrowser()){
				moIBrowser* pBrowser = pBut->GetMO()->GetIBrowser();
				mlPoint pntAbsXY = pBut->GetVisible()->GetAbsXY();
				if(aEvent.ev == omsUserEvent::MOUSEMOVE){
					pBrowser->FireMouseMove(mMouseXY.x - pntAbsXY.x, mMouseXY.y - pntAbsXY.y);
				}else if(aEvent.ev == omsUserEvent::LBUTTONDOWN){
					// pBrowser->FireMouseDown(mMouseXY.x, mMouseXY.y); вызывается на onPress-е
				}else if(aEvent.ev == omsUserEvent::LBUTTONUP){
					pBrowser->FireMouseUp(mMouseXY.x - pntAbsXY.x, mMouseXY.y - pntAbsXY.y);
				}else if(aEvent.ev == omsUserEvent::MOUSEWHEEL){
					if(pBut == mpButFocused)
						pBrowser->FireMouseWheel(mMouseXY.x - pntAbsXY.x, mMouseXY.y - pntAbsXY.y, aEvent.key_code >> 16);
				}
			}
		}
		v_elems* pvButtonParents;
		pvButtonParents = pBut->GetButtonParents();
		if(aEvent.ev == omsUserEvent::MOUSEMOVE){
			// если не были на кнопке и наехали
			if(!mpButUnderMouse && pBut){
				CallListenersOfAll(pBut, pvButtonParents, mlRMMLButton::EVID_onRollOver);
				mpButUnderMouse = pBut;
				if (mpBUMParents != NULL)
					MP_DELETE(mpBUMParents);
				mpBUMParents = pvButtonParents;
				SetCursorForUnderMouse();
			}else if(mpButUnderMouse && !pBut){ // если были на кнопке и съехали с нее
				CallListenersOfAll(mpButUnderMouse, mpBUMParents, mlRMMLButton::EVID_onRollOut);
				mpButUnderMouse = NULL;
				if (mpBUMParents != NULL)
					MP_DELETE(mpBUMParents);
				SetNormalCursor();
			}else if(mpButUnderMouse && pBut){
				// если были на кнопке и переехали на другую
				if(mpButUnderMouse != pBut){
					if(mpBUMParents != NULL || pvButtonParents != NULL){
						// у всех, с кого ушли вызываем onRollOut
						if(mpBUMParents == NULL){
							MP_NEW_VECTOR(pv, v_elems);
							if (mpBUMParents != NULL)
								MP_DELETE(mpBUMParents);
							mpBUMParents = pv;
						}
						if(mpButUnderMouse != (mlRMMLElement*)1)
							mpBUMParents->push_back(mpButUnderMouse);
						v_elems_iter vi = mpBUMParents->begin();
						for(; vi != mpBUMParents->end(); vi++){
							if(*vi == pBut)	
								continue;	// под мышкой оказалась одна из родительских композиций
							if(pvButtonParents != NULL){
								v_elems_iter vi2 = pvButtonParents->begin();
								for(; vi2 != pvButtonParents->end(); vi2++){
									if(*vi == *vi2)
										break;
								}
								if(vi2 != pvButtonParents->end())
									continue;	// мышка все еще над кнопкой
							}
							(*vi)->CallListeners(mlRMMLButton::EVID_onRollOut);
						}
						// у всех, кто вновь появился под мышкой вызываем onRollOver
						if(pvButtonParents == NULL){
							MP_NEW_VECTOR(pv, v_elems);
							if (pvButtonParents != NULL)
								MP_DELETE(pvButtonParents);
							pvButtonParents = pv;
						}
						pvButtonParents->insert(pvButtonParents->begin(), pBut);
						vi = pvButtonParents->begin();
						for(; vi != pvButtonParents->end(); vi++){
							if(*vi == mpButUnderMouse)
								continue;
							if(mpBUMParents != NULL){
								v_elems_iter vi2 = mpBUMParents->begin();
								for(; vi2 != mpBUMParents->end(); vi2++){
									if(*vi == *vi2)
										break;
								}
								if(vi2 != mpBUMParents->end())
									continue;	// мышка все еще над кнопкой
							}
							(*vi)->CallListeners(mlRMMLButton::EVID_onRollOver);
						}
						pvButtonParents->erase(pvButtonParents->begin());
						if(pvButtonParents->size() == 0)
							//pvButtonParents.reset();
							MP_DELETE(pvButtonParents);
					}else{
						if(mpButUnderMouse != (mlRMMLElement*)1)
							mpButUnderMouse->CallListeners(mlRMMLButton::EVID_onRollOut);
						pBut->CallListeners(mlRMMLButton::EVID_onRollOver);
					}
					mpButUnderMouse = pBut;
					if (mpBUMParents != NULL)
						MP_DELETE(mpBUMParents);
					mpBUMParents = pvButtonParents;
					mbUseHandCursorChanged = true;
				}
			}
			if(mbForceSetCursor){
				SetCursorForUnderMouse(true);
				mbForceSetCursor = false;
			}
		}else{
			// нажали или отпустили кнопку мыши или клавиатуры
			if(aEvent.ev == omsUserEvent::LBUTTONDOWN){
				mpButPressed = pBut;
				mpBPParents = pvButtonParents;
				//mlTrace(cx, "#0000FF button under mouse: %s\n", pBut);
				if(pBut){
					//mlTrace(cx, "#0000FF button under mouse: %s\n", cLPCSTR(pBut->GetName()));
					// если pBut и mpVisibleUnderMouse ссылаются на разные объекты, 
					// то скорее всего mpVisibleUnderMouse - это viewport и надо вызвать у него onBeforePress
					bool bCallOnPress = true;
					if(mBUM.mpVisible != NULL && pBut->GetVisible() != mBUM.mpVisible){
						if(mBUM.mpVisible->CallOnBeforePress(mBUM))
							bCallOnPress = false;
					}
					if(bCallOnPress){
						CallListenersOfAll(pBut, pvButtonParents, mlRMMLButton::EVID_onPress);
					}
				}
				mbDragging = true;
			}else if(aEvent.ev == omsUserEvent::RBUTTONDOWN 
				  || aEvent.ev == omsUserEvent::MBUTTONDOWN
				  || aEvent.ev == omsUserEvent::LBUTTONDBLCLK
				  || aEvent.ev == omsUserEvent::RBUTTONDBLCLK
				  || aEvent.ev == omsUserEvent::MBUTTONDBLCLK
			){
				mpButRPressed = pBut;
				mpBRPParents = pvButtonParents;
				if(pBut){
					if(aEvent.ev == omsUserEvent::RBUTTONDOWN){
						mMouseEventParams.rightButton = 1;
						mMouseEventParams.doubleClick = 0;
						if (mBUM.mp3DObject != NULL)
							mpContext->mpInfoMan->ShowObject( mBUM.mp3DObject->GetFullPath(), false);
					}else if(aEvent.ev == omsUserEvent::MBUTTONDOWN){
						mMouseEventParams.middleButton = 1;
						mMouseEventParams.doubleClick = 0;
					}else if(aEvent.ev == omsUserEvent::LBUTTONDBLCLK){
						mMouseEventParams.leftButton = 1;
						mMouseEventParams.doubleClick = 1;
					}else if(aEvent.ev == omsUserEvent::RBUTTONDBLCLK){
						mMouseEventParams.rightButton = 1;
						mMouseEventParams.doubleClick = 1;
					}else if(aEvent.ev == omsUserEvent::MBUTTONDBLCLK){
						mMouseEventParams.middleButton = 1;
						mMouseEventParams.doubleClick = 1;
					}
					if(aEvent.key_code & omsUserEvent::KEYCONTROL){
						mMouseEventParams.ctrl = 1;
					}
					if(aEvent.key_code & omsUserEvent::KEYSHIFT){
						mMouseEventParams.shift = 1;
					}
					SetMouseEventParams();
					CallListenersOfAll(pBut, pvButtonParents, mlRMMLButton::EVID_onAltPress);
					DeleteMouseEventParams();
					mMouseEventParams.doubleClick = -1;
					if(aEvent.ev == omsUserEvent::LBUTTONDBLCLK){
						mMouseEventParams.leftButton = 0;
					}else if(aEvent.ev == omsUserEvent::RBUTTONDBLCLK){
						mMouseEventParams.rightButton = 0;
					}else if(aEvent.ev == omsUserEvent::MBUTTONDBLCLK){
						mMouseEventParams.middleButton = 0;
					}
					mMouseEventParams.ctrl = -1;
					mMouseEventParams.shift = -1;
				}
				mbDragging = true;
			}else if(aEvent.ev == omsUserEvent::LBUTTONUP){
				mbDragging = false;
				SetCursorForUnderMouse();
				if(mpButPressed){
					if(mpBPParents){
						mpBPParents->insert(mpBPParents->begin(), mpButPressed);
						v_elems* pBPParents = mpBPParents;
						v_elems_iter vi = mpBPParents->begin();
						for(; vi != mpBPParents->end(); vi++){
							mlRMMLElement* pMLEl = *vi;
							// если pBut является child-ом или самим pMLEl, то onRelease
							bool bOnRelease = false;
							if(pBut == pMLEl){
								bOnRelease = true;
							}else{
								mlRMMLElement* pParent = pBut;
								for(; pParent != NULL; pParent = pParent->mpParent){
									if(pParent == pMLEl){
										bOnRelease = true;
										break;
									}
								}
							}
							if(bOnRelease){
								pMLEl->CallListeners(mlRMMLButton::EVID_onRelease);
							}else{
								pMLEl->CallListeners(mlRMMLButton::EVID_onReleaseOutside);
							}
							if(mpBPParents != pBPParents)
								break;
						}
						if(mpBPParents != NULL)
							MP_DELETE(mpBPParents);
					}else{
						if(mpButPressed == pBut){
							mpButPressed->CallListeners(mlRMMLButton::EVID_onRelease);
						}else{
							mpButPressed->CallListeners(mlRMMLButton::EVID_onReleaseOutside);
						}
					}
				}
			}else if(aEvent.ev == omsUserEvent::RBUTTONUP
				  || aEvent.ev == omsUserEvent::MBUTTONUP
			){
				mbDragging = false;
				SetCursorForUnderMouse();
				if(mpButRPressed){
					if(aEvent.ev == omsUserEvent::RBUTTONUP){
						mMouseEventParams.rightButton = 1;
					}else if(aEvent.ev == omsUserEvent::MBUTTONUP){
						mMouseEventParams.middleButton = 1;
					}
					if(aEvent.key_code & omsUserEvent::KEYCONTROL){
						mMouseEventParams.ctrl = 1;
					}
					if(aEvent.key_code & omsUserEvent::KEYSHIFT){
						mMouseEventParams.shift = 1;
					}
					SetMouseEventParams();
					if(mpBRPParents){
						mpBRPParents->insert(mpBRPParents->begin(), mpButRPressed);
						v_elems* pBRPParents = mpBRPParents;
						v_elems_iter vi = mpBRPParents->begin();
						for(; vi != mpBRPParents->end(); vi++){
							mlRMMLElement* pMLEl = *vi;
							// если pBut является child-ом или самим pMLEl, то onRelease
							bool bOnRelease = false;
							if(pBut == pMLEl){
								bOnRelease = true;
							}else{
								mlRMMLElement* pParent = pBut;
								for(; pParent != NULL; pParent = pParent->mpParent){
									if(pParent == pMLEl){
										bOnRelease = true;
										break;
									}
								}
							}
							if(bOnRelease){
								pMLEl->CallListeners(mlRMMLButton::EVID_onAltRelease);
							}else{
								pMLEl->CallListeners(mlRMMLButton::EVID_onAltReleaseOutside);
							}
							if(mpBRPParents != pBRPParents)
								break;
						}
						if(mpBRPParents != NULL)
							MP_DELETE(mpBRPParents);
					}else{
						if(mpButRPressed == pBut){
							mpButRPressed->CallListeners(mlRMMLButton::EVID_onAltRelease);
						}else{
							mpButRPressed->CallListeners(mlRMMLButton::EVID_onAltReleaseOutside);
						}
					}
					DeleteMouseEventParams();
					if(aEvent.ev == omsUserEvent::RBUTTONUP){
						mMouseEventParams.rightButton = 0;
					}else if(aEvent.ev == omsUserEvent::MBUTTONUP){
						mMouseEventParams.middleButton = 0;
					}
					mMouseEventParams.ctrl = -1;
					mMouseEventParams.shift = -1;
				}
			}
		}
		if(mbUseHandCursorChanged && mpButUnderMouse != NULL && mpButUnderMouse != (mlRMMLElement*)1){
			SetCursorForUnderMouse();
			mbUseHandCursorChanged = false;
		}
	}
	// если мышь на 3D-объекте, 
	// то надо поместить в Event необходимые данные и 
	// вызвать обработчик и у mpButUnderMouse, если он не в списке maMouseListeners
	if(mBUM.mp3DObject != NULL){
		// ?? (пока не решил какие свойства нужны)
	}

	int iIdx=aEvent.ev-omsUserEvent::MOUSEMOVE+1;
	if(aEvent.ev==omsUserEvent::MOUSEWHEEL)
		iIdx=7;
	
	if (isRealEvent)
	{
		CallVisibleListeners(&(maMouseListeners[iIdx]), iIdx);
		CallVisibleListeners(&(maMouseListeners[0]), iIdx);
	}
	if(mBUM.mp3DObject != NULL && mBUM.mpTextureObject != NULL){
		// текстурный объект, поэтому его надо показывать и скрывать
		if(mBUM.mpTextureObject != NULL)
			mBUM.mpTextureObject->GetVisible()->ShowInternal();
		// подменяем мышиные координаты сцены локальными (относительно объекта текстуры)
		mlPoint pntSrcMouseXY = mMouseXY;
		mMouseXY = mBUM.mLocalXY; // mpntTextureObjectMouseXY;
		if (mpButUnderMouse && ((int)mpButUnderMouse!= 1))
		{
			mpButUnderMouse->CallListeners(aVisEvIds[iIdx]);
		}
		// восстанавливаем мышиные координаты сцены обратно
		mMouseXY = pntSrcMouseXY;
		if(mBUM.mpTextureObject != NULL)
			mBUM.mpTextureObject->GetVisible()->HideInternal();
		// сбросить свойства Event-а
		// ?? (пока не решил какие свойства нужны)
	}
}

//struct mlPressedKey{ 
//	unsigned short muKeyCode;
//	unsigned short muKeyStates;
//	unsigned short muScanCode;
//	unsigned short muAscii;
//	unsigned short muUnicode;
//	__int64 miTime;	
//};
//std::vector<mlPressedKey> mvPressedKeys;

bool mlSceneManager::IsEqual(const mlPressedKey& aKey1, const mlPressedKey& aKey2){
	return (aKey1.muKeyCode == aKey2.muKeyCode);
}

void mlSceneManager::AddPressedKey(const mlPressedKey& aKey){
	// если клавиша уже была нажата, то просто обновить запись
	PressedKeyVector::iterator vi = mvPressedKeys.begin();
	PressedKeyVector::iterator viEnd = mvPressedKeys.end();
	for(; vi != viEnd; vi++){
		if(IsEqual(*vi, aKey)){
			*vi = aKey;
			return;
		}
	}
	mvPressedKeys.push_back(aKey);
}

void mlSceneManager::RemovePressedKey(const mlPressedKey& aKey){
	// если клавиша уже была нажата, то просто обновить запись
	PressedKeyVector::iterator vi = mvPressedKeys.begin();
	PressedKeyVector::iterator viEnd = mvPressedKeys.end();
	for(; vi != viEnd; vi++){
		if(IsEqual(*vi, aKey)){
			mvPressedKeys.erase(vi);
			return;
		}
	}
}

// проверяет вектор нажатых клавиш на наличие "просроченных" и если они на самом деле уже отжаты, 
// то генерирует для них onKeyUp
void mlSceneManager::CheckPressedKeys(){
	PressedKeyVector::iterator vi = mvPressedKeys.begin();
	PressedKeyVector::iterator viEnd = mvPressedKeys.end();
	for(; vi != viEnd; vi++){
		const mlPressedKey& oKey = *vi;
		// если прошло уже больше трех секунд с момента последнего события нажатия
		if((mlTime - oKey.miTime) / 1000 > 3000){
			// то проверяем, нажата ли все еще клавиша на самом деле?
			if(!(mpContext->mpApp->IsKeyDown(oKey.muKeyCode, oKey.muScanCode))){
				// если уже не нажата, 
				// то эмулируем ее отжатие
				long lParam = oKey.muKeyStates | (((long)muScanCode) << 16);
				omsUserEvent oEvent(omsUserEvent::KEYUP, oKey.muKeyCode, lParam);
				HandleKeyEvent(oEvent);
				return;
			}
		}
	}
}

short mlSceneManager::GetGamepadKey()
{
	if (!m_ctrl || !m_ctrl->IsConnected())
	{
		return XBOXButtons::XBOX_UNKNOWN;
	}

	for (size_t i = 0; i < m_XInputButtons.size(); ++i)
	{
		if (m_pressedButton == m_XInputButtons[i])
		{
			return i + 1; //возвращается соотвествующая кнопка из перечисления в XBOXController.h 
		}
	}
	
	return XBOXButtons::XBOX_NONE;
}

short mlSceneManager::GetThumbState(SHORT xPower, SHORT yPower, SHORT deadzone)
{
	if (abs(xPower) < deadzone && abs(yPower) < deadzone)
	{
		return ThumbState::NEUTRAL;
	}

	if (abs(xPower) > abs(yPower))
	{
		return (xPower > 0) ? ThumbState::RIGHT : ThumbState::LEFT;
	}
	else
	{
		return (yPower > 0) ? ThumbState::UP : ThumbState::DOWN;
	}
}

short mlSceneManager::GetLeftThumbState()
{
	return m_pressedLeftThumb;
}

short mlSceneManager::GetRightThumbState()
{
	return m_pressedRightThumb;
}

short mlSceneManager::InternalGetLeftThumbState()
{
	if (!m_ctrl || !m_ctrl->IsConnected())
	{
		return XBOXButtons::XBOX_UNKNOWN;
	}

	return GetThumbState(m_ctrl->GetState().Gamepad.sThumbLX,
						 m_ctrl->GetState().Gamepad.sThumbLY,
						 XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
}

short mlSceneManager::InternalGetRightThumbState()
{
	if (!m_ctrl || !m_ctrl->IsConnected())
	{
		return XBOXButtons::XBOX_UNKNOWN;
	}

	return GetThumbState(m_ctrl->GetState().Gamepad.sThumbRX,
						 m_ctrl->GetState().Gamepad.sThumbRY,
						 XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
}

//возвращает строку=число, первые 2 байта - смещение по X, 2 - ые - по Y
const char* mlSceneManager::GetThumbValue(SHORT xPower, SHORT yPower, SHORT deadzone)
{
	SHORT xVal = abs(xPower) < deadzone ? 0 : xPower;
	SHORT yVal = abs(yPower) < deadzone ? 0 : yPower;
	
	int value = (xVal << 16) + yVal;

	return std::to_string(static_cast<long long>(value)).c_str();
}

const char* mlSceneManager::GetLeftThumbValue()
{
	if (!m_ctrl || !m_ctrl->IsConnected())
	{
		return ""; //== XBOXButtons::XBOX_UNKNOWN;
	}

	return GetThumbValue(m_ctrl->GetState().Gamepad.sThumbLX,
						 m_ctrl->GetState().Gamepad.sThumbLY,
						 XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
}

const char* mlSceneManager::GetRightThumbValue()
{
	if (!m_ctrl || !m_ctrl->IsConnected())
	{
		return ""; //== XBOXButtons::XBOX_UNKNOWN;
	}

	return GetThumbValue(m_ctrl->GetState().Gamepad.sThumbRX,
						 m_ctrl->GetState().Gamepad.sThumbRY,
						 XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
}

unsigned short mlSceneManager::GetTrigger(BYTE power, SHORT threshold)
{
	if (!m_ctrl || !m_ctrl->IsConnected())
	{
		return XBOXButtons::XBOX_UNKNOWN;
	}

	if (power < threshold)
	{
		return 0;
	}

	return power;
}

unsigned short mlSceneManager::GetLeftTrigger()
{
	return GetTrigger(m_ctrl->GetState().Gamepad.bLeftTrigger,
					  XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

unsigned short mlSceneManager::GetRightTrigger()
{
	return GetTrigger(m_ctrl->GetState().Gamepad.bRightTrigger,
					  XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

void mlSceneManager::HandleKeyEvent(omsUserEvent &aEvent){
	//if(meMode!=smmNormal) return;
	if(meMode==smmInitilization) return;	// Alex 2007-03-12 Иначе в режиме проигрывания камеру по кнопкам не переместить
	char chEvID=0;
	bool isAlt=false;
	bool isCtrl=false;
	bool isShift=false;
	muKeyCode = aEvent.key_code; // wParam//  
	muKeyStates = aEvent.mousexy.mouse_x; 
	muScanCode = aEvent.mousexy.mouse_y; 
	mpContext->mpApp->TranslateKeyCode(muKeyCode, muKeyStates, muScanCode, muAscii, muUnicode, isAlt, isCtrl, isShift);
	
	if( (aEvent.ev == omsUserEvent::KEYDOWN) && 
		(GetKeyStates() & 0x8) && 
		aEvent.key_code == 84)
	{
		mlTrace(cx, "\nStart internal traceRefs ========= \n");
		if(	mpPreviousScene)
			TraceRefs( cx, NULL, mpPreviousScene);
		mlTrace(cx, "End internal traceRefs ========= \n\n");
	}

	mlPressedKey oKey(muKeyCode, muKeyStates, muScanCode, muAscii, muUnicode, mlTime);
	if(aEvent.ev == omsUserEvent::KEYDOWN || aEvent.ev == omsUserEvent::GAMEPAD_KEYDOWN){
		chEvID = mlRMMLButton::EVID_onKeyDown;
		if(mpButFocused != NULL){
			mpButFocused->GetButton()->onKeyDown();
		}
		// запоминаем в векторе, что пришло событие о нажатии кнопки, чтобы позже отследить, 
		// пришло ли вовремя событие об ее отжатии
		AddPressedKey(oKey);
	}else if(aEvent.ev == omsUserEvent::KEYUP || aEvent.ev == omsUserEvent::GAMEPAD_KEYUP){
		chEvID = mlRMMLButton::EVID_onKeyUp;
		if(mpButFocused != NULL){
			mpButFocused->GetButton()->onKeyUp();
		}
		// удаляем из вектора запись о том, что была нажата клавиша
		/*if (aEvent.ev != omsUserEvent::GAMEPAD_KEYUP)*/
		RemovePressedKey(oKey);
	}
	
	// Установим свойство keyHandled для объекта Event.
	// Им будем обозначать, что нажатая кнопка была обработана в скрипте
	jsval v = BOOLEAN_TO_JSVAL( false);
	JS_SetProperty( cx, GetEventGO(),"keyHandled",&v);

	int iIdx=aEvent.ev-omsUserEvent::KEYDOWN+1;
	if (iIdx > 2)
	{
		iIdx -= 2;
	}
	v_elems* pv=&(maKeyListeners[iIdx]);
	v_elems::const_iterator viBegin = pv->begin();
	v_elems::const_iterator viEnd = pv->end();
	v_elems::const_iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		mlRMMLElement* pMLEl=*vi;
		pMLEl->CallListeners(chEvID);
		// если во время выполнения обработчика изменился список слушателей,
		// то прерываем цикл (но по идее правильнее сделать как в mlRMMLElement::CallListeners)
		if(pv->begin() != viBegin || pv->end() != viEnd)
			break;
	}
	pv=&(maKeyListeners[0]);
	viBegin = pv->begin();
	viEnd = pv->end();
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		mlRMMLElement* pMLEl=*vi;
		pMLEl->CallListeners(chEvID,true);
		// если во время выполнения обработчика изменился список слушателей,
		// то прерываем цикл (но по идее правильнее сделать как в mlRMMLElement::CallListeners)
		if(pv->begin() != viBegin || pv->end() != viEnd)
			break;
	}

	v = JSVAL_NULL;
	JS_GetProperty(cx, GetEventGO(), "keyHandled", &v);
	if(JSVAL_IS_BOOLEAN(v))
	{
		bool keyHandled = JSVAL_TO_BOOLEAN( v);
		if( !keyHandled)
		{
			//begin  whiteboard и управление шарингом
			if (mpContext->mpRM && mpContext->mpRM->GetMouseController())		
			{
				if( chEvID == mlRMMLButton::EVID_onKeyDown)
					mpContext->mpRM->GetMouseController()->OnKeyDown(muUnicode, muKeyCode, isAlt, isCtrl, isShift);
				else if( chEvID == mlRMMLButton::EVID_onKeyUp)
					mpContext->mpRM->GetMouseController()->OnKeyUp(muUnicode, isAlt, isCtrl, isShift);
			}
			//end  whiteboard и управление шарингом
		}
	}

	JS_DeleteProperty( cx, GetEventGO(), "keyHandled");
}

void mlSceneManager::HandleScriptEvent(omsUserEvent &aEvent){
	jsval v;
	jschar* jscSctipt=aEvent.script;
	if(jscSctipt==NULL) return;
	unsigned int uLength=aEvent.key_code;
	JSBool bR=JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), jscSctipt, uLength, "execScript", 1, &v);
}

void mlSceneManager::HandleCustomEvent(oms::omsUserEvent &aEvent){
	mlSynchData oData(aEvent.script,aEvent.key_code);
	unsigned short usEventID;
	oData >> usEventID;
	switch(usEventID){
	case CEID_serverQueryComplete:
		mServerCallbackManager->onQueryCompleteInternal(oData);
		break;
	case CEID_mapTeleported:
		// Исправление #414
		mlLastOldSyncElementsTime = mpContext->mpApp->GetTimeStamp();
		mlLastOldAvatarsTime = mlLastOldSyncElementsTime;
		// Конец #414
		mMapCallbackManager->onTeleportedInternal(oData);
		break;
	case CEID_mapTeleportError:
		mMapCallbackManager->onTeleportErrorInternal(oData);
		break;
	case CEID_mapURLChanged:
		mMapCallbackManager->onURLChangedInternal(oData);
		break;
	case CEID_mapShowZone:
		mMapCallbackManager->onShowZoneInternal(oData);
		break;
	case CEID_syncDeleteObject:
	{
		unsigned int uObjID;
		unsigned int uBornRealityID;
		oData >> uObjID;
		oData >> uBornRealityID;
		DeleteObjectInternal( uObjID, uBornRealityID);
	}	break;
	case CEID_syncHandleMessage:
		HandleSyncMessageInternal(oData);
		break;
	case CEID_syncOnReceivedRecordsInfo:
		mSyncCallbackManager->onReceivedRecordsInfoInternal(oData);
		break;
	case CEID_syncOnReceivedCreatedSeanceInfo:
		mSyncCallbackManager->onReceivedSeanceInfoInternal(oData);
		break;
	case CEID_syncOnReceivedSeancesInfo:
		mSyncCallbackManager->onReceivedSeancesInfoInternal(oData);
		break;
	case CEID_mapObjectCreated:
		mMapCallbackManager->onObjectCreatedInternal(oData);
		break;
	case CEID_syncOnObjectStatusReceived:
		mSyncCallbackManager->onReceivedObjectStatusInternal(oData);
		break;
	case CEID_mapObjectStatusReceived:
		mMapCallbackManager->onReceivedObjectStatusInernal(oData);
		break;
	case CEID_syncOnReceivedFullSeanceInfo:
		mSyncCallbackManager->OnReceivedFullSeanceInfoInternal(oData);
		break;
	case CEID_serviceOnGetService:
		mServiceCallbackManager->onGetServiceInternal(oData);
		break;
	case CEID_serviceOnReceivedMethodResult:
		mServiceCallbackManager->onReceivedMethodResultInternal(oData);
		break;
	case CEID_serviceOnNotify:
		mServiceCallbackManager->onNotify(oData);
		break;
	case CEID_serviceOnConnectRestored:
		mServiceCallbackManager->onConnectRestoredInternal(oData);
		break;
	case CEID_syncOnFreezeSeance:
		miUpdateNumber = 0;
		// FreezeAllObjects();	// Alex(PauseLevel)
		onSeancePaused();
		break;
	case CEID_syncOnJoinedToSeance:
		assert(false);
		/* Теперь все работает через onEventEntered
		unsigned int uRecordID, uSeanceID;
		int iMinutes;
		unsigned char isAuthor;
		unsigned char isSuspended;
		oData >> uRecordID;
		oData >> uSeanceID;
		oData >> isAuthor;
		oData >> isSuspended;
		oData >> iMinutes;
		mSyncCallbackManager->OnSeanceLoadedInternal(uSeanceID, uRecordID, isAuthor, isSuspended, iMinutes);
		mVoipCallbackManager->onEventEntered(uRecordID, 0);*/
		break;
	case CEID_syncOnUnfreezeSeance:
		//UnFreezeAllObjects();	// Alex(PauseLevel)
		break;
	case CEID_syncRestoreSynchronization:
		{
			wchar_t* apwcRequestedUrl = NULL;
			oData >> apwcRequestedUrl;
			RestoreSynchronizationInternal( apwcRequestedUrl);
		}	break;
	case CEID_voipOnConnectSuccess:
		mVoipCallbackManager->onConnectSuccessInternal();
		break;
	case CEID_voipOnConnectStart:
		mVoipCallbackManager->onConnectStartInternal();
		break;
	case CEID_voipOnConnectFailed:
		mVoipCallbackManager->onConnectFailedInternal( oData);
		break;
	case CEID_voipOnConnectLost:
		mVoipCallbackManager->onConnectionLostInternal();
		break;
	case CEID_voipOnDisconnected:
		mVoipCallbackManager->onDisconnectedInternal();
		break;
	case CEID_voipOnKicked:
		mVoipCallbackManager->onKicked();
		break;
	case CEID_voipOnAddRoom:
		mVoipCallbackManager->onAddRoomInternal(oData);
		break;
	case CEID_voipOnRemoveRoom:
		mVoipCallbackManager->onRemoveRoomInternal(oData);
		break;
	case CEID_voipOnUserTalking:
		mVoipCallbackManager->onUserTalkingInternal(oData);
		break;
	case CEID_voipOnUserStoppedTalking:
		mVoipCallbackManager->onUserStoppedTalkingInternal(oData);
		break;
	case CEID_voipOnUserFileSaved:
		mVoipCallbackManager->onUserFileSavedInternal(oData);
		break;
	case CEID_voipOnAudioFilePlayed:
		mVoipCallbackManager->onAudioFilePlayedInternal(oData);
		break;
	case CEID_voipOnRecordingChanged:
		mVoipCallbackManager->onRecordingChangedInternal(oData);
		break;
	case CEID_voipOnEvoClientWorkStateChanged:
		mVoipCallbackManager->onEvoClientWorkStateChangedInternal(oData);
		break;
	case CEID_voipOnSRModeStatusChanged:
		mVoipCallbackManager->onSRModeStatusChangedInternal(oData);
		break;
	case CEID_voipOnNotified:
		mVoipCallbackManager->onNotifiedInternal(oData);
		break;
	case CEID_voipOnSetInputDevice:
		mVoipCallbackManager->onSetInputDeviceInternal(oData);
		break;
	case CEID_voipOnSetOutputDevice:
		mVoipCallbackManager->onSetOutputDeviceInternal(oData);
		break;
	case CEID_voipOnGetInputDevices:
		mVoipCallbackManager->onGetInputDevicesInternal(oData);
		break;
	case CEID_voipOnGetOutputDevices:
		mVoipCallbackManager->onGetOutputDevicesInternal(oData);
		break;
	case CEID_voipOnDeviceErrorCode:
		mVoipCallbackManager->onDeviceErrorCodeInternal(oData);
		break;
	case CEID_voipOnReInitializeVoipSystem:
		mVoipCallbackManager->onReInitializeVoipSystemInternal();
		break;
	case CEID_voipOnRecievedFindedMicName:
		mVoipCallbackManager->OnRecievedFindedMicNameInternal(oData);
		break;
	case CEID_ConnectRecordInLocation:
		mWorldEventManager->onRecordConnectedInternal(oData);
		break;
	case CEID_desktopOnApplyFrame:
		mDesktopCallbackManager->onApplyFrameRequestInternal();
		break;
	case CEID_desktopOnSharingFileSaved:
		mDesktopCallbackManager->onSharingServerFileSavedInternal(oData);
		break;
	case CEID_desktopOnDestinationRequest:
		mDesktopCallbackManager->onDesktopDestinationRequestInternal(oData);
		break;
	case CEID_desktopOnRaiseSessionStartedEvent:
		mDesktopCallbackManager->onRaiseSessionStartedEventInternal(oData);
		break;
	case CEID_desktopOnRaisePreviewCancelEvent:
		mDesktopCallbackManager->onRaisePreviewCancelEventInternal(oData);
		break;
	case CEID_desktopOnRaisePreviewOKEvent:
		mDesktopCallbackManager->onRaisePreviewOKEventInternal(oData);
		break;
	case CEID_desktopOnRaiseViewSessionStartedEvent:
		mDesktopCallbackManager->onRaiseViewSessionStartedEventInternal(oData);
		break;
	case CEID_desktopOnTextureSizeChangedEvent :
		mDesktopCallbackManager->onTextureSizeChangedEventInternal(oData);
		break;
	case CEID_desktopOnSessionClosedEvent:
		mDesktopCallbackManager->onSessionClosedEventInternal(oData);
		break;
	case CEID_desktopOnViewSessionClosedEvent:
		mDesktopCallbackManager->onViewSessionClosedEventInternal(oData);
		break;
	case CEID_desktopOnWaitOwnerEvent:
		mDesktopCallbackManager->onWaitOwnerEventInternal(oData);
		break;
	case CEID_desktopOnRemoteAccessModeChangedEvent:
		mDesktopCallbackManager->onRemoteAccessModeChangedEventInternal(oData);
		break;
	case CEID_desktopOnSharingEventByCode:
		mDesktopCallbackManager->onSharingEventByCodeInternal(oData);
		break;
	case CEID_desktopOnGlobalMouseEvent:
		mDesktopCallbackManager->onRaiseGlobalMouseEventInternal(oData);
		break;
	case CEID_desktopOnVideoInputDeviceInitializedEvent:
		mDesktopCallbackManager->onVideoInputDeviceInitializedInternal(oData);
		break;
	case CEID_desktopOnSetBarPositionEvent:
		mDesktopCallbackManager->onSetBarPositionEventInternal(oData);
		break;
	case CEID_desktopOnVideoEndEvent:
		mDesktopCallbackManager->onVideoEndEventInternal(oData);
		break;		
	case CEID_RecordStartFailed:
		mWorldEventManager->onRecordStartFailedInternal(oData);
		break;
	case CEID_DisconnectRecordInLocation:
		mWorldEventManager->onRecordDisconnectInternal(oData);
		break;
	case CEID_StopRecordInLocation:
		mWorldEventManager->onRecordStopedInternal(oData);
		break;
	case CEID_eventEntered:
		mWorldEventManager->onEventEnteredInternal(oData);
		break;
	case CEID_eventLeaved:
		mWorldEventManager->onEventLeavedInternal(oData);
		break;
	case CEID_locationEntered:
		mWorldEventManager->onLocationEnteredInternal(oData);
		break;
	case CEID_eventUpdateInfo:
		mWorldEventManager->onEventUpdateInfoInternal(oData);
		break;
	case CEID_SeanceSeekPos:
		mSyncCallbackManager->OnSeanceSeekPosInternal(oData);
		break;
	case CEID_UpdatePlayingTime:
		mSyncCallbackManager->OnUpdatePlayingTimeInternal(oData);
		break;
	case CEID_syncOnSeancePlayed:
		onSeancePlayed();
		break;
	case CEID_syncOnSeanceRewinded:
		onSeanceRewinded();
		break;
	case CEID_syncOnSeanceFinishedPlaying:
		onSeanceFinishedPlaying();
		break;
	case CEID_serverOnConnected:
		OnServerConnectedInternal(oData);
		break;
	case CEID_serverOnConnectionFailed:
		OnServerConnectionErrorInternal(oData);
		break;
	case CEID_serverOnDisconnected:
		OnServerDisconnectedInternal(oData);
		break;
	case CEID_syncObjectInServerNotFound:
		OnObjectInServerNotFound(oData);
		break;
	case CEID_syncOnSeanceLeaved:
		onSeanceLeaved();
		break;

//	case CEID_syncOnServerError:
//		onSyncServerErrorInternal(oData);
//		break;
	case CEID_moduleOnOpened:
	case CEID_moduleOnCreated:
	case CEID_moduleOnError:
		mlRMMLModule::OnModuleEventInternal(cx, oData, usEventID);
		break;

	case CEID_setModuleToLoadAndLoadScene:
		LoadModuleMetaAndLoadSceneInternal(oData);
		break;
	case CEID_onPreciseURL:
		OnPreciseURLInternal(oData);
		break;
	case CEID_onShowInfoState:
		OnShowInfoStateInternal(oData);
		break;
	case CEID_onVisibilityDistanceKoefChanged:
		OnVisibilityDistanceKoefChangedInternal(oData);
		break;
	case CEID_onLogout:
		OnLogoutInternal(oData);
		break;
	case CEID_playerFileDroped:
		OnWindowFileDropInternal(oData);
		break;
	case CEID_ResourceLoadedAsynchronously:{
		mlRMMLElement* pMLEl = LoadAndCheckInternalPath(oData);
		if(pMLEl != NULL && pMLEl->GetLoadable() != NULL){
			pMLEl->GetLoadable()->ResourceLoadedAsynchronously(oData);
		}
		}break;
	case CEID_onNotifyServerTOInfo:
		OnNotifyServerTOInfoInternal(oData);
		break;
	case CEID_onSeancesInfo:
		OnSeancesInfoReceivedInternal(oData);
		break;
	case CEID_onEventsInfoIntoLocation:
		OnEventsInfoIntoLocationReceivedInternal(oData);
		break;
	case CEID_onProxyAuthError:
		OnProxyAuthErrorInternal(oData);
		break;
	case CEID_onConnectToServerError:
		OnConnectToServerErrorInternal(oData);
		break;
	case CEID_onEventUserRightsInfo:
		mWorldEventManager->onEventUserRightsInfoReceivedInternal(oData);
		break;
	case CEID_onEventUserRightUpdate:
		mWorldEventManager->onEventUserRightUpdateReceivedInternal(oData);
		break;
	case CEID_onEventUserBanUpdate:
		mWorldEventManager->onEventUserBanUpdateReceivedInternal(oData);
		break;
	case CEID_onUserSleep:
		mMapCallbackManager->OnUserSleepInternal();
		break;
	case CEID_onVideoCaptureError:
		OnVideoCaptureErrorInternal(oData);
		break;
	case CEID_onCodeEditorClosed:
		OnCodeEditorClosedInternal(oData);
		break;
	case CEID_onUserWakeUp:
		mMapCallbackManager->OnUserWakeUpInternal();
		break;
	case CEID_onKinectFound:
		OnKinectFoundInternal();
		break;
	case CEID_onOculusRiftFound:
		OnOculusRiftFoundInternal();
		break;
	case CEID_onWindowMoved:
		OnWindowMovedInternal();
		break;
	case CEID_onWindowSizeChangedExternally:
		OnWindowSizeChangedInternal(oData);
		break;
	case CEID_onCutOperationComplete:
		OnCutOperationCompleteInternal(oData);
		break;
	case CEID_onCopyOperationComplete:
		OnCopyOperationCompleteInternal(oData);
		break;
	case CEID_onPasteOperationComplete:
		OnPasteOperationCompleteInternal(oData);
		break;
	case CEID_onDeleteOperationComplete:
		OnDeleteOperationCompleteInternal(oData);
		break;
	case CEID_onUndoOperationComplete:
		OnUndoOperationCompleteInternal(oData);
		break;
	case CEID_onRedoOperationComplete:
		OnRedoOperationCompleteInternal(oData);
		break;
	case CEID_onSaveOperationComplete:
		OnSaveOperationCompleteInternal(oData);
		break;
	case CEID_onSaveAsOperationComplete:
		OnSaveAsOperationCompleteInternal(oData);
		break;
	case CEID_onPlayOperationComplete:
		OnPlayOperationCompleteInternal(oData);
		break;
	case CEID_onPauseOperationComplete:
		OnPauseOperationCompleteInternal(oData);
		break;
	case CEID_onRewindOperationComplete:
		OnRewindOperationCompleteInternal(oData);
		break;
	case CEID_onSeekOperationComplete:
		OnSeekOperationCompleteInternal(oData);
		break;
	case CEID_onOpenOperationComplete:
		OnOpenOperationCompleteInternal(oData);
		break;
	case CEID_serverIPadSessionChangeSlide:
		OnIPadSessionChangeSlideInternal(oData);
		break;
	case CEID_onLoadStateChanged:
		OnLoadStateChanged(oData);
		break;
	case CEID_syncOnInfoMessage:
		OnInfoMessageInternal(oData);
		break;
	case CEID_updaterKey:
		OnUpdaterKeyByThreadInternal(oData);
		break;
	case CEID_onVersionReceived:
		OnVersionReceivedInternal(oData);
		break;		
	}
}

void mlSceneManager::OnKinectFoundInternal()
{
	NotifyPlayerListeners(EVID_onKinectFound);
}

void mlSceneManager::OnOculusRiftFoundInternal()
{
	NotifyPlayerListeners(EVID_onOculusRiftFound);
}

void mlSceneManager::OnWindowMovedInternal()
{
	NotifyPlayerListeners(EVID_onWindowMoved);
}

void mlSceneManager::UnFreezeAllObjects()
{
	mbObjectsFrozen = false;
	if (mpMyAvatar == NULL)
		return;

	// Берем 3D сцену 
	mlRMMLElement* p3DScene = mpMyAvatar->mpParent;
	if (p3DScene == NULL)
		return;

	while (p3DScene !=NULL)
	{
		if (p3DScene->mRMMLType == MLMT_SCENE3D)
			break;
		p3DScene = p3DScene->mpParent;
	}

	mlRMMLIterator Iter(cx, p3DScene, true, false);
	mlRMMLElement* pElem = NULL;
	while((pElem = Iter.GetNext()) != NULL)
	{
		if (pElem->GetFrozen())
			pElem->Unfreeze();
	}
mlTrace(cx,"UnFreezeAllObjects \n");	
}

void mlSceneManager::FreezeElementIfNeed(mlRMMLElement* pElem)
{
	if (pElem->GetBornRealityID() != mMyAvatarBornRealityID)
		pElem->Freeze();
}

void mlSceneManager::UnfreezeElement(mlRMMLElement* pElem)
{
	if (pElem->GetBornRealityID() != mMyAvatarBornRealityID)
		pElem->Unfreeze();
	mlTrace(cx,"UnfreezeElement:  objectName=%s \n ",cLPCSTR(pElem->GetName()));	
}

void mlSceneManager::FreezeAllObjects()
{
	mbObjectsFrozen = true;

	if (mpMyAvatar == NULL)
		return;

	// Берем 3D сцену 
	mlRMMLElement* p3DScene = mpMyAvatar->mpParent;
	if (p3DScene == NULL)
		return;
	while (p3DScene != NULL)
	{
		if (p3DScene->mRMMLType == MLMT_SCENE3D)
			break;
		p3DScene = p3DScene->mpParent;
	}
	
	mlRMMLIterator Iter(cx, p3DScene, true, false);
	mlRMMLElement* pElem = NULL;
	while((pElem = Iter.GetNext()) != NULL)
	{
		if (pElem->GetBornRealityID() != mMyAvatarBornRealityID)
		{
			if (!pElem->GetFrozen())
				pElem->Freeze();
		}
	}
	mlTrace(cx,"FreezeAllObjects \n");	
}

void mlSceneManager::OnLoadStateChanged(mlSynchData& aData){
	mlRMMLElement* pMLEl = LoadAndCheckInternalPath(aData);
	if(pMLEl==NULL)
		return;

	pMLEl->OnLoadStateChanged();
}

void mlSceneManager::HandleInternalEvent(omsUserEvent &aEvent){
//	mlRMMLElement* *paPath=(mlRMMLElement**)aEvent.script;
//	unsigned int uLength=aEvent.key_code/sizeof(mlRMMLElement*);
	mlSynchData Data(aEvent.script,aEvent.key_code);
	Data.setJSContext(cx);
	//
	if(meMode == smmInitilization) SceneStructChanged();
	mlRMMLElement* pMLEl = LoadAndCheckInternalPath(Data);
	if(pMLEl==NULL)	{return;	}
	char chEventCode=aEvent.ev & 0xFF;
	if(chEventCode == -1){ // call function (ActiveX)
		pMLEl->CallFunction(Data);
		return;
	}
	bool bDontSendSynchEvents;
	Data >> bDontSendSynchEvents;
	if(Data.getPos()<Data.size())
		pMLEl->SetEventData(chEventCode & 0x7F, Data);
	mlDontSendSynchEvents oDSSE(this);
//	if(!pMLEl->NeedToSynchronize())
		GPSM(cx)->mbDontSendSynchEvents = bDontSendSynchEvents;
#ifdef MLD_TRACE_INIT_EVENTS
if(GPSM(cx)->GetMode()==smmInitilization){
mlTrace(cx, "---SynchInitInfo--- Handling %s.%s\n", 
		cLPCSTR(pMLEl->GetStringID().c_str()), cLPCSTR(pMLEl->GetEventName(chEventCode & 0x7F))
);
}
#endif
	pMLEl->CallListeners(chEventCode & 0x7F,false,chEventCode & 0x80);
	ResetEvent();	// 2007-03-02 Alex
}

void mlSceneManager::SaveInternalPath(mlSynchData &Data,mlRMMLElement* apMLEl){
	std::vector<mlRMMLElement*> vChildPath;
	mlRMMLElement* pMLEl=apMLEl;
	while(pMLEl!=NULL){
		vChildPath.insert(vChildPath.begin(),pMLEl);
		pMLEl=pMLEl->mpParent;
	}
	Data << (unsigned short)vChildPath.size();
	Data << (unsigned int)apMLEl->mCountId;
	if(vChildPath.size() > 0){
#if _MSC_VER >= 1600
		Data.put( vChildPath.begin()._Ptr,vChildPath.size()*sizeof(mlRMMLElement*));
#elif _MSC_VER > 1200
		Data.put( vChildPath.begin()._Myptr,vChildPath.size()*sizeof(mlRMMLElement*));
#else
		Data.put(vChildPath.begin(),vChildPath.size()*sizeof(mlRMMLElement*));
#endif
	}
	vChildPath.clear();
}

mlRMMLElement* mlSceneManager::LoadAndCheckInternalPath(mlSynchData &Data){
	unsigned short uLength;
	unsigned int uiCount;
	Data >> uLength;
	Data >> uiCount;
	mlRMMLElement* *paPath = (mlRMMLElement**)Data.getDataInPos();
	Data.skip(uLength*sizeof(mlRMMLElement*));
	mlRMMLElement* pCurEl = paPath[uLength-1];
	if(mvDeletedInUpdate.size() > 0){
		std::vector<unsigned int>::const_iterator cvi = mvDeletedInUpdate.begin();
		for(; cvi != mvDeletedInUpdate.end(); cvi++){
			if(*cvi == uiCount)
				return NULL;
		}
	}
	if(IsSceneStructChanged()){
		// проверяем наличие элемента
		if(mpScene != *paPath) return NULL;
		pCurEl=mpScene;
		paPath++; uLength--;
		for(; uLength > 0; uLength--){
			if(!pCurEl->IsChild(*paPath)) return NULL;
			pCurEl=*paPath;
			paPath++;
		}
	}
	return paPath[uLength-1];
}

void mlSceneManager::RemoveEventListener(mlRMMLElement* apMLEl){
	// удалить из слушателей мышиных событий
	removeMouseListener(-1,apMLEl);
	// удалить из слушателей клавиатурных событий
	removeKeyListener(-1,apMLEl);
	// удалить из слушателей событий других глобальных объектов
	RemoveEventListener(-1,apMLEl);
	// удалить из слушателей URL событий
	removeUrlListener(-1,apMLEl);
	// удалить из слушателей Voice событий
	removeVoiceListener(-1,apMLEl);
	// удалить из слушателей Recorder событий
	removeRecorderListener(-1,apMLEl);
	// удалить из слушателей серверных событий
	removeServerListener(-1,apMLEl);
	// удалить из слушателей событий редактора записей
	removeRecordEditorListener(-1,apMLEl);
}

void mlSceneManager::RemoveEventListener(char aidSender,mlRMMLElement* apMLEl){
	if(aidSender<0 || aidSender==GOID_Player || aidSender==GOID_Module || aidSender==GOID_Map
		 || aidSender==GOID_Voice || aidSender==GOID_Url || aidSender==GOID_Recoder
		 || aidSender==GOID_Server || aidSender==GOID_RecordEditor || aidSender==GOID_VideoCapture || aidSender==GOID_CodeEditor){
		removePlayerListener(-1,apMLEl);
		removeModuleListener(-1,apMLEl);
		removeMapListener(-1,apMLEl);
		removeVoiceListener(-1,apMLEl);
		removeUrlListener(-1,apMLEl);
		removeRecorderListener(-1,apMLEl);
		removeServerListener(-1,apMLEl);
		removeRecordEditorListener(-1,apMLEl);
		removeVideoCaptureListener(-1,apMLEl);
		removeCodeEditorListener(-1,apMLEl);
	}
}

/**
 * Реализация синхронизации
 */ 

struct mlJSStackFrame {
    JSObject        *callobj;       /* lazily created Call object */
    JSObject        *argsobj;       /* lazily created arguments object */
    JSObject        *varobj;        /* variables object, where vars go */
    JSScript        *script;        /* script being interpreted */
    JSFunction      *fun;           /* function being called or null */
    JSObject        *thisp;         /* "this" pointer if in method */
    uintN           argc;           /* actual argument count */
    jsval           *argv;          /* base of argument stack slots */
    jsval           rval;           /* function return value */
    uintN           nvars;          /* local variable count */
    jsval           *vars;          /* base of variable stack slots */
    JSStackFrame    *down;          /* previous frame */
    void            *annotation;    /* used by Java security */
    JSObject        *scopeChain;    /* scope chain */
    jsbytecode      *pc;            /* program counter */
    jsval           *sp;            /* stack pointer */
    jsval           *spbase;        /* operand stack base */
    uintN           sharpDepth;     /* array/object initializer depth */
    JSObject        *sharpArray;    /* scope for #n= initializer vars */
    uint32          flags;          /* frame flags -- see below */
    JSStackFrame    *dormantNext;   /* next dormant frame chain */
    JSAtomMap       *objAtomMap;    /* object atom map, non-null only if we
                                       hit a regexp object literal */
};

struct mlJSScript {
    jsbytecode   *code;         /* bytecodes and their immediate operands */
    uint32       length;        /* length of code vector */
    jsbytecode   *main;         /* main entry point, after predef'ing prolog */
    JSVersion    version;       /* JS version under which script was compiled */
};
/*    JSAtomMap    atomMap;       // maps immediate index to literal struct
    const char   *filename;     // source filename or null
    uintN        lineno;        // base line number of script
    uintN        depth;         // maximum stack depth in slots
    JSTryNote    *trynotes;     // exception table for this script
    JSPrincipals *principals;   // principals for this script
    JSObject     *object;       // optional Script-class object wrapper
};*/

void* mlSceneManager::SynchJSHook(JSContext *cx, JSStackFrame *fp, JSBool before,
						JSBool *ok, void *closure){

	mlEModes mode = GPSM(cx)->GetMode();
	if(((mlJSStackFrame*)fp)->rval == DONT_IMPL_NM_RVAL) // на всякий случай
		((mlJSStackFrame*)fp)->rval = JSVAL_VOID; 
	if(mode == smmNormal){
//		if(GPSM(cx)->mbDontSendSynchEvents) return NULL; // 1
	}else if(mode == smmInitilization){
		if(!(GPSM(cx)->mbDontSendSynchEvents)) return NULL; // 0 или onRestoreUnsynched
	}else
		return NULL;
	if(before==JS_FALSE){
		if(closure!=NULL){
			uint32 length=(uint32)closure;
			mlJSStackFrame* pf=(mlJSStackFrame*)fp;
			mlJSScript* script=(mlJSScript*)pf->script;
			if(script!=NULL)
				script->length=length;
		}
		return NULL;
	}
	JSFunction* fun=JS_GetFrameFunction(cx, fp);
	if(fun==NULL) return NULL;
	JSObject* funobj=JS_GetFrameFunctionObject(cx, fp);
	//
	mlRMMLElement* pMLEl=NULL;
	if(pMLEl==NULL){
		// возможно это свойство мультимедиа-элемента
		JSObject* objThis=JS_GetFrameThis(cx, fp);
		if(objThis!=NULL){
			JSClass* pClass=ML_JS_GETCLASS(cx, objThis);
			if(pClass!=NULL && 
			   (pClass->flags & JSCLASS_HAS_PRIVATE) && 
			   JSO_IS_MLEL(cx, objThis)){
				void* pPriv=JS_GetPrivate(cx,objThis);
				pMLEl=(mlRMMLElement*)pPriv;
			}
		}
	}
	//
	if(pMLEl==NULL){
		// возможно это свойство композиции
		JSObject* objFunParent=JS_GetParent(cx, funobj);
		if(objFunParent!=NULL){
			JSClass* pClass=ML_JS_GETCLASS(cx, objFunParent);
			if(pClass!=NULL && 
			   (pClass->flags & JSCLASS_HAS_PRIVATE) &&
			   JSO_IS_MLEL(cx, objFunParent)
			){
				void* pPriv=JS_GetPrivate(cx,objFunParent);
				pMLEl=(mlRMMLElement*)pPriv;
			}
		}
	}
	if(pMLEl == NULL || GPSM(cx)->GetContext()->mpSynch==NULL)
		return NULL;
	// в 3-м режиме синхронизации выполняем все без синхронизации
	if(pMLEl->mchSynchronized == 3)
		return NULL;
/*JSString* jssFun = JS_GetFunctionId(fun);
if( jssFun)
{jschar* jscFun = JS_GetStringChars(jssFun);
if(isEqual(jscFun, L"updateAvatarList"))
int hh=9;}*/
	if(mode == smmNormal){
		if(GPSM(cx)->mbDontSendSynchEvents){
			// вызов из синхронизируемой области в несинхронизируемую (1 -> 0)
			// в нормальном режиме делаем, но сбрасываем флаг mbDontSendSynchEvents
			if(!pMLEl->NeedToSynchronize()){ 
				GPSM(cx)->mbDontSendSynchEvents = false;
			}else
				return NULL; // остались в синхронизируемой области (1 -> 1)
		}
	}else if(mode == smmInitilization){
		// вызов из синхронизируемой области в несинхронизируемую (1 -> 0)
		// в режиме инициализации не делаем
			// если это активируются скрипты элементов UI, то выполнять их
		if(GPSM(cx)->IsUIScriptIsActivating())
			return NULL;
		if(!pMLEl->NeedToSynchronize()){ 
			mlJSStackFrame* pf=(mlJSStackFrame*)fp;
			mlJSScript* script=(mlJSScript*)pf->script;
			uint32 length=0;
			if(script!=NULL){
				length=script->length;
				script->length=0;
			}else
				pf->rval = DONT_IMPL_NM_RVAL;
			return (void*)length;
		}
		return NULL;
	}
	// normal mode
	if(pMLEl->NeedToSynchronize()){ 
		// вызов из несинхронизируемой области в синхронизируемую (0 -> 1)
		// в нормальном режиме синхронизируем

		// если 2-й режим синхронизации объекта (частичная синхронизация)
		if(pMLEl->mchSynchronized == 2){
			mlSynchData oData;
			oData << SNCMAG_SERIES; // 0x7E
			int iNObjsPos=oData.getPos();
			unsigned char iNObjs=0;
			oData << iNObjs;
			/// сохраняем значения свойств объекта
			// получаем значение свойства метода synch
			JSString* jss = JS_GetFunctionId(fun);
			if(jss == NULL) return NULL;
			jschar* jsc = JS_GetStringChars(jss);
//if(isEqual(jsc, L"register"))
//int hh=9;
			jsval vFun = JSVAL_NULL;
			JS_GetUCProperty(cx, pMLEl->mjso, jsc, -1, &vFun);
			ML_ASSERTION(cx, vFun != JSVAL_NULL, "mlSceneManager::SynchJSHook");
			jsval vSynch = JSVAL_NULL;
			JS_GetUCProperty(cx, JSVAL_TO_OBJECT(vFun), (const jschar*)L"synch", -1, &vSynch);
//JSType jst = JS_TypeOfValue(cx, vSynch);
//const char* pTN = JS_GetTypeName(cx, jst);
			if(!JSVAL_IS_STRING(vSynch))
				return NULL; // не синхронизируемая функция
			JSString* jssSynch = JSVAL_TO_STRING(vSynch);
			mlString sSynch = (const wchar_t*)JS_GetStringChars(jssSynch);
			// перебираем все свойства и сохраняем их значения
			pMLEl->PutPropsForSynch(oData, sSynch.c_str(), iNObjs);
			/// сохраняем данные метода
			oData << SNCOT_FUN;
			// имя функции
			oData << jsc; 
			iNObjs++;
			// сохраняем аргументы функции
			mlJSStackFrame* pf=(mlJSStackFrame*)fp;
			unsigned int uArgc=pf->argc;
			oData << uArgc;
			for(unsigned int ij=0; ij<pf->argc; ij++){
				jsval vProp=pf->argv[ij];
				pMLEl->PutJSValForSynch(cx, vProp, oData);
				iNObjs++;
			}
			//
			oData.setPos(iNObjsPos); 
			oData << iNObjs;
			oData.setPos(); 
			// посылаем запрос на возможность выполнить функцию
			MLWORD wID=pMLEl->GetID();
			if(wID!=0xFFFF){
				mlISinchronizer* pSynch = GPSM(cx)->GetContext()->mpSynch;
				omsresult res=OMS_OK;
				res=pSynch->CallEvent(wID, (unsigned char*)oData.data(), oData.size());
				if(OMS_SUCCEEDED(res)){
					// как-то сказать, что не надо выполнять функцию
					mlJSStackFrame* pf=(mlJSStackFrame*)fp;
					mlJSScript* script=(mlJSScript*)pf->script;
					uint32 length = 0;
					if(script!=NULL){
						length = script->length;
						script->length = 0;
					}else{
						pf->rval = DONT_IMPL_NM_RVAL; // native object method
					}
					return (void*)length;
				}
			}
			return NULL;
		}

		// сохраняем данные для синхронизации
		mlSynchData oData;
		oData << (char)0x7F;
		// сохраняем имя функции
		JSString* jss=JS_GetFunctionId(fun);
		jschar* jsc=JS_GetStringChars(jss);
		oData << jsc;
		// сохраняем аргументы функции
		mlJSStackFrame* pf=(mlJSStackFrame*)fp;
		unsigned int uArgc=pf->argc;
		oData << uArgc;
		for(unsigned int ij=0; ij<pf->argc; ij++){
			jsval vProp=pf->argv[ij];
			if(JSVAL_IS_NULL(vProp)){
				oData << (char)0;
			}else if(JSVAL_IS_INT(vProp)){
				oData << (char)1;
				int iArg=JSVAL_TO_INT(vProp);
				oData << iArg;
			}else if(JSVAL_IS_STRING(vProp)){
				oData << (char)2;
				JSString* jssArg=JSVAL_TO_STRING(vProp);
				jschar* jscArg=JS_GetStringChars(jssArg);
				oData << jscArg;
			}else if(JSVAL_IS_DOUBLE(vProp)){
				oData << (char)3;
				jsdouble* pjsdbl=JSVAL_TO_DOUBLE(vProp);
				oData << (*pjsdbl);
			}else if(JSVAL_IS_OBJECT(vProp)){
				oData << (char)4;
				JSObject* jsoArg=JSVAL_TO_OBJECT(vProp);
				mlRMMLElement* pMLElArg=(mlRMMLElement*)JS_GetPrivate(cx,jsoArg);
				if(pMLElArg==NULL){
					ML_ASSERTION(cx, false,"SynchJSHook (unsupported JS-object argument type)");
					return NULL;
				}
				MLWORD wID=pMLElArg->GetID();
				oData << wID;
			}else if(JSVAL_IS_VOID(vProp)){
				oData << (char)5;
			}else{
				ML_ASSERTION(cx, false,"SynchJSHook (unsupported argument type)");
				return NULL;
			}
//			jschar* jscVal=JS_GetStringChars(JS_ValueToString(cx,vProp));
//			int kk=0;
		}
		// сохраняем данные для синхронизации элемента, которому принадлежит функция
		moMedia* pMO=pMLEl->GetMO();
		if(!PMO_IS_NULL(pMO)) pMO->GetSynchData(oData);
		// посылаем запрос на возможность выполнить функцию
		MLWORD wID=pMLEl->GetID();
		if(wID!=0xFFFF){
			mlISinchronizer* pSynch = GPSM(cx)->GetContext()->mpSynch;
			omsresult res=OMS_OK;
			if(GPSM(cx)->meMode!=smmInitilization)
				res=pSynch->CallEvent(wID, (unsigned char*)oData.data(), oData.size());
			if(OMS_SUCCEEDED(res)){
				// как-то сказать, что не надо выполнять функцию
				mlJSScript* script=(mlJSScript*)pf->script;
				uint32 length=0;
				if(script!=NULL){
					length=script->length;
					script->length=0;
				}else
					pf->rval = DONT_IMPL_NM_RVAL;
				return (void*)length;
			}
		}
	}
	return NULL;
}

void mlSceneManager::SetSynchJSHook(){
//	JS_SetCallHook(rt, SynchJSHook, void *closure);
	JS_SetCallHook(gJSRunTime, SynchJSHook, NULL);
	// ??
}

mlRMMLElement* mlSceneManager::FindElemByStringID(const wchar_t* apwcStringID){
	mlRMMLElement* pMLEl=NULL;
	if(mpScene!=NULL){
		const wchar_t* pwcStringID=apwcStringID;
		wchar_t* pwc=(wchar_t*)pwcStringID;
		while(*pwc!=L'\0' && *pwc!=L'.') pwc++;
		wchar_t* pwcSceneName=mpScene->GetName();
		if(!isEqual(pwcSceneName,pwcStringID,pwc-pwcStringID)){
			wchar_t wch=*pwc;
//			*pwc=L'\0';
			// ?? возможно это ссылка на элемент из глобального объекта
			mlTraceError(cx, "Name of scene in the string id (%S) is not match to name of scene (%S)\n",pwcSceneName,pwcStringID);
//			*pwc=wch;
			return NULL;
		}
		if(*pwc==L'\0') return mpScene;
		pwc++;
		pMLEl=mpScene->FindElemByStringID(pwc);
	}
	return pMLEl;
}

// Изменилась локация у аватара. Пошлем ее на сервак
void mlSceneManager::LocationChanged(const wchar_t* apwcLocationName,  unsigned int auiObjectID, unsigned int auiBornRealityID)
{
	mlRMMLElement* pMLEl = scriptedObjectManager.GetObject(auiObjectID, auiBornRealityID);
	if(pMLEl != NULL){
		pMLEl->SetLocationSID( apwcLocationName);
	}
	/*if (mpContext != NULL && mpContext->mpSyncMan != NULL)
		mpContext->mpSyncMan->AvatarChangeLocation(apwcLocationName, auiObjectID, auiBornRealityID);*/
}

// Изменилась зона общения у аватара. Пошлем ее на сервак
void mlSceneManager::CommunicationAreaChanged(const wchar_t* apCommunicationAreaName, unsigned int auiObjectID, unsigned int auiBornRealityID)
{
	mlRMMLElement* pMLEl = scriptedObjectManager.GetObject(auiObjectID, auiBornRealityID);
	if(pMLEl != NULL){
		pMLEl->SetCommunicationAreaSID(apCommunicationAreaName);
	}
	/*if (mpContext != NULL && mpContext->mpSyncMan != NULL)
		mpContext->mpSyncMan->AvatarChangeCommunicationArea(apwcLocationName, auiObjectID, auiBornRealityID);*/
}

/**
 * 
 */ 
void mlSceneManager::SetFocus(mlRMMLElement* apMLEl, bool abDontCheckActive){
	if(mpButFocused == apMLEl) return;
	if(!abDontCheckActive && !mpContext->mpApp->IsActive()){
		if(mFocusedButInternalPath.size() > 0){
			mlRMMLElement* pButFocused = LoadAndCheckInternalPath(mFocusedButInternalPath);
			if(pButFocused != NULL)
				FREE_FOR_GC(cx, JS_GetGlobalObject(cx), pButFocused->mjso);
		}
		mFocusedButInternalPath.clear();
		if(apMLEl != NULL){
			SaveInternalPath(mFocusedButInternalPath, apMLEl);
			SAVE_FROM_GC(cx, JS_GetGlobalObject(cx), apMLEl->mjso);
		}
		return;
	}
	if(mpButFocused != NULL){
		mpButFocused->CallListeners(mlRMMLButton::EVID_onKillFocus);
		if( mpButFocused->GetButton())
			mpButFocused->GetButton()->onKillFocus();	// 2007-02-21 Tandy
	}
	mpButFocused = apMLEl;
	if(mpButFocused != NULL){
		mpButFocused->CallListeners(mlRMMLButton::EVID_onSetFocus);
		if( mpButFocused->GetButton())
			mpButFocused->GetButton()->onSetFocus();	// 2007-02-21 Tandy
	}
}

/**
 * Инициализация глобальных JS-объектов
 */ 
mlresult mlSceneManager::InitGlobalJSObjects(JSContext* cx , JSObject* obj){
	unsigned int uGOPropFlags=JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT;

	JS_DefineProperty(cx, obj, "_global", OBJECT_TO_JSVAL(obj), NULL, NULL, JSPROP_READONLY | JSPROP_PERMANENT);

	// "Classes"
    mjsoClasses = JS_DefineObject(cx, obj, GJSONM_CLASSES, &JSRMMLClassesClass, NULL, uGOPropFlags);

	JSBool bR;
	// "Mouse"
    mjsoMouse = JS_DefineObject(cx, obj, GJSONM_MOUSE, &JSRMMLMouseClass, NULL, uGOPropFlags);
	JS_SetPrivate(cx, mjsoMouse, this);
	bR=JS_DefineProperties(cx, mjsoMouse, _JSMousePropertySpec);
	bR=JS_DefineFunctions(cx, mjsoMouse, _JSMouseFunctionSpec);

	// "Key"
    mjsoKey = JS_DefineObject(cx, obj, GJSONM_KEY, &JSRMMLKeyClass, NULL, uGOPropFlags);
	JS_SetPrivate(cx, mjsoKey, this);
	bR=JS_DefineProperties(cx, mjsoKey, _JSKeyPropertySpec);
	bR=JS_DefineFunctions(cx, mjsoKey, _JSKeyFunctionSpec);

	// "Player"
    mjsoPlayer = JS_DefineObject(cx, obj, GJSONM_PLAYER, &JSRMMLPlayerClass, NULL, uGOPropFlags);
	JS_SetPrivate(cx, mjsoPlayer, this);
	InitPlayerJSObject(cx,mjsoPlayer);

	// "Event"
    mjsoEvent = JS_DefineObject(cx, obj, GJSONM_EVENT, NULL, NULL, uGOPropFlags);

	// "Math3D"
    JSObject* jsoMath3D = JS_DefineObject(cx, obj, "Math3D", &JSRMMLMath3DClass, NULL, uGOPropFlags);
	InitMath3DJSObject(cx, jsoMath3D);

	return ML_OK;
}

/**
 * 
 */ 

// ?? для каждого контекста надо сделать свой mlSceneManager

//std::auto_ptr<mlSceneManager> pSceneManager;

omsresult CreateSceneManager(omsContext* amcx){
#ifdef _DEBUG
//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//_CrtSetBreakAlloc(7925);
#endif
//	pSceneManager=std::auto_ptr<mlSceneManager>(new mlSceneManager(amcx));
//	amcx->mpSM=pSceneManager.get();

	MP_NEW_P( manager, mlSceneManager, amcx);
	amcx->mpSM = manager;
	return OMS_OK;
}

void DestroySceneManager(omsContext* amcx){
//	pSceneManager=std::auto_ptr<mlSceneManager>(NULL);
	if(amcx->mpSM != NULL){
		if(amcx->mpSyncMan != NULL)
			amcx->mpSyncMan->CloseClient();
	    mlSceneManager *pSM = (mlSceneManager*)amcx->mpSM;
		MP_DELETE( pSM);
		amcx->mpSM = NULL;
	}
}

/**
 * 
 */ 

typedef char_traits<char> _Traits;

class cstr_streambuf: public streambuf{
	char* mpszStr;
	char* mpchCurPos;
public:
	cstr_streambuf(const char* apszStr){
		mpszStr=(char*)apszStr;
		mpchCurPos=mpszStr;
	}
	virtual ~cstr_streambuf(){
	}
	//virtual int showmanyc(){
	virtual streamsize showmanyc(){
		return strlen(mpchCurPos);
	}
	virtual int_type underflow(){	// get a character from stream, but don't point past it
		if(*mpchCurPos=='\0') return (_Traits::eof());
		return _Traits::to_int_type(*mpchCurPos);
	}
	virtual int_type uflow(){	// get a character from stream, point past it
		if(*mpchCurPos=='\0') return (_Traits::eof());
		return _Traits::to_int_type(*mpchCurPos++);
	}
};
__forceinline unsigned int mlXMLArchive::Read( unsigned char* aBuffer, unsigned int aBufferSize, unsigned int *aErrorRead){
	return mpStrStreamBuf->sgetn((char*)aBuffer, aBufferSize);
}
__forceinline unsigned long mlXMLArchive::GetSize(){
	return mpStrStreamBuf->in_avail();
}

//class mlStrResource: public res::resIResource
//{
//	cstr_streambuf* mpStrStreamBuf;
//	istream* mpIS;
//public:
//	mlStrResource(const char* apszStr){
//		mpStrStreamBuf = new cstr_streambuf(gpszUIDefault);
//		mpIS = new istream(mpStrStreamBuf);
//	}
//	virtual ~mlStrResource(){
//		 MP_DELETE( mpIS;
//		 MP_DELETE( mpStrStreamBuf;
//	}
//
//// res::resIResource
//public:
//	omsresult GetIStream(istream* &apistrm){
//		apistrm = mpIS;
//		return OMS_OK;
//	}
//	omsresult GetStream(oms::Stream* &apOMSStream){
//		return OMS_ERROR_NOT_IMPLEMENTED;
//	}
//	unsigned int Read( unsigned char* aBuffer, unsigned int aBufferSize){
//		return mpStrStreamBuf->sgetn((char*)aBuffer, aBufferSize);
//	}
//	unsigned int Write( unsigned char* aBuffer, unsigned int aBufferSize){
//		return 0;
//	}
//	bool Seek( long aPos, unsigned char aFrom){
//		return false;
//	}
//	long GetPos(){
//		return 0; //mpStrStreamBuf->
//	}
//	long GetSize(){
//		return mpStrStreamBuf->in_avail();
//	}
//	void Close(){
//		 MP_DELETE( this;
//	}
//	const wchar_t* GetURL(){
//		return L"";
//	}
//};

#define UIDEFAULT_WSTR L"_UIDefault_"

mlresult mlSceneManager::OpenXMLArchive(const wchar_t* apwcSrc, mlXMLArchive* &apXMLArch, bool abCreateGDArchive){
	apXMLArch=NULL;
	mlXMLArchive* pXMLArch = MP_NEW( mlXMLArchive);
	pXMLArch->msSrc=apwcSrc;
	if(isEqual(apwcSrc,UIDEFAULT_WSTR)){
		MP_NEW_V( pXMLArch->mpStrStreamBuf, cstr_streambuf, gpszUIDefault);
		MP_NEW_V( pXMLArch->mpIS, istream, pXMLArch->mpStrStreamBuf);
		// не забыть удалить mpIS, если mpStreamBuf!=NULL
		if(!abCreateGDArchive){
			pXMLArch->mpRes = pXMLArch;
		}
	}else{
		// hep + | RES_FILE_CACHE
		DWORD dwOpenFlags = RES_MEM_CACHE;
		if( wcsstr( apwcSrc, L"ui.xml") != NULL)
		{
			dwOpenFlags = RES_LOCAL;
		}
		if(OMS_FAILED(mpContext->mpResM->OpenResource(apwcSrc,pXMLArch->mpRes, dwOpenFlags))){ // , res::RES_MEM_CACHE
			MP_DELETE( pXMLArch);	
			mlTraceError(cx, "Cannot open XML-file (%S)\n",apwcSrc);
			return ML_ERROR_FAILURE;
		}
	//	{	char* pchSrc=WC2MB(apwcSrc);
	//		pXMLArch->mIFS.open(pchSrc, ios_base::in);
	//		 MP_DELETE( pchSrc;
	//	}
	//	if(pXMLArch->mIFS.fail()) return ML_ERROR_FAILURE;
		pXMLArch->mpRes->GetIStream(pXMLArch->mpIS);
	}
	pXMLArch->miSrcIdx=mSrcFileColl.Add(apwcSrc);
	//CLogValue logValue(
	//	"[RMML.SOURCES]: Source index ", pXMLArch->miSrcIdx, ":", (const char*)cLPCSTR(apwcSrc), ""
	//	);
	//mpContext->mpLogWriter->WriteLnLPCSTR( logValue.GetData());
//mlTrace(cx, "Source index %d: %s\n", pXMLArch->miSrcIdx, cLPCSTR(apwcSrc));

//	mSrcFileModuleIDs.Add(apwcOwnerModuleID);
	if(abCreateGDArchive){
		MP_NEW_V( pXMLArch->mpBuf, mlXMLBuffer, pXMLArch->mpIS);
		MP_NEW_V( pXMLArch->mpAr, CXMLArchiveWithXmlBuffer, pXMLArch->mpBuf);
	}

	//
//	CXMLArchive<mlXMLBuffer> ar((mlXMLBuffer*)pXMLFile.get());
	apXMLArch=pXMLArch;
	return ML_OK;
}

//mlresult mlSceneManager::OpenXMLArchive(const wchar_t* apwcSrc, res::resIResource* &apRes, int &aiSrcIdx){
//	if(OMS_FAILED(mpContext->mpResM->OpenResource(apwcSrc,apRes))){
//		TRACE("Cannot open XML-file (%S)\n",apwcSrc);
//		return ML_ERROR_FAILURE;
//	}
//	aiSrcIdx=mSrcFileColl.Add(apwcSrc);
//	return ML_OK;
//}

mlresult mlSceneManager::OpenXMLArchive2Write(const wchar_t* apwcSrc, mlXMLArchive* &apXMLArch){
	apXMLArch=NULL;
	mlXMLArchive* pXMLArch = MP_NEW( mlXMLArchive);
	pXMLArch->msSrc=apwcSrc;
	if(OMS_FAILED(mpContext->mpResM->OpenToWrite(apwcSrc,pXMLArch->mpWrite))){
		MP_DELETE( pXMLArch);
		mlTraceError(cx, "Cannot open XML-file for writing (%S)\n",apwcSrc);
		return ML_ERROR_FAILURE;
	}

	pXMLArch->mpWrite->GetIOStream(pXMLArch->mpOS);
	char* XMLHdr="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	pXMLArch->mpOS->write(XMLHdr, strlen(XMLHdr));
	MP_NEW_V( pXMLArch->mpBuf, mlXMLBuffer, pXMLArch->mpOS);
	MP_NEW_V2( pXMLArch->mpAr, CXMLArchiveWithXmlBuffer, pXMLArch->mpBuf, CXMLArchiveWithXmlBuffer::eWrite | CXMLArchiveWithXmlBuffer::eFormat);

	//
	apXMLArch=pXMLArch;
	return ML_OK;
}

mlresult mlSceneManager::CloseXMLArchive(mlXMLArchive* &apXMLArch){
	if(apXMLArch==NULL) return ML_FALSE;
	if(apXMLArch->mpAr)
		 MP_DELETE( apXMLArch->mpAr);
	if(apXMLArch->mpBuf)
		 MP_DELETE( apXMLArch->mpBuf);
	if(apXMLArch->mpRes && apXMLArch->mpRes!=apXMLArch)
		apXMLArch->mpRes->Close();
	if(apXMLArch->mpWrite)
		apXMLArch->mpWrite->Close();
	if(apXMLArch->mpStrStreamBuf){
		 MP_DELETE( apXMLArch->mpStrStreamBuf);
		 MP_DELETE( apXMLArch->mpIS);
	}
	MP_DELETE( apXMLArch);
	apXMLArch=NULL;
	return ML_OK;
}

omsresult mlSceneManager::LoadUI(const char* asSrc){ 
	return LoadUI(cLPWCSTR(asSrc));
}
omsresult mlSceneManager::LoadUI(const wchar_t* asSrc){ 
	mlresult res;
	GetContext()->mpApp->SendMessage2Launcher( APP_PERCENT, 75);
	if(ML_FAILED(res = LoadXML(asSrc, true, false)))
		return (omsresult)res;
	GetContext()->mpApp->SendMessage2Launcher( APP_PERCENT, 95);
	return OMS_OK; 
}

mlresult mlSceneManager::LoadInto(const wchar_t* asSrc, mlRMMLElement* apParent){ 
	if(apParent == NULL){
		apParent = mpScene;
		if(apParent == NULL){
			apParent = mpUI;
		}
	}
	return LoadXML(asSrc, false, false, apParent);	
}

mlresult mlSceneManager::LoadXML(const char* asSrc, bool abUI, bool abAsynch, mlRMMLElement* apParent){
	return LoadXML(cLPWCSTR(asSrc), abUI, abAsynch, apParent);
}

mlresult mlSceneManager::LoadXML(const wchar_t* asSrc, bool abUI, bool abAsynch, mlRMMLElement* apParent){
TRACE(cx, "Begin LoadXML (%d)\n", GetContext()->mpApp->GetTimeStamp() - mStartLoadSceneTime);
//unsigned long
	// заглушка
//	mpScene=shared_ptr<mlRMMLScene>(new mlRMMLScene);

//	mbSceneLoaded=false;
	bool bDontSendSynchEventsWas = mbDontSendSynchEvents;
	mbDontSendSynchEvents = true;

	if(!abUI && mpScene==NULL && mpUI==NULL){
		LoadXML(UIDEFAULT_WSTR, true, false);
	}
	if(abUI)
		ResetKey(abUI);
	//ResetKey(abUI);	Alex 2007-04-09
	
	if( asSrc != NULL && wcscmp(asSrc, L"scene.xml") == 0)
	{
		mlSynchData oData;
		oData << 1;
		GPSM(cx)->GetContext()->mpInput->AddCustomEvent(CEID_onShowInfoState, (unsigned char*)oData.data(), oData.size());
	}

	if(abAsynch && !abUI && asSrc != NULL){
		// выцепить из файла asSrc все include-ы верхнего уровня и сначала подгрузить их
		// а потом уже и сам asSrc
		if(mScriptPreloader->AddScript(asSrc, false)){
TRACE(cx, "LoadXML: added to includes preloader\n");
			return ML_OK;
		}
	}

	std::auto_ptr<mlRMMLBuilder> pBuilder(new mlRMMLBuilder(cx));

//	if(!pXMLFile->Open(asSrc,CFile::modeRead)) return ML_ERROR_FAILURE;
/*
	ifstream file;
	file.open(asSrc, ios_base::in);
	if(file.fail()) return MME_ERROR_FAILURE;
	std::auto_ptr<mlXMLBuffer> pXMLFile(new mlXMLBuffer(&file));

	//
	CXMLArchive<mlXMLBuffer> ar((mlXMLBuffer*)pXMLFile.get());
*/
	mlTrace(cx, "Loading \'%S\'...\n",asSrc);
	mlRMMLComposition* pOldScene = mpScene;

//	res::resIResource* pRes = NULL;
//	if(isEqual(asSrc, UIDEFAULT_WSTR)){
//		mlStrResource* pStrRes = new mlStrResource(gpszUIDefault);
//		pRes = pStrRes;
//	}
TRACE(cx, "Begin Build (%d)\n", GetContext()->mpApp->GetTimeStamp() - mStartLoadSceneTime);
	if ( abUI) // Vladimir 2011.11.24
		GetContext()->mpApp->SendMessage2Launcher( APP_PERCENT, 80);
	pBuilder->Build(asSrc, this, abUI, apParent);
	if ( abUI) // Vladimir 2011.11.24
		GetContext()->mpApp->SendMessage2Launcher( APP_PERCENT, 92);
TRACE(cx, "End Build (%d)\n", GetContext()->mpApp->GetTimeStamp() - mStartLoadSceneTime);
	if(mpScene!=pOldScene){
		msSceneXML=asSrc;
	}
	mlTrace(cx, "Loaded \'%S\'\n",asSrc);

//	file.close();
	
	if(meMode!=smmAuthoring && meMode!=smmLoading && mpScene!=pOldScene)
		SetMode(smmNormal);

	mbDontSendSynchEvents = bDontSendSynchEventsWas;

TRACE(cx, "End LoadXML (%d)\n", GetContext()->mpApp->GetTimeStamp() - mStartLoadSceneTime);
	return ML_OK;
}

enum{
	EVID_onLoad=0,
	EVID_onUnload
};

omsresult mlSceneManager::LoadModuleMeta(){
	bMainModuleLoaded = false;
	GUARD_JS_CALLS(cx);
	NotifyModuleListeners(EVID_onUnload);
	{ // загружаем учебные метаданные
		wchar_t* pwcEduMETA=L"\\META-INF\\manifest.xml";
//		mlTrace(cx, "Loading module learning META \'%S\'...\n",pwcEduMETA);
		mlXMLArchive* pXMLArchive=NULL;
		if(ML_FAILED(OpenXMLArchive(pwcEduMETA,pXMLArchive))){
			LoadEduMeta(NULL);
			mlTraceError(cx, "Cannot find module learning META \'%S\'.\n",pwcEduMETA);
			return OMS_ERROR_RESOURCE_NOT_FOUND;
		}
		mlresult res=LoadEduMeta(pXMLArchive);
		if(ML_FAILED(res)){
			mlTrace(cx, "Error while loading module learning META \'%S\'.\n",pwcEduMETA);
			return OMS_ERROR_FAILURE;
		}
		CloseXMLArchive(pXMLArchive); pXMLArchive=NULL;
		mlTrace(cx, "Loaded module learning META \'%S\'\n",pwcEduMETA);
		bMainModuleLoaded = true;
	}
	LoadTechMetaDefault();
	/*{ // загружаем технические метаданные
		wchar_t* pwcTechMETA=L"\\META-INF\\tech-data.xml";
//		mlTrace(cx, "Loading module META \'%S\'...\n",pwcTechMETA);
		mlXMLArchive* pXMLArchive=NULL;
		if(ML_FAILED(OpenXMLArchive(pwcTechMETA,pXMLArchive))){
			LoadTechMeta(NULL);
			mlTraceError(cx, "Cannot find module META \'%S\'.\n",pwcTechMETA);
			return OMS_ERROR_RESOURCE_NOT_FOUND;
		}
		mlresult res=LoadTechMeta(pXMLArchive);
		if(ML_FAILED(res)){
			mlTrace(cx, "Error while loading module META \'%S\'.\n",pwcTechMETA);
			return OMS_ERROR_FAILURE;
		}
		CloseXMLArchive(pXMLArchive); pXMLArchive=NULL;
		mlTrace(cx, "Loaded module META \'%S\'\n",pwcTechMETA);
	}*/
	NotifyModuleListeners(EVID_onLoad);
	return OMS_OK;
}

omsresult mlSceneManager::LoadModuleMetaAndLoadScene( unsigned int aID, wchar_t *asModuleID, wchar_t* asScene)
{
	mlSynchData oData;
	oData << aID;
	oData << asModuleID;
	oData << asScene;
	mpContext->mpInput->AddCustomEvent(mlSceneManager::CEID_setModuleToLoadAndLoadScene, (unsigned char*)oData.data(), oData.size());
	return 1;
}

bool mlSceneManager::IsMainModuleLoaded()
{
	return bMainModuleLoaded;
}

omsresult mlSceneManager::LoadModuleMetaAndLoadSceneInternal(mlSynchData& aData)
{
	mlString sScene;
	wchar_t *sModuleID = NULL;
	wchar_t *psScene = NULL;
	unsigned int iID = 0;
	aData >> iID;
	aData >> sModuleID;
	aData >> psScene;
	sScene = psScene;
	if( LoadModuleMeta() == OMS_OK)
	{
		if(sScene.empty()){
			LoadScene(NULL);
			miCurrentSceneIndex=0;
			NotifyCurrentSceneIndex();
		}else{
			int iSceneNum=_wtoi(sScene.c_str());
			if(iSceneNum>0){
				LoadScene(iSceneNum-1);
			}else{
				LoadScene(sScene.c_str());
			}
		}
		mpContext->mpResM->OnLoadedModuleMeta(iID, 0);
	}
	else
	{
		mpContext->mpResM->OnLoadedModuleMeta(iID, 1);
		mlTrace(cx, "Cannot load module meta %S \n", sModuleID);
		return 0;
	}
	return 1;
}

mlresult mlSceneManager::LoadModuleMeta(const wchar_t* apwcModuleID, JSObject* ajsoModule, bool abGetInfo){
	{ // загружаем учебные метаданные
		mlString sEduMETA = apwcModuleID;
		sEduMETA += L":";
		sEduMETA += L"\\META-INF\\manifest.xml";
		mlXMLArchive* pXMLArchive=NULL;
		if(ML_FAILED(OpenXMLArchive(sEduMETA.c_str(),pXMLArchive))){
			mlTraceError(cx, "Cannot find module learning META \'%S\'.\n",sEduMETA.c_str());
			return ML_ERROR_FAILURE;
		}
		mlresult res=LoadEduMeta(pXMLArchive, ajsoModule, abGetInfo);
		if(ML_FAILED(res)){
			mlTrace(cx, "Error while loading module learning META \'%S\'.\n",sEduMETA.c_str());
			return ML_ERROR_FAILURE;
		}
		CloseXMLArchive(pXMLArchive); pXMLArchive=NULL;
	}
	{ // загружаем технические метаданные
		mlString sTechMETA = apwcModuleID;
		sTechMETA += L":";
		sTechMETA += L"\\META-INF\\tech-data.xml";
		mlXMLArchive* pXMLArchive=NULL;
		if(ML_FAILED(OpenXMLArchive(sTechMETA.c_str(),pXMLArchive))){
			LoadTechMeta(NULL);
			mlTraceError(cx, "Cannot find module META \'%S\'.\n",sTechMETA.c_str());
			return ML_ERROR_FAILURE;
		}
		mlresult res=LoadTechMeta(pXMLArchive, ajsoModule, abGetInfo);
		if(ML_FAILED(res)){
			mlTrace(cx, "Error while loading module META \'%S\'.\n",sTechMETA.c_str());
			return ML_ERROR_FAILURE;
		}
		CloseXMLArchive(pXMLArchive); pXMLArchive=NULL;
	}
	return ML_OK;
}

std::vector<JSObject*> vLooked;

bool FindJSORef(JSContext* cx,JSObject* obj,JSObject* objToFind,JSObject* &objFound,jschar* &jscPropName){
	if(obj==JS_GetGlobalObject(cx))
		vLooked.clear();
	JSIdArray* pGPropIDArr= JS_Enumerate(cx, obj);
	for(int ikl=0; ikl<pGPropIDArr->length; ikl++){
		jsid id=pGPropIDArr->vector[ikl];
		jsval v;
		if(!JS_IdToValue(cx,id,&v)) continue;
		jschar* jsc = NULL;
		jsval vProp;
		if(JSVAL_IS_STRING(v)){
			jsc = JS_GetStringChars(JSVAL_TO_STRING(v));
			JS_GetUCProperty(cx,obj,jsc,-1,&vProp);
		}else if(JSVAL_IS_INT(v)){
			int iId = JSVAL_TO_INT(v);
			JS_GetElement(cx, obj, iId, &vProp);
			jsc = (jschar*)iId;
		}else continue;
		if(!JSVAL_IS_REAL_OBJECT(vProp)) continue;
		JSObject* jsoProp=JSVAL_TO_OBJECT(vProp);
		bool bLooked=false;
		std::vector<JSObject*>::iterator vi;
		for(vi=vLooked.begin(); vi!=vLooked.end(); vi++){
			if(jsoProp==*vi){
				bLooked=true;
				break;
			}
		}
		if(bLooked) continue;
		if(!JSVAL_IS_INT(v) && isEqual((const wchar_t*)jsc, L"target")) continue;
		if(jsoProp==objToFind){
			objFound=obj;
			jscPropName=jsc;
			return true;
		}
		vLooked.push_back(jsoProp);
//if(JSVAL_IS_MLEL(vProp)){
//mlRMMLElement* pMLEl = JSVAL_TO_MLEL(cx, vProp);
//if(isEqual(pMLEl->GetName(),L"htmlReport"))
//int hh=0;
//
//if(pMLEl->mjso != NULL && pMLEl->mRMMLType != MLMT_SCRIPT){
//JSObject* jsoProto = JS_GetPrototype(cx, pMLEl->mjso);
//if(JSO_IS_MLEL(jsoProto)){
//	JSClass* pClass = JS_GetClass(jsoProto);
//	jsval vParent;
//	JS_GetProperty(cx, jsoProto, "parent", &vParent);
//	if(JSVAL_IS_MLEL(vParent)){
//		int hh=0;
//	}
//	mlRMMLElement* pMLELProto = GET_RMMLEL(cx, jsoProto);
//	if(pMLELProto != NULL)
//	int hh=0;
//}
//}
//
//if(pMLEl->GetMO() != NULL)
//pMLEl->MakeAllPropsEnumerable();
//}
		if(FindJSORef(cx,jsoProp,objToFind,objFound,jscPropName)){
			return true;
		}
	}
	JS_DestroyIdArray(cx, pGPropIDArr);
	jsval vProp = JSVAL_NULL;
	JS_GetUCProperty(cx,obj,(const jschar*)L"__proto__",-1,&vProp);
	if(JSVAL_IS_REAL_OBJECT(vProp)){
		JSObject* jsoProp=JSVAL_TO_OBJECT(vProp);
		if(jsoProp==objToFind){
			objFound = obj;
			jscPropName = (jschar*)L"__proto__";
			return true;
		}
	}
	return false;
}
/*
inline std::wstring replace(std::wstring text, std::wstring s, std::wstring d)
{
	for(unsigned index=0; index=text.find(s, index), index!=std::wstring::npos;)
	{
		text.replace(index, s.length(), d);
		index+=d.length();
	}
	return text;
}
*/
omsresult mlSceneManager::LoadScene(int aiNum){
	if(aiNum < 0){
//mlRMMLElement* pOldScene = (mlRMMLElement*)GetActiveScene();
		SetScene(NULL);
//JSObject* jsoRef = NULL;
//jschar* jscRefPropName = NULL;
//FindJSORef(cx, JS_GetGlobalObject(cx), pOldScene->mjso, jsoRef, jscRefPropName);
		return OMS_OK;
	}
	GUARD_JS_CALLS(cx);

	jsval vScenes=JSVAL_NULL;
	if(!JS_GetProperty(cx,mjsoModule,"scenes",&vScenes)) return OMS_ERROR_NOT_INITIALIZED;
	if(!JSVAL_IS_OBJECT(vScenes) || vScenes==JSVAL_VOID || vScenes==JSVAL_NULL)
		return OMS_ERROR_NOT_INITIALIZED;
	JSObject* jsaScenes=JSVAL_TO_OBJECT(vScenes);
	jsval vScene=JSVAL_NULL;
	if(!JS_GetElement(cx,jsaScenes,aiNum,&vScene))
		return OMS_ERROR_FAILURE;
	if(vScene==JSVAL_NULL || vScene==JSVAL_VOID)
		return OMS_ERROR_FAILURE;
	JSObject* jsoScene=JSVAL_TO_OBJECT(vScene);
	jsval vPath=JSVAL_NULL;
	if(!JS_GetProperty(cx,jsoScene,"path",&vPath))
		return OMS_ERROR_FAILURE;
	if(!JSVAL_IS_STRING(vPath)) return OMS_ERROR_FAILURE;
	JSString* jssPath=JSVAL_TO_STRING(vPath);
	jschar* jscPath=JS_GetStringChars(jssPath);
	LoadScene((const wchar_t*)jscPath);
	miCurrentSceneIndex=aiNum;
	NotifyCurrentSceneIndex();
	return OMS_OK;
}

omsresult mlSceneManager::LoadScene(const char* asSrc){
	return LoadScene(cLPWCSTR(asSrc));
}

omsresult mlSceneManager::LoadScene(const wchar_t* asSrc){ 
	// стереть все классы, кроме UI
	// ??
	if( meMode==smmPlay || meMode==smmPause)
		SetMode( smmNormal);
	if(asSrc==(const wchar_t*)1){
		// перегрузить сцену
		mlString	sNewSceneXML = msSceneXML;
		NotifyCurrentSceneIndex();
		return LoadScene(sNewSceneXML.c_str());
	}
	if(asSrc==NULL || *asSrc==L'\0')
		return LoadScene(0);
//	if(msSceneXML.empty())
//		msSceneXML = asSrc;
mStartLoadSceneTime = GetContext()->mpApp->GetTimeStamp();
	mlRMMLComposition* pOldScene = mpScene;
	mlresult res;
	if(ML_FAILED(res = LoadXML(asSrc, false, true)))
		return (omsresult)res;
	// если при загрузке сцены не создалась новая сценв,
	if(pOldScene != NULL && pOldScene == mpScene){
		// значит в файле asSrc нет описания сцены
		// удалим старую сцену
		SetScene(NULL);
	}
	return OMS_OK; 
}

unsigned long GetEventGlobalTime(omsUserEvent aEvent){
	if(aEvent.ev == 0) return 0;
	unsigned char* pData=(unsigned char*)aEvent.script; 
	pData += sizeof(unsigned short);
	return *(unsigned long*)pData;
}

void DumpNUDTime(JSContext* cx, mlNextUpdateDTimeList& mlNUDTime){
//TRACE(cx, "NUDTime:");
	std::list<int>::iterator it;
	for(it = mlNUDTime.begin(); it != mlNUDTime.end(); ++it){
//TRACE(cx, "%d, ",*it);
	}
//TRACE(cx, "\n");
}

void mlSceneManager::AddElemToList(v_elems& avElements, mlRMMLElement* apElem, bool abCheck){
	v_elems::iterator vi = avElements.begin();
	v_elems::iterator viEnd = avElements.end();
	v_elems::iterator viNull = viEnd;
	if(abCheck){
		for(; vi != viEnd; ++vi){
			mlRMMLElement* pElem = *vi;
			if(pElem == apElem)
				return; // уже в списке
			if(pElem == NULL)
				viNull = vi;
		}
	}else{
		for(; vi != viEnd; ++vi){
			if(*vi == NULL){
				viNull = vi;
				break;
			}
		}
	}
	if (viNull != viEnd)
		*viNull = apElem;
	else
		avElements.push_back(apElem);
}

void mlSceneManager::AddUpdatableElement(mlRMMLElement* apElem, bool abCheck){
	mlMedia* pParentScene = apElem->GetScene();
	if(pParentScene == NULL)
		return;
	if(pParentScene == mpUI){
		AddElemToList(mvUpdatableUIElements, apElem, abCheck);
		mbUpdatableElementsChanged = true;
		return;
	}
	if(pParentScene == mpScene){
		AddElemToList(mvUpdatableElements, apElem, abCheck);
		mbUpdatableElementsChanged = true;
	}
}

void mlSceneManager::AddSynchedElement(mlRMMLElement* apElem, bool abCheck){
if(isEqual(apElem->GetName(), L"dqvu_myhome_flag"))
int hh=0;
	mlMedia* pParentScene = apElem->GetScene();
	if(pParentScene == NULL)
		return;
	if(pParentScene != mpScene)
		return;
	if(apElem->MayHaveSynchChildren()){
		// если элемент не имеет своего ID, 
		// то помещаем первый родительский элемент, который имеет свой ID
		mlRMMLElement* pParent;
		for(pParent = apElem; pParent != NULL; pParent = pParent->GetParent()){
			unsigned int uID = pParent->GetID(false);
			if(uID != OBJ_ID_UNDEF)
				break;
		}
		if(pParent != NULL){
			AddElemToList( mvObjectsToSynch, pParent, abCheck);
		}else{
			mlTrace(cx, "Error: synchronized object %s without parent with ID found!", cLPCSTR(apElem->GetName()));
		}
	}
}

inline bool TimeHasRunOut(unsigned int aCurrentTime, unsigned int anEndTime, int anIterations)
{
#ifdef DEBUG
	return (anIterations >= 10);
#else
	return (aCurrentTime > anEndTime);
#endif
}

// Запрашивает полное состояние объекта. Исправление #84
inline void QueryElementFullState(mlRMMLElement* aElem, sync::syncISyncManager* aSyncMan,
								  const __int64& aCurrentTime, int aWaitTime)
{
	if (aCurrentTime - aElem->GetFullStateQueryTime() > aWaitTime)
	{
		// Запрашиваем полное состояние элеммента
		aSyncMan->QueryObject( aElem->GetID(), aElem->GetBornRealityID(), aElem->GetRealityID(), 0);
		aElem->SetFullStateQueryTime( aCurrentTime);
	}
}

inline void HandleElementSynchronization( mlRMMLElement* aElem, sync::syncISyncManager* aSyncMan,
										 unsigned int aCurrentRealityID, const __int64& aCurrentTime)
{
	ML_ASSERTION( aElem->mcx, aElem->IsSynchronized(), "");
	syncObjectState oState;
	if (!aElem->IsSynchInitialized())
	{
		// #8772 для создаваемого мной объекта не запрашиваем фул стейт при создании, мои свойства верные (размещаемые мной, мой аватар)
		// при простановке синх свойств объекту до onLoad из SendSyncElements зайдет сюда фул стейт тоже не запрашиваем
		if (  aElem->IsMyOwner() ) 
		{
			return;
		}
		// Рефакторинг. Исправление #84
		QueryElementFullState( aElem, aSyncMan, aCurrentTime, 10000000);

	}
	else if (aElem->GetSynchProps(oState))
	{
		// получим полное состояние для хэша
		oState.uiHashObj = aElem->GetObjectStateHash();
		aSyncMan->SendObjectState(oState);
	}
}

void mlSceneManager::CreateSyncObjects( unsigned int aTimeQuota)
{
 	if (!m_realityLoadedEventWasGeneratedAlready)
	{
		NotifyAfterCreatingOfObjects( m_synchObjectsInRealityCreated, m_synchObjectsInRealityCount);
		if (m_synchObjectsInRealityCreated >= m_synchObjectsInRealityCount)
		{
			m_realityLoadedEventWasGeneratedAlready = true;			
		}
	}

	// пока scene.objects недоступно, объекты не создаем!
	if (!mpContext->mpMapMan->IsScene3DReady())
	{
		return;
	}

	float avatarX;
	float avatarY;
	float avatarZ;

	char* avatarLocation;

	mpContext->mpMapMan->GetAvatarPosition( avatarX, avatarY, avatarZ, &avatarLocation);
	mpContext->mpSyncMan->SetAvatarPosition(avatarX, avatarY, avatarLocation);

	unsigned int endTime = mpContext->mpApp->GetTimeStamp() + aTimeQuota;
	int i = 0;
	while (!TimeHasRunOut( mpContext->mpApp->GetTimeStamp(), endTime, i))
	{
		ZONE_OBJECT_INFO* mapObject = NULL;
		if (!mpContext->mpSyncMan->NextMapObject(mapObject))
			break;

		if (mapObject == NULL)
			break;		

		mlObjectInfo4Creation objectInfo;
		EFillingObjectInfoResult fillingObjectInfoResult = mpContext->mpMapMan->FillObjectInfo( *mapObject, objectInfo);		
		if (fillingObjectInfoResult == FOI_SUCCESS && objectInfo.objectFullState)
		{
			mlMedia* obj = NULL;
			if (objectInfo.objectFullState->sys_props.IsConnected())
			{
				if (objectInfo.isMyAvatar)
				{
					SetInfoState( 5);
					CLogValue logValue("Creating my avatar...");
					mpContext->mpLogWriter->WriteLnLPCSTR( logValue.GetData());
					mlMedia* pMyAvatar = CreateAvatar( objectInfo);					
					if (pMyAvatar != NULL)
					{
						CLogValue logValue("Creating my avatar - ok. ");
						mpContext->mpLogWriter->WriteLnLPCSTR( logValue.GetData());
						mpContext->mpMapMan->SetAvatar( pMyAvatar, *mapObject);
						SetInfoState( 6);
						obj = pMyAvatar;
					}
					else
					{
						CLogValue logValue("[ERROR] Creating my avatar - failed!!! ");
						mpContext->mpLogWriter->WriteLnLPCSTR( logValue.GetData());
					}
				}
				else
				{
					int t1 = mpContext->mpApp->GetTimeStamp();
					obj = CreateObject( objectInfo);
					int t2 = mpContext->mpApp->GetTimeStamp();
					if (objectInfo.musType == MLMT_CHARACTER)
					{
						CLogValue l("created avatar ", objectInfo.mszName, " in ", t2 - t1, " ms");
						mpContext->mpLogWriter->WriteLnLPCSTR( l.GetData());
					}
				}
			}

			// Apply all cached sync states
			// Вектор идентификаторов не наденных синхронируемых объектов
			CVectorID notFoundSynchObjectIDs;
			syncObjectState* pState = NULL;
			while( mpContext->mpSyncMan->NextObjectState( mapObject, pState))
			{
				if (pState == NULL)
					break;
				ApplySyncState( pState, &notFoundSynchObjectIDs);

				mpContext->mpSyncMan->FreeCachedObjectState( pState);
			}
			//assert( notFoundSynchObjectIDs.size() == 0);
		}

		mpContext->mpSyncMan->FreeMapObject( mapObject);
		m_synchObjectsInRealityCreated++;
	}
}

void mlSceneManager::ApplySyncState( const syncObjectState* const aState, CVectorID* aNotFoundSynchObjectIDs)
{
	ESynchStateError error = SSE_NO_ERRORS;

	if (aState == NULL)
		error = SSE_WRONG_STATE;
	else if( aState->type == syncObjectState::ST_NOTFOUND)
	{
		// Удаляем этот объект, т.к. его нет на сервере
		if (aState->uiID >= OBJ_ID_EDITABLE) // TODO: Необходимо сделать так, чтобы клиент не запрашивал статичные объекты ненулевой реальности
			error = SSE_DELETE_NOT_FOUND;
	}
	else
	{
		
		int currentRealityID = mpContext->mpMapMan->GetCurrentRealityID();		
		// если пакет не из нашей реальности, его не обрабатываем
		if (aState->uiRealityID != currentRealityID)
		{
			// Удаляем аватара, перешедшего в другую реальность. Исправление #84
			if (aState->uiID >= OBJ_ID_AVATAR && aState->uiBornRealityID == 0)
			{
				error = SSE_DELETE_WRONG_REALITY_AVATAR;
			}
			else
				error = SSE_WRONG_STATE;
		}
		else
		{
			// ищем синхронизируемый объект среди скриптовых
			mlRMMLElement* pMLEl = scriptedObjectManager.GetObject( aState->uiID, aState->uiBornRealityID);

			// если объект найден, то проверим был ли он удален
			if (pMLEl != NULL && PMO_IS_NULL( pMLEl->mpMO))
			{
				// Разрегисмтрируем его и будем считать, что его нет
				scriptedObjectManager.UnregisterObject( pMLEl);
				pMLEl = NULL;
			}

			// объект должен существовать
			if (pMLEl == NULL)
			{
				if( aState->sys_props.IsConnected())
				{
					// объект должен быть создан
					error = SSE_OBJECT_NOT_FOUND;
				}
				else
				{
					// При получении свойств синхронизации с connection==false запрос на параметры объекта не отправляется
					error = SSE_NO_ERRORS;
				}
			}
			else
			{				
				//
				// обновляем состояние элемента
				error = pMLEl->SetSynchProps(*aState);				

				if (error == SSE_NO_ERRORS)
				{
					if( aState->type == syncObjectState::ST_FULL || aState->type == syncObjectState::ST_FORCE)
					{
						if( mpContext->mpIPadMan != NULL)
						{							
							if( mpContext->mpIPadMan->IsIPadObject( pMLEl->GetName(), aState->uiID, aState->uiBornRealityID, aState->uiRealityID))
							{								
								mpContext->mpIPadMan->SendForceFullState();
							}
						}
					}
					if( aState->type == syncObjectState::ST_UPDATE /*&& (pMLEl->IsAvatar() || isEqual( pMLEl->GetClass()->GetName(), L"panel_object"))*/)
					{
						// Проверка контрольной суммы
						uint uiServerHashObj = aState->uiHashObj;
						uint uiHashObj = pMLEl->GetObjectStateHash();
						if( uiServerHashObj > 0 && uiHashObj != uiServerHashObj)
						{
//MessageBox(0, "Call Alex please", "Hash. It happened...", MB_OK);
							mpContext->mpLogWriter->WriteLn("[HASH] Hash error for objectID = ", aState->uiID, " realityID = ", aState->uiRealityID, " bornRealityID = ", aState->uiBornRealityID, " clientVersion = ", aState->mwClientVersion, " serverVersion = ", aState->mwServerVersion);
							mpContext->mpLogWriter->WriteLn("[HASH] uiServerHashObj = ", uiServerHashObj, " uiHashObj = ", uiHashObj);

							// сделана защита от повторения запросов на полное состояние аватара так как есть запрос полного состояния через 10 секунд если не приходило обновление.
							// сделан таймаут, если не пришел ответ на полное состояние, считаем, что надо запросить снова.
							if( pMLEl->IsAvatar())
							{								
								__int64 currentTime = mpContext->mpApp->GetTimeStamp();
								if( currentTime - pMLEl->GetFullStateQueryTime() > 3000)
									error = SSE_HASHERROR;
							}
							else
								error = SSE_HASHERROR;
						}	
					}					
					if (aState->uiID == uiCreatedAvatarID && aState->uiBornRealityID == 0)
					{
						uiCreatedAvatarID = 0xFFFFFFFF;
						/*jsval vAvatar = JSVAL_NULL;
						vAvatar = OBJECT_TO_JSVAL(pMLEl->mjso);
						JS_SetProperty(cx, GetEventGO(), "avatar", &vAvatar);

						NotifyMapOnMyAvatarCreatedListeners();
						JS_DeleteProperty( cx, GetEventGO(), "avatar");*/
					}
				}
			}
		}
	}

	switch (error)
	{
	case SSE_NO_ERRORS:
		// Обновление применено - ошибок нет
		break;
	case SSE_WRONG_STATE:
		// Не смогли интерпретировать данные, например: aState == NULL
 		//assert(false);
		break;
	case SSE_OBJECT_NOT_FOUND:
		{
			if (aNotFoundSynchObjectIDs)
			{
				// уведомить MapManager о том, 
				// что получены синхронизируемые данные для несуществующего объекта
				cm::cmObjectIDs ids( aState->uiID, aState->uiBornRealityID);
				if (!mpContext->mpMapMan->IsObjectCreating( aState->uiID))
				{
					CVectorID::const_iterator it = find( aNotFoundSynchObjectIDs->begin(), aNotFoundSynchObjectIDs->end(), ids);
					if (it == aNotFoundSynchObjectIDs->end())
						aNotFoundSynchObjectIDs->push_back( ids);
				}
			}
		}
		break;
	case SSE_NOT_SYNCRONIZED:
		// Пришёл пакет обновления для не синхронизируемого объекта
		assert(false);
		break;
	case SSE_DELETE_WRONG_REALITY_AVATAR:
	case SSE_DELETE_DISCONECTED:
	case SSE_DELETE_CHANGED_REALITY_AVATAR:
	case SSE_DELETE_NOT_FOUND:
	{
		// Объект нужно удалить с этого клиента, потому что он вышел из мира или из текущей реальности
		DeleteObjectInternal(aState->uiID, aState->uiBornRealityID);
	}	break;
	case SSE_REJECTED:
	case SSE_HASHERROR:
	{
		mlRMMLElement* pElem = scriptedObjectManager.GetObject( aState->uiID, aState->uiBornRealityID);
		if (pElem != NULL)
			QueryElementFullState( pElem, mpContext->mpSyncMan, mlTime, 0);
		else
			assert(false);
	}	break;
	case SSE_OBSOLETE:
		break;
	}
}

void mlSceneManager::UpdateIPadElements()
{
	if (mpContext->mpIPadMan == NULL)
		return;
	mpContext->mpIPadMan->UpdateIPadObjects();
}

void mlSceneManager::UpdateSyncElements()
{
	// Вектор идентификаторов не наденных синхронируемых объектов
	CVectorID notFoundSynchObjectIDs;

	// без SyncMan делать нечего
	if (mpContext->mpSyncMan == NULL)
		return;

	// запрашиваем все NextObjectState
	while (true)
	{
		syncObjectState* pState = NULL;
		if (!mpContext->mpSyncMan->NextObjectState( pState))
			break;
		if (pState == NULL)
			break;

		ApplySyncState( pState, &notFoundSynchObjectIDs);

		mpContext->mpSyncMan->FreeObjectState( pState);
	}

	// если при обновлении синхронизируемых объектов какие-то из них не были найдены,
	// то нужно запросить их создание у MapManager'а
	if (mpContext->mpMapMan != NULL && notFoundSynchObjectIDs.size() > 0)
	{
#if defined(_MSC_VER) && (_MSC_VER >= 1600 )
		cm::cmObjectIDs *p = notFoundSynchObjectIDs.begin()._Ptr;
#else
		cm::cmObjectIDs *p = notFoundSynchObjectIDs.begin()._Myptr;
#endif		
		mpContext->mpMapMan->CreateObjects( p, notFoundSynchObjectIDs.size());
	}
}

void mlSceneManager::SendSyncElements()
{
	if (mpContext->mpSyncMan == NULL)
		return;

	std::vector<mlRMMLElement*>::const_iterator cvi = mvObjectsToSynch.begin();
	for (;  cvi != mvObjectsToSynch.end();  cvi++)
	{
		mlRMMLElement* pElem = *cvi;
		if (pElem == NULL)
			continue;

		HandleElementSynchronization( pElem, mpContext->mpSyncMan, mpContext->mpMapMan->GetCurrentRealityID(), mlTime);
	}
}

void mlSceneManager::SendQueryOldSyncElements( ObjectsVector &objects, unsigned int aCurrentTime, unsigned int aWaitTime, mlRMMLElement* pNonSendElem)
{
	//aWaitTime = 1000;

	ObjectsVector::const_iterator it = objects.cbegin();
	for (;  it != objects.cend();  it++)
	{
		mlRMMLElement* pObj = (*it);
		if (pObj == NULL)
			continue;
		if( pNonSendElem != NULL && pNonSendElem == pObj)
			continue;
		if (aCurrentTime - pObj->GetSynchChangingTime() > aWaitTime)
		{		
			unsigned int uiHashObj = pObj->GetObjectStateHash();
			mpContext->mpSyncMan->AddQueryLatentedObject( pObj->GetID(), pObj->GetBornRealityID(), mpContext->mpMapMan->GetCurrentRealityID(), uiHashObj);
		}
	}
	mpContext->mpSyncMan->QueryLatentedObjects();
}

// Исправление #414
void mlSceneManager::QueryOldSyncElements()
{
	if (mpContext->mpSyncMan == NULL)
		return;

	if (mlLastOldAvatarsTime == 0)
		return;

	__int64 currentTime = mpContext->mpApp->GetTimeStamp();
	//mlTrace(cx, "[DEBUG CODE] start synch objects\n");
	if (currentTime - mlLastOldAvatarsTime < 30000)
		return;

	mlLastOldAvatarsTime = currentTime;

	// Запрашиваем полное состояние всех аватаров
	ObjectsVector avatars;
	scriptedObjectManager.GetRealAvatars( avatars);
	scriptedObjectManager.GetRecordedAvatars( avatars);	
	mlTrace(cx, "[DEBUG CODE] start sendQueryOldSyncElements 1\n");
	SendQueryOldSyncElements(avatars, currentTime, 30000, mpMyAvatar);	

	if (mlLastOldSyncElementsTime == 0)
		return;

	if (currentTime - mlLastOldSyncElementsTime < 30000)
		return;

	mlLastOldSyncElementsTime = currentTime;

	// Запрашиваем полное состояние всех объектов не аватаров
	ObjectsVector objects;
	scriptedObjectManager.GetNoAvatarSyncObjects( objects);
	mlTrace(cx, "[DEBUG CODE] start sendQueryOldSyncElements 2\n");
	SendQueryOldSyncElements(objects, currentTime, 30000);
}
// Конец #414

bool mlSceneManager::RebuildUpdatableElements( const __int64 alTime)
{
	bool rebuilt = false;
	
	// через каждые 5 минут обновляем список обновляемых элементов сцены
	if (IsSceneStructChanged() || (alTime - mlLastUpdatableListBuildingTime) > 5*60*1000*1000)
	{
		int iLastUpdatableObjectCount = mvUpdatableElements.size();
		mvUpdatableElements.clear();
		mvUpdatableElements.reserve(iLastUpdatableObjectCount + 10);
		int miLastSynchObjectCount = mvObjectsToSynch.size();
		mvObjectsToSynch.clear();
		if (miLastSynchObjectCount > 10)
			mvObjectsToSynch.reserve(miLastSynchObjectCount);
		mlRMMLElement* pElem = NULL;
		mlRMMLIterator Iter( cx, mpScene, false, true);
		while ((pElem = Iter.GetNext()) != NULL)
		{
			mlresult res = pElem->Update(alTime);
			if (res != ML_ERROR_NOT_IMPLEMENTED)
				mvUpdatableElements.push_back(pElem);

			if (pElem->MayHaveSynchChildren())
			{
				// если элемент не имеет своего ID, 
				// то помещаем первый родительский элемент, который имеет свой ID
				mlRMMLElement* pParent;
				for (pParent = pElem; pParent != NULL; pParent = pParent->GetParent())
				{
					unsigned int uID = pParent->GetID(false);
					if (uID != OBJ_ID_UNDEF)
						break;
				}
				if (pParent != NULL)
				{
					if (isEqual( pParent->GetClass()->GetName(), L"CLocationTable"))
					{
						int n = 0;
					}
					mvObjectsToSynch.push_back(pParent);
				}
				else
					mlTrace(cx, "Error: synchronized object %s without parent with ID found!", cLPCSTR(pElem->GetName()));
			}
		}
		mlLastUpdatableListBuildingTime = alTime;
		rebuilt = true;
	}
	return rebuilt;
}

void mlSceneManager::UpdateUpdatableElements(const __int64 alTime)
{
	mbUpdatableElementsChanged = false;
	v_elems::iterator vi = mvUpdatableElements.begin();
	for(; vi != mvUpdatableElements.end(); vi++)
	{
		mlRMMLElement* pElem = *vi;
		if (pElem == NULL)
			continue;

		// обновляем элемент
		mlresult res = pElem->Update( alTime);

		// если удалились обновляемые элементы
		if(mbUpdatableElementsChanged)
		{
			mbUpdatableElementsChanged = false;
			// и похоже буфер перестроился, то находим только что обновленный элемент заново
			vi = find( mvUpdatableElements.begin(), mvUpdatableElements.end(), pElem);
			if(vi == mvUpdatableElements.end())
				break;
		}

		// если не нужно обновлять в будущем, то исключаем его из списка обновляемых элементов
		if (res == ML_ERROR_NOT_IMPLEMENTED && vi != mvUpdatableElements.end())
			*vi = NULL;
	}
}

void mlSceneManager::CreateSyncObjectsIfNeeded(const bool isNormalMode)
{
	// максимальное время на создание скриптуемых объектов на 1 кадр в режиме обычной работы
	// для дебага ставим значение побольше
#ifdef _DEBUG
	const unsigned int maxUsedFrameTimeInNormalMode = 50;

#else
	const unsigned int maxUsedFrameTimeInNormalMode = 10;
#endif
	// максимальное время на создание скриптуемых объектов на 1 кадр в режиме загрузки (картинки)
	// для дебага ставим значение побольше
#ifdef _DEBUG
	const unsigned int maxUsedFrameTimeInLoadingMode = 500;
#else
	const unsigned int maxUsedFrameTimeInLoadingMode = 200;
#endif
	// время, на которое было превышение на прошлом кадре/кадрах
	static unsigned int overTimeCounter = 0;	

	int maxUsedFrameTime = isNormalMode ? maxUsedFrameTimeInNormalMode : maxUsedFrameTimeInLoadingMode;

	if ((!m_realityLoadedEventWasGeneratedAlready) && (mpContext->mpMapMan->GetCurrentRealityID() != 0))	
	{
		overTimeCounter = 0;
		maxUsedFrameTime = maxUsedFrameTimeInLoadingMode;
	}

	if ((int)overTimeCounter >= maxUsedFrameTime)
	{
		overTimeCounter -= maxUsedFrameTime;
		return;
	}
	else
	{
		maxUsedFrameTime -= overTimeCounter;
	}

	if (GetWorldScene() != NULL)
	{
		unsigned int startTime = mpContext->mpApp->GetTimeStamp();
		CreateSyncObjects(maxUsedFrameTime);
		unsigned int endTime = mpContext->mpApp->GetTimeStamp();

		if ((int)(endTime - startTime) > maxUsedFrameTime)
		{
			overTimeCounter = endTime - startTime - maxUsedFrameTime;
		}
	}
}

void mlSceneManager::UpdateAllElements(const __int64 alTime, const bool isNormalMode)
{
	CreateSyncObjectsIfNeeded(isNormalMode);

	// обновляем синхронизируемые объекты
	UpdateSyncElements();
	UpdateIPadElements();

	if (mbObjetcsFreezeUnfreezenObjects)
	{	
		  mbObjetcsUnfreezenAfterFreeze  = false;
		  mbObjetcsFreezeUnfreezenObjects = false;
		  miUpdateNumber = 0;
		  FreezeAllObjects();
	}
	
	bool updated = false;

#ifdef ML_FULL_LISTS_UPDATE
	// будем сохранять при Update-е элементы в вектор, 
	// а потом его переберем и тогда уже будем посылать изменения на синхронизацию
	updated = RebuildUpdatableElements( alTime);
#endif

	// обновляем обновляемые элементы
	if (!updated)
		UpdateUpdatableElements( alTime);

	// отправляем на сервер измения синхронизируемых объектов
	SendSyncElements();

	// Исправление #414
	QueryOldSyncElements();
}

void mlSceneManager::GlobalTimeShifted(const __int64 alTime, const __int64 alTimeShift){
	mlRMMLElement* pElem=NULL;
	mlRMMLIterator IterUI(cx,mpUI);
	while((pElem=IterUI.GetNext())!=NULL)
		pElem->GlobalTimeShifted(alTime, alTimeShift, mePrevMode);
	mlRMMLIterator IterScn(cx,mpScene);
	while((pElem=IterScn.GetNext())!=NULL)
		pElem->GlobalTimeShifted(alTime, alTimeShift, mePrevMode);
}

// если в режиме инициализации при обработке события лога
// запустился на вопроизведение длящийся объект, 
// то вернуть управление RM-у, чтобы он сделал у себя update,
// установил в начало очереди событий соответствующее событие
// и снова вызвал mlSceneManager::Update
bool mlSceneManager::NeedToUpdateRMNow(const __int64 alTime, int &aiDeltaTime, const bool isNormalMode){
	if(meMode != smmInitilization)
		return false;
#ifdef MLD_TRACE_INIT_EVENTS
DumpNUDTime(cx, mlNUDTime);
#endif
	// если при инициализации есть внутренние события в начале очереди 
	// то обработать их в принудительном порядке перед reupdate-ом РМ-а
	oms::omsIInput* pInp = mpContext->mpInput;
	if(pInp!=NULL){
		// берем
		omsUserEvent Event = pInp->GetEvent();
		if(Event.ev != 0){
			// проверяем
			if((Event.ev&0xFF00)==omsUserEvent::INTERNAL){
				// возвращаем в очередь и выходим
				pInp->AddEvent(Event, true);
				return false;
			}
			// возвращаем в очередь
			pInp->AddEvent(Event, true);
		}
	}
	int iNextUpdateDeltaTime = mlNUDTime.GetMinimumDTime();
	if(iNextUpdateDeltaTime < 0)
		return false;
	// если dt следующего события синхронизации
	// больше наименьшего из mlNUDTime
	omsUserEvent NextSynchEvent = mpContext->mpInput->GetNextCallEvent();
	unsigned long ulNextEventGTime = GetEventGlobalTime(NextSynchEvent);
	if(ulNextEventGTime == 0)
		return false;
	int iNextEventDeltaTime = (int)(ulNextEventGTime - mulSynchEventsGlobalTime);
#ifdef MLD_TRACE_INIT_EVENTS
TRACE(cx, "---SynchInitInfo--- iNextEventDeltaTime: %d\n", iNextEventDeltaTime);
#endif
	if(iNextEventDeltaTime <= iNextUpdateDeltaTime)
		return false;
	// то обновляем состояние всех элементов 
//TRACE("UpdateAllElements ");
//DumpNUDTime(mlNUDTime);
	UpdateAllElements(alTime, isNormalMode);
	// возможно во время обновления появилось новое значение
	iNextUpdateDeltaTime = mlNUDTime.GetMinimumDTime();
//DumpNUDTime(mlNUDTime);
	mlNUDTime.pop_front();
	aiDeltaTime = iNextUpdateDeltaTime;
	mlNUDTime.Update(iNextUpdateDeltaTime); // ??
	mulSynchEventsGlobalTime += iNextUpdateDeltaTime;
#ifdef MLD_TRACE_INIT_EVENTS
DumpNUDTime(cx, mlNUDTime);
mlTrace(cx, "---SynchInitInfo--- Reupdate %d SGT:%d\n", aiDeltaTime, (int)mulSynchEventsGlobalTime);
#endif
	mlLastUpdateTime = alTime;
	miLastUpdateDeltaTime = aiDeltaTime;
	return true;
}

unsigned int mlSceneManager::GetUpdateEventHandlingTimeQuota(){
	// максимальное время на обработку событий на 1 кадр в режиме обычной работы
	// для дебага ставим значение побольше
#ifdef _DEBUG
	const unsigned int maxUsedFrameTimeInNormalMode = 50;

#else
	const unsigned int maxUsedFrameTimeInNormalMode = 10;
#endif
	// максимальное время на обработку событий на 1 кадр в режиме загрузки (картинки)
	// для дебага ставим значение побольше
#ifdef _DEBUG
	const unsigned int maxUsedFrameTimeInLoadingMode = 500;
#else
	const unsigned int maxUsedFrameTimeInLoadingMode = 200;
#endif
	unsigned int uiUEHTimeQuota = maxUsedFrameTimeInNormalMode;
	if(meMode == smmLoading)
		uiUEHTimeQuota = maxUsedFrameTimeInLoadingMode;
	return uiUEHTimeQuota;
}

/**
 * Update
 */ 

using oms::omsIInput;

#define MMCHKBUTDELAYMS ((1000/24)*4)

// 1 секунда - время минимальной задержки между вызовами сборщика мусора
#define CALL_GC_MIN_DELAY (1*1000*1000)
// 3 минуты - время максимальной задержки между вызовами сборщика мусора
#define CALL_GC_MAX_DELAY (3*60*1000*1000)
// 700 мегабайт - размер памяти при превышении которого будет принудительно вызван GC с периодичностью в CALL_GC_MEM_EXCEED_CHECK_DELAY секунд
#define CALL_GC_MEM_EXCEED_SIZE (700*1024*1024)
// 15 секунд - время задержки между вызовами сборщика мусора при превышении лимита памяти
#define CALL_GC_MEM_EXCEED_CHECK_DELAY (15*1000*1000)

void mlSceneManager::CallGCIfNeeded(const __int64 alTime){
	// через каждые 3 минуты по любому вызывать GC
	if((mbNeedToCallGC && (alTime - mlLastCallGCTime) > CALL_GC_MIN_DELAY)		// если нужно вызвать и прошло больше секунды с последнего вызова
		|| (alTime - mlLastCallGCTime) > CALL_GC_MAX_DELAY						// или прошло больше 3х минут с последнего вызова 3*60*1000*1000
		)
	{ 
		ResetMouseTarget();
		ResetEvent();
		if(mpElemRefToClear != NULL){
//			mpElemRefToClear->ClearRefs();	// не помогло до конца :(
			mpElemRefToClear = NULL;
		}
		/*if (mpContext->mpLogWriter)
			mpContext->mpLogWriter->WriteLnLPCSTR( "[RMML]: Garbage collector");*/
//unsigned long tmStartGC = mpContext->mpApp->GetTimeStamp();
		OptimizeList(mvNewElements);
		OptimizeList(mvUpdatableElements);
		OptimizeList(mvUpdatableUIElements);
		OptimizeList(mvObjectsToSynch);
		//JS_GC(cx);
		CallJS_GC();
//mlTrace(cx, "JS_GC working time: %d\n",(int)(mpContext->mpApp->GetTimeStamp() - tmStartGC));
		mbNeedToCallGC=false;
		mlLastCallGCTime = alTime;
	}
}

// Обновить сцену (alTime и aiDeltaTime - в миллисекундах * 1000)
omsresult mlSceneManager::Update(const __int64 alTime, int &aiDeltaTime, const bool isNormalMode){
	//	if(meMode==smmAuthoring) return AuthoringUpdate();
	if(mbDontUpdateOneTime){
		mbDontUpdateOneTime = false;
		return OMS_OK;
	}
	if(mbUpdating) 
		return OMS_OK;
	mbUpdating=true;
	GUARD_JS_CALLS(cx);
	giInUpdateNewObjectCount = 0;
//	aiDeltaTime = miNextUpdateDeltaTime = -1; // -1 - by default, 0 - right now, >0 later
	aiDeltaTime = -1; // -1 - by default, 0 - right now, >0 later
//if(mpScene!=NULL){
//mlSynchData oData;
//mpScene->GetPropertiesInfo(oData);
//}
#ifdef JS_THREADSAFE
	if(mpUtilityThread.get() != NULL){
		mlGuardElemsToDelete oGuard(mpUtilityThread.get());
		v_elems::iterator vi = mvElemsToDelete.begin();
		v_elems::iterator viEnd = mvElemsToDelete.end();
		for(; vi != viEnd; vi++){
			mlRMMLElement* pElem = *vi;
			MP_DELETE( pElem);
		}
		mvElemsToDelete.clear();
		std::vector<mlJSClass*>::iterator vi2 = mvObjectsToDelete.begin();
		std::vector<mlJSClass*>::iterator vi2End = mvObjectsToDelete.end();
		for(; vi2 != vi2End; vi2++){
			mlJSClass* pObj = *vi2;
			MP_DELETE( pObj);
		} 
		mvObjectsToDelete.clear();
	}
#endif

	unsigned long PageFaultCount;
	unsigned long PeakWorkingSetSize;
	unsigned long WorkingSetSize;
	unsigned long QuotaPeakPagedPoolUsage;
	unsigned long QuotaPagedPoolUsage;
	unsigned long QuotaPeakNonPagedPoolUsage;
	unsigned long QuotaNonPagedPoolUsage;
	unsigned long PagefileUsage;
	unsigned long PeakPagefileUsage;

	mpContext->mpApp->GetConsumedMemorySize(PageFaultCount
											, PeakWorkingSetSize
											, WorkingSetSize
											, QuotaPeakPagedPoolUsage
											, QuotaPagedPoolUsage
											, QuotaPeakNonPagedPoolUsage
											, QuotaNonPagedPoolUsage
											, PagefileUsage
											, PeakPagefileUsage);

	CallGCIfNeeded(alTime);

	if( (alTime - mlLastMemSizeCheckTime) > 5 * 100 * 1000) // 0.5 sec
	{
		if( mpContext->mpApp->GetConsumedMemorySize(PageFaultCount , PeakWorkingSetSize , WorkingSetSize , QuotaPeakPagedPoolUsage, QuotaPagedPoolUsage, QuotaPeakNonPagedPoolUsage, QuotaNonPagedPoolUsage, PagefileUsage, PeakPagefileUsage))
		{
			if((alTime - mlLastForciblyCallGCTime) > CALL_GC_MEM_EXCEED_CHECK_DELAY && (WorkingSetSize+QuotaPagedPoolUsage+QuotaNonPagedPoolUsage)>CALL_GC_MEM_EXCEED_SIZE)
			{
				JS_GC(cx);
				mpContext->mpRM->OnMemoryOverflow();
				mbNeedToCallGC=false;
				mlLastForciblyCallGCTime = alTime;
			}
		}
		mlLastMemSizeCheckTime = alTime;
	}

	if(muStructChangedUpdCntr > 0) muStructChangedUpdCntr--;
	mlTime=alTime;
	// устанавливаем глобальную переменную _time
	jsval vTime = JSVAL_NULL;
	mlJS_NewDoubleValue(cx, (jsdouble)alTime/1000, &vTime);
	//jsval vTime=INT_TO_JSVAL((int)alTime/1000);
	JS_SetProperty(cx, mjsoPlayer,"_time",&vTime);
	if(mbNeedToNotifyMouseModalButtons){
		NotifyMouseModalButtons();
		mbNeedToNotifyMouseModalButtons = false;
	}
	if(mbNeedToShiftGlobalTime){
		// то надо уведомить элементы о том, что сменилось глобальное время
		mbNeedToShiftGlobalTime = false;
TRACE(cx, "GlobalTimeShifted: %d\n", (alTime - mlLastUpdateTime)/1000);
		GlobalTimeShifted(alTime, alTime - mlLastUpdateTime );
	}
	if(ML_TRACE_ELEM_COUNT_TIME > 0){
		if((alTime - mlLastTraceElemCountTime) > ML_TRACE_ELEM_COUNT_TIME*60*1000*1000){
			mlTrace(cx, "[RMML] UI elements: %d, Scene elements: %ld\n", muiUIElementCount, muiSceneElementCount);
			#ifdef TRACE_LOST_ELEMS 
			RecheckNotInScenesElems();
			mlTrace(cx, "[RMML] Lost elements: %d\n", mmapNotInScenesElems.size());
			#endif
			if(glTotalGCObjectCount > 0)
				mlTrace(cx, "[RMML] Total JS-object count: %d \n", glTotalGCObjectCount);
			uint32 uiAllocatedBytes = JS_GetGCParameter(gJSRunTime, JSGC_BYTES);
			mlTrace(cx, "[RMML] Allocated bytes in JS: %d\n", uiAllocatedBytes);
			DumpLists();
			mlLastTraceElemCountTime = alTime;
		}
		
	}

	// пока даем 5ms на работу прелоадера скриптов
	// вполседствии надо будет как-то более гибко к определению квоты подходить
	int iScriptPreloaderQuota = 5; // 5 ms
	if(mbPreloadingMode)
		iScriptPreloaderQuota = 40;
#ifdef _DEBUG
	iScriptPreloaderQuota = 250;
#endif 
	mScriptPreloader->Update(iScriptPreloaderQuota);
	//
//#ifdef MLD_TRACE_INIT_EVENTS
//if(gpSM->GetMode()==smmInitilization){
//mlTrace(cx, "---SynchInitInfo--- Update %d\n", (int)(alTime/1000));
//}
//#endif
	// обрабатываем события от пользователя
	/*
			Фейковое событие нажатия на кнопку геймпада
			Перезаписывает под собой нажатие на клавиатуру
	*/
	if (m_ctrl && m_ctrl->IsConnected())
	{
		if (m_prevButtons != m_ctrl->GetState().Gamepad.wButtons)
		{
			WORD newButtons = m_ctrl->GetState().Gamepad.wButtons;
			for (size_t i = 0; i < m_XInputButtons.size(); ++i)
			{
				handleGamepadButton(m_XInputButtons[i], newButtons);
			}
			m_prevButtons = newButtons;
		}
		else
		{
			m_pressedButton = 0;
		}

		short newLeftState = InternalGetLeftThumbState(), 
			  newRightState = InternalGetRightThumbState();
		if (newLeftState != m_leftThumbState)
		{
			HandleThumb(m_leftThumbState, m_pressedLeftThumb, newLeftState);
		}
		/*else
		{
			m_pressedLeftThumb = ThumbState::NONE;
		}*/

		if (newRightState != m_rightThumbState)
		{
			HandleThumb(m_rightThumbState, m_pressedRightThumb, newRightState);
		}
		/*else
		{
			m_pressedRightThumb = ThumbState::NONE;
		}*/
	}
	bool bEventHandlingBreaked = false; // прервана ли обработка событий до того, как обработаются все события в очереди
	omsIInput* pInp = mpContext->mpInput;
	if(pInp!=NULL){
		//
		if(NeedToUpdateRMNow(alTime, aiDeltaTime, isNormalMode)){
			mbUpdating=false;
			return OMS_OK;
		}
		// ожидающие своего update-а события помещаем в очередь
		mlNextUpdatesEvents::iterator vi = mvlNUEvents.begin();
		for(; vi != mvlNUEvents.end(); ){
			mlNextUpdatesEvent* pNUEvent = *vi;
			if(pNUEvent->muUpdateCounter == 0){
				pInp->AddCustomEvent(pNUEvent->miEventID, (unsigned char*)pNUEvent->mData.data(), pNUEvent->mData.size());
				vi = mvlNUEvents.erase(vi);
				if(pNUEvent->mbForceBUMCheck){
					// выключаем у viewport-а флаг более частой проверки объекта под мышкой
					//?? по идее было бы неплохо сохранять предыдущее значение флага, а не тупо его сбрасывать
					if(mBUM.mpVisible){
						mlRMMLElement* pMLElViewport = mBUM.mpVisible->GetElem_mlRMMLVisible();
						if(pMLElViewport->mRMMLType == MLMT_VIEWPORT){
							mlRMMLViewport* pViewport = (mlRMMLViewport*)pMLElViewport;
							pViewport->SetTraceObjectOnMouseMove(false);
						}
					}
				}
				 MP_DELETE( pNUEvent);
			}else{
				pNUEvent->muUpdateCounter--;
				vi++;
				if(pNUEvent->mbForceBUMCheck){
					// включаем у viewport-а флаг более частой проверки объекта под мышкой
					if(mBUM.mpVisible){
						mlRMMLElement* pMLElViewport = mBUM.mpVisible->GetElem_mlRMMLVisible();
						if(pMLElViewport->mRMMLType == MLMT_VIEWPORT){
							mlRMMLViewport* pViewport = (mlRMMLViewport*)pMLElViewport;
							pViewport->SetTraceObjectOnMouseMove(true);
						}
					}
				}
			}
		}
		//
		mEventQueueUpdateID = pInp->IncrementUpdateID();
		bool bNoEvents = true;
		CVectorID* pNewObjIDs = NULL;
		unsigned int iEventHandlingTimeQuota = GetUpdateEventHandlingTimeQuota();
		unsigned long ulEventHandlingBeginTime = GPSM(cx)->GetContext()->mpApp->GetTimeStamp();
		for(int iEventCount = 0; ; iEventCount++){
			omsUserEvent Event = pInp->GetEvent();
			if(Event.ev == 0) break;
			// в режиме обработки новых событий на следующем update-е
			if(mbNewEventsToNextUpdateMode){
				// если пришло новое событие, 
				if(Event.update_id == mEventQueueUpdateID){
					pInp->AddEvent(Event, true);	// то возвращаем событие в очередь
					bEventHandlingBreaked = true;
					break; // и прерываем обработку событий
				}
			}
//if(!(Event.ev & 0x8000))
//if(Event.ev != omsUserEvent::MOUSEMOVE)
//TRACE("Event: %x\n",Event.ev);
			mbDontSendSynchEvents = false;
			bNoEvents = false;

			if(Event.ev >= omsUserEvent::MOUSEMOVE && Event.ev <= omsUserEvent::MOUSEWHEEL){
				if(Event.ev == omsUserEvent::MOUSEMOVE){
					{MI_WITH_ALL_MOUSEMOVE_STORE;} // прост чтобы mi.h правильный был
					mvSkipedMouseMoves.clear();
					if(Event.script != NULL){
						int iSkipedMouseMoveCnt = ((Event.key_code) & 0xFFFF);
						int iArrayLength = iSkipedMouseMoveCnt * 2;
						mvSkipedMouseMoves.reserve(iArrayLength);
						for(int i = 0; i < iArrayLength; i++){
							mvSkipedMouseMoves.push_back(Event.script[i]);
						}
						mpContext->mpInput->FreeEventData(Event);
					}
					Event.key_code = 0; // don't get button element under mouse
					if((alTime-alLastInpEvTime) > MMCHKBUTDELAYMS*1000){ // через каждые 164 ms
						alLastInpEvTime = alTime;
						Event.key_code = 1; // get button element under mouse
					}
				}else{
					alLastInpEvTime = alTime;
					if(Event.ev != omsUserEvent::MOUSEWHEEL)
						Event.key_code = 1; // get button element under mouse
				}
				HandleMouseEvent(Event);
			}else if(Event.ev >= omsUserEvent::KEYDOWN && Event.ev <= omsUserEvent::KEYUP){
				HandleKeyEvent(Event);
			}else if(Event.ev == omsUserEvent::EXECSCRIPT){
				HandleScriptEvent(Event);
				mpContext->mpInput->FreeEventData(Event);
			}else if(Event.ev == omsUserEvent::SYNCH){
				if (pNewObjIDs == NULL)
					pNewObjIDs = MP_NEW( CVectorID);
				//HandleSynchEvent2(Event, mulSynchEventsGlobalTime, *pNewObjIDs);
				mpContext->mpInput->FreeEventData(Event);
			}else if(Event.ev == omsUserEvent::SETMODE){
				if( meMode != smmLoading)
					SetMode((mlEModes)Event.key_code);
			}else if(Event.ev == omsUserEvent::GETSTATE){
				void* pData=NULL;
				unsigned long lSize=0;
				GetState(pData,lSize);
				mpContext->mpSynch->NotifyGetState((unsigned char*)pData,lSize);
			}else if(Event.ev==omsUserEvent::SETSTATE){
				SetState((void*)Event.script,Event.key_code);
				mpContext->mpInput->FreeEventData(Event);
			}else if(Event.ev==omsUserEvent::RELOADSCENE){
				LoadScene(NULL);
			}else if(Event.ev == omsUserEvent::CUSTOM){
				HandleCustomEvent(Event);
				mpContext->mpInput->FreeEventData(Event);
			}else if((Event.ev&0xFF00) == omsUserEvent::INTERNAL){
				// tandy: не помню зачем было надо (22.03.06)
				// но для второй модели синхронизации мешает
//				if(meMode != smmInitilization && meMode != smmNormal){
//					mpContext->mpInput->AddEvent(Event);
//					mpContext->mpInput->SetStopper();
//				}else{
					HandleInternalEvent(Event);
					mpContext->mpInput->FreeEventData(Event);
					if(NeedToUpdateRMNow(alTime, aiDeltaTime, isNormalMode)){
						mbUpdating=false;
						return OMS_OK;
					}
//				}
			}else if((Event.ev&0xFF00)==omsUserEvent::GETDEBUGINFO){
				HandleGetDebugInfoEvent(Event);
				mpContext->mpInput->FreeEventData(Event);
			}
			if(iEventCount > 3){
				unsigned long ulEventHandlingTotalTime = GPSM(cx)->GetContext()->mpApp->GetTimeStamp() - ulEventHandlingBeginTime;
				if(ulEventHandlingTotalTime > iEventHandlingTimeQuota){
					mlTrace(cx, "[RMML] per-update event handling overtime: %d ms (%d events)\n", (ulEventHandlingTotalTime), iEventCount);
					bEventHandlingBreaked = true;
					break;
				}
			}
		}
		if (pNewObjIDs != NULL)
		{
			if (mpContext != NULL && mpContext->mpMapMan != NULL && pNewObjIDs->size() > 0)
			{
#if defined(_MSC_VER) && (_MSC_VER >= 1600 )
				cm::cmObjectIDs *p = pNewObjIDs->begin()._Ptr;
#else
				cm::cmObjectIDs *p = pNewObjIDs->begin()._Myptr;
#endif
				mpContext->mpMapMan->CreateObjects( p, pNewObjIDs->size());
			}
			 MP_DELETE( pNewObjIDs);
		}

		if(bNoEvents){
			// время от времени вызываем MouseMove 
			// чтобы вид курсора соответствовал состоянию зоны,
			// на которой находится кусор
			if((alTime-alLastInpEvTime)>MMCHKBUTDELAYMS*1000){ // через каждые 164 ms
				alLastInpEvTime=alTime;
				omsUserEvent Event;
				Event.ev=omsUserEvent::MOUSEMOVE;
				Event.mousexy.mouse_x=mMouseXY.x;
				Event.mousexy.mouse_y=mMouseXY.y;
				Event.key_code=1; // get button element under mouse
				HandleMouseEvent(Event);
			}
		}
	}
	// проверяет вектор нажатых клавиш на наличие "просроченных" и если они на самом деле уже отжаты, 
	// то генерирует для них onKeyUp
	CheckPressedKeys();
	//
	if(!bEventHandlingBreaked) // если дообработка событий переносится на следующий Update, то не надо чистить этот вектор, иначе падать будет
		mvDeletedInUpdate.clear();
	if(bEventHandlingBreaked)
		muStructChangedUpdCntr++;
	typedef std::vector<mlRMMLElement*> v_elems_s;
	v_elems_s vElements;
	vElements.reserve(mvNewElements.size());
	v_elems::iterator vi = mvNewElements.begin();
	v_elems::iterator viEnd = mvNewElements.end();
	for(; vi != viEnd; ++vi){
		mlRMMLElement* pElem = *vi;
		if(pElem == NULL)
			continue;
		if(pElem->IsClassChildren())
			continue;
		mlMedia* pParentScene = pElem->GetScene();
		if(pParentScene == NULL){
			vElements.push_back(pElem);
/*
if(pElem->mpParent == NULL){
mlString wsType = cLPWCSTR(mlRMML::GetTypeName(pElem->GetType()).c_str());
mlString wsName = pElem->GetName();
mlString wsClass;
if(pElem->mpClass != NULL){
if(isEqual(pElem->mpClass->mpName, L"CUserRightsListItem")){
int jj=0;
}
wsClass = pElem->mpClass->GetName();
//mlTrace(cx, "mvNewElements root item class: %s\n", cLPCSTR(pElem->mpClass->GetName()));
}
int hh=0;
mlString wsSrc;
const wchar_t* pwcSrc = mSrcFileColl[pElem->miSrcFilesIdx];
if(pwcSrc != NULL) wsSrc = pwcSrc;
mlTrace(cx, "mvNewElements root item: type: %s, name: %s, class: %s, src: %s, line: %d\n", cLPCSTR(wsType.c_str()), cLPCSTR(wsName.c_str()), cLPCSTR(wsClass.c_str()), cLPCSTR(wsSrc.c_str()), pElem->miSrcLine);
//mlTrace(cx, "mvNewElements root item refs begin:\n");
//TraceRefs(cx, JS_GetGlobalObject(cx), pElem->mjso);
//mlTrace(cx, "mvNewElements root item refs end.\n");
}
*/
			continue;
		}
//if(pElem->_name && isEqual(pElem->mpName, L"sqvu_cveto4ek_ekran09"))
//mvDeletedInUpdate.clear();
		// добавляем в список обновляемых элементов независимо 
		// от того, является ли он обновляемым.
		// Если он на самом деле не обновляемый, то он удалится из списка после первого вызова Update
		AddUpdatableElement(pElem, true);	// секвенсеры могут попасть в список еще до этого момента, поэтому true
		// если надо, добавляем в список синхронизируемых объектов
		AddSynchedElement(pElem, true);	// дочерние синхронизируемые объекты добавляют своего родителя, поэтому надо проверять, может он уже в списке
		// считаем статистику
		if(pParentScene == mpUI){
			muiUIElementCount++;
		}else{
			muiSceneElementCount++;
		}
	}
	mvNewElements.clear();
	mvNewElements.reserve(vElements.size());
	vi = vElements.begin();
	viEnd = vElements.end();
	for(; vi != viEnd; ++vi){
		mvNewElements.push_back(*vi);
	}
//mlTrace(cx, "mvNewElements.size()=%d\n",mvNewElements.size());

	// обновляем состояние всех элементов UI
		// через каждые 5 минут обновляем список обновляемых элементов UI
	#ifdef ML_FULL_LISTS_UPDATE
	if(IsSceneStructChanged() || (alTime - mlLastUIUpdatableListBuildingTime) > 5*60*1000*1000){
		int iLastUIUpdatableObjectCount = mvUpdatableUIElements.size();
		mvUpdatableUIElements.clear();
		mvUpdatableUIElements.reserve(iLastUIUpdatableObjectCount + 10);
		mlRMMLElement* pElem = NULL;
		mlRMMLIterator IterUI(cx,mpUI);
		while((pElem=IterUI.GetNext())!=NULL){
			if(pElem->Update(alTime) != ML_ERROR_NOT_IMPLEMENTED){
				mvUpdatableUIElements.push_back(pElem);
			}
		}
		mlLastUIUpdatableListBuildingTime = alTime;
	}else{
	#else
	if(true){
	#endif
		mbUpdatableElementsChanged = false;
		int iSrcSize = mvUpdatableUIElements.size();
		v_elems::iterator vi = mvUpdatableUIElements.begin();
//TRACE(cx, "mvUpdatableUIElements.size()=%d\n",mvUpdatableUIElements.size());
		for(; vi != mvUpdatableUIElements.end(); vi++){
			mlRMMLElement* pElem = *vi;
			if(pElem != NULL){
				mlresult res = pElem->Update(alTime);
				//if(mvUpdatableUIElements.size() != iSrcSize && *vi != pElem){
				if(mbUpdatableElementsChanged){
					mbUpdatableElementsChanged = false;
					// и похоже буфер перестроился, то находим только что обновленный элемент заново
					vi = find(mvUpdatableUIElements.begin(), mvUpdatableUIElements.end(), pElem);
					if(vi == mvUpdatableUIElements.end())
						break;
				}
				// если не нужно обновлять в будущем, то исключаем его из списка обновляемых элементов
				if (res == ML_ERROR_NOT_IMPLEMENTED && vi != mvUpdatableUIElements.end())
					*vi = NULL;
			}
		}
	}
#ifndef CSCL
	list<shared_ptr<Updatable> >::iterator i = _updbl.begin(), iend = _updbl.end();
	for (; i != iend; i++){
		(*i)->update(alTime);
	}
#endif
	if(meMode!=smmLoading){
		// обновляем состояние всех элементов сцены
		UpdateAllElements(alTime, isNormalMode);
	}else{
		// motions и blends загрузились до загрузки элементов
		if (/*IsMainModuleLoaded() && */mpContext->mpRM->IsMotionPreloadedEnd() && mpContext->mpRM->IsBlendsPreloadedEnd()) {
			// загружаем очередную порцию элементов
	//mlTrace(cx, "muElToLoadCnt=%d\n", muElToLoadCnt);
			if(muElLoadedCnt < muElToLoadCnt){
				// решаем, сколько элементов загрузить 
				unsigned int iElToLoadNowCnt = muElToLoadCnt / 100; // 1% от общего числа объектов, которые надо загрузить
				if(iElToLoadNowCnt < 10)
					iElToLoadNowCnt = 10; // пока будем грузить по 10 элементов за раз как минимум
				if((muElToLoadCnt-muElLoadedCnt) < iElToLoadNowCnt)
					iElToLoadNowCnt=muElToLoadCnt-muElLoadedCnt+1;
				if(mpScene!=NULL){
					mlRMMLElement::mlCreateAndLoadMOsInfo oCLMOsInfo(iElToLoadNowCnt);
					oCLMOsInfo.miTimeQuota = 40; // ms (надо как-то более гибко определять)
					mpScene->CreateAndLoadMOs(&oCLMOsInfo);
	//mlTrace(cx, "oCLMOsInfo.miLoaded=%d\n", oCLMOsInfo.miLoaded);
					if(oCLMOsInfo.miLoaded == 0){
						// все что можно загрузили уже
						muElLoadedCnt = muElToLoadCnt;
					}else
						muElLoadedCnt += oCLMOsInfo.miLoaded;
	//mlTrace(cx, "muElLoadedCnt=%d\n", muElLoadedCnt);
				}else{
					mlTrace(cx, "Scene elements creation and loading is aborted.\n");
					muElLoadedCnt = muElToLoadCnt;
				}
				if(muElLoadedCnt > muElToLoadCnt) muElLoadedCnt=muElToLoadCnt;
				SetLoadingProgress((1.0*muElLoadedCnt)/muElToLoadCnt);
			}
			mlRMMLComposition* pUI = mpUI;
			JSIdArray* pGPropIDArr = JS_Enumerate(cx, mpUI->mjso);
			if(muElLoadedCnt >= muElToLoadCnt){
				// все загрузили
				if(mpScene!=NULL){
					mpScene->CreateAndLoadMOs(); // если кому-то флаг "создан" не выставили, выставляем
	//				mpScene->mpMO = PMOV_CREATED;
					mlTrace(cx, "Done. (Creating elements...)\n");
					mpScene->GetIVisible()->Show();
					if(!mbWasAuthoringMode){
						mlTrace(cx, "Executing and activating scripts...\n");
						mpScene->ActivateScripts();
						mlTrace(cx, "Done. (Executing and activating scripts...)\n");
						mlSynchData oData;
						oData << 4;
						GPSM(cx)->GetContext()->mpInput->AddCustomEvent(CEID_onShowInfoState, (unsigned char*)oData.data(), oData.size());
						// запускаем логмэн
						if(mpScene != NULL)
							mlString msSceneName = mpScene->GetName();
					}
				}else{
					mlTrace(cx, "Elements of scene was not created.\n");
				}
				for(int ikl=0; ikl<pGPropIDArr->length; ikl++){
					jsid id=pGPropIDArr->vector[ikl];
					jsval v;
					if(!JS_IdToValue(cx,id,&v)) continue;
					if(!JSVAL_IS_STRING(v)) continue;
					JSString* jss = JSVAL_TO_STRING(v);
					jschar* jsc=JS_GetStringChars(JSVAL_TO_STRING(v));
					int hh=0;
				//		jsval vProp;
				//		JS_GetUCProperty(mcx, jsoClasses, jsc, -1, &vProp);
				//		if(!JSVAL_IS_REAL_OBJECT(vProp)) continue;
				//		if(JSVAL_TO_OBJECT(vProp) == apMLElClass->mjso)
				//			return jsc;
				}
				JS_DestroyIdArray(cx, pGPropIDArr);
				/*if (mpContext->mpLogWriter)
					mpContext->mpLogWriter->WriteLnLPCSTR( "[RMML]: Garbage collector");*/
				JS_GC(cx);
	//			if(meMode!=smmAuthoring)
				SetMode(smmNormal);
			}
		}
	}
	CallGCIfNeeded(alTime);
//	if(meMode == smmInitilization && miNextUpdateDeltaTime > 0){
//		aiDeltaTime = miNextUpdateDeltaTime;
//	}
	//
	// если было переключение с режима инициализации(воспроизведения?) в нормальный режим
	if(aiDeltaTime < 0 && miLastUpdateDeltaTime > 0){
		// то надо будет уведомить элементы о том, что сменилось глобальное время
		mbNeedToShiftGlobalTime = true;
	}
	mlLastUpdateTime = alTime;
	miLastUpdateDeltaTime = aiDeltaTime;
	mbUpdating=false;
	return OMS_OK;
}

void mlSceneManager::handleGamepadButton(WORD button, WORD newButtons) {
	unsigned int evType;
	if (newButtons & button) { //нажата
		m_pressedButton = button;
		evType = omsUserEvent::GAMEPAD_KEYDOWN;
	} else
	if (m_prevButtons & button) { //отпущена
		m_pressedButton = button;
		evType = omsUserEvent::GAMEPAD_KEYUP;
	} else
		return; //без изменений

	omsUserEvent ctrlEvent(evType, 0, 0);
	HandleKeyEvent(ctrlEvent);
}

void mlSceneManager::HandleThumb(short &curState, short &thumb, short newState)
{
	unsigned int evType;
	if (curState != ThumbState::NEUTRAL)
	{
		thumb = curState;
		evType = omsUserEvent::GAMEPAD_KEYUP;
		omsUserEvent ctrlEvent(evType, 0, 0);
		HandleKeyEvent(ctrlEvent);
	}

	curState = newState;
	if (newState != ThumbState::NEUTRAL)
	{
		thumb = newState;
		evType = omsUserEvent::GAMEPAD_KEYDOWN;
		omsUserEvent ctrlEvent(evType, 0, 0);
		HandleKeyEvent(ctrlEvent);
	}
	else
	{
		thumb = ThumbState::NONE;
	}
}

// вызвать сборщик мусора
// если abAsynch == true, то по возможности в асинхроннном режиме
void mlSceneManager::CallJS_GC(bool abAsynch, bool abForceCall){
#ifdef JS_THREADSAFE
	if(abAsynch){
		if(mpUtilityThread.get() != NULL){
			if(mpUtilityThread->StartGC())
				return;
		}
	}
#endif
	//JS_GC(cx);
	if (abForceCall)
		JS_GC(cx);
	else
		JS_MaybeGC(cx);
}

void mlSceneManager::UpdateNextUpdateDeltaTime(int aiDeltaTime){
	if(aiDeltaTime <= 0) return;
	if(meMode == smmNormal) return; // added 11.04.06
//#ifdef MLD_TRACE_INIT_EVENTS
//if(gpSM->GetMode()==smmInitilization){
//TRACE("---SynchInitInfo--- GT:%d\n", );
//}
//#endif
	mlNUDTime.InsertDTime(aiDeltaTime);
//TRACE("Inserted ");
//DumpNUDTime(mlNUDTime);
//mlNUDTime.InsertDTime(aiDeltaTime+1);
//	if(miNextUpdateDeltaTime < 0 || aiDeltaTime < miNextUpdateDeltaTime)
//		miNextUpdateDeltaTime = aiDeltaTime;
}

omsresult mlSceneManager::AuthoringUpdate(){
	omsIInput* pInp=mpContext->mpInput;
	if(pInp==NULL) return OMS_ERROR_FAILURE;
	bool bNoEvents=true;
	for(;;){
		omsUserEvent Event=pInp->GetEvent();
		if(Event.ev==0){ break;
		}else if((Event.ev&0xFF00)==omsUserEvent::GETDEBUGINFO){
			HandleGetDebugInfoEvent(Event);
			mpContext->mpInput->FreeEventData(Event);
		}
	}
	return OMS_OK;
}

mlRMMLElement* mlSceneManager::CreateElementByType(int aiType){
	JSObject* jsoNewElem=mRMML.CreateElement(cx,"",aiType);
	if(jsoNewElem==NULL) return NULL;
	return (mlRMMLElement*)JS_GetPrivate(cx,jsoNewElem);
}

mlresult mlSceneManager::SaveXML(const wchar_t* asSrc){
	if(meMode!=smmAuthoring) return ML_ERROR_FAILURE;

	std::auto_ptr<mlRMMLBuilder> pBuilder(new mlRMMLBuilder(cx));

	mlTrace(cx, "Saving \'%S\'...\n",asSrc);
	mlXMLArchive* pXMLArchive=NULL;
	if(ML_FAILED(OpenXMLArchive2Write(asSrc,pXMLArchive)))
		return ML_ERROR_NOT_AVAILABLE;
	pBuilder->Save(pXMLArchive,this);
	CloseXMLArchive(pXMLArchive);
	mlTrace(cx, "Saved \'%S\'\n",asSrc);
	return ML_OK;
}

/**
 * 
 */ 

void mlSceneManager::RemoveSceneElemsRefs(JSObject* obj, v_jso* apLookedup){
	bool bLookedupWasNULL = apLookedup==NULL;
	if(apLookedup == NULL){
		apLookedup = MP_NEW( v_jso);
	}
	JSIdArray* pGPropIDArr = JS_Enumerate(cx, obj);
	for(int ikl=0; ikl<pGPropIDArr->length; ikl++){
		jsid id=pGPropIDArr->vector[ikl];
		jsval v;
		if(!JS_IdToValue(cx,id,&v)) continue;
		jsval vProp = JSVAL_NULL;
		jschar* jsc = NULL;
		if(JSVAL_IS_STRING(v)){
			jsc = JS_GetStringChars(JSVAL_TO_STRING(v));
			unsigned int attrs;
			JSBool found;
			if (JS_GetUCPropertyAttributes(cx, obj, jsc, -1, &attrs, &found) &&
				found == JS_TRUE && (attrs & JSPROP_READONLY)
			)
				continue;
			JS_GetUCProperty(cx, obj, jsc, -1, &vProp);
		}else if(JSVAL_IS_INT(v)){
			// элемент массива
			JS_GetElement(cx, obj, JSVAL_TO_INT(v), &vProp);
		}else continue;
		if(!JSVAL_IS_REAL_OBJECT(vProp)) continue;
		JSObject* jso = JSVAL_TO_OBJECT(vProp);
		if(JSO_IS_MLEL(cx, jso)){
			mlRMMLElement* pMLEL = GET_RMMLEL(cx, jso);
			if(pMLEL->GetScene() != GetActiveScene()) continue;
			vProp = JSVAL_NULL;
			if(JSVAL_IS_STRING(v)){
				jschar* jsc = JS_GetStringChars(JSVAL_TO_STRING(v));
				JS_SetUCProperty(cx, obj, jsc, -1, &vProp);
			}else if(JSVAL_IS_INT(v)){
				JS_SetElement(cx, obj, JSVAL_TO_INT(v), &vProp);
			}
		}else{
			if(apLookedup != NULL){
				v_jso::iterator vi;
				for(vi = apLookedup->begin(); vi != apLookedup->end(); vi++){
					if(*vi == jso)
						break;
				}
				if(vi != apLookedup->end())
					continue;
				apLookedup->push_back(obj);
			}
			RemoveSceneElemsRefs(jso, apLookedup);
		}
	}
	JS_DestroyIdArray(cx, pGPropIDArr);
	if(bLookedupWasNULL && apLookedup != NULL)
		 MP_DELETE( apLookedup);
}

void mlSceneManager::ResetEvent(){
	// 
	if(mjsoEvent == NULL) return;
	JSIdArray* pPropIDArr = JS_Enumerate(cx, mjsoEvent);
	for(int ikl = 0; ikl < pPropIDArr->length; ikl++){
		jsid id=pPropIDArr->vector[ikl];
		jsval v;
		if(!JS_IdToValue(cx,id,&v)) continue;
		if(!JSVAL_IS_STRING(v)) continue;
		jschar* jsc=JS_GetStringChars(JSVAL_TO_STRING(v));
		JS_DeleteUCProperty2(cx,mjsoEvent,jsc,-1,&v);
	}
	JS_DestroyIdArray(cx, pPropIDArr);
}

void mlSceneManager::SetScene(mlRMMLComposition* apNewScene){
TRACE(cx, "Begin SetScene (%d)\n", GetContext()->mpApp->GetTimeStamp() - mStartLoadSceneTime);
unsigned long ulTime = GetContext()->mpApp->GetTimeStamp();
	if(mpScene == apNewScene) return;
	// сбрасываем все ссылки на элементы предыдущей сцены
	SetFocus(NULL);
	ResetMouse(NULL);
	ResetKey(false);
//	ResetModule();
	ResetPlayer();
//	ResetEvent(); // очищаем глобальный объект Event
	if(mpScene != NULL)
		NotifyUnloadScene();

	SceneStructChanged();
	// очищаем все временные глобальные переменные, 
	// созданные во время работы скриптов старой сцены
	JSObject* glob = JS_GetGlobalObject(cx);
	JSIdArray* pGPropIDArr = JS_Enumerate(cx, JS_GetGlobalObject(cx));
	for(int ikl=0; ikl<pGPropIDArr->length; ikl++){
		jsid id=pGPropIDArr->vector[ikl];
		jsval v;
		if(!JS_IdToValue(cx,id,&v)) continue;
		if(!JSVAL_IS_STRING(v)) continue;
		jschar* jsc=JS_GetStringChars(JSVAL_TO_STRING(v));
		uintN attrs = 0;
		JSBool found = JS_FALSE;
		if(JS_GetUCPropertyAttributes(cx, glob, jsc, -1, &attrs, &found)){
			// если объект глобальный
			if(found && (attrs & JSPROP_PERMANENT) && (attrs & JSPROP_READONLY)){
				jsval vGlobProp = JSVAL_NULL;
				JS_GetUCProperty(cx, glob, jsc, -1, &vGlobProp);
				if(JSVAL_IS_REAL_OBJECT(vGlobProp))
					RemoveSceneElemsRefs(JSVAL_TO_OBJECT(vGlobProp));
			}
		}
		if(!isEqual((const wchar_t*)jsc,GJSOWNM_SCENE)){
			JS_DeleteUCProperty2(cx,glob,jsc,-1,&v);
		}
	}
	JS_DestroyIdArray(cx, pGPropIDArr);
	// устанавливаем/удаляем глобальное свойство 'scene'
	if(apNewScene==NULL){
//jsval v;
//JS_GetProperty(cx, JS_GetGlobalObject(cx), GJSONM_SCENE, &v);
//JSObject* jsoOldScene=JSVAL_TO_OBJECT(v);
		// удаляем глобальное свойство 'scene'
		JS_DeleteProperty(cx, JS_GetGlobalObject(cx), GJSONM_SCENE);
//JSObject* jsoPar=NULL;
//jschar* jscName=NULL;
//if(FindJSORef(cx,JS_GetGlobalObject(cx),jsoOldScene,jsoPar,jscName)){
//int hh=0;
//}
//JS_GetProperty(cx, JS_GetGlobalObject(cx), GJSONM_SCENE, &v);
//int hh=0; 
		// msSceneXML=L"";
	}else{
		// определяем глобальное свойство 'scene'
		UpdateNotInScenesElems(apNewScene, true);
		jsval vs=OBJECT_TO_JSVAL(apNewScene->mjso);
		JS_DefineProperty(cx, JS_GetGlobalObject(cx), GJSONM_SCENE, vs, NULL, NULL, 
			JSPROP_ENUMERATE | JSPROP_READONLY);
//if(mpScene){
//mlRMMLElement* pBg=mpScene->GetChild(L"bg");
//if(pBg){
//JSObject* jsoBg=pBg->mjso;
//JSObject* jsoPar=NULL;
//jschar* jscName=NULL;
//if(FindJSORef(cx,JS_GetGlobalObject(cx),jsoBg,jsoPar,jscName)){
//int hh=0;
//}
//}
//}
	}
	mbNeedToCallGC=true;
#ifndef JS_THREADSAFE
if(mpScene != NULL)
JS_SetContextPrivate(cx, mpScene->mjso);
#endif

//jsval v;
//JS_GetProperty(cx, mjsoClasses, "ui_COrderedCompositionBase", &v);
//if(JSVAL_IS_REAL_OBJECT(v)){
//jsval vParent;
//JS_GetProperty(cx, JSVAL_TO_OBJECT(v), "_parent", &vParent);
//int hh=0;
////if(JSVAL_IS_REAL_OBJECT(vParent)){
////	JSObject* jsoParent = JSVAL_TO_OBJECT(vParent);
////	JSClass* pParentClass = JS_GetClass(jsoParent);
////	int hh=0;
////}
//JSObject* jsoProto = JS_GetPrototype(cx, JSVAL_TO_OBJECT(v));
//if(JSO_IS_MLEL(jsoProto)){
//	JSClass* pClass = JS_GetClass(jsoProto);
//	JSIdArray* idArr=JS_Enumerate(cx, jsoProto);
//	for(int ij=0; ij<idArr->length; ij++){
//		jsid id=idArr->vector[ij]; jsval v;
//		if(!JS_IdToValue(cx,id,&v)) continue;
//		if(!JSVAL_IS_STRING(v)) continue;
//		JSString* jss = JSVAL_TO_STRING(v);
//		jschar* jsc = JS_GetStringChars(jss);
//		int hh=0;
//	}
//	mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, jsoProto);
//	jsval vName;
//	JS_GetProperty(cx, jsoProto, "_name", &vName);
//	if(JSVAL_IS_STRING(vName)){
//		JSString* jssName = JSVAL_TO_STRING(vName);
//		jschar* jsc = JS_GetStringChars(jssName);
//		int hh=0;
//	}
//	jsval vParent;
//	JS_GetProperty(cx, jsoProto, "parent", &vParent);
//	if(JSVAL_IS_REAL_OBJECT(vParent)){
//		JSObject* jsoParent = JSVAL_TO_OBJECT(vParent);
//		JSClass* pParentClass = JS_GetClass(jsoParent);
//		int hh=0;
//	}
//	int hh=0;
//}
//
//}
	// чистим ссылки объектов сцены на другие объекты (для исправления бага с неудалением сцены из-за intersections)
	// реально ClearRefs() сейчас реализован только у 3D-сцен (вызывает очистку intersections у каждого 3D-объекта)
	if(mpScene != NULL){
		mlRMMLElement* pElem;
		mlRMMLIterator IterScn(cx,mpScene);
mlTrace(cx, "ClearRefs-iterator");
		while((pElem=IterScn.GetNext())!=NULL)
			pElem->ClearRefs();
	}
	// устанавливаем новую активную сцену
//CheckBug();
//if(mpScene != NULL)
//JS_SetContextPrivate(cx, mpScene->mjso);
	
	mpPreviousScene = NULL;
	if( mpScene)
	{
		jsval v;
		JS_GetProperty( cx, mpScene->mjso, "wndInstruction", &v);
		if( JSVAL_IS_REAL_OBJECT(v))
			mpPreviousScene = JSVAL_TO_OBJECT( v);
	}
	// scene.wndInstruction

	mpScene=apNewScene;
	// удаляем старую сцену и все ее объекты
	ResetMouseTarget();
//TRACE(cx, "SetScene without GC (%d %d)\n", GetContext()->mpApp->GetTimeStamp() - mStartLoadSceneTime, GetContext()->mpApp->GetTimeStamp() - ulTime);
unsigned long ulTimeGC = GetContext()->mpApp->GetTimeStamp();

	/*if (mpContext->mpLogWriter)
		mpContext->mpLogWriter->WriteLnLPCSTR( "[RMML]: Garbage collector");*/
	//JS_GC(cx);
	JS_MaybeGC(cx);
	//mbNeedToCallGC = false;
	// удалить из очереди событий все события синхронизации
	omsIInput* pInp = mpContext->mpInput;
	if(pInp != NULL){
		pInp->RemoveAllSynchEvents();
	}
//TRACE(cx, "  JS_GC (%d %d)\n", GetContext()->mpApp->GetTimeStamp() - mStartLoadSceneTime, GetContext()->mpApp->GetTimeStamp() - ulTimeGC);
	// старая сцена удалится автоматически при сборщиком мусора
	// который вызывается в Builder'е при чтении XML-файлов
//TRACE(cx, "End SetScene (%d)\n", GetContext()->mpApp->GetTimeStamp() - mStartLoadSceneTime);
}

omsresult mlSceneManager::ExecScript(const wchar_t* apwcScript, int aiLength, wchar_t* *appwcRetVal){
	if(mbDestroying) return OMS_ERROR_FAILURE;
	GUARD_JS_CALLS(cx);
	if(apwcScript==NULL) return OMS_ERROR_INVALID_ARG;
	if(appwcRetVal!=NULL){
		unsigned int uLength=aiLength;
		if(aiLength < 0)
			uLength=wcslen(apwcScript);
		jsval v;
		JSBool bR=wr_JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), apwcScript, uLength, "execScript", 1, &v);
		if(bR==JS_TRUE){
			*appwcRetVal=wr_JS_GetStringChars(JS_ValueToString(cx,v));
		}
	}else{
		mpContext->mpInput->AddExecScript((const jschar*)apwcScript,aiLength);
	}
	return OMS_OK;
}

#define ML_DBG_TRACE_MODE_CH 0

omsresult mlSceneManager::SetMode(mlEModes aeMode){
	if(aeMode == meMode) return OMS_OK;
	mePrevMode = meMode;
	if(aeMode == smmInitilization){
		// не будем обрабативать никакие события от мыши и клавиатуры
		// Update только на UI
		// ??
		mlNUDTime.Reset();
		mulSynchEventsGlobalTime = 0;
		meMode = aeMode;
		#ifdef ML_DBG_TRACE_MODE_CH
		mlTrace(cx, "Mode changed: Initialization\n");
		#endif
	}else if(aeMode == smmLoading){
		if(meMode != smmInitilization && meMode != smmNormal && meMode != smmAuthoring)
			return OMS_ERROR_FAILURE;
		if(meMode == smmAuthoring)
			mbWasAuthoringMode = true;
		meMode = aeMode;
		#ifdef ML_DBG_TRACE_MODE_CH
		mlTrace(cx, "Mode changed: Loading\n");
		#endif
	}else if(aeMode==smmNormal){
		mlNUDTime.Reset(); // added 11.04.06
		if(meMode==smmLoading && mbWasAuthoringMode){
			meMode=smmAuthoring;
			if(mpScene!=NULL) mpScene->Freeze();
		}else{
			if(meMode==smmPause || meMode==smmAuthoring){
				// запускаем динамические объекты
				if(mpScene!=NULL) mpScene->Unfreeze();
			}
			if(meMode==smmInitilization){
				mlDontSendSynchEvents oDSSE(this);
				mbDontSendSynchEvents = false; // чтобы не отсекало вызовы 1 -> 0
				NotifyPlayerOnRestoreUnsynchedListeners();
			}
			meMode=aeMode;
			#ifdef ML_DBG_TRACE_MODE_CH
			mlTrace(cx, "Mode changed: Normal\n");
			#endif
		}
		SceneStructChanged();
	}else if(aeMode==smmPlay){
		if(meMode==smmPause){
			// запускаем динамические объекты
			if(mpScene!=NULL) mpScene->Unfreeze();
		}
		// режим воспроизведения:
		// не будем обрабативать никакие события от мыши и клавиатуры
		// ??
		// если остались какие-нибудь внутренние события
		// то включаем временную проверку ...
		SceneStructChanged();
		meMode=aeMode;
		#ifdef ML_DBG_TRACE_MODE_CH
		mlTrace(cx, "Mode changed: Play\n");
		#endif
	}else if(aeMode==smmPause){
		if(meMode!=smmNormal && meMode!=smmPlay)
			return OMS_ERROR_FAILURE;
		// приостанавливаем все динамические объекты
		// (animation, audio, video, sequencer, character, object)
//		if(mpScene!=NULL) mpScene->Freeze();
		meMode=aeMode;
		#ifdef ML_DBG_TRACE_MODE_CH
		mlTrace(cx, "Mode changed: Pause\n");
		#endif
	}else if(aeMode==smmAuthoring){
		if(mpScene!=NULL) mpScene->Freeze();
		meMode=aeMode;
//		if(mpAuthScene == NULL)
//			LoadAuthScene();
		#ifdef ML_DBG_TRACE_MODE_CH
		mlTrace(cx, "Mode changed: Authoring\n");
		#endif
	}else if(aeMode==smmDisconnection){
		NotifyPlayerOnDisconnectListeners();
	}
	jsval v=INT_TO_JSVAL((int)meMode);
	JS_SetProperty(cx,mjsoPlayer,"mode",&v);
	/*if (mpContext->mpLogWriter)
		mpContext->mpLogWriter->WriteLnLPCSTR( "[RMML]: Garbage collector");*/
	//JS_GC(cx);
	JS_MaybeGC(cx);
	return OMS_OK;
}

void mlSceneManager::SetTestMode(mlETestModes aeTestMode)
{
	if(aeTestMode == meTestMode) return;
	meTestMode = aeTestMode;	
}

void mlSceneManager::SetTestResDir(const wchar_t* apwсTestResDir)
{
	if(apwсTestResDir != NULL)
	{
		msTestResDir = apwсTestResDir;
		if( msTestResDir[msTestResDir.size() - 1] != '\\' && msTestResDir[msTestResDir.size() - 1] != '/')
			msTestResDir += '\\';
		int pos = -1;
		while( std::wstring::npos != (pos = msTestResDir.find(L"\"")))
		{
			msTestResDir.replace( pos, 1, L"");
		}
	}
}

// получить директорию тестовых ресурсов
bool mlSceneManager::GetTestResDir( wchar_t* apwсTestResDir, int aSizeTestResDir)
{
	if(apwсTestResDir == NULL || aSizeTestResDir < 2) return false;
	int copyCharectersCount = msTestResDir.size();
	if((int)msTestResDir.size() >= aSizeTestResDir)
		copyCharectersCount = aSizeTestResDir-1;
	wcsncpy_s( apwсTestResDir, aSizeTestResDir, msTestResDir.c_str(), copyCharectersCount);
	apwсTestResDir[aSizeTestResDir] = L'\0';
	return true;
}

// взять состояние сцены
omsresult mlSceneManager::GetState(void* &apData, unsigned long &aSize){
TRACE(cx, "GetState\n");
	msdLastState.reset();
	// сохранить состояние глобальных объектов и переменных
	if(mSynchMode == synchMode2)
		msdLastState.setJSOutMode(mlSynchData::JSOM_STATE2);
	else
		msdLastState.setJSOutMode(mlSynchData::JSOM_STATE);
	msdLastState << cx;
	apData=msdLastState.data();
	aSize=msdLastState.size();
//SetScene(NULL);
//SetState(apData,aSize);
//jsval vScene;
//JS_GetProperty(cx,JS_GetGlobalObject(cx),GJSONM_SCENE,&vScene);
//if(JSVAL_IS_OBJECT(vScene)){
//JSObject* jsoScene=JSVAL_TO_OBJECT(vScene);
//if(JS_GetClass(jsoScene)->spare==1){
//mpScene=(mlRMMLComposition*)(mlRMMLElement*)JS_GetPrivate(cx,jsoScene);
//}
//}
	return OMS_OK;
}

// восстановить состояние сцены
omsresult mlSceneManager::SetState(void* aData, unsigned long aDataSize){
TRACE(cx, "SetState\n");
	mlSynchData oData(aData,aDataSize);
	oData >> cx;
	mlDontSendSynchEvents oDSSE(this);
	mbDontSendSynchEvents = false; // чтобы не отсекало вызовы 1 -> 0
	NotifyPlayerOnRestoreStateUnsynchedListeners();
	ResetEvent();
	return OMS_OK;
}

void mlSceneManager::GetClassesState(mlSynchData &aData){
	// ??
}

void mlSceneManager::SetClassesState(mlSynchData &aData){
	// ??
}

void mlSceneManager::GetGOState(mlSynchData &aData){
	// ??
}

mlMedia* mlSceneManager::GetObjectByTarget(const wchar_t* apwcTarget)
{
	ML_ASSERTION(cx, apwcTarget!=NULL, "mlSceneManager::GetObjectByTarget");
	if( !apwcTarget)	return	NULL;
	GUARD_JS_CALLS(cx);
	jsval v = JSVAL_NULL;
	unsigned int uLength=wcslen(apwcTarget);
	JSBool bR=wr_JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), apwcTarget, uLength, "execScript", 1, &v);
	return JSVAL_TO_MLEL( cx, v);
}

unsigned int mlSceneManager::GetObjectRealityIDByTarget(const wchar_t* apwcTarget)
{
	ML_ASSERTION(cx, apwcTarget!=NULL, "mlSceneManager::GetObjectRealityIDByTarget");
	if( !apwcTarget)	return	-1;
	GUARD_JS_CALLS(cx);
	jsval v = JSVAL_NULL;
	unsigned int uLength=wcslen(apwcTarget);
	JSBool bR=wr_JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), apwcTarget, uLength, "execScript", 1, &v);
	mlRMMLElement* elem = JSVAL_TO_MLEL( cx, v);
	if (elem != NULL)
		return elem->GetRealityID();
	else
		return -1;
}

void mlSceneManager::GetSceneData(mlRMMLElement* apScene, mlSynchData &aData){
	mlSceneDatas::mlSceneData* pSceneData=mSceneDatas.GetSceneData(apScene);
	if(pSceneData==NULL){
		aData << L"???";
	}else{
		aData << pSceneData->msPath.data();
	}
}

void mlSceneManager::SetSceneData(mlRMMLElement* apScene, mlSynchData &aData){
	MP_NEW_P( pSceneData, mlSceneDatas::mlSceneData, apScene);
	wchar_t* pwc;
	aData >> pwc;
	if(pwc!=NULL)
		pSceneData->msPath=pwc;
	mSceneDatas.AddSceneData(pSceneData);
}

__forceinline mlRMMLElement* GetElemScene(mlRMMLElement* apElem){
	mlRMMLElement* mpParent=apElem;
	while(mpParent!=NULL){
		if(mpParent->mRMMLType==MLMT_SCENE)
			return mpParent;
		mpParent=mpParent->mpParent;
	}
	return NULL;
}

// если элемент объявлен/уточнен в ресурсном модуле, то 
// к его пути надо добавить алиас xml-ки, в которой он объявлен (если он есть)

mlString mlSceneManager::RefineResPath(mlString sXPath,const wchar_t* apwcResPath){
	const wchar_t* pwc=apwcResPath;
	// если начинается с ':', то это уже полный путь
	if(pwc[0] == L':') return apwcResPath;
	// если второй символ ':' и за ним '\\', то это абсолютный путь
	if(pwc[0] != L'\0' && pwc[1] == L':' && pwc[2] == L'\\')
		return apwcResPath;
	// если в начале '\\\\', то это тоже абсолютный путь
	if(pwc[0] == L'\\' && pwc[1] == L'\\')
		return apwcResPath;
	// если это путь с префиксом или начинается со слеша,
	// то надо добывить только префикс XML-и
	if((wcschr(pwc, L':') != NULL) || (*pwc == L'\\' || *pwc == L'/')){
		// и в XPath нет alias-а, то не надо уточнять
		wstring::size_type iLastColon = sXPath.rfind(L':');
		if(iLastColon == wstring::npos){
			sXPath = apwcResPath;
			if(sXPath[0] != L':') sXPath.insert(0, L":"); // больше уточнять не надо
			return sXPath;
		}
		// если в XPath есть alias, 
		sXPath.erase(iLastColon+1);
		// то выдрать его оттуда и добавить к ResPath
		// (к XPath-у должен быть добавлен alias-path к модулю, в котором содержится XML-ка)
		sXPath.append(apwcResPath);
		if(sXPath[0] != L':') sXPath.insert(0, L":"); // больше уточнять не надо
		return sXPath;
	}
	// заменяем все '/' на '\'
	wstring::iterator si=sXPath.begin();
	for(; si!=sXPath.end(); si++)
		if(*si==L'/') *si=L'\\';
	// ищем последний слэш
	bool bDontAddStartSlash=false;
	wstring::size_type iLastSl=sXPath.rfind(L'\\');
	if(iLastSl==wstring::npos){
		// если слэша нет, ищем ':'
		iLastSl=sXPath.rfind(L':');
		if(iLastSl==wstring::npos){
			sXPath = apwcResPath;
			if(sXPath[0] != L':') sXPath.insert(0, L":"); // больше уточнять не надо
			return sXPath;
		}
		iLastSl++; // ':' оставляем
		bDontAddStartSlash=true;
	}
	// удаляем название XML-файла
	sXPath.erase(iLastSl);
	// если относительный путь начинается с "..", то убираем последнюю папку
	while(*pwc==L'.' && pwc[1]!=L'\0' && pwc[1]==L'.'){
		pwc+=2;
		if(*pwc==L'\\' || *pwc==L'/') pwc++;
		wstring::size_type iLastSl=sXPath.rfind(L'\\');
		if(iLastSl==wstring::npos){
			iLastSl=sXPath.rfind(L':');
			if(iLastSl==wstring::npos)
			{
				sXPath.erase();	// 2006-06-26 Tandy, Alex
				break;
			}
			iLastSl++; // оставляем ':'
		}
		if(iLastSl==wstring::npos) break;
		sXPath.erase(iLastSl);
	}
	if(*pwc==L'\\' || *pwc==L'/') pwc++;
	// добавляем недостающие слэши
	if(*(sXPath.rbegin()) != L'\\' && *sXPath.rbegin() != L':') // "&& *sXPath.rbegin()!=L':'" added by PG 17.09.05
		sXPath.append(1, L'\\');

	// если начинается с префикса (id ресурсного модуля например)
	if(sXPath.find(L':') != wstring::npos){
		// то не надо добавлять в начало слэш
		bDontAddStartSlash = true;
	}
	if(!bDontAddStartSlash && sXPath[0]!=L'\\' && sXPath[1]!=L':') sXPath.insert(0,L"\\");
	sXPath.append(pwc);
	// заменяем все '/' на '\'
	for(si=sXPath.begin(); si!=sXPath.end(); si++)
		if(*si==L'/') *si=L'\\';
	if(sXPath[0] != L':') sXPath.insert(0, L":"); // больше уточнять не надо
	return sXPath;
}

mlString mlSceneManager::RefineResPathEl(mlRMMLElement* apElem,const wchar_t* apwcResPath){
	const wchar_t* pwc=apwcResPath;
	// если начинается с ':', то это уже полный путь
	if(pwc[0] == L':') return apwcResPath;
	// если начинается с префикса ui:, то не надо уточнять
	if(pwc[0] == L'u' && pwc[1] == L'i' && pwc[2]==L':') return apwcResPath;
	// если начинается с префикса http:// или https://, то тоже не надо уточнять
	if(pwc[0] == L'h' && pwc[1] == L't' && pwc[2]==L't' && pwc[3]==L'p'){
		int iIdx = 4;
		if(pwc[iIdx] == L's') iIdx++;
		if(pwc[iIdx] == L':' && pwc[iIdx+1] == L'/' && pwc[iIdx+2] == L'/')
			return apwcResPath;
	}
	//
	if(apElem == NULL) return apwcResPath;
	//
	mlString sXMLPath;
	if(apElem->miSrcFilesIdx >= 0){
		// если есть префикс scn
		const wchar_t* pwc = mSrcFileColl[apElem->miSrcFilesIdx];
		if(pwc != NULL)
			sXMLPath = pwc;
	}
	if(sXMLPath.empty()){
		mlRMMLElement* pScene = GetElemScene(apElem);
		if(pScene == NULL) pScene = mpScene;
		mlSceneDatas::mlSceneData* pData = mSceneDatas.GetSceneData(pScene);
		if(pData != NULL)
			sXMLPath = pData->msPath;
	}
	return RefineResPath(sXMLPath,apwcResPath);
}

// заменяет все псевдонимы модулей на идентификаторы модулей
mlString mlSceneManager::ReplaceAliasesToModuleIDs(const mlString awsPath)
{
	mlString wsPath(awsPath);
	// перебираем alias-ы в пути и заменяем их на идентификаторы модулей
	if(wsPath[0] != L':')
		return wsPath;
	std::vector<mlString> vModuleIDs;
	vModuleIDs.push_back(GetModuleID());
	int iCommaIdx = 0;
	int iNextCommaIdx = iCommaIdx;
	while((iNextCommaIdx = wsPath.find(L':', iCommaIdx+1)) != -1){
		mlString wsAlias = wsPath.substr(iCommaIdx+1, (iNextCommaIdx-iCommaIdx-1));
		if(wsAlias == L"ui")
			break;
		mlString wsOwnerModuleID = vModuleIDs[vModuleIDs.size()-1];
		wchar_t moduleID[256] = {0};
		wchar_t *pwcModuleID = &moduleID[0];
		bool bRes = mpContext->mpResM->GetModuleIDByAlias(wsAlias.c_str(), wsOwnerModuleID.c_str(), &pwcModuleID, 256);
		if(bRes){
			wsPath.replace(iCommaIdx+1, (iNextCommaIdx-iCommaIdx-1), pwcModuleID);
		}
		iCommaIdx = iNextCommaIdx;
	}
	return wsPath;
}

mlSyncCallbackManager* mlSceneManager::GetSyncCallbackManager()
{
	return mSyncCallbackManager;
}

mlWorldEventManager*	mlSceneManager::GetWorldEventCallbackManager()
{
	return mWorldEventManager;
}

mlString mlSceneManager::GetFullResPath(const wchar_t* apwcResPath){
	mlString sFullResPath = apwcResPath;
	//
	JSScript* script = NULL;
	JSStackFrame* fp = NULL;
	fp = JS_FrameIterator(cx, &fp);
	for(;fp != NULL; fp = JS_FrameIterator(cx, &fp)){
		if(JS_GetFrameFunction(cx, fp) != NULL){
			JSFunction* fun = JS_GetFrameFunction(cx, fp);
	//JSString* jssFun = JS_DecompileFunction(cx, fun, 0);
	//jschar* jscFun = JS_GetStringChars(jssFun);
			script = JS_GetFunctionScript(cx, fun);
			if(script != NULL) // пропускаем саму getFullPath
				break;
		}
		if(JS_GetFrameScript(cx, fp) != NULL){
			script = JS_GetFrameScript(cx, fp);
			break;
		}
	}
	if(script){
		const char* pszScriptFilename = JS_GetScriptFilename(cx, script);
		mlString sXMLPath = cLPWCSTR(pszScriptFilename);
		sFullResPath = RefineResPath(sXMLPath, sFullResPath.c_str());
	}
	return sFullResPath;
}

#define WSZMODULEDOT L"module."
#define WSZUIDOT L"UI."
#define WSZMAPDOT L"map."
#define WSZURLDOT L"url."
#define WSZVOICEDOT L"voice."
#define WSZSERVERDOT L"server."
#define WSZRECORDERDOT L"server.recorder."
#define WSZRECORDEDITORDOT L"recordEditor."
#define WSZVIDEOCAPTUREDOT L"videoCapture."
#define WSZCODEEDITORDOT L"codeEditor."

bool mlSceneManager::IsGlobalObjectEvent(const wchar_t* apwcName,const wchar_t* apwcEvent,char &aidObj,char &aidEvent){
	if(isEqual(apwcName, GJSONMW_PLAYER)){
		if(isEqual(apwcEvent,WSZMODULEDOT,wcslen(WSZMODULEDOT))){
			int iModuleDotLen = wcslen(WSZMODULEDOT);
			aidObj = GOID_Module;
			aidEvent = GetModuleEventID(apwcEvent+iModuleDotLen);
		}else if(isEqual(apwcEvent, WSZMAPDOT, wcslen(WSZMAPDOT))){
			int iMapDotLen = wcslen(WSZMAPDOT);
			aidObj = GOID_Map;
			aidEvent = GetMapEventID(apwcEvent+iMapDotLen);
		}else if(isEqual(apwcEvent, WSZURLDOT, wcslen(WSZURLDOT))){
			int iUrlDotLen = wcslen(WSZURLDOT);
			aidObj = GOID_Url;
			aidEvent = GetUrlEventID(apwcEvent+iUrlDotLen);
		}else if(isEqual(apwcEvent, WSZVOICEDOT, wcslen(WSZVOICEDOT))){
			int iVoiceDotLen = wcslen(WSZVOICEDOT);
			aidObj = GOID_Voice;
			aidEvent = GetVoiceEventID(apwcEvent+iVoiceDotLen);
		}else if(isEqual(apwcEvent, WSZRECORDERDOT, wcslen(WSZRECORDERDOT))){
			int iRecorderDotLen = wcslen(WSZRECORDERDOT);
			aidObj = GOID_Recoder;
			aidEvent = GetRecordEventID(apwcEvent+iRecorderDotLen);
		}else if(isEqual(apwcEvent, WSZSERVERDOT, wcslen(WSZSERVERDOT))){
            int iServerDotLen = wcslen(WSZSERVERDOT);
			aidObj = GOID_Server;
			aidEvent = GetServerEventID(apwcEvent+iServerDotLen);
		}else if(isEqual(apwcEvent, WSZRECORDEDITORDOT, wcslen(WSZRECORDEDITORDOT))){
			int iRecordEditorDotLen = wcslen(WSZRECORDEDITORDOT);
			aidObj = GOID_RecordEditor;
			aidEvent = GetRecordEditorEventID(apwcEvent+iRecordEditorDotLen);
		}else if(isEqual(apwcEvent, WSZVIDEOCAPTUREDOT, wcslen(WSZRECORDEDITORDOT))){
			int iVideoCaptureDotLen = wcslen(WSZVIDEOCAPTUREDOT);
			aidObj = GOID_VideoCapture;
			aidEvent = GetVideoCaptureEventID(apwcEvent+iVideoCaptureDotLen);						
		}
		else if(isEqual(apwcEvent, WSZCODEEDITORDOT, wcslen(WSZCODEEDITORDOT))){
			int iCodeEditorDotLen = wcslen(WSZCODEEDITORDOT);
			aidObj = GOID_CodeEditor;
			aidEvent = GetCodeEditorEventID(apwcEvent+iCodeEditorDotLen);		
		}else if(isEqual(apwcEvent, WSZUIDOT, wcslen(WSZUIDOT))){
			return false;
		}else{
			aidObj = GOID_Player;
			aidEvent = GetPlayerEventID(apwcEvent);
			if(aidEvent == 0) aidEvent = -1;
		}
		return true;
	}
	if(isEqual(apwcName, GJSONMW_MOUSE)){
		aidObj = GOID_Mouse;
		aidEvent = GetMouseEventID(apwcEvent);
		if(aidEvent == '\0') return false;
		return true;
	}
	// ??
	return false;
}

void mlSceneManager::SetEventListener(char aidSender,char aidEvent,mlRMMLElement* apListener){
//	if((mlRMMLElement*)apListener->GetScene()==mpUI){
//		mvUIGOListeners.push_back(apListener);
//	}
	if(aidSender == GOID_Player){
		addPlayerListener(aidEvent,apListener);
	}
	if(aidSender == GOID_Module){
		addModuleListener(aidEvent,apListener);
	}
	if(aidSender == GOID_Map){
		addMapListener(aidEvent,apListener);
	}
	if(aidSender == GOID_Url){
		addUrlListener(aidEvent,apListener);
	}
	if(aidSender == GOID_Voice){
		addVoiceListener(aidEvent,apListener);
	}
	if(aidSender == GOID_Recoder){
		addRecorderListener(aidEvent,apListener);
	}
	if(aidSender == GOID_Mouse){
		addMouseListener(aidEvent,apListener);
	}
	if(aidSender == GOID_Server){
		addServerListener(aidEvent,apListener);
	}
	if(aidSender == GOID_RecordEditor){
		addRecordEditorListener(aidEvent,apListener);
	}
	if(aidSender == GOID_VideoCapture){
		addVideoCaptureListener(aidEvent,apListener);
	}
	if(aidSender == GOID_CodeEditor){
		addCodeEditorListener(aidEvent,apListener);
	}
	// ??
}

mlString mlSceneManager::GetEventName(char aidSender,char aidEvent){
	if(aidSender == GOID_Player){
		mlString ws = cLPWCSTRq(GJSONM_PLAYER);
		ws += L'.';
		ws += GetPlayerEventName(aidEvent);
		return ws;
	}
	if(aidSender == GOID_Module){
		mlString ws = cLPWCSTRq(GJSONM_PLAYER);
		ws += L'.';
		ws += cLPWCSTRq(GJSONM_MODULE);
		ws += L'.';
		ws += GetModuleEventName(aidEvent);
		return ws;
	}
	if(aidSender == GOID_Map){
		mlString ws = cLPWCSTRq(GJSONM_PLAYER);
		ws += L'.';
		ws += cLPWCSTRq(GJSONM_MAP);
		ws += L'.';
		ws += GetMapEventName(aidEvent);
		return ws;
	}
	if(aidSender == GOID_Url){
		mlString ws = cLPWCSTRq(GJSONM_PLAYER);
		ws += L'.';
		ws += cLPWCSTRq(GJSONM_URL);
		ws += L'.';
		ws += GetUrlEventName(aidEvent);
		return ws;
	}
	if(aidSender == GOID_Voice){
		mlString ws = cLPWCSTRq(GJSONM_PLAYER);
		ws += L'.';
		ws += cLPWCSTRq(GJSONM_VOICE);
		ws += L'.';
		ws += GetVoiceEventName(aidEvent);
		return ws;
	}
	if(aidSender == GOID_Recoder){
		mlString ws = cLPWCSTRq(GJSONM_PLAYER);
		ws += L'.';
		ws += cLPWCSTRq(GJSONM_SRECORDER);
		ws += L'.';
		ws += GetRecordEventName(aidEvent);
		return ws;
	}
	if(aidSender == GOID_Server){
		mlString ws = cLPWCSTRq(GJSONM_PLAYER);
		ws += L'.';
		ws += cLPWCSTRq(GJSONM_SERVER);
		ws += L'.';
		ws += GetServerEventName(aidEvent);
		return ws;
	}
	if(aidSender == GOID_RecordEditor){
		mlString ws = cLPWCSTRq(GJSONM_PLAYER);
		ws += L'.';
		ws += cLPWCSTRq(GJSONM_RECORD_EDITOR);
		ws += L'.';
		ws += GetRecordEditorEventName(aidEvent);
		return ws;
	}
	if(aidSender == GOID_VideoCapture){
		mlString ws = cLPWCSTRq(GJSONM_PLAYER);
		ws += L'.';
		ws += cLPWCSTRq(GJSONM_VIDEOCAPTURE);
		ws += L'.';
		ws += GetVideoCaptureEventName(aidEvent);
		return ws;
	}
	if(aidSender == GOID_CodeEditor){
		mlString ws = cLPWCSTRq(GJSONM_PLAYER);
		ws += L'.';
		ws += cLPWCSTRq(GJSONM_CODEEDITOR);
		ws += L'.';
		ws += GetCodeEditorEventName(aidEvent);
		return ws;
	}

	return L"???";
}

/**
 * поддержка отладки 
 */
void mlSceneManager::HandleGetDebugInfoEvent(oms::omsUserEvent &aEvent){
	char cInfoType=aEvent.ev&0xFF;
	if(cInfoType==rmmld::WCDC_GET_TREE_INFO){
		rmmld::mlIDebugger* pIDebug=rmmld::CreateDebugger(this);
		oDebugInfoData.reset();
		pIDebug->GetTree(oDebugInfoData);
		void* hwndDebugger=*((void**)aEvent.script);
//unsigned char* pBuf=new unsigned char[oDebugInfoData.size()];
//memcpy(pBuf,oDebugInfoData.data(),oDebugInfoData.size());
		mpContext->mpApp->SendDebugInfo(cInfoType, hwndDebugger, 
						(unsigned char*)oDebugInfoData.data(), oDebugInfoData.size());
//						pBuf, oDebugInfoData.size());
	}else if(cInfoType==rmmld::WCDC_GET_PROPERTIES){
		rmmld::mlIDebugger* pIDebug=rmmld::CreateDebugger(this);
		oDebugInfoData.reset();
		int* pInt=(int*)aEvent.script;
		pInt++;
		int iID=*pInt;
		pIDebug->GetProperties(oDebugInfoData,iID);
		void* hwndDebugger=*((void**)aEvent.script);
		mpContext->mpApp->SendDebugInfo(cInfoType, hwndDebugger, 
						(unsigned char*)oDebugInfoData.data(), oDebugInfoData.size());
	}else if(cInfoType==rmmld::WCDC_GET_JSTREE){
		rmmld::mlIDebugger* pIDebug=rmmld::CreateDebugger(this);
		oDebugInfoData.reset();
		pIDebug->GetJSTree(oDebugInfoData);
		void* hwndDebugger=*((void**)aEvent.script);
		mpContext->mpApp->SendDebugInfo(cInfoType, hwndDebugger, 
						(unsigned char*)oDebugInfoData.data(), oDebugInfoData.size());
	}else{
		// ??
	}
}

/**
 * поддержка событий 
 */
int mlSceneManager::CheckEvent(char aIdEvent, mlRMMLElement* apSender, jsval *vp){
	if(mpvFiredEvents == NULL && mpSELMLEl==NULL) return -1;
	//if(mEventCheckMode != ECM_Check) return -1;
	// запомнить значение свойства onXXX и сделать запись в mvEventsToRestore
	if(vp != NULL){
		if(JSVAL_IS_STRING(*vp) || 
			((*vp)!=JSVAL_VOID && JSVAL_IS_OBJECT((*vp)) && JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT((*vp)))))
		{
			SAVE_VAL_FROM_GC(cx, apSender->mjso, (*vp))
			mvEventsToRestore.push_back(EventToRestore(apSender, aIdEvent, *vp));
		}
	}
	if(mpvFiredEvents != NULL){
		// если событие уже есть в mvFiredEvents, то выдать true
		std::vector<EventSender>::iterator vi;
		for(vi=mpvFiredEvents->begin(); vi!=mpvFiredEvents->end(); vi++){
			if(vi->pSender == apSender && vi->idEvent == aIdEvent)
				return true;
		}
	}else{
		// иначе установить его слушателем и выдать false
		if(mpSELMLEl != NULL){
			apSender->SetEventListener(aIdEvent, mpSELMLEl);
			mpSELMLEl->YouAreSetAsEventListener(aIdEvent, apSender);
		}
	}
	return false;
}

void mlSceneManager::RestoreEventJSVals(){
	if(mvEventsToRestore.size() == 0) return;
	std::vector<EventToRestore>::iterator vi;
	for (vi = mvEventsToRestore.begin(); vi != mvEventsToRestore.end(); ){
		(vi->pElem)->SetEventJSVal(vi->evID, vi->val);
		FREE_VAL_FOR_GC(cx, vi->pElem->mjso, vi->val)
		mvEventsToRestore.erase(vi);
	}
}

#define GET_FUNC_ADDR(NM) \
	mLibXml2Funcs.NM = (mlLibXml2Funcs::Func_##NM)pApp->GetProcAddress(mhmLibXml2, #NM);

void mlSceneManager::DetectLibXml2(){
	mhmLibXml2 = NULL;
#ifdef RMML_USE_LIBXML2
	oms::omsIApplication* pApp = mpContext->mpApp;
	if(pApp != NULL){
		mhmLibXml2 = pApp->LoadLibrary(L"libxml2.dll");
//mhmLibXml2 = NULL;
		if(mhmLibXml2 != NULL){
			GET_FUNC_ADDR(xmlNewIOInputStream);
			GET_FUNC_ADDR(xmlAllocParserInputBuffer);
			GET_FUNC_ADDR(xmlNewParserCtxt);
			GET_FUNC_ADDR(xmlFreeParserCtxt);
			GET_FUNC_ADDR(xmlParseDocument);
			GET_FUNC_ADDR(xmlFreeDoc);
			GET_FUNC_ADDR(xmlGetLastError);
			GET_FUNC_ADDR(xmlAddEncodingAlias);
			GET_FUNC_ADDR(xmlSetInputStreamFilePath);
			GET_FUNC_ADDR(xmlNewTextReader);
			GET_FUNC_ADDR(xmlFreeTextReader);
			GET_FUNC_ADDR(xmlTextReaderRead);
			GET_FUNC_ADDR(xmlTextReaderName);
			GET_FUNC_ADDR(xmlTextReaderNodeType);
			GET_FUNC_ADDR(xmlTextReaderDepth);
			GET_FUNC_ADDR(xmlTextReaderIsEmptyElement);
			GET_FUNC_ADDR(xmlTextReaderMoveToNextAttribute);
			GET_FUNC_ADDR(xmlTextReaderHasValue);
			GET_FUNC_ADDR(xmlTextReaderValue);
			GET_FUNC_ADDR(xmlSetGenericErrorFunc);
			GET_FUNC_ADDR(xmlTextReaderHasAttributes);
			GET_FUNC_ADDR(xmlTextReaderMoveToElement);
			GET_FUNC_ADDR(xmlParseCharEncoding);
			GET_FUNC_ADDR(xmlFindCharEncodingHandler);
			GET_FUNC_ADDR(xmlSwitchInputEncoding);
			//
			mLibXml2Funcs.xmlAddEncodingAlias("CP1251","WIN-1251");
			mLibXml2Funcs.xmlAddEncodingAlias("CP1251","WINDOWS-1251");
			mLibXml2Funcs.xmlAddEncodingAlias("CP1251","CP-1251");
			mLibXml2Funcs.xmlAddEncodingAlias("CP1251","WIN1251");
		}else{
			mlTrace(cx, "LibXml2 library not loaded. Using build-in XML parser.\n");
		}
	}
#endif
}

void mlSceneManager::RemoveFromModalButList(mlRMMLElement* apMLEl, bool abNotifyLater){
	std::list<mlModalButton>::iterator li;
	for(li = mlModalButs.begin(); li != mlModalButs.end(); li++){
		mlModalButton& but = *li;
		mlRMMLElement* pMLEl = but.mpButton;
		if(pMLEl == apMLEl){
			mlModalButs.erase(li);
			if(abNotifyLater) 
				mbNeedToNotifyMouseModalButtons = true;
			else
				NotifyMouseModalButtons();
			break;
		}
	}
}

void mlSceneManager::UpdateModalButList(mlRMMLElement* apMLEl){
	int iModal = apMLEl->GetButton()->GetModal();
	if(iModal == 0){
		// удалить его из списка
		RemoveFromModalButList(apMLEl);
		// NotifyMouseModalButtons();
		return;
	}
	// если в списке его еще не было
	std::list<mlModalButton>::iterator li;
	for(li = mlModalButs.begin(); li != mlModalButs.end(); li++){
		mlModalButton& but = *li;
		mlRMMLElement* pMLEl = but.mpButton;
		if(pMLEl == apMLEl)
			break;
	}
	if(li == mlModalButs.end()){
		// добавить его в список
		//  если добавляется 3D-объект, то и viewport для него тоже надо поместить в список
		mlRMMLVisible* pVisible = NULL;
		if(apMLEl->GetVisible() == NULL){
			// то наверное это 3D-объект  
			pVisible = mBUM.mpVisible;
		}
		mlModalButs.push_back(mlModalButton(apMLEl, pVisible));
		NotifyMouseModalButtons();
	}
}

mlRMMLElement* mlSceneManager::GetModalButtonUnderMouse(mlPoint &aMouseXY, bool &bAllModalsInvisible){
	bAllModalsInvisible = true;
	// взять текущий верхний уровень модальности
	int iMaxModalLevel = 0;
	std::list<mlModalButton>::iterator li;
	for(li = mlModalButs.begin(); li != mlModalButs.end(); li++){
		mlModalButton& but = *li;
		mlRMMLElement* pMLEl = but.mpButton;
		if(pMLEl->GetVisible() && pMLEl->GetVisible()->GetAbsVisible())
			bAllModalsInvisible = false;
		int iModal = pMLEl->GetButton()->GetModal();
		if(iModal > iMaxModalLevel)
			iMaxModalLevel = iModal;
	}
	// перебираем все кнопки верхнего уровня модальности и 
	mlRMMLElement* pMLElMinDepth=NULL;
	int iMinDepth=32000;
	for(li = mlModalButs.begin(); li != mlModalButs.end(); li++){
		mlModalButton& but = *li;
		mlRMMLElement* pMLEl = but.mpButton;
		int iModal = pMLEl->GetButton()->GetModal();
		if(iModal != iMaxModalLevel) continue;
		if(pMLEl->GetButton()==NULL) continue;
		mlRMMLVisible* pVisible = NULL;
		mlRMMLComposition* pScene = (mlRMMLComposition*)pMLEl->GetScene();
		if(pScene == NULL) return NULL;
		mlPoint MouseXY = aMouseXY;
		// взять родительский Visible
		mlRMMLElement* pParentVisibleMLEl = pMLEl->GetParent();
		// если модальный элемент - 3D объект 
		if(pMLEl->GetVisible() == NULL){
			// то родительским Visible-ом будем считать родителя viewport-а
			mlRMMLVisible* pVisible = but.mpVisible;
			if(pVisible != NULL){
				mlRMMLElement* pMLElVis = pVisible->GetElem_mlRMMLVisible();
				pParentVisibleMLEl = pMLElVis->GetParent();
			}
		}
		mlRMMLVisible* pParentVisible = NULL;
		while(pParentVisibleMLEl != NULL){
			pParentVisible = pParentVisibleMLEl->GetVisible();
			if(pParentVisible != NULL) break;
			pParentVisibleMLEl = pParentVisibleMLEl->mpParent;
		}
		if(pParentVisible == NULL) 
			continue;
		// переводим мышиные координаты в координаты мыши относительно родителя-Visible-а
		mlPoint ParentXY = pParentVisible->GetAbsXY();
		MouseXY.x -= ParentXY.x;
		MouseXY.y -= ParentXY.y;

		/*mlPoint SceneXY = pScene->GetVisible()->GetAbsXY();
		MouseXY.x -= SceneXY.x;
		MouseXY.y -= SceneXY.y;*/
		// если модальный элемент - 3D объект 
		if(pMLEl->GetVisible() == NULL){
			// то GetButtonUnderMouse надо вызывать у Viewport-а
			mlButtonUnderMouse oBUM;
			oBUM.mpVisible = but.mpVisible;
			if(oBUM.mpVisible == NULL)
				continue;
			mlRMMLElement* pMLElVis = oBUM.mpVisible->GetElem_mlRMMLVisible();
			mlRMMLElement* pMLElBut = pMLElVis->GetButtonUnderMouse(MouseXY, oBUM);
			if(pMLElBut != NULL){
				int iDepth=pMLElVis->GetVisible()->GetAbsDepth();
				if(iDepth < iMinDepth){
					pMLElMinDepth=pMLElBut;
					iMinDepth=iDepth;
				}
			}
		}else{
			mlButtonUnderMouse oBUM;
			mlRMMLElement* pMLElBut=pMLEl->GetButtonUnderMouse(MouseXY, oBUM);
			pVisible = oBUM.mpVisible;
			if(pMLElBut){
				mlRMMLVisible* pMLElVis=pMLElBut->GetVisible();
				if(pMLElVis == NULL){
					if(pVisible == NULL){
						if(pMLEl->mRMMLType == MLMT_VIEWPORT)
							pVisible = pMLEl->GetVisible();
					}
					pMLElVis = pVisible;
				}
				ML_ASSERTION(cx, pMLElVis!=NULL,"mlSceneManager::GetModalButtonUnderMouse");
				int iDepth=pMLElVis->GetAbsDepth();
				if(iDepth < iMinDepth){
					pMLElMinDepth=pMLElBut;
					iMinDepth=iDepth;
				}
			}
		}
	}
	return pMLElMinDepth;
}

void mlSceneManager::LoadAuthScene(){
	if(ML_FAILED(LoadXML(AUTH_SCENE_FILE_NAME, true, false)))
		return;
}

//const wchar_t cwsBugCheckScript[] = L"Player.UI.tools.stCaptionNormal";

const wchar_t cwsBugCheckScript2[] = L"scene.objects.linda";

void mlSceneManager::CheckBug(){
/////////////////// 1
//jsval v;
//JS_GetProperty(cx, mjsoClasses, "ui_COrderedCompositionBase", &v);
//if(JSVAL_IS_REAL_OBJECT(v)){
//jsval vParent;
//JS_GetProperty(cx, JSVAL_TO_OBJECT(v), "_parent", &vParent);
//int hh=0;
//}
/////////////////// 2
//	jsval v;
//	JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), cwsBugCheckScript, sizeof(cwsBugCheckScript)/sizeof(wchar_t)-1, "bugCheck", 1, &v);
//	if(v == JSVAL_VOID){
//		int hh=0;
//	}else{
//		mlRMMLElement* pMLEl = JSVAL_TO_MLEL(cx, v);
//		JSIdArray* pPropIDArr = JS_Enumerate(cx, pMLEl->mjso);
//		for(int i = 0; i < pPropIDArr->length; i++){
//			jsval v;
//			if(!JS_IdToValue(cx, pPropIDArr->vector[i], &v)) continue;
//			if(JSVAL_IS_STRING(v)){
//				jschar* jsc = JS_GetStringChars(JSVAL_TO_STRING(v));
//				int len = wcslen(jsc);
//			}
//		}
//		int hh=0;
//	}
/////////////////// 3
//	jsval v;
//	JS_GetProperty(cx, mjsoClasses, "ceBaseEquip", &v);
//	if(!JSVAL_IS_REAL_OBJECT(v)) return;
//	JS_GetProperty(cx, JSVAL_TO_OBJECT(v), "obj", &v);
//	if(!JSVAL_IS_REAL_OBJECT(v)) return;
////	JS_GetProperty(cx, JSVAL_TO_OBJECT(v), "__proto__", &v);
////	if(!JSVAL_IS_REAL_OBJECT(v)) return;
//	JS_GetProperty(cx, JSVAL_TO_OBJECT(v), "materials", &v);
//	if(!JSVAL_IS_REAL_OBJECT(v)) return;
//	JS_GetProperty(cx, JSVAL_TO_OBJECT(v), "__2", &v);
//	if(!JSVAL_IS_REAL_OBJECT(v)) return;
//	JS_GetProperty(cx, JSVAL_TO_OBJECT(v), "diffuse_map", &v);
//	if(!JSVAL_IS_REAL_OBJECT(v)) return;
//	int hh=0;
///////////////////// 4
	/*jsval v;
	JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), cwsBugCheckScript2, sizeof(cwsBugCheckScript2)/sizeof(wchar_t)-1, "bugCheck", 1, &v);
	if(JSVAL_IS_REAL_OBJECT(v)){
		if(JSVAL_IS_MLEL(cx, v)){
			mlRMMLElement* pMLEl = JSVAL_TO_MLEL(cx, v);
			int hh=0;
		}
	}*/
}

void mlSceneManager::SetActivationUIScriptsFlag(mlRMMLElement* apStartElem){
	if(mbUIScriptActivatingStartElem == NULL)
		mbUIScriptActivatingStartElem = apStartElem;
}
void mlSceneManager::ResetActivationUIScriptsFlag(mlRMMLElement* apElem){ 
	if(mbUIScriptActivatingStartElem == apElem)
		mbUIScriptActivatingStartElem = NULL; 
}

void mlSceneManager::SetClientStringID(const wchar_t* apwcStringID){
	msClientStringID = apwcStringID;
	_wcslwr_s((wchar_t*)msClientStringID.c_str(), msClientStringID.length()+1);
}

// создать моего аватара
mlMedia* mlSceneManager::CreateAvatar(mlObjectInfo4Creation &aAvatarInfo)
{
	// Запоминаем ID аватара
	uiCreatedAvatarID = aAvatarInfo.muObjectID;

	if( mpMyAvatar == NULL)
	{
		// создаем объект аватара
		mlMedia* pAvatar = CreateObject(aAvatarInfo);
		mpMyAvatar = (mlRMMLElement*)pAvatar;
	}
	else
	{
		// При попятке повторного сохдания моего аватара функция mlMedia* pAvatar = CreateObject(aAvatarInfo); вернет NULL
		// так как аватары не регистрятся в CScriptedObjectManager::oneRealityObjects, а проверка идет сначала по ней
		// Поэтому просто вернем моего аватара сразу.
		mpMyAvatar = mpMyAvatar;
	}

	if (mpMyAvatar != NULL)
	{
		CLogValue logValue(
			"[RMML.OBJECTS]: Avatar '", (const char*)cLPCSTR( mpMyAvatar->GetName()), "' has been created."
		);
		mpContext->mpLogWriter->WriteLnLPCSTR( logValue.GetData());

		mpMyAvatar->RealityChanged( mpContext->mpMapMan->GetCurrentRealityID());

		muiMyAvatarID = mpMyAvatar->GetID();
		mMyAvatarBornRealityID = mpMyAvatar->GetBornRealityID();

		// устанавливаем флаги синхронизации
		// 4 означает, что объект должен быть уничтожен сразу как только исчезает его собственник (т.е. я как клиент )
		mpMyAvatar->SetSynchFlags(MLSYNCH_EXCLUSIVE_OWN_MASK);

		// делаем себя его собственником
		if (!mpMyAvatar->Capture())
		{
			CLogValue logValue(
				"RMML.0001: Avatar can not be captured (mlSceneManager::CreateAvatar)"
				);
			mpContext->mpLogWriter->WriteLnLPCSTR( logValue.GetData());
		}
		// устанавливаем его в определенную позицию (aAvatarInfo.mPos)
		// ??
		uiCreatedAvatarID = 0xFFFFFFFF;
		NotifyMapOnMyAvatarCreatedListeners();
	}
	else
	{
		CLogValue logValue(
			"[RMML]: Avatar '", aAvatarInfo.mszName, "' has not created."
		);
		mpContext->mpLogWriter->WriteLnLPCSTR( logValue.GetData());
	}
	return mpMyAvatar;
}

/*
struct mlObjectInfo4Creation{
	int miObjectID;			// идентификатор объекта
	unsigned short musType;	// RMML-тип объекта (обычно MLMT_OBJECT или MLMT_GROUP)
	const char* mszName;	// имя объекта
	const char* mszClassName;	// имя класса объекта
	const char* mszClassFileName;	// имя файла класса объекта (для проверки)
	const mlMedia* mpParent;	// родительская 3D-сцена или 3D-группа
	const wchar_t* mwcParams;	// скрипт инициализации (userProps)
	mlObjectCreationCallback mCallBackFunc;
	void* mCallbackParams;		// параметр для mCallBackFunc
};
*/

// Tandy: для выявления одной исключительной ситуации...
class mlSMCreatingObject
{
	mlSceneManager* mpSM;
public:
	mlSMCreatingObject(mlSceneManager* apSM):mpSM(apSM){
		mpSM->mbObjectCreating = true;
	}
	~mlSMCreatingObject(){
		mpSM->mbObjectCreating = false;
	}
};

// создать объект
mlMedia* mlSceneManager::CreateObject( mlObjectInfo4Creation &aObjectInfo)
{
	GUARD_JS_CALLS(cx);
	GPSM(cx)->SetAllocComment(L"mlSceneManager::CreateObject");
	mlSMCreatingObject oSMOC(this);
	bool bIsNewObject;
	JSObject* jso3DObject = NULL;

	if (aObjectInfo.musType == MLMT_CHARACTER)
	{
		CLogValue logValue("Start creating avatar ", aObjectInfo.mszName);
		mpContext->mpLogWriter->WriteLn( logValue);
	}

	mlRMMLElement* pMLEl = scriptedObjectManager.GetOneRealityObject( aObjectInfo.muObjectID);
	if (pMLEl != NULL)
	{
		bIsNewObject = false;
		jso3DObject = pMLEl->mjso;
	}
	else
	{
		bIsNewObject = true;
		// проверить наличие объекта
		pMLEl = scriptedObjectManager.GetObject(aObjectInfo.muObjectID, aObjectInfo.muBornRealityID);
		if (pMLEl != NULL)
		{
			GPSM(cx)->SetAllocComment(L"");
			return NULL; 
		}

		// проверить тип объекта
		if(aObjectInfo.musType != MLMT_OBJECT && aObjectInfo.musType != MLMT_GROUP && aObjectInfo.musType != MLMT_CHARACTER
			&& aObjectInfo.musType != MLMT_COMMINICATION_AREA && aObjectInfo.musType != MLMT_PARTICLES)
		{
			GPSM(cx)->SetAllocComment(L"");
			mlTraceError(cx, "Object type is not valid (mlSceneManager::CreateObject)\n");
			return NULL;
		}
		// проверить наличие родительской сцены
		if(aObjectInfo.mpParent == NULL)
		{
			GPSM(cx)->SetAllocComment(L"");
			mlTraceError(cx, "Parent 3D scene is not defined (mlSceneManager::CreateObject)\n");
			return NULL;
		}
		// создать объект
		if(aObjectInfo.musType == MLMT_OBJECT){
			jso3DObject = mlRMMLObject::newJSObject(cx);
		}else if(aObjectInfo.musType == MLMT_GROUP){
			jso3DObject = mlRMMLGroup::newJSObject(cx);
		}else if(aObjectInfo.musType == MLMT_CHARACTER){
			jso3DObject = mlRMMLCharacter::newJSObject(cx);
		}else if(aObjectInfo.musType == MLMT_COMMINICATION_AREA){
			jso3DObject = mlRMMLObject::newJSObject(cx);
		}else if(aObjectInfo.musType == MLMT_PARTICLES){
			jso3DObject = mlRMMLParticles::newJSObject(cx);
		}
		if(jso3DObject == NULL)
		{
			GPSM(cx)->SetAllocComment(L"");
			mlTraceError(cx, "Object is not created by unknown reasons (mlSceneManager::CreateObject)\n");
			return NULL;
		}

		ML_ASSERTION(cx, mpScene != NULL, "mlSceneManager::CreateObject");
		SAVE_FROM_GC(cx, mpScene->mjso, jso3DObject);

		pMLEl = (mlRMMLElement*)JS_GetPrivate(cx, jso3DObject);
		ML_ASSERTION(cx, pMLEl != NULL, "mlSceneManager::CreateObject");
	}

	// (name)
	if(aObjectInfo.mszName != NULL){
		JSString* jssName = JS_NewStringCopyZ(cx, aObjectInfo.mszName);
		if(!mlRMMLElement::IsNameValid(jssName)){
			mlTraceWarning(cx, "[RMML] We have got object description with invalid name '%s' and id=%d from server\n", aObjectInfo.mszName, aObjectInfo.muObjectID);
			// создаем уникальное имя
			char pszUniqueName[50];
			rmmlsafe_sprintf(pszUniqueName, 40, 0, "un%x", jso3DObject);
			jssName = JS_NewStringCopyZ(cx, pszUniqueName);
		}
		SAVE_STR_FROM_GC(cx, mpScene->mjso, jssName);
		jsval vName = STRING_TO_JSVAL(jssName);
		JSBool bR = JS_SetProperty(cx, jso3DObject, "name", &vName);
		FREE_STR_FOR_GC(cx, mpScene->mjso, jssName);
	}

	// (class)
	if (bIsNewObject)
	{
		if(aObjectInfo.mszClassName != NULL && aObjectInfo.mszClassName[ 0] != 0){
			mlString wsClassName = cLPWCSTR(aObjectInfo.mszClassName);
			JSObject* jsoClass = pMLEl->FindClass(wsClassName.c_str());
			if(jsoClass == NULL){
				mlTraceError(cx, "Class %s is not found (mlSceneManager::CreateObject)\n", aObjectInfo.mszClassName);
				FREE_FOR_GC(cx, mpScene->mjso, jso3DObject);
				GPSM(cx)->SetAllocComment(L"");
				return NULL;
			}
			mlString wsAllocComment = L"mlSceneManager::CreateObject class:";
			wsAllocComment += wsClassName;
			GPSM(cx)->SetAllocComment(wsAllocComment.c_str());
			// сделать проверку соответствия класса файлу с его описанием (aObjectInfo.mszClassFileName)
			// ??
			pMLEl->SetClass(jsoClass);
		}
	}

	// (userProps)
	if (aObjectInfo.musType == MLMT_CHARACTER)
	{
		CLogValue logValue("Before user props", aObjectInfo.mszName);
		mpContext->mpLogWriter->WriteLn( logValue);
	}

	if(aObjectInfo.mwcParams != NULL && aObjectInfo.mwcParams[0] != 0){

		jsval vUserProps = JSVAL_NULL;
		mlString wsScript = L"[";
		wsScript += aObjectInfo.mwcParams;
		wsScript += L"]";
		JSBool bR = JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), (const jschar*)wsScript.c_str(), wsScript.size(), "userProps", 1, &vUserProps);
		if( bR == JS_TRUE)
		{
			if (aObjectInfo.musType == MLMT_CHARACTER)
			{
				CLogValue logValue("user props compiled", aObjectInfo.mszName);
				mpContext->mpLogWriter->WriteLn( logValue);
			}
		}
		else
		{
			if (aObjectInfo.musType == MLMT_CHARACTER)
			{
				CLogValue logValue("user props not compiled", aObjectInfo.mszName);
				mpContext->mpLogWriter->WriteLn( logValue);
			}
		}
		bR = JS_SetProperty(cx, jso3DObject, "userProps", &vUserProps);
	}
	else
	{

		if (aObjectInfo.musType == MLMT_CHARACTER)
		{
			CLogValue logValue("user props is null", aObjectInfo.mszName);
			mpContext->mpLogWriter->WriteLn( logValue);
		}
	}

	if (aObjectInfo.musType == MLMT_CHARACTER)
	{
		CLogValue logValue("after user props", aObjectInfo.mszName);
		mpContext->mpLogWriter->WriteLn( logValue);
	}

	// (callback)
	if(aObjectInfo.mCallback.mFunc != NULL){
		pMLEl->SetCreationCallback(&(aObjectInfo.mCallback));
	}

	if (bIsNewObject)
	{
		pMLEl->SetProp("level", (int)aObjectInfo.muLevel);
		if (aObjectInfo.mwcSrc != NULL && aObjectInfo.mwcSrc[ 0] != 0)
		{			
			pMLEl->SetProp("src", aObjectInfo.mwcSrc);
		}		
	}

	if (aObjectInfo.isHighPriority)
	{
		pMLEl->SetProp("highPriority", aObjectInfo.isHighPriority ? "true" : "false");
	}
	
	pMLEl->SetProp("className", aObjectInfo.mszClassName);

	if (bIsNewObject)
	{
		// установить у него нужные свойства
		if(aObjectInfo.mPos.x != 0.0 && aObjectInfo.mPos.y != 0.0 && (aObjectInfo.mwcSrc == NULL || aObjectInfo.mwcSrc[0] == 0)){
			mlRMML3DObject* p3DO = pMLEl->Get3DObject();
			if(p3DO != NULL){
				ml3DPosition pos;
				pos.x = aObjectInfo.mPos.x;
				pos.y = aObjectInfo.mPos.y;
				pos.z = aObjectInfo.mPos.z;
				p3DO->SetPos(pos);				
			}
		}
		if(aObjectInfo.muObjectID != OBJ_ID_UNDEF){
			// (ID)
			pMLEl->muID = aObjectInfo.muObjectID;
			// (bornRealityID)
			ML_ASSERTION(cx, aObjectInfo.muBornRealityID != REALITY_ID_UNDEF, "mlSceneManager::CreateObject. BornRealityID is undefined");
			pMLEl->muBornRealityID = aObjectInfo.muBornRealityID;
			pMLEl->muRealityID = aObjectInfo.muRealityID;
		}
		if (aObjectInfo.mpParent == NULL)
		{
			mlTraceError(cx, "Parent is NULL (set NULL in OBJECT_INFO in creating object)");
			FREE_FOR_GC(cx, mpScene->mjso, jso3DObject);
			GPSM(cx)->SetAllocComment(L"");
			return NULL;
		}
		// добавить объект в сцену
		if(ML_FAILED(pMLEl->SetParent((mlRMMLElement*)aObjectInfo.mpParent))){
			mlTraceError(cx, "Parent is not set (mlSceneManager::CreateObject)");
			FREE_FOR_GC(cx, mpScene->mjso, jso3DObject);
			GPSM(cx)->SetAllocComment(L"");
			return NULL;
		}
		// Регистрируем синхронизируемый объект
		if(aObjectInfo.muObjectID != OBJ_ID_UNDEF)
			scriptedObjectManager.RegisterObject( pMLEl);
		FREE_FOR_GC(cx, mpScene->mjso, jso3DObject);
	}
	else
	{
		// установить у него нужные свойства
		ML_ASSERTION(cx, aObjectInfo.muBornRealityID != REALITY_ID_UNDEF, "mlSceneManager::CreateObject. BornRealityID is undefined");
		scriptedObjectManager.ChangeObjectRealityAndBornReality( pMLEl, aObjectInfo.muRealityID, aObjectInfo.muBornRealityID);
	}

	if (pMLEl->IsSynchronized())
	{
		// Исправление #252.
		// Всё, что изменили в момент создания объекта, на сервер не посылаем,
		// потому что сначала должны отработать изменения, пришедшие с сервера
		// Если с сервера ничего не пришло, то все начальные установки должны быть
		// сделаны при обработке события synch.onInitialized
		pMLEl->DontSynchLastChanges();
		// Конец #252. 

		// Исправление #477.
		if (aObjectInfo.objectFullState == NULL)
		{
			// Если с сервера не пришло полное состояние синхронизированного объекта, то это значит, что на сервере такого
			// объекта нет, объект нужно инициализировать и отправить на сервер полное состояние
			syncObjectState notFoundObjectState;
			notFoundObjectState.type = syncObjectState::ST_NOTFOUND;
			notFoundObjectState.uiID = aObjectInfo.muObjectID;
			notFoundObjectState.uiBornRealityID = aObjectInfo.muBornRealityID;
			notFoundObjectState.uiRealityID = mpContext->mpMapMan->GetCurrentRealityID();
			ApplySyncState( &notFoundObjectState);
		}
		else
		{
			// Обрабатываем полное синхронизируемое состояние, полученное с сервера
			ApplySyncState( aObjectInfo.objectFullState);
		}
		// Конец #477. 

		// TODO Надо определять локацию на сервере
		if( pMLEl->GetMO() && !ML_MO_IS_NULL(pMLEl->GetMO()))
		{
			pMLEl->SetLocationSID( cLPWCSTR(mpContext->mpMapMan->GetObjectLocationName( pMLEl)));
			if( aObjectInfo.musType != MLMT_COMMINICATION_AREA)
			{
				pMLEl->SetCommunicationAreaSID( cLPWCSTR(mpContext->mpMapMan->GetObjectCommunicationAreaName( pMLEl).c_str()));
			}
		}

		HandleElementSynchronization( pMLEl, mpContext->mpSyncMan, mpContext->mpMapMan->GetCurrentRealityID(), mlTime);
		if (aObjectInfo.mCallback.mFunc != NULL)
			aObjectInfo.mCallback.mFunc((mlMedia*)pMLEl, aObjectInfo.mCallback.mpParams);
	}

	MapObjectCreationResultHandlers::iterator it = mapObjectCreationResultHandlers.find(aObjectInfo.muObjectID);
	if ( it != mapObjectCreationResultHandlers.end())
	{
		// Дергаем событие на создание объекта
		mMapCallbackManager->onObjectCreated( (unsigned int)it->second, 
			aObjectInfo.muObjectID, aObjectInfo.muBornRealityID, 0, NULL);
	}

	if (aObjectInfo.musType == MLMT_CHARACTER)
	{
		CLogValue logValue("Object '", aObjectInfo.mszName, "' has been created.");
		mpContext->mpLogWriter->WriteLn( logValue);
	}

	pMLEl->SetCreationCallback(NULL);

//TRACE(cx, " SceneManager::CreateObject of %s for %d ms\n", cLPCSTR(pMLEl->GetName()), GPSM(cx)->GetContext()->mpApp->GetTimeStamp() - tmStart);
	GPSM(cx)->SetAllocComment(L"");

	return pMLEl;
}

// Уведомление пользователя об ошибках и нештатных ситуациях
void mlSceneManager::reportError(const wchar_t *code, ...){
	if(!Updating()){ // в другом потоке (не очень корректное определение того, что в другом потоке. Пока не придумал как это сделать правильно)
		mlSynchData Data;
		GPSM(cx)->SavePlayerInternalPath(Data);

		// сохраняем данные
		Data << L"error";
		Data << code;

		int iParamCntPos = Data.getPos();
		unsigned short iParamCnt = 0;
		Data << iParamCnt;

		va_list ptr;
		va_start(ptr, code);

		const wchar_t *param = va_arg(ptr, const wchar_t*);

		for(uintN i = 0; param; ++i, ++iParamCnt){
			Data << param;
			param = va_arg(ptr, const wchar_t*);
		}

		va_end(ptr);

		Data.setPos(iParamCntPos);
		Data << iParamCnt;
		Data.setPos(); 
		// ставим в очередь событий
		GPSM(cx)->GetContext()->mpInput->AddInternalEvent(EVID_onErrorReport, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(cx)->GetMode() == smmInitilization);
	}
	else
	{
		ResetEvent(); /*??????*/

		jsval v = STRING_TO_JSVAL(JS_NewUCStringCopyZ(GetJSContext(), (const jschar*)L"error"));
		JS_SetProperty(GetJSContext(), GetEventGO(), "type", &v);

		jsval val_code = STRING_TO_JSVAL(JS_NewUCStringCopyZ(GetJSContext(), (const jschar*)code));
		JS_SetProperty(GetJSContext(), GetEventGO(), "code", &val_code);

		va_list ptr;
		va_start(ptr, code);

		const wchar_t *param = va_arg(ptr, const wchar_t*);

		char buf[64];
		for(uintN i = 0; param; ++i){
			//sprintf(buf, "param%d", i);
			rmmlsafe_sprintf(buf, sizeof(buf), 0, "param%d", i);
			jsval val_param = STRING_TO_JSVAL(JS_NewUCStringCopyZ(GetJSContext(), (const jschar*)param));
			JS_SetProperty(GetJSContext(), GetEventGO(), buf, &val_param);
			param = va_arg(ptr, const wchar_t*);
		}

		va_end(ptr);

		NotifyPlayerOnErrorReportListeners();
	}
}

void mlSceneManager::reportWarning(const wchar_t *code, ...){
	if(!Updating()){ // в другом потоке (не очень корректное определение того, что в другом потоке. Пока не придумал как это сделать правильно)
		mlSynchData Data;
		GPSM(cx)->SavePlayerInternalPath(Data);
		// сохраняем данные
		Data << L"warning";
		Data << code;

		int iParamCntPos = Data.getPos();
		unsigned short iParamCnt = 0;
		Data << iParamCnt;

		va_list ptr;
		va_start(ptr, code);

		const wchar_t *param = va_arg(ptr, const wchar_t*);

		for(uintN i = 0; param; ++i, ++iParamCnt){
			Data << param;
			param = va_arg(ptr, const wchar_t*);
		}

		va_end(ptr);

		Data.setPos(iParamCntPos);
		Data << iParamCnt;
		Data.setPos(); 
		// ставим в очередь событий
		GPSM(cx)->GetContext()->mpInput->AddInternalEvent(EVID_onErrorReport, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(cx)->GetMode() == smmInitilization);
	}
	else
	{
		ResetEvent(); /*??????*/

		jsval v = STRING_TO_JSVAL(JS_NewUCStringCopyZ(GetJSContext(), (const jschar*)L"warning"));
		JS_SetProperty(GetJSContext(), GetEventGO(), "type", &v);

		jsval val_code = STRING_TO_JSVAL(JS_NewUCStringCopyZ(GetJSContext(), (const jschar*)code));
		JS_SetProperty(GetJSContext(), GetEventGO(), "code", &val_code);

		va_list ptr;
		va_start(ptr, code);

		const wchar_t *param = va_arg(ptr, const wchar_t*);

		char buf[64];
		for(uintN i = 0; param; ++i){
			//sprintf(buf, "param%d", i);
			rmmlsafe_sprintf(buf, sizeof(buf), 0, "param%d", i);
			jsval val_param = STRING_TO_JSVAL(JS_NewUCStringCopyZ(GetJSContext(), (const jschar*)param));
			JS_SetProperty(GetJSContext(), GetEventGO(), buf, &val_param);
			param = va_arg(ptr, const wchar_t*);
		}

		va_end(ptr);

		NotifyPlayerOnErrorReportListeners();
	}
}

// данные, приходящий с сервака, если там объект не найден
omsresult mlSceneManager::HandleObjectNotFound( unsigned int aObjectID, unsigned int aBornReality, unsigned int aRealityID)
{
	if (mpContext == NULL)
		return OMS_ERROR_FAILURE;

	if (mpContext->mpInput == NULL)
	{
		mlTraceError(cx, "Sync data is not handled because Input is NULL");
		return OMS_ERROR_FAILURE;
	}

	mlSynchData oData;
	oData << aObjectID;
	oData << aBornReality;
	oData << aRealityID;

	mpContext->mpInput->AddCustomEvent( CEID_syncObjectInServerNotFound,
		(unsigned char*)oData.data(), oData.size());

	return OMS_OK;
}

// восстанавливает синхронизацию после потери связи с сервером
omsresult mlSceneManager::RestoreSynchronization( const wchar_t* apwcRequestedUrl)
{
	mlSynchData oData;
	oData << apwcRequestedUrl;
	mpContext->mpInput->AddCustomEvent( CEID_syncRestoreSynchronization, (unsigned char*)oData.data(), oData.size());
	return OMS_OK;
}

// apwcRequestUrl - URL в которую должен телепортироваться мой аватар после разрыва
void mlSceneManager::RestoreSynchronizationInternal( const wchar_t* apwcRequestedUrl)
{
	if (mpMyAvatar == NULL)
	{
		mpContext->mpSyncMan->ConnectToWorld( apwcRequestedUrl, NULL);
		return;
	}

	// Передаем на сервер состояние моего аватара и команду на подключение моего аватара к текущей реальности
	syncObjectState oState;
	mpMyAvatar->GetForcedState( oState);
	mpContext->mpSyncMan->ConnectToWorld( apwcRequestedUrl, &oState);

	if (mpContext->mpMapMan != NULL)
		mpContext->mpMapMan->ActivateZones();

	// Посылаем локацию при восстановлении связи
/*		if (mpContext->mpMapMan && mpContext->mpSyncMan)
	{
		uint auiObjectID, auiBornRealityID;
		mpContext->mpMapMan->GetSynchID(auiObjectID, auiBornRealityID);

		wchar_t *locationName = (wchar_t *)mpContext->mpMapMan->GetCurrentLocationName();
		unsigned short size = wcslen(locationName);
		wchar_t * pwcLocationName = MP_NEW_ARR( wchar_t, size+1);
		rmmlsafe_wcscpy(pwcLocationName, size+1, 0, (wchar_t *)mpContext->mpMapMan->GetCurrentLocationName());

		//mpContext->mpSyncMan->AvatarChangeLocation(locationName , auiObjectID , auiBornRealityID);
	}*/

	// Запрашиваем полное состояние всех аватаров. Исправление #84
	ObjectsVector avatars;
	scriptedObjectManager.GetRealAvatars( avatars);
	ObjectsVector::const_iterator it = avatars.begin();
	for (;  it != avatars.end();  it++)
	{
		mlRMMLElement* pAvatar = (*it);
		if (pAvatar == NULL)
			continue;
		if (pAvatar == mpMyAvatar)
			continue;
		QueryElementFullState( pAvatar, mpContext->mpSyncMan, mlTime, 0);
	}
	// Конец исправление #84
}

mlRMMLElement* mlSceneManager::GetScriptedObjectByID( unsigned int auObjID, unsigned int auRealityID, bool aAccurately)
{
	return scriptedObjectManager.GetObject( auObjID, auRealityID, aAccurately);
}

/*void mlSceneManager::ChangeObjectBornReality( mlMedia* apMedia, unsigned int auBornRealityID)
{
	mlRMMLElement* pMLEl = (mlRMMLElement*) apMedia;

	if (pMLEl->IsAvatar())
	{
		CLogValue logValue(
			"[RMML.OBJECTS] ChangeObjectBornReality - ",
			(const char*)cLPCSTR( pMLEl->GetName())
			);
		mpContext->mpLogWriter->WriteLn( logValue);
	}

	scriptedObjectManager.ChangeObjectBornReality( pMLEl, auBornRealityID);
}*/

omsresult mlSceneManager::DeletePreviousRealityObjects(const std::vector<unsigned int>& aExcludeObjects)
{
	GUARD_JS_CALLS(cx);

	if (mpMyAvatar == NULL)
		return OMS_OK;

	CLogValue logValue(
		"[RMML.OBJECTS]: DeletePreviousRealityObjects"
		);
	mpContext->mpLogWriter->WriteLn( logValue);

	ObjectsVector vObjectsToDelete;
	scriptedObjectManager.GetObjects( vObjectsToDelete);

	ObjectsVector::iterator itObjectsToDelete = vObjectsToDelete.begin();
	for( ; itObjectsToDelete != vObjectsToDelete.end(); itObjectsToDelete++)
	{
		mlRMMLElement* pMLEl = (*itObjectsToDelete);
		if (pMLEl == NULL)
			continue;
		if (pMLEl == mpMyAvatar)
			continue;
		if (!pMLEl->IsSynchronized())
			continue;
		if (pMLEl->IsExclusiveOwn() || pMLEl->IsAvatar())
		{
			DeleteObjectInternal( pMLEl->GetID(), pMLEl->GetBornRealityID());
			continue;
		}

		std::vector<unsigned int>::const_iterator it = std::find(aExcludeObjects.begin(), aExcludeObjects.end(), pMLEl->GetID());
		if (it != aExcludeObjects.end())
			continue;

		DeleteObjectInternal( pMLEl->GetID(), pMLEl->GetBornRealityID());
	}

	return OMS_OK;
}

void mlSceneManager::DeleteObjectInternal( unsigned int auObjectID, unsigned int auBornRealityID)
{
	mlRMMLElement* pMLEl = scriptedObjectManager.GetObject( auObjectID, auBornRealityID);	
	if(pMLEl == NULL){
		return;
	}
	if (mpContext->mpMapMan->IsAvatar( pMLEl)){
		mlTraceError( cx, "[RMML.OBJECTS] Can not delete my avatar (objectID: %u\n)", auObjectID);
		return;
	}

	CLogValue logValue(
		"[RMML.OBJECTS] Object ", (LPCSTR)cLPCSTR( pMLEl->GetName()), " deleted"
		);
	mpContext->mpLogWriter->WriteLnLPCSTR( logValue.GetData());

	DeleteObject( pMLEl, NULL);
}

// определение существования объекта
bool mlSceneManager::IsObjectExist(unsigned int auObjectID, unsigned int auBornRealityID)
{
	mlRMMLElement* pMLEl = scriptedObjectManager.GetObject( auObjectID, auBornRealityID);	
	if(pMLEl == NULL){
		return false;
	}

	return true;
}

// удаляет объект
omsresult mlSceneManager::DeleteObject( unsigned int auObjectID, unsigned int auBornRealityID)
{
	if (mpContext == NULL)
		return OMS_ERROR_FAILURE;


	if (mpContext->mpInput == NULL)
	{
		mlTraceError(cx, "Sync data is not handled because Input is NULL");
		return OMS_ERROR_FAILURE;\
	}

	mlSynchData oData;
	oData << auObjectID;
	oData << auBornRealityID;

	mpContext->mpInput->AddCustomEvent( CEID_syncDeleteObject,
		(unsigned char*)oData.data(), oData.size());
	return OMS_OK;
}

// удаляет объект
omsresult mlSceneManager::DeleteObject( mlMedia* apObject, moMedia** *appaObjects)
{
	GUARD_JS_CALLS(cx);
	mlRMMLElement* pMLEl = (mlRMMLElement*)apObject;

	if( pMLEl != NULL && pMLEl->GetClass()!=NULL && wcscmp(pMLEl->GetClass()->GetName(), L"panel_object") == 0)
		int ii=0;

	if (mpContext->mpMapMan->IsAvatar( pMLEl))
	{
		CLogValue logValue("[ERROR][RMML.OBJECTS]: Can not delete my avatar in mlSceneManager::DeleteObject");
		mpContext->mpLogWriter->WriteLn( logValue);
		return OMS_ERROR_FAILURE;
	}

	NotifyLinksElemDestroying(pMLEl);

	// вызвать функцию изменения статуса
	jsval vFunc=JSVAL_NULL;
	#define ON_MODESTROYING_FUNC_NAME L"onDestroying"
	if(JS_GetUCProperty(cx, pMLEl->mjso, (const jschar*)ON_MODESTROYING_FUNC_NAME, -1, &vFunc))
	{
		if(JSVAL_IS_REAL_OBJECT(vFunc)){
			if(JS_ObjectIsFunction(cx,JSVAL_TO_OBJECT(vFunc))){
				unsigned char ucArgC = 0;
				jsval* vArgs=NULL;
				jsval v;
				JS_CallFunctionValue(cx, pMLEl->mjso, vFunc, ucArgC, vArgs, &v);
			}
		}
	}

	ResetMouse(pMLEl);
	scriptedObjectManager.UnregisterObject( pMLEl);
	pMLEl->SetParent( NULL);
	ResetMouse(pMLEl);
	if(appaObjects == NULL){
		pMLEl->ForceMODestroy();
	}else{
		mvMOToDeleteOut.clear();
		pMLEl->ForceMODestroy(&mvMOToDeleteOut);
		mvMOToDeleteOut.push_back(NULL);
		*appaObjects = &mvMOToDeleteOut[0];
	}
	CallGC(pMLEl);
	//LogRefs( cx, NULL, pMLEl->mjso);
	// ?? на объект могут остаться прямые ссылки, 
	// поэтому нужен механизм постепенного их нахождения и удаления

	CLogValue logValue("[RMML.OBJECTS]: Object '", (const char*)cLPCSTR( pMLEl->GetName()), "' has been deleted.");
	mpContext->mpLogWriter->WriteLn( logValue);
	return OMS_OK;
}

// обрабатывает сообщение от другого клиента
omsresult mlSceneManager::HandleSyncMessage(unsigned int auObjectID, unsigned int auBornRealityID, const wchar_t* apwcSender, unsigned char* apData, unsigned short aiDataSize)
{
	if (mpContext == NULL || mpContext->mpInput == NULL)
		return OMS_ERROR_FAILURE;

	mlSynchData oData;
	oData << auObjectID;
	oData << auBornRealityID;
	if(apwcSender == NULL)
		oData << L"";
	else
		oData << apwcSender;
	oData.put(apData, aiDataSize);

	mpContext->mpInput->AddCustomEvent( CEID_syncHandleMessage, (unsigned char*)oData.data(), oData.size());
	return OMS_OK;
}

// возвращает список инклудов
int mlSceneManager::ExtractIncludes( const wchar_t* apcFileSrc, const wchar_t** appIncludes, int count)
{
	return mScriptPreloader->ExtractIncludes( apcFileSrc, appIncludes, count);
}

void mlSceneManager::HandleSyncMessageInternal(mlSynchData& aData){
	unsigned int uObjectID = OBJ_ID_UNDEF;
	unsigned int uBornRealityID = 0;
	aData >> uObjectID;
	aData >> uBornRealityID;
	mlRMMLElement* pMLEl = scriptedObjectManager.GetObject(uObjectID, uBornRealityID);
	wchar_t* pwcSender = L"";
	aData >> pwcSender;
	pMLEl->HandleSynchMessage(pwcSender, aData);
}

// захватить мышь (все обработчики мыши будут вызываться только у этого объекта)
bool mlSceneManager::CaptureMouse(mlRMMLElement* apMLEl){
	if(apMLEl == NULL){
		return ReleaseMouse(NULL);
	}
	mCaptureMouseObject.mpButton = apMLEl;
	mCaptureMouseObject.mpVisible = NULL;
	if(apMLEl->GetVisible() == NULL)
		mCaptureMouseObject.mpVisible = mBUM.mpVisible;
	return false;
}

bool mlSceneManager::ReleaseMouse(mlRMMLElement* apMLEl){
	if(apMLEl == NULL || mCaptureMouseObject.mpButton == apMLEl){
	mCaptureMouseObject.mpButton = NULL;
	mCaptureMouseObject.mpVisible = NULL;
	}
	return false;
}

void mlSceneManager::SetLogin(const wchar_t* apwcLogin)
{
	GUARD_JS_CALLS(cx);
	JSString* jss=JS_NewUCStringCopyZ(cx,(const jschar*)apwcLogin);
	JS_DefineProperty(cx, mjsoPlayer, "login", STRING_TO_JSVAL(jss), NULL, NULL, 0);
}

void mlSceneManager::SetPassword(const wchar_t* apwcPassword)
{
	GUARD_JS_CALLS(cx);
	JSString* jss=JS_NewUCStringCopyZ(cx,(const jschar*)apwcPassword);
	JS_DefineProperty(cx, mjsoPlayer, "password", STRING_TO_JSVAL(jss), NULL, NULL, 0);
}

void mlSceneManager::SetPasswordEncoding(int iPasswordEncodingMode)
{
	GUARD_JS_CALLS(cx);		
	JS_DefineProperty(cx, mjsoPlayer, "bEncoding", INT_TO_JSVAL(iPasswordEncodingMode), NULL, NULL, 0);
}

void mlSceneManager::SetLoginKey(const wchar_t* apwcLoginKey)
{
	GUARD_JS_CALLS(cx);
	JSString* jss=JS_NewUCStringCopyZ(cx,(const jschar*)apwcLoginKey);
	JS_DefineProperty(cx, mjsoPlayer, "loginKey", STRING_TO_JSVAL(jss), NULL, NULL, 0);
}

void mlSceneManager::SetUpdaterKeyByThread(const wchar_t* apwcUpdaterKey)
{
	mlSynchData oData;
	oData << apwcUpdaterKey;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_updaterKey, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::OnUpdaterKeyByThreadInternal( mlSynchData &oData)
{
	wchar_t *updaterKey = NULL;
	oData >> updaterKey;
	SetUpdaterKey( updaterKey);
}

void mlSceneManager::SetUpdaterKey(const wchar_t* apwcUpdaterKey)
{
	GUARD_JS_CALLS(cx);
	JSString* jss=JS_NewUCStringCopyZ(cx,(const jschar*)apwcUpdaterKey);
	JS_DefineProperty(cx, mjsoPlayer, "updaterKey", STRING_TO_JSVAL(jss), NULL, NULL, 0);
}

extern UrlProperties	urlStorage;
void mlSceneManager::SetLocationUrl(const wchar_t* apwcURL)
{
	GUARD_JS_CALLS(cx);
	JSString* jss=JS_NewUCStringCopyZ(cx,(const jschar*)apwcURL);
	JS_DefineProperty(cx, mjsoPlayer, "startURL", STRING_TO_JSVAL(jss), NULL, NULL, 0);
	msStartURL = apwcURL;
}

void mlSceneManager::SetLocationUrlAndTeleport(const wchar_t* apwcURL)
{
	SetLocationUrl( apwcURL);	
	mlSynchData oData;
	oData << apwcURL;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_mapURLChanged, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::SetLocationUrl(const wchar_t* apwcLocation, const wchar_t* apwcPlace
									, float x, float y, float z, unsigned long zoneID
									, const wchar_t* apwcAction, const wchar_t* apwcActionParam)
{
	if( apwcLocation && wcslen(apwcLocation)<255)
		//wcscpy( urlStorage.wcLocation, apwcLocation);
		rmmlsafe_wcscpy(urlStorage.wcLocation, URL_BUFFERS_SIZE, 0, apwcLocation);
	else	
		urlStorage.wcLocation[0] = 0;

	if( apwcPlace && wcslen(apwcPlace)<255)	
		//wcscpy( urlStorage.wcPlace, apwcPlace);
		rmmlsafe_wcscpy(urlStorage.wcPlace, URL_BUFFERS_SIZE, 0, apwcPlace);
	else	
		urlStorage.wcPlace[0] = 0;

	if( apwcAction && wcslen(apwcAction)<255)	
		//wcscpy( urlStorage.wcAction, apwcAction);
		rmmlsafe_wcscpy(urlStorage.wcAction, URL_BUFFERS_SIZE, 0, apwcAction);
	else	
		urlStorage.wcAction[0] = 0;

	if( apwcActionParam && wcslen(apwcActionParam)<255)
		//wcscpy( urlStorage.wcActionParam, apwcActionParam);
		rmmlsafe_wcscpy(urlStorage.wcActionParam, URL_BUFFERS_SIZE, 0, apwcActionParam);
	else	
		urlStorage.wcActionParam[0] = 0;

	urlStorage.x = x;
	urlStorage.y = y;
	urlStorage.z = z;
	urlStorage.zoneID = zoneID;
	if( mjsoUrl)
	{
		NotifyUrlOnSet();
	}
}

const wchar_t* mlSceneManager::GetLocationURL()
{
	/*
	GUARD_JS_CALLS(cx);

	jsval v;
	JS_GetProperty(cx,mjsoPlayer,"startURL",&v);
	if(JSVAL_IS_STRING(v))
		return JS_GetStringChars(JSVAL_TO_STRING(v));

	return NULL;*/
	return msStartURL.c_str();
}

// существует ли определение для класса rmml-объектов
bool mlSceneManager::IsJSObjClassDefined(const wchar_t* apwcClassName){
	GUARD_JS_CALLS(cx);
	jsval vClss;
	JS_GetProperty(cx, JS_GetGlobalObject(cx), GJSONM_CLASSES, &vClss);
	if(!JSVAL_IS_REAL_OBJECT(vClss)) return false;
	JSObject* jsoClasses = JSVAL_TO_OBJECT(vClss);
	// получаем объект класса
	jsval vc = JSVAL_NULL;
	JS_GetUCProperty(cx, jsoClasses, (const jschar*)apwcClassName, -1, &vc);
	if(JSVAL_IS_REAL_OBJECT(vc)) return true;
	return false;
}

#ifdef ML_TRACE_ALL_ELEMS
void ElementPrecreated(mlRMMLElement* apElem){
	gmapAllElems.insert(ExistingRMMLElementPair(apElem, 1));
}
#endif

void mlSceneManager::ElementCreating(mlRMMLElement* apMLEl){
	apMLEl->mCountId = mCountIds;
	++mCountIds;
#ifdef TRACE_LOST_ELEMS
	UpdateNotInScenesElems(apMLEl);
#endif
	AddElemToList(mvNewElements, apMLEl, false);
}

bool mlSceneManager::RemoveFromList(v_elems& avElements, mlRMMLElement* apMLEl){
	v_elems::iterator vi = avElements.begin();
	v_elems::iterator viEnd = avElements.end();
	int iCnt = 0;
	for(; vi != viEnd; ++vi){
		if(*vi == apMLEl){
			*vi = NULL;
			iCnt++;
//			return true;
		}
	}
	//return false;
if(iCnt > 1)
mlTrace(apMLEl->mcx, "RemoveFromList: iCnt > 1 %s\n", apMLEl->_name?cLPCSTR(apMLEl->mpName):"??");
	return (iCnt > 0);
}

int CountListEmptySlots(v_elems& avElements){
	v_elems::iterator vi = avElements.begin();
	v_elems::iterator viEnd = avElements.end();
	int iCnt = 0;
	for(; vi != viEnd; ++vi){
		if(*vi == NULL){
			iCnt++;
		}
	}
	return iCnt;
}

void mlSceneManager::DumpList(v_elems& avElements, const wchar_t* apwcListName){
	int iEmptySlotCount = CountListEmptySlots(avElements);
	mlTrace(cx, "[RMML] %S size: %d (%d empty)\n", apwcListName, avElements.size(), iEmptySlotCount);
}

// Выводит в лог статистику по спискам
void mlSceneManager::DumpLists(){
	DumpList(mvNewElements, L"mvNewElements");
	DumpList(mvUpdatableElements, L"mvUpdatableElements");
	DumpList(mvUpdatableUIElements, L"mvUpdatableUIElements");
	DumpList(mvObjectsToSynch, L"mvObjectsToSynch");
}

void mlSceneManager::OptimizeList(v_elems& avElements){
	// считаем количество элементов в векторе
	v_elems::iterator vi = avElements.begin();
	v_elems::iterator viEnd = avElements.end();
	int iCnt = 0;
	for(; vi != viEnd; ++vi){
		if(*vi != NULL){
			iCnt++;
		}
	}
	if(iCnt == avElements.size())
		return;
	if(iCnt == 0){
		avElements.empty();
		return;
	}
	// переносим все элементы из той части списка, которая будет удалена в начало на пустые места
	v_elems::iterator viEmpty = avElements.begin();
	for(; viEmpty != viEnd; ++viEmpty){
		if(*viEmpty == NULL)
			break;
	}
	for(unsigned int i = iCnt; i < avElements.size(); i++){
		if(avElements[i] != NULL){
			if(viEmpty == viEnd){ // так не должно быть в принципе
				//#ifdef _DEBUG
				assert(false);
				//#endif
				return;	
			}
			*viEmpty = avElements[i];
			avElements[i] = NULL;
			for(; viEmpty != viEnd; ++viEmpty){
				if(*viEmpty == NULL)
					break;
			}
		}
	}
	avElements.resize(iCnt);
}

void mlSceneManager::ElementDestroying(mlRMMLElement* apMLEl){
//mlTrace(cx, "ElementDestroying: %s\n", apMLEl->_name!=NULL?cLPCSTR(apMLEl->mpName):"??");

	if ((apMLEl) && (apMLEl->GetIVisible() == mBUM.mpVisible))
	{
		mBUM.mpVisible = NULL;
	}

	mvDeletedInUpdate.push_back(apMLEl->mCountId);
	SceneStructChanged();
	if(apMLEl->GetScene() == mpUI){
		muiUIElementCount--;
	}else{
		muiSceneElementCount--;
	}
#ifdef TRACE_LOST_ELEMS
	UpdateNotInScenesElems(apMLEl, true);
#endif
	#ifdef ML_TRACE_ALL_ELEMS
	gmapAllElems.erase(apMLEl);
	#endif
	RemoveFromList(mvNewElements, apMLEl);
	if(!RemoveFromList(mvUpdatableElements, apMLEl))
		RemoveFromList(mvUpdatableUIElements, apMLEl);
	mbUpdatableElementsChanged = true;
	NotifyLinksElemDestroying(apMLEl);
	if(RemoveFromList(mvObjectsToSynch, apMLEl)){
		// Tandy (03.09.2009): однажды плеер упал на том, что возможно в mvObjectsToSynch осталась ссылка на дубликат, который непонятным образом туда попал
		// поэтому проверяем на всякий случай на наличие повторных ссылок
		while(RemoveFromList(mvObjectsToSynch, apMLEl));
	}
	ResetMouse(apMLEl);
	if(apMLEl->muID != OBJ_ID_UNDEF)
	{
		if(apMLEl->IsAvatar())
		{
			char strPtr[ 20];
			CLogValue logValue(
				"[RMML.OBJECTS] ElementDestroying - ",
				(const char*)cLPCSTR( apMLEl->GetName()),
				" (0x",
				_itoa_s((int)apMLEl, strPtr, 20, 16),
				")"
				);
			mpContext->mpLogWriter->WriteLn( logValue);
		}
		scriptedObjectManager.UnregisterObject(apMLEl);	// Tandy (01.06.2010): так-то вызывается на parent=null, 
										// но почему-то не во всех случаях, 
										// поэтому на всякий случай повторяем вызов
										//? возможно стоит оттуда убрать, а здесь оставить
	}
}

// у rmml-элемента parent был null, а стал не null: добавим его в список новых объектов
void mlSceneManager::MayBeAddedToScene(mlRMMLElement* apMLEl){ 
	if(apMLEl->mpChildren == NULL){
		AddElemToList(mvNewElements, apMLEl, true);
	}else{
		mlRMMLIterator iter(cx, apMLEl, true, true);
		mlRMMLElement* pElem = NULL;
		while((pElem = iter.GetNext()) != NULL){
			AddElemToList(mvNewElements, pElem, true);
		}
	}
}

// у rmml-элемента parent был не null, а стал null: уберем его из списков
void mlSceneManager::MayBeRemovedFromScene(mlRMMLElement* apMLEl){
	if(apMLEl->mpChildren == NULL){
		RemoveFromList(mvNewElements, apMLEl);
	}else{
		// ? Tandy: есть потенциальная проблема, что parent могут установить заново, но из списков-то объект уже будет удален. Для этого введен mlSceneManager::MayBeAddedToScene
		// ? Tandy: возможно стоит его убрать и из других списков
		// и все его дочерние объекты тоже надо убрать
		mlRMMLIterator iter(cx, apMLEl, true, true);
		mlRMMLElement* pElem = NULL;
		while((pElem = iter.GetNext()) != NULL){
			RemoveFromList(mvNewElements, pElem);
		}
	}
	apMLEl->UnlinkFromEventSenders();
	// и сразу удалим медиа-объекты (включая дочерние)
	apMLEl->ForceMODestroy();
}

// выводит инфу о ссылках на заданные объекты в trace
// abLost - выводить инфу только по потерянным RMML-элементам (parent==null)
// aiType - 0: все, 1: 3D-объекты, 2: 3D-персонажи, 3: 3D-группы, 4: композиции
// apwcObjName - имя объекта
bool mlSceneManager::TraceObjectsRefs(bool abLost, int aiType, const wchar_t* apwcObjName){
	mlTrace(cx, "begin TraceObjectsRefs\n");
	mlRMMLElement* pElem = NULL;
	mlRMMLIterator Iter(cx, mpScene, false, true);
	MapNotInScenesElems::iterator mi;
	if(abLost){
		// перепроверяем все "потерянные элементы"
		RecheckNotInScenesElems();
		mi = mmapNotInScenesElems.begin();
		pElem = mi->first;
	}else{
		pElem = Iter.GetNext();
	}
	mlRMMLElement* pNextElem = NULL;
	for(; pElem != NULL; pElem = pNextElem){
		if(abLost){
			mi++;
			if(mi == mmapNotInScenesElems.end())
				pNextElem = NULL;
			else
				pNextElem = mi->first;
		}else{
			pNextElem = Iter.GetNext();
		}
		switch(aiType){
		case 0:
			break;
		case 1: // любые 3D-объекты
			if(pElem->GetI3DObject() == NULL)
				continue;
			break;
		case 2: // 3D-персонажи
			if(pElem->mRMMLType != MLMT_CHARACTER)
				continue;
			break;
		case 3: // 3D-группы
			if(pElem->mRMMLType != MLMT_GROUP)
				continue;
			break;
		case 4: // композиции
			if(pElem->mRMMLType != MLMT_COMPOSITION)
				continue;
			break;
		}
		if(abLost && pElem->mpParent != NULL)
			continue;
		if(apwcObjName != NULL && *apwcObjName != L'\0'){
			if(!isEqual(pElem->GetName(), apwcObjName))
				continue;
		}
		if(abLost || pElem->mpParent == NULL)
			mlTrace(cx, "Lost object ");
		else 
			mlTrace(cx, "Object ");
		TRACE(cx, "'%s'", cLPCSTR(pElem->GetName())); 
		TRACE(cx, "(");
		JSClass* pClass = ML_JS_GETCLASS(cx, pElem->mjso);
		if(pClass != NULL)
			TRACE(cx, "%s", pClass->name);
		if(pElem->mpClass != NULL)
			TRACE(cx, ", %s", cLPCSTR(pElem->mpClass->GetName()));
		TRACE(cx, ")");
		TRACE(cx, " references:\n");
		JSBool bRet = TraceRefs(cx, JS_GetGlobalObject(cx), pElem->mjso);
//		JSObject* objFound = NULL;
//		jschar* jscPropName = NULL;
//		FindJSORef(cx, JS_GetGlobalObject(cx), pElem->mjso, objFound, jscPropName);
		int hh=0;
		// ??
	}
	// ??
	mlTrace(cx, "end TraceObjectsRefs\n");
	return true;
}

// Выводит в лог список RMML-элементов, которые не в иерархии сцены, но JS-ссылка на них имеется
void mlSceneManager::TraceLeakedElements(){
	mlTrace(cx, "[RMML] mvNewElements root elements:\n");
	v_elems::iterator vi = mvNewElements.begin();
	v_elems::iterator viEnd = mvNewElements.end();
	for(; vi != viEnd; ++vi){
		mlRMMLElement* pElem = *vi;
		if(pElem == NULL)
			continue;
		if(pElem->IsClassChildren())
			continue;
		mlMedia* pParentScene = pElem->GetScene();
		if(pParentScene == NULL){
			if(pElem->mpParent == NULL){ // trace-им только "элементы" верхнего уровня
				mlString wsType = cLPWCSTR(mlRMML::GetTypeName(pElem->GetType()).c_str());
				mlString wsName = pElem->GetName();
				mlString wsClass;
				if(pElem->mpClass != NULL){
					//if(isEqual(pElem->mpClass->mpName, L"CUserRightsListItem")){
					//	int jj=0;
					//}
					wsClass = pElem->mpClass->GetName();
				}
				mlString wsSrc;
				const wchar_t* pwcSrc = mSrcFileColl[pElem->miSrcFilesIdx];
				if(pwcSrc != NULL) wsSrc = pwcSrc;
				mlTrace(cx, "[RMML] type: %s, name: %s, class: %s, src: %s, line: %d\n", cLPCSTR(wsType.c_str()), cLPCSTR(wsName.c_str()), cLPCSTR(wsClass.c_str()), cLPCSTR(wsSrc.c_str()), pElem->miSrcLine);
				//mlTrace(cx, "mvNewElements root item refs begin:\n");
				//TraceRefs(cx, JS_GetGlobalObject(cx), pElem->mjso);
				//mlTrace(cx, "mvNewElements root item refs end.\n");
			}
			continue;
		}
	}
}

// Выводит в лог список обновляемых RMML-элементов (у которых вызывается Update() на каждый update sceneManager-а)
void mlSceneManager::TraceUpdatableElements(){
	mlTrace(cx, "[RMML] mvUpdatableElements elements:\n");
	v_elems::iterator vi = mvUpdatableElements.begin();
	v_elems::iterator viEnd = mvUpdatableElements.end();
	for(; vi != viEnd; ++vi){
		mlRMMLElement* pElem = *vi;
		if(pElem == NULL)
			continue;
		mlString wsType = cLPWCSTR(mlRMML::GetTypeName(pElem->GetType()).c_str());
		//mlString wsName = pElem->GetName();
		mlString wsFullPath = pElem->GetFullPath();
		mlString wsClass;
		if(pElem->mpClass != NULL){
			wsClass = pElem->mpClass->GetName();
		}
		mlString wsSrc;
		const wchar_t* pwcSrc = mSrcFileColl[pElem->miSrcFilesIdx];
		if(pwcSrc != NULL) wsSrc = pwcSrc;
		mlTrace(cx, "[RMML] type: %s, path: %s, class: %s, src: %s, line: %d\n", cLPCSTR(wsType.c_str()), cLPCSTR(wsFullPath.c_str()), cLPCSTR(wsClass.c_str()), cLPCSTR(wsSrc.c_str()), pElem->miSrcLine);
		//mlTrace(cx, "mvNewElements root item refs begin:\n");
		//TraceRefs(cx, JS_GetGlobalObject(cx), pElem->mjso);
		//mlTrace(cx, "mvNewElements root item refs end.\n");
	}
}

// вывести в tracer инфу об объекте, который под мышкой
void mlSceneManager::TraceObjectUnderMouse(){
	GUARD_JS_CALLS(cx);
	mlRMMLElement* pElem = GetVisibleAt(GetMouseXY());
	if(pElem == NULL)
		return;
	mlTrace(cx, "[RMML] Visible object under mouse: %s\n", cLPCSTR(pElem->GetFullPath().c_str()));
}

void mlSceneManager::RecheckNotInScenesElems(){
	MapNotInScenesElems::iterator mi;
	mi = mmapNotInScenesElems.begin();
	int size1 = mmapNotInScenesElems.size();
	bool iteratorUpdated;
	for(; mi != mmapNotInScenesElems.end();){
		iteratorUpdated = false;
		mlRMMLElement* pElem = mi->first;
		if(pElem->GetScene() != NULL){
			mi = mmapNotInScenesElems.erase(mi);
			iteratorUpdated = true;
		}else{
			// классы тоже удалить
			if(mmapClasses.find(pElem) != mmapClasses.end()){
				mi = mmapNotInScenesElems.erase(mi);
				iteratorUpdated = true;
			}
			// и сцену удалить
			if(pElem == mpScene || pElem == mpUI){
				mi = mmapNotInScenesElems.erase(mi);
				iteratorUpdated = true;
			}
		}
		if( !iteratorUpdated)
			mi++;
	}
}

void mlSceneManager::UpdateNotInScenesElems(mlRMMLElement* apElem, bool abRemove, bool abClass){
	if(abRemove || abClass){
		if(abClass)
			apElem->SetClassFlag();
		RemoveFromList(mvNewElements, apElem);
	}
#ifdef TRACE_LOST_ELEMS 
	if(abClass)
		mmapClasses[apElem] = 1;
	if(apElem->GetParent() == NULL && !abRemove){
		mmapNotInScenesElems[apElem] = 1;
		return;
	}else
		abRemove = true;
	if(abRemove){
		MapNotInScenesElems::iterator mi = mmapNotInScenesElems.find(apElem);
		if(mi != mmapNotInScenesElems.end()){
			mmapNotInScenesElems.erase(mi);
		}
	}
#endif
}

void mlSceneManager::OnObjectInServerNotFound(mlSynchData& aData)
{
	unsigned int objectID,bornRealityID, realityID;
	aData >> objectID;
	aData >> bornRealityID;
	aData >> realityID;

	mlRMMLElement* pMLEl = scriptedObjectManager.GetObject(objectID, bornRealityID);
	
	if (pMLEl==NULL)
		return;

	mlSynchData oEventData;
	oEventData << objectID;
	oEventData << bornRealityID;
	oEventData << realityID;
	oEventData << (short) 0;
	oEventData << (unsigned char) 0;
	oEventData << (short) 0;
	//HandleSynchData2( pMLEl, oEventData);

	pMLEl->ForceWholeStateSending();
}

mlRMMLElement* mlSceneManager::GetMyAvatar(){
	return mpMyAvatar;
}

mlMedia* mlSceneManager::GetVisibleUnderNouse(){
	return (mBUM.mpVisible!=NULL)?mBUM.mpVisible->GetElem_mlRMMLVisible():NULL;
}

void mlSceneManager::TrackLink(mlSynchLinkJSO* apLink){
	VecSynchLinks::iterator vi = mvTrackingLinks.begin();
	for(; vi != mvTrackingLinks.end(); vi++){
		if(*vi == apLink)
			return;
		if(*vi == NULL){
			*vi = apLink;
			return;
		}
	}
	mvTrackingLinks.push_back(apLink);
}

void mlSceneManager::LinkDestroying(mlSynchLinkJSO* apLink){
	VecSynchLinks::iterator vi = mvTrackingLinks.begin();
	for(; vi != mvTrackingLinks.end(); vi++){
		if(*vi == apLink){
			*vi = NULL;
		}
	}
}

void mlSceneManager::NotifyLinksElemDestroying(mlRMMLElement* apElem){
	VecSynchLinks::iterator vi = mvTrackingLinks.begin();
	for(; vi != mvTrackingLinks.end(); vi++){
		mlSynchLinkJSO* pLink = *vi;
		if(pLink == NULL)
			continue;
		if(pLink->GetRef() == apElem->mjso){
			pLink->UpdateRef(true);
			*vi = NULL; // больше не надо отслеживать
		}
	}
}

void mlSceneManager::OutRMMLObjectTreeByIter(mlRMMLIterator& iter, std::ostream* aOStream, mlRMMLElement* apElem){
	if(apElem != NULL){
		mlString sElemPath = apElem->GetFullPath();
		(*aOStream) << cLPCSTR(sElemPath.c_str());
		(*aOStream) << "\n";
	}
	mlRMMLElement* pElem=NULL;
	while((pElem = iter.GetNext()) != NULL){
		if(!pElem->IsJSO())
			continue;
		int iDepth = iter.GetDepth();
		for(; iDepth > 0; iDepth--){
			(*aOStream) << "    ";
		}
		mlString sElemPath = pElem->GetFullPath();
		(*aOStream) << cLPCSTR(sElemPath.c_str());
		(*aOStream) << "\n";
		//pElem->GlobalTimeShifted(alTime, alTimeShift, mePrevMode);
	}
}

void mlSceneManager::OutRMMLObjectTree(std::ostream* aOStream, int aiMode){
	jsval vClasses = NULL;
	JS_GetProperty(cx, JS_GetGlobalObject(cx), GJSONM_CLASSES, &vClasses);
	JSObject* jsoClasses = JSVAL_TO_OBJECT(vClasses);
	JSIdArray* pGPropIDArr= JS_Enumerate(cx, jsoClasses);
	for(int ikl=0; ikl<pGPropIDArr->length; ikl++){
		jsid id=pGPropIDArr->vector[ikl];
		jsval v;
		if(!JS_IdToValue(cx,id,&v)) continue;
		jschar* jsc = NULL;
		jsval vProp;
		if(JSVAL_IS_STRING(v)){
			jsc = JS_GetStringChars(JSVAL_TO_STRING(v));
			JS_GetUCProperty(cx,jsoClasses,jsc,-1,&vProp);
		}else if(JSVAL_IS_INT(v)){
			int iId = JSVAL_TO_INT(v);
			JS_GetElement(cx, jsoClasses, iId, &vProp);
			jsc = (jschar*)iId;
		}else continue;
		if(!JSVAL_IS_MLEL(cx, vProp)) continue;
		mlRMMLElement* pElem = JSVAL_TO_MLEL(cx, vProp);
		mlRMMLIterator iter(cx, pElem);
		OutRMMLObjectTreeByIter(iter, aOStream, pElem);
	}
	JS_DestroyIdArray(cx, pGPropIDArr);

	mlRMMLIterator IterUI(cx,mpUI);
	OutRMMLObjectTreeByIter(IterUI, aOStream, mpUI);
	if(mpScene != NULL){
		mlRMMLIterator IterScn(cx,mpScene);
		OutRMMLObjectTreeByIter(IterScn, aOStream, mpScene);
	}
	// ??
}

// Исправление #621
bool checkFilterObjectTreeByIter(mlRMMLElement* pElem, int aiMode)
{
	if (aiMode > 0)
	{
		int res1 = aiMode & 1;
		int res2 = aiMode & 2;
		if (res1 == 1)
		{
			if ( !pElem->IsSynchronized() || (pElem->GetType() == MLMT_CHARACTER && res2 != 2))
				return false;
		}
		else if( res2 == 2){
			if( pElem->GetType() != MLMT_CHARACTER)
				return false;
		}			
	}
	return true;
}
// Конец #621

// Исправление #621
void PutRMMLObjectToTree( std::vector<mlObjectInfo> &avElements, mlRMMLElement* apElem)
{
			mlObjectInfo info;
			info.id = apElem->GetID();
			info.name = apElem->GetName();
			info.src =  apElem->GetFullPath();
	if (apElem->mpParent != NULL)
		info.parent = apElem->mpParent->GetFullPath();
			avElements.push_back( info);
		}
// Конец #621

void GetRMMLObjectTreeByIter(mlRMMLIterator& iter, std::vector<mlObjectInfo> &avElements, mlRMMLElement* aRootElem, int aiMode)
{
	std::vector<mlObjectInfo> tmpvElements;
	tmpvElements.clear();
	if(aRootElem != NULL)
	{
		if( checkFilterObjectTreeByIter( aRootElem, aiMode))
		{
			// Исправление #621
			PutRMMLObjectToTree( tmpvElements, aRootElem);
			// Конец #621
	}
	}
	mlRMMLElement* pElem=NULL;
	while((pElem = iter.GetNext()) != NULL){
		if(!pElem->IsJSO())
			continue;
		//int iDepth = iter.GetDepth();
		
		if( !checkFilterObjectTreeByIter( pElem, aiMode))
			continue;
		
		// Исправление #621
		PutRMMLObjectToTree( tmpvElements, pElem);
		// Исправление #621
	}
	avElements = tmpvElements;
}

void mlSceneManager::GetRMMLObjectTree( std::vector<mlObjectInfo> &avElements, int aiMode){
	// Исправление #621
	/*
	GUARD_JS_CALLS(cx);
	jsval vClasses = NULL;
	JS_GetProperty(cx, JS_GetGlobalObject(cx), GJSONM_CLASSES, &vClasses);
	JSObject* jsoClasses = JSVAL_TO_OBJECT(vClasses);
	JSIdArray* pGPropIDArr= JS_Enumerate(cx, jsoClasses);
	for (int ikl=0; ikl<pGPropIDArr->length; ikl++)
	{
		jsid id=pGPropIDArr->vector[ikl];
		jsval v;
		if(!JS_IdToValue(cx,id,&v)) continue;
		jschar* jsc = NULL;
		jsval vProp;
		if(JSVAL_IS_STRING(v)){
			jsc = JS_GetStringChars(JSVAL_TO_STRING(v));
			JS_GetUCProperty(cx,jsoClasses,jsc,-1,&vProp);
		}else if(JSVAL_IS_INT(v)){
			int iId = JSVAL_TO_INT(v);
			JS_GetElement(cx, jsoClasses, iId, &vProp);
			jsc = (jschar*)iId;
		}else continue;
		if(!JSVAL_IS_MLEL(cx, vProp)) continue;
		mlRMMLElement* pElem = JSVAL_TO_MLEL(cx, vProp);
		mlRMMLIterator iter(cx, pElem);
		GetRMMLObjectTreeByIter(iter, avElements, pElem, aiMode);
	}
	JS_DestroyIdArray(cx, pGPropIDArr);

	mlRMMLIterator IterUI(cx,mpUI);
	GetRMMLObjectTreeByIter(IterUI, avElements, mpUI, aiMode);
	*/
	if (mpScene != NULL)
	{
		mlRMMLIterator IterScn(cx,mpScene);
		GetRMMLObjectTreeByIter(IterScn, avElements, mpScene, aiMode);
	}
	// Конец #621
}

void mlSceneManager::CopyObjectUnderMouseToGallery(){
	GUARD_JS_CALLS(cx);
	// узнать какой RMML-объект под мышкой
	mlIVisible* pVisible = NULL;
	mlRMMLElement* pSceneElement = GetSceneVisibleAt(GetMouseXY(), &pVisible);
	mlString sObjStringID;
	// если он является viewport-ом
	if(pVisible != NULL){
		mlRMMLElement* pVisibleElem = (mlRMMLElement*)pVisible->GetMLMedia();
		if(pVisibleElem->mRMMLType == MLMT_VIEWPORT){
			moMedia* pMO = pVisibleElem->GetMO();
			if(!PMO_IS_NULL(pMO)){
				moIViewport* pViewport = pMO->GetIViewport();
				if(pViewport){
					// то получить строковый идентификатор 3D-объекта 
					mlPoint pntVAbsXY = pVisibleElem->GetVisible()->GetAbsXY();
					mlPoint pntXY = GetMouseXY();
					pntXY.x -= pntVAbsXY.x;
					pntXY.y -= pntVAbsXY.y;
					mlOutString sOutStringID;
					if(pViewport->GetObjectStringIDAt(pntXY, sOutStringID)){
						sObjStringID = sOutStringID.mstr;
					}
				}
			}
		}
	}
	// и вызвать JS-функцию Player.UI.__copyObjectToGallery(stringID)
	mlString sScript = L"Player.UI.__copyObjectToGallery(\"";
	sScript += sObjStringID;
	sScript += L"\");";
	jsval v = JSVAL_NULL;
	JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx ), (const jschar*)sScript.c_str(), sScript.size(), "mlSceneManager::CopyObjectUnderMouseToGallery", 1, &v);
	int hh=0;
}

unsigned int mlSceneManager::GetCurSeanceID()
{
	return pSeance->GetSeanceID();
}

bool mlSceneManager::isCurSeancePlaying()
{
	return pSeance->isPlaying();
}

void mlSceneManager::SetSeancePlaying(bool playing)
{
	pSeance->setPlaying(playing);
}

// Исправление #621
void AddNewLine(mlString& sRetVal)
{
	sRetVal += L"\r\n";
}

void AddIndent(mlString& sRetVal, int iIndent, wchar_t* sIndentStr)
{
	for (int i = 0;  i < iIndent;  i++)
		sRetVal += sIndentStr;
}
// Конец #621

mlString mlSceneManager::JSVal2String(JSContext* mcx, jsval vProp, int iFormatOptions, int iIndent, wchar_t* sIndentStr)
{
	if(JSVAL_IS_INT(vProp)){
		int iVal = JSVAL_TO_INT(vProp);
		wchar_t pwcBuf[20];
		swprintf_s(pwcBuf, 20, L"%d", iVal);
		return pwcBuf;
	}else if(JSVAL_IS_BOOLEAN(vProp)){
		bool bVal=JSVAL_TO_BOOLEAN(vProp);
		if(bVal){
			return L"true";
		}else
			return L"false";
	}else if(JSVAL_IS_STRING(vProp)){
		jschar* jsc = JS_GetStringChars(JSVAL_TO_STRING(vProp));
		mlString sRetVal;
		sRetVal += L"'";
		sRetVal += (const wchar_t*)jsc;
		sRetVal += L"'";
		return sRetVal;
	}else if(JSVAL_IS_NULL(vProp)){
		return L"null";
	}else if(JSVAL_IS_VOID(vProp)){
		return L"undefined";
	}else if(JSVAL_IS_DOUBLE(vProp)){
		double dVal = *JSVAL_TO_DOUBLE(vProp);
		wchar_t pwcBuf[100];
		swprintf_s(pwcBuf, 100, L"%6.3f", dVal);
		return pwcBuf;
	}else if(JSVAL_IS_OBJECT(vProp)){
		if(JS_TypeOfValue(mcx,vProp)==JSTYPE_FUNCTION){
			JSObject* jsoFun = JSVAL_TO_OBJECT(vProp);
			JSClass* pClass = ML_JS_GETCLASS(mcx, jsoFun);
			JSFunction* jsf = JS_ValueToFunction(mcx, vProp);
			if(jsf){
				JSString* jssFun=JS_DecompileFunction(mcx, jsf, 0);
				jschar* jsc=JS_GetStringChars(jssFun);
				return (const wchar_t*)jsc;
			}else{
				return L"?function?";
			}
		}else{
			JSObject* jsoP = JSVAL_TO_OBJECT(vProp);
			JSClass* pClass = ML_JS_GETCLASS(mcx, jsoP);
			// Исправление #621
			if(pClass == NULL){
				return L"?wrong object?";
			}
			// Конец #621
			if(isEqual(pClass->name, "Link") && mlSynchLinkJSO::IsInstance(mcx, jsoP)){
				mlSynchLinkJSO* pSynchLink = (mlSynchLinkJSO*)JS_GetPrivate(mcx, jsoP);
				mlString sRetVal = L"Link(";
				JSObject* jsoRef = pSynchLink->GetRef();
				if(jsoRef == NULL || !(JSO_IS_MLEL(mcx, jsoRef))){
					wchar_t pwcBuf[30];
					unsigned int uiBornRealityID = 0; 
					unsigned int uiID = pSynchLink->GetObjectID(uiBornRealityID);
					swprintf_s(pwcBuf, 30, L"%d:%d", uiBornRealityID, uiID);
					sRetVal += pwcBuf;
				}else{
					mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(mcx, jsoRef);
					mlString sStringID = pMLEl->GetStringID();
					sRetVal += sStringID;
				}
				sRetVal += L")";
				return sRetVal;
			}
			// Исправление #621
			bool bArray = isEqual(pClass->name, "Array");
			mlString sRetVal;
			bool bMultiline = false;
				if(bArray){
					sRetVal = L"[";
				if ((iFormatOptions & JSVS_MULTILINE_ARRAY) != 0)
					bMultiline = true;
				}else{
				sRetVal = L"{";
				if ((iFormatOptions & JSVS_MULTILINE_OBJECT) != 0)
					bMultiline = true;
			}
			JSIdArray* pGPropIDArr = JS_Enumerate(mcx, jsoP);
			int iLen = pGPropIDArr->length;
			if (iLen == 0)
				bMultiline = false;
			if (bMultiline)
				AddNewLine(sRetVal);
			iIndent++;
			if(!bArray && (iFormatOptions & JSVS_CLASS) != 0){
				if (bMultiline)
					AddIndent(sRetVal, iIndent, sIndentStr);
					sRetVal += L"(";
					sRetVal += cLPWCSTRq(pClass->name);
					sRetVal += L") ";
				if (bMultiline)
					AddNewLine(sRetVal);
				}
			for(int ii = 0; ii < iLen; ii++){
				jsid id = pGPropIDArr->vector[ii];
				jsval v;
				if(!JS_IdToValue(cx, id, &v))
				{
					
				}
				jschar* jscProp = NULL;
				jsval vProp2;
				if (bMultiline)
					AddIndent( sRetVal, iIndent, sIndentStr);
				if(JSVAL_IS_STRING(v)){
					jscProp=JS_GetStringChars(JSVAL_TO_STRING(v));
					std::string strProp = "";
					jschar* it = jscProp;
					for ( ; *((char *)it) != '\0'; ++it)
					{	
						strProp += (const char *)it;
					}
					if ( strProp != "parent" && strProp != "_parent")
					{
						sRetVal += (const wchar_t*)jscProp;
						sRetVal += L":";
						JS_GetUCProperty(cx, jsoP, jscProp, -1, &vProp2);
						sRetVal += JSVal2String(mcx, vProp2, iFormatOptions, iIndent, sIndentStr);
					}
				}else if(JSVAL_IS_INT(v)){
					int iIdx = JSVAL_TO_INT(v);
					JS_GetElement(cx, jsoP, JSVAL_TO_INT(v), &vProp2);
					sRetVal += JSVal2String(mcx, vProp2, iFormatOptions, iIndent, sIndentStr);
				}
				if(ii < (iLen - 1))
				{
					sRetVal += L", ";
			}
				if (bMultiline)
					AddNewLine(sRetVal);
			}
			iIndent--;
			if (bMultiline)
				AddIndent( sRetVal, iIndent, sIndentStr);
			if(bArray){
				sRetVal += L"]";
			}else
				sRetVal += L"}";
			// Конец #621
			return sRetVal;
		}
	}else{ // unknown jsval type
		JSType jst=JS_TypeOfValue(mcx,vProp);
	}
	return L"??";
}

// Исправление #621
void AddObjectIDs(mlString& aStrProps, mlRMMLElement* elem, int aIndent, wchar_t* aIndentStr)
{
	if (elem->GetID() == OBJ_ID_UNDEF)
		return;

	aStrProps += L",";
	AddNewLine( aStrProps);
	AddIndent( aStrProps, aIndent, aIndentStr);
	char strIDs[ 200];
	rmmlsafe_sprintf(strIDs, sizeof(strIDs), 0, "objectID: %u, bornReailityID: %u, reailityID: %u",
		elem->GetID(), elem->GetBornRealityID(), elem->GetRealityID());
	aStrProps += cLPWCSTR( strIDs);

	AddNewLine( aStrProps);
	AddIndent( aStrProps, aIndent, aIndentStr);
	char strVersions[ 200];
	rmmlsafe_sprintf(strVersions, sizeof(strVersions), 0, "serverVersion: %u, clientVersion: %u, hash: %u",
		elem->GetServerVersion(), elem->GetClientVersion(), elem->GetLastEvaluatedHash());
	aStrProps += cLPWCSTR( strVersions);
}

void AddPosition(mlString& aStrProps, mlRMMLElement* elem, int aIndent, wchar_t* aIndentStr)
	{
	mlRMML3DObject* object3D = elem->Get3DObject();
	if (object3D != NULL)
		{
		aStrProps += L",";
		AddNewLine( aStrProps);
		ml3DPosition position = object3D->GetAbsolutePosition();
		AddIndent( aStrProps, aIndent, aIndentStr);
		char strPosition[ 70];
		//sprintf( strPosition, "position: { x: %4.2f, y: %4.2f, z: %4.2f}", position.x, position.y, position.z);
		rmmlsafe_sprintf(strPosition, sizeof(strPosition), 0, "position: { x: %4.2f, y: %4.2f, z: %4.2f}", position.x, position.y, position.z);
		aStrProps += cLPWCSTR( strPosition);
		return;
	}
	mlRMMLVisible* visible = elem->GetVisible();
	if (visible != NULL)
	{
		aStrProps += L",";
		AddNewLine( aStrProps);
		mlPoint position = visible->GetAbsXY();
		AddIndent( aStrProps, aIndent, aIndentStr);
		char strPosition[ 40];
		//sprintf( strPosition, "position: { x: %4.2f, y: %4.2f}", position.x, position.y);
		rmmlsafe_sprintf(strPosition, sizeof(strPosition), 0, "position: { x: %4.2f, y: %4.2f}", position.x, position.y);
		aStrProps += cLPWCSTR( strPosition);
		return;
	}
}

void mlSceneManager::TransposeBinState(const wchar_t* apwcTarget)
{
	mlRMMLElement* elem = (mlRMMLElement*)GetObjectByTarget( apwcTarget);
	if (elem == NULL)
		return;
	elem->SetForcedUpdateBinState(true);
	elem->ReadyForBinState();
	elem->SetForcedUpdateBinState(false);
}

wchar_t* sIndentStr = L"    ";

std::wstring mlSceneManager::GetJsonFormatedProps(const wchar_t* apwcTarget, int aIndent)
{
	GUARD_JS_CALLS(cx)

	std::wstring strProps = L"";
	mlRMMLElement* elem = (mlRMMLElement*)GetObjectByTarget( apwcTarget);
	if (elem == NULL)
		return strProps;

	AddIndent( strProps, aIndent, sIndentStr);
	strProps += L"{";
	AddNewLine( strProps);
	aIndent++;
	AddIndent( strProps, aIndent, sIndentStr);
	strProps += L"target: ";
	strProps += apwcTarget;

	if (elem->GetID() != OBJ_ID_UNDEF)
	{
		AddObjectIDs( strProps, elem, aIndent, sIndentStr);
		AddPosition( strProps, elem, aIndent, sIndentStr);
	}

	if (elem->IsSynchronized())
	{
		strProps += L",";
		AddNewLine( strProps);

		AddNewLine( strProps);
		AddIndent( strProps, aIndent, sIndentStr);
		strProps += L"synch: {";
		aIndent++;

		AddNewLine( strProps);
		AddIndent( strProps, aIndent, sIndentStr);
		strProps += L"sysProperties: ";

		mlSynch* pSynch = ((mlRMMLElement *)elem)->mpSynch;
		strProps += pSynch->GetSysPropertiesJson( aIndent, sIndentStr);
		strProps += L",";

		aIndent--;
		jsval synchObjVal = OBJECT_TO_JSVAL(elem->mpSynch->mjso);
		mlString strSynchProps = JSVal2String(cx, synchObjVal, JSVS_MULTILINE_OBJECT, aIndent, sIndentStr);
		if (strSynchProps.size() > 2)
			{
			strSynchProps.erase(0, 1);
			strProps += strSynchProps;
			}
		else
		{
			AddNewLine( strProps);
			AddIndent( strProps, aIndent, sIndentStr);
			strProps += L"}";
		}
	}

	AddNewLine( strProps);
	aIndent--;
	AddIndent( strProps, aIndent, sIndentStr);
	strProps += L"}";

	return strProps;
}
// Конец #621

std::wstring mlSceneManager::GetJsonSynchedProps(const wchar_t* apwcTarget){
	std::wstring strProps = L"";
	mlMedia * elem = GetObjectByTarget( apwcTarget);

	if (elem == NULL || !elem->IsSynchronized())
		return L"";

	jsval synchObjVal = OBJECT_TO_JSVAL(((mlRMMLElement *)elem)->mpSynch->mjso);
	strProps += JSVal2String(cx, synchObjVal);

/*	mlPropertyValues values;
	((mlRMMLElement *)elem)->GetAllProperties(values);
	std::vector< mlPropertyValues::mlPropertyValue * >::iterator it;
	int i = 0;
	for (it = values.mvPropVals.begin(); it != values.mvPropVals.end(); ++it)
	{
		mlPropertyValues::mlPropertyValue * value2 = *it;
		strProps += cLPWCSTR(value2->name);
		strProps += L":";
		jsval val = value2->val;
		mlString str = JSVal2String(cx, val);
		strProps += str.c_str();
		strProps += L",";
		i++;
	}*/
	
	strProps += L"[";

	v_elems * pv =  ((mlRMMLElement *)elem)->GetSynchedChildren();
	if (pv != NULL && pv != ML_SYNCH_CHILDREN_EMPTY_VAL)
	{
		v_elems::const_iterator vi;
		for(vi=pv->begin(); vi != pv->end(); vi++ )
		{
			mlRMMLElement* pMLEl=*vi;
			std::wstring name = pMLEl->GetName();
			strProps += name;
			strProps += L":";
			std::wstring childrenFullTargetName = apwcTarget;
			childrenFullTargetName += L".";
			childrenFullTargetName += pMLEl->GetName();
			wstring prop = GetJsonSynchedProps(childrenFullTargetName.c_str());
			if (prop == L"")
				prop = L"null";
			strProps += prop;
			strProps += L",";
		}
	}
	strProps += L"]";

	return strProps;
}

void mlSceneManager::AddObjToDeleteLater(void* apObj, int aiType){
#ifdef JS_THREADSAFE
	if(mpUtilityThread.get() != NULL){
		mlGuardElemsToDelete oGuard(mpUtilityThread.get());
		if(aiType == 1){
			mvElemsToDelete.push_back((mlRMMLElement*)apObj);
		}else{
			mvObjectsToDelete.push_back((mlJSClass*)apObj);
		}
		return;
	}
#endif
	if(aiType == 1){
		mlRMMLElement *pObj = (mlRMMLElement*)apObj;
		pObj->destroy();		
	}else{
		mlJSClass *pObj = (mlJSClass*)apObj;
		pObj->destroy();
	}
}

// приложение стало активным
void mlSceneManager::OnAppActivated(){
	GUARD_JS_CALLS(cx);
	// если при деактивации фокус был на элементе
	if(mFocusedButInternalPath.size() > 0){
		// который еще существует
		// и через скрипт у него фокус ввода не забирали
		mlRMMLElement* pButFocused = LoadAndCheckInternalPath(mFocusedButInternalPath);
		// то вернуть ему фокус ввода
		SetFocus(pButFocused, true);
		if(pButFocused != NULL)
			FREE_FOR_GC(cx, JS_GetGlobalObject(cx), pButFocused->mjso);
	}
	mFocusedButInternalPath.clear();
	NotifyPlayerListeners(EVID_onActivated);
}

// приложение перестало быть активным
void mlSceneManager::OnAppDeactivated(){
	GUARD_JS_CALLS(cx);
	// убрать фокус ввода с элемента, у которого он сейчас есть
	mFocusedButInternalPath.clear();
	if(mpButFocused != NULL){
		mlRMMLElement* pButFocused = mpButFocused;
		// за время пока приложение свернуто, GC может успеть прибрать этот элемент, поэтому:
		SAVE_FROM_GC(cx, JS_GetGlobalObject(cx), pButFocused->mjso);
		SetFocus(NULL, true);
		SaveInternalPath(mFocusedButInternalPath, pButFocused);
	}
	NotifyPlayerListeners(EVID_onDeactivated);
}

// выполнить предзагрузку и компиляцию include-ов для скрипта
// чтобы когда он реально стал грузиться, то грузился бы быстрее
mlresult mlSceneManager::Preload(const wchar_t* asSrc)
{
	mScriptPreloader->AddScript(asSrc, true);
	return ML_OK;
}

// Приложение пытаются закрыть
// Если вернет false, то приложение не надо закрывать.
bool mlSceneManager::OnClose(){
	if(mjsoPlayer == NULL)
		return true;
	if(mpUI == NULL)
		return true;

	if( GetContext() && GetContext()->mpServiceWorldMan)
	{
		// пользователь выбрал закрытие окна
		GetContext()->mpServiceWorldMan->SetSessionStateValue( service::CSS_EXIT_CODE, 10/*см. ui.xml, EXIT_CODE_EXIT_MESSAGE_SHOWN*/);
	}

	GUARD_JS_CALLS(cx);
	// если есть свойство onClose
	jsval vFunc = JSVAL_NULL;
	mlString sEventFunc = L"onClose";
	if(JS_GetUCProperty(cx, mpUI->mjso, (const jschar*)sEventFunc.c_str(), sEventFunc.length(), &vFunc) == JS_FALSE)
		return true;
	if(!JSVAL_IS_REAL_OBJECT(vFunc))
		return true;
	if(!JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(vFunc)))
		return true;
	jsval v = JSVAL_TRUE;
	if(!JS_CallFunctionValue(cx, mpUI->mjso, vFunc, 0, NULL, &v))
		return true;
	if(v != JSVAL_FALSE)
		return true;
	return false;	// функция вернула false: не надо закрывать приложение
}

mlString toString(int aiValue){
	wchar_t pwcBuf[20];
	swprintf_s(pwcBuf, 20, L"%d", aiValue);
	return pwcBuf;
}

// вывести в tracer и лог глубины 2D-объекта под мышкой и его родителей
void mlSceneManager::TraceUnderMouseObjectDepth(){
	GUARD_JS_CALLS(cx);
	mlRMMLElement* pElem = GetVisibleAt(GetMouseXY());
	if(pElem == NULL){
		mlTrace(cx, "[RMML] 2D object under mouse not found\n");
		return;
	}
	if(!pElem->GetVisible()){
		mlTrace(cx, "[RMML] Object under mouse (%s) is not visible\n", pElem->GetFullPath());
		return;
	}
	mlString sLogStr = L"[RMML] \n";
	
	for(int iTreeDepth = 0; pElem != NULL; iTreeDepth++, pElem = pElem->GetParent()){
		for(int i = 0; i < iTreeDepth; i++){
			sLogStr += L"  ";
		}
		sLogStr += pElem->GetName();
		sLogStr += L": depth=";
		sLogStr += toString(pElem->GetVisible()->GetDepth());
		sLogStr += L", absdepth=";
		sLogStr += toString(pElem->GetVisible()->GetAbsDepth());
		sLogStr += L"\n";
	}
	mlTrace(cx, "%s", cLPCSTR(sLogStr.c_str()));
}

typedef vector<mlString> StringVector;

StringVector SplitString(mlString aString, mlString aSplitter){
	StringVector retv;
	mlString::size_type posBegin = 0;
	for(;;){
		mlString sSubst;
		mlString::size_type pos = aString.find(aSplitter, posBegin);
		if(pos != mlString::npos){
			sSubst = aString.substr(posBegin, pos - posBegin);
			if(!sSubst.empty())
				retv.push_back(sSubst);
			posBegin = pos + aSplitter.size();
		}else{
			sSubst = aString.substr(posBegin);
			if(!sSubst.empty())
				retv.push_back(sSubst);
			break;
}
	}
	return retv;
}

// получить JavaScript call stack в виде строки
mlString mlSceneManager::GetJSCallStack(){
/*
	mlString sCS = L"??";
	JSStackFrame* fp = NULL;
	fp = JS_FrameIterator(cx, &fp);
	for(;fp != NULL; fp = JS_FrameIterator(cx, &fp)){
		JSScript* jssScript = JS_GetFrameScript(cx, fp);
		if(jssScript == NULL){
			sCS += L"[jssScript == NULL]";
		}else{
			const char* lpcFileName = JS_GetScriptFilename(cx, jssScript);
			sCS += cLPWCSTR(lpcFileName);
unsigned int uiLineNum = JS_GetScriptBaseLineNumber(cx, jssScript);
			wchar_t pwcBuf[20];
			swprintf(pwcBuf, L"%d", uiLineNum);
			sCS += L":";
			sCS += pwcBuf;
//unsigned int uiLineExtent = JS_GetScriptLineExtent(cx, jssScript);
//			swprintf(pwcBuf, L"%d", uiLineExtent);
//			sCS += L":";
//			sCS += pwcBuf;
		}
		sCS += L"\n";
	}
*/

	jsval v;
	mlString sScript = L"(new Error())";
	//JSBool bR=JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), sScript.c_str(), sScript.length(), "GetJSCallStack", 1, &v);
	JSStackFrame* fp = NULL;
	fp = JS_FrameIterator(cx, &fp);
	if(fp == NULL)
		return L"";
	fp = JS_FrameIterator(cx, &fp);
	if(fp == NULL)
		return L"";
	JSObject *jsoCallObject = JS_GetFrameCallObject(cx, fp);
	JSBool bR = JS_EvaluateUCInStackFrame(cx, fp, (const jschar*)sScript.c_str(), sScript.length(), "GetJSCallStack", 1, &v);
	if(!bR)
		return L"[Cannot create Error object]";
	if(!JSVAL_IS_REAL_OBJECT(v))
		return L"[Cannot create Error object]";
	JSObject* jsoError = JSVAL_TO_OBJECT(v);
	JS_GetUCProperty(cx, jsoError, (const jschar*)L"stack", -1, &v);
	if(!JSVAL_IS_STRING(v))
		return L"[Cannot get Error.stack]";
	JSString* jssCS = JSVAL_TO_STRING(v);
	mlString sCS = (const wchar_t*)JS_GetStringChars(jssCS);

	StringVector vCS = SplitString(sCS, L"\n");
	sCS = L"";
	vCS.erase(vCS.begin(), vCS.begin()+2);
	StringVector::const_iterator cvi = vCS.begin();
	for(; cvi != vCS.end(); cvi++){
		sCS += *cvi;
		sCS += L"\n";
	}
	return sCS;
}

// Обрабатывает ответ от сервера по созданию объекта
void mlSceneManager::OnObjectCreationResult( unsigned int uErrorCode, unsigned int uObjectID, unsigned int uBornRealityID, unsigned int auRmmlHandlerID)
{
	if( uErrorCode == 0)
	{
		// Связываем обработчик и номер объекта. Когда он создастся - обработчик вызовется
		mapObjectCreationResultHandlers[uObjectID] = auRmmlHandlerID;
	}
	else
	{
		// Дергаем событие на создание объекта
		mMapCallbackManager->onObjectCreated( auRmmlHandlerID, uObjectID, uBornRealityID, uErrorCode, NULL);
	}
}

// Приостановка посылки синхронизируемых состояний при телепортации
bool mlSceneManager::IsSynchUpdatingSuspended()
{
	return m_isSynchUpdatesSuspended;
}

void mlSceneManager::OnPreciseURL( const wchar_t* aURL)
{
	mlSynchData oData;
	oData << aURL;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onPreciseURL, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::OnSeancesInfo( const wchar_t* aSeancesInfo, bool bFull)
{
	mlSynchData oData;
	oData << aSeancesInfo;
	oData << bFull;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onSeancesInfo, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::OnEventsInfoIntoLocation( const wchar_t* aEventsInfoIntoLocation)
{
	mlSynchData oData;
	oData << aEventsInfoIntoLocation;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onEventsInfoIntoLocation, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::OnProxyAuthError(const wchar_t* aSettingName, int aProxyType, const wchar_t* aProxyServer)
{
	mlSynchData oData;
	oData << aSettingName;
	oData << aProxyType;
	oData << aProxyServer;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onProxyAuthError, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::OnConnectToServerError(int errorCode)
{
	mlSynchData oData;
	oData << errorCode;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onConnectToServerError, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::OnMapObjectsLoaded()
{
	GUARD_JS_CALLS(cx);
	m_synchObjectsInRealityCreated = 0;
	m_synchObjectsInRealityCount = GetContext()->mpSyncMan->GetNearestMapObjectCount();
	m_realityLoadedEventWasGeneratedAlready = false;
	NotifyBeforeCreatingOfObjects();
}

void mlSceneManager::ShowZoneEvent( const bool abVal)
{
	mlSynchData oData;
	oData << abVal;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_mapShowZone, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::SetInfoState( int idInfoState)
{
	mlSynchData oData;
	oData << idInfoState;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onShowInfoState, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::OnLogout( int iCode)
{
	mlSynchData oData;
	oData << iCode;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onLogout, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::OnNotifyServerTOInfo( int minutes)
{
	mlSynchData oData;
	oData << minutes;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onNotifyServerTOInfo, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::SuspendSynchStatesSending()
{
	m_isSynchUpdatesSuspended = true;
}

void mlSceneManager::ResumeSynchStatesSending()
{
	m_isSynchUpdatesSuspended = false;
}

// Инициализирует объект класса
void mlSceneManager::InitClassObject(mlRMMLElement* apClass){
	// Создаем общий для всех экземпляров класса объект classCommon
	JSObject* jsoClassCommon = JS_NewObject(cx, NULL, NULL, NULL);
	JS_DefineProperty(cx, apClass->mjso, MLELPN_CLASSCOMMON, OBJECT_TO_JSVAL(jsoClassCommon), NULL, NULL, JSPROP_READONLY); // JSPROP_ENUMERATE | 
}

void mlSceneManager::OnPreciseURLInternal(mlSynchData& aData)
{
	wchar_t* url;
	aData >> url;
	mlString sUrl( url);
	mpContext->mpComMan->ConnectToWorld( sUrl.c_str());
}

JSObject* mlSceneManager::LocationToJSObject(const ml3DLocation& aLocation, bool abWithParams)
{
	JSObject* jsoLocation = JS_NewObject(cx,NULL,NULL,NULL);
	SAVE_FROM_GC(cx, JS_GetGlobalObject(cx), jsoLocation);
	jsval v = OBJECT_TO_JSVAL(jsoLocation);
	JSString* jssLocationID = JS_NewUCStringCopyZ(cx, (const jschar*)aLocation.ID.mstr.c_str());
	v = STRING_TO_JSVAL(jssLocationID);
	JS_SetProperty(cx, jsoLocation, "id", &v);
	if(abWithParams){
		jsval va[1];
		v = OBJECT_TO_JSVAL(JS_NewArrayObject(cx, 0, va));
		JS_SetProperty(cx, jsoLocation, "params", &v);
		if(!aLocation.params.mstr.empty()){
			mlString sParams(L"[");
			sParams = sParams + aLocation.params.mstr + L"]";

			unsigned int uLength = sParams.size();
			JSBool bR = JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), (const jschar*)sParams.c_str(), uLength, "LocationToJSObject", 1, &v);
			if (JSVAL_IS_REAL_OBJECT(v)){
				JS_SetProperty(cx, jsoLocation, "params", &v);
			}
		}
	}
	// ??
	FREE_FOR_GC(cx, JS_GetGlobalObject(cx), jsoLocation);
	return jsoLocation;
}

// вызывается, когда на окно плеера перетащили файл
void mlSceneManager::OnWindowFileDrop(const wchar_t* apwcFilePath, int x, int y)
{
	if (mpContext->mpLogWriter)
		mpContext->mpLogWriter->WriteLnLPCSTR( "[RMML]: OnWindowFileDrop");

	GUARD_JS_CALLS(cx);
	mlSynchData oData;
	oData << apwcFilePath;
	oData << x;
	oData << y;
	MP_NEW_P4(temp, mlNextUpdatesEvent, oData, CEID_playerFileDroped, 4, true);
	mvlNUEvents.push_back(temp);
	//mpContext->mpInput->AddCustomEvent(CEID_playerFileDroped, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::OnWindowFileDropInternal(mlSynchData& aData)
{
	if (mpContext->mpLogWriter)
		mpContext->mpLogWriter->WriteLnLPCSTR( "[RMML]: mlSceneManager OnWindowFileDropInternal");

	wchar_t* pwcFilePath = NULL;
	aData >> pwcFilePath;
	int x = 0, y = 0;
	aData >> x;
	aData >> y;
	mlString wsFilePath(pwcFilePath);
	mlPoint pnt;
	pnt.x = x;
	pnt.y = y;

	mlRMMLElement* pElemUnderMouse = NULL;
	if(mBUM.mp3DObject){
		pElemUnderMouse = mBUM.mp3DObject;
	}else if(mBUM.mpVisible){
		pElemUnderMouse = mBUM.mpVisible->GetElem_mlRMMLVisible();
	}
	else 
	{
		pElemUnderMouse = mpButUnderMouse;
	}

	pElemUnderMouse = GPSM(cx)->GetVisibleAt(pnt);

	if(pElemUnderMouse != NULL){
		if (!pElemUnderMouse->OnFileDrop(wsFilePath.c_str()))
		{
			for(; pElemUnderMouse != NULL; pElemUnderMouse = pElemUnderMouse->mpParent){
				if (pElemUnderMouse->OnFileDrop(wsFilePath.c_str()))
				{
					break;
				}
			}
		}
		else
		{
			if (mpContext->mpLogWriter)
				mpContext->mpLogWriter->WriteLnLPCSTR( "[RMML]: mlSceneManager OnWindowFileDropInternal pElemUnderMouse->OnFileDrop");
		}
	}
	else
	{
		if (mpContext->mpLogWriter)
			mpContext->mpLogWriter->WriteLnLPCSTR( "[RMML]: mlSceneManager OnWindowFileDropInternal pElemUnderMouse == NULL");
	}

	if(mBUM.mpVisible){
		mlRMMLElement* pMLElViewport = mBUM.mpVisible->GetElem_mlRMMLVisible();
		if(pMLElViewport->mRMMLType == MLMT_VIEWPORT){
			mlRMMLViewport* pViewport = (mlRMMLViewport*)pMLElViewport;
			pViewport->SetTraceObjectOnMouseMove(false);
		}
	}
}

// вызывается, когда от пользователя не приходили больше 5 минут события мыши и клавиатуры или он свернул окно  (спящий режим)
void mlSceneManager::OnUserSleep()
{
	if (mpContext->mpLogWriter)
		mpContext->mpLogWriter->WriteLnLPCSTR( "[RMML]: OnUserSleep");

	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onUserSleep, NULL, 0);
}

void mlSceneManager::OnWindowMoved()
{
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onWindowMoved, NULL, 0);
}

void mlSceneManager::OnWindowSizeChanged( const int& auClientWidth, const int& auClientHeight)
{
	mlSynchData oData;
	oData << auClientWidth;
	oData << auClientHeight;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onWindowSizeChangedExternally, (unsigned char*)oData.data(), oData.size());
}

bool mlSceneManager::OnControllerFound()
{
	if (m_ctrl)
	{
		return true;
	}

	//В проекте возникают ошибки, если использовать Unicode
	//Поэтому используется ANSI-string
	char buffer[MAX_PATH];
	GetModuleFileName(nullptr, buffer, MAX_PATH);
	std::string::size_type pos = ((std::string)(buffer)).find_last_of("\\/");
	std::string path = ((std::string)(buffer)).substr(0, pos);
	path += "/XBOXController.dll";

	hGamepadDll = LoadLibrary(path.c_str());
	if (hGamepadDll != nullptr)
	{
		NewController = (newControllerType)GetProcAddress(hGamepadDll, "NewController");
		if (NewController)
		{
			m_ctrl = NewController();
			m_pressedButton = 0;
			m_prevButtons = 0;
			m_leftThumbState = ThumbState::NEUTRAL;
			m_rightThumbState = ThumbState::NEUTRAL;

			if (m_ctrl->IsConnected())
			{
				return true;
			}
		}
		OnControllerLost(); // в случае неудачи - удаляем
	}

	return false;
}

void mlSceneManager::OnControllerLost()
{
	if (m_ctrl)
	{
		delete m_ctrl;
		m_ctrl = nullptr;
	}

	if (hGamepadDll)
	{
		FreeLibrary(hGamepadDll);
		hGamepadDll = nullptr;

		NewController = nullptr;
	}

	m_leftThumbState = -1;
	m_rightThumbState = -1;
}

void mlSceneManager::OnOculusRiftFound()
{
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onOculusRiftFound, NULL, 0);
}

void mlSceneManager::OnKinectFound()
{
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onKinectFound, NULL, 0);
}

// вызывается, когда пользователь уходил в спящий режим и вернулся
void mlSceneManager::OnUserWakeUp()
{
	if (mpContext->mpLogWriter)
		mpContext->mpLogWriter->WriteLnLPCSTR( "[RMML]: OnUserSleep");

	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onUserWakeUp, NULL, 0);
}

// Случился timeout выполнения JS-скрипта (вызывается из mlScriptTerminationThread)
// abInterrupted=true, если выполнение прервано
// aiTimeout - сколько времени в ms прошло с начала выполнения скрипта
void mlSceneManager::JSScriptExecutionTimeout(bool abInterrupted, int aiTimeout){
//	mlTraceWarning(cx, "!!! Script execution timeout is exceeded");
	if(!abInterrupted){
		JS_ReportWarning(cx, "Script is executing more then %d ms", aiTimeout);
	}else{
		JS_ReportError(cx, "!!! Script execution timeout (%d ms) is exceeded. Execution is terminated.", aiTimeout);
	}
	if(abInterrupted){
		mlString wsCallStack = GetJSCallStack();
		mlTrace(cx, "[RMML] Script is hanging up in (%S)\n", wsCallStack.c_str());
	}
}

}

/// returns used heap size in bytes or negative if heap is corrupted.
long HeapUsed()
{
	_HEAPINFO info = { 0, 0, 0 };
	long used = 0;
	int rc;

	intptr_t hCrtHeap = _get_heap_handle();

	while ((rc=_heapwalk(&info)) == _HEAPOK)
	{
		if (info._useflag == _USEDENTRY)
			used += info._size;
	}
	if (rc != _HEAPEND && rc != _HEAPEMPTY)
		used = (used?-used:-1);

	return used;
}

// “¤ «Ёвм Ё§ Classes ббл«Єг ­  гЄ § ­­л© Є« бб
jsval mlSceneManager::FreeClass(const mlString wsClassName){
	jsval vc = JSVAL_NULL;
	// в Є Є Є ®бв овбп Є ЄЁҐ-в® ббл«ЄЁ, в® "а §ЎЁа Ґ¬" ўбо Є®¬Ї®§ЁжЁо ­  ®в¤Ґ«м­лҐ н«Ґ¬Ґ­вл
	wr_JS_GetUCProperty(cx, mjsoClasses, wsClassName.c_str(), -1, &vc);
	mlRMMLElement* pClass = JSVAL_TO_MLEL(cx, vc);
	if(pClass == NULL)
		return JSVAL_FALSE;
	std::vector<mlRMMLElement*> vElems;
	mlRMMLIterator iter(cx, pClass);
	mlRMMLElement* pElem = NULL;
	while((pElem = iter.GetNext()) != NULL){
		if(pElem->IsJSO())
			vElems.push_back(pElem);
	}
	v_elems::const_iterator viBegin = vElems.begin();
	v_elems::const_iterator viEnd = vElems.end();
	v_elems::const_iterator vi = viBegin;
	for(; vi != viEnd; vi++){
		mlRMMLElement* pMLEl = *vi;
		jsval vNull = JSVAL_NULL;
		JS_SetProperty(cx, pMLEl->mjso, "parent", &vNull);
	}
	//   Є®а­Ґў®© ®ЎкҐЄв Є« бб  ®бв ўЁ¬, в Є Є Є ­  ­ҐЈ® Ґбвм ббл«ЄЁ ў mlRMMLScript- е (mpClass)
	jsval rval = JSVAL_TRUE;
	// JS_DeleteUCProperty2(cx, mjsoClasses, wsClassName.c_str(), wsClassName.size(), &rval);
	return rval;
}

// Установить в глобальный объект Event текстурные координаты из mBUM
void mlSceneManager::SetTextureXYToEventGO(mlRMMLElement* apMLEl){
	// Устанавливаем текстурные координаты в Event если таковые есть
	// если под мышкой именно этот объект
	if(mjsoEvent != NULL && mBUM.mp3DObject != NULL){
		if(apMLEl == NULL || mBUM.mp3DObject == apMLEl){
			jsval v;
			mlPoint pntXY = mBUM.mLocalXY;
			v = INT_TO_JSVAL(pntXY.x);
			JS_SetProperty(cx, mjsoEvent, ML_EVENT_TEXTURE_X_PROP_NAME, &v);
			v = INT_TO_JSVAL(pntXY.y);
			JS_SetProperty(cx, mjsoEvent, ML_EVENT_TEXTURE_Y_PROP_NAME, &v);
		}
	}
}

void mlSceneManager::SetVersionInfo(wchar_t* version){
	mlSynchData oData;
	oData << version;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onVersionReceived, (unsigned char*)oData.data(), oData.size());
};

	
void mlSceneManager::OnVersionReceivedInternal(mlSynchData& aData){
	wchar_t* aVersionInfo;
	aData >> aVersionInfo;
	mlString sResult(L"[");
	sResult = sResult + aVersionInfo + L"]";

	jsval vJSONProps = NULL;
	JSBool bR = wr_JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), sResult.c_str(), sResult.size(), "result", 1, &vJSONProps);	
	if (JSVAL_IS_REAL_OBJECT(vJSONProps))
	{	
		JSObject *obj=JSVAL_TO_OBJECT(vJSONProps);
		JS_GetElement(cx,obj,0,&vJSONProps);
		if (JSVAL_IS_REAL_OBJECT(vJSONProps))
		{
			JS_SetProperty(cx, GetEventGO(), "versionInfo", &vJSONProps);
		}
	}

	NotifyPlayerListeners(EVID_onVersionReceived);
	JS_DeleteProperty(cx, GetEventGO(), "versionInfo");
}
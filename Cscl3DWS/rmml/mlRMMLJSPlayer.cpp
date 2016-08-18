#include "mlRMML.h"
#include "config/oms_config.h"
#include "mlRMMLJSPlayer.h"
#include "config/prolog.h"
#include "config/prolog.h"
#include "../rm/rm.h"
#include "../rm/rmExt.h"
#include <Windows.h>
#include <ctime>
#include "../../Launcher/StartUpdater.h"
#include "../../Launcher/launchererrors.h"
#include "../Cscl3DWS.h"
#include "atlconv.h"
#include "../oms/oms_context.h"

namespace rmml {

/**
 * Реализация глобального JS-объекта "Player"
 */

JSBool JSPlayerGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool JSPlayerSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	
JSClass JSRMMLPlayerClass = {
	"_Player", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, 
	JSPlayerGetProperty, JSPlayerSetProperty,
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL,
	NULL, NULL, NULL
};

#define SM_MODE_MASK 0x40

JSPropertySpec _JSPlayerPropertySpec[] = {
	{ "MODE_INIT", smmInitilization|SM_MODE_MASK, JSPROP_ENUMERATE | JSPROP_READONLY, 0, 0},
	{ "MODE_LOADING", smmLoading|SM_MODE_MASK, JSPROP_ENUMERATE | JSPROP_READONLY, 0, 0},
	{ "MODE_NORMAL", smmNormal|SM_MODE_MASK, JSPROP_ENUMERATE | JSPROP_READONLY, 0, 0},
	{ "MODE_PLAY", smmPlay|SM_MODE_MASK, JSPROP_ENUMERATE | JSPROP_READONLY, 0, 0},
	{ "MODE_PAUSE",	smmPause|SM_MODE_MASK, JSPROP_ENUMERATE | JSPROP_READONLY, 0, 0},
	JSPROP_SC(WARNING_OPTIMUS_INTEL_USED, ENGINE_WARNING_OPTIMUS_INTEL_USED)
	JSPROP_SC(WARNING_BETA_DRIVER, ENGINE_WARNING_BETA_DRIVER)
	JSPROP_SC(WARNING_OLD_DRIVER, ENGINE_WARNING_OLD_DRIVER)
	JSPROP_SC(WARNING_VENDOR_UNSUPPORTED, ENGINE_WARNING_VENDOR_UNSUPPORTED)
	JSPROP_SC(WARNING_DRIVER_INVALID, ENGINE_WARNING_DRIVER_INVALID)
	JSPROP_SC(WARNING_DRIVERS_NOT_INSTALLED, ENGINE_WARNING_DRIVERS_NOT_INSTALLED)
	JSPROP_SC(WARNING_RAM_NOT_ENOUGH, ENGINE_WARNING_RAM_NOT_ENOUGH)
	JSPROP_SC(WARNING_VIDEOCARD_TOO_WEAK, ENGINE_WARNING_VIDEOCARD_TOO_WEAK)
	JSPROP_SC(WARNING_CPU_FREQ_NOT_ENOUGH, ENGINE_WARNING_CPU_FREQ_NOT_ENOUGH)
	JSPROP_SC(WARNING_OS_OUTDATED, ENGINE_WARNING_OS_OUTDATED)
	JSPROP_SC(WARNING_VIDEOMEM_NOT_ENOUGH, ENGINE_WARNING_VIDEOMEM_NOT_ENOUGH)
	JSPROP_SC(WARNING_NEED_TO_ENLARGE_VIDEOMEM, ENGINE_WARNING_NEED_TO_ENLARGE_VIDEOMEM)
	JSPROP_RW(mode)
	JSPROP_RO(module)
	JSPROP_RW(soundMode)
	JSPROP_RW(soundVolume)
	JSPROP_RO(stencilBuffer)
	JSPROP_RO(bigCursor)
	JSPROP_RO(version)
	JSPROP_RO(language)
	JSPROP_RO(debugVersion)
	JSPROP_RO(arguments)	// массив аргументов, с которыми был запущен плеер (из командной строки)
	JSPROP_RO(logRestoreProgress)
	JSPROP_RO(reality)
	JSPROP_RW(preloadingMode) // (bool) включение режима загрузки, при котором не требуется быстрого отклика системы
	JSPROP_RW(loadingMode) // (bool) режим загрузочной картинки или нет	
	JSPROP_RO(testMode) // (int) тестовый режим
	JSPROP_RO(testResDir) // (string) директория тестовых файлов
//	JSPROP_RO(globals)
	{ 0, 0, 0, 0, 0 }
};

JSBool JSPlayerSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){
	if(JSVAL_IS_INT(id)){
		mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
		int iID=JSVAL_TO_INT(id);
		switch(iID){
		case JSPROP_mode:
			pSM->SetMode((mlEModes)JSVAL_TO_INT(*vp));
			break;
		case JSPROP_soundMode:{
			sm::smISoundManager* pSndM=pSM->GetContext()->mpSndM;
			if(pSndM!=NULL){
				uint16 uiMode=1;
				if(JS_ValueToUint16(cx,*vp,&uiMode)){
					pSndM->SetMode(uiMode);
				}
			}
			}break;
		case JSPROP_soundVolume:{
			sm::smISoundManager* pSndM=pSM->GetContext()->mpSndM;
			if(pSndM!=NULL){
				jsdouble jsdVol=1.0;
				if(JS_ValueToNumber(cx,*vp,&jsdVol)){
					if(jsdVol < 0.0) jsdVol=0.0;
					if(jsdVol > 1.0) jsdVol=1.0;
					pSndM->SetVolume(jsdVol);
				}
			}
			}break;
		case JSPROP_preloadingMode:{
			ML_JSVAL_TO_BOOL(pSM->mbPreloadingMode,*vp);
			}break;

		case JSPROP_loadingMode:{
            ML_JSVAL_TO_BOOL(pSM->mbLoadingMode,*vp);
			pSM->GetContext()->mpRM->LoadingModeChanged(pSM->mbLoadingMode);
			}break;
		case JSPROP_testMode:{
			pSM->SetTestMode( (mlETestModes)JSVAL_TO_INT(*vp));
			}break;
		// ??
		}
		// уведомляем слушателей
		char chEvID=iID;
		pSM->NotifyPlayerListeners(chEvID);
	}
	return JS_TRUE;
}

JSBool JSPlayerGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){
	if(JSVAL_IS_INT(id)){
		mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
		int iID=JSVAL_TO_INT(id);
		if(iID & SM_MODE_MASK){
			*vp=INT_TO_JSVAL(iID & ~SM_MODE_MASK);
			return JS_TRUE;
		}
		switch(iID){
		case JSPROP_mode:
			*vp=INT_TO_JSVAL(pSM->GetMode());
			break;
		case JSPROP_module:
			break;
		case JSPROP_soundMode:{
			sm::smISoundManager* pSndM=pSM->GetContext()->mpSndM;
			if(pSndM!=NULL){
				*vp=INT_TO_JSVAL(pSndM->GetMode());
			}
			}break;
		case JSPROP_soundVolume:{
			sm::smISoundManager* pSndM=pSM->GetContext()->mpSndM;
			if(pSndM!=NULL){
				double dVol=pSndM->GetVolume();
				mlJS_NewDoubleValue(cx,dVol,vp);
			}
			}break;
		case JSPROP_stencilBuffer:{
			*vp=JSVAL_FALSE;
			rm::rmIRenderManager* pRM=pSM->GetContext()->mpRM;
			if(pRM!=NULL){
				if(pRM->StencilBufferExists())
					*vp=JSVAL_TRUE;
			}
			}break;
		case JSPROP_version:{
			*vp = JS_GetEmptyStringValue(cx);
			oms::omsIApplication* pApp = pSM->GetContext()->mpApp;
			wchar_t wcsVersion[1000];
			if(pApp->GetVersion((wchar_t*)wcsVersion, 990)){
				JSString* jssVersion = wr_JS_NewUCStringCopyZ(cx, wcsVersion);
				*vp = STRING_TO_JSVAL(jssVersion);
			}
			}break;
		case JSPROP_language:{
			*vp = JS_GetEmptyStringValue(cx);
			oms::omsIApplication* pApp = pSM->GetContext()->mpApp;
			wchar_t wcsLanguage[100] = L"";
			if(pApp->GetLanguage((wchar_t*)wcsLanguage, 99)){
				JSString* jssLanguage = wr_JS_NewUCStringCopyZ(cx, wcsLanguage);
				*vp = STRING_TO_JSVAL(jssLanguage);
			}
			}break;
		case JSPROP_debugVersion:{
			oms::omsIApplication* pApp = pSM->GetContext()->mpApp;
			bool debugVersion = false;
			if( pApp)	debugVersion = pApp->GetDebugVersion();
			*vp = BOOLEAN_TO_JSVAL(debugVersion);
			}break;
		case JSPROP_logRestoreProgress:{
			double logRestoreProgress = 0.0;
			mlJS_NewDoubleValue(cx, logRestoreProgress, vp);
			}break;
		case JSPROP_arguments:{
			oms::omsIApplication* pApp = pSM->GetContext()->mpApp;
			wchar_t** ppArgs = pApp->GetArguments();
			JSObject* jsoArgs = NULL;
			if(ppArgs == NULL){
				jsval v = JSVAL_NULL;
				jsoArgs = JS_NewArrayObject(cx, 0, &v);
			}else{
				int iArgCount = 0;
				while(ppArgs[iArgCount] != NULL)
					iArgCount++;
				jsval* paArgs = MP_NEW_ARR( jsval, iArgCount + 1);
				for(int ii = 0; ii < iArgCount; ii++)
					paArgs[ii] = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, ppArgs[ii]));
				jsoArgs = JS_NewArrayObject(cx, iArgCount, paArgs);
				MP_DELETE_ARR( paArgs);
			}
			*vp = OBJECT_TO_JSVAL(jsoArgs);
			}break;
		case JSPROP_reality:{
			*vp = JSVAL_NULL;
			oms::omsContext* pContext = GPSM(cx)->GetContext();
			if(pContext->mpMapMan == NULL)
				return JS_TRUE;
			JSObject* pRealityObject = GetRealityObject( cx, pContext->mpMapMan, pSM->uCurrentRealityID);
			*vp = OBJECT_TO_JSVAL( pRealityObject);
			}break;
		case JSPROP_preloadingMode:{
			*vp = BOOLEAN_TO_JSVAL(pSM->mbPreloadingMode);
			}break;
		case JSPROP_loadingMode:{
			*vp = BOOLEAN_TO_JSVAL(pSM->mbLoadingMode);
			}break;
		case JSPROP_testMode:
			*vp=INT_TO_JSVAL(pSM->GetTestMode());	
			break;
		case JSPROP_testResDir:
			{
				*vp = JS_GetEmptyStringValue(cx);
				wchar_t wcsTestResDir[3124] = L"";
				pSM->GetTestResDir((wchar_t*)wcsTestResDir, 3124);
				JSString* jssTestResDir = wr_JS_NewUCStringCopyZ(cx, wcsTestResDir);
				*vp = STRING_TO_JSVAL(jssTestResDir);
			}
			break;
		}
	}
	return JS_TRUE;
}

void CorrectRealitiesParents(JSContext *cx, cm::cmIMapManager* aMapMan, JSObject* pRealityObject)
{
	jsval v = JSVAL_NULL;
	JS_GetProperty(cx, pRealityObject, "parent", &v);
	JSObject* jsoParent = NULL;
	if (v != JSVAL_NULL)
	{	jsoParent = JSVAL_TO_OBJECT(v);
		CorrectRealitiesParents(cx, aMapMan, jsoParent);
	}
	else
	{
		cm::cmIRealityInfo* pRealityInfo = NULL;
		jsval v_parentId = JSVAL_NULL;
		JS_GetProperty(cx, pRealityObject, "id", &v_parentId);
		if (JSVAL_IS_INT(v_parentId))
		{
			int iParentRealityID=JSVAL_TO_INT(v_parentId);
			if (iParentRealityID==0)
				return;
			jsval val;
			JSObject* pParentObject = GetRealityObject( cx, aMapMan, iParentRealityID);
			if (pParentObject != NULL)
				val = OBJECT_TO_JSVAL( pParentObject);
			else
			{
				val = JSVAL_NULL;
			}
			JS_DefineProperty( cx, pRealityObject, "parent", val,
				NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

			pRealityInfo->SetData( pRealityObject);
		}
	}


}


JSObject* GetRealityObject( JSContext *cx, cm::cmIMapManager* aMapMan, unsigned int aRealityID)
{
	cm::cmIRealityInfo* pRealityInfo = NULL;
	if (!aMapMan->GetRealityInfo( aRealityID, pRealityInfo))
	{
		mlTrace(cx, "!aMapMan->GetRealityInfo\n");
		return NULL;
	}

	wchar_t sID[20];
	swprintf_s(sID, 20, L"%u", pRealityInfo->GetRealityID());
	//mlTrace(cx, " GetRealityObject.SetData realityID= (%d)\n",(int)pRealityInfo->GetRealityID());
	JSObject* pRealityObject = (JSObject*) pRealityInfo->GetData();
	if (pRealityObject == NULL)
	{
		pRealityObject = JS_NewObject( cx, NULL, NULL, NULL);
		JSObject* jsoMap = GPSM(cx)->GetPlMapGO();
		SAVE_FROM_GC( cx, jsoMap, pRealityObject);

		jsval val;

		val = INT_TO_JSVAL(  pRealityInfo->GetRealityID()); 
		JS_DefineProperty( cx, pRealityObject, "id", val,
			NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

		val = INT_TO_JSVAL(  pRealityInfo->GetRealityDepth()); 
		JS_DefineProperty( cx, pRealityObject, "depth", val,
			NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

		val = STRING_TO_JSVAL( wr_JS_NewUCStringCopyZ( cx, pRealityInfo->GetName()));
		JS_DefineProperty( cx, pRealityObject, "name", val,
			NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

		val = INT_TO_JSVAL( pRealityInfo->GetType());
		JS_DefineProperty( cx, pRealityObject, "type", val,
			NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

		val = INT_TO_JSVAL( 0); 
		JS_DefineProperty( cx, pRealityObject, "REALITY_ZERO", val,
			NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

		val = INT_TO_JSVAL( 1);
		JS_DefineProperty( cx, pRealityObject, "REALITY_CUSTOM", val,
			NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

		val = INT_TO_JSVAL( 2);
		JS_DefineProperty( cx, pRealityObject, "REALITY_RECORDING", val,
			NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

		val = INT_TO_JSVAL( 3);
		JS_DefineProperty( cx, pRealityObject, "REALITY_PLAYING", val,
			NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

		val = INT_TO_JSVAL( 4);
		JS_DefineProperty( cx, pRealityObject, "REALITY_EDITING", val,
			NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

		val = INT_TO_JSVAL( 5);
		JS_DefineProperty( cx, pRealityObject, "REALITY_USER", val,
			NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

		JSObject* pParentObject = GetRealityObject( cx, aMapMan, pRealityInfo->GetParentRealityID());
		if (pParentObject != NULL)
			val = OBJECT_TO_JSVAL( pParentObject);
		else
		{

			mlTrace(cx, "NEXT PARENT NULL\n");
			val = JSVAL_NULL;
		}
		JS_DefineProperty( cx, pRealityObject, "parent", val,
			NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);

		pRealityInfo->SetData( pRealityObject);
	}

	return pRealityObject;
}

JSFUNC_DECL(openURL)
JSFUNC_DECL(startUpdater)
JSFUNC_DECL(startProxySettings)
JSFUNC_DECL(startTracer)
JSFUNC_DECL(restart)
JSFUNC_DECL(minimize)
JSFUNC_DECL(exit)
JSFUNC_DECL(setGlobalDataSpace)
//JSFUNC_DECL(saveGlobalData) // сохранить Player.globals для заданного пространства имен
//JSFUNC_DECL(clearGlobalData) // очистить Player.globals для заданного пространства имен
JSFUNC_DECL(resize)
JSFUNC_DECL(setSetting)
JSFUNC_DECL(getSetting)
JSFUNC_DECL(getLocalFilePath)
JSFUNC_DECL(getUserSetting)
JSFUNC_DECL(setUserSetting)
JSFUNC_DECL(getScreenSize)
JSFUNC_DECL(setAudioMode)
JSFUNC_DECL(setAudioVolume)
JSFUNC_DECL(enableLastScreen)
JSFUNC_DECL(assert)
JSFUNC_DECL(getTimeTicks)	// получить некое время в ms 
JSFUNC_DECL(getClipboardContentType) // получить тип содержимого буфера
JSFUNC_DECL(getMaxAntialiasingPower)
JSFUNC_DECL(startTrackVisibleScreensInCurrentLocation) // начать отслеживать видимость экранов в текущей локации
JSFUNC_DECL(endTrackVisibleScreensInCurrentLocation) // закончить отслеживать видимость экранов в текущей локации
JSFUNC_DECL(getVisibleScreensInCurrentLocation) // вернуть текущие видимые экраны и кол-во видимых пикселей
JSFUNC_DECL(getEngineWarningID) // вернуть ID предупреждения от графического движка
JSFUNC_DECL(getToolbarParams)
JSFUNC_DECL(getResourceConsumingApplications)
JSFUNC_DECL(getSessionID)
JSFUNC_DECL(sendConnectionRestoredStatus)
JSFUNC_DECL(getSessionKey)
JSFUNC_DECL(getProxySettings)
JSFUNC_DECL(getDriverLink)
JSFUNC_DECL(isAnsiFilePath)
JSFUNC_DECL(isComputerSlow)
JSFUNC_DECL(startKinect)
JSFUNC_DECL(stopKinect)
JSFUNC_DECL(setKinectMode)

// ---
JSFUNC_DECL(setMarkerType)
JSFUNC_DECL(enableFaceTracking)
JSFUNC_DECL(disableFaceTracking)
// ---
JSFUNC_DECL(getKinectPoints)
JSFUNC_DECL(getKinectState)
JSFUNC_DECL(getKeysShortcutsEnabled)
JSFUNC_DECL(getRealityGuardName)

JSFUNC_DECL(initOculusRift)
JSFUNC_DECL(initKinect)
JSFUNC_DECL(initGamepad)

///// JavaScript Function Table
JSFunctionSpec _JSPlayerFunctionSpec[] = {
	JSFUNC(openURL,1) // открыть URL
	JSFUNC(startUpdater,1) // запустить обновление
	JSFUNC(startProxySettings,1) // запустить настройки прокси
	JSFUNC(startTracer,0) // запустить настройки прокси
	JSFUNC(restart,1) // перезапуск вакадемии
	JSFUNC(minimize,1) // минимизация главного окна 
	JSFUNC(exit,1) // закрытие главного окна (завершение работы программы) 
	JSFUNC(setGlobalDataSpace,1) // установить пространство имен для Player.globals
//	JSFUNC(saveGlobalData,1) // сохранить Player.globals для заданного пространства имен
//	JSFUNC(clearGlobalData,1) // очистить Player.globals для заданного пространства имен
	JSFUNC(getUserSetting, 1) // получить определенное значение пользовательских настроек
	JSFUNC(setUserSetting, 2) // сохранить определенное значение пользовательских настроек
	JSFUNC(getSetting,1) // получить значение из реестра по имени
	JSFUNC(setSetting,2) // сохранить значение из реестра по имени
	JSFUNC(getLocalFilePath,1) // через стандартное диалоговое окно получить путь к файлу на локальном компьютере 
	JSFUNC(getScreenSize,0)	// возвращает объект с шириной и высотой экрана
	JSFUNC(setAudioMode,2)	// устанавливает режим воспроизведения звуков у 3d-объектов
	JSFUNC(setAudioVolume,2)	// устанавливает уровень громкости воспроизведения звуков у аудио-объектов	
	JSFUNC(enableLastScreen,1)	// устанавливает режим "картинки с прошлого захода"
	JSFUNC(assert,1)			// проверяет данное выражение на true. Если оно false, то выдает callstack и окно accert.
	JSFUNC(getTimeTicks,1)	// получить некое время в ms 
	JSFUNC(getClipboardContentType,0) // получить тип содержимого буфера
	JSFUNC(getMaxAntialiasingPower,0) // получить максимальное число семплов для антиалиасинга
	JSFUNC(startTrackVisibleScreensInCurrentLocation,0)
	JSFUNC(endTrackVisibleScreensInCurrentLocation,0)
	JSFUNC(getVisibleScreensInCurrentLocation,0)
	JSFUNC(getEngineWarningID,0)
	JSFUNC(getToolbarParams,0)
	JSFUNC(getResourceConsumingApplications,0)
	JSFUNC(getSessionID,0)
	JSFUNC(sendConnectionRestoredStatus,0)
	JSFUNC(getSessionKey,0)	
	JSFUNC(getProxySettings,0)	
	JSFUNC(getDriverLink,0)	
	JSFUNC(isAnsiFilePath,0)
	JSFUNC(isComputerSlow,0)	
	JSFUNC(startKinect,0)	
	JSFUNC(stopKinect,0)
	JSFUNC(setKinectMode,1)	
	// ---
	JSFUNC(setMarkerType,1)
	JSFUNC(enableFaceTracking,0)
	JSFUNC(disableFaceTracking,0)
	// ---
	JSFUNC(getKinectPoints,0)
	JSFUNC(getKinectState,0)
	JSFUNC(getKeysShortcutsEnabled,0)
	JSFUNC(getRealityGuardName,0)
	JSFUNC(initOculusRift,1)
	JSFUNC(initKinect,1)
	JSFUNC(initGamepad,1)
	{ 0, 0, 0, 0, 0 }
};

JSFunctionSpec _JSPlayerWindowFunctionSpec[] = {
	JSFUNC(resize,3) // изменить размер окна
	{ 0, 0, 0, 0, 0 }
};

void SetWindowSubObject(JSContext *cx, JSObject *ajsoPlayer , unsigned int auWidth = 0 , unsigned int auHeight = 0){
	JSObject* jsoWindow = JS_DefineObject(cx, ajsoPlayer, "window", NULL, NULL, 0);
	JS_DefineFunctions(cx, jsoWindow, _JSPlayerWindowFunctionSpec);
	int iWidth=0;
	int iHeight=0;
	if (auWidth!=0 && auHeight!=0)
	{
		iWidth = auWidth;
		iHeight = auHeight;
	}
	else
	{
#ifdef CSCL
	oms::mwIWindow* pWindow = GPSM(cx)->GetContext()->mpWindow;
	if(pWindow!=NULL){
		iWidth=pWindow->GetClientWidth();
		iHeight=pWindow->GetClientHeight();
	}
#else
	oms::omsIApplication* pApp = GPSM(cx)->GetContext()->mpApp;
	std::pair<long, long> client_size(0,0);
	if (pApp)
		client_size = pApp->client_size();
	iWidth  = client_size.first;
	iHeight = client_size.second;
#endif
	}
	JS_DefineProperty(cx,jsoWindow,"clientWidth",INT_TO_JSVAL(iWidth),NULL,NULL,0);
	JS_DefineProperty(cx,jsoWindow,"clientHeight",INT_TO_JSVAL(iHeight),NULL,NULL,0);
}

void InitPlayerJSObject(JSContext *cx, JSObject *ajsoPlayer){
	JSBool bR;
	bR=JS_DefineProperties(cx, ajsoPlayer, _JSPlayerPropertySpec);
	bR=JS_DefineFunctions(cx, ajsoPlayer, _JSPlayerFunctionSpec);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, ajsoPlayer);
	// инициализация "window"
	SetWindowSubObject(cx, ajsoPlayer);
	// инициализация "module"
	CreateModuleJSObject(cx,ajsoPlayer);
#ifdef CSCL
//	if(pSM->GetContext()->mpComMan != NULL)
	CreateServerJSObject(cx,ajsoPlayer);
	CreateRecordEditorJSObject(cx,ajsoPlayer);
	CreateMapJSObject(cx,ajsoPlayer);
	CreateUrlJSObject(cx,ajsoPlayer);
	CreateVoiceJSObject(cx,ajsoPlayer);
	CreateVideoCaptureJSObject(cx,ajsoPlayer);
	CreateCodeEditorJSObject(cx,ajsoPlayer);
	//CreateRealityJSObject(cx,ajsoPlayer);
#endif
	// инициализация "globals"
//	JSObject* jsoGlobals = JS_DefineObject(cx, ajsoPlayer, "globals", NULL, NULL, 0);
}

char GetPlayerEventID(const wchar_t* apwcEventName){
	if(isEqual(apwcEventName,L"mode"))
		return (char)JSPROP_mode;
	if(isEqual(apwcEventName,L"module"))
		return (char)JSPROP_module;
	if(isEqual(apwcEventName,L"soundMode"))
		return (char)JSPROP_soundMode;
	if(isEqual(apwcEventName,L"soundVolume"))
		return (char)JSPROP_soundVolume;
	if(isEqual(apwcEventName,L"stencilBuffer"))
		return (char)JSPROP_stencilBuffer;
	if(isEqual(apwcEventName,L"tesMode"))
		return (char)JSPROP_testMode;
	if(isEqual(apwcEventName,L"testResDir"))
		return (char)JSPROP_testResDir;
	if(isEqual(apwcEventName,L"onRestoreUnsynched"))
		return (char)EVID_onRestoreUnsynched;
	if(isEqual(apwcEventName,L"onRestoreStateUnsynched"))
		return (char)EVID_onRestoreStateUnsynched;
	if(isEqual(apwcEventName,L"onDisconnect"))
		return (char)EVID_onDisconnect;
	if(isEqual(apwcEventName,L"onErrorReport"))
		return (char)EVID_onErrorReport;
	if(isEqual(apwcEventName,L"onActivated"))
		return (char)EVID_onActivated;
	if(isEqual(apwcEventName,L"onDeactivated"))
		return (char)EVID_onDeactivated;
	if(isEqual(apwcEventName,L"onShowInfoState"))
		return (char)EVID_onShowInfoState;
	if(isEqual(apwcEventName,L"onWindowSizeChanged"))
		return (char)EVID_onWindowSizeChanged;
	if(isEqual(apwcEventName,L"onWindowMoved"))
		return (char)EVID_onWindowMoved;
	if(isEqual(apwcEventName,L"onKinectFound"))
		return (char)EVID_onKinectFound;
	if(isEqual(apwcEventName,L"onOculusRiftFound"))
		return (char)EVID_onOculusRiftFound;
	if(isEqual(apwcEventName,L"onVisibilityDistanceKoefChanged"))
		return (char)EVID_onVisibilityDistanceKoefChanged;
	if(isEqual(apwcEventName,L"onVersionReceived"))
		return (char)EVID_onVersionReceived;
	if(isEqual(apwcEventName,L"onControllerFound"))
		return (char)EVID_onControllerFound;
	return '\0';
}

wchar_t* GetPlayerEventName(char aidEvent){
	switch(aidEvent){
	case (char)JSPROP_mode:
		return L"mode";
	case (char)JSPROP_module:
		return L"module";
	case (char)JSPROP_soundMode:
		return L"soundMode";
	case (char)JSPROP_soundVolume:
		return L"soundVolume";
	case (char)JSPROP_stencilBuffer:
		return L"stencilBuffer";
	case (char)JSPROP_testMode:
		return L"testMode";
	case (char)JSPROP_testResDir:
		return L"testResDir";
	case (char)EVID_onRestoreUnsynched:
		return L"onRestoreUnsynched";
	case (char)EVID_onRestoreStateUnsynched:
		return L"onRestoreStateUnsynched";
	case (char)EVID_onDisconnect:
		return L"onDisconnect";
	case (char)EVID_onErrorReport:
		return L"onErrorReport";
	case (char)EVID_onActivated:
		return L"onActivated";
	case (char)EVID_onDeactivated:
		return L"onDeactivated";
	case (char)EVID_onShowInfoState:
		return L"onShowInfoState";
	case (char)EVID_onWindowSizeChanged:
		return L"onWindowSizeChanged";
	case (char)EVID_onWindowMoved:
		return L"onWindowMoved";
	case (char)EVID_onKinectFound:
		return L"onKinectFound";
	case (char)EVID_onOculusRiftFound:
		return L"onOculusRiftFound";
	case (char)EVID_onVisibilityDistanceKoefChanged:
		return L"onVisibilityDistanceKoefChanged";
	case (char)EVID_onVersionReceived:
		return L"onVersionReceived";
	case (char)EVID_onControllerFound:
		return L"onControllerFound";
	}
	
	return L"???";
}

bool mlSceneManager::addPlayerListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	//char chEvID=aidEvent-1;
	char chEvID=aidEvent-1;
	if(chEvID >= ML_PLAYER_EVENTS_COUNT) return false;
	v_elems* pv=&(maPlayerListeners[chEvID]);
	v_elems::const_iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl) return true;
	}
	pv->push_back(apMLEl);
	return true;
}

void mlSceneManager::NotifyPlayerListeners(char chEvID){
	if(chEvID <= ML_PLAYER_EVENTS_COUNT){
		v_elems* pv=&(maPlayerListeners[chEvID-1]);
		v_elems::const_iterator vi;
		for(vi=pv->begin(); vi != pv->end(); vi++ ){
			mlRMMLElement* pMLEl=*vi;
			pMLEl->EventNotify(chEvID,(mlRMMLElement*)GOID_Player);
		}
	}
}

void mlSceneManager::NotifyPlayerOnRestoreUnsynchedListeners(){
	NotifyPlayerListeners(EVID_onRestoreUnsynched);
}

void mlSceneManager::NotifyPlayerOnRestoreStateUnsynchedListeners(){
	NotifyPlayerListeners(EVID_onRestoreStateUnsynched);
}

void mlSceneManager::NotifyPlayerOnDisconnectListeners(){
	NotifyPlayerListeners(EVID_onDisconnect);
}

void mlSceneManager::NotifyPlayerOnWindowSizeChanged( const int& auClientWidth, const int& auClientHeight)
{
	jsval vParam = JSVAL_NULL;
	vParam = INT_TO_JSVAL(auClientWidth);
	JS_SetProperty(cx, GetEventGO(), "clientWidth", &vParam );
	vParam = INT_TO_JSVAL(auClientHeight);
	JS_SetProperty(cx, GetEventGO(), "clientHeight", &vParam );
	NotifyPlayerListeners(EVID_onWindowSizeChanged);
	JS_DeleteProperty(cx, GetEventGO(), "clientHeight");
	JS_DeleteProperty(cx, GetEventGO(), "clientWidth");
}

void mlSceneManager::SavePlayerInternalPath(mlSynchData &Data){
	Data << (unsigned short)1;
	Data << (mlRMMLElement*)GOID_Player;
}

bool mlSceneManager::SetPlayerEventData(char aidEvent, mlSynchData &Data){
	switch(aidEvent){
		case EVID_onErrorReport:{
			ResetEvent();
			wchar_t* pwc;
			Data >> pwc;
			jsval v = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwc));
			JS_SetProperty(cx, GetEventGO(), "type", &v);
			Data >> pwc;
			v = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwc));
			JS_SetProperty(cx, GetEventGO(), "code", &v);
			unsigned short iParamCnt = 0;
			Data >> iParamCnt;
			for(int ii = 0; ii < iParamCnt; ii++){
				Data >> pwc;
				char buf[64];
				//sprintf(buf, "param%d", ii);
				rmmlsafe_sprintf(buf, sizeof(buf), 0, "param%d", ii);
				jsval v = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwc));
				JS_SetProperty(cx, GetEventGO(), buf, &v);
			}
			}break;
	}
	return true;
}

void mlSceneManager::NotifyPlayerOnErrorReportListeners(){
	if(Updating()){ // в другом потоке (не очень корректное определение того, что в другом потоке. Пока не придумал как это сделать правильно)
		mlSynchData Data;
		GPSM(cx)->SavePlayerInternalPath(Data);
		// сохраняем параметры
		jsval v = JSVAL_NULL;
		JS_GetProperty(cx, GetEventGO(), "type", &v);
		if(JSVAL_IS_STRING(v)){
			Data << JS_GetStringChars(JSVAL_TO_STRING(v));
		}else
			Data << L"";
		v = JSVAL_NULL;
		JS_GetProperty(cx, GetEventGO(), "code", &v);
		if(JSVAL_IS_STRING(v)){
			Data << JS_GetStringChars(JSVAL_TO_STRING(v));
		}else
			Data << L"";
		int iParamCntPos = Data.getPos();
		unsigned short iParamCnt = 0;
		Data << iParamCnt;
		for(int i = 0; ; ++i, ++iParamCnt){
			char buf[64];
			//sprintf(buf, "param%d", i);
			rmmlsafe_sprintf(buf, sizeof(buf), 0, "param%d", i);
			v = JSVAL_VOID;
			if(!JS_GetProperty(cx, GetEventGO(), buf, &v))
				break;
			if(v == JSVAL_VOID)
				break;
			//			Data << v;
			Data << JS_GetStringChars(JSVAL_TO_STRING(v));
		}
		Data.setPos(iParamCntPos);
		Data << iParamCnt;
		Data.setPos(); 
		// ставим в очередь событий
		GPSM(cx)->GetContext()->mpInput->AddInternalEvent(EVID_onErrorReport, false, 
			(unsigned char*)Data.data(), Data.size(), GPSM(cx)->GetMode() == smmInitilization);
		return;
	}
	NotifyPlayerListeners(EVID_onErrorReport);
}

bool mlSceneManager::removePlayerListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	if(aidEvent==-1){
		// и удалим его из списка слушателей всех событий
		for(int ii=0; ii<ML_PLAYER_EVENTS_COUNT; ii++){
			v_elems* pv=&maPlayerListeners[ii];
			v_elems::iterator vi;
			for(vi=pv->begin(); vi != pv->end(); vi++ ){
				if(*vi==apMLEl){
					pv->erase(vi);
					break;
				}
			}
		}
		return true;
	}
	v_elems* pv=&(maPlayerListeners[aidEvent-1]);
	v_elems::iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl){
			pv->erase(vi);
			return true;
		}
	}
	return true;
}

void mlSceneManager::SetUIScene(mlRMMLComposition* apNewUIScene){
//SetScene(apNewUIScene);
//return;
	mpUI=apNewUIScene;
	if(apNewUIScene==NULL){
		JS_DeleteProperty(cx,mjsoPlayer,"UI");
	}else{
		UpdateNotInScenesElems(apNewUIScene, true);
		jsval v=OBJECT_TO_JSVAL(apNewUIScene->mjso);
		JS_SetProperty(cx,mjsoPlayer,"UI",&v);
	}
//	SetScene(NULL);
}

void mlSceneManager::ResetPlayer(){
	// tandy: нельзя раскомметаривать, потому что в UI тоже могут быть обработчики событий
	//for(int ii=0; ii<ML_PLAYER_EVENTS_COUNT; ii++){
	//	maPlayerListeners[ii].clear();
	//}
	ResetPlayerServer();
}

void mlSceneManager::GetPlayerState(mlSynchData &aData){
	// ??
}

void mlSceneManager::SetPlayerState(mlSynchData &aData){
	// ??
}

void mlSceneManager::OnShowInfoStateInternal(mlSynchData& aData){
	int iState = 0;
	aData >> iState;
	jsval vParam = JSVAL_NULL;
	vParam = INT_TO_JSVAL(iState);
	JS_SetProperty(cx, GetEventGO(), "infoState", &vParam );
	NotifyPlayerListeners(EVID_onShowInfoState);
	JS_DeleteProperty(cx, GetEventGO(), "infoState");

	if( GetContext() && GetContext()->mpServiceWorldMan)
		GetContext()->mpServiceWorldMan->SetSessionStateValue( service::CSS_CLIENT_WORK_STATE, iState);
}

void mlSceneManager::OnVisibilityDistanceKoefChangedInternal(mlSynchData& aData)
{
	int koef = 0;
	aData >> koef;
	jsval vParam = JSVAL_NULL;
	vParam = INT_TO_JSVAL(koef);
	JS_SetProperty(cx, GetEventGO(), "koef", &vParam );
	NotifyPlayerListeners(EVID_onVisibilityDistanceKoefChanged);
	JS_DeleteProperty(cx, GetEventGO(), "koef");
}

JSBool JSFUNC_openURL(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc < 1) return JS_FALSE;
	*rval = INT_TO_JSVAL(0);
	mlString sCommand;
	for(int ii=0; ii<(int)argc; ii++){
		JSString* jss=JS_ValueToString(cx,argv[ii]);
		if(ii > 0) sCommand+=L' ';
		sCommand+=wr_JS_GetStringChars(jss);
	}
	if(sCommand.find(L"http://")==-1)
		return JS_FALSE;
	omsContext* pContext = GPSM(cx)->GetContext();
	unsigned int auErrorCode = 0;
	bool abExecResult = false;
#ifdef CSCL
	if(pContext->mpApp){
		abExecResult = pContext->mpApp->execSysCommand(sCommand.c_str(), &auErrorCode);
	}
#else
	oms::omsIApplication *pApp = pContext->mpApp;
//	if (AppPtr)
//		iRetVal = AppPtr->execSysCommand(sCommand.c_str());
#endif
	*rval = INT_TO_JSVAL( auErrorCode);
	return JS_TRUE;
}

JSBool startProcess(const wchar_t* apwcProcessName, JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = INT_TO_JSVAL(0);

	if(apwcProcessName == NULL) 
		return JS_FALSE;
	
	omsContext* pContext = GPSM(cx)->GetContext();
	mlString sCommand;

	if( wcsstr(apwcProcessName, L":") == NULL)
	{
		#define BUFSIZE 4096
		wchar_t	moduleFileName[ BUFSIZE] = L"";
		int retval = GetModuleFileNameW( NULL, moduleFileName, BUFSIZE);
		if (retval == 0) 
		{
			// Handle an error condition.
			if( pContext->mpLogWriter)
				pContext->mpLogWriter->WriteLn("GetFullPathName failed. GetLastError() = ", GetLastError());

			*rval = INT_TO_JSVAL(1);
			return JS_TRUE;
		}

		wchar_t *lpFileName = NULL;
		wchar_t	modulePath[ BUFSIZE] = L"";
		retval = ::GetFullPathNameW(moduleFileName, BUFSIZE, modulePath, &lpFileName);
		if (retval == 0) 
		{
			// Handle an error condition.
			if( pContext->mpLogWriter)
				pContext->mpLogWriter->WriteLn("GetFullPathName failed. GetLastError() = ", GetLastError());

			*rval = INT_TO_JSVAL(1);
			return JS_TRUE;
		}

		if( lpFileName != NULL)
			*lpFileName = 0;

		sCommand = modulePath;

		sCommand += apwcProcessName;
		int nPos = sCommand.find(L"\\\\");
		if(nPos > -1)
			sCommand.replace(nPos, 2, L"\\");
	}
	else
	{
		sCommand = apwcProcessName;
	}
	
	// Get arguments
	for(int ii=0; ii<(int)argc; ii++){
		JSString* jss=JS_ValueToString(cx,argv[ii]);
		sCommand+=L' ';
		sCommand+=wr_JS_GetStringChars(jss);
	}

	bool execResult = false;
	unsigned int auErrorCode = 0;

	if( pContext->mpLogWriter)
		pContext->mpLogWriter->WriteLn("ExecSysCommand command line: ", sCommand.c_str());

	if( pContext->mpApp)
		execResult = pContext->mpApp->execSysCommand(sCommand.c_str(), &auErrorCode);

	if( pContext->mpLogWriter)
		pContext->mpLogWriter->WriteLn("ExecSysCommand command result = ", execResult, ", errorCode = ", auErrorCode);

	*rval = INT_TO_JSVAL(auErrorCode);
	return JS_TRUE;
}

JSBool JSFUNC_startProxySettings(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	return startProcess( L"settings.exe", cx, obj, argc, argv, rval);
}

JSBool JSFUNC_startTracer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	return startProcess( L"tracer.exe", cx, obj, argc, argv, rval);
}

JSBool JSFUNC_startUpdater(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	omsContext* pContext = GPSM(cx)->GetContext();

	if( pContext->mpLogWriter)
		pContext->mpLogWriter->WriteLnLPCSTR("mlRMMLJSPlayer.cpp::JSFUNC_startUpdater");

	std::wstring rootDir = pContext->mp3DWS->GetApplicationRoot();
	std::wstring appDir = pContext->mpLogWriter->GetApplicationDataDirectory().c_str();
	//MessageBoxW( 0, appDir.c_str(), L"", MB_OK);
	//MessageBox( 0, pContext->mpLogWriter->GetApplicationDataDirectory().c_str(), "", MB_OK);

	std::wstring sCommandWS;
	for(int ii=0; ii<(int)argc; ii++){
		JSString* jss=JS_ValueToString(cx,argv[ii]);
		sCommandWS+=L' ';
		sCommandWS+=wr_JS_GetStringChars(jss);

	}

	/*int posEnc = sCommandWS.find(L"encoding=\"0\"");
	if (posEnc!=-1)
	{
		int pos1 = sCommandWS.find(L"password=\"");
		if (pos1!=-1)
		{
			std::wstring bCommand = sCommandWS.substr(0,posEnc+10);
			std::wstring endCommand = sCommandWS.substr(posEnc + 11, sCommandWS.length());
			sCommandWS = bCommand;
			sCommandWS += L"1";
			sCommandWS += endCommand;

			bCommand = sCommandWS.substr(0,pos1+10);
			std::wstring pass = sCommandWS.substr(pos1+10,sCommandWS.length());
			int pos2 = pass.find_first_of(L" ");
			endCommand = pass.substr(pos2-1, pass.length());
			pass = pass.substr(0,pos2-1);
			if (pass!=L"")
			{
				USES_CONVERSION;
				std::string encPass = W2A(pass.c_str()) ;
				pContext->mpRM->GetEncodingPassword(encPass);
				pass = A2W(encPass.c_str());
				sCommandWS = bCommand;
				sCommandWS += pass;
				sCommandWS += endCommand;
			}
		}
	}*/



	// добавим параметр - путь к клиенту
	/*mlString clientPathWS = rootDir;//cLPWCSTRq(rootDir.c_str());
	sCommandWS += L" -clientPath \"";
	sCommandWS += clientPathWS;
	sCommandWS += L"\"";*/

	//std::string sCommand = cLPCSTRq(sCommandWS.c_str());
	CStartUpdater* stUpdate = new CStartUpdater(); 

	unsigned int errorCode = LAUNCH_ERROR_NO_ERROR;
	unsigned int systemErrorCode = 0;
	std::wstring updaterPath = stUpdate->prepareUpdate(rootDir, appDir, errorCode, systemErrorCode);

	if( pContext->mpLogWriter != NULL)
	{
		pContext->mpLogWriter->WriteLn("prepareUpdate errorCode = ", errorCode, ",  systemErrorCode", systemErrorCode);
		pContext->mpLogWriter->WriteLn("prepareUpdate updaterPath = ", updaterPath.c_str());
	}

	if (errorCode != LAUNCH_ERROR_NO_ERROR)
	{	
		*rval=BOOLEAN_TO_JSVAL(false);

		mlTrace(cx, "[RMML]JSFUNC_startUpdater return false and errorCode==%d, systemErrorCode==%d", errorCode, systemErrorCode);

		return JS_TRUE; 
	}

	std::wstring updaterFullFilePath = L"\"" + updaterPath + L"updater.exe\"" + sCommandWS;

	if( pContext->mpLogWriter != NULL)
	{
		pContext->mpLogWriter->WriteLn("updaterFullFilePath = ", updaterFullFilePath.c_str());
	}

	startProcess( updaterFullFilePath.c_str(), cx, obj, 0, argv, rval);

	*rval=BOOLEAN_TO_JSVAL(true);

	mlTrace(cx, "[RMML]JSFUNC_startUpdater return true");

	return JS_TRUE;
}

JSBool JSFUNC_restart(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	return startProcess( L"vacademia.exe", cx, obj, argc, argv, rval);
}

JSBool JSFUNC_minimize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
//	if(argc > 0){
//	}
	omsContext* pContext = GPSM(cx)->GetContext();
	if(pContext->mpApp){
		pContext->mpApp->minimize();
	}
	return JS_TRUE;
}

JSBool JSFUNC_exit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int aiExitCode = 0;

	if(argc > 0)
	{
		if(!JSVAL_IS_INT(argv[0]))
		{
			JS_ReportError(cx,"Player.exit-method first parameter must be an integer");
			return JS_TRUE;
		}
		aiExitCode = JSVAL_TO_INT(argv[0]);

		wchar_t* apwExitComment = NULL;
		if(argc > 1)
		{
			if( JSVAL_IS_STRING(argv[1]))
			{
				JSString* jssExitComment=JSVAL_TO_STRING(argv[1]);
				apwExitComment = wr_JS_GetStringChars(jssExitComment);
			}
		}

		omsContext* pContext = GPSM(cx)->GetContext();
		pContext->mpServiceWorldMan->SetSessionStateValue( service::CSS_EXIT_CODE, aiExitCode, apwExitComment);
	}

	omsContext* pContext = GPSM(cx)->GetContext();
	if(pContext->mpApp){
#ifdef CSCL
		pContext->mpRM->OnExit();
		pContext->mpApp->exit();
#endif
	}
	return JS_TRUE;
}

JSBool JSFUNC_setGlobalDataSpace(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
//	if(argc > 0){
//	}
	// ??
	return JS_TRUE;
}

void mlSceneManager::OnWindowSizeChangedInternal(mlSynchData& aData)
{
	int aClientWidth = 0;
	aData >> aClientWidth;
	int aClientHeight = 0;
	aData >> aClientHeight;

	jsval vPlayer = JSVAL_NULL;
	JS_GetProperty(cx, JS_GetGlobalObject(cx), GJSONM_PLAYER, &vPlayer);

	if(vPlayer == JSVAL_NULL)
		return;
	JSObject* jsoPlayer = JSVAL_TO_OBJECT(vPlayer);
	SetWindowSubObject(cx, jsoPlayer, aClientWidth, aClientHeight);

	NotifyPlayerOnWindowSizeChanged( aClientWidth, aClientHeight);
}

JSBool JSFUNC_resize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc < 2){
		JS_ReportError(cx, "resizeWindow method must get two unsigned integer arguments at least");
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	uint16 uiClientWidth = 0;
	uint16 uiClientHeight = 0;
	if(!JS_ValueToUint16(cx, argv[0], &uiClientWidth) || 
	   !JS_ValueToUint16(cx, argv[1], &uiClientHeight)){
		JS_ReportError(cx, "resizeWindow method must get two unsigned integer arguments at least");
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	int iFullScreen = -1; // undefined
	if(argc >= 3){
		JSBool bFullScreen = false;
		if(!JS_ValueToBoolean(cx, argv[2], &bFullScreen)){
			JS_ReportError(cx, "third resizeWindow method argument must get boolean value");
			*rval = JSVAL_FALSE;
			return JS_TRUE;
		}
		if(bFullScreen)
			iFullScreen = 1;
		else
			iFullScreen = 0;
	}
	bool abScreenChangedCorrectly = false;
	omsContext* pContext = GPSM(cx)->GetContext();
	if(pContext->mpApp)
		abScreenChangedCorrectly = pContext->mpApp->ResizeWindow(uiClientWidth, uiClientHeight, iFullScreen);

	jsval vPlayer = JSVAL_NULL;
	JS_GetProperty(cx, JS_GetGlobalObject(cx), GJSONM_PLAYER, &vPlayer);
	if(vPlayer == JSVAL_NULL)
		return JS_FALSE;
    JSObject* jsoPlayer = JSVAL_TO_OBJECT(vPlayer);

	bool abCave = false;

	/*if (pContext->mpRM)
		abCave = pContext->mpRM->IsCaveSupport();*/

	if (!abCave || (abCave && !abScreenChangedCorrectly) )
	{
		SetWindowSubObject(cx, jsoPlayer);
	}
	else
	{
		SetWindowSubObject(cx, jsoPlayer, uiClientWidth, uiClientHeight);
	}

	if (abScreenChangedCorrectly)
	{
		GPSM(cx)->NotifyPlayerOnWindowSizeChanged(uiClientWidth, uiClientHeight);
		GPSM(cx)->DontUpdateOneTime();
		GPSM(cx)->DontUpdateOneTime();
		pContext->mpRM->GetExtendedRM()->UpdateAndRender();
		*rval = JSVAL_TRUE;
	}
	else
	{
		*rval = JSVAL_FALSE;
	}

	return JS_TRUE;
}

JSBool JSFUNC_setSetting(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	// должно быть 2 строковых аргумента
	if(argc < 2){
		JS_ReportError(cx, "setSetting method must get two string arguments");
		return JS_TRUE;
	}
	const wchar_t* pwcName = NULL;
	if(JSVAL_IS_STRING(argv[0])){
		JSString* jss = JSVAL_TO_STRING(argv[0]);
		pwcName = wr_JS_GetStringChars(jss);
	}else{
		JS_ReportError(cx, "setSetting first argument must be a string");
		return JS_TRUE;
	}
	const wchar_t* pwcValue = NULL;
	if(JSVAL_IS_STRING(argv[1])){
		JSString* jss = JSVAL_TO_STRING(argv[1]);
		pwcValue = wr_JS_GetStringChars(jss);
	}else{
		JS_ReportError(cx, "setSetting first argument must be a string");
		return JS_TRUE;
	}
	omsContext* pContext = GPSM(cx)->GetContext();
	if(pContext->mpApp){
		if(pContext->mpApp->SetSetting(pwcName, pwcValue))
			*rval = JSVAL_TRUE;
	}
	return JS_TRUE;
}

JSBool JSFUNC_getUserSetting(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = INT_TO_JSVAL(-1);
	// должно быть 2 строковых аргумента
	if(argc < 1 || argc > 1){
		JS_ReportError(cx, "getUserSetting method must get one string arguments");
		return JS_TRUE;
	}
	const wchar_t* pwcName = NULL;
	if(JSVAL_IS_STRING(argv[0])){
		JSString* jss = JSVAL_TO_STRING(argv[0]);
		pwcName = wr_JS_GetStringChars(jss);
	}else{
		JS_ReportError(cx, "getUserSetting first argument must be a string");
		return JS_TRUE;
	}
	omsContext* pContext = GPSM(cx)->GetContext();
	if (wcscmp(pwcName, L"server") == 0)
	{
		char pchValue[3000] = "";
		if(pContext->mpApp){
			if ((!pContext->mpApp->GetSettingFromIni("paths", "server_ip", pchValue, 2995)) &&
				(!pContext->mpApp->GetSettingFromIni("paths", "server", pchValue, 2995)))
			{
				JS_ReportWarning(cx, "Unknown error occured while calling getSetting method");
			}else{
				*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, cLPWCSTR(pchValue)));
				return JS_TRUE;
			}
		}
	}
	if(pContext->mpRM)
	{
		char pchValue[3000] = "";
		if( pContext->mpRM)
		{
			pContext->mpRM->GetUserSetting((wchar_t*)pwcName, pchValue, sizeof(pchValue));
			*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, cLPWCSTR(pchValue)));
		}
	}
	return JS_TRUE;
}

JSBool JSFUNC_setUserSetting(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = INT_TO_JSVAL(-1);
	// должно быть 2 строковых аргумента
	if(argc < 2 || argc > 2){
		JS_ReportError(cx, "setUserSetting method must get two string arguments");
		return JS_TRUE;
	}
	const wchar_t* pwcName = NULL;
	if(JSVAL_IS_STRING(argv[0])){
		JSString* jss = JSVAL_TO_STRING(argv[0]);
		pwcName = wr_JS_GetStringChars(jss);
	}else{
		JS_ReportError(cx, "setUserSetting first argument must be a string");
		return JS_TRUE;
	}
	const wchar_t* pwcValue = NULL;
	if(JSVAL_IS_STRING(argv[1])){
		JSString* jss = JSVAL_TO_STRING(argv[1]);
		pwcValue = wr_JS_GetStringChars(jss);
	}else{
		JS_ReportError(cx, "setUserSetting second argument must be a string");
		return JS_TRUE;
	}
	omsContext* pContext = GPSM(cx)->GetContext();
	if(pContext->mpRM){
		*rval = INT_TO_JSVAL(pContext->mpRM->SetUserSetting((wchar_t*)pwcName, (wchar_t*)pwcValue));
	}
	return JS_TRUE;
}


JSBool JSFUNC_getSetting(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JS_GetEmptyStringValue(cx);
	// должен быть один строковый аргумент
	if(argc < 1){
		JS_ReportError(cx, "getSetting method must get one or two string argument");
		return JS_TRUE;
	}
	const wchar_t* pwcName = NULL;
	if(JSVAL_IS_STRING(argv[0])){
		JSString* jss = JSVAL_TO_STRING(argv[0]);
		pwcName = wr_JS_GetStringChars(jss);
	}else{
		JS_ReportError(cx, "getSetting first argument must be a string");
		return JS_TRUE;
	}
	const wchar_t* pwcSectionName = NULL;
	if (argc == 2)
	{
		if ( JSVAL_IS_STRING(argv[1]))
		{
			JSString* jss = JSVAL_TO_STRING(argv[1]);
			pwcSectionName = wr_JS_GetStringChars(jss);
		}
		else
		{
			JS_ReportError(cx, "getSetting second argument must be a string");
			return JS_TRUE;
		}
	}
	pwcSectionName = pwcSectionName == NULL ? L"" : pwcSectionName;
	wchar_t pwcValue[3000] = L"";
	omsContext* pContext = GPSM(cx)->GetContext();
	/*if(!wcscmp( pwcName, L"rights"))
	{
		wcscpy(pwcValue, L"lite");
		*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwcValue));
		return JS_TRUE;
	}*/
	if(pContext->mpApp){
		if(!pContext->mpApp->GetSetting(pwcName, pwcValue, 2995, pwcSectionName)){
			//JS_ReportWarning(cx, "Unknown error occured while calling getSetting method");
		}else{
			*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwcValue));
		}
	}
	return JS_TRUE;
}

// getLocalFilePath(sFilter, [sStartPath[, bExists]])
// sFilter - фильтр имен файлов в формате "All|*.*|Text|*.TXT"
// sStartPath - путь к файлу
// bExists == true, то путь должен быть к существующему файлу (open)
JSBool JSFUNC_getLocalFilePath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JS_GetEmptyStringValue(cx);
	bool bExists = false;
	const wchar_t* pwcFilter = L"";
	const wchar_t* pwcStartPath = L"";
	jsval vArg;
	unsigned int iArg = 0;
	// sFilter
	if(argc > iArg){
		vArg = argv[iArg++];
		if(!JSVAL_IS_STRING(vArg)){
			JS_ReportError(cx, "First argument of getLocalFilePath must be a string");
			return JS_TRUE;
		}
		pwcFilter = wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
	}
	// sStartPath
	if(argc > iArg){
		vArg = argv[iArg++];
		if(!JSVAL_IS_STRING(vArg)){
			JS_ReportError(cx, "Second argument of getLocalFilePath must be a string");
			return JS_TRUE;
		}
		pwcStartPath = wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
	}
	// bExists
	if(argc > iArg){
		vArg = argv[iArg++];
		if(!JSVAL_IS_BOOLEAN(vArg)){
			JS_ReportError(cx, "Third argument of getLocalFilePath must be a boolean");
			return JS_TRUE;
		}
		bExists = JSVAL_TO_BOOLEAN(vArg);
	}
	// sDefaultName
	const wchar_t* pwcDefaultName = L"";
	if(argc > iArg){
		vArg = argv[iArg++];
		if(!JSVAL_IS_STRING(vArg)){
			JS_ReportError(cx, "Five argument of getLocalFilePath must be a string");
			return JS_TRUE;
		}
		pwcDefaultName = wr_JS_GetStringChars(JSVAL_TO_STRING(vArg));
	}

	omsContext* pContext = GPSM(cx)->GetContext();
	wchar_t pwcPath[3000] = L"";
	if(pContext->mpApp)
	{
		mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
		pSM->JSScriptCancelTerimation();

		if(pContext->mpApp->GetLocalFilePath(pwcFilter, pwcStartPath, bExists, pwcPath, 2995, pwcDefaultName)){
			*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, pwcPath));
		}
	}
	return JS_TRUE;
}

JSBool JSFUNC_getScreenSize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	omsContext* pContext = GPSM(cx)->GetContext();
	int iWidth = 0, iHeight = 0, iOffsetX = 0;
	pContext->mpApp->GetDesktopSize(iOffsetX, iWidth, iHeight);
	if(iWidth == 0){
		*rval = JSVAL_NULL;
	}else{
		JSObject* jso = JS_NewObject( cx, NULL, NULL, NULL);
		jsval v = INT_TO_JSVAL(iWidth);
		JS_SetProperty(cx, jso, "width", &v);
		v = INT_TO_JSVAL(iHeight);
		JS_SetProperty(cx, jso, "height", &v);
		*rval = OBJECT_TO_JSVAL(jso);
	}
	return JS_TRUE;
}

JSBool JSFUNC_setAudioMode( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if ( argc < 2 || argc > 2)
	{
		JS_ReportError(cx, "setAudioMode method must get two string arguments");
		return JS_TRUE;
	}
	if ( !JSVAL_IS_STRING( argv[ 0]))
	{
		JS_ReportError(cx, "First argument of setAudioMode must be a string");
		return JS_TRUE;
	}
	if ( !JSVAL_IS_INT( argv[ 1]))
	{
		JS_ReportError(cx, "Second argument of setAudioMode must be a int");
		return JS_TRUE;
	}
	JSString *sType = JSVAL_TO_STRING( argv[ 0]);
	int iMode = JSVAL_TO_INT( argv[ 1]);	
	omsContext* pContext = GPSM(cx)->GetContext();
	pContext->mpRM->SetAudioMode( (wchar_t *) JS_GetStringChars( sType), iMode);
	return JS_TRUE;
}

JSBool JSFUNC_setAudioVolume( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if ( argc < 2 || argc > 2)
	{
		JS_ReportError(cx, "setAudioVolume method must get two string arguments");
		return JS_TRUE;
	}
	if ( !JSVAL_IS_STRING( argv[ 0]))
	{
		JS_ReportError(cx, "First argument of setAudioVolume must be a string");
		return JS_TRUE;
	}
	if ( !JSVAL_IS_NUMBER( argv[ 1]))
	{
		JS_ReportError(cx, "Second argument of setAudioVolume must be a int or double");
		return JS_TRUE;
	}
	JSString *sType = JSVAL_TO_STRING( argv[ 0]);
	float fVolume = (float) JSVAL_TO_INT( argv[ 1]) / 100.0f;	
	omsContext* pContext = GPSM(cx)->GetContext();
	pContext->mpRM->SetAudioVolume( (wchar_t *) JS_GetStringChars( sType), fVolume);
	return JS_TRUE;
}

JSBool JSFUNC_enableLastScreen( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if ( argc == 0 || !JSVAL_IS_BOOLEAN( argv[ 0]))
	{
		JS_ReportError(cx, "First argument of enableLastScreen must be a boolean");
		return JS_TRUE;
	}
	bool isEnabled = JSVAL_TO_BOOLEAN( argv[ 0]);
	omsContext* pContext = GPSM(cx)->GetContext();
	pContext->mpRM->EnableLastScreen(isEnabled);
	return JS_TRUE;
}

JSBool JSFUNC_assert( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if ( argc == 0)
	{
		JS_ReportError(cx, "Player.assert should take one bool argument");
		return JS_TRUE;
}
    if( !JSVAL_IS_BOOLEAN( argv[ 0]))
	{
		JS_ReportError(cx, "Argument of Player.assert must be a boolean");
	}

	bool assertValue = JSVAL_TO_BOOLEAN( argv[ 0]);
	if( !assertValue)
	{
		omsContext* pContext = GPSM(cx)->GetContext();
		mlString sCallStack = GPSM(cx)->GetJSCallStack();
		pContext->mpApp->ShowMessageBox( sCallStack.c_str(), L"Assert failed");
	}
	return JS_TRUE;
}

JSBool JSFUNC_getTimeTicks( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	unsigned long ulTime = GPSM(cx)->GetContext()->mpApp->GetTimeStamp();
	*rval = INT_TO_JSVAL((int)ulTime);
	return JS_TRUE;
}

JSBool JSFUNC_getClipboardContentType( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	unsigned long uType=0;
	if ( IsClipboardFormatAvailable(CF_BITMAP))
	{
		uType = 2;
	}
	else if (IsClipboardFormatAvailable(CF_TEXT))
	{
		uType = 1;
	}

	*rval = INT_TO_JSVAL((int)uType);
	return JS_TRUE;
}

JSBool JSFUNC_getMaxAntialiasingPower( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	int power = GPSM(cx)->GetContext()->mpRM->GetMaxAntialiasingPower();
	*rval = INT_TO_JSVAL(power);
	return JS_TRUE;
}


JSBool JSFUNC_startTrackVisibleScreensInCurrentLocation( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	GPSM(cx)->GetContext()->mpMapMan->StartTrackVisibleScreensInCurrentLocation();
	return JS_TRUE;
}

JSBool JSFUNC_endTrackVisibleScreensInCurrentLocation( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	GPSM(cx)->GetContext()->mpMapMan->StartTrackVisibleScreensInCurrentLocation();
	return JS_TRUE;
}

JSBool JSFUNC_getVisibleScreensInCurrentLocation( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	std::map<std::wstring, int> arr = GPSM(cx)->GetContext()->mpMapMan->GetVisibleScreensInCurrentLocation();
	
	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);
	jsval val;

	std::map<std::wstring, int>::iterator it = arr.begin();
	std::map<std::wstring, int>::iterator itEnd = arr.end();

	for ( ; it != itEnd; it++)
	{
		mlJS_NewDoubleValue(cx, (*it).second, &val); 
		wr_JS_SetUCProperty(cx,jso, (*it).first.c_str(),(*it).first.size(), &val);
	}
	

	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

JSBool JSFUNC_getEngineWarningID( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	std::vector<unsigned int> ids = GPSM(cx)->GetContext()->mpRM->GetEngineWarningIDs();

	jsval* pjsvals = MP_NEW_ARR( jsval, ids.size()+1);
	for(unsigned int ii = 0; ii < ids.size(); ii++)
	{
		mlJS_NewDoubleValue(cx, ids[ii], pjsvals + ii); 		
	}
	JSObject* jso=JS_NewArrayObject(cx,ids.size(),pjsvals);
	MP_DELETE_ARR( pjsvals);
	*rval=OBJECT_TO_JSVAL(jso);

	return JS_TRUE;
}

JSBool JSFUNC_getToolbarParams(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	omsContext* pContext = GPSM(cx)->GetContext();	
	res::resIResource *pResource = NULL;
	omsresult ires = 0;

	int toolbarOffsetX, toolbarOffsetY, toolbarWidth, toolbarHeight;
	pContext->mpApp->GetToolbarParams( toolbarOffsetX, toolbarOffsetY, toolbarWidth, toolbarHeight);

	JSObject* jsoObj = JS_NewObject(cx,NULL,NULL,NULL);
	jsval vX = INT_TO_JSVAL(toolbarOffsetX);
	JS_SetProperty(cx, jsoObj, "x", &vX);
	jsval vY = INT_TO_JSVAL(toolbarOffsetX);
	JS_SetProperty(cx, jsoObj, "y", &vY);
	jsval vWidth = INT_TO_JSVAL(toolbarWidth);
	JS_SetProperty(cx, jsoObj, "width", &vWidth);
	jsval vHeight = INT_TO_JSVAL(toolbarHeight);
	JS_SetProperty(cx, jsoObj, "height", &vHeight);

	*rval = OBJECT_TO_JSVAL( jsoObj);

	return JS_TRUE;
}

JSBool JSFUNC_getSessionID( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	unsigned int sessionID = GPSM(cx)->GetContext()->mpComMan->GetSessionID();
	*rval = INT_TO_JSVAL(sessionID);
	return JS_TRUE;
}

JSBool JSFUNC_getSessionKey( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	std::wstring compName;
	std::string processName;
	unsigned int processorCount;
	unsigned int realCoreCount;
	bool hyperthreadingEnabled;
	unsigned __int64 physicalMemorySize;
	unsigned __int64 virtualMemorySize;
	std::string osVersion;
	unsigned char osBits;
	GPSM(cx)->GetContext()->mpRM->GetComputerInfo(compName, processName, processorCount, realCoreCount, hyperthreadingEnabled, 
		physicalMemorySize, virtualMemorySize, osVersion, osBits);
	std::wstring res = compName;

	char buffer[80];
	buffer[0] = 0;
	time_t seconds = time(NULL);
	tm timeinfo;
	localtime_s(&timeinfo, &seconds);
	char* format = "_%m_%d_%Y_%I_%M_%S";
	strftime(buffer, 80, format, &timeinfo);

	res += cLPWCSTR(buffer);
	
	JSString* jssRes = wr_JS_NewUCStringCopyZ(cx, res.c_str());
	*rval = STRING_TO_JSVAL(jssRes);
	return JS_TRUE;
}

JSBool JSFUNC_getProxySettings( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	std::wstring res = L"";
	
	JSString* jssRes = wr_JS_NewUCStringCopyZ(cx, res.c_str());
	*rval = STRING_TO_JSVAL(jssRes);
	return JS_TRUE;
}

JSBool JSFUNC_isAnsiFilePath( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	*rval = BOOLEAN_TO_JSVAL(false);

	// должен быть один строковый аргумент
	if(argc != 1){
		JS_ReportError(cx, "isAnsiFilePath method must get one and only one string argument");
		return JS_TRUE;
	}

	const wchar_t* pwcFilePath = NULL;
	if(JSVAL_IS_STRING(argv[0]))
	{
		JSString* jss = JSVAL_TO_STRING(argv[0]);
		pwcFilePath = wr_JS_GetStringChars(jss);
	}
	else
	{
		JS_ReportError(cx, "isAnsiFilePath argument must be a string");
		return JS_TRUE;
	}

	cLPCSTR	sAnsiFileName( pwcFilePath);
	cLPWCSTR	sUnicodeFileName( sAnsiFileName);

	if( wcscmp( sUnicodeFileName, pwcFilePath) == 0)
		*rval = BOOLEAN_TO_JSVAL(true);

	return JS_TRUE;
}

JSBool JSFUNC_startKinect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	GPSM(cx)->GetContext()->mpMapMan->StartKinect();
	return JS_TRUE;
}

JSBool JSFUNC_getKinectPoints( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	const int pointCount = 20;
	float points[pointCount * 3];
	bool res = GPSM(cx)->GetContext()->mpMapMan->GetKinectPoints(&points[0]);

	if(!res){
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}

	jsval vals[pointCount + 1];

	for (int i = 0; i < pointCount; i++)
	{
		JSObject* mjsoPos = mlPosition3D::newJSObject(cx);
		SAVE_FROM_GC(cx,obj,mjsoPos);
		mlPosition3D* pPos3D = (mlPosition3D*)JS_GetPrivate(cx,mjsoPos);
		ml3DPosition tmp;
		tmp.x = points[i * 3 + 0];
		tmp.y = points[i * 3 + 1];
		tmp.z = points[i * 3 + 2];
		pPos3D->SetPos(tmp);
		vals[i] = OBJECT_TO_JSVAL(mjsoPos);
	}
	
	JSObject* jsoObjs=JS_NewArrayObject(cx,pointCount, vals);

	for (int i = 0; i < pointCount; i++)
	{
		JSObject* jso = JSVAL_TO_OBJECT(vals[i]);
		FREE_FOR_GC(cx, obj, jso);
	}

	//
	*rval=OBJECT_TO_JSVAL(jsoObjs);

	return JS_TRUE;
}

JSBool JSFUNC_stopKinect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	GPSM(cx)->GetContext()->mpMapMan->StopKinect();
	return JS_TRUE;
}

JSBool JSFUNC_setKinectMode( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if ( argc!=1)
	{
		JS_ReportError(cx, "setKinectMode must get one int argument");
		return JS_TRUE;
	}

	if ( !JSVAL_IS_INT( argv[0]))
	{
		JS_ReportError(cx, "Argument of setKinectMode must be a int");
		return JS_TRUE;
	}

	int iMode = JSVAL_TO_INT( argv[0]);
	GPSM(cx)->GetContext()->mpMapMan->SetKinectMode(iMode);
	return JS_TRUE;
}

// ---
JSBool JSFUNC_setMarkerType( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if ( argc!=1)
	{
		JS_ReportError(cx, "setMarkerType must get one int argument");
		return JS_TRUE;
	}

	if ( !JSVAL_IS_INT( argv[0]))
	{
		JS_ReportError(cx, "Argument of setMarkerType must be a int");
		return JS_TRUE;
	}

	int iMarker = JSVAL_TO_INT( argv[0]);
	GPSM(cx)->GetContext()->mpMapMan->SetMarkerType(iMarker);
	return JS_TRUE;
}
// ---
JSBool JSFUNC_enableFaceTracking ( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	GPSM(cx)->GetContext()->mpMapMan->EnableFaceTracking();
	return JS_TRUE;
}

JSBool JSFUNC_disableFaceTracking ( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	GPSM(cx)->GetContext()->mpMapMan->DisableFaceTracking();
	return JS_TRUE;
}

// Возвращает true - если пользователь включил режим ctrl + alt + shift + d
JSBool JSFUNC_getKeysShortcutsEnabled( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_FALSE;

	if( GPSM(cx)->GetContext() && GPSM(cx)->GetContext()->mpApp)
	{
		if( GPSM(cx)->GetContext()->mpApp->GetKeysShortcutsEnabled())
			*rval = JSVAL_TRUE;
	}

	return JS_TRUE;
}

// Если параметры GuardFlags реальности содержат флаг public const uint WAIT_GUARD_OBJECT, то функция
// вычисляет имя объекта, который должен использоваться для ожидания входа в реальность.
// Если такой флаг не установлен - возвращает пустую строку
JSBool JSFUNC_getRealityGuardName( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, L""));

	oms::omsContext* pContext = GPSM(cx)->GetContext();
	if(pContext->mpMapMan == NULL)
	{
		JS_ReportError(cx,"getRealityGuardName-method MapManager is not initialized");
		return JS_TRUE;
	}

	if (argc != 1)
	{
		JS_ReportError(cx,"getRealityGuardName-method must get only one parameter");
		return JS_TRUE;
	}

	if(!JSVAL_IS_INT(argv[0]))
	{
		JS_ReportError(cx,"getRealityGuardName-method first parameter must be an integer");
		return JS_TRUE;
	}

	unsigned int auRealityID = JSVAL_TO_INT(argv[0]);

	#define	GUARD_NAME_LENGTH 50 /*see objects_map.name*/
	wchar_t	buffer[GUARD_NAME_LENGTH + 1] = {0};
	
	cm::MapManagerError errorCode = pContext->mpMapMan->GetRealityGuardName( auRealityID, buffer, GUARD_NAME_LENGTH);
	if( errorCode == cm::MapManagerErrors::ERROR_NO_ERROR)
	{
		*rval = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, buffer));
	}
	else
	{
		JS_ReportError(cx,"getRealityGuardName-method failed to get reality guard name");
	}

	return JS_TRUE;
}

JSBool JSFUNC_getKinectState( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	bool isHeadFound;
	bool isLeftHandFound;
	bool isLeftLegFound;
	bool isRightHandFound;
	bool isRightLegFound;
	bool isOneSkeletFound;
	bool isMarkersNotDetected;

	GPSM(cx)->GetContext()->mpMapMan->GetKinectState(isHeadFound, isLeftHandFound, isLeftLegFound, 
		isRightHandFound, isRightLegFound, isOneSkeletFound, isMarkersNotDetected);

	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);
	SAVE_FROM_GC(cx, obj, jso);
	JS_DefineProperty(cx,jso,"head",BOOLEAN_TO_JSVAL(isHeadFound),0,0,0);
	JS_DefineProperty(cx,jso,"left_hand",BOOLEAN_TO_JSVAL(isLeftHandFound),0,0,0);
	JS_DefineProperty(cx,jso,"right_hand",BOOLEAN_TO_JSVAL(isRightHandFound),0,0,0);
	JS_DefineProperty(cx,jso,"left_leg",BOOLEAN_TO_JSVAL(isLeftLegFound),0,0,0);
	JS_DefineProperty(cx,jso,"right_leg",BOOLEAN_TO_JSVAL(isRightLegFound),0,0,0);
	JS_DefineProperty(cx,jso,"one_skelet",BOOLEAN_TO_JSVAL(isOneSkeletFound),0,0,0);
	JS_DefineProperty(cx,jso,"no_markers",BOOLEAN_TO_JSVAL(isMarkersNotDetected),0,0,0);
	*rval=OBJECT_TO_JSVAL(jso);
	FREE_FOR_GC(cx, obj, jso);

	return JS_TRUE;
}

JSBool JSFUNC_getDriverLink( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	std::string link = "";
	std::string videoURL = "";
	unsigned char linkType = 0;

	mlSceneManager* pSM = GPSM(cx);
	if (pSM->GetContext() && pSM->GetContext()->mpRM)
	{
		pSM->GetContext()->mpRM->GetDriverLink(link, videoURL, linkType);
	}
	

	JSString* jssRes = wr_JS_NewUCStringCopyZ(cx, cLPWCSTR(link.c_str()));
	JSString* jssRes2 = wr_JS_NewUCStringCopyZ(cx, cLPWCSTR(videoURL.c_str()));

	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);
	SAVE_FROM_GC(cx, obj, jso);
	JS_DefineProperty(cx,jso,"link",STRING_TO_JSVAL(jssRes),0,0,0);
	JS_DefineProperty(cx,jso,"video",STRING_TO_JSVAL(jssRes2),0,0,0);
	JS_DefineProperty(cx,jso,"linkType",INT_TO_JSVAL(linkType),0,0,0);
	*rval=OBJECT_TO_JSVAL(jso);
	FREE_FOR_GC(cx, obj, jso);
	
	return JS_TRUE;
}

JSBool JSFUNC_isComputerSlow( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	bool isComputerSlow = false;
	mlSceneManager* pSM = GPSM(cx);
	if (pSM->GetContext() && pSM->GetContext()->mpRM)
	{
		isComputerSlow = pSM->GetContext()->mpRM->IsComputerSlow();
	}
	
	*rval=BOOLEAN_TO_JSVAL(isComputerSlow);
		
	return JS_TRUE;
}

struct SWindowClientPosAndSize {
	int w;
	int h;
	int x;
	int y;
};

SWindowClientPosAndSize prevPosAndSize;

bool saveWindowPosAndSize(JSContext *cx, SWindowClientPosAndSize& posAndSize) {
	oms::mwIWindow* pWindow = GPSM(cx)->GetContext()->mpWindow;
	if(pWindow!=NULL){
		HWND hwnd = (HWND)pWindow->GetMainWindowHandle();
		RECT rc;
		GetClientRect(hwnd, &rc);
		ClientToScreen(hwnd, reinterpret_cast<POINT*>(&rc.left));
		ClientToScreen(hwnd, reinterpret_cast<POINT*>(&rc.right));
		posAndSize.x = rc.left;
		posAndSize.y = rc.top;
		posAndSize.w = rc.right - rc.left;
		posAndSize.h = rc.bottom - rc.top;
		return true;
	}
	return false;
}

bool resizeForOculus(JSContext *cx, int w, int h, int x, int y) {	
	omsContext* pContext = GPSM(cx)->GetContext();
	if(pContext->mpApp)
		pContext->mpApp->ResizeWindowForOculus(w, h, x, y);
	else
		return false;

	jsval vPlayer = JSVAL_NULL;
	JS_GetProperty(cx, JS_GetGlobalObject(cx), GJSONM_PLAYER, &vPlayer);
	if(vPlayer == JSVAL_NULL)
		return false;

	JSObject* jsoPlayer = JSVAL_TO_OBJECT(vPlayer);

	SetWindowSubObject(cx, jsoPlayer);
	GPSM(cx)->NotifyPlayerOnWindowSizeChanged(w, h);
	GPSM(cx)->DontUpdateOneTime();
	GPSM(cx)->DontUpdateOneTime();
	pContext->mpRM->GetExtendedRM()->UpdateAndRender();
	return true;
}

JSBool JSFUNC_initOculusRift( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if (argc != 1)
	{
		JS_ReportError(cx,"initOculusRift-method must get only one parameter");
		return JS_TRUE;
	}

	if(!JSVAL_IS_BOOLEAN(argv[0]))
	{
		JS_ReportError(cx,"initOculusRift-method first parameter must be an boolean");
		return JS_TRUE;
	}

	bool isNeedToEnable = JSVAL_TO_BOOLEAN(argv[0]);

	int initErr = 1;
	mlSceneManager* pSM = GPSM(cx);
	if (pSM->GetContext() && pSM->GetContext()->mpMapMan)
	{
		initErr = pSM->GetContext()->mpMapMan->InitOculusRift(isNeedToEnable);
		if (isNeedToEnable) {
			if (initErr == 0) {
				saveWindowPosAndSize(cx, prevPosAndSize);

				int w, h, x, y;
 				pSM->GetContext()->mpMapMan->GetOculusRiftDesktopParams(w, h, x, y);
				if (!resizeForOculus(cx, w, h, x, y)) {
					JS_ReportError(cx,"failed to resize for oculus");
					return JS_FALSE;
				}
			}
		} else {
			if (!resizeForOculus(cx, prevPosAndSize.w, prevPosAndSize.h, prevPosAndSize.x, prevPosAndSize.y)) {
				JS_ReportError(cx,"failed to resize for oculus");
				return JS_FALSE;
			}
		}
	}
	
	*rval = INT_TO_JSVAL(initErr);
		
	return JS_TRUE;
}

JSBool JSFUNC_initKinect( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if (argc != 1)
	{
		JS_ReportError(cx,"initKinect-method must get only one parameter");
		return JS_TRUE;
	}

	if(!JSVAL_IS_BOOLEAN(argv[0]))
	{
		JS_ReportError(cx,"initKinect-method first parameter must be an boolean");
		return JS_TRUE;
	}

	bool isNeedToEnable = JSVAL_TO_BOOLEAN(argv[0]);

	int initErr = 1;
	mlSceneManager* pSM = GPSM(cx);
	if (pSM->GetContext() && pSM->GetContext()->mpMapMan)
	{
		initErr = pSM->GetContext()->mpMapMan->InitKinect(isNeedToEnable);
	}
	
	*rval = INT_TO_JSVAL(initErr);
		
	return JS_TRUE;
}

JSBool JSFUNC_initGamepad( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if (argc != 1)
	{
		JS_ReportError(cx,"initGamepad-method must get only one parameter");
		return JS_TRUE;
	}

	if(!JSVAL_IS_BOOLEAN(argv[0]))
	{
		JS_ReportError(cx,"initGamepad-method first parameter must be an boolean");
		return JS_TRUE;
	}

	bool isNeedToEnable = JSVAL_TO_BOOLEAN(argv[0]);

	int initErr = 1;
	mlSceneManager* pSM = GPSM(cx);
	if (pSM->GetContext() && pSM->GetContext()->mpMapMan)
	{
		initErr = pSM->GetContext()->mpMapMan->InitGamepad(isNeedToEnable);
	}
	
	*rval = INT_TO_JSVAL(initErr);
		
	return JS_TRUE;
}

JSBool JSFUNC_sendConnectionRestoredStatus( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	mlSceneManager* pSM = GPSM(cx);

	if( pSM->GetContext() && pSM->GetContext()->mpServiceWorldMan)
		pSM->GetContext()->mpServiceWorldMan->SendConnectionRestoredStatus();

	return JS_TRUE;
}



JSBool JSFUNC_getResourceConsumingApplications( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject* jso=JS_NewObject(cx,NULL,NULL,NULL);
	jsval val;

	std::vector<SResourceConsumingApplication> apps = GPSM(cx)->GetContext()->mpRM->GetResourceConsumingApplications();

	std::vector<SResourceConsumingApplication>::iterator it = apps.begin();
	std::vector<SResourceConsumingApplication>::iterator itEnd = apps.end();

	for ( ; it != itEnd; it++)
	if ((*it).name != "player.exe")
	{
		JSObject* jsoObj = JS_NewObject(cx,NULL,NULL,NULL);
		jsval vX = INT_TO_JSVAL((int)(*it).cpuPercent);
		JS_SetProperty(cx, jsoObj, "cpu", &vX);
		jsdouble* jsdbl=JS_NewDouble(cx, (*it).memorySize); 
		jsval vY = DOUBLE_TO_JSVAL(jsdbl);
		JS_SetProperty(cx, jsoObj, "memory", &vY);		
		val = OBJECT_TO_JSVAL( jsoObj);
	
		wr_JS_SetUCProperty(cx,jso, cLPWCSTR((*it).name.c_str()),(*it).name.size(), &val);
	}
	
	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

}


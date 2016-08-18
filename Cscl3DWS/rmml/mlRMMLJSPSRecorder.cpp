#include "mlRMML.h"
#include "config/oms_config.h"
#include "SyncMan.h"
#include "mlLogRecord.h"
#include "mlLogSeance.h"
#include "IDesktopMan.h"
#include "config/prolog.h"
#include <time.h>
#include "../sm/sm.h"
#include "mlRecorderEvents.h"

namespace rmml {

/**
 * Реализация глобального JS-объекта "Player.server.recorder"
 */

#define JSSRC_MASK 0x40

#define JSSRCONST(NM,VL) \
	{ #NM, VL | JSSRC_MASK, JSPROP_ENUMERATE | JSPROP_READONLY, 0, 0},

JSBool JSSRecorderGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool JSSRecorderSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){ return JS_TRUE; }
	
JSClass JSRMMLSRecorderClass = {
	"_SRecorder", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, 
	JSSRecorderGetProperty, JSSRecorderSetProperty,
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL,
	NULL, NULL, NULL
};

JSPropertySpec _JSSRecorderPropertySpec[] = { 
	JSPROP_RO(recordingStatus)	// статус записи (0 - не идет, 1 - отправлен запрос на начало записи, 2 - идет запись)
	JSSRCONST(SR_CONTINUE_MODE, 0)
	JSSRCONST(SR_STOP_MODE, 1)
	JSSRCONST(SR_DELETE_MODE, 2)
	{ 0, 0, 0, 0, 0 }
};

char GetRecordEventID(const wchar_t* apwcEventName){
	if(isEqual(apwcEventName,L"onSynchronizedObjectsLoaded"))
		return (char)EVID_onSynchronizedObjectsLoaded;
	if(isEqual(apwcEventName,L"onDesktopImageFileSaved"))
		return (char)EVID_onDesktopImageFileSaved;
	if(isEqual(apwcEventName,L"onRecordInLocationFailed"))
		return (char)EVID_onRecordInLocationFailed;
	
	if(isEqual(apwcEventName,L"onRecordConnected"))
		return (char)EVID_onRecordConnected;
	if(isEqual(apwcEventName,L"onRecordDisconnected"))
		return (char)EVID_onRecordDisconnected;
	if(isEqual(apwcEventName,L"onRecordStoped"))
		return (char)EVID_onRecordStoped;
	
	if(isEqual(apwcEventName,L"onUpdatePlayingTime"))
		return (char)EVID_onUpdatePlayingTime;
	if(isEqual(apwcEventName,L"onSeanceSeekPos"))
		return (char)EVID_onSeanceSeekPos;
	if(isEqual(apwcEventName,L"onSeancePaused"))
		return (char)EVID_onSeancePaused;
	if(isEqual(apwcEventName,L"onSeancePlayed"))
		return (char)EVID_onSeancePlayed;
	if(isEqual(apwcEventName,L"onSeanceRewinded"))
		return (char)EVID_onSeanceRewinded;
	if(isEqual(apwcEventName,L"onSeanceFinishedPlaying"))
		return (char)EVID_onSeanceFinishedPlaying;
	if(isEqual(apwcEventName,L"onRecordInLocationNoneSound"))
		return (char)EVID_onRecordInLocationNoneSound;
	if(isEqual(apwcEventName,L"onSeanceLoaded"))
		return (char)EVID_onSeanceLoaded;
	if(isEqual(apwcEventName,L"onLessonLoaded"))
		return (char)EVID_onLessonLoaded;
	if(isEqual(apwcEventName,L"onSeanceLeaved"))
		return (char)EVID_onSeanceLeaved;
	if(isEqual(apwcEventName,L"onSharingSessionStarted"))
		return (char)EVID_onSharingSessionStarted;
	if (isEqual(apwcEventName,L"onSharingFileSaved"))
		return (char)EVID_onSharingFileSaved;
	if(isEqual(apwcEventName,L"onSharingViewSessionStarted"))
		return (char)EVID_onSharingViewSessionStarted;
	if(isEqual(apwcEventName,L"onSharingPreviewCancel"))	
		return (char)EVID_onSharingPreviewCancel;
	if(isEqual(apwcEventName,L"onSharingPreviewOK"))	
		return (char)EVID_onSharingPreviewOK;
	if(isEqual(apwcEventName,L"onTextureSizeChanged"))
		return (char)EVID_onTextureSizeChanged;
	if(isEqual(apwcEventName,L"onSessionClosed"))
		return (char)EVID_onSessionClosed;
	if (isEqual(apwcEventName,L"onViewSessionClosed"))
		return (char)EVID_onViewSessionClosed;
	if (isEqual(apwcEventName,L"onRemoteAccessModeChanged"))
		return (char)EVID_onRemoteAccessModeChanged;
	if (isEqual(apwcEventName,L"onWebCameraState"))
		return (char)EVID_onWebCameraState;
	if (isEqual(apwcEventName,L"onAvatarImageFileSaved"))
		return (char)EVID_onAvatarImageFileSaved;
	if(isEqual(apwcEventName,L"onEventEntered"))
		return (char)EVID_onEventEntered;
	if(isEqual(apwcEventName,L"onLocationEntered"))
		return (char)EVID_onLocationEntered;
	if(isEqual(apwcEventName,L"onEventUpdateInfo"))
		return (char)EVID_onEventUpdateInfo;
	if(isEqual(apwcEventName,L"onEventLeaved"))
		return (char)EVID_onEventLeaved;	
	if(isEqual(apwcEventName,L"onEventUserRightsInfo"))
		return (char)EVID_onEventUserRightsInfo;
	/*if(isEqual(apwcEventName,L"onEventUserRolesUpdate"))
		return (char)EVID_onEventUserRolesUpdate;*/
	if(isEqual(apwcEventName,L"onEventUserRightUpdate"))
		return (char)EVID_onEventUserRightUpdate;
	if(isEqual(apwcEventName,L"onEventUserBanUpdate"))
		return (char)EVID_onEventUserBanUpdate;
	if(isEqual(apwcEventName,L"onSharingEventByCode"))
		return (char)EVID_onSharingEventByCode;
	if(isEqual(apwcEventName,L"onSharingEventSetBarPosition"))
		return (char)EVID_onSharingEventSetBarPosition;
	if(isEqual(apwcEventName,L"onSharingEventVideoEnd"))
		return (char)EVID_onSharingEventVideoEnd;
	if(isEqual(apwcEventName,L"onGlobalMouseEvent"))
		return (char)EVID_onGlobalMouseEvent;
	
	return -1;
}

wchar_t* GetRecordEventName(char aidEvent){
	switch(aidEvent){
	case (char)EVID_onSynchronizedObjectsLoaded:
		return L"onSynchronizedObjectsLoaded";
	case (char)EVID_onDesktopImageFileSaved:
		return L"onDesktopImageFileSaved";
	case (char)EVID_onRecordInLocationFailed:
		return L"onRecordInLocationFailed";
	
	case (char)EVID_onRecordConnected:
		return L"onRecordConnected";
	case (char)EVID_onRecordDisconnected:
		return L"onRecordDisconnected";
	case (char)EVID_onRecordStoped:
		return L"onRecordStoped";
	
	case (char)EVID_onUpdatePlayingTime:
		return L"onUpdatePlayingTime";
	case (char)EVID_onSeanceSeekPos:
		return L"onSeanceSeekPos";
	case (char)EVID_onSeancePaused:
		return L"onSeancePaused";
	case (char)EVID_onSeancePlayed:
		return L"onSeancePlayed";
	case (char)EVID_onSeanceRewinded:
		return L"onSeanceRewinded";
	case (char)EVID_onSeanceFinishedPlaying:
		return L"onSeanceFinishedPlaying";
	case (char)EVID_onRecordInLocationNoneSound:
		return L"onRecordInLocationNoneSound";
	case (char)EVID_onSeanceLoaded:
		return L"onSeanceLoaded";
	case (char)EVID_onLessonLoaded:
		return L"onLessonLoaded";
	case (char)EVID_onSeanceLeaved:
		return L"onSeanceLeaved";
	case (char)EVID_onSharingSessionStarted:
		return L"onSharingSessionStarted";
	case (char)EVID_onSharingFileSaved:
		return L"onSharingFileSaved";
	case (char)EVID_onSharingViewSessionStarted:
		return L"onSharingViewSessionStarted";
	case (char)EVID_onSharingPreviewCancel:
		return L"onSharingPreviewCancel";
	case (char)EVID_onSharingPreviewOK:
		return L"onSharingPreviewOK";
	case (char)EVID_onSessionClosed:
		return L"onSessionClosed";
	case (char)EVID_onTextureSizeChanged:
		return L"onTextureSizeChanged";
	case (char)EVID_onViewSessionClosed:
		return L"onViewSessionClosed";
	case (char)EVID_onRemoteAccessModeChanged:
		return L"onRemoteAccessModeChanged";
	case (char)EVID_onWebCameraState:
		return L"onWebCameraState";
	case (char)EVID_onAvatarImageFileSaved:
		return L"onAvatarImageFileSaved";
	case (char)EVID_onEventEntered:
		return L"onEventEntered";
	case (char)EVID_onLocationEntered:
		return L"onLocationEntered";
	case (char)EVID_onEventUpdateInfo:
		return L"onEventUpdateInfo";
	case (char)EVID_onEventLeaved:
		return L"onEventLeaved";	
	case (char)EVID_onEventUserRightsInfo:
		return L"onEventUserRightsInfo";
	/*case (char)EVID_onEventUserRolesUpdate:
		return L"onEventUserRolesUpdate";*/
	case (char)EVID_onEventUserRightUpdate:
		return L"onEventUserRightUpdate";
	case (char)EVID_onEventUserBanUpdate:
		return L"onEventUserBanUpdate";
	case (char)EVID_onSharingEventByCode:
		return L"onSharingEventByCode";
	case (char)EVID_onSharingEventSetBarPosition:
		return L"onSharingEventSetBarPosition";
	case (char)EVID_onSharingEventVideoEnd:
		return L"onSharingEventVideoEnd";
	case (char)EVID_onGlobalMouseEvent:
		return L"onGlobalMouseEvent";
		
	}
	return L"???";
}

JSFUNC_DECL(createSeance)
JSFUNC_DECL(stopSeance)
JSFUNC_DECL(startRecording)
JSFUNC_DECL(stopRecording)
JSFUNC_DECL(updateRecordInfo)
JSFUNC_DECL(setPreviewDefaultImage)
JSFUNC_DECL(getRecords)
JSFUNC_DECL(getSeances)
JSFUNC_DECL(deleteRecord)
JSFUNC_DECL(deleteRecordIntoRecycleBin)
JSFUNC_DECL(deleteRecords)
JSFUNC_DECL(play)
JSFUNC_DECL(stop)
JSFUNC_DECL(pause) 
JSFUNC_DECL(rewind)
JSFUNC_DECL(seek)
JSFUNC_DECL(addPreviewFrame)
JSFUNC_DECL(addStatisticFrame)
JSFUNC_DECL(addPreviewEvent)
JSFUNC_DECL(addChatMessage)
JSFUNC_DECL(addPreviewStatistic)
JSFUNC_DECL(getPreviewHost)
JSFUNC_DECL(setScreenCaptureObject)
JSFUNC_DECL(startSharing)
JSFUNC_DECL(startSharingView)
JSFUNC_DECL(startSharingVideoFile)
JSFUNC_DECL(stopSharing)
JSFUNC_DECL(startSharingPlay)
JSFUNC_DECL(showSharingRect)
JSFUNC_DECL(setSharingViewFps)
JSFUNC_DECL(sharingSeek)
JSFUNC_DECL(sharingPlay)
JSFUNC_DECL(sharingPause)
JSFUNC_DECL(getSharingDuration)
JSFUNC_DECL(getOnlyKeyFrame)
JSFUNC_DECL(getAllFrames)
JSFUNC_DECL(updateFileNameInSession)
JSFUNC_DECL(traceAllSession)
JSFUNC_DECL(setRemoteAccessMode)
JSFUNC_DECL(stopWaitWebCameraResponce)
JSFUNC_DECL(addAvatarImage)
JSFUNC_DECL(pauseSharing)
JSFUNC_DECL(resumeSharing)
JSFUNC_DECL(setCursorMode)
JSFUNC_DECL(setPilotLogin)
JSFUNC_DECL(emulateSharingRect)
JSFUNC_DECL(setEventUserRights)
JSFUNC_DECL(setEventUserDefaultRights)
JSFUNC_DECL(setEventUserBan)
JSFUNC_DECL(getEventUserRight)
JSFUNC_DECL(boardOwnerChanged)
JSFUNC_DECL(fullControlChanged)

//// JavaScript Function Table
JSFunctionSpec _JSSRecorderFunctionSpec[] = {
	JSFUNC(createSeance, 4)		// Создать на сервере сеанс с соответствующими характеристиками
	JSFUNC(startRecording, 0) // начать запись 
	JSFUNC(stopRecording, 0) // остановить запись (возвращает объект типа Record)
	JSFUNC(updateRecordInfo, 1) // изменить на сервере информацию о записи 
	JSFUNC(setPreviewDefaultImage, 2) // изменить на сервере информацию о картинке превью по умолчанию для записи	
	JSFUNC(getRecords, 3) // получить набор записей в виде объекта Records (асинхронный механизм)
	JSFUNC(getSeances,1) // получить список сеансов воспроизведения для записи лога
	JSFUNC(play, 1) // начать воспроизведение записи
	JSFUNC(deleteRecord, 1) // удалить запись
	JSFUNC(deleteRecordIntoRecycleBin, 1) // удалить запись в козрину
	JSFUNC(deleteRecords, 1) // удалить записи
	JSFUNC(stop, 0) // остановить воспроизведение записи
	JSFUNC(pause, 0) // приостановить воспроизведение записи
	JSFUNC(rewind, 0) // перемотать запись на начало
	JSFUNC(seek, 1) // перемотать запись на определенный момент
	JSFUNC(addPreviewFrame, 4) // добавить кадр превью
	JSFUNC(addStatisticFrame, 4) // добавить кадр статистики
	JSFUNC(addPreviewEvent, 2) // добавить  событие для превью
	JSFUNC(addPreviewStatistic, 1) // добавить  событие для превью
	JSFUNC(addChatMessage, 3) // добавить сообщение приватного чата
	JSFUNC(getPreviewHost, 0) // получить адрес PreviewServer-а
	JSFUNC(setScreenCaptureObject, 4) // начать screen capture для заданного объекта
	JSFUNC(startSharing, 3) // начать sharing сессию на экране
	JSFUNC(startSharingView, 3) // начать view sharing сессию на экране
	JSFUNC(startSharingVideoFile, 4) // начать view sharing сессию на экране	
	JSFUNC(stopSharing, 1) // закончить view sharing сессию на экране
	JSFUNC(startSharingPlay, 6) //начать проигрывание видео 
	JSFUNC(showSharingRect, 2) //показать/cкпыть  awareness window
	JSFUNC(setSharingViewFps, 3) // задать view sharing сессии fps
	JSFUNC(sharingSeek, 2) 
	JSFUNC(sharingPlay, 1) 
	JSFUNC(sharingPause, 1) 
	JSFUNC(getSharingDuration, 1)
	JSFUNC(getOnlyKeyFrame, 1) //получать только ключевые кадры
	JSFUNC(getAllFrames, 1)  //получать все кадры
	JSFUNC(updateFileNameInSession, 2) // обновление имени файла в сессии
	JSFUNC(traceAllSession, 0)
	JSFUNC(stopWaitWebCameraResponce, 1) //ожидание отклика веб-камеры завершено
	JSFUNC(setRemoteAccessMode, 2) // установка режима работы с экраном
	JSFUNC(addAvatarImage, 8) //сохраняет указанную картинку в временный файл
	JSFUNC(pauseSharing, 1) // приостановить проигрывание шаринга
	JSFUNC(resumeSharing, 1) // возобновить проигрывание шаринга
	JSFUNC(setCursorMode, 2) // установить режим отображения курсора мыши в шаринге
	JSFUNC(setPilotLogin, 1) // установить логин человека, управляющего воспроизведением записи
	JSFUNC(emulateSharingRect, 2) // эмулировать sharing сессию на экране
	JSFUNC(setEventUserRights, 1) // установить права пользователей в событии
	JSFUNC(setEventUserDefaultRights, 1) // установить права по умолчанию пользователей в событии
	JSFUNC(setEventUserBan, 2) // забанить или разбанить пользователя в событии	
	JSFUNC(getEventUserRight, 0) // запросить свои права
	JSFUNC(boardOwnerChanged, 3) //сменился owner доски
	JSFUNC(fullControlChanged, 3)
	{ 0, 0, 0, 0, 0 }
};

int getObjectProperty(JSContext *cx, jsval vArg , char* sFunctionName)
{
	int val=0;
	jsval v = JSVAL_NULL;
	bool b = JS_CallFunctionName(cx, JSVAL_TO_OBJECT(vArg), sFunctionName, 0, NULL, &v);
	unsigned long n = 0;
	if (b && JSVAL_IS_INT(v))
		val = JSVAL_TO_INT(v);
	return val;
}


// Создание rmml-го объекта сеанса
unsigned int CreateLogSeanceObject(JSContext *cx, mlSceneManager* pSM,  syncSeanceInfo seanceInfo)
{
	JSObject* jsoLogSeance = mlLogSeanceJSO::newJSObject(cx);
	JSObject* jsoSRecorder = pSM->GetSRecorderGO();
	mlLogSeanceJSO* pSeance = (mlLogSeanceJSO*)JS_GetPrivate(cx, jsoLogSeance);
	mlLogSeanceInfo aLogSeanceInfo(seanceInfo);
	pSeance->Init(aLogSeanceInfo);
	SAVE_FROM_GC(cx, jsoSRecorder, jsoLogSeance)
	return (unsigned int)jsoLogSeance;
}

// Функции, относящиеся к созданию сеанса 
// 1 вариант
//		1. Запись лога, для которой должен быть создан сеанс
//		2. Название сеанса
//		3. Дата проигрывания сеанса
//		4. Список участников

// 2 вариант
//		1. URL
JSBool JSFUNC_createSeance(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;

	/*
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;
	}
	
	// Недостаточно аргументов
	if(argc == 0){
		JS_ReportError(cx, "CreateSeance-method must get one argument at least");
		return JS_TRUE;
	}

	sync::syncSeanceInfo seanceInfo;
	unsigned int logSeanceRmmlObjectId;

	// Только 1 аргумент 
	if(argc == 1){
		jsval vArg = argv[0];
		// Это URL
		if(!JSVAL_IS_STRING(vArg)){
			JS_ReportWarning(cx, "First argument of createSeance must be a URL or LogRecord");
			return JS_TRUE;
		}
		seanceInfo.mpwcURL = JS_GetStringChars(JSVAL_TO_STRING(vArg));			
		// Создание rmml-го объекта сеанса
		logSeanceRmmlObjectId = CreateLogSeanceObject(cx,pSM,seanceInfo);
		omsresult omsres = pSM->GetContext()->mpSyncMan->createSeanceByURL(seanceInfo, logSeanceRmmlObjectId);
	}

	// Как минимум 3 аргумента
	if (argc>=3)
	{
		mlLogRecordJSO* pRecord = NULL;

		// Запись лога
		jsval vObj = argv[0];
		if(!JSVAL_IS_REAL_OBJECT(vObj) || !mlLogRecordJSO::IsInstance(cx, JSVAL_TO_OBJECT(vObj))){
			JS_ReportError(cx, "createSeance-method must get a Record-object");
			return JS_TRUE;
		}
		JSObject* jsoRecord = JSVAL_TO_OBJECT(vObj);
		pRecord = (mlLogRecordJSO*)JS_GetPrivate(cx, jsoRecord);

		if (pRecord==NULL){	
			JS_ReportWarning(cx, "LogRecord object is not found");
			return JS_TRUE;
		}

		// Name
		jsval vArg = argv[1];
		if(!JSVAL_IS_STRING(vArg)){
			JS_ReportWarning(cx, "Second argument of createSeance must be a string - name of the Seance");
			return JS_TRUE;
		}
		seanceInfo.mpwcName = JS_GetStringChars(JSVAL_TO_STRING(vArg));

		// DateTime
		vArg = argv[2];
		if(!JSVAL_IS_REAL_OBJECT(vArg)){
			JS_ReportWarning(cx, "This argument of createSeance must be a string - date of the Seance");
			return JS_TRUE;
		}
		// перевод времени 
		struct tm userTime;
		userTime.tm_year = getObjectProperty(cx,vArg,"getYear");
		userTime.tm_mon = getObjectProperty(cx,vArg,"getMonth");
		userTime.tm_mday = getObjectProperty(cx,vArg,"getDate");
		userTime.tm_hour = getObjectProperty(cx,vArg,"getHours");
		userTime.tm_min =  getObjectProperty(cx,vArg,"getMinutes");
		userTime.tm_sec =  getObjectProperty(cx,vArg,"getSeconds");
		time_t userTimeUTC = _mktime64(&userTime);
		seanceInfo.mlDate = userTimeUTC;

		// Members
		if(argc > 3){
			vArg = argv[3];
			if(!JSVAL_IS_STRING(vArg)){
				JS_ReportWarning(cx, "Forth argument of createSeance must be a string - members of the Seance");
				return JS_TRUE;
			}
			seanceInfo.mpwcMembers = JS_GetStringChars(JSVAL_TO_STRING(vArg));
		}

		logSeanceRmmlObjectId = CreateLogSeanceObject(cx,pSM,seanceInfo);
	}
	*rval = OBJECT_TO_JSVAL((JSObject*)logSeanceRmmlObjectId);
	*/
	return JS_TRUE; 
}


void CreateSRecorderJSObject(JSContext *cx, JSObject *ajsoPlServer){
    JSObject* jsoSRecorder = JS_DefineObject(cx, ajsoPlServer, GJSONM_SRECORDER, &JSRMMLSRecorderClass, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
	JSBool bR;
	bR=JS_DefineProperties(cx, jsoSRecorder, _JSSRecorderPropertySpec);
	bR=JS_DefineFunctions(cx, jsoSRecorder, _JSSRecorderFunctionSpec);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, ajsoPlServer);
	JS_SetPrivate(cx, jsoSRecorder, pSM);
	pSM->mjsoSRecorder = jsoSRecorder;
	JSObject *jso = mlLogSeanceJSO::newJSObject(cx); 
	pSM->pSeance = (mlLogSeanceJSO *)JS_GetPrivate(cx,jso);
	JS_DefineProperty(cx,jsoSRecorder,"currentSeance",OBJECT_TO_JSVAL(jso),0,0,JSPROP_READONLY);

	JSObject *jsoRecord = mlLogRecordJSO::newJSObject(cx); 
	pSM->pRecord = (mlLogRecordJSO *)JS_GetPrivate(cx,jsoRecord);
	JS_DefineProperty(cx,jsoSRecorder,"currentRecord",OBJECT_TO_JSVAL(jsoRecord),0,0,JSPROP_READONLY);
	//
}


JSBool JSSRecorderGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){ 
	if(JSVAL_IS_INT(id)){
		mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
		cm::cmICommunicationManager* pComMan = pSM->GetContext()->mpComMan;
		if(pComMan == NULL){
			JS_ReportError(cx, "Communication manager is not set");
			return JS_TRUE;
		}
		int iID=JSVAL_TO_INT(id);
		if(iID & JSSRC_MASK){
			*vp = INT_TO_JSVAL(iID & ~JSSRC_MASK);
			return JS_TRUE;
		}
		switch(iID){
		case JSPROP_recordingStatus:{
			int iStatus = 0;
			// ??
			*vp = INT_TO_JSVAL(iStatus);
			}break;
		}

		// уведомляем слушателей
		char chEvID=iID-1;
//		pSM->NotifyModuleListeners(chEvID);
	}
	return JS_TRUE; 
}

JSBool JSFUNC_startRecording(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;
	}
	pSM->SetSyncCallbacks();
	
	
	if(argc < 2){
		JS_ReportError(cx, "startRecording must get two arguments");
		return JS_TRUE;
	}
	
	jsval vObj = argv[0];
	if(!JSVAL_IS_INT(vObj) ){
		JS_ReportError(cx, "Аrgument of StartRecording must be constant: NEW = 0, APPEND = 1 or OVERWRITE = 2");
		return JS_TRUE;
	}

	int recordMode = JSVAL_TO_INT(vObj);
	if (recordMode>2 || recordMode<0){
		JS_ReportError(cx, "Аrgument of StartRecording must be constant: NEW = 0, APPEND = 1 or OVERWRITE = 2");
		return JS_TRUE;
	}

	// target
	if( !JSVAL_IS_STRING( argv[1]))
	{
		JS_ReportError(cx,"Second parameter of StartRecording(..) must be a string (record name)");
		return JS_FALSE;
	}

	mlString sRecordName;
	jsval vArg = argv[1];
	if(JSVAL_IS_STRING(vArg))
		sRecordName = (const wchar_t*)JS_GetStringChars(JSVAL_TO_STRING(vArg));

	bool bRetVal = false;
	int iErrorCode = 0;

	omsresult omsres = pSM->GetContext()->mpSyncMan->StartRecord(recordMode, sRecordName.c_str(), iErrorCode);

	/*if(pSM->GetContext()->mpVoipMan != NULL)
		pSM->GetContext()->mpVoipMan->OnBeforeStartRecording();*/

	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));
	if(iErrorCode > 0)
		TraceServerError(cx, iErrorCode);
	return JS_TRUE; 
}

JSBool JSFUNC_stopRecording(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;
	}
	int iErrorCode = 0;
	
	if(argc == 0){
		JS_ReportError(cx, "stopRecording must get one argument at least");
		return JS_TRUE;
	}
	jsval vObj = argv[0];
	if(!JSVAL_IS_INT(vObj) ){
		JS_ReportError(cx, "Аrgument of stopRecording must be constant: CONTINUE = 0, STOP = 1,  DELETE = 2 ");
		return JS_TRUE;
	}

	JSObject* jsoLogRecord = mlLogRecordJSO::newJSObject(cx);
	unsigned int iID = 0;
	syncRecordInfo* aRecordInfo; 
	omsresult omsRes = pSM->GetContext()->mpSyncMan->StopRecord(iID, iErrorCode, &aRecordInfo);
	if(iErrorCode > 0 || iID == 0){
		jsval vErrorCode = INT_TO_JSVAL(iErrorCode);
		JS_SetProperty(cx, jsoLogRecord, "errorCode", &vErrorCode);
	}

	if( pSM->GetContext()->mpPreviewRecorder)
		pSM->GetContext()->mpPreviewRecorder->StopRecording(iID);

	mlLogRecordJSO* pLogRecord = (mlLogRecordJSO*)JS_GetPrivate(cx, jsoLogRecord);
	mlLogRecordInfo aLogRecordInfo(aRecordInfo);
	pLogRecord->Init(aLogRecordInfo);
	*rval = OBJECT_TO_JSVAL(jsoLogRecord);
	return JS_TRUE; 
}

// bool updateRecordInfo(LogRecord rec)
JSBool JSFUNC_updateRecordInfo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;
	}
	if(argc == 0){
		JS_ReportError(cx, "updateRecordInfo must get one argument at least");
		return JS_TRUE;
	}
	jsval vObj = argv[0];
	if(!JSVAL_IS_REAL_OBJECT(vObj) || !mlLogRecordJSO::IsInstance(cx, JSVAL_TO_OBJECT(vObj))){
		JS_ReportError(cx, "updateRecordInfo must get a Record-object");
		return JS_TRUE;
	}
	JSObject* jsoRecord = JSVAL_TO_OBJECT(vObj);
	mlLogRecordJSO* pRecord = (mlLogRecordJSO*)JS_GetPrivate(cx, jsoRecord);
	syncRecordInfo recordInfo;
	pRecord->FillRecordInfo(recordInfo);
	omsresult omsres = pSM->GetContext()->mpSyncMan->UpdateRecordInfo(recordInfo);
	if( recordInfo.mpwcPreview.compare(L"default.jpg")!=0)
		pSM->GetContext()->mpPreviewRecorder->SetPreviewDefaultImage( recordInfo.muRecordID, (wchar_t*)recordInfo.mpwcPreview.c_str());
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));
	return JS_TRUE; 
}

JSBool JSFUNC_setPreviewDefaultImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;
	}
	if(argc < 2 && argc > 2){
		JS_ReportError(cx, "setPreviewDefaultImage must get two argument at least");
		return JS_TRUE;
	}
	jsval vRecID = argv[0];	
	int recID = 0;
	if(!JSVAL_IS_INT(vRecID)){
		if(!JSVAL_IS_STRING(vRecID))
		{
			JS_ReportError(cx, "setPreviewDefaultImage first parameter must get a int or string");
			return JS_TRUE;
		}
		else
		{
			mlString s = (const wchar_t*)JS_GetStringChars(JSVAL_TO_STRING(vRecID ));
			recID = _wtoi( s.c_str());
		}
	}
	else
		recID = JSVAL_TO_INT(vRecID );
	jsval vPath = argv[1];
	if(!JSVAL_IS_STRING(vPath)){
		JS_ReportError(cx, "setPreviewDefaultImage second parameter must get a string");
		return JS_TRUE;
	}	
	mlString sPath = (const wchar_t*)JS_GetStringChars(JSVAL_TO_STRING( vPath ));
	pSM->GetContext()->mpPreviewRecorder->SetPreviewDefaultImage( recID, (wchar_t*)sPath.c_str());
	*rval = NULL;
	return JS_TRUE; 
}

// Records getRecords(int begin, int count[, string filter[, string orderBy]]);
JSBool JSFUNC_getRecords(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	int iBegin = 0;
	int iCount = -1; // все
	mlString sFilter;
	mlString sOrderBy;

	if(argc > 0){
		jsval vArg = argv[0];
		if(!JSVAL_IS_INT(vArg)){
			JS_ReportWarning(cx, "First argument of getRecords must be an integer");
			return JS_TRUE;
		}
		iBegin = JSVAL_TO_INT(vArg);
		if(iBegin < 0){
			JS_ReportWarning(cx, "First argument of getRecords must be positive");
			return JS_TRUE;
		}
	}
	if(argc > 1){
		jsval vArg = argv[1];
		if(!JSVAL_IS_INT(vArg)){
			JS_ReportWarning(cx, "Second argument of getRecords must be an integer");
			return JS_TRUE;
		}
		iCount = JSVAL_TO_INT(vArg);
	}
	if(argc > 2){
		jsval vArg = argv[2];
		if(!JSVAL_IS_STRING(vArg)){
			JS_ReportWarning(cx, "Third argument of getRecords must be a string");
			return JS_TRUE;
		}
		sFilter = (const wchar_t*)JS_GetStringChars(JSVAL_TO_STRING(vArg));
	}
	if(argc > 3){
		jsval vArg = argv[3];
		if(!JSVAL_IS_STRING(vArg)){
			JS_ReportWarning(cx, "Fourth argument of getRecords must be a string");
			return JS_TRUE;
		}
		sOrderBy = (const wchar_t*)JS_GetStringChars(JSVAL_TO_STRING(vArg));
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;
	}
	pSM->SetSyncCallbacks();

	JSObject* jsoLogRecords = mlLogRecordsJSO::newJSObject(cx);
	JSObject* jsoSRecorder = pSM->GetSRecorderGO();
	SAVE_FROM_GC(cx, jsoSRecorder, jsoLogRecords)
	pSM->GetContext()->mpSyncMan->GetRecords((unsigned int)jsoLogRecords, iBegin, iCount, sFilter.c_str(), sOrderBy.c_str(), pSM->GetSyncCallbackManager());
	*rval = OBJECT_TO_JSVAL(jsoLogRecords);
	return JS_TRUE; 
}

// Records getSeances( );
JSBool JSFUNC_getSeances(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;
	}
	pSM->SetSyncCallbacks();
	if(argc == 0){
		JS_ReportError(cx, "GetSeances-method must get one argument at least");
		return JS_TRUE;
	}
	mlLogRecordJSO* pRecord = NULL;
	if(argc > 0){
		jsval vObj = argv[0];
		if(!JSVAL_IS_REAL_OBJECT(vObj) || !mlLogRecordJSO::IsInstance(cx, JSVAL_TO_OBJECT(vObj))){
			JS_ReportError(cx, "getSeances-method must get a Record-object");
			return JS_TRUE;
		}
		JSObject* jsoRecord = JSVAL_TO_OBJECT(vObj);
		pRecord = (mlLogRecordJSO*)JS_GetPrivate(cx, jsoRecord);
	}

	if (pRecord==NULL)
	{
		JS_ReportWarning(cx, "LogRecord object is not found");
		return JS_TRUE;
	}

	JSObject* jsoLogSeances = mlLogSeancesJSO::newJSObject(cx);
	JSObject* jsoSRecorder = pSM->GetSRecorderGO();
	SAVE_FROM_GC(cx, jsoSRecorder, jsoLogSeances)

	pSM->GetContext()->mpSyncMan->GetSeances(pRecord->GetRecordID(), (unsigned int)jsoLogSeances );

	*rval = OBJECT_TO_JSVAL(jsoLogSeances);
	return JS_TRUE; 
}

JSBool JSFUNC_deleteRecord(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	assert( false);

	JS_ReportWarning(cx, "deleteRecord is obsolete. Please use deleteRecordIntoRecycleBin.");
	return JS_TRUE;

	/**rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;
	}
	if(argc == 0){
		JS_ReportError(cx, "deleteRecord-method must get one argument at least");
		return JS_TRUE;
	}

	jsval vObj = argv[0];
	if(!JSVAL_IS_INT(vObj) ){
		JS_ReportError(cx, "Аrgument of DeleteRecord must be INT (recordID)");
		return JS_TRUE;
	}

	int uiRecordID = JSVAL_TO_INT(vObj);
	omsresult omsres = pSM->GetContext()->mpSyncMan->DeleteRecord(uiRecordID);
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));
	if( pSM->GetContext()->mpVoipMan != NULL)
		pSM->GetContext()->mpVoipMan->DeleteRecord(uiRecordID);
	if( pSM->GetContext()->mpPreviewRecorder != NULL)
		pSM->GetContext()->mpPreviewRecorder->DeleteRecord(uiRecordID);
	if (pSM->GetContext()->mpSharingMan != NULL)
		pSM->GetContext()->mpSharingMan->DeleteRecord(uiRecordID);

	return JS_TRUE;*/

}

JSBool JSFUNC_deleteRecordIntoRecycleBin(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_FALSE;

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;
	}
	if(argc == 0){
		JS_ReportError(cx, "deleteRecordIntoRecycleBin-method must get one argument at least");
		return JS_TRUE;
	}

	jsval vObj = argv[0];
	if(!JSVAL_IS_INT(vObj) ){
		JS_ReportError(cx, "Аrgument of deleteRecordIntoRecycleBin must be INT (recordID)");
		return JS_TRUE;
	}

	int uiRecordID = JSVAL_TO_INT(vObj);
	omsresult omsres = pSM->GetContext()->mpSyncMan->DeleteRecordIntoRecycleBin(uiRecordID);
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));

	if( pSM->GetContext()->mpVoipMan != NULL)
		pSM->GetContext()->mpVoipMan->DeleteRecord(uiRecordID);

	if( pSM->GetContext()->mpPreviewRecorder != NULL)
		pSM->GetContext()->mpPreviewRecorder->DeleteRecord(uiRecordID);

	if (pSM->GetContext()->mpSharingMan != NULL)
		pSM->GetContext()->mpSharingMan->DeleteRecord(uiRecordID);

	return JS_TRUE; 

}


JSBool JSFUNC_deleteRecords(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;
	}
	if(argc == 0){
		JS_ReportError(cx, "deleteRecord-method must get one argument at least");
		return JS_TRUE;
	}
	jsval vObj = argv[0];
	/*if(!JSVAL_IS_REAL_OBJECT(vObj) || !mlLogRecordJSO::IsInstance(cx, JSVAL_TO_OBJECT(vObj))){
		JS_ReportError(cx, "deleteRecord-method must get a Record-object");
		return JS_TRUE;
	}*/
	JSObject *jsObj = JSVAL_TO_OBJECT(vObj);
	JSIdArray* idArr = JS_Enumerate( cx, jsObj);
	vector<unsigned int> recIDs;
	for(int ij = 0; ij < idArr->length; ij++){
		jsval v;
		if(!JS_IdToValue( cx,idArr->vector[ij],&v))
			continue;
		if(!JSVAL_IS_INT(v))
			continue;
		int index = JSVAL_TO_INT(v);
		jsval val;
		JS_GetElement(cx, jsObj, index, &val);
		unsigned int idRec = JSVAL_TO_INT(val);
		recIDs.push_back(idRec);
	}
	omsresult omsres = pSM->GetContext()->mpSyncMan->DeleteRecords( &recIDs[0], recIDs.size());
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));
	/*if( pSM->GetContext()->mpVoipMan != NULL)
		pSM->GetContext()->mpVoipMan->DeleteRecords( &recIDs[0], recIDs.size());
	if( pSM->GetContext()->mpPreviewRecorder != NULL)
		pSM->GetContext()->mpPreviewRecorder->DeleteRecords( &recIDs[0], recIDs.size());*/
	JS_DestroyIdArray( cx, idArr);
	recIDs.clear();
	return JS_TRUE; 

}

JSBool JSFUNC_play(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;
	}
	/*
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;
	pSharingMan->ResumeServerSharing();
*/
	unsigned int auBornRealityID = 1;
	pSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnUnfreezeSeance, (unsigned char*)&auBornRealityID, 4);
	omsresult omsres = pSM->GetContext()->mpSyncMan->Play(pSM->GetCurrentEventID());
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));
	
	return JS_TRUE; 
}

JSBool JSFUNC_stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;
	}
	omsresult omsres = pSM->GetContext()->mpSyncMan->Stop();
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));

	return JS_TRUE; 
}

JSBool JSFUNC_pause(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;
	}
	omsresult omsres = pSM->GetContext()->mpSyncMan->Pause();
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));
/*
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;
	pSharingMan->PauseServerSharing();
*/	
	return JS_TRUE; 
}

JSBool JSFUNC_pauseSharing(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;

	if(argc != 1)
	{
		JS_ReportError(cx,"JSFUNC_pauseSharing-method must get one parameter");
		return JS_FALSE;
	}

	// target
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of pauseSharing(..) must be a string (target image)");
		return JS_FALSE;
	}
	JSString* jssTarget = JSVAL_TO_STRING(argv[0]);

	
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;
	pSharingMan->PauseUserSharing(cLPCSTR(jssTarget));
		
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();
	return JS_TRUE; 
}

JSBool JSFUNC_resumeSharing(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;

	if(argc != 1)
	{
		JS_ReportError(cx,"JSFUNC_resumeSharing-method must get one parameter");
		return JS_FALSE;
	}

	// target
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of resumeSharing(..) must be a string (target image)");
		return JS_FALSE;
	}
	JSString* jssTarget = JSVAL_TO_STRING(argv[0]);

	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;
	pSharingMan->ResumeUserSharing(cLPCSTR(jssTarget));

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();
	return JS_TRUE; 
}

JSBool JSFUNC_rewind(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;\
	}
	unsigned int auBornRealityID = pSM->GetContext()->mpMapMan->GetCurrentRealityID();
	pSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnFreezeSeance, (unsigned char*)&auBornRealityID, 4);
	omsresult omsres = pSM->GetContext()->mpSyncMan->Rewind();
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));

	return JS_TRUE; 
}

JSBool JSFUNC_setPilotLogin(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "setPilotLogin error. Synchronization manager is not set");
		return JS_TRUE;
	}

	if(argc != 1 || !JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx, "setPilotLogin-method must get one string argument (userLogin)");
		return JS_TRUE;
	}

	JSString* jssPilotLogin = JSVAL_TO_STRING(argv[0]);
	wchar_t* pwcPilotLogin = (wchar_t*)JS_GetStringChars(jssPilotLogin);

	omsresult omsres = pSM->GetContext()->mpSyncMan->SetPilotLogin( pwcPilotLogin, pSM->GetCurrentEventID());
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));

	return JS_TRUE;
}

JSBool JSFUNC_seek(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "Synchronization manager is not set");
		return JS_TRUE;
	}
	if(argc == 0){
		JS_ReportError(cx, "seek-method must get one argument at least");
		return JS_TRUE;
	}
	jsval vVal = argv[0];
	if(!JSVAL_IS_INT(vVal)){
		JS_ReportError(cx, "seek-method must get an integer");
		return JS_TRUE;
	}
	int iTime = JSVAL_TO_INT(vVal);

	omsresult omsres = pSM->GetContext()->mpSyncMan->Seek(/*pSM->pSeance->GetSeanceID(),*/ iTime);
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));
	return JS_TRUE; 
}

JSBool JSFUNC_addAvatarImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);

	if(pSM->GetContext()->mpPreviewRecorder == NULL){
		JS_ReportError(cx, "PreviewRecorder is not set");
		return JS_TRUE;
	}
	if (argc != 8){
		JS_ReportError(cx,"addAvatarImage method must get only eight parameter");
		return JS_TRUE;
	}
	mlRMMLImage* pImage=NULL;
	if(argv[0]!=JSVAL_NULL && JSVAL_IS_OBJECT(argv[0]))
	{
		if(  !JSVAL_IS_STRING(argv[1]))
		{
			JS_ReportError(cx,"Parameter 2 of addAvatarImage(..) must be a string (result file name)");
			return JS_FALSE;
		}
		jsval vVal = argv[2];
		if(!JSVAL_IS_INT(vVal)){
			JS_ReportError(cx, "Parameter 3 of addAvatarImage(..) must be an integer");
			return JS_TRUE;
		}
		int left = JSVAL_TO_INT(vVal);
		vVal = argv[3];
		if(!JSVAL_IS_INT(vVal)){
			JS_ReportError(cx, "Parameter 4 of addAvatarImage(..) must be an integer");
			return JS_TRUE;
		}
		int top = JSVAL_TO_INT(vVal);
		vVal = argv[4];
		if(!JSVAL_IS_INT(vVal)){
			JS_ReportError(cx, "Parameter 5 of addAvatarImage(..) must be an integer");
			return JS_TRUE;
		}
		int width = JSVAL_TO_INT(vVal);
		vVal = argv[5];
		if(!JSVAL_IS_INT(vVal)){
			JS_ReportError(cx, "Parameter 6 of addAvatarImage(..) must be an integer");
			return JS_TRUE;
		}
		int height = JSVAL_TO_INT(vVal);
		vVal = argv[6];
		if(!JSVAL_IS_INT(vVal)){
			JS_ReportError(cx, "Parameter 7 of addAvatarImage(..) must be an integer");
			return JS_TRUE;
		}
		int resultWidth = JSVAL_TO_INT(vVal);
		vVal = argv[7];
		if(!JSVAL_IS_INT(vVal)){
			JS_ReportError(cx, "Parameter 8 of addAvatarImage(..) must be an integer");
			return JS_TRUE;
		}
		int resultHeight = JSVAL_TO_INT(vVal);

		JSString* jssText = JSVAL_TO_STRING(argv[1]);

		JSObject* jsoMotion=JSVAL_TO_OBJECT(argv[0]);
		mlRMMLElement* pMtnMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jsoMotion);
		if(pMtnMLEl != NULL && pMtnMLEl->mRMMLType == MLMT_IMAGE)
		{
			pImage=(mlRMMLImage*)pMtnMLEl;
			wchar_t* tmpFileName = pSM->GetContext()->mpPreviewRecorder->AddImage( pImage, cLPCSTR(jssText), left, top, width, height, resultWidth, resultHeight);
			if (tmpFileName != NULL)
				pSM->NotifyAvatarImageFileSaved(tmpFileName, cLPCSTR(jssText));
			return JS_TRUE;
		}
	}
	return JS_FALSE;
}

JSBool JSFUNC_addPreviewFrame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpPreviewRecorder == NULL){
		JS_ReportError(cx, "PreviewRecorder is not set");
		return JS_TRUE;
	}

	if (argc != 3){
		JS_ReportError(cx,"addPreviewFrame method must get only 3 parameter");
		return JS_TRUE;
	}

	if (argv[1]==JSVAL_NULL || !JSVAL_IS_STRING(argv[1]))
	{
		JS_ReportError(cx,"2 parameter of addPreviewFrame method must be a string");
		return JS_TRUE;
	}

	if (argv[2]==JSVAL_NULL || !JSVAL_IS_INT(argv[2]))
	{
		JS_ReportError(cx,"3 parameter of addPreviewFrame method must be a int");
		return JS_TRUE;
	}

	JSString* jssCameraName = JSVAL_TO_STRING(argv[1]);
	wchar_t* jscCameraName = (wchar_t*)JS_GetStringChars(jssCameraName);

	char jsbCountScreenImage = JSVAL_TO_INT(argv[2]);	

	mlRMMLImage* pImage=NULL;
	if(argv[0]!=JSVAL_NULL && JSVAL_IS_OBJECT(argv[0]))
	{
		JSObject* jsoMotion=JSVAL_TO_OBJECT(argv[0]);
		mlRMMLElement* pMtnMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jsoMotion);
		if(pMtnMLEl != NULL && pMtnMLEl->mRMMLType == MLMT_IMAGE)
		{
			pImage=(mlRMMLImage*)pMtnMLEl;
			*rval = (pSM->GetContext()->mpPreviewRecorder->AddFrame(pImage, jscCameraName, jsbCountScreenImage)) ? JSVAL_TRUE : JSVAL_FALSE;
			return JS_TRUE;
		}
	}
	return JS_TRUE;
}

JSBool JSFUNC_addStatisticFrame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpPreviewRecorder == NULL){
		JS_ReportError(cx, "PreviewRecorder is not set");
		return JS_TRUE;
	}

	if (argc != 1){
		JS_ReportError(cx,"addStatisticFrame method must get only one parameters");
		return JS_TRUE;
	}

	mlRMMLImage* pImage=NULL;
	if(argv[0]!=JSVAL_NULL && JSVAL_IS_OBJECT(argv[0]))
	{
		JSObject* jsoMotion=JSVAL_TO_OBJECT(argv[0]);
		mlRMMLElement* pMtnMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jsoMotion);
		if(pMtnMLEl != NULL && pMtnMLEl->mRMMLType == MLMT_IMAGE)
		{
			pImage=(mlRMMLImage*)pMtnMLEl;
			*rval = (pSM->GetContext()->mpPreviewRecorder->AddStatisticFrame(pImage)) ? JSVAL_TRUE : JSVAL_FALSE;
			return JS_TRUE;
		}
	}
	return JS_FALSE;
}

JSBool JSFUNC_addPreviewEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpPreviewRecorder == NULL){
		JS_ReportError(cx, "PreviewRecorder is not set");
		return JS_TRUE;
	}

	if (argc != 2){
		JS_ReportError(cx,"addPreviewEvent method must get only 2 parameter");
		return JS_TRUE;
	}

	if (argv[0]==JSVAL_NULL || !JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx,"1 parameter of addPreviewEvent method must be a string");
		return JS_TRUE;
	}
	if (argv[1]==JSVAL_NULL || !JSVAL_IS_STRING(argv[1]))
	{
		JS_ReportError(cx,"2 parameter of addPreviewEvent method must be a string");
		return JS_TRUE;
	}

	JSString* jssEvent = JSVAL_TO_STRING(argv[0]);
	JSString* jssCameraName = JSVAL_TO_STRING(argv[1]);
	wchar_t* jscCameraName = (wchar_t*)JS_GetStringChars(jssCameraName);
	wchar_t* jscEvent = (wchar_t*)JS_GetStringChars(jssEvent);

	pSM->GetContext()->mpPreviewRecorder->AddEvent( jscEvent, jscCameraName);

	return JS_TRUE;
}

JSBool JSFUNC_addPreviewStatistic(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpPreviewRecorder == NULL){
		JS_ReportError(cx, "PreviewRecorder is not set");
		return JS_TRUE;
	}

	if (argc != 1){
		JS_ReportError(cx,"addPreviewStatistic method must get only 1 parameter");
		return JS_TRUE;
	}

	if (argv[0]==JSVAL_NULL || !JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx,"1 parameter of addPreviewEvent method must be a string");
		return JS_TRUE;
	}

	JSString* jssStatistic = JSVAL_TO_STRING(argv[0]);
	wchar_t* jscStatistic = (wchar_t*)JS_GetStringChars(jssStatistic);

	pSM->GetContext()->mpPreviewRecorder->AddStatistic( jscStatistic);

	return JS_TRUE;
}

JSBool JSFUNC_addChatMessage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpPreviewRecorder == NULL){
		JS_ReportError(cx, "PreviewRecorder is not set");
		return JS_TRUE;
	}
	if( argc != 4)
	{
		JS_ReportError(cx,"addGroupChatMessage-method must get 4 parameters");
		return JS_FALSE;
	}

	if(  !JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx,"Parameter of addGroupChatMessage(..) must be a string (type of chat)");
		return JS_FALSE;
	}

	JSString *jssType = JSVAL_TO_STRING(argv[0]);

	if(  !JSVAL_IS_STRING(argv[1]))
	{
		JS_ReportError(cx,"Parameter of addGroupChatMessage(..) must be a string (name of the user)");
		return JS_FALSE;
	}

	JSString* jssLogin = JSVAL_TO_STRING(argv[1]);

	if(  !JSVAL_IS_STRING(argv[2]))
	{
		JS_ReportError(cx,"Parameter of addGroupChatMessage(..) must be a string (text of chat)");
		return JS_FALSE;
	}

	JSString* jssText = JSVAL_TO_STRING(argv[2]);

	if(  !JSVAL_IS_BOOLEAN(argv[3]))
	{
		JS_ReportError(cx,"Parameter of addGroupChatMessage(..) must be a bool (isCloudedEnabled)");
		return JS_FALSE;
	}
	
	JSBool jIsCloudedEnabled = JSVAL_TO_BOOLEAN(argv[3]);

	jschar* jscType = JS_GetStringChars(jssType);
	jsint jsType = rtl_atoi( (const char*)jscType);
	jschar* jscLogin = JS_GetStringChars(jssLogin);
	jschar* jscText = JS_GetStringChars(jssText);
	bool jsbIsCloudedEnabled = (jIsCloudedEnabled);

	pSM->GetContext()->mpPreviewRecorder->AddChatMessage( jsType, (wchar_t*)jscLogin, (wchar_t*)jscText, jsbIsCloudedEnabled == JS_TRUE ? true : false);
	return JS_TRUE;
}

JSBool JSFUNC_getEventUserRight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "getEventUserRight error. Synchronization manager is not set");
		return JS_TRUE;
	}		

	omsresult omsres = pSM->GetContext()->mpSyncMan->GetEventUserRight( pSM->GetCurrentEventID());
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));

	return JS_TRUE;
}

JSBool JSFUNC_setEventUserRights(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "setEventUserRights error. Synchronization manager is not set");
		return JS_TRUE;
	}

	if(argc != 1 || !JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx, "setEventUserRights-method must get one string argument");
		return JS_TRUE;
	}

	JSString* jssDescr = JSVAL_TO_STRING(argv[0]);
	wchar_t* pwcDescr = (wchar_t*)JS_GetStringChars(jssDescr);

	omsresult omsres = pSM->GetContext()->mpSyncMan->SetEventUserRights( pSM->GetCurrentEventID(), pwcDescr);
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));

	return JS_TRUE;
}


JSBool JSFUNC_setEventUserDefaultRights(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "setEventUserDefaultRights error. Synchronization manager is not set");
		return JS_TRUE;
	}

	if(argc != 1 || !JSVAL_IS_INT(argv[0]))
	{
		JS_ReportError(cx, "setEventUserDefaultRights-method must get one int argument");
		return JS_TRUE;
	}

	unsigned int right = JSVAL_TO_INT(argv[0]);
	omsresult omsres = pSM->GetContext()->mpSyncMan->SetEventUserDefaultRights( pSM->GetCurrentEventID(), right);
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));

	return JS_TRUE;
}

JSBool JSFUNC_setEventUserBan(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	if(pSM->GetContext()->mpSyncMan == NULL){
		JS_ReportError(cx, "setEventUserBan error. Synchronization manager is not set");
		return JS_TRUE;
	}

	if(argc != 1 || !JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx, "setEventUserBan-method must get one string argument");
		return JS_TRUE;
	}

	/*if(argc != 2 )
	{
		JS_ReportError(cx, "setEventUserBan-method must get two argument ");
		return JS_TRUE;
	}

	if(!JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx, "setEventUserBan-method first argument is not string");
		return JS_TRUE;
	}

	if(!JSVAL_IS_BOOLEAN(argv[1]))
	{
		JS_ReportError(cx, "setEventUserBan-method second argument is not bool");
		return JS_TRUE;
	}*/

	/*JSString* jssLogin = JSVAL_TO_STRING( argv[0]);
	wchar_t* pwcLogin = (wchar_t*)JS_GetStringChars(jssLogin);

	bool bBan = JSVAL_TO_BOOLEAN( argv[1]);*/
	JSString* jssDescr = JSVAL_TO_STRING(argv[0]);
	wchar_t* pwcDescr = (wchar_t*)JS_GetStringChars(jssDescr);
	omsresult omsres = pSM->GetContext()->mpSyncMan->SetEventUserBan( pSM->GetCurrentEventID(), pwcDescr);
	*rval = BOOLEAN_TO_JSVAL(OMS_SUCCEEDED(omsres));

	return JS_TRUE;
}

JSBool JSFUNC_getPreviewHost(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_FALSE;

	if(argc != 1 || !JSVAL_IS_NUMBER(argv[0]))
	{
		JS_ReportError(cx, "getPreviewHost-method must get one number argument");
		return JS_TRUE;
	}
	int recordID = JSVAL_TO_INT(argv[0]);

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	wchar_t *lpPreviewAddrerss = NULL;
	pSM->GetContext()->mpPreviewRecorder->GetPreviewCurrentRecordAddress(recordID, lpPreviewAddrerss);
	if(lpPreviewAddrerss != NULL){
		*rval = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)lpPreviewAddrerss ));
	}
	return JS_TRUE;
}

JSBool JSFUNC_setScreenCaptureObject(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopManager* pDesktopMan = GPSM(cx)->GetContext()->mpDesktopMan;
	if( !pDesktopMan)
		return JS_FALSE;	

	if(argc == 0 || argc > 4)
	{
		JS_ReportError(cx,"setScreenCaptureObject-method must get one , two , three or four parameters");
		return JS_FALSE;
	}

	double proportion = 1.33;
	if(  !JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx,"Parameter of setScreenCaptureObject(..) must be a string (name of the user)");
		return JS_FALSE;
	}

	if( ( argc == 2) || (argc == 3) || (argc == 4))
	{
		if( JSVAL_IS_INT(argv[1]))
			proportion=JSVAL_TO_INT(argv[1]);
		else	if( JSVAL_IS_DOUBLE(argv[1]))
			ML_JSVAL_TO_DOUBLE(proportion, argv[1]);
	}


	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();

	JSString* jssObjectTarget=JSVAL_TO_STRING(argv[0]);

	if( jssObjectTarget!=NULL)
	{
		if ( (argc == 3) || (argc == 4))
		{
			int mode = JSVAL_TO_INT(argv[2]);
		
			if (argc == 3)
			{
				pDesktopMan->SetScreenCaptureObjectTarget( cLPCSTR(jssObjectTarget), mode, (float)proportion);
			}
			else
			{
				if(  !JSVAL_IS_STRING(argv[3]))
				{
					JS_ReportError(cx,"4 parameter of setScreenCaptureObject(..) must be a string (file path)");
					return JS_FALSE;
				}

				JSString* jssFileName = JSVAL_TO_STRING(argv[3]);
				pDesktopMan->SetScreenCaptureObjectTarget( cLPCSTR(jssObjectTarget), mode, (float)proportion, wr_JS_GetStringChars(jssFileName));
			}
		}
		else
		{
			pDesktopMan->SetScreenCaptureObjectTarget( cLPCSTR(jssObjectTarget), (float)proportion);
		}
	}

	return JS_TRUE;
}

/************************************************************************/
/* startSharing( 3DObject - for location identifying, Image - as a texture, Number - for mode: rect or window)
/************************************************************************/
JSBool JSFUNC_startSharing(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;	

	if(argc != 3)
	{
		JS_ReportError(cx,"JSFUNC_startSharing-method must get three parameters");
		return JS_FALSE;
	}

	// Object to define location
	mlRMMLElement* pObjMLEl = NULL;
	const char* psDefaultLocation = "WorldLocation";
	const char* psLocationName = NULL;
	if(JSVAL_IS_REAL_OBJECT(argv[0]))
	{
		JSObject* jso=JSVAL_TO_OBJECT(argv[0]);
		pObjMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jso);
		if(pObjMLEl->GetType() != MLMT_OBJECT)
		{
			JS_ReportError(cx,"First parameter of startSharing(..) must be a 3DObject");
			return JS_FALSE;
		}
		psLocationName = GPSM(cx)->GetContext()->mpMapMan->GetObjectLocationName( pObjMLEl);
		if( psLocationName == NULL)
		{
			JS_ReportError(cx,"StartSharing in default world location");
			psLocationName = psDefaultLocation;
		}
	}
	else
	{
		JS_ReportError(cx,"First parameter of startSharing(..) must be a 3DObject");
		return JS_FALSE;
	}

	// Image target
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[1]);
	if( jssImageTarget == NULL)
	{
		JS_ReportError(cx,"Second parameter of startSharing(..) must be a string");
		return JS_FALSE;
	}

	// Sharing mode
	int mode = 0;
	if(  !JSVAL_IS_INT(argv[2]))
	{
		JS_ReportError(cx,"Third parameter of startSharing(..) must be a integer (mode 1-rect, 2-wnd)");
		return JS_FALSE;
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();

	mode = JSVAL_TO_INT( argv[2]);

	if( mode == 1)
		pSharingMan->StartCapturingRectRequest( cLPCSTR(jssImageTarget), psLocationName);
	else if( mode == 2)
		pSharingMan->StartCapturingWndRequest( cLPCSTR(jssImageTarget), psLocationName);
	else if( mode == 3)
		pSharingMan->StartCapturingVideoInputRequest( cLPCSTR(jssImageTarget), psLocationName);
	else if (mode == 4)
		pSharingMan->StartCapturingNotepadWndRequest( cLPCSTR(jssImageTarget), psLocationName);
	else if (mode == 5)
		pSharingMan->StartCapturingInteractiveBoard( cLPCSTR(jssImageTarget), psLocationName);	
	else
	{
		JS_ReportError(cx,"Third parameter of startSharing(..) must be equal to 1 (rect mode) or 2 (window mode) or 3 (video input mode)");
		return JS_FALSE;
	}

	return JS_TRUE;
}

/************************************************************************/
/* startSharingVideoFile( 3DObject - for location identifying, Image - as a texture, Number - for mode: rect or window)
/************************************************************************/
JSBool JSFUNC_startSharingVideoFile( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;	

	if(argc != 3)
	{
		JS_ReportError(cx,"JSFUNC_startSharingVideoFile-method must get three parameters");
		return JS_FALSE;
	}

	// Object to define location
	mlRMMLElement* pObjMLEl = NULL;
	const char* psDefaultLocation = "WorldLocation";
	const char* psLocationName = NULL;
	if(JSVAL_IS_REAL_OBJECT(argv[0]))
	{
		JSObject* jso=JSVAL_TO_OBJECT(argv[0]);
		pObjMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jso);
		if( pObjMLEl->GetType() != MLMT_OBJECT)
		{
			JS_ReportError(cx,"First parameter of startSharingVideoFile(..) must be a 3DObject");
			return JS_FALSE;
		}
		psLocationName = GPSM(cx)->GetContext()->mpMapMan->GetObjectLocationName( pObjMLEl);
		if( psLocationName == NULL)
		{
			JS_ReportError(cx,"StartSharing in default world location");
			psLocationName = psDefaultLocation;
		}
	}
	else
	{
		JS_ReportError(cx,"First parameter of startSharingVideoFile(..) must be a 3DObject");
		return JS_FALSE;
	}

	// Image target
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[1]);
	if( jssImageTarget == NULL)
	{
		JS_ReportError(cx,"Second parameter of startSharingVideoFile(..) must be a string");
		return JS_FALSE;
	}

	JSString* jssFileName = JSVAL_TO_STRING(argv[2]);
	if( jssFileName == NULL)
	{
		JS_ReportError(cx,"Third parameter of startSharing(..) must be a string");
		return JS_FALSE;
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();

	pSharingMan->StartCapturingVideoFileRequest( cLPCSTR(jssImageTarget), psLocationName, (LPCWSTR)JS_GetStringChars(jssFileName));

	return JS_TRUE;
}

JSBool JSFUNC_emulateSharingRect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;	

	if(argc != 2)
	{
		JS_ReportError(cx,"JSFUNC_startSharing-method must get two parameters");
		return JS_FALSE;
	}

	// Object to define location
	mlRMMLElement* pObjMLEl = NULL;
	const char* psDefaultLocation = "WorldLocation";
	const char* psLocationName = NULL;
	if(JSVAL_IS_REAL_OBJECT(argv[0]))
	{
		JSObject* jso=JSVAL_TO_OBJECT(argv[0]);
		pObjMLEl=(mlRMMLElement*)JS_GetPrivate(cx, jso);
		if(pObjMLEl->GetType() != MLMT_OBJECT)
		{
			JS_ReportError(cx,"First parameter of startSharing(..) must be a 3DObject");
			return JS_FALSE;
		}
		psLocationName = GPSM(cx)->GetContext()->mpMapMan->GetObjectLocationName( pObjMLEl);
		if( psLocationName == NULL)
		{
			JS_ReportError(cx,"StartSharing in default world location");
			psLocationName = psDefaultLocation;
		}
	}
	else
	{
		JS_ReportError(cx,"First parameter of startSharing(..) must be a 3DObject");
		return JS_FALSE;
	}

	// Image target
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[1]);
	if( jssImageTarget == NULL)
	{
		JS_ReportError(cx,"Second parameter of startSharing(..) must be a string");
		return JS_FALSE;
	}
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();

	pSharingMan->EmulateRectRequest( cLPCSTR(jssImageTarget), psLocationName);	

	return JS_TRUE;
}

/************************************************************************/
/* startSharingView( string - location identifier, Image - as a texture, Filename - for session)
/************************************************************************/
JSBool JSFUNC_startSharingView(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;	

	if(argc != 3)
	{
		JS_ReportError(cx,"JSFUNC_startSharingView-method must get three parameters");
		return JS_FALSE;
	}

	// Location identifier
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of startSharingView(..) must be a string (location name)");
		return JS_FALSE;
	}
	JSString* jssLocationName = JSVAL_TO_STRING(argv[0]);

	// Image target
	if( !JSVAL_IS_STRING( argv[1]))
	{
		JS_ReportError(cx,"Second parameter of startSharingView(..) must be a string");
		return JS_FALSE;
	}
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[1]);

	// Sharing filename
	if(  !JSVAL_IS_STRING(argv[2]))
	{
		JS_ReportError(cx,"Third parameter of startSharingView(..) must be a string");
		return JS_FALSE;
	}
	JSString* jssFileName = JSVAL_TO_STRING( argv[2]);

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();


	pSharingMan->StartSharingView( cLPCSTR(jssImageTarget), cLPCSTR(jssLocationName), cLPCSTR(jssFileName));

	return JS_TRUE;
}

JSBool JSFUNC_sharingSeek( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
	{
		JS_ReportError(cx,"JSFUNC_sharingSeek pSharingMan == NULL");
		return JS_FALSE;	
	}

	if(argc != 2)
	{
		JS_ReportError(cx,"JSFUNC_sharingSeek-method must get two parameters");
		return JS_FALSE;
	}	

	// Image target
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of sharingSeek(..) must be a string");
		return JS_FALSE;
	}
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[0]);
	
	if( !JSVAL_IS_DOUBLE( argv[1]))
	{
		JS_ReportError(cx,"Second parameter of sharingSeek(..) must be a double");
		return JS_FALSE;
	}
	double second = *JSVAL_TO_DOUBLE(argv[1]);

	pSharingMan->Seek( cLPCSTR(jssImageTarget), second);
	return JS_TRUE;
}

JSBool JSFUNC_sharingPlay( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
	{
		JS_ReportError(cx,"JSFUNC_sharingSeek pSharingMan == NULL");
		return JS_FALSE;	
	}

	if(argc != 1)
	{
		JS_ReportError(cx,"JSFUNC_sharingPlay-method must get one parameter");
		return JS_FALSE;
	}	

	// Image target
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of sharingPlay(..) must be a string");
		return JS_FALSE;
	}
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[0]);

	pSharingMan->Play(cLPCSTR(jssImageTarget));
	return JS_TRUE;
}

JSBool JSFUNC_sharingPause( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
	{
		JS_ReportError(cx,"JSFUNC_sharingPause pSharingMan == NULL");
		return JS_FALSE;	
	}

	if(argc != 1)
	{
		JS_ReportError(cx,"JSFUNC_sharingPause-method must get one parameter");
		return JS_FALSE;
	}	

	// Image target
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of sharingPause(..) must be a string");
		return JS_FALSE;
	}

	JSString* jssImageTarget = JSVAL_TO_STRING(argv[0]);
	pSharingMan->Pause(cLPCSTR(jssImageTarget));
	return JS_TRUE;
}

JSBool JSFUNC_getSharingDuration( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;	

	if(argc != 1)
	{
		JS_ReportError(cx,"JSFUNC_getSharingDuration-method must get one parameter");
		return JS_FALSE;
	}	

	// Image target
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of getSharingDuration(..) must be a string");
		return JS_FALSE;
	}

	JSString* jssImageTarget = JSVAL_TO_STRING(argv[0]);
	double duration = pSharingMan->GetDuration(cLPCSTR(jssImageTarget));
	*rval = DOUBLE_TO_JSVAL( &duration);
	return JS_TRUE;
}

JSBool JSFUNC_setCursorMode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;

	if(argc != 2)
	{
		JS_ReportError(cx,"JSFUNC_setCursorMode-method must get three parameters");
		return JS_FALSE;
	}

	// Image target
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of setCursorMode(..) must be a string");
		return JS_FALSE;
	}
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[0]);

	int mode = 0;
	if(  !JSVAL_IS_INT(argv[1]))
	{
		JS_ReportError(cx,"Second parameter of setCursorMode(..) must be a integer");
		return JS_FALSE;
	}
	mode = JSVAL_TO_INT( argv[1]);

	pSharingMan->SetCursorMode(cLPCSTR(jssImageTarget), mode);

	return JS_TRUE;
}

/************************************************************************/
/* getOnlyKeyFrame( ImageTarget - as a session id)
/************************************************************************/
JSBool JSFUNC_getOnlyKeyFrame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;

	if(argc != 1)
	{
		JS_ReportError(cx,"JSFUNC_getOnlyKeyFrame-method must get three parameters");
		return JS_FALSE;
	}

	// Image target
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of getOnlyKeyFrame(..) must be a string");
		return JS_FALSE;
	}
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[0]);

	pSharingMan->GetOnlyKeyFrame(cLPCSTR(jssImageTarget));

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();

	return JS_TRUE;
}

/************************************************************************/
/* getAllFrames( ImageTarget - as a session id)
/************************************************************************/
JSBool JSFUNC_getAllFrames(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;

	if(argc != 1)
	{
		JS_ReportError(cx,"JSFUNC_getAllFrames-method must get one parameter");
		return JS_FALSE;
	}

	// Image target
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of getAllFrames(..) must be a string");
		return JS_FALSE;
	}
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[0]);

	pSharingMan->GetAllFrames(cLPCSTR(jssImageTarget));

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();

	return JS_TRUE;
}
/************************************************************************/
/* updateFileNameInSession( ImageTarget - as a session name, Filename - for session)
/************************************************************************/
JSBool JSFUNC_updateFileNameInSession(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;

	if(argc != 2)
	{
		JS_ReportError(cx,"JSFUNC_updateFileNameInSession-method must get two parameters");
		return JS_FALSE;
	}

	// Image target
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of updateFileNameInSession(..) must be a string");
		return JS_FALSE;
	}
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[0]);

	if(  !JSVAL_IS_STRING(argv[1]))
	{
		JS_ReportError(cx,"Second parameter of updateFileNameInSession(..) must be a string");
		return JS_FALSE;
	}
	JSString* jssFileName = JSVAL_TO_STRING( argv[1]);

	pSharingMan->UpdateFileName(cLPCSTR(jssImageTarget), cLPCSTR(jssFileName));

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();

	return JS_TRUE;
}

/************************************************************************/
/* traceAllSession( )
/************************************************************************/
JSBool JSFUNC_traceAllSession(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;
	pSharingMan->TraceAllSession();

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();

	return JS_TRUE;
}

/************************************************************************/
/* stopWaitWebCameraResponce( )
/************************************************************************/
JSBool JSFUNC_stopWaitWebCameraResponce(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;
	if(argc != 1)
	{
		JS_ReportError(cx,"JSFUNC_stopWaitWebCameraResponce-method must get one parameters");
		return JS_FALSE;
	}
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of stopWaitWebCameraResponce(..) must be a string");
		return JS_FALSE;
	}
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[0]);

	pSharingMan->StopWaitWebCameraResponce(cLPCSTR(jssImageTarget));

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();
	return JS_TRUE;
}

/************************************************************************/
/* setSharingViewFps( ImageTarget - as a session id)
/************************************************************************/

JSBool JSFUNC_setSharingViewFps(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;	

	if(argc != 3)
	{
		JS_ReportError(cx,"JSFUNC_setSharingViewFps-method must get three parameters");
		return JS_FALSE;
	}

	// Location identifier
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of setSharingViewFps(..) must be a string (location name)");
		return JS_FALSE;
	}
	JSString* jssLocationName = JSVAL_TO_STRING(argv[0]);

	// Image target
	if( !JSVAL_IS_STRING( argv[1]))
	{
		JS_ReportError(cx,"Second parameter of setSharingViewFps(..) must be a string");
		return JS_FALSE;
	}
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[1]);

	// Sharing quality
	int fps = 0;
	if(  !JSVAL_IS_INT(argv[2]))
	{
		JS_ReportError(cx,"Third parameter of setSharingViewFps(..) must be a integer (fps)");
		return JS_FALSE;
	}
	fps = JSVAL_TO_INT( argv[2]);

	bool result = pSharingMan->SetSharingViewFps( cLPCSTR(jssImageTarget), cLPCSTR(jssLocationName), fps);

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();
	return JS_TRUE;
}

/************************************************************************/
/* boardOwnerChanged( ImageTarget - as a session id)
/************************************************************************/

JSBool JSFUNC_boardOwnerChanged(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;	

	if(argc != 3)
	{
		JS_ReportError(cx,"JSFUNC_boardOwnerChanged-method must get three parameters");
		return JS_FALSE;
	}

	// Location identifier
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of boardOwnerChanged(..) must be a string (location name)");
		return JS_FALSE;
	}
	JSString* jssLocationName = JSVAL_TO_STRING(argv[0]);

	// Image target
	if( !JSVAL_IS_STRING( argv[1]))
	{
		JS_ReportError(cx,"Second parameter of setSharingViewFps(..) must be a string");
		return JS_FALSE;
	}
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[1]);

	// Sharing quality
	int state = 0;
	if(  !JSVAL_IS_INT(argv[2]))
	{
		JS_ReportError(cx,"Third parameter of boardOwnerChanged(..) must be a integer (state)");
		return JS_FALSE;
	}
	state = JSVAL_TO_INT( argv[2]);

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();

	bool result = pSharingMan->SetBoardOwnerChanged( cLPCSTR(jssImageTarget), cLPCSTR(jssLocationName), state);

	*rval = BOOLEAN_TO_JSVAL(result);

	return JS_TRUE;
}

/************************************************************************/
/* stopSharing( ImageTarget - as a session id)
/************************************************************************/
JSBool JSFUNC_stopSharing(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;	

	if(argc != 1)
	{
		JS_ReportError(cx,"stopSharing-method must get one parameters");
		return JS_FALSE;
	}

	// Image Target
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of stopSharing(..) must be a string (image target)");
		return JS_FALSE;
	}
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[0]);

	pSharingMan->StopSharing( cLPCSTR(jssImageTarget));

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();

	return JS_TRUE;
}
/************************************************************************/
/* startSharingPlay(string - location identifier, ImageTarget - as a session id, aFileName - name file on sharing server, Bool - flag cyclic )
/************************************************************************/
JSBool JSFUNC_startSharingPlay(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;	

	if(argc != 7)
	{
		JS_ReportError(cx,"JSFUNC_startSharingPlay-method must get six parameters");
		return JS_FALSE;
	}

	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of startSharingPlay(..) must be a string (location name)");
		return JS_FALSE;
	}
	JSString* jssLocationName = JSVAL_TO_STRING(argv[0]);
	// Image target
	if( !JSVAL_IS_STRING( argv[1]))
	{
		JS_ReportError(cx,"Second parameter of startSharingPlay(..) must be a string (target image)");
		return JS_FALSE;
	}
	JSString* jssImageTarget = JSVAL_TO_STRING(argv[1]);

	// File name
	if( !JSVAL_IS_STRING( argv[2]))
	{
		JS_ReportError(cx,"Third parameter of startSharingPlay(..) must be a string");
		return JS_FALSE;
	}
	JSString* jssImageFileName = JSVAL_TO_STRING(argv[2]);

	bool isPaused = false;
	if(  !JSVAL_IS_BOOLEAN(argv[3]))
	{
		JS_ReportError(cx,"Fourth parameter of startSharingPlay(..) must be a boolean (isPaused)");
		return JS_FALSE;
	}
	isPaused = JSVAL_TO_BOOLEAN( argv[3]);

	int recordID = 0;
	if(  !JSVAL_IS_INT(argv[4]))
	{
		JS_ReportError(cx,"Five parameter of startSharingPlay(..) must be a integer (recordID)");
		return JS_FALSE;
	}
	recordID = JSVAL_TO_INT( argv[4]);

	unsigned int shiftTime = 0;
	if(  !JSVAL_IS_INT(argv[5]))
	{
		JS_ReportError(cx,"Six parameter of startSharingPlay(..) must be a integer (shiftTime)");
		return JS_FALSE;
	}
	shiftTime = JSVAL_TO_INT( argv[5]);

	unsigned int serverVersion = 0;
	if(  !JSVAL_IS_INT(argv[6]))
	{
		JS_ReportError(cx,"Seven parameter of startSharingPlay(..) must be a integer (serverVersion)");
		return JS_FALSE;
	}
	serverVersion = JSVAL_TO_INT( argv[6]);

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();

	pSharingMan->StartSharingPlay( cLPCSTR(jssImageTarget), cLPCSTR(jssLocationName), cLPCSTR(jssImageFileName), isPaused, recordID, shiftTime, serverVersion);

	return JS_TRUE;
}
/************************************************************************/
/* showSharingRect(sessionId - Session Id, flag=1 - Show awareneessRect | flag=0 - Hide awaenessRect   )
/************************************************************************/
JSBool JSFUNC_showSharingRect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, 
							  jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;	

	if(argc != 2)
	{
		JS_ReportError(cx,"JSFUNC_showSharingRect-method must get three parameters");
		return JS_FALSE;
	}

	//Session Id
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of showSharingRect(..) must be a string (session Id)");
		return JS_FALSE;
	}

	JSString* jssSessionId = JSVAL_TO_STRING(argv[0]);

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();

	// flag
	if(  !JSVAL_IS_INT(argv[1]))
	{
		JS_ReportError(cx,"Second parameter of startSharingRect(..) must be a integer (flag)");
		return JS_FALSE;
	}
	int flag = JSVAL_TO_INT( argv[1]);

	if (flag  == 1)
	{
        pSharingMan->ShowSharingSessionRectangle(cLPCSTR(jssSessionId));
	}
	else
	{
		pSharingMan->HideSharingSessionRectangle(cLPCSTR(jssSessionId));
	}

	return JS_TRUE;
}
/************************************************************************/
/* setRemoteAccessMode(sessionName - Session Name, flag=1 - Allow remote access | flag=0 - Deny remote access   )
/************************************************************************/
JSBool JSFUNC_setRemoteAccessMode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, 
							  jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;	

	if(argc != 2)
	{
		JS_ReportError(cx,"JSFUNC_setRemoteAccessMode-method must get two parameters");
		return JS_FALSE;
	}

	//Session name
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of setRemoteAccessMode(..) must be a string (session name)");
		return JS_FALSE;
	}

	JSString* jssSessionId = JSVAL_TO_STRING(argv[0]);

	// flag
	if(  !JSVAL_IS_INT(argv[1]))
	{
		JS_ReportError(cx,"Second parameter of setRemoteAccessMode(..) must be a integer (flag)");
		return JS_FALSE;
	}
	int flag = JSVAL_TO_INT( argv[1]);

	if (flag  == 1)
	{
		pSharingMan->AllowRemoteControl(cLPCSTR(jssSessionId));
	}
	else
	{
		pSharingMan->DenyRemoteControl(cLPCSTR(jssSessionId));
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();

	return JS_TRUE;
}

JSBool JSFUNC_fullControlChanged(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, 
	jsval *rval)
{
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return JS_FALSE;	

	if(argc != 2)
	{
		JS_ReportError(cx,"JSFUNC_fullControlChanged-method must get two parameters");
		return JS_FALSE;
	}

	//Session name
	if( !JSVAL_IS_STRING( argv[0]))
	{
		JS_ReportError(cx,"First parameter of fullControlChanged(..) must be a string (session name)");
		return JS_FALSE;
	}

	JSString* jssSessionId = JSVAL_TO_STRING(argv[0]);

	// flag
	if(  !JSVAL_IS_INT(argv[1]))
	{
		JS_ReportError(cx,"Second parameter of fullControlChanged(..) must be a integer (flag)");
		return JS_FALSE;
	}
	int flag = JSVAL_TO_INT( argv[1]);

	pSharingMan->FullControlChanged(cLPCSTR(jssSessionId), flag);


	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->SetDesktopCallbacks();

	return JS_TRUE;
}

// Callback function
mlSyncCallbackManager::mlSyncCallbackManager(mlSceneManager* apSM){
	mpSM = apSM;
	mbCallbacksIsSet = false;
	m_isInRecording = false;
	m_isMyRecord = false;
	m_recordID = 0xFFFFFFFF;
}

void mlSyncCallbackManager::SetCallbacks(JSContext *cx){
	if(mbCallbacksIsSet) return;
	omsContext* pOMSContext = mpSM->GetContext();
	if(pOMSContext == NULL) return;
	sync::syncISyncManager* pSyncMan = pOMSContext->mpSyncMan;
	if(pSyncMan == NULL)
		return;
	mbCallbacksIsSet = pSyncMan->SetDefaultCallbacks(this, this, mpSM->GetWorldEventCallbackManager());
}

void mlSyncCallbackManager::onSeanceSeekPos(unsigned int auTime)
{
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_SeanceSeekPos, (unsigned char*)&auTime, 4);
}

// с сервака для шкалы пришло время
void mlSyncCallbackManager::onUpdatePlayingTime(unsigned int auTime)
{
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_UpdatePlayingTime, (unsigned char*)&auTime, 4);

}

// Сеанс закрыт
void mlSyncCallbackManager::onSeanceClosed(unsigned int aiSeanceID)
{
	JSContext* cx = mpSM->GetJSContext();
	jsval v = JSVAL_NULL;
}

// --------------------------------  Работа с сеансами ------------------------------------
// сеанс создан
void mlSyncCallbackManager::onSeanceCreated(unsigned int aiSeanceID, unsigned int aiSeanceObjectID, unsigned int aSeanceMode, const wchar_t* aAuthor, const wchar_t* aSeanceURL, int aiErrorCode)
{
	JSContext* cx = mpSM->GetJSContext();
	jsval v = JSVAL_NULL;
	JSObject* jso = (JSObject*)aiSeanceObjectID;
	GET_SAVED_FROM_GC(cx, mpSM->GetSRecorderGO(), jso, (&v))
	mlSynchData LogData;
	LogData<<aiSeanceID;
	LogData<<aAuthor;
	LogData<<aSeanceURL;
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnReceivedCreatedSeanceInfo, (unsigned char*)LogData.data(), LogData.size());
}



void mlSyncCallbackManager::onSeanceLeaved()
{ 
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnSeanceLeaved, NULL, 0 );

}

void mlSyncCallbackManager::onReceivedFullSeanceInfo(unsigned int aiSeanceObjectID,  syncSeanceInfo* appSeancesInfo, int aiErrorCode)
{
		// Отложенный сеанс
		mlSynchData LogData;
		LogData<<aiSeanceObjectID;
		LogData<<appSeancesInfo->muSeanceID;
		LogData<<appSeancesInfo->mpwcName.c_str();
		LogData<<appSeancesInfo->mpwcAuthor.c_str();
		LogData<<appSeancesInfo->mpwcMembers.c_str();
		LogData<<appSeancesInfo->mpwcURL.c_str();
		LogData << appSeancesInfo->mlMode;
		mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnReceivedFullSeanceInfo, (unsigned char*)LogData.data(), LogData.size());
}


// с сервера получена информация о сенасах по записи лога
void mlSyncCallbackManager::onReceivedSeancesInfo(unsigned int auSeancesCount, syncSeanceInfo** appSeancesInfo, unsigned int auID, int aiErrorCode)
{
	JSContext* cx = mpSM->GetJSContext();
	jsval v = JSVAL_NULL;
	JSObject* jso = (JSObject*)auID;
	GET_SAVED_FROM_GC(cx, mpSM->GetSRecorderGO(), jso, (&v))
	// если объект по какой-то причине не нашелся
	if(!JSVAL_IS_REAL_OBJECT(v)){
		return;
	}
	// так как возможно этот вызов происхордит в другом потоке
	// то просто запоминаем все в вектор, чтобы потом создать объекты-обертки (mlLogRecordJSO)
	JSObject* jsoSeances = JSVAL_TO_OBJECT(v);
	mlLogSeancesJSO* pSeances = (mlLogSeancesJSO*)JS_GetPrivate(cx, jsoSeances);
	if(pSeances == NULL)
		return;
	// если произошла ошибка
	if(aiErrorCode > 0){
		// то надо будет сгенерировать сообщение об ошибке
		pSeances->SetErrorCode(aiErrorCode);
	}else{
		pSeances->SaveSeanceInfo(appSeancesInfo);
	}

	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnReceivedSeancesInfo, (unsigned char*)&auID, 4);
}
// Обработка информации о списке сеансов на UPDATE-е
void mlSyncCallbackManager::onReceivedSeancesInfoInternal(mlSynchData& aData){
	unsigned int uID;
	aData >> uID;
	JSContext* cx = mpSM->GetJSContext();
	jsval v = JSVAL_NULL;
	JSObject* jso = (JSObject*)uID;

	GET_SAVED_FROM_GC(cx, mpSM->GetSRecorderGO(), jso, (&v))
	// если объект по какой-то причине не нашелся
	if(!JSVAL_IS_REAL_OBJECT(v))
		return;

	FREE_FOR_GC((mpSM->GetJSContext()), mpSM->GetSRecorderGO(), jso);
	JSObject* jsoSeances = JSVAL_TO_OBJECT(v);
	mlLogSeancesJSO* pSeances = (mlLogSeancesJSO*)JS_GetPrivate(cx, jsoSeances);

	if(pSeances == NULL)
		return;

	if(pSeances  != NULL)
		pSeances->OnReceivedSeancesInfo();		
}

void mlSyncCallbackManager::OnSeanceSeekPosInternal(mlSynchData& aData)
{
	unsigned int iTime= 0;
	aData >> iTime;

	jsval jsv = INT_TO_JSVAL(iTime);
	JSContext *mcx = (mpSM->GetJSContext());
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "Time", &jsv);
	mpSM->NotifyRecorderListeners(EVID_onSeanceSeekPos);
	JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "Time");
}

void mlSyncCallbackManager::OnUpdatePlayingTimeInternal(mlSynchData& aData)
{
	unsigned int iTime= 0;
	aData >> iTime;
	
	jsval jsv = INT_TO_JSVAL(iTime);
	JSContext *mcx = (mpSM->GetJSContext());
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "Time", &jsv);
	mpSM->NotifyRecorderListeners(EVID_onUpdatePlayingTime);
	JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "Time");
}

void mlSyncCallbackManager::OnSeanceLoadedInternal(unsigned int aSeanceID,  unsigned int aRecordID, unsigned char isAuthor, unsigned char isSuspended, int aMinutes)
{
		JSContext *cx = (mpSM->GetJSContext());
		jsval vAuthor = JSVAL_NULL;
		JSString *jssAuthor;
		if (isAuthor==1)
			jssAuthor= JS_NewUCStringCopyZ(cx,(const jschar*)L"Author");
		else 
			jssAuthor= JS_NewUCStringCopyZ(cx,(const jschar*)L"NONAuthor");

		JSString *jssSeanceType;
		if (isSuspended==0)
			jssSeanceType = JS_NewUCStringCopyZ(cx,(const jschar*)L"REAL");
		else 
			jssSeanceType = JS_NewUCStringCopyZ(cx,(const jschar*)L"SUSPENDED");

		jsval vSeanceType = JSVAL_NULL;
		vSeanceType = STRING_TO_JSVAL(jssSeanceType);
		JS_SetProperty(cx, mpSM->GetEventGO(), "seanceType", &vSeanceType);

		vAuthor = STRING_TO_JSVAL(jssAuthor);
		JS_SetProperty(cx, mpSM->GetEventGO(), "author", &vAuthor);

		jsval vMinutes = JSVAL_NULL;
		vMinutes = INT_TO_JSVAL(aMinutes);
		JS_SetProperty(cx, mpSM->GetEventGO(), "minutes", &vMinutes);

		mpSM->pSeance->SetSeanceID(aSeanceID);
		mpSM->pSeance->SetRecordID(aRecordID);
		//if (aSeanceID!=0)
		if( aRecordID != 0)	// Alex 2010-09-18
			mpSM->NotifyRecorderListeners(EVID_onSeanceLoaded);
		else
			mpSM->NotifyRecorderListeners(EVID_onLessonLoaded);
		
		JS_DeleteProperty((mpSM->GetJSContext()), mpSM->GetEventGO(), "author");
		JS_DeleteProperty((mpSM->GetJSContext()), mpSM->GetEventGO(), "seanceType");
		JS_DeleteProperty((mpSM->GetJSContext()), mpSM->GetEventGO(), "minutes");
}

void mlSyncCallbackManager::OnReceivedFullRecordInfoInternal(mlSynchData& aData)
{
	unsigned int uRMMLID;
	unsigned int uRecordID;
	unsigned int uDuration;
	unsigned int uDepth, uVisitorCount, uReality, uStatus, uAvatarInRecordCount;
	bool isReal = false;

	aData >> uRMMLID;
	aData >> uRecordID;
	wchar_t* sParam = L"";
	aData >> sParam;
	mlString sName(sParam);
	aData >> sParam;
	mlString sDescription(sParam);
	aData >> sParam;
	mlString sAuthor(sParam);
	aData >> sParam;
	mlString sAuthorFullName(sParam);
	aData >> sParam;
	mlString sLocation(sParam);
	aData >> uDuration;
	aData >> uDepth;
	aData >> uAvatarInRecordCount;
	aData >> uVisitorCount;
	aData >> uReality;
	aData >> uStatus;
	aData >> sParam;
	mlString sURL(sParam);
	aData >> sParam;
	mlString sOutsideURL(sParam);
	aData >> isReal;


	// Заполняем внутренний объект текущей записи
	syncRecordInfo recordInfo(uRecordID,sName.c_str(),sDescription.c_str(), sAuthor.c_str(), sAuthorFullName.c_str(),  sLocation.c_str(), NULL, NULL, NULL, uDepth , NULL, uAvatarInRecordCount, uVisitorCount, isReal,uStatus,L"" );
	recordInfo.mlDuration = uDuration;
	recordInfo.mpwcCreationTime.clear();
	recordInfo.mpwcURL = sURL.c_str();
	mlLogRecordInfo mlRecInfo(&recordInfo);
	mpSM->pRecord->Init(mlRecInfo);
	mpSM->pRecord->SetRecordURL(sURL.c_str());
	mpSM->pRecord->SetRecordOutsideURL(sOutsideURL.c_str());

	jsval v = JSVAL_NULL;
	JSObject* jso = (JSObject*)uRMMLID;
	GET_SAVED_FROM_GC((mpSM->GetJSContext()), mpSM->GetSRecorderGO(), jso, &v);

	if(JSVAL_IS_REAL_OBJECT(v)){
		FREE_FOR_GC((mpSM->GetJSContext()), mpSM->GetSRecorderGO(), jso);
		JSObject* jsoRecord = JSVAL_TO_OBJECT(v);
		mlLogRecordJSO* pRecord = (mlLogRecordJSO*)JS_GetPrivate((mpSM->GetJSContext()), jsoRecord);
		if(pRecord != NULL){
			pRecord->SetRecordURL(sURL.c_str());
			pRecord->SetRecordOutsideURL(sOutsideURL.c_str());
			pRecord->OnReceivedFullInfo();
		}
	}
}

void mlSyncCallbackManager::OnReceivedFullSeanceInfoInternal(mlSynchData& aData)
{
	unsigned int uID;
	unsigned int uSeanceID;
	unsigned int uMode;
	wchar_t* sParam = L"";
	aData >> uID;
	aData >> uSeanceID;
	aData >> sParam;
	mlString sName(sParam);
	aData >> sParam;
	mlString sAuthor(sParam);
	aData >> sParam;
	mlString sMembers(sParam);
	aData >> sParam;
	mlString sURL(sParam);
	aData >> uMode;

	jsval v = JSVAL_NULL;
	JSObject* jso = (JSObject*)uID;
	GET_SAVED_FROM_GC((mpSM->GetJSContext()), mpSM->GetSRecorderGO(), jso, &v);
	if(JSVAL_IS_REAL_OBJECT(v)){
		FREE_FOR_GC((mpSM->GetJSContext()), mpSM->GetSRecorderGO(), jso);
		JSObject* jsoSeance = JSVAL_TO_OBJECT(v);
		mlLogSeanceJSO* pSeance = (mlLogSeanceJSO*)JS_GetPrivate((mpSM->GetJSContext()), jsoSeance);
		if(pSeance != NULL){
			pSeance->SetSeanceID(uSeanceID);
			pSeance->SetSeanceAuthor(sAuthor.c_str());
			pSeance->SetSeanceURL(sURL.c_str());
			pSeance->OnReceivedFullInfo();
		}
	}
	else
	{
		mpSM->pSeance->SetSeanceID(uSeanceID);
		mpSM->pSeance->SetSeanceAuthor(sAuthor.c_str());
		mpSM->pSeance->SetSeanceURL(sURL.c_str());
		mpSM->pSeance->SetSeanceName(sName.c_str());
	}
}

// Обработка информации о созданном сеансе на UPDATE-е
void mlSyncCallbackManager::onReceivedSeanceInfoInternal(mlSynchData& aData){
	unsigned int uSeanceID;
	wchar_t* sParam = L"";
	aData >> uSeanceID;
	aData >> sParam;
	mlString sAuthor(sParam);
	aData >> sParam;
	mlString sURL(sParam);

	JSContext *mcx = (mpSM->GetJSContext());

	mpSM->pSeance->SetSeanceID(uSeanceID);
	mpSM->pSeance->SetSeanceURL(sURL.c_str());
	mpSM->pSeance->SetSeanceAuthor(sAuthor.c_str());

}

// Получено сообщение о паузе в проигрывании записи
void mlSyncCallbackManager::onSeancePaused(unsigned int auBornRealityID)
{
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnFreezeSeance, (unsigned char*)&auBornRealityID, 4);
	mpSM->SetSeancePlaying(false);
}

// Получено сообщение о то что начато проигрывание сеанса
void mlSyncCallbackManager::onSeancePlayed()
{
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnSeancePlayed, NULL, 0);
	mpSM->SetSeancePlaying(true);
}
// Получено сообщение о то что сработала перемотка
void mlSyncCallbackManager::onSeanceRewinded()
{
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnSeanceRewinded, NULL, 0);
	mpSM->SetSeancePlaying(false);
}

// Получено сообщение о то что сеанс проигран до конца
void mlSyncCallbackManager::onSeanceFinishedPlaying()
{
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnSeanceFinishedPlaying, NULL, 0);
	mpSM->SetSeancePlaying(false);
}

void mlSyncCallbackManager::onUserListReceived(unsigned int realityID, unsigned short userCount, void* userList)
{
}

// Получено сообщение о входе в сеанс
void mlSyncCallbackManager::onJoinedToSeance(unsigned int aiSeanceRecord,unsigned int aiSeanceID, unsigned char isAuthor, unsigned char isSuspended, int aMinutes)
{
	mlSynchData recordData;
	recordData<<aiSeanceRecord;
	recordData<<aiSeanceID;
	recordData<<isAuthor;
	recordData<<isSuspended;
	recordData<<aMinutes;

	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnJoinedToSeance, (unsigned char*)recordData.data(), recordData.size());
}

// -----------------------------   Работа с записями лога -------------------------------------
// с сервера получена информация о записях (auID - идентификатор запроса, переданный в GetRecords())
void mlSyncCallbackManager::onReceivedRecordsInfo( sync::syncCountRecordInfo& totalCountRec, unsigned int auRecordCount, syncRecordInfo** appRecordsInfo, unsigned int auID, int aiErrorCode){
/*
	JSContext* cx = mpSM->GetJSContext();
	GUARD_JS_CALLS(cx);
	jsval v = JSVAL_NULL;
	JSObject* jso = (JSObject*)auID;
	GET_SAVED_FROM_GC(cx, mpSM->GetSRecorderGO(), jso, (&v))
	// если объект по какой-то причине не нашелся
	if(!JSVAL_IS_REAL_OBJECT(v)){
		return;
	}
*/
	// так как возможно этот вызов происхордит в другом потоке
	// то просто запоминаем все в вектор, чтобы потом создать объекты-обертки (mlLogRecordJSO)
//	else{
//		pRecords->SaveRecordsInfo(totalCountRec, auRecordCount, appRecordsInfo);
//	}
	// на следующем Update-е надо будет создать объекты-обертки и сгенерировать соответствующие сообщения
	mlSynchData recordData;
	recordData<<auID;
	recordData<<totalCountRec.totalCountRecord;
	recordData<<totalCountRec.totalCountRecordNow;
	recordData<<totalCountRec.totalCountRecordWithoutStatus;
	recordData<<totalCountRec.totalCountRecordSoon;

// 10.03 переделываю заполнение списка на апдейте
// Пишем сначала количество а потом инфу по каждой записи
	recordData << auRecordCount;
	for (unsigned int i = 0;  i < auRecordCount;  i++)
	{
		recordData << (*appRecordsInfo)->muRecordID;
		recordData << (*appRecordsInfo)->mpwcName.c_str();
		recordData << (*appRecordsInfo)->mpwcDescription.c_str();
		recordData << (*appRecordsInfo)->mpwcAuthor.c_str();
		recordData << (*appRecordsInfo)->mpwcAuthorFullName.c_str();
		recordData << (*appRecordsInfo)->mpwcLocation.c_str();
		recordData << (*appRecordsInfo)->mpwcPreview.c_str();
		recordData << (*appRecordsInfo)->mlDuration;
		recordData << (*appRecordsInfo)->muDepth;
		recordData << (*appRecordsInfo)->muAvatarInRecordCount;
		recordData << (*appRecordsInfo)->muVisitorCount;
		recordData << (*appRecordsInfo)->mbPublishNow;
		recordData << (*appRecordsInfo)->status;
		recordData <<(__int64)(*appRecordsInfo)->mlCreationTime;
		
		*appRecordsInfo++;
	}

	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_syncOnReceivedRecordsInfo, (unsigned char*)recordData.data(), recordData.size());
}


void mlSyncCallbackManager::onReceivedRecordsInfoInternal(mlSynchData& aData){
	unsigned int uID;
	//unsigned int uTotalCount;
	sync::syncCountRecordInfo totalCountRec;
	aData >> uID;
	jsval v = JSVAL_NULL;
	JSObject* jso = (JSObject*)uID;
	GET_SAVED_FROM_GC((mpSM->GetJSContext()), mpSM->GetSRecorderGO(), jso, &v);
	if(JSVAL_IS_REAL_OBJECT(v)){
		// 3.04.08
		FREE_FOR_GC((mpSM->GetJSContext()), mpSM->GetSRecorderGO(), jso);
		JSObject* jsoRecords = JSVAL_TO_OBJECT(v);
		mlLogRecordsJSO* pRecords = (mlLogRecordsJSO*)JS_GetPrivate((mpSM->GetJSContext()), jsoRecords);
		if(pRecords != NULL){
			aData >> totalCountRec.totalCountRecord;
			aData >> totalCountRec.totalCountRecordNow;
			aData >> totalCountRec.totalCountRecordWithoutStatus;
			aData >> totalCountRec.totalCountRecordSoon;

			unsigned int auRecordCount;
			aData >> auRecordCount;
	
			for (unsigned int i = 0;  i < auRecordCount;  i++)
			{
				syncRecordInfo recordInfo;
				aData >> recordInfo.muRecordID;
				wchar_t* sString = L"";
				aData >> sString;
                recordInfo.mpwcName = sString;
				aData >> sString;
				recordInfo.mpwcDescription = sString;
				aData >> sString;
				recordInfo.mpwcAuthor = sString;
				aData >> sString;
				recordInfo.mpwcAuthorFullName = sString;
				aData >> sString;
				recordInfo.mpwcLocation = sString;
				aData >> sString;
				recordInfo.mpwcPreview = sString;
				aData >> recordInfo.mlDuration;
				aData >> recordInfo.muDepth;
				aData >> recordInfo.muAvatarInRecordCount;
				aData >> recordInfo.muVisitorCount;
				aData >> recordInfo.mbPublishNow;
				aData >> recordInfo.status;
				__int64 time;
				aData >> time;
				recordInfo.mlCreationTime = time;
				pRecords->AddNewRecord(&recordInfo);
			}
			pRecords->OnReceivedRecordsInfo(totalCountRec);
		}
	}
}

}

void mlSyncCallbackManager::onAllSyncObjectsLoaded()
{
	mpSM->NotifyRecorderListeners(EVID_onSynchronizedObjectsLoaded);
}

void mlSceneManager::onSeancePaused(){
	NotifyRecorderListeners(EVID_onSeancePaused);
	if( GetContext()->mpVoipMan)
		GetContext()->mpVoipMan->PauseServerAudio();
}

void mlSceneManager::onSeancePlayed(){
	NotifyRecorderListeners(EVID_onSeancePlayed);
	if( GetContext()->mpVoipMan)
		GetContext()->mpVoipMan->ResumeServerAudio();
}

void mlSceneManager::onSeanceRewinded(){
	NotifyRecorderListeners(EVID_onSeanceRewinded);
	if( GetContext()->mpVoipMan)
		GetContext()->mpVoipMan->RewindServerAudio();
	if (GetContext()->mpSharingMan)
		GetContext()->mpSharingMan->RewindServerSharing();
}


void mlSceneManager::onSeanceFinishedPlaying()
{
	NotifyRecorderListeners(EVID_onSeanceFinishedPlaying);
	if( GetContext()->mpVoipMan)
		GetContext()->mpVoipMan->PauseServerAudio();
	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( !pSharingMan)
		return;
	pSharingMan->PauseServerSharing();
	
}

void mlSceneManager::onSeanceLeaved()
{
	assert( false);
	NotifyRecorderListeners(EVID_onSeanceLeaved);
	mVoipCallbackManager->onEventLeaved();

	desktop::IDesktopSharingManager* pSharingMan = GPSM(cx)->GetContext()->mpSharingMan;
	if( pSharingMan)
		pSharingMan->StopPlayingSharing();
}

bool mlSceneManager::addRecorderListener(char aidEvent, mlRMMLElement* apMLEl){
	if(apMLEl == NULL) return false;
	char chEvID=aidEvent;
	if(chEvID >= ML_RECORDER_EVENTS_COUNT) return false;
	v_elems* pv=&(maRecorderListeners[chEvID]);
	v_elems::const_iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl) return true;
	}
	pv->push_back(apMLEl);
	return true;
}

void mlSceneManager::NotifyOnDesktopFileSaved( wchar_t* fileName, char* target)
{
	JSString* str = wr_JS_NewUCStringCopyZ( cx, fileName);
	jsval v = STRING_TO_JSVAL( str);
	JSString* str2 = JS_NewStringCopyZ( cx, target);
	jsval v2 = STRING_TO_JSVAL( str2);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"fileName",&v);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"target",&v2);
	NotifyRecorderListeners(EVID_onDesktopImageFileSaved);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "fileName");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "target");
}

void mlSceneManager::NotifyOnSharingSessionStarted(const char* alpcTarget, const char* alpcSessionKey, const char* alpcFileName)
{
	JSString* str = JS_NewStringCopyZ( cx, alpcTarget);
	jsval jsTarget = STRING_TO_JSVAL( str);
	JSString* str2 = JS_NewStringCopyZ( cx, alpcSessionKey);
	jsval jsSessionKey = STRING_TO_JSVAL( str2);
	JSString* str3 = JS_NewStringCopyZ( cx, alpcFileName);
	jsval jsFileName = STRING_TO_JSVAL( str3);
	
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"target",&jsTarget);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"sessionKey",&jsSessionKey);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"fileName",&jsFileName);
	NotifyRecorderListeners(EVID_onSharingSessionStarted);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "target");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "sessionKey");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "fileName");
}

void mlSceneManager::NotifyOnSharingPreviewCancel(const char* alpcTarget)
{
	JSString* str = JS_NewStringCopyZ( cx, alpcTarget);
	jsval jsTarget = STRING_TO_JSVAL( str);	

	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"target",&jsTarget);	
	NotifyRecorderListeners(EVID_onSharingPreviewCancel);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "target");
}

void mlSceneManager::NotifyOnSharingPreviewOK(const char* alpcTarget)
{
	JSString* str = JS_NewStringCopyZ( cx, alpcTarget);
	jsval jsTarget = STRING_TO_JSVAL( str);	

	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"target",&jsTarget);	
	NotifyRecorderListeners(EVID_onSharingPreviewOK);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "target");
}


void mlSceneManager::NotifyOnSharingViewSessionStarted(const char* alpcTarget, const char* alpcSessionKey, bool canRemoteAccess)
{
	JSString* str = JS_NewStringCopyZ( cx, alpcTarget);
	jsval jsTarget = STRING_TO_JSVAL( str);
	JSString* str2 = JS_NewStringCopyZ( cx, alpcSessionKey);
	jsval jsSessionKey = STRING_TO_JSVAL( str2);
	jsval jsCanRemoteAccess = JSVAL_NULL;
	jsCanRemoteAccess = BOOLEAN_TO_JSVAL(canRemoteAccess);

	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"target",&jsTarget);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"sessionKey",&jsSessionKey);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(), "canRemoteAccess", &jsCanRemoteAccess);
	NotifyRecorderListeners(EVID_onSharingViewSessionStarted);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "target");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "sessionKey");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "canRemoteAccess");
}

void mlSceneManager::NotifyOnTextureSizeChanged(const char* alpcTarget, unsigned int width, unsigned int height)
{
	JSString* str = JS_NewStringCopyZ( cx, alpcTarget);
	jsval jsTarget = STRING_TO_JSVAL( str);

	jsval jsWidth = JSVAL_NULL;
	jsWidth = INT_TO_JSVAL(width);
	
	jsval jsHeight = JSVAL_NULL;
	jsHeight = INT_TO_JSVAL(height);

	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"target",&jsTarget);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(), "width", &jsWidth);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(), "height", &jsHeight);
	
	NotifyRecorderListeners(EVID_onTextureSizeChanged);
	
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "target");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "width");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "height");
}

void mlSceneManager::NotifyOnSessionClosed(const char* alpcTarget)
{
	JSString* str = JS_NewStringCopyZ( cx, alpcTarget);
	jsval jsTarget = STRING_TO_JSVAL( str);
	
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"target",&jsTarget);
	NotifyRecorderListeners(EVID_onSessionClosed);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "target");
}

void mlSceneManager::NotifyOnViewSessionClosed(const char* alpcTarget)
{
	JSString* str = JS_NewStringCopyZ( cx, alpcTarget);
	jsval jsTarget = STRING_TO_JSVAL( str);

	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"target",&jsTarget);
	NotifyRecorderListeners(EVID_onViewSessionClosed);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "target");
}

void mlSceneManager::NotifyRecorderListeners(char chEvID)
{
	assert( ML_RECORDER_EVENTS_COUNT >= EVID_RecorderEventsLast);

	if(chEvID < ML_RECORDER_EVENTS_COUNT)
	{
		v_elems* pv=&(maRecorderListeners[chEvID]);
		v_elems::const_iterator vi;
		for(vi=pv->begin(); vi != pv->end(); vi++ ){
			mlRMMLElement* pMLEl=*vi;
			pMLEl->EventNotify(chEvID,(mlRMMLElement*)GOID_Recoder);
		}
	}
}

bool mlSceneManager::removeRecorderListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	if(aidEvent==-1){
		// и удалим его из списка слушателей всех событий
		for(int ii=0; ii<ML_RECORDER_EVENTS_COUNT; ii++){
			v_elems* pv=&maRecorderListeners[ii];
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
	v_elems* pv=&(maRecorderListeners[aidEvent-1]);
	v_elems::iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl){
			pv->erase(vi);
			return true;
		}
	}
	return true;
}

void mlSceneManager::NotifyOnSharingFileSaved(const char* alpcTarget, const char* alpcFileName)
{
	JSString* str = JS_NewStringCopyZ( cx, alpcTarget);
	jsval jsTarget = STRING_TO_JSVAL( str);
	JSString* str2 = JS_NewStringCopyZ( cx, alpcFileName);
	jsval jsFileName = STRING_TO_JSVAL( str2);

	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"target",&jsTarget);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"fileName",&jsFileName);
	NotifyRecorderListeners(EVID_onSharingFileSaved);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "target");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "fileName");	
}

void mlSceneManager::NotifyOnRemoteAccessModeChanged(const char* alpcTarget, bool canRemoteAccess)
{
	JSString* str = JS_NewStringCopyZ( cx, alpcTarget);
	jsval jsTarget = STRING_TO_JSVAL( str);
	jsval jsCanRemoteAccess = JSVAL_NULL;
	jsCanRemoteAccess = BOOLEAN_TO_JSVAL(canRemoteAccess);

	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"target", &jsTarget);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(), "canRemoteAccess", &jsCanRemoteAccess);
	NotifyRecorderListeners(EVID_onRemoteAccessModeChanged);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "target");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "canRemoteAccess");
}

void mlSceneManager::NotifyWebCameraState(const char* alpcTarget, int stateCode)
{
	JSString* str = JS_NewStringCopyZ( cx, alpcTarget);
	jsval jsTarget = STRING_TO_JSVAL( str);
	jsval jsWebCamState = JSVAL_NULL;
	jsWebCamState = INT_TO_JSVAL(stateCode);

	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"target", &jsTarget);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(), "webCameraState", &jsWebCamState);
	NotifyRecorderListeners(EVID_onWebCameraState);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "target");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "webCameraState");
}

void mlSceneManager::NotifySharingEventByCode(const char* alpcTarget, int aCode)
{
	JSString* str = JS_NewStringCopyZ( cx, alpcTarget);
	jsval jsTarget = STRING_TO_JSVAL( str);
	jsval jsEvCode = JSVAL_NULL;
	jsEvCode = INT_TO_JSVAL(aCode);

	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"target", &jsTarget);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(), "eventCode", &jsEvCode);
	NotifyRecorderListeners(EVID_onSharingEventByCode);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "target");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "eventCode");
}

void mlSceneManager::NotifyOnSharingSetBarPosition(const char* alpcTarget, double aSecond, double aDuration)
{
	JSString* str = JS_NewStringCopyZ( cx, alpcTarget);
	jsval jsTarget = STRING_TO_JSVAL( str);
	jsval jsEvSecond = JSVAL_NULL;
	jsdouble* jsdbSecond = JS_NewDouble(cx, aSecond);
	jsEvSecond = DOUBLE_TO_JSVAL( jsdbSecond);
	
	jsval jsEvDuration = JSVAL_NULL;
	jsdouble* jsdbDuration = JS_NewDouble(cx, aDuration);
	jsEvDuration = DOUBLE_TO_JSVAL( jsdbDuration);

	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"target",&jsTarget);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(), "eventSecond", &jsEvSecond);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(), "eventDuration", &jsEvDuration);
	NotifyRecorderListeners(EVID_onSharingEventSetBarPosition);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "target");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "eventSecond");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "eventDuration");
}

void mlSceneManager::NotifyOnSharingVideoEnd(const char* alpcTarget)
{
	JSString* str = JS_NewStringCopyZ( cx, alpcTarget);
	jsval jsTarget = STRING_TO_JSVAL( str);

	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"target",&jsTarget);
	NotifyRecorderListeners(EVID_onSharingEventVideoEnd);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "target");
}

void mlSceneManager::NotifyGlobalMouseEvent(unsigned int wParam, float aX, float aY)
{
	jsval jsEvX = JSVAL_NULL;
	jsval jsEvY = JSVAL_NULL;
	jsval jsEvType = JSVAL_NULL;
	jsdouble* jsdblX = JS_NewDouble(cx, aX); 
	jsdouble* jsdblY = JS_NewDouble(cx, aY); 
	jsEvX = DOUBLE_TO_JSVAL(jsdblX);
	jsEvY = DOUBLE_TO_JSVAL(jsdblY);
	jsEvType = INT_TO_JSVAL(wParam);

	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"eventX", &jsEvX);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(), "eventY", &jsEvY);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(), "eventType", &jsEvType);
	NotifyRecorderListeners(EVID_onGlobalMouseEvent);
	JS_DeleteProperty( cx,GPSM(cx)->GetEventGO(),"eventX");
	JS_DeleteProperty( cx,GPSM(cx)->GetEventGO(), "eventY");
	JS_DeleteProperty( cx,GPSM(cx)->GetEventGO(), "eventType");
}

void mlSceneManager::NotifyAvatarImageFileSaved( wchar_t* tmpFileName, const char* fileName)
{
	JSString* str = wr_JS_NewUCStringCopyZ( cx, tmpFileName);
	jsval tmpF = STRING_TO_JSVAL( str);
	str = JS_NewStringCopyZ( cx, fileName);
	jsval resF = STRING_TO_JSVAL( str);
	
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"tmpFileName",&tmpF);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"fileName",&resF);
	NotifyRecorderListeners(EVID_onAvatarImageFileSaved);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "tmpFileName");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "fileName");
}
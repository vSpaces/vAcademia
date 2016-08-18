#include "mlRMML.h"
#include "config/oms_config.h"
#include "SyncMan.h"
#include "mlLogRecord.h"
#include "mlJSONRPCJSO.h"
#include "config/prolog.h"
#include "ILogWriter.h"
#include "mdump.h"

namespace rmml {

/**
 * Реализация глобального JS-объекта "RecordEditor"
 */

JSBool JSRecordEditorGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool JSRecordEditorSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){ return JS_TRUE; }
	
JSClass JSRMMLRecordEditorClass = {
	"_RecordEditor", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, 
	JSRecordEditorGetProperty, JSRecordEditorSetProperty,
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL,
	NULL, NULL, NULL
};

enum {
	EVID_onCutOperationComplete=1,
	EVID_onCopyOperationComplete,
	EVID_onPasteOperationComplete,
	EVID_onDeleteOperationComplete,
	EVID_onUndoOperationComplete,
	EVID_onRedoOperationComplete,
	EVID_onPlayOperationComplete,
	EVID_onPauseOperationComplete,
	EVID_onRewindOperationComplete,
	EVID_onSeekOperationComplete,
	EVID_onSaveOperationComplete,
	EVID_onSaveAsOperationComplete,
	EVID_onOpenOperationComplete
};

JSPropertySpec _JSRecordEditorPropertySpec[] = {
//	JSPROP_RO(name) // доменноое имя сервера
	{ 0, 0, 0, 0, 0 }
};

JSFUNC_DECL(cutOperation)
JSFUNC_DECL(copyOperation)
JSFUNC_DECL(pasteOperation)
JSFUNC_DECL(deleteOperation)

JSFUNC_DECL(undoOperation)
JSFUNC_DECL(redoOperation)

JSFUNC_DECL(openRecord)
JSFUNC_DECL(saveRecord)
JSFUNC_DECL(saveAsRecord)

JSFUNC_DECL(playOperation)
JSFUNC_DECL(pauseOperation)
JSFUNC_DECL(rewindOperation)
JSFUNC_DECL(seekOperation)

///// JavaScript Function Table
JSFunctionSpec _JSRecordEditorFunctionSpec[] = {
	JSFUNC(cutOperation,2)		// вырезать кусок записи
	JSFUNC(copyOperation,2)		// скопировать кусок записи
	JSFUNC(pasteOperation,1)		// вставить кусок записи
	JSFUNC(deleteOperation,2)		// удалить кусок записи

	JSFUNC(undoOperation,0)		// отменить операцию
	JSFUNC(redoOperation,0)		// повторить операцию

	JSFUNC(openRecord,1)		// открыть запись для редактирования
	JSFUNC(saveRecord,0)		// сохранить изменения в записи
	JSFUNC(saveAsRecord,3)		// сохранить запись под новым именем

	JSFUNC(playOperation,0)		// начать проигрывать запись
	JSFUNC(pauseOperation,0)		// приостановить проигрывать запись
	JSFUNC(rewindOperation,0)		// перемотать воспоизведение записи на начало
	JSFUNC(seekOperation,1)		// перемотать воспроизведение записи в позицию

	{ 0, 0, 0, 0, 0 }
};

void CreateRecordEditorJSObject(JSContext *cx, JSObject *ajsoPlayer){
    JSObject* jsoRecordEditor = JS_DefineObject(cx, ajsoPlayer, GJSONM_RECORD_EDITOR, &JSRMMLRecordEditorClass, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
	JSBool bR;
	bR=JS_DefineProperties(cx, jsoRecordEditor, _JSRecordEditorPropertySpec);
	bR=JS_DefineFunctions(cx, jsoRecordEditor, _JSRecordEditorFunctionSpec);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, ajsoPlayer);
	JS_SetPrivate(cx,jsoRecordEditor,pSM);
	CreateSRecorderJSObject(cx, jsoRecordEditor);
	pSM->mjsoRecordEditor = jsoRecordEditor;
}

JSBool JSRecordEditorGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){ 
/*	if(JSVAL_IS_INT(id)){
		int iID=JSVAL_TO_INT(id);
		switch(iID){
		case JSPROP_name:
			// ??
			break;
		}
		// уведомляем слушателей
		char chEvID=iID-1;
	}*/
	return JS_TRUE; 
}

char GetRecordEditorEventID(const wchar_t* apwcEventName){
	if(isEqual(apwcEventName,L"onCutOperationComplete"))
		return (char)EVID_onCutOperationComplete;
	if(isEqual(apwcEventName,L"onCopyOperationComplete"))
		return (char)EVID_onCopyOperationComplete;
	if(isEqual(apwcEventName,L"onPasteOperationComplete"))
		return (char)EVID_onPasteOperationComplete;
	if(isEqual(apwcEventName,L"onDeleteOperationComplete"))
		return (char)EVID_onDeleteOperationComplete;
	if(isEqual(apwcEventName,L"onSaveOperationComplete"))
		return (char)EVID_onSaveOperationComplete;
	if(isEqual(apwcEventName,L"onSaveAsOperationComplete"))
		return (char)EVID_onSaveAsOperationComplete;
	if(isEqual(apwcEventName,L"onUndoOperationComplete"))
		return (char)EVID_onUndoOperationComplete;
	if(isEqual(apwcEventName,L"onRedoOperationComplete"))
		return (char)EVID_onRedoOperationComplete;
	if(isEqual(apwcEventName,L"onPlayOperationComplete"))
		return (char)EVID_onPlayOperationComplete;
	if(isEqual(apwcEventName,L"onPauseOperationComplete"))
		return (char)EVID_onPauseOperationComplete;
	if(isEqual(apwcEventName,L"onSeekOperationComplete"))
		return (char)EVID_onSeekOperationComplete;
	if(isEqual(apwcEventName,L"onRewindOperationComplete"))
		return (char)EVID_onRewindOperationComplete;
	if(isEqual(apwcEventName,L"onOpenOperationComplete"))
		return (char)EVID_onOpenOperationComplete;

	return -1;
}

wchar_t* GetRecordEditorEventName(char aidEvent){
	switch(aidEvent){
	case (char)EVID_onCutOperationComplete:
		return L"onCutOperationComplete";
	case (char)EVID_onCopyOperationComplete:
		return L"onCopyOperationComplete";
	case (char)EVID_onPasteOperationComplete:
		return L"onPasteOperationComplete";
	case (char)EVID_onDeleteOperationComplete:
		return L"onDeleteOperationComplete";
	case (char)EVID_onSaveOperationComplete:
		return L"onSaveOperationComplete";
	case (char)EVID_onSaveAsOperationComplete:
		return L"onSaveAsOperationComplete";
	case (char)EVID_onUndoOperationComplete:
		return L"onUndoOperationComplete";
	case (char)EVID_onRedoOperationComplete:
		return L"onRedoOperationComplete";
	case (char)EVID_onPlayOperationComplete:
		return L"onPlayOperationComplete";
	case (char)EVID_onPauseOperationComplete:
		return L"onPauseOperationComplete";
	case (char)EVID_onRewindOperationComplete:
		return L"onRewindOperationComplete";
	case (char)EVID_onSeekOperationComplete:
		return L"onSeekOperationComplete";
	case (char)EVID_onOpenOperationComplete:
		return L"onOpenOperationComplete";
	}
	return L"???";
}

bool mlSceneManager::addRecordEditorListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	char chEvID=aidEvent-1;
	if(chEvID >= ML_RECORD_EDITOR_EVENTS_COUNT) return false;
	v_elems* pv=&(maRecordEditorListeners[chEvID]);
	v_elems::const_iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl) return true;
	}
	pv->push_back(apMLEl);
	return true;
}

void mlSceneManager::NotifyRecordEditorListeners(char chEvID){
	if(chEvID < ML_RECORD_EDITOR_EVENTS_COUNT){
		v_elems* pv=&(maRecordEditorListeners[chEvID-1]);
		v_elems::const_iterator vi;
		for(vi=pv->begin(); vi != pv->end(); vi++ ){
			mlRMMLElement* pMLEl=*vi;
			pMLEl->EventNotify(chEvID,(mlRMMLElement*)GOID_RecordEditor);
		}
	}
}

bool mlSceneManager::removeRecordEditorListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	if(aidEvent==-1){
		// и удалим его из списка слушателей всех событий
		for(int ii=0; ii<ML_RECORD_EDITOR_EVENTS_COUNT; ii++){
			v_elems* pv=&maRecordEditorListeners[ii];
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
	v_elems* pv=&(maRecordEditorListeners[aidEvent-1]);
	v_elems::iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl){
			pv->erase(vi);
			return true;
		}
	}
	return true;
}

// результат выполнения операции Cut Редактора записей
void mlSceneManager::OnCutOperationComplete(int aErrorCode, unsigned int aRecordDuration){
	mlSynchData oData;
	oData << aErrorCode;
	oData << aRecordDuration;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onCutOperationComplete, (unsigned char*)oData.data(), oData.size());
}

// результат выполнения операции Copy Редактора записей
void mlSceneManager::OnCopyOperationComplete(int aErrorCode){
	mlSynchData oData;
	oData << aErrorCode;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onCopyOperationComplete, (unsigned char*)oData.data(), oData.size());
}

// результат выполнения операции Paste Редактора записей
void mlSceneManager::OnPasteOperationComplete(int aErrorCode, unsigned int aRecordDuration){
	mlSynchData oData;
	oData << aErrorCode;
	oData << aRecordDuration;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onPasteOperationComplete, (unsigned char*)oData.data(), oData.size());
}

// результат выполнения операции Delete Редактора записей
void mlSceneManager::OnDeleteOperationComplete(int aErrorCode, unsigned int aRecordDuration){
	mlSynchData oData;
	oData << aErrorCode;
	oData << aRecordDuration;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onDeleteOperationComplete, (unsigned char*)oData.data(), oData.size());
}

// результат выполнения операции Save Редактора записей
void mlSceneManager::OnSaveOperationComplete(int aErrorCode){
	mlSynchData oData;
	oData << aErrorCode;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onSaveOperationComplete, (unsigned char*)oData.data(), oData.size());
}

// результат выполнения операции SaveAs Редактора записей
void mlSceneManager::OnSaveAsOperationComplete(int aErrorCode){
	mlSynchData oData;
	oData << aErrorCode;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onSaveAsOperationComplete, (unsigned char*)oData.data(), oData.size());
}

// результат выполнения операции Play Редактора записей
void mlSceneManager::OnPlayOperationComplete(int aErrorCode){
	mlSynchData oData;
	oData << aErrorCode;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onPlayOperationComplete, (unsigned char*)oData.data(), oData.size());
}

// результат выполнения операции Pause Редактора записей
void mlSceneManager::OnPauseOperationComplete(int aErrorCode){
	mlSynchData oData;
	oData << aErrorCode;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onPauseOperationComplete, (unsigned char*)oData.data(), oData.size());
}

// результат выполнения операции Rewind Редактора записей
void mlSceneManager::OnRewindOperationComplete(int aErrorCode){
	mlSynchData oData;
	oData << aErrorCode;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onRewindOperationComplete, (unsigned char*)oData.data(), oData.size());
}

// результат выполнения операции Seek Редактора записей
void mlSceneManager::OnSeekOperationComplete(int aErrorCode){
	mlSynchData oData;
	oData << aErrorCode;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onSeekOperationComplete, (unsigned char*)oData.data(), oData.size());
}

// результат выполнения операции Open Редактора записей
void mlSceneManager::OnOpenOperationComplete( int aErrorCode, std::wstring aRecordName, std::wstring aRecordDesciption, unsigned int aRecordDuration, std::wstring aAuthor, std::wstring aCreationDate, std::wstring aLocation, std::wstring aLanguage, std::wstring aURL){
	mlSynchData oData;
	oData << aErrorCode;
	oData << aRecordName.c_str();
	oData << aRecordDesciption.c_str();
	oData << aRecordDuration;
	oData << aAuthor.c_str();
	oData << aCreationDate.c_str();
	oData << aLocation.c_str();
	oData << aLanguage.c_str();
	oData << aURL.c_str();
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onOpenOperationComplete, (unsigned char*)oData.data(), oData.size());
}

// результат выполнения операции Undo Редактора записей
void mlSceneManager::OnUndoOperationComplete(int aErrorCode, unsigned int aRecordDuration){
	mlSynchData oData;
	oData << aErrorCode;
	oData << aRecordDuration;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onUndoOperationComplete, (unsigned char*)oData.data(), oData.size());
}

// результат выполнения операции Redo Редактора записей
void mlSceneManager::OnRedoOperationComplete(int aErrorCode, unsigned int aRecordDuration){
	mlSynchData oData;
	oData << aErrorCode;
	oData << aRecordDuration;
	GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onRedoOperationComplete, (unsigned char*)oData.data(), oData.size());
}

void mlSceneManager::OnCutOperationCompleteInternal(mlSynchData& aData){
	int aErrorCode = 0;
	unsigned int aRecordDuration = 0;
	aData >> aErrorCode;
	aData >> aRecordDuration;
	jsval jsvErrorCode = INT_TO_JSVAL(aErrorCode);
	jsval jsvRecordDuration = INT_TO_JSVAL(aRecordDuration);

	JS_SetProperty(cx, GetEventGO(), "errorCode", &jsvErrorCode);
	JS_SetProperty(cx, GetEventGO(), "recordDuration", &jsvRecordDuration);
	NotifyRecordEditorListeners(EVID_onCutOperationComplete);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
	JS_DeleteProperty(cx, GetEventGO(), "recordDuration");
}

void mlSceneManager::OnCopyOperationCompleteInternal(mlSynchData& aData){
	int aErrorCode = 0;
	aData >> aErrorCode;
	jsval jsv = INT_TO_JSVAL(aErrorCode);

	JS_SetProperty(cx, GetEventGO(), "errorCode", &jsv);
	NotifyRecordEditorListeners(EVID_onCopyOperationComplete);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
}

void mlSceneManager::OnPasteOperationCompleteInternal(mlSynchData& aData){
	int aErrorCode = 0;
	unsigned int aRecordDuration = 0;
	aData >> aErrorCode;
	aData >> aRecordDuration;
	jsval jsvErrorCode = INT_TO_JSVAL(aErrorCode);
	jsval jsvRecordDuration = INT_TO_JSVAL(aRecordDuration);

	JS_SetProperty(cx, GetEventGO(), "errorCode", &jsvErrorCode);
	JS_SetProperty(cx, GetEventGO(), "recordDuration", &jsvRecordDuration);
	NotifyRecordEditorListeners(EVID_onPasteOperationComplete);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
	JS_DeleteProperty(cx, GetEventGO(), "recordDuration");
}

void mlSceneManager::OnDeleteOperationCompleteInternal(mlSynchData& aData){
	int aErrorCode = 0;
	unsigned int aRecordDuration = 0;
	aData >> aErrorCode;
	aData >> aRecordDuration;
	jsval jsvErrorCode = INT_TO_JSVAL(aErrorCode);
	jsval jsvRecordDuration = INT_TO_JSVAL(aRecordDuration);

	JS_SetProperty(cx, GetEventGO(), "errorCode", &jsvErrorCode);
	JS_SetProperty(cx, GetEventGO(), "recordDuration", &jsvRecordDuration);
	NotifyRecordEditorListeners(EVID_onDeleteOperationComplete);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
	JS_DeleteProperty(cx, GetEventGO(), "recordDuration");
}

void mlSceneManager::OnUndoOperationCompleteInternal(mlSynchData& aData){
	int aErrorCode = 0;
	int aRecordDuration = 0;

	aData >> aErrorCode;
	jsval jsvErrorCode = INT_TO_JSVAL(aErrorCode);
	aData >> aRecordDuration;
	jsval jsvDuration = INT_TO_JSVAL(aRecordDuration);

	JS_SetProperty(cx, GetEventGO(), "errorCode", &jsvErrorCode);
	JS_SetProperty(cx, GetEventGO(), "recordDuration", &jsvDuration);
	NotifyRecordEditorListeners(EVID_onUndoOperationComplete);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
	JS_DeleteProperty(cx, GetEventGO(), "recordDuration");
}

void mlSceneManager::OnRedoOperationCompleteInternal(mlSynchData& aData){
	int aErrorCode = 0;
	int aRecordDuration = 0;

	aData >> aErrorCode;
	jsval jsvErrorCode = INT_TO_JSVAL(aErrorCode);
	aData >> aRecordDuration;
	jsval jsvDuration = INT_TO_JSVAL(aRecordDuration);

	JS_SetProperty(cx, GetEventGO(), "errorCode", &jsvErrorCode);
	JS_SetProperty(cx, GetEventGO(), "recordDuration", &jsvDuration);
	NotifyRecordEditorListeners(EVID_onRedoOperationComplete);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
	JS_DeleteProperty(cx, GetEventGO(), "recordDuration");
}

void mlSceneManager::OnSaveOperationCompleteInternal(mlSynchData& aData){
	int aErrorCode = 0;
	aData >> aErrorCode;
	jsval jsv = INT_TO_JSVAL(aErrorCode);

	JS_SetProperty(cx, GetEventGO(), "errorCode", &jsv);
	NotifyRecordEditorListeners(EVID_onSaveOperationComplete);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
}

void mlSceneManager::OnSaveAsOperationCompleteInternal(mlSynchData& aData){
	int aErrorCode = 0;
	aData >> aErrorCode;
	jsval jsv = INT_TO_JSVAL(aErrorCode);

	JS_SetProperty(cx, GetEventGO(), "errorCode", &jsv);
	NotifyRecordEditorListeners(EVID_onSaveAsOperationComplete);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
}

void mlSceneManager::OnPlayOperationCompleteInternal(mlSynchData& aData){
	int aErrorCode = 0;
	aData >> aErrorCode;
	jsval jsv = INT_TO_JSVAL(aErrorCode);

	JS_SetProperty(cx, GetEventGO(), "errorCode", &jsv);
	NotifyRecordEditorListeners(EVID_onPlayOperationComplete);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
}

void mlSceneManager::OnPauseOperationCompleteInternal(mlSynchData& aData){
	int aErrorCode = 0;
	aData >> aErrorCode;
	jsval jsv = INT_TO_JSVAL(aErrorCode);

	JS_SetProperty(cx, GetEventGO(), "errorCode", &jsv);
	NotifyRecordEditorListeners(EVID_onPauseOperationComplete);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
}

void mlSceneManager::OnRewindOperationCompleteInternal(mlSynchData& aData){
	int aErrorCode = 0;
	aData >> aErrorCode;
	jsval jsv = INT_TO_JSVAL(aErrorCode);

	JS_SetProperty(cx, GetEventGO(), "errorCode", &jsv);
	NotifyRecordEditorListeners(EVID_onRewindOperationComplete);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
}

void mlSceneManager::OnSeekOperationCompleteInternal(mlSynchData& aData){
	int aErrorCode = 0;
	aData >> aErrorCode;
	jsval jsv = INT_TO_JSVAL(aErrorCode);

	JS_SetProperty(cx, GetEventGO(), "errorCode", &jsv);
	NotifyRecordEditorListeners(EVID_onSeekOperationComplete);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
}


void mlSceneManager::OnOpenOperationCompleteInternal(mlSynchData& aData){
	int iErrorCode = 0;
	aData >> iErrorCode;
	jsval jsvErrorCode = INT_TO_JSVAL(iErrorCode);
	JS_SetProperty(cx, GetEventGO(), "errorCode", &jsvErrorCode);

	wchar_t* sRecordName;
	aData >> sRecordName;
	jsval jsvRecordName = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, sRecordName));
	JS_SetProperty(cx, GetEventGO(), "recordName", &jsvRecordName);

	wchar_t* sRecordDescription;
	aData >> sRecordDescription;
	jsval jsvRecordDescription = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, sRecordDescription));
	JS_SetProperty(cx, GetEventGO(), "recordDescription", &jsvRecordDescription);

	int iRecordDuration = 0;
	aData >> iRecordDuration;
	jsval jsvRecordDuration = INT_TO_JSVAL(iRecordDuration);
	JS_SetProperty(cx, GetEventGO(), "recordDuration", &jsvRecordDuration);

	wchar_t* sAuthor;
	aData >> sAuthor;
	jsval jsvAuthor = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, sAuthor));
	JS_SetProperty(cx, GetEventGO(), "recordAuthor", &jsvAuthor);

	wchar_t* sCreationDate;
	aData >> sCreationDate;
	jsval jsvCreationDate = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, sCreationDate));
	JS_SetProperty(cx, GetEventGO(), "recordCreationDate", &jsvCreationDate);

	wchar_t* sLocation;
	aData >> sLocation;
	jsval jsvLocation = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, sLocation));
	JS_SetProperty(cx, GetEventGO(), "recordLocation", &jsvLocation);

	wchar_t* sLanguage;
	aData >> sLanguage;
	jsval jsvLanguage = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, sLanguage));
	JS_SetProperty(cx, GetEventGO(), "recordLanguage", &jsvLanguage);

	wchar_t* sURL;
	aData >> sURL;
	jsval jsvURL = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(cx, sURL));
	JS_SetProperty(cx, GetEventGO(), "recordURL", &jsvURL);

	NotifyRecordEditorListeners(EVID_onOpenOperationComplete);
	JS_DeleteProperty(cx, GetEventGO(), "errorCode");
	JS_DeleteProperty(cx, GetEventGO(), "recordName");
	JS_DeleteProperty(cx, GetEventGO(), "recordDescription");
	JS_DeleteProperty(cx, GetEventGO(), "recordDuration");
	JS_DeleteProperty(cx, GetEventGO(), "recordAuthor");
	JS_DeleteProperty(cx, GetEventGO(), "recordCreationDate");
	JS_DeleteProperty(cx, GetEventGO(), "recordLocation");
	JS_DeleteProperty(cx, GetEventGO(), "recordLanguage");
	JS_DeleteProperty(cx, GetEventGO(), "recordURL");
}

JSBool JSFUNC_cutOperation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_TRUE;
	if(argc != 2){
		JS_ReportError(cx, "cut operation can't has 2 parameters - start and end time marker needed.");
		return JS_TRUE;
	}
	double jsStartMarker;
	double jsEndMarker;

	if(JS_ValueToNumber(cx, argv[0], &jsStartMarker)==JS_FALSE){ 
		JS_ReportError(cx, "cut operation - 1 parameter need type number.");
		return JS_TRUE;
	}

	if(JS_ValueToNumber(cx, argv[1], &jsEndMarker)==JS_FALSE){ 
		JS_ReportError(cx, "cut operation - 2 parameter need type number.");
		return JS_TRUE;
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->GetContext()->mpSyncMan->RecordEditorCut((unsigned int)jsStartMarker, (unsigned int)jsEndMarker);

	return JS_TRUE;
}

JSBool JSFUNC_copyOperation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_TRUE;
	if(argc != 2){
		JS_ReportError(cx, "copy operation can't has 2 parameters - start and end time marker needed.");
		return JS_TRUE;
	}

	jsdouble jsStartMarker;
	double jsEndMarker;

	if(JS_ValueToNumber(cx, argv[0], &jsStartMarker)==JS_FALSE){ 
		JS_ReportError(cx, "cut operation - 1 parameter need type number.");
		return JS_TRUE;
	}

	if(JS_ValueToNumber(cx, argv[1], &jsEndMarker)==JS_FALSE){ 
		JS_ReportError(cx, "cut operation - 2 parameter need type number.");
		return JS_TRUE;
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->GetContext()->mpSyncMan->RecordEditorCopy((unsigned int)jsStartMarker, (unsigned int)jsEndMarker);

	return JS_TRUE;
}

JSBool JSFUNC_pasteOperation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_TRUE;
	if(argc != 1){
		JS_ReportError(cx, "paste operation can't has 1 parameters - position marker to paste needed.");
		return JS_TRUE;
	}

	jsdouble jsMarker;

	if(JS_ValueToNumber(cx, argv[0], &jsMarker)==JS_FALSE){ 
		JS_ReportError(cx, "paste operation - parameter need type number.");
		return JS_TRUE;
	}


	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->GetContext()->mpSyncMan->RecordEditorPaste((unsigned int)jsMarker);

	return JS_TRUE;
}

JSBool JSFUNC_deleteOperation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_TRUE;
	if(argc != 2){
		JS_ReportError(cx, "delete operation can't has 2 parameters - start and end time marker needed.");
		return JS_TRUE;
	}

	double jsStartMarker;
	double jsEndMarker;

	if(JS_ValueToNumber(cx, argv[0], &jsStartMarker)==JS_FALSE){ 
		JS_ReportError(cx, "delete operation - 1 parameter need type number.");
		return JS_TRUE;
	}

	if(JS_ValueToNumber(cx, argv[1], &jsEndMarker)==JS_FALSE){ 
		JS_ReportError(cx, "delete operation - 2 parameter need type number.");
		return JS_TRUE;
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->GetContext()->mpSyncMan->RecordEditorDelete((unsigned int)jsStartMarker, (unsigned int)jsEndMarker);

	return JS_TRUE;
}

JSBool JSFUNC_undoOperation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_TRUE;
	if(argc != 0){
		JS_ReportError(cx, "cut operation can't has 0 parameters.");
		return JS_TRUE;
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->GetContext()->mpSyncMan->RecordEditorUndo();

	return JS_TRUE;
}

JSBool JSFUNC_redoOperation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_TRUE;
	if(argc != 0){
		JS_ReportError(cx, "redo operation need has 0 parameters.");
		return JS_TRUE;
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->GetContext()->mpSyncMan->RecordEditorRedo();

	return JS_TRUE;
}

JSBool JSFUNC_openRecord(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_TRUE;
	if(argc != 1){
		JS_ReportError(cx, "open record function need has 1 parameter.");
		return JS_TRUE;
	}

	double jsRecordID;

	if(JS_ValueToNumber(cx, argv[0], &jsRecordID) == JS_FALSE){ 
		JS_ReportError(cx, "open record - First parameter need type number.");
		return JS_TRUE;
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->GetContext()->mpSyncMan->RecordEditorOpen((unsigned int)jsRecordID);

	return JS_TRUE;
}

JSBool JSFUNC_saveRecord(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_TRUE;
	if(argc != 0){
		JS_ReportError(cx, "cut operation need has 0 parameters.");
		return JS_TRUE;
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->GetContext()->mpSyncMan->RecordEditorSave();

	return JS_TRUE;
}

JSBool JSFUNC_saveAsRecord(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_TRUE;
	if(argc != 4){
		JS_ReportError(cx, "save as record operation can't has 3 parameters - new name of record, record descriptiin, overwrite flag and publicate flag.");
		return JS_TRUE;
	}

	if(!JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx, "saveAsRecord method must get First string parameter");
		return JS_TRUE;
	}

	if(!JSVAL_IS_STRING(argv[1])){
		JS_ReportError(cx, "saveAsRecord method must get Second string parameter");
		return JS_TRUE;
	}

	if(!JSVAL_IS_BOOLEAN(argv[2])){
		JS_ReportError(cx, "saveAsRecord method must get Third boolean parameter");
		return JS_TRUE;
	}

	if(!JSVAL_IS_BOOLEAN(argv[3])){
		JS_ReportError(cx, "saveAsRecord method must get Four boolean parameter");
		return JS_TRUE;
	}

	wchar_t* jsName = (wchar_t*)JS_GetStringChars(JSVAL_TO_STRING(argv[0]));
	wchar_t* jsDescription = (wchar_t*)JS_GetStringChars(JSVAL_TO_STRING(argv[1]));
	bool overwrite = false;
	ML_JSVAL_TO_BOOL(overwrite, argv[2]);
	bool publicate = false;
	ML_JSVAL_TO_BOOL(publicate, argv[3]);

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->GetContext()->mpSyncMan->RecordEditorSaveAs(jsName, jsDescription, overwrite, publicate);

	return JS_TRUE;
}


JSBool JSFUNC_playOperation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_TRUE;
	if(argc != 0){
		JS_ReportError(cx, "play operation has 0 parameters.");
		return JS_TRUE;
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->GetContext()->mpSyncMan->RecordEditorPlay();

	return JS_TRUE;
}

JSBool JSFUNC_pauseOperation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_TRUE;
	if(argc != 0){
		JS_ReportError(cx, "play operation has 0 parameters.");
		return JS_TRUE;
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->GetContext()->mpSyncMan->RecordEditorPause();

	return JS_TRUE;
}

JSBool JSFUNC_rewindOperation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_TRUE;
	if(argc != 0){
		JS_ReportError(cx, "play operation has 0 parameters.");
		return JS_TRUE;
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->GetContext()->mpSyncMan->RecordEditorRewind();

	return JS_TRUE;
}

JSBool JSFUNC_seekOperation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	*rval = JSVAL_TRUE;
	if(argc != 1){
		JS_ReportError(cx, "seek operation has 1 parameters - seek position.");
		return JS_TRUE;
	}

	double jsSeekPos;

	if(JS_ValueToNumber(cx, argv[0], &jsSeekPos)==JS_FALSE){ 
		JS_ReportError(cx, "seek operation - First parameter need type number.");
		return JS_TRUE;
	}

	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);
	pSM->GetContext()->mpSyncMan->RecordEditorSeek( (unsigned int)jsSeekPos);

	return JS_TRUE;
}


}
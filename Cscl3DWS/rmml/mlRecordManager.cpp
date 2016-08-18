#include "mlRMML.h"	// for precompile headers
#include "config/oms_config.h"
#include "mlRecordManager.h"
#include "mlRecorderEvents.h"

namespace rmml {

mlWorldEventManager::mlWorldEventManager(mlSceneManager* apSM)
{
	mpSM = apSM;
	m_currentRecordingID = INCORRECT_RECORD_ID;
	m_currentEventID = INCORRECT_EVENT_ID;
	m_isMyRecord = false;
	m_isInRecording = false;
}

mlWorldEventManager::~mlWorldEventManager()
{

}

//////////////////////////////////////////////////////////////////////////////////////////////
// ѕолучение событий
//////////////////////////////////////////////////////////////////////////////////////////////

void mlWorldEventManager::onEventLeaved(unsigned int auiPlayingSeanceID,unsigned int auiPlayingRecordID)
{
	mlSynchData data;
	data << auiPlayingSeanceID;
	data << auiPlayingRecordID;

	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_eventLeaved, (unsigned char*)data.data(), data.size() );
}

void mlWorldEventManager::onEventUserRightsInfo( unsigned int aEventID, const wchar_t *aEventUserRights)
{
	mlSynchData oData;
	oData << aEventID;
	oData << aEventUserRights;
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onEventUserRightsInfo, (unsigned char*)oData.data(), oData.size());
}

void mlWorldEventManager::onEventUserRightUpdate( unsigned int aEventID, const wchar_t *aEventUserRight)
{
	mlSynchData oData;
	oData << aEventID;
	oData << aEventUserRight;
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onEventUserRightUpdate, (unsigned char*)oData.data(), oData.size());
}

void mlWorldEventManager::onEventUserBanUpdate( const wchar_t *aEventName, const wchar_t *aEventUrl, char aBan)
{
	mlSynchData oData;
	oData << aEventName;	
	oData << aBan;
	oData << aEventUrl;
	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_onEventUserBanUpdate, (unsigned char*)oData.data(), oData.size());
}

void mlWorldEventManager::onEventEntered(unsigned int auiEventId, const wchar_t* awcLocationID, unsigned int auiEventRealityId, unsigned int auiPlayingRecordId, unsigned int auiBaseRecordId, unsigned int auiEventRecordId, bool abIsEventRecording, const wchar_t* aJSONDescription)
{
	mlSynchData data;
	data << auiEventId;
	data << awcLocationID;
	data << auiEventRealityId;
	data << auiPlayingRecordId;
	data << auiBaseRecordId;
	data << auiEventRecordId;
	data << abIsEventRecording;
	data << aJSONDescription;

	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_eventEntered, (unsigned char*)data.data(), data.size() );
}

void mlWorldEventManager::onLocationEntered( const wchar_t* awcLocationID, const wchar_t* aJSONDescription)
{
	mlSynchData data;
	data << awcLocationID;
	data << aJSONDescription;

	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_locationEntered, (unsigned char*)data.data(), data.size() );
}

void mlWorldEventManager::onEventUpdateInfo(unsigned int auiEventId, const wchar_t* awcLocationID, unsigned int auiEventRealityId, unsigned int auiPlayingRecordId, const wchar_t* aJSONDescription)
{
	mlSynchData data;
	data << auiEventId;
	data << awcLocationID;
	data << auiEventRealityId;
	data << auiPlayingRecordId;
	data << aJSONDescription;

	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_eventUpdateInfo, (unsigned char*)data.data(), data.size() );
}

// запись лога на сервере реально начата 
void mlWorldEventManager::onRecordConnected(unsigned int aiRecordID, const wchar_t* aAuthor, unsigned char isAuthor, unsigned int aDuration, const wchar_t* aLocationName)
{
	mlSynchData oData;
	oData << aiRecordID;
	oData << aAuthor;
	oData << isAuthor;
	oData << aDuration;
	oData << aLocationName;

	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_ConnectRecordInLocation, (unsigned char*)oData.data(), oData.size());
}

// запись лога на сервере в текущей локации остановлена
void mlWorldEventManager::onRecordDisconnected(unsigned int auRecordID, bool isAuthor, bool isNeedToAsk)
{
	mlSynchData recordData;
	recordData<<auRecordID;
	recordData<<isAuthor;
	recordData<<isNeedToAsk;

	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_DisconnectRecordInLocation, (unsigned char*)recordData.data(), recordData.size());
}


// запись лога на сервере реально начата 
void mlWorldEventManager::onRecordStartFailed(int iErrorCode, const wchar_t* aAdditionalInfo) 
{
	mlSynchData oData;
	oData<<iErrorCode;
	oData<<aAdditionalInfo;

	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_RecordStartFailed, (unsigned char*)oData.data(), oData.size());
}

void mlWorldEventManager::onRecordStoped(unsigned int auRecordID, unsigned int auDuration, syncRecordInfo* appRecordInfo)
{
	mlSynchData recordData;
	recordData<<auRecordID;
	recordData<<auDuration;
	if (appRecordInfo!=NULL)
	{
		recordData<<appRecordInfo->mpwcName.c_str();
		recordData<<appRecordInfo->mpwcAuthor.c_str();
		recordData<<appRecordInfo->muDepth;
		recordData<<appRecordInfo->mpwcDescription.c_str();
		recordData<<appRecordInfo->mpwcTags.c_str();
	}

	mpSM->GetContext()->mpInput->AddCustomEvent(mlSceneManager::CEID_StopRecordInLocation, (unsigned char*)recordData.data(), recordData.size());
}

// возникла ошибка при записи лога
void mlWorldEventManager::onRecordError(int aiErrorCode){
	// ??
}

//////////////////////////////////////////////////////////////////////////////////////////////
// ќбработка событий
//////////////////////////////////////////////////////////////////////////////////////////////

void mlWorldEventManager::onRecordStartFailedInternal(mlSynchData& aData)
{
	int iErrorCode = 0;
	aData >> iErrorCode;
	wchar_t* apwcAdditionalInfo = NULL;
	aData >> apwcAdditionalInfo;

	bool infoEvaluated = false;
	mlString sDescription(L"[");
	sDescription = sDescription + apwcAdditionalInfo+ L"]";
	unsigned int uLength = sDescription.size();
	JSContext *mcx = (mpSM->GetJSContext());
	jsval v;
	JSBool bR = JS_EvaluateUCScript(mcx, JS_GetGlobalObject(mcx), (const jschar*)sDescription.c_str(), uLength, "previousEventInfo", 1, &v);
	if (JSVAL_IS_REAL_OBJECT(v))
	{	
		JSObject *obj=JSVAL_TO_OBJECT(v);
		JS_GetElement(mcx,obj,0,&v);
		if (JSVAL_IS_REAL_OBJECT(v))
		{
			JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "previousEventInfo", &v);
			infoEvaluated = true;
		}
	}

	jsval jsv = INT_TO_JSVAL(iErrorCode);
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "errorCode", &jsv);

	mpSM->NotifyRecorderListeners(EVID_onRecordInLocationFailed);

	JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "errorCode");

	if( infoEvaluated)
		JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "previousEventInfo");
}

void mlWorldEventManager::onRecordStopedInternal(mlSynchData& aData)
{
	unsigned int recordID;
	unsigned int duration;
	unsigned int depth;
	wchar_t* sParam = L"";

	aData>>recordID;
	aData>>duration;
	aData>>sParam;
	mlString sRecordName(sParam);	
	aData>>sParam;
	mlString sAuthor(sParam);	
	aData>>depth;
	aData>>sParam;
	mlString sDescription(sParam);
	aData>>sParam;
	mlString sTags(sParam);

	jsval jsv = INT_TO_JSVAL(duration);	
	JSContext *mcx = (mpSM->GetJSContext());
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "duration", &jsv);

	jsv = INT_TO_JSVAL(recordID);	
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "recordID", &jsv);

	syncRecordInfo recInfo;
	recInfo.SetID(recordID);
	recInfo.SetName(sRecordName.c_str());
	recInfo.SetAuthor(sAuthor.c_str());
	recInfo.SetDepth(depth);
	recInfo.mpwcDescription = sDescription.c_str();
	recInfo.mpwcTags = sTags.c_str();

	JSObject* jsoLogRecord = mlLogRecordJSO::newJSObject(mcx);
	mlLogRecordJSO* pLogRecord = (mlLogRecordJSO*)JS_GetPrivate(mcx, jsoLogRecord);
	mlLogRecordInfo aLogRecordInfo(&recInfo);
	pLogRecord->Init(aLogRecordInfo);

	// про остановку поговорить в четверг с лешей или сашей
	if( mpSM->GetContext()->mpVoipMan)
		mpSM->GetContext()->mpVoipMan->StopRecording(recordID);

	if( mpSM->GetContext()->mpPreviewRecorder)
		mpSM->GetContext()->mpPreviewRecorder->StopRecording(recordID);

	if( mpSM->GetContext()->mpSharingMan)
		mpSM->GetContext()->mpSharingMan->StopRecording(recordID);

	jsval vRecord = OBJECT_TO_JSVAL(jsoLogRecord);
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "record", &vRecord);

	//mpSM->GetContext()->mpRM->SetPlayingEnabled(SND_BACK_MUSIC, true);

	mpSM->NotifyRecorderListeners(EVID_onRecordStoped);
	JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "duration");
	JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "recordID");	
}

void mlWorldEventManager::onRecordDisconnectInternal(mlSynchData& aData)
{
	if( !m_isInRecording)
		return;

	unsigned int recordID;
	bool isAuthor;
	bool isNeedToAsk;
	wchar_t* sParam = L"";

	aData>>recordID;
	aData>>isAuthor;
	aData>>isNeedToAsk;

	JSContext *mcx = (mpSM->GetJSContext());
	jsval jsvAsk = BOOLEAN_TO_JSVAL(isNeedToAsk);
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "needToAsk", &jsvAsk);

	jsval jsvAuthor = BOOLEAN_TO_JSVAL(isAuthor);
	JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "author", &jsvAuthor);

	// про остановку поговорить в четверг с лешей или сашей
	if( isAuthor)
	{
		// ??
		// ќстановку записи перенес в onRecordStopedInternal
		if( mpSM->GetContext()->mpVoipMan)
			mpSM->GetContext()->mpVoipMan->StopListenRecording( recordID);
	}
	else
	{
		if( mpSM->GetContext()->mpVoipMan)
			mpSM->GetContext()->mpVoipMan->StopListenRecording( recordID);
		if( mpSM->GetContext()->mpSharingMan)
			mpSM->GetContext()->mpSharingMan->StopListenRecording( recordID);
		if( mpSM->GetContext()->mpPreviewRecorder)
			mpSM->GetContext()->mpPreviewRecorder->StopStatisticRecording();
	}

	//mpSM->GetContext()->mpRM->SetPlayingEnabled(SND_BACK_MUSIC, true);

	mpSM->NotifyRecorderListeners(EVID_onRecordDisconnected);
	JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "needToAsk");
	JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "author");

	m_isInRecording = false;
	m_isMyRecord = false;
	m_currentRecordingID = INCORRECT_RECORD_ID;
}

void mlWorldEventManager::onEventEnteredInternal(mlSynchData& aData)
{
	unsigned int eventID = 0;
	unsigned int eventRealityId = 0;
	unsigned int playingRecordID = 0;
	unsigned int baseRecordID = 0;
	unsigned int eventRecordID = 0;
	wchar_t * apwsLocationID = NULL;
	std::wstring wsLocationID;
	wchar_t * sJSONDescription = NULL;
	bool isEventRecording = false;

	aData >> eventID;
	aData >> apwsLocationID;
	wsLocationID = apwsLocationID;
	aData >> eventRealityId;
	aData >> playingRecordID;
	aData >> baseRecordID;
	aData >> eventRecordID;
	aData >> isEventRecording;
	aData >> sJSONDescription;


	// ≈сли сервер прислал вход в новое событие, до которого не было прислано событие выхода.
	// Ёто может быть только в случае разрыва св€зи, при котором клиент оказываетс€ в другом событии.
	if( m_isInRecording)
	{
		if(!isEventRecording || m_currentRecordingID != eventRecordID)
		{
			LeaveCurrentRecording();
		}
	}
	if( eventID != INCORRECT_EVENT_ID && m_currentEventID != eventID)
	{
		LeaveCurrentEvent();
	}

	m_currentEventID = eventID;

	mpSM->mVoipCallbackManager->onEventEntered(eventRealityId, playingRecordID, baseRecordID, wsLocationID.c_str());
	mpSM->mDesktopCallbackManager->onEventEntered(eventRealityId, playingRecordID, baseRecordID);

	mlString sDescription(L"[");
	sDescription = sDescription + sJSONDescription+ L"]";

	unsigned int uLength = sDescription.size();
	JSContext *mcx = (mpSM->GetJSContext());
	jsval v;
	JSBool bR = JS_EvaluateUCScript(mcx, JS_GetGlobalObject(mcx), (const jschar*)sDescription.c_str(), uLength, "eventEnteredDescription", 1, &v);
	if (JSVAL_IS_REAL_OBJECT(v))
	{
		JSObject *obj=JSVAL_TO_OBJECT(v);
		JS_GetElement(mcx,obj,0,&v);
		if (JSVAL_IS_REAL_OBJECT(v))
		{
			mpSM->SetCurrentEventID(eventID);
			mpSM->SetPlayingRecordID(playingRecordID);
			JSString *jssLoc = JS_NewUCStringCopyZ(mcx, (const jschar*)wsLocationID.c_str());
			jsval vLoc = STRING_TO_JSVAL(jssLoc);
			JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "eventLocation", &vLoc);
			JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "eventDescription", &v);
			mpSM->NotifyRecorderListeners(EVID_onEventEntered);
			JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "eventDescription");
			JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "eventLocation");
		}
	}
}

void mlWorldEventManager::onEventUpdateInfoInternal(mlSynchData& aData)
{
	unsigned int eventID = 0;
	unsigned int eventRealityId = 0;
	unsigned int recordPlayingID = 0;
	wchar_t * apwsLocationID = NULL;
	std::wstring wsLocationID;
	wchar_t * sJSONDescription = NULL;
	aData >> eventID;
	aData >> apwsLocationID;
	wsLocationID = apwsLocationID;
	aData >> eventRealityId;
	aData >> recordPlayingID;
	aData >> sJSONDescription;

	mlString sDescription(L"[");
	sDescription = sDescription + sJSONDescription+ L"]";

	unsigned int uLength = sDescription.size();
	JSContext *mcx = (mpSM->GetJSContext());
	jsval v;
	JSBool bR = JS_EvaluateUCScript(mcx, JS_GetGlobalObject(mcx), (const jschar*)sDescription.c_str(), uLength, "eventEnteredDescription", 1, &v);
	if (JSVAL_IS_REAL_OBJECT(v))
	{	
		JSObject *obj=JSVAL_TO_OBJECT(v);
		JS_GetElement(mcx,obj,0,&v);
		if (JSVAL_IS_REAL_OBJECT(v))
		{
			mpSM->SetCurrentEventID(eventID);
			mpSM->SetPlayingRecordID(recordPlayingID);
			JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "eventDescription", &v);
			mpSM->NotifyRecorderListeners(EVID_onEventUpdateInfo);
			JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "eventDescription");
		}
	}
}

// ≈сли мы только пришли в событие - записи быть не должно.
// »наче ситуаци€:
// - оборвалась св€зь с сервером мира, на нем запись остановилась. ѕотом св€зь восстановилась - а автор об этом не узнал. 
// ќн не сообщает голосовому и др. серверам об этом.  ак следствие - они продолжают писать запись.
void mlWorldEventManager::LeaveCurrentRecording()
{
	if( !m_isInRecording)
		return;

	mlSynchData recordData;
	recordData<<m_currentRecordingID;
	recordData<<m_isMyRecord;
	recordData<<false;
	onRecordDisconnectInternal( recordData);
}

// ≈сли сервер прислал вход в новое событие, до которого не было прислано событие выхода.
// Ёто может быть только в случае разрыва св€зи, при котором клиент оказываетс€ в другом событии.
void mlWorldEventManager::LeaveCurrentEvent()
{
	if( m_currentEventID == INCORRECT_EVENT_ID)
		return;

	mlSynchData data;
	data << m_currentEventID;
	data << m_currentRecordingID;
	onEventLeavedInternal( data);
}

void mlWorldEventManager::onEventLeavedInternal(mlSynchData& aData)
{
	if( m_currentEventID == INCORRECT_EVENT_ID)
		return;

	unsigned int eventID = 0;
	unsigned int recordPlayingID = 0;
	aData >> eventID;
	aData >> recordPlayingID;

	JSContext *cx = (mpSM->GetJSContext());

	jsval vEventID = INT_TO_JSVAL(eventID);
	JS_SetProperty(cx, mpSM->GetEventGO(), "eventID", &vEventID);

	mpSM->NotifyRecorderListeners(EVID_onEventLeaved);

	JS_DeleteProperty(cx, mpSM->GetEventGO(), "eventID");

	mpSM->mVoipCallbackManager->onEventLeaved();
	mpSM->mDesktopCallbackManager->onEventLeaved();

	mpSM->SetCurrentEventID( 0xFFFFFFFF);

	// ≈сли после восстановлени€ св€зи мы оказываемс€ вне событий, то должны остановить запись
	LeaveCurrentRecording();

	m_currentEventID = INCORRECT_EVENT_ID;
}

void mlWorldEventManager::onLocationEnteredInternal(mlSynchData& aData)
{
	std::wstring wsLocationID;
	wchar_t * pwsLocationID = NULL;
	wchar_t * pwsJSONDescription = NULL;

	aData >> pwsLocationID;
	wsLocationID = pwsLocationID;
	aData >> pwsJSONDescription;

	mlString sDescription(L"[");
	sDescription = sDescription + pwsJSONDescription+ L"]";

	unsigned int uLength = sDescription.size();
	JSContext *mcx = (mpSM->GetJSContext());
	jsval v;
	JSBool bR = JS_EvaluateUCScript(mcx, JS_GetGlobalObject(mcx), (const jschar*)sDescription.c_str(), uLength, "locationEnteredDescription", 1, &v);
	if (JSVAL_IS_REAL_OBJECT(v))
	{
		JSObject *obj=JSVAL_TO_OBJECT(v);
		JS_GetElement(mcx,obj,0,&v);
		if (JSVAL_IS_REAL_OBJECT(v))
		{
			JSString *jssLoc = JS_NewUCStringCopyZ(mcx, (const jschar*)wsLocationID.c_str());
			jsval vLoc = STRING_TO_JSVAL(jssLoc);
			JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "locationID", &vLoc);

			JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "eventParams", &v);

			mpSM->NotifyRecorderListeners(EVID_onLocationEntered);

			JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "eventParams");
			JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "locationID");
		}
	}
}

void mlWorldEventManager::onEventUserRightsInfoReceivedInternal(mlSynchData& aData)
{
	unsigned int eventID = 0;
	wchar_t * sJSONDescription = NULL;
	aData >> eventID;
	aData >> sJSONDescription;

	mlString sDescription(L"[");
	sDescription = sDescription + sJSONDescription+ L"]";

	unsigned int uLength = sDescription.size();
	JSContext *mcx = (mpSM->GetJSContext());
	jsval v;
	JSBool bR = JS_EvaluateUCScript(mcx, JS_GetGlobalObject(mcx), (const jschar*)sDescription.c_str(), uLength, "eventUserRightsInfoDescription", 1, &v);
	if (JSVAL_IS_REAL_OBJECT(v))
	{	
		JSObject *obj=JSVAL_TO_OBJECT(v);
		JS_GetElement(mcx,obj,0,&v);
		if (JSVAL_IS_REAL_OBJECT(v))
		{
			JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "rightsInfoDescription", &v);
			mpSM->NotifyRecorderListeners(EVID_onEventUserRightsInfo);		
			JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "rightsInfoDescription");
		}
	}
}

void mlWorldEventManager::onEventUserRightUpdateReceivedInternal(mlSynchData& aData)
{
	unsigned int eventID = 0;
	wchar_t * sJSONDescription = NULL;
	aData >> eventID;
	aData >> sJSONDescription;

	mlString sDescription(L"[");
	sDescription = sDescription + sJSONDescription+ L"]";

	unsigned int uLength = sDescription.size();
	JSContext *mcx = (mpSM->GetJSContext());
	jsval v;
	JSBool bR = JS_EvaluateUCScript(mcx, JS_GetGlobalObject(mcx), (const jschar*)sDescription.c_str(), uLength, "eventUserRightUpdateDescription", 1, &v);
	if (JSVAL_IS_REAL_OBJECT(v))
	{	
		JSObject *obj=JSVAL_TO_OBJECT(v);
		JS_GetElement(mcx,obj,0,&v);
		if (JSVAL_IS_REAL_OBJECT(v))
		{
			JS_SetProperty(mcx, GPSM(mcx)->GetEventGO(), "userRight", &v);
			mpSM->NotifyRecorderListeners(EVID_onEventUserRightUpdate);		
			JS_DeleteProperty(mcx, GPSM(mcx)->GetEventGO(), "userRight");
		}
	}
}

void mlWorldEventManager::onEventUserBanUpdateReceivedInternal(mlSynchData& aData)
{
	JSContext *mcx = mpSM->GetJSContext();	
	wchar_t *pEventName = L"";
	wchar_t *pEventUrl = L"";
	char ban = 0;
	aData >> pEventName;

	JSString* jssEventName = JS_NewUCStringCopyZ( mcx, (const jschar*)pEventName);
	jsval vEventName = STRING_TO_JSVAL( jssEventName);

	aData >> ban;	
	jsval vBan = INT_TO_JSVAL( ban);

	aData >> pEventUrl;	
	JSString* jssEventUrl = JS_NewUCStringCopyZ( mcx, (const jschar*)pEventUrl);
	jsval vEventUrl = STRING_TO_JSVAL( jssEventUrl);

	JS_SetProperty( mcx, GPSM(mcx)->GetEventGO(),"eventName",&vEventName);	
	JS_SetProperty( mcx, GPSM(mcx)->GetEventGO(),"ban",&vBan);
	JS_SetProperty( mcx, GPSM(mcx)->GetEventGO(),"unBanEventUrl",&vEventUrl);
	mpSM->NotifyRecorderListeners( EVID_onEventUserBanUpdate);
	JS_DeleteProperty( mcx, GPSM(mcx)->GetEventGO(), "unBanEventUrl");
	JS_DeleteProperty( mcx, GPSM(mcx)->GetEventGO(), "ban");	
	JS_DeleteProperty( mcx, GPSM(mcx)->GetEventGO(), "eventName");
}

void mlWorldEventManager::onRecordConnectedInternal(mlSynchData& aData)
{
	wchar_t* sParam = L"";
	unsigned int recordID;
	unsigned char isAuthor;
	aData >> recordID;
	aData >> sParam;
	mlString sAuthor(sParam);
	aData >> isAuthor;
	unsigned int aDuration;
	aData >> aDuration;
	aData >> sParam;
	mlString sLocationName(sParam);
	unsigned char isAssistant = 0;

	JSObject* jsoSRecorder = mpSM->GetSRecorderGO();
	JSContext *cx = (mpSM->GetJSContext());
	jsval vRecordID = JSVAL_NULL;
	vRecordID = INT_TO_JSVAL(recordID);
	JS_SetProperty(cx, jsoSRecorder, "recordID", &vRecordID);

	// ƒобавл€ем и посылаем Event
	JS_SetProperty(cx, mpSM->GetEventGO(), "recordID", &vRecordID);

	jsval vAuthor = JSVAL_NULL;
	JSString *jssAuthor = JS_NewUCStringCopyZ(cx,(const jschar*)sAuthor.c_str());
	vAuthor = STRING_TO_JSVAL(jssAuthor);
	JS_SetProperty(cx, mpSM->GetEventGO(), "author", &vAuthor);

	jsval vDuration = JSVAL_NULL;
	vDuration = INT_TO_JSVAL(aDuration / 1000); // в скрипт отдаем в секундах
	JS_SetProperty(cx, mpSM->GetEventGO(), "duration", &vDuration);

	jsval vIsAuthor = JSVAL_NULL;
	if( isAuthor == DEF_AUTHOR)
		vIsAuthor = BOOLEAN_TO_JSVAL( true);
	else
		vIsAuthor = BOOLEAN_TO_JSVAL( false);
	JS_SetProperty(cx, mpSM->GetEventGO(), "isAuthor", &vIsAuthor);

	jsval vIsAssistant = JSVAL_NULL;
	if( isAssistant == DEF_ASSISTANT)
		vIsAssistant = BOOLEAN_TO_JSVAL( true);
	else
		vIsAssistant = BOOLEAN_TO_JSVAL( false);
	JS_SetProperty(cx, mpSM->GetEventGO(), "isAssistant", &vIsAssistant);


	mpSM->NotifyRecorderListeners(EVID_onRecordConnected);

	JS_DeleteProperty((mpSM->GetJSContext()), mpSM->GetEventGO(), "author");
	JS_DeleteProperty((mpSM->GetJSContext()), mpSM->GetEventGO(), "duration");
	JS_DeleteProperty((mpSM->GetJSContext()), mpSM->GetEventGO(), "recordID");
	JS_DeleteProperty((mpSM->GetJSContext()), mpSM->GetEventGO(), "isAssistant");
	JS_DeleteProperty((mpSM->GetJSContext()), mpSM->GetEventGO(), "isAuthor");

	if( isAuthor == DEF_AUTHOR)
	{
		m_isMyRecord = true;
		if( mpSM->GetContext()->mpVoipMan)
		{	
			bool result = mpSM->GetContext()->mpVoipMan->StartRecording( recordID, cLPCSTR(sParam), mpSM->GetContext()->mpMapMan->GetCurrentRealityID());
			if (!result)
				mpSM->NotifyRecorderListeners(EVID_onRecordInLocationNoneSound);
		}
		if( mpSM->GetContext()->mpPreviewRecorder)
			mpSM->GetContext()->mpPreviewRecorder->StartRecording( recordID, aDuration);

		if( mpSM->GetContext()->mpSharingMan)
			mpSM->GetContext()->mpSharingMan->StartRecording( recordID, cLPCSTR(sParam));
	}
	else
	{
		m_isMyRecord = false;
		if( mpSM->GetContext()->mpVoipMan)
			mpSM->GetContext()->mpVoipMan->StartListenRecording( recordID);
		if( mpSM->GetContext()->mpSharingMan)
			mpSM->GetContext()->mpSharingMan->StartListenRecording( recordID, cLPCSTR(sParam));
		if( mpSM->GetContext()->mpPreviewRecorder)
			mpSM->GetContext()->mpPreviewRecorder->StartStatisticRecording( recordID, aDuration);
	}

	//mpSM->GetContext()->mpRM->SetPlayingEnabled(SND_BACK_MUSIC, false);
	m_isInRecording = true;
	m_currentRecordingID = recordID;
}

}
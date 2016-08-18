#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"
#include "mlRMMLJSPlVoice.h"
#include "ILogWriter.h"

namespace rmml {

/**
 * Реализация глобального JS-объекта "Voice"
 */
#define MICROPHONE_CONST_VALUE		1
#define MIXER_CONST_VALUE			2
#define VIRTUALCABLE_CONST_VALUE	3

JSBool JSVoiceGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool JSVoiceSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	
JSClass JSRMMLVoiceClass = {
	"_Voice", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, 
	JSVoiceGetProperty, JSVoiceSetProperty,
	JS_EnumerateStub, JS_ResolveStub, 
	JS_ConvertStub, JS_FinalizeStub,
	NULL, NULL, NULL,
	NULL, NULL, NULL
};

enum {
	JSPROP_useDirectSound=1,
	JSPROP_micGainLevel,
	JSPROP_micActivationLevel,
	JSPROP_recorderFreq,
	JSPROP_voiceQuality,
	JSPROP_inputLevel,
	JSPROP_inputLevelNormalized,
	JSPROP_voiceActivated,
	JSPROP_microphoneEnabled,
	JSPROP_autoVolume,
	JSPROP_audioSource,
	JSPROP_autoGainControlEnabled,
	JSPROP_denoisingEnabled,
	JSPROP_SOURCE_MICROPHONE,
	JSPROP_SOURCE_MIXER,
	JSPROP_SOURCE_VIRTUALCABLE,
	TIDE_mlRMMLVoice
};

enum {
	EVID_onConnectSuccess = 0,
	EVID_onConnectStart,
	EVID_onConnectFailed,
	EVID_onConnectLost,
	EVID_onDisconnected,
	EVID_onKicked,
	EVID_onAddRoom,
	EVID_onRemoveRoom,
	EVID_onUserTalking,
	EVID_onUserStoppedTalking,
	EVID_onUserFileSaved,
	EVID_onAudioFilePlayed,
	EVID_onRecordStarted,
	EVID_onRecordStopped,
	EVID_onSRModeStatusChanged,
	EVID_onNotified,
	EVID_onEvoClientWorkStateChanged,
	// новые асинхронная работа через пайпы
	EVID_onSetInputDevice,
	EVID_onSetOutputDevice,
	EVID_onGetInputDevices,
	EVID_onGetOutputDevices,
	EVID_onVoipDeviceErrorCode,
	EVID_onRecievedFindedMicName,
	EVID_onReInitializeVoipSystem,
	TEVIDE_mlRMMLVoice
};

JSPropertySpec _JSVoicePropertySpec[] = {
	JSPROP_RW(useDirectSound)		// 
	JSPROP_RW(micGainLevel)			// 
	JSPROP_RW(micActivationLevel)	// 
	JSPROP_RW(recorderFreq)			// 
	JSPROP_RW(voiceQuality)			// 
	JSPROP_RO(inputLevel)			// 
	JSPROP_RO(inputLevelNormalized)	// 
	JSPROP_RW(voiceActivated)		// 
	JSPROP_RW(microphoneEnabled)	// 
	JSPROP_RW(autoVolume)			// 
	JSPROP_RW(autoGainControlEnabled)			// 
	JSPROP_RW(denoisingEnabled)		// 
	JSPROP_RO(audioSource)		// 
	JSPROP_RO(SOURCE_MICROPHONE)	// 1 - источник микрофон
	JSPROP_RO(SOURCE_MIXER)			// 2 - источник микшер
	JSPROP_RO(SOURCE_VIRTUALCABLE)	// 3 - источник Virtual Cable 1
	{ 0, 0, 0, 0, 0 }
};

// Function declare
JSFUNC_DECL(restoreConnection)
JSFUNC_DECL(startRecording)
JSFUNC_DECL(stopRecording)
JSFUNC_DECL(removeRecord)
JSFUNC_DECL(getInputDevices)
JSFUNC_DECL(getOutputDevices)
JSFUNC_DECL(checkEnableCurrentDevice)
JSFUNC_DECL(setInputDeviceMode)
JSFUNC_DECL(setOutputDeviceMode)
JSFUNC_DECL(setInputMixerMode)
JSFUNC_DECL(isMixerSet)
//JSFUNC_DECL(setInputVirtualAudioCableMode)
JSFUNC_DECL(setUserVolume)
JSFUNC_DECL(setUserGainLevel)
// вход в режим прослушивания своего голоса
JSFUNC_DECL(srEnterMode)
// начало записи своего голоса
JSFUNC_DECL(srStartRecording)
// остановить запись своего голоса
JSFUNC_DECL(srStopRecording)
// воспроизвечти записанный голос
JSFUNC_DECL(srPlayRecording)
// выйти из режима прослушивания своего голоса
JSFUNC_DECL(srExitMode)
// общая громкость входящего звука пользователей
JSFUNC_DECL(setIncomingVoiceVolume)
// активация подавления эха
JSFUNC_DECL(setEchoCancellation)
// активация подавления эха для микшера
JSFUNC_DECL(setMixerEchoCancellation)
// включили режим поиска работающего микрофона
JSFUNC_DECL(startFindRealMicrophon)
// выключили режим поиска работающего микрофона
JSFUNC_DECL(endFindRealMicrophon)

///// JavaScript Function Table
JSFunctionSpec _JSVoiceFunctionSpec[] = {
	JSFUNC(restoreConnection,0)
	JSFUNC(startRecording,0)
	JSFUNC(stopRecording,0)
	JSFUNC(removeRecord,1)
	JSFUNC(getInputDevices,1)
	JSFUNC(getOutputDevices,1)
	JSFUNC(checkEnableCurrentDevice,0)
	JSFUNC(setInputDeviceMode,1)
	JSFUNC(setOutputDeviceMode,1)
	JSFUNC(setInputMixerMode,1)
	JSFUNC(isMixerSet,0)	
	//JSFUNC(setInputVirtualAudioCableMode,1)
	JSFUNC(setUserVolume,1)
	JSFUNC(setUserGainLevel,1)
	// вход в режим прослушивания своего голоса
	JSFUNC(srEnterMode,1)
	// начало записи своего голоса
	JSFUNC(srStartRecording,1)
	// остановить запись своего голоса
	JSFUNC(srStopRecording,1)
	// воспроизвечти записанный голос
	JSFUNC(srPlayRecording,1)
	// выйти из режима прослушивания своего голоса
	JSFUNC(srExitMode,1)
	// общая громкость входящего звука пользователей
	JSFUNC(setIncomingVoiceVolume, 1)
	// активация подавления эха
	JSFUNC(setEchoCancellation, 1)
	//активация подавления эха для микшера
	JSFUNC(setMixerEchoCancellation, 1)
	JSFUNC(startFindRealMicrophon,0)
	JSFUNC(endFindRealMicrophon,0)
	{ 0, 0, 0, 0, 0 }
};

void CreateVoiceJSObject(JSContext *cx, JSObject *ajsoPlayer){
    JSObject* jsoVoice = JS_DefineObject(cx, ajsoPlayer, GJSONM_VOICE, &JSRMMLVoiceClass, NULL, JSPROP_ENUMERATE | JSPROP_READONLY | JSPROP_PERMANENT);
	JSBool bR;
	bR=JS_DefineProperties(cx, jsoVoice, _JSVoicePropertySpec);
	bR=JS_DefineFunctions(cx, jsoVoice, _JSVoiceFunctionSpec);
	mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, ajsoPlayer);
	JS_SetPrivate(cx,jsoVoice,pSM);
	pSM->mjsoVoice = jsoVoice;
	//
}

char GetVoiceEventID(const wchar_t* apwcEventName){
	if(isEqual(apwcEventName,L"onConnectSuccess"))
		return (char)EVID_onConnectSuccess;
	if(isEqual(apwcEventName,L"onConnectStart"))
		return (char)EVID_onConnectStart;
	if(isEqual(apwcEventName,L"onConnectFailed"))
		return (char)EVID_onConnectFailed;
	if(isEqual(apwcEventName,L"onConnectLost"))
		return (char)EVID_onConnectLost;
	if(isEqual(apwcEventName,L"onDisconnected"))
		return (char)EVID_onDisconnected;
	if(isEqual(apwcEventName,L"onKicked"))
		return (char)EVID_onKicked;
	if(isEqual(apwcEventName,L"onAddRoom"))
		return (char)EVID_onAddRoom;
	if(isEqual(apwcEventName,L"onRemoveRoom"))
		return (char)EVID_onRemoveRoom;
	if(isEqual(apwcEventName,L"onUserTalking"))
		return (char)EVID_onUserTalking;
	if(isEqual(apwcEventName,L"onUserStoppedTalking"))
		return (char)EVID_onUserStoppedTalking;
	if(isEqual(apwcEventName,L"onUserFileSaved"))
		return (char)EVID_onUserFileSaved;
	if(isEqual(apwcEventName,L"onAudioFilePlayed"))
		return (char)EVID_onAudioFilePlayed;
	if(isEqual(apwcEventName,L"onRecordStarted"))
		return (char)EVID_onRecordStarted;
	if(isEqual(apwcEventName,L"onRecordStopped"))
		return (char)EVID_onRecordStopped;
	if(isEqual(apwcEventName,L"onSRModeStatusChanged"))
		return (char)EVID_onSRModeStatusChanged;
	if(isEqual(apwcEventName,L"onNotified"))
		return (char)EVID_onNotified;
	if(isEqual(apwcEventName,L"onSetInputDevice"))
		return (char)EVID_onSetInputDevice;
	if(isEqual(apwcEventName,L"onSetOutputDevice"))
		return (char)EVID_onSetOutputDevice;
	if(isEqual(apwcEventName,L"onGetInputDevices"))
		return (char)EVID_onGetInputDevices;
	if(isEqual(apwcEventName,L"onGetOutputDevices"))
		return (char)EVID_onGetOutputDevices;
	if(isEqual(apwcEventName,L"onEvoClientWorkStateChanged"))
		return (char)EVID_onEvoClientWorkStateChanged;
	if (isEqual(apwcEventName, L"onVoipDeviceErrorCode"))
		return (char)EVID_onVoipDeviceErrorCode;
	if (isEqual(apwcEventName, L"onReInitializeVoipSystem"))
		return (char)EVID_onReInitializeVoipSystem;
	if (isEqual(apwcEventName, L"onRecievedFindedMicName"))
		return (char)EVID_onRecievedFindedMicName;
	
	return -1;
}

wchar_t* GetVoiceEventName(char aidEvent){
	switch(aidEvent){
	case (char)EVID_onConnectSuccess:
		return L"onConnectSuccess";
	case (char)EVID_onConnectStart:
		return L"onConnectStart";
	case (char)EVID_onConnectFailed:
		return L"onConnectFailed";
	case (char)EVID_onConnectLost:
		return L"onConnectLost";
	case (char)EVID_onDisconnected:
		return L"onDisconnected";
	case (char)EVID_onKicked:
		return L"onKicked";
	case (char)EVID_onAddRoom:
		return L"onAddRoom";
	case (char)EVID_onRemoveRoom:
		return L"onRemoveRoom";
	case (char)EVID_onUserTalking:
		return L"onUserTalking";
	case (char)EVID_onUserStoppedTalking:
		return L"onUserStoppedTalking";
	case (char)EVID_onUserFileSaved:
		return L"onUserFileSaved";
	case (char)EVID_onAudioFilePlayed:
		return L"onAudioFilePlayed";
	case (char)EVID_onRecordStarted:
		return L"onRecordStarted";
	case (char)EVID_onRecordStopped:
		return L"onRecordStopped";
	case (char)EVID_onSRModeStatusChanged:
		return L"onSRModeStatusChanged";
	case (char)EVID_onNotified:
		return L"onNotified";
	case (char)EVID_onSetInputDevice:
		return L"onSetInputDevice";	
	case (char)EVID_onSetOutputDevice:
		return L"onSetOutputDevice";	
	case (char)EVID_onGetInputDevices:
		return L"onGetInputDevices";
	case (char)EVID_onGetOutputDevices:
		return L"onGetOutputDevices";
	case (char)EVID_onEvoClientWorkStateChanged:
		return L"onEvoClientWorkStateChanged";
	case (char) EVID_onVoipDeviceErrorCode:
		return L"onVoipDeviceErrorCode";
	case (char) EVID_onReInitializeVoipSystem:
		return L"onVoipDeviceErrorCode";
	case (char) EVID_onRecievedFindedMicName:
		return L"onRecievedFindedMicName";
		
	}
	return L"???";
}

__forceinline int GetVoiceEventIdx(char aidEvent){
	assert( false);
	switch(aidEvent){
	case EVID_onConnectSuccess:
		return aidEvent;
	}
	return 0;
}

bool mlSceneManager::addVoiceListener(char aidEvent, mlRMMLElement* apMLEl){
	if(apMLEl == NULL) return false;
	char chEvID=aidEvent;
	if(chEvID >= ML_VOICE_EVENTS_COUNT) return false;
	v_elems* pv=&(maVoiceListeners[chEvID]);
	v_elems::const_iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl) return true;
	}
	pv->push_back(apMLEl);
	return true;
}

void mlSceneManager::NotifyVoiceListeners(char chEvID){
	if(chEvID < ML_VOICE_EVENTS_COUNT){
		v_elems* pv=&(maVoiceListeners[chEvID]);
		v_elems::const_iterator vi;
		for(vi=pv->begin(); vi != pv->end(); vi++ ){
			mlRMMLElement* pMLEl=*vi;
			pMLEl->EventNotify(chEvID,(mlRMMLElement*)GOID_Voice);			
		}
	}
}

bool mlSceneManager::removeVoiceListener(char aidEvent,mlRMMLElement* apMLEl){
	if(apMLEl==NULL) return false;
	if(aidEvent==-1){
		// и удалим его из списка слушателей всех событий
		for(int ii=0; ii<ML_VOICE_EVENTS_COUNT; ii++){
			v_elems* pv=&maVoiceListeners[ii];
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
	v_elems* pv=&(maVoiceListeners[aidEvent-1]);
	v_elems::iterator vi;
	for(vi=pv->begin(); vi != pv->end(); vi++ ){
		if(*vi==apMLEl){
			pv->erase(vi);
			return true;
		}
	}
	return true;
}

void mlSceneManager::NotifyVoiceOnConnectSuccess()
{
	NotifyVoiceListeners(EVID_onConnectSuccess);
}

void mlSceneManager::NotifyVoiceOnConnectStart()
{
	NotifyVoiceListeners(EVID_onConnectStart);
}

void mlSceneManager::NotifyVoiceOnConnectFailed( char* aVoipServerIP)
{
	jsval vVoipServerIP = JSVAL_NULL;
	JSString *ssVoipServerIP = JS_NewStringCopyZ(cx, aVoipServerIP);
	vVoipServerIP = STRING_TO_JSVAL( ssVoipServerIP);
	JS_SetProperty(cx, GPSM(cx)->GetEventGO(), "voipServerIP", &vVoipServerIP );
	NotifyVoiceListeners(EVID_onConnectFailed);
	JS_DeleteProperty(cx,GPSM(cx)->GetEventGO(), "voipServerIP");	
}

void mlSceneManager::NotifyVoiceOnConnectLost()
{
	NotifyVoiceListeners(EVID_onConnectLost);
}

void mlSceneManager::NotifyVoiceOnDisconnected()
{
	NotifyVoiceListeners(EVID_onDisconnected);
}

void mlSceneManager::NotifyVoiceOnKicked()
{
	NotifyVoiceListeners(EVID_onKicked);
}

void mlSceneManager::NotifyVoiceOnAddRoom(const char* roomName)
{
	NotifyVoiceListeners(EVID_onAddRoom);
}

void mlSceneManager::NotifyVoiceOnRemoveRoom(const char* roomName)
{
	NotifyVoiceListeners(EVID_onRemoveRoom);
}

void mlSceneManager::NotifyVoiceOnUserTalking(const char* userName)
{
	JSString* str = JS_NewStringCopyZ( cx, userName);
	jsval v = STRING_TO_JSVAL( str);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"userName",&v);
	NotifyVoiceListeners(EVID_onUserTalking);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "userName");
}

void mlSceneManager::NotifyVoiceOnUserStoppedTalking(const char* userName)
{
	JSString* str = JS_NewStringCopyZ( cx, userName);
	jsval v = STRING_TO_JSVAL( str);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"userName",&v);
	NotifyVoiceListeners(EVID_onUserStoppedTalking);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "userName");
}

void mlSceneManager::NotifyVoiceOnUserFileSaved(const char* userName, const char* fileName)
{
	if( GetContext()->mpLogWriter)
	{
		CLogValue logValue(
			"NotifyVoiceOnUserFileSaved: UserName = ", userName, " fileName = ", fileName
			);
		mpContext->mpLogWriter->WriteLnLPCSTR( logValue.GetData());
	}

	JSString* str = JS_NewStringCopyZ( cx, userName);
	jsval v = STRING_TO_JSVAL( str);
	JSString* str2 = JS_NewStringCopyZ( cx, fileName);
	jsval v2 = STRING_TO_JSVAL( str2);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"userName",&v);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"fileName",&v2);
	NotifyVoiceListeners(EVID_onUserFileSaved);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "userName");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "fileName");
}

void mlSceneManager::NotifyVoiceOnAudioFilePlayed(const char* userName, const char* fileName)
{
	JSString* str = JS_NewStringCopyZ( cx, userName);
	jsval v = STRING_TO_JSVAL( str);
	JSString* str2 = JS_NewStringCopyZ( cx, fileName);
	jsval v2 = STRING_TO_JSVAL( str2);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"userName",&v);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"fileName",&v2);
	NotifyVoiceListeners(EVID_onAudioFilePlayed);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "userName");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "fileName");
}

void mlSceneManager::NotifyVoiceOnRecordStarted()
{
	NotifyVoiceListeners( EVID_onRecordStarted);
}

void mlSceneManager::NotifyVoiceOnSRModeStatusChanged(unsigned int auStatus)
{
	jsval v = INT_TO_JSVAL( auStatus);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"status",&v);
	NotifyVoiceListeners( EVID_onSRModeStatusChanged);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "status");
}

void mlSceneManager::NotifyVoiceOnNotified(unsigned int auCode, unsigned int auData)
{
	jsval v = INT_TO_JSVAL( auCode);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"notifyCode",&v);
	v = INT_TO_JSVAL( auData);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"notifyData",&v);
	NotifyVoiceListeners( EVID_onNotified);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "notifyData");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "notifyCode");
}

void mlSceneManager::NotifySetInputDevice(unsigned int auCode, int aiToIncludeSoundDevice)
{
	jsval v = INT_TO_JSVAL( auCode);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"deviceErrorCode",&v);
	jsval v2 = INT_TO_JSVAL( aiToIncludeSoundDevice);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"deviceToInclude",&v2);
	NotifyVoiceListeners( EVID_onSetInputDevice);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "deviceErrorCode");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "deviceToInclude");
}

void mlSceneManager::NotifySetOutputDevice(unsigned int auCode, int aiToIncludeSoundDevice)
{
	jsval v = INT_TO_JSVAL( auCode);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"deviceErrorCode",&v);
	jsval v2 = INT_TO_JSVAL( aiToIncludeSoundDevice);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"deviceToInclude",&v2);
	NotifyVoiceListeners( EVID_onSetOutputDevice);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "deviceErrorCode");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "deviceToInclude");
}

void mlSceneManager::NotifyGetInputDevices( const wchar_t *apMicDeviceNames, const wchar_t *apMixDeviceNames)
{
	//jsval v = INT_TO_JSVAL( auCode);
	JSString* str = JS_NewUCStringCopyZ( cx, (const jschar*)apMicDeviceNames);
	jsval v = STRING_TO_JSVAL( str);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"micDeviceNames",&v);
	JSString* str2 = JS_NewUCStringCopyZ( cx, (const jschar*)apMixDeviceNames);
	jsval v2 = STRING_TO_JSVAL( str2);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"mixDeviceNames",&v2);
	NotifyVoiceListeners( EVID_onGetInputDevices);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "micDeviceNames");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "mixDeviceNames");
}

void mlSceneManager::NotifyGetOutputDevices( wchar_t *aDeviceNames)
{
	//jsval v = INT_TO_JSVAL( auCode);
	JSString* str = JS_NewUCStringCopyZ( cx, (const jschar*)aDeviceNames);
	jsval v = STRING_TO_JSVAL( str);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"outDeviceNames",&v);
	NotifyVoiceListeners( EVID_onGetOutputDevices);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "outDeviceNames");
}

void mlSceneManager::NotifyAutoFindedMicName(const wchar_t *aDeviceGuid, const wchar_t *aDeviceName, const wchar_t *apDeviceLineName)
{
	JSString* strGuid = JS_NewUCStringCopyZ( cx, (const jschar*)aDeviceGuid);
	jsval vGuid = STRING_TO_JSVAL( strGuid);
	JSString* strName = JS_NewUCStringCopyZ( cx, (const jschar*)aDeviceName);
	jsval vName = STRING_TO_JSVAL( strName);
	JSString* strLineName = JS_NewUCStringCopyZ( cx, (const jschar*)apDeviceLineName);
	jsval vLine = STRING_TO_JSVAL( strLineName);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"findedMicGuid",&vGuid);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"findedMicName",&vName);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"findedMicLineName",&vLine);
	NotifyVoiceListeners( EVID_onRecievedFindedMicName);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "findedMicGuid");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "findedMicName");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "findedMicLineName");
}

void mlSceneManager::NotifyDeviceErrorCode( int anErrorCode)
{
	jsval v = INT_TO_JSVAL( anErrorCode);
	
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"voipDeviceErrorCode",&v);
	NotifyVoiceListeners( EVID_onVoipDeviceErrorCode);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "voipDeviceErrorCode");
}

void mlSceneManager::NotifyReInitializeVoipSystem()
{
	NotifyVoiceListeners( EVID_onReInitializeVoipSystem);
}

void mlSceneManager::NotifyVoiceOnRecordStopped(unsigned int auErrorCode)
{
	jsval v = INT_TO_JSVAL( auErrorCode);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"errorCode",&v);
	NotifyVoiceListeners( EVID_onRecordStopped);
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "errorCode");
}

void mlSceneManager::NotifyVoiceOnEvoClientWorkStateChanged(unsigned int aSessionType, unsigned int aWorkState, unsigned int auErrorCode)
{
	jsval v = INT_TO_JSVAL( aSessionType);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"sessionType",&v);
	v = INT_TO_JSVAL( aWorkState);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"workState",&v);
	v = INT_TO_JSVAL( auErrorCode);
	JS_SetProperty( cx,GPSM(cx)->GetEventGO(),"errorCode",&v);

	NotifyVoiceListeners( EVID_onEvoClientWorkStateChanged);

	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "errorCode");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "workState");
	JS_DeleteProperty( cx, GPSM(cx)->GetEventGO(), "sessionType");
}

void mlSceneManager::ResetVoice(){
	for(int ii=0; ii<ML_VOICE_EVENTS_COUNT; ii++){
		maVoiceListeners[ii].clear();
	}
}

JSBool JSVoiceGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){ 
	if(JSVAL_IS_INT(id)){
		mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);

		voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
		if( !pVoip)
		{
			JS_ReportError( cx, "Player.voice.JSVoiceGetProperty failed. Voip manager in not initialized.");
			return JS_TRUE;
		}

		int iID=JSVAL_TO_INT(id);
		switch(iID){
			case JSPROP_useDirectSound:{
			}break;
			case JSPROP_micGainLevel:{
				double micGainLevel= (double)pVoip->GetMicGainLevel();
				mlJS_NewDoubleValue( cx, micGainLevel, vp);
			}break;
			case JSPROP_micActivationLevel:{
				double micActivationLevel = (double)pVoip->GetMicActivationLevel();
				mlJS_NewDoubleValue( cx, micActivationLevel, vp);
			}break;
			case JSPROP_recorderFreq:{
				unsigned int recorderFreq = pVoip->GetRecorderFreq();
				*vp = INT_TO_JSVAL( recorderFreq);
			}break;
			case JSPROP_voiceQuality:{
				double voiceQuality = (double)pVoip->GetVoiceQuality();
				mlJS_NewDoubleValue( cx, voiceQuality, vp);
			}break;
			case JSPROP_inputLevel:{
				double inputLevel = (double)pVoip->GetCurrentInputLevel();
				mlJS_NewDoubleValue( cx, inputLevel, vp);
			}break;
			case JSPROP_inputLevelNormalized:{
				double inputLevelNormalized = (double)pVoip->GetCurrentInputLevelNormalized();
				mlJS_NewDoubleValue( cx, inputLevelNormalized, vp);
			}break;
			case JSPROP_voiceActivated:{
				bool voiceActivated = pVoip->GetVoiceActivated();
				*vp = BOOLEAN_TO_JSVAL( voiceActivated);
			}break;
			case JSPROP_microphoneEnabled:{
				bool microphoneEnabled = pVoip->GetMicrophoneEnabled();
				*vp = BOOLEAN_TO_JSVAL( microphoneEnabled);
			}break;
			case JSPROP_audioSource: {
				voip::voipAudioSource audioSource = pVoip->GetAudioSource();
				*vp = INT_TO_JSVAL( (int)audioSource);
			}break;
			case JSPROP_denoisingEnabled: {
				*vp = BOOLEAN_TO_JSVAL( pVoip->IsDenoisingEnabled());
			}break;
			case JSPROP_autoGainControlEnabled: {
				*vp = BOOLEAN_TO_JSVAL( pVoip->IsAutoGaneControlEnabled());
			}break;
			case JSPROP_autoVolume: {
				bool autoVolume = pVoip->GetAutoVolume();
				*vp = BOOLEAN_TO_JSVAL( autoVolume);
			}break;
			case JSPROP_SOURCE_MICROPHONE: {
				*vp = INT_TO_JSVAL( MICROPHONE_CONST_VALUE);
			}break;
			case JSPROP_SOURCE_MIXER: {
				*vp = INT_TO_JSVAL( MIXER_CONST_VALUE);
			}break;
			case JSPROP_SOURCE_VIRTUALCABLE: {
				*vp = INT_TO_JSVAL( VIRTUALCABLE_CONST_VALUE);
			}break;
		}
	}
	return JS_TRUE; 
}

JSBool JSVoiceSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp){ 
	if(JSVAL_IS_INT(id)){
		mlSceneManager* pSM = (mlSceneManager*)JS_GetPrivate(cx, obj);

		voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
		if( !pVoip)
		{
			JS_ReportError( cx, "Player.voice.JSVoiceSetProperty failed. Voip manager in not initialized.");
			return JS_TRUE;
		}

		int iID=JSVAL_TO_INT(id);
		switch(iID){
			case JSPROP_useDirectSound:{
			}break;
			case JSPROP_micGainLevel:{
				double micGainLevel = 0.0;
				ML_JSVAL_TO_DOUBLE(micGainLevel,*vp);
				pVoip->SetMicGainLevel( micGainLevel);
			}break;
			case JSPROP_micActivationLevel:{
				double micActivationLevel = 0.0;
				ML_JSVAL_TO_DOUBLE(micActivationLevel,*vp);
				pVoip->SetMicActivationLevel( micActivationLevel);
			}break;
			case JSPROP_recorderFreq:{
				unsigned int recFreq = 0;
				ML_JSVAL_TO_INT(recFreq,*vp);
				pVoip->SetRecorderFreq( recFreq);
			}break;
			case JSPROP_voiceQuality:{
				double voiceQiality = 0.0;
				ML_JSVAL_TO_DOUBLE(voiceQiality,*vp);
				pVoip->SetVoiceQuality( voiceQiality);
			}break;
			case JSPROP_voiceActivated:{
				bool voiceActivated = false;
				ML_JSVAL_TO_BOOL(voiceActivated,*vp);
				pVoip->SetVoiceActivated( voiceActivated);
			}break;
			case JSPROP_microphoneEnabled:{
				bool microphoneEnabled = false;
				ML_JSVAL_TO_BOOL(microphoneEnabled,*vp);
				if(microphoneEnabled)
					pVoip->StartTransmitting();
				else
					pVoip->StopTransmitting();
			}break;
			case JSPROP_autoVolume:{
				bool audioVolume = false;
				ML_JSVAL_TO_BOOL(audioVolume, *vp);
				pVoip->SetAutoVolume( audioVolume);
			}break;
			case JSPROP_denoisingEnabled: {
				bool denoisingEnabled = false;
				ML_JSVAL_TO_BOOL(denoisingEnabled,*vp);
				pVoip->SetDenoisingEnabled( denoisingEnabled);
			}break;
			case JSPROP_autoGainControlEnabled: {
				bool autoGainControlEnabled = false;
				ML_JSVAL_TO_BOOL(autoGainControlEnabled,*vp);
				pVoip->SetAutoGaneControlEnabled( autoGainControlEnabled);
			}break;
		}
	}
	return JS_TRUE; 
}

JSBool JSFUNC_restoreConnection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.restoreConnection failed. Voip manager in not initialized.");
		return JS_TRUE;
	}
	pVoip->RestoreConnection();
	return JS_TRUE;
}

JSBool JSFUNC_startRecording(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.startRecording failed. Voip manager in not initialized.");
		return JS_TRUE;
	}
	bool bSucc = pVoip->StartMicRecording();
	*rval = BOOLEAN_TO_JSVAL( bSucc);

	return JS_TRUE;
}

JSBool JSFUNC_stopRecording(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.stopRecording failed. Voip manager in not initialized.");
		return JS_TRUE;
	}
	wchar_t *fileName = MP_NEW_ARR( wchar_t, 255);
	bool bSucc = pVoip->StopMicRecording( &fileName, 255);

	if ( bSucc)
	{
		mlRMMLAudio* pNewEL=(mlRMMLAudio*)GET_RMMLEL(cx, mlRMMLAudio::newJSObject(cx));
		pNewEL->SetParent(GPSM(cx)->GetScene());

		JSString* jss = wr_JS_NewUCStringCopyZ( cx, fileName);
		jsval v = STRING_TO_JSVAL( jss);
		JS_SetProperty( cx, pNewEL->mjso, "src", &v);

		*rval = OBJECT_TO_JSVAL( pNewEL->mjso);
	}
	else
		*rval = NULL;
	MP_DELETE_ARR( fileName);
	return JS_TRUE;
}

JSBool JSFUNC_getInputDevices(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.getInputDevices failed. Voip manager in not initialized.");
		return JS_TRUE;
	}

	if ( argc != 1 || !JSVAL_IS_INT( argv[ 0]))
	{
		JS_ReportError( cx, "argument of getInputDevices function must be int");
		return JS_FALSE;
	}

	bool bForce = JSVAL_TO_INT(argv[0]);
	bool bSucc = pVoip->GetInputDevices( bForce);
	*rval = JS_TRUE;
	return JS_TRUE;
}

JSBool JSFUNC_getOutputDevices(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.getOutputDevices failed. Voip manager in not initialized.");
		return JS_TRUE;
	}

	if ( argc != 1 || !JSVAL_IS_INT( argv[ 0]))
	{
		JS_ReportError( cx, "argument of getOutputDevices function must be int");
		return JS_FALSE;
	}

	bool bForce = JSVAL_TO_INT(argv[0]);
	bool bSucc = pVoip->GetOutputDevices( bForce);
	*rval = JS_TRUE;
	return JS_TRUE;
}

JSBool JSFUNC_checkEnableCurrentDevice(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.checkEnableCurrentDevice failed. Voip manager in not initialized.");
		return JS_TRUE;
	}	
	bool bSucc = pVoip->CheckEnableCurrentDevice();
	*rval = INT_TO_JSVAL( (int)bSucc);
	return JS_TRUE;
}

JSBool JSFUNC_setInputDeviceMode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.setInputDeviceMode failed. Voip manager in not initialized.");
		return JS_TRUE;
	}

	if ( argc != 3 || !JSVAL_IS_STRING( argv[ 0]) || !JSVAL_IS_STRING( argv[ 1]) || !JSVAL_IS_INT( argv[ 2]))
	{
		JS_ReportError( cx, "Two arguments of setInputDevice function must be strings and third argument is int");
		return JS_FALSE;
	}

	wchar_t* pwcDeviceGuid=NULL;
	JSString* jssDeviceGuid=JSVAL_TO_STRING(argv[0]);
	pwcDeviceGuid=(wchar_t*)JS_GetStringChars(jssDeviceGuid);

	wchar_t* pwcDeviceLine=NULL;
	JSString* jssDeviceLine=JSVAL_TO_STRING(argv[1]);
	pwcDeviceLine=(wchar_t*)JS_GetStringChars(jssDeviceLine);

	int iToIncludeSoundDevice = JSVAL_TO_INT(argv[2]);

	bool bSucc = pVoip->SetInputDeviceMode( pwcDeviceGuid, pwcDeviceLine, iToIncludeSoundDevice);	
	
	*rval = BOOLEAN_TO_JSVAL( bSucc);

	return JS_TRUE;
}

JSBool JSFUNC_setOutputDeviceMode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.setOutputDeviceMode failed. Voip manager in not initialized.");
		return JS_TRUE;
	}

	sm::smISoundManager* pSoundMan = GPSM(cx)->GetContext()->mpSndM;
	if ( !pSoundMan)
	{
		JS_ReportError( cx, "Player.voice.setOutputDeviceMode failed. Sound manager in not initialized.");
		return JS_TRUE;
	}

	if ( argc != 3 || !JSVAL_IS_STRING( argv[ 0]) || !JSVAL_IS_STRING( argv[ 1]) || !JSVAL_IS_INT( argv[ 2]))
	{
		JS_ReportError( cx, "Two arguments of setOutputDeviceMode function must be strings and third argument is int");
		return JS_FALSE;
	}


	wchar_t* pwcDeviceGuid=NULL;
	JSString* jssDeviceGuid=JSVAL_TO_STRING(argv[0]);
	pwcDeviceGuid=(wchar_t*)JS_GetStringChars(jssDeviceGuid);

	wchar_t* pwcDeviceLine=NULL;
	JSString* jssDeviceLine=JSVAL_TO_STRING(argv[1]);
	pwcDeviceLine=(wchar_t*)JS_GetStringChars(jssDeviceLine);

	int iToIncludeSoundDevice = JSVAL_TO_INT(argv[2]);

	bool bSucc = pVoip->SetOutputDeviceMode( pwcDeviceGuid, pwcDeviceLine, iToIncludeSoundDevice);	

	std::wstring wcDeviceGuid = pwcDeviceGuid;
	omsresult res = pSoundMan->ReplaceDevice( wcDeviceGuid);

	*rval = BOOLEAN_TO_JSVAL( bSucc && res == S_OK);

	return JS_TRUE;
}

JSBool JSFUNC_setInputMixerMode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.setInputMixerMode failed. Voip manager in not initialized.");
		return JS_TRUE;
	}

	if ( argc != 3 || !JSVAL_IS_STRING( argv[ 0]) || !JSVAL_IS_STRING( argv[ 1]) || !JSVAL_IS_INT( argv[ 2]))
	{
		JS_ReportError( cx, "Two arguments of setInputMixerMode function must be strings and third argument is int");
		return JS_FALSE;
	}

	wchar_t* pwcDeviceGuid=NULL;
	JSString* jssDeviceGuid=JSVAL_TO_STRING(argv[0]);
	pwcDeviceGuid=(wchar_t*)JS_GetStringChars(jssDeviceGuid);

	wchar_t* pwcDeviceLine=NULL;
	JSString* jssDeviceLine=JSVAL_TO_STRING(argv[1]);
	pwcDeviceLine=(wchar_t*)JS_GetStringChars(jssDeviceLine);

	int iToIncludeSoundDevice = JSVAL_TO_INT(argv[2]);
	bool bSucc = pVoip->SetInputMixerMode( pwcDeviceGuid, pwcDeviceLine, iToIncludeSoundDevice);
	*rval = BOOLEAN_TO_JSVAL( bSucc);

	return JS_TRUE;
}

/*JSBool JSFUNC_setInputVirtualAudioCableMode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.setInputMixerMode failed. Voip manager in not initialized.");
		return JS_TRUE;
	}

	if ( argc != 1 || !JSVAL_IS_INT( argv[ 0]))
	{
		JS_ReportError( cx, "One argument of setInputVirtualAudioCableMode function must be int");
		return JS_FALSE;
	}

	bool bToIncludeSoundDevice = JSVAL_TO_INT(argv[0]);
	bool bSucc = pVoip->SetInputVirtualAudioCableMode( bToIncludeSoundDevice);
	*rval = BOOLEAN_TO_JSVAL( bSucc);

	return JS_TRUE;
}*/

JSBool JSFUNC_isMixerSet(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.isMixerSet failed. Voip manager in not initialized.");
		return JS_TRUE;
	}

	/*if ( argc !=  || !JSVAL_IS_STRING( argv[ 0]) || !JSVAL_IS_STRING( argv[ 1]) || !JSVAL_IS_INT( argv[ 2]))
	{
		JS_ReportError( cx, "Two arguments of isMixerSet function must be strings and third argument is int");
		return JS_FALSE;
	}*/
	
	bool bSucc = pVoip->IsMixerSet();
	*rval = BOOLEAN_TO_JSVAL( bSucc);

	return JS_TRUE;
}

JSBool JSFUNC_startFindRealMicrophon(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.startFindRealMicrophon failed. Voip manager in not initialized.");
		return JS_TRUE;
	}
	
	bool bSucc = pVoip->StartFindRealMicrophon();
	*rval = BOOLEAN_TO_JSVAL( bSucc);

	return JS_TRUE;
}

JSBool JSFUNC_endFindRealMicrophon(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.endFindRealMicrophon failed. Voip manager in not initialized.");
		return JS_TRUE;
	}
	
	bool bSucc = pVoip->EndFindRealMicrophon();
	*rval = BOOLEAN_TO_JSVAL( bSucc);

	return JS_TRUE;
}

JSBool JSFUNC_setUserVolume(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.setUserVolume failed. Voip manager in not initialized.");
		return JS_TRUE;
	}

	if( argc != 2)
	{
		JS_ReportError(cx, "setUserVolume must take two arguments: user login and voice level");
		return JS_FALSE;
	}

	if(!JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx,"First parameter of setUserVolume(..) must be a string (user login)");
		return JS_FALSE;
	}
	wchar_t* pwcUserLogin = NULL;
	JSString* jssUserLogin = JSVAL_TO_STRING(argv[0]);
	pwcUserLogin = (wchar_t*)JS_GetStringChars(jssUserLogin);
	
	jsdouble jsd;
	double userVolume = 1.0;
	if(!JS_ValueToNumber(cx, argv[1], &jsd))
	{
		JS_ReportError(cx,"Second parameter of setUserVolume(..) must be a double (voice level)");
		return JS_FALSE;
	}
	userVolume = jsd;

	pVoip->SetUserVolume( pwcUserLogin, userVolume);

	return JS_TRUE;
}

JSBool JSFUNC_setEchoCancellation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.setEchoCancellation failed. Voip manager in not initialized.");
		return JS_TRUE;
	}

	if( argc != 1)
	{
		JS_ReportError(cx, "setEchoCancellation must take one arguments: enabled EchoCancellation");
		return JS_FALSE;
	}


	jsdouble jsd;
	int echoState = 0;
	double dEchoState = 0.0;
	if(!JS_ValueToNumber(cx, argv[0], &jsd))
	{
		JS_ReportError(cx,"First parameter of setEchoCancellation(..) must be a int (state)");
		return JS_FALSE;
	}
	dEchoState = jsd;
	echoState = (int)dEchoState;

	if (echoState == 1)
		pVoip->SetEchoCancellation( true);
	else
		pVoip->SetEchoCancellation( false);

	return JS_TRUE;
}

JSBool JSFUNC_setMixerEchoCancellation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.setMixerEchoCancellation failed. Voip manager in not initialized.");
		return JS_TRUE;
	}

	if( argc != 1)
	{
		JS_ReportError(cx, "setMixerEchoCancellation must take one arguments: enabled EchoCancellation");
		return JS_FALSE;
	}


	jsdouble jsd;
	int echoState = 0;
	double dEchoState = 0.0;
	if(!JS_ValueToNumber(cx, argv[0], &jsd))
	{
		JS_ReportError(cx,"First parameter of setMixerEchoCancellation(..) must be a int (state)");
		return JS_FALSE;
	}
	dEchoState = jsd;
	echoState = (int)dEchoState;

	if (echoState == 1)
		pVoip->SetMixerEchoCancellation( true);
	else
		pVoip->SetMixerEchoCancellation( false);

	return JS_TRUE;
}

JSBool JSFUNC_setIncomingVoiceVolume(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.setIncomingVoiceVolume failed. Voip manager in not initialized.");
		return JS_TRUE;
	}

	if( argc != 1)
	{
		JS_ReportError(cx, "setIncomingVoiceVolume must take one arguments: voice level");
		return JS_FALSE;
	}


	jsdouble jsd;
	double masterVolume = 1.0;
	if(!JS_ValueToNumber(cx, argv[0], &jsd))
	{
		JS_ReportError(cx,"First parameter of setIncomingVoiceVolume(..) must be a double (voice level)");
		return JS_FALSE;
	}
	masterVolume = jsd;

	pVoip->SetIncomingVoiceVolume( masterVolume);

	return JS_TRUE;
}

JSBool JSFUNC_setUserGainLevel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;
	if ( !pVoip)
	{
		JS_ReportError( cx, "Player.voice.setUserGainLevel failed. Voip manager in not initialized.");
		return JS_TRUE;
	}

	if( argc != 2)
	{
		JS_ReportError(cx, "setUserGainLevel must take two arguments: user login and voice gain level");
		return JS_FALSE;
	}

	if(!JSVAL_IS_STRING(argv[0]))
	{
		JS_ReportError(cx,"First parameter of setUserGainLevel(..) must be a string (user login)");
		return JS_FALSE;
	}
	wchar_t* pwcUserLogin = NULL;
	JSString* jssUserLogin = JSVAL_TO_STRING(argv[0]);
	pwcUserLogin = (wchar_t*)JS_GetStringChars(jssUserLogin);

	jsdouble jsd;
	double userGainLevel = 1.0;
	if(!JS_ValueToNumber(cx, argv[1], &jsd))
	{
		JS_ReportError(cx,"Second parameter of setUserGainLevel(..) must be a double (voice gain level)");
		return JS_FALSE;
	}
	userGainLevel = jsd;

	pVoip->SetUserGainLevel( pwcUserLogin, userGainLevel);

	return JS_TRUE;
}

JSBool JSFUNC_removeRecord(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if( argc == 0 || !JSVAL_IS_OBJECT(argv[0])){
		JS_ReportError(cx, "First argument of removeRecord function must be a object");
		return JS_TRUE;
	}

	JSObject* jso = JSVAL_TO_OBJECT( argv[ 0]);
	if ( !mlRMMLAudio::IsInstance( cx, jso))
	{
		JS_ReportError(cx, "First argument of removeRecord function must be mlRMMLAudio");
		return JS_TRUE;
	}
	
	mlRMMLAudio* pSndEL = (mlRMMLAudio*) JS_GetPrivate(cx, jso);

	if ( pSndEL == NULL)
	{
		JS_ReportError(cx, "First argument of removeRecord function must be not null");
		return JS_TRUE;
	}

	omsContext* pContext = GPSM(cx)->GetContext();	
	pContext->mpResM->RemoveResource( pSndEL->GetILoadable()->GetSrc());

	return JS_TRUE;
}

#define SELF_RECORDING_FUNCION_MACRO(jsfunc, voipfunc)	JSBool JSFUNC_##jsfunc(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)	\
{	\
	voip::IVoipManager* pVoip = GPSM(cx)->GetContext()->mpVoipMan;	\
	if ( !pVoip)	\
{	\
	JS_ReportError( cx, "Player.voice.srEnterMode failed. Voip manager in not initialized.");	\
	return JS_TRUE;	\
}	\
	int errorCode = pVoip->##voipfunc();	\
	*rval = INT_TO_JSVAL( errorCode);	\
	return JS_TRUE;	\
}

SELF_RECORDING_FUNCION_MACRO(srEnterMode, EnterSelfRecordingMode);
SELF_RECORDING_FUNCION_MACRO(srStartRecording, StartSelfRec);
SELF_RECORDING_FUNCION_MACRO(srStopRecording, StopSelfRec);
SELF_RECORDING_FUNCION_MACRO(srPlayRecording, PlaySelfRec);
SELF_RECORDING_FUNCION_MACRO(srExitMode, ExitSelfRecordingMode);

}


#pragma once

namespace rmml {

class mlWorldEventManager : public sync::syncIEventManagerCallback
{
public: 
	mlWorldEventManager(mlSceneManager* apSM);
	~mlWorldEventManager();

public:
	// возникла ошибка при записи лога
	virtual void onRecordError(int aiErrorCode);
	// запись не может быть начата так как сервер вернул код об ошибке
	virtual void onRecordStartFailed(int iErrorCode, const wchar_t* aAdditionalInfo);
	// клиент подключилс€ к записи в аудитории
	virtual void onRecordConnected(unsigned int aiID, const wchar_t* aAuthorOrAssistant, unsigned char isAuthor, unsigned int aDuration, const wchar_t* aLocationName);
	// клиент отключилс€ от записи в аудитории
	virtual void onRecordDisconnected(unsigned int auRecordID,  bool isAuthor, bool isNeedToAsk);
	// запись данного клиента в аудитории остановлена
	virtual void onRecordStoped(unsigned int auRecordID, unsigned int auDuration, syncRecordInfo* appRecordInfo = NULL);
	// ѕолучена мессага о присоединении к событию
	virtual void onEventEntered(unsigned int auiEventId, const wchar_t* awcLocationID, unsigned int auiEventRealityId, unsigned int auiPlayingRecordId, unsigned int auiBaseRecordId, unsigned int auiEventRecordId, bool abIsEventRecording, const wchar_t* aJSONDescription);
	// ѕолучена мессага об отсоединении от событи€
	virtual void onEventLeaved(unsigned int auiPlayingSeanceID,unsigned int auiPlayingRecordID);
	// ѕолучена мессага об изменении в событии
	virtual void onEventUpdateInfo(unsigned int auiEventId, const wchar_t* awcLocationID, unsigned int auiEventRealityId, unsigned int auiPlayingRecordId, const wchar_t* aJSONDescription);
	// сообщаем автору права пользователей в данном событии (актуально если автор выходил из зан€ти€)
	virtual void onEventUserRightsInfo( unsigned int aEventID, const wchar_t *aEventUserRights);		
	// сообщаем  права пользовател€
	virtual void onEventUserRightUpdate( unsigned int aEventID, const wchar_t *aEventUserRights);
	// сообщаем о бане (разбанивании)
	virtual void onEventUserBanUpdate( const wchar_t *aEventName, const wchar_t *aEventUrl, char aBan);
	// ѕолучена мессага о входе в локацию
	virtual void onLocationEntered( const wchar_t* awcLocationID, const wchar_t* aJSONDescription);

public:
	// —обыти€ с записью
	void onRecordConnectedInternal(mlSynchData& aData);
	void onRecordStartFailedInternal(mlSynchData& aData);
	void onRecordDisconnectInternal(mlSynchData& aData);
	void onRecordStopedInternal(mlSynchData& aData);
	// —обыти€ от "WorldEvent"
	void onEventEnteredInternal(mlSynchData&  aData);
	void onEventLeavedInternal(mlSynchData& aData);
	void onLocationEnteredInternal(mlSynchData& aData);
	void onEventUpdateInfoInternal(mlSynchData&  aData);
	void onEventUserRightsInfoReceivedInternal(mlSynchData& aData);
	void onEventUserRightUpdateReceivedInternal(mlSynchData& aData);
	void onEventUserBanUpdateReceivedInternal(mlSynchData& aData);	

private:
	void LeaveCurrentRecording();
	void LeaveCurrentEvent();

private:
	bool			m_isInRecording;
	bool			m_isMyRecord;
	unsigned int	m_currentRecordingID;
	unsigned int	m_currentEventID;
	mlSceneManager* mpSM;
};


}

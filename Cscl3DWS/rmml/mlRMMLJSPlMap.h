#ifndef _mlRMMLJSPlMap_h__
#define _mlRMMLJSPlMap_h__

namespace rmml{


class mlMapCallbackManager: public cm::cmIMapManCallbacks{
	bool mbCallbacksIsSet;
	mlSceneManager* mpSM;
public:
	mlMapCallbackManager(mlSceneManager* apSM);
	void onTeleportInfoReceivedInternal(const wchar_t* asJSONInfo);
	void onTeleported( cm::cmTeleportInfo* aTeleportInfo);
	void onTeleportedInternal(mlSynchData& aData);
	void onTeleportError(int aiError, const wchar_t* apwcError, const wchar_t* apwcEventInfo);
	void onTeleportErrorInternal(mlSynchData& aData);
	void onURLChangedInternal(mlSynchData& aData);
	void onShowZoneInternal(mlSynchData& aData);
	// объект был создан по запросу из Player.map.CreateObject()
	// (aiUserData - ссылка на объект, переданный в CreateObject)
	void onObjectCreated(int aiUserData, unsigned int auiObjectID, unsigned int auiBornRealityID, int aiError, const wchar_t* apwcError);
	void onObjectCreatedInternal(mlSynchData& aData);
	void SetCallbacks(JSContext *cx);
	// Получен статус объекта
	void onReceivedObjectStatus(unsigned int auObjectID, unsigned int auBornRealityID, int aiStatus, void* apUserData);
	void onReceivedObjectStatusInernal(mlSynchData& aData);
	void onReceivedFullSeanceInfo(mlSynchData& aData);
	void OnUserSleepInternal();
	void OnUserWakeUpInternal();
};

}

#endif
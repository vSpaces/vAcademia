#ifndef _mlRMMLJSPlServer_h__
#define _mlRMMLJSPlServer_h__

#include "SyncMan.h"
#include "ServiceMan.h"

namespace rmml{

/**
 * JS-класс для реализации объектов HTTP-запросов
 */
struct iresulthandler
{
	virtual void onCompleteFunction(mlSceneManager* apSM, wchar_t* apwcResponse, int *aiErrorCode) = 0;
};

class mlHTTPRequest:	
	public mlJSClass
{
	int miID;
	int miRequestID;
	int miStatus;
	JSString* mjssResponse;
public:
	MP_WSTRING msQuery; // for debug purpose
	iresulthandler* pResultHandler;	
public:
	mlHTTPRequest(void):
		MP_WSTRING_INIT(msQuery)
	{
		miID = -1;
		miStatus = -1;
		mjssResponse = NULL;
		pResultHandler = NULL;
	}
	void destroy() { MP_DELETE_THIS; }
	~mlHTTPRequest(void);
MLJSCLASS_DECL2(mlHTTPRequest)
private:
	enum {
		JSPROP_status,	// -1 - не определен, 0 - ошибка, 1 - в процессе, 2 - готово
		JSPROP_response,
	};
public:
	void SetID(int aiID){ miID = aiID; }
	int GetID(){ return miID; }
	void SetRequestID(int aiID){ miRequestID = aiID; }
	int GetRequestID(){ return miRequestID; }
	void onQueryComplete(int aiID, const wchar_t* apwcResponse=NULL, int* apiErrorCode=NULL);
};

// до этого числа идентификаторы совпадают с индексом. 
//#define MAX_IN_ARRAY_ID 200
#define MAX_IN_ARRAY_ID 10

typedef MP_VECTOR_DC<mlHTTPRequest*> mlHTTPRequestVector;
typedef std::vector<mlHTTPRequest*>::iterator mlHTTPRequestVectorIter;
class mlRequestList: public mlHTTPRequestVector
{
#define MIN_SERVICE_REQUEST_ID	0xFFFF
#define MAX_SERVICE_REQUEST_ID	0xFFFFFFF
	MP_VECTOR<mlHTTPRequest*> mvAfterMax;
	int					mCurrentServiceRequestID;
public:
	mlRequestList():
	  MP_VECTOR_INIT(mvAfterMax)
	{
		mCurrentServiceRequestID = MIN_SERVICE_REQUEST_ID;	
		MP_VECTOR_INIT_THIS;
	}
	void RemoveRequestByID(int aiID){
		mlHTTPRequestVector::iterator it = mvAfterMax.begin();
		mlHTTPRequestVector::iterator itEnd = mvAfterMax.end();

		for ( ; it != itEnd; it++)
		{
			if ((*it)->GetRequestID() == aiID)
			{
				mvAfterMax.erase(it);
				break;
			}
		}
	}

	// 
	// mlRMMLJSPlServer::JSFUNC_JSONMethod -> ID == GetNextServiceIDRequest()
	// mlRMMLJSPlModule::JSFUNC_gotoLink -> ID == pResMan->SetModuleID
	// mlRMMLJSPlModule::JSFUNC_login -> ID == GetNextServiceIDRequest()
	// mlRMMLJSPlModule::JSFUNC_sendQuery -> ID == mpComMan->SendQuery(..)
	// mlSoapJSO::JSFUNC_soapMethod -> ID == GetNextServiceIDRequest()
	// 
	void AddRequest(mlHTTPRequest* apRequest){
		mvAfterMax.push_back(apRequest);
		int iID = apRequest->GetRequestID();
		mlHTTPRequest* apCurrentRequest = GetRequestByID( iID);
		if( apCurrentRequest!=NULL && apCurrentRequest!=apRequest)
		{
			mlTrace(apRequest->mcx, "#FF0000 qwedqwedqwedq\n");
			mlTrace(apRequest->mcx, "#FF0000 qwedqwedqwedq\n");
			mlTrace(apRequest->mcx, "#FF0000 qwedqwedqwedq\n");
			mlTrace(apRequest->mcx, "#FF0000 qwedqwedqwedq\n");
			mlTrace(apRequest->mcx, "#FF0000 qwedqwedqwedq\n");
			mlTrace(apRequest->mcx, "#FF0000 qwedqwedqwedq\n");
			mlTrace(apRequest->mcx, "#FF0000 ERROR!!!!! AddRequest(mlHTTPRequest* apRequest)\n");
		}
	}

	// ~mlHTTPRequest
	// mlServerCallbackManager::onQueryCompleteInternal
	void RemoveRequest(mlHTTPRequest* apRequest){
		if(apRequest == NULL) return;

		mlHTTPRequestVector::iterator it = mvAfterMax.begin();
		mlHTTPRequestVector::iterator itEnd = mvAfterMax.end();

		for ( ; it != itEnd; it++){
			if (*it == apRequest)
			{
				mvAfterMax.erase(it);
				break;
			}
		}
	}

	mlHTTPRequest* GetRequestByID(int aiID, int queryID = -1){
		if(aiID < 0) return NULL;
		
		mlHTTPRequestVector::iterator it = mvAfterMax.begin();
		mlHTTPRequestVector::iterator itEnd = mvAfterMax.end();

		for ( ; it != itEnd; it++)
		if ((*it)->GetRequestID() == aiID)
		{
				return *it;
		}

		if (queryID != -1)
		{
			it = mvAfterMax.begin();

			for ( ; it != itEnd; it++)
			if ((*it)->GetID() == aiID)
			{
				return *it;
			}
		}

		return NULL;
	}

	// Возвращает идентификатор запроса для сервиса, который уйдет в пакет и вернется назад.
	// Для запросов, выполяемых через ComMan - нумерацию ведет сам ComMan
	int GetNextServiceIDRequest()
	{
		mCurrentServiceRequestID++;

		while (GetRequestByID(mCurrentServiceRequestID))
		{
			mCurrentServiceRequestID++;
		}

		return mCurrentServiceRequestID;
	}
};

class mlServerCallbackManager: public cm::cmIComManCallbacks{
	bool mbCallbacksIsSet;
    mlSceneManager* mpSM;
public:
	mlServerCallbackManager(mlSceneManager* apSM);
	void onQueryComplete(int aiID, const wchar_t* apwcResponse=NULL, int* apiErrorCode=NULL);
	void onQueryCompleteInternal(mlSynchData& aData);
	void SetCallbacks(JSContext *cx);
};

class mlSyncCallbackManager: 
	public sync::syncISyncManCallbacks,
	public sync::syncIRecordManagerCallback
{
	bool mbCallbacksIsSet;
    mlSceneManager* mpSM;
public:
	mlSyncCallbackManager(mlSceneManager* apSM);
	void SetCallbacks(JSContext *cx);
	void onReceivedRecordsInfoInternal(mlSynchData& aData);
	void onReceivedSeanceInfoInternal(mlSynchData& aData);
	void onReceivedSeancesInfoInternal(mlSynchData& aData);
	void OnReceivedFullSeanceInfoInternal(mlSynchData& aData);
	void OnReceivedFullRecordInfoInternal(mlSynchData& aData);
	void OnSeanceLoadedInternal(unsigned int aSeanceID, unsigned int aRecordID, unsigned char isAuthor,  unsigned char isSuspended, int aMinutes);
	void OnUpdatePlayingTimeInternal(mlSynchData& aData);
	void OnSeanceSeekPosInternal(mlSynchData& aData);
	void onSeanceJoined(mlSynchData& aData);
	// void RecordSaved(unsigned int);
	void onReceivedObjectStatusInternal(mlSynchData& aData);
	void onAllSyncObjectsLoaded();	

 //реализация sync::syncIRecordManagerCallback
public:
	// с сервера получена информация о записях (auID - идентификатор запроса, переданный в GetRecords())
	virtual void onReceivedSeancesInfo(unsigned int auSeancesCount, syncSeanceInfo** appRecordsInfo, unsigned int auID, int aiErrorCode);

// реализация sync::syncISyncManCallbacks
public:
	virtual void onUpdatePlayingTime(unsigned int auTime);
	virtual void onSeanceSeekPos(unsigned int auTime);
	// с сервера получена информация о сеансах по записи лога
	virtual void onReceivedFullSeanceInfo(unsigned int aiSeanceObjectID,  syncSeanceInfo* appSeancesInfo, int aiErrorCode);
	// с сервера получена инфа что человек покинул сеанс
	virtual void onSeanceLeaved();
	// с сервера получена информация о записях (auID - идентификатор запроса, переданный в GetRecords())
	virtual void onReceivedRecordsInfo( sync::syncCountRecordInfo& totalCountRec, unsigned int auRecordCount, syncRecordInfo** appRecordsInfo, unsigned int auID, int aiErrorCode);
	// Сеанс воспроизведения на сервере синхронизации создан
	virtual void onSeanceCreated(unsigned int aiSeanceID, unsigned int aiSeanceObjectID, unsigned int aSeanceMode, const wchar_t* aAuthor,const wchar_t* SeanceURL, int aiErrorCode);
	virtual void onSeanceClosed(unsigned int aiSeanceID);
	// Получен статус объекта
	virtual void onReceivedObjectStatus(unsigned int auObjectID, unsigned int auBornRealityID, int aiStatus);
	// Получено сообщение о паузе в проигрывании записи
	virtual void onSeancePaused(unsigned int auBornRealityID);
	// Получено сообщение о входе в сеанс
	virtual void onJoinedToSeance(unsigned int aiSeanceRecord, unsigned int aiSeanceID, unsigned char isAuthor,  unsigned char isSuspended, int aMinutes);
	// Получено сообщение о то что начато проигрывание сеанса
	virtual void onSeancePlayed();
	// Получено сообщение о то что сработала перемотка
	virtual void onSeanceRewinded();
	// Получено сообщение о то что сеанс проигран до конца
	virtual void onSeanceFinishedPlaying();
	// Получен список пользователей в реальности
	virtual void onUserListReceived(unsigned int realityID, unsigned short userCount, void* userList);
	virtual void onServerErrorReceived(unsigned int errorCode);
	virtual void onObjectCreationResult(unsigned int auiErrorCode, unsigned int auiObjectID, unsigned int auiBornRealityID, unsigned int auiRmmlHandlerID);
	virtual void onErrorCreateTempLocation( unsigned int aErrorCode);
	virtual void onInfoMessage(unsigned int auMsgCode, const wchar_t *apMessage);

private:
	void	LeaveCurrentRecording();
	bool		 m_isInRecording;
	bool		 m_isMyRecord;
	unsigned int m_recordID;
};

class mlServiceCallbackManager: 
	public service::IServiceManCallbacks
{
protected:
	bool mbCallbacksSMIsSet;
	bool mbCallbacksWSMIsSet;
	mlSceneManager* mpSM;
public:
	void onGetServiceInternal(mlSynchData& aData);
	void onReceivedMethodResultInternal(mlSynchData& aData);
	void onNotify(mlSynchData& aData);
	void onConnectRestoredInternal(mlSynchData& aData);
public:
	mlServiceCallbackManager(mlSceneManager* apSM);
	void SetCallbacks(JSContext *cx);
	void onGetService(unsigned int rmmlID, const wchar_t* apwcServiceName, const wchar_t* apwcServiceDescription, int* apiErrorCode=NULL);
	void onMethodComplete(int aRequestID, int aRMMLID, const wchar_t* apwcResponse=NULL, bool isService=false, int aiErrorCode=NULL);
	void onServiceNotify(int aiID, const wchar_t* apwcResponse=NULL, int* aiErrorCode=NULL);
	void onConnectRestored();
};


}

#endif
#pragma once

#include "SyncMan.h"
#include <string>

namespace rmml {

struct mlLogSeanceInfo{
#define MLLSI_NAME			0x1
#define MLLSI_AUTHOR		0x2
#define MLLSI_MEMBERS		0x4
#define MLLSI_DATE			0x8
#define MLLSI_MODE			0x10
#define MLLSI_URL			0x20
#define MLLSI_RECORD_ID		0x40
	// флаги, определяющие какая информация действительна 
	int miFlags;
	// (пока будет без флагов. Если == NULL, значит не определена. Если == "", значит пустая строка)
	// идентификатор записи 
	unsigned int muSeanceID;
	// название записи
	MP_WSTRING msName;
	// автор 
	MP_WSTRING msAuthor;
	// URL
	MP_WSTRING msURL;
	// тэги для поиска
	MP_WSTRING msMembers;
	// дата
	MP_WSTRING msDate;
	unsigned int mlDate;
	unsigned int mlMode;
	unsigned int mlRecordID;
public:
	mlLogSeanceInfo();
	mlLogSeanceInfo(syncSeanceInfo& aSeanceInfo);
	mlLogSeanceInfo(const mlLogSeanceInfo& aSrc);
	//		const mlLogRecordInfo& operator=(const mlLogRecordInfo& aSrc);
};

class mlLogSeanceJSO:
					public mlJSClass
{
	int miSeanceID; // хандл сеанса (предоставляется SyncMan-ом)
	unsigned long mlDate;
	int miFlags;
public:
	mlLogSeanceJSO(void);
	void destroy() { MP_DELETE_THIS; }
	~mlLogSeanceJSO(void){}
MLJSCLASS_DECL2(mlLogSeanceJSO)
private:
	enum {
		JSPROP_recordID,
		JSPROP_name,
		JSPROP_author,
		JSPROP_members,
		JSPROP_date,
		JSPROP_mode,
		JSPROP_URL
	};

	MP_WSTRING msName;
	MP_WSTRING msAuthor;
	MP_WSTRING msMembers;
	MP_WSTRING msDate;
	MP_WSTRING msURL;
	int mode;
	int recordID;	// хандл записи (предоставляется SyncMan-ом)
	bool mbPlaying; 

	enum {
		JSPROP_errorCode = 10000000,
		JSPROP_length,
	};
	int errorCode;

	void NameChanged();
	void MembersChanged();
	void DateChanged();
	JSFUNC_DECL(queryFullInfo)

	//void CallListeners(const wchar_t* apwcEventName);
public:
	void SetRecordID(unsigned int iID){
		recordID = iID;
	}
	unsigned int GetSeanceID(){ return miSeanceID ; }
	mlString getSeanceURL(){ return msURL; /* JS_GetStringChars(URL); */ }
	void Init(mlLogSeanceInfo& aLRI);
	bool FillRecordInfo(syncSeanceInfo& aSeanceInfo);
	void SetSeanceID(unsigned int auID);
	void SetErrorCode(int aiErrorCode){ errorCode = aiErrorCode; }
	void SetSeanceURL(const wchar_t* aURL);
	void SetSeanceAuthor(const wchar_t* aAuthor);
	void SetSeanceName(const wchar_t* aName);

	void OnSeanceCreated();
	void OnSeanceClosed();
	void OnReceivedFullInfo();

	bool isPlaying();
	void setPlaying(bool playing);

};

#define EVNM_onError L"onError"
#define EVNM_onReceived L"onReceived"
#define EVNM_onCreated  L"onCreated"
#define EVNM_onClosed  L"onClosed"
#define EVNM_onReceivedFullInfo  L"onReceivedInfo"

// Список записей лога
// Возвращается методом Player.server.recorder.getRecords(...).
// Записи приходят с сервера и вызывается onReceived() или onError() (после вызова временная ссылка на объект удаляется).
class mlLogSeancesJSO:
					public mlJSClass
{
	MP_VECTOR<mlLogSeanceInfo> mvSeanceInfo;
	// mlSynchPropListeners mListeners;
public:
	mlLogSeancesJSO(void);
	void destroy() { MP_DELETE_THIS; }
	~mlLogSeancesJSO(void);
	MLJSCLASS_DECL2(mlLogSeancesJSO)
private:
	enum {
		JSPROP_errorCode = 10000000,
		JSPROP_length,
	};
	int errorCode;
	int length;

	//void CallListeners(const wchar_t* apwcEventName);
	// ??
public:
	void SetErrorCode(int aiErrorCode){ errorCode = aiErrorCode; }
	void SaveSeanceInfo(syncSeanceInfo** appRecordsInfo);
	void SaveSeanceInfo(mlLogSeanceInfo seanceInfo);
	void OnReceivedSeancesInfo();

};

}

#pragma once

#include "SyncMan.h"
#include <string>

namespace rmml {

struct mlLogRecordInfo{
#define MLLRI_NAME			0x1
#define MLLRI_DESCRIPTION	0x2
#define MLLRI_AUTHOR		0x4
#define MLLRI_LOCATION		0x8
#define MLLRI_CRTIME		0x10
#define MLLRI_SUBJECT		0x20
#define MLLRI_LESSON		0x40
#define MLLRI_LEVEL			0x80
#define MLLRI_PREVIEW		0x100
#define MLLRI_URL			0x200
#define MLLRI_OUTSIDE_URL	0x400
#define MLLRI_IS_REAL		0x800
#define MLLRI_AUTHOR_FULL_NAME		0x1000
#define MLLRI_TAGS			0x2000

	// флаги, определяющие какая информация действительна 
	int miFlags;
	// (пока будет без флагов. Если == NULL, значит не определена. Если == "", значит пустая строка)
	// идентификатор записи 
	unsigned int muRecordID;
	// название записи
	MP_WSTRING msName;
	// описание записи
	MP_WSTRING msDescription;
	// предмет
	MP_WSTRING msSubject;
	// урок
	MP_WSTRING msLesson;
	// тэги
	MP_WSTRING msTags;
	// уровень
	MP_WSTRING msLevel;
	// автор Ник (RO)
	MP_WSTRING msAuthor;
	// автор Имя Отчество (RO)	
	MP_WSTRING msAuthorFullName;
	// тэги для поиска
	MP_WSTRING msLocation;
	// URL
	MP_WSTRING msURL;
	MP_WSTRING msOutsideURL;
	// превьюшка
	MP_WSTRING msPreview;
	// время создания записи (RO)
	MP_WSTRING msCreationTime;
	unsigned long mlCreationTime;
	// длительность в ms (RO)
	unsigned long mlDuration;
	// количество комментариев (RO)
	unsigned int muiCommentCount;
	// длительность в ms (RO)
	unsigned int muDepth;
	unsigned int muVisitorCount;
	unsigned int muAvatarInRecordCount;
	bool isReal;
	unsigned int muStatus;
public:
	mlLogRecordInfo();
	mlLogRecordInfo(syncRecordInfo* aRecordInfo);
	mlLogRecordInfo(const mlLogRecordInfo& aSrc);
	//		const mlLogRecordInfo& operator=(const mlLogRecordInfo& aSrc);
};

#define EVNM_onReceivedFullInfo  L"onReceivedInfo"

class mlLogRecordJSO:
					public mlJSClass
{
	int miRecordID; // хандл записи (предоставляется SyncMan-ом)
	unsigned long mlCreationTime;
	int miFlags;
public:
	mlLogRecordJSO(void);
	void destroy() { MP_DELETE_THIS; }
	~mlLogRecordJSO(void){}
MLJSCLASS_DECL2(mlLogRecordJSO)
private:
	enum {
		JSPROP_name,
		JSPROP_description,
		JSPROP_author,
		JSPROP_authorFullName,
		JSPROP_location,
		JSPROP_creationTime,
		JSPROP_duration,
		JSPROP_commentCount,
		JSPROP_lesson,
		JSPROP_subject,
		JSPROP_level,
		JSPROP_price,
		JSPROP_depth, 
		JSPROP_preview, 
		JSPROP_visitorCount,
		JSPROP_avatarInRecordCount,
		JSPROP_URL,
		JSPROP_outsideURL,
		JSPROP_isReal,
		JSPROP_status, 
		JSPROP_id,
		JSPROP_tags

	};
	JSString* name;
	JSString* description;
	JSString* author;
	JSString* authorFullName;
	JSString* location;
	JSString* lesson;
	JSString* tags;
	JSString* subject;
	JSString* level;
	JSString* preview;
	MP_WSTRING URL;
	MP_WSTRING outsideURL;
	int price;
	JSObject* creationTime;
	int duration;
	int commentCount;
	int depth;
	int visitorCount;
	int avatarInRecordCount;
	bool isReal;
	int status;

	void NameChanged();
	void DescriptionChanged();
	void SubjectChanged();
	void LessonChanged();
	void LevelChanged();
	void TagsChanged();
	void PreviewChanged();
	void IsRealChanged();

	void LocationChanged();
//	void CallListeners(const wchar_t* apwcEventName);

public:

	unsigned int GetRecordID(){ return miRecordID; }
	void SetRecordID(unsigned int iID){ miRecordID = iID; }
	
	mlString getRecordURL(){ return URL; }
	void SetRecordURL(const wchar_t* aURL);
	void SetRecordOutsideURL(const wchar_t* aURL);

	void Init(mlLogRecordInfo& aLRI);
	bool FillRecordInfo(syncRecordInfo& aRecordInfo);

	void OnReceivedFullInfo();
};

#define EVNM_onError L"onError"
#define EVNM_onReceived L"onReceived"

// Список записей лога
// Возвращается методом Player.server.recorder.getRecords(...).
// Записи приходят с сервера и вызывается onReceived() или onError() (после вызова временная ссылка на объект удаляется).
class mlLogRecordsJSO:
					public mlJSClass
{
	MP_VECTOR<mlLogRecordInfo> mvRecordInfo;
	unsigned int uiRecordTotalCount;
	// mlSynchPropListeners mListeners;
public:
	mlLogRecordsJSO(void);
	void destroy() { MP_DELETE_THIS; }
	~mlLogRecordsJSO(void);
	MLJSCLASS_DECL2(mlLogRecordsJSO)
private:
	enum {
		JSPROP_errorCode = 10000000,
		JSPROP_length,
		JSPROP_totalCount,
		JSPROP_totalCountNow,
		JSPROP_totalCountWithoutStatus,
		JSPROP_totalCountSoon,
	};
	int errorCode;
	int length;
	int totalCount;
	int totalCountNow;
	int totalCountWithoutStatus;
	int totalCountSoon;

//	void CallListeners(const wchar_t* apwcEventName);

	// ??
public:
	void SetErrorCode(int aiErrorCode){ errorCode = aiErrorCode; }
	void SaveRecordsInfo( sync::syncCountRecordInfo& totalCountRec, unsigned int auRecordCount, syncRecordInfo** appRecordsInfo);
	void OnReceivedRecordsInfo( sync::syncCountRecordInfo& totalCountRec);
	void AddNewRecord(syncRecordInfo* appRecordsInfo);
};

}


#include "mlRMML.h"
#include "mlLogRecord.h"
#include "mlLogSeance.h"
#include <time.h>

namespace rmml {

mlLogRecordJSO::mlLogRecordJSO():
	MP_WSTRING_INIT(URL),
	MP_WSTRING_INIT(outsideURL)
{
	name = NULL;
	description = NULL;
	author = NULL;
	authorFullName = NULL;
	location = NULL;
	lesson = NULL;
	tags = NULL;
	subject = NULL;
	level = NULL;
	preview = NULL;
	price = 0;

	creationTime = NULL;
	duration = 0;
	commentCount = 0;
	depth = 0;
	status = 0;
	isReal = false;
}


///// JavaScript Variable Table
JSPropertySpec mlLogRecordJSO::_JSPropertySpec[] = {
	JSPROP_RW(name)
	JSPROP_RW(description)
	JSPROP_RO(author)
	JSPROP_RO(authorFullName)
	JSPROP_RW(location)
	JSPROP_RW(lesson)
	JSPROP_RW(tags)
	JSPROP_RW(subject)
	JSPROP_RW(level)
	JSPROP_RW(price)
	JSPROP_RW(preview)
	JSPROP_RO(creationTime)
	JSPROP_RO(duration)
	JSPROP_RO(commentCount)
	JSPROP_RO(depth)
	JSPROP_RO(visitorCount)
	JSPROP_RO(avatarInRecordCount)
	JSPROP_RO(URL)
	JSPROP_RO(outsideURL)
	JSPROP_RO(status)
	JSPROP_RW(price)
	JSPROP_RW(isReal)
	JSPROP_RO(id)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlLogRecordJSO::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Record,mlLogRecordJSO,0)
	MLJSCLASS_ADDPROPFUNC
//	MLJSCLASS_ADDPROPENUMOP
//	MLJSCLASS_ADDALTERCONSTR(AlterJSConstructor)
MLJSCLASS_IMPL_END(mlLogRecordJSO)

JSBool mlLogRecordJSO::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN2(mlLogRecordJSO);
		switch(iID){
		case JSPROP_name:
			if(JSVAL_IS_STRING(*vp)){
				priv->name = JSVAL_TO_STRING(*vp);
			}else{
				JS_ReportError(cx,"'name' property value must be a string");
				priv->name = NULL;
			}
			priv->NameChanged();
			break;
		case JSPROP_description:
			if(JSVAL_IS_STRING(*vp)){
				priv->description = JSVAL_TO_STRING(*vp);
			}else{
				JS_ReportError(cx,"'description' property value must be a string");
				priv->description = NULL;
			}
			priv->DescriptionChanged();
			break;
		case JSPROP_subject:
			if(JSVAL_IS_STRING(*vp)){
				priv->subject = JSVAL_TO_STRING(*vp);
			}else{
				JS_ReportError(cx,"'subject' property value must be a string");
				priv->subject = NULL;
			}
			priv->SubjectChanged();
			break;
		case JSPROP_preview:
			if(JSVAL_IS_STRING(*vp)){
				priv->preview = JSVAL_TO_STRING(*vp);
			}else{
				JS_ReportError(cx,"'preview' property value must be a string");
				priv->preview = NULL;
			}
			priv->PreviewChanged();
			break;
		case JSPROP_lesson:
			if(JSVAL_IS_STRING(*vp)){
				priv->lesson = JSVAL_TO_STRING(*vp);
			}else{
				JS_ReportError(cx,"'lesson' property value must be a string");
				priv->lesson = NULL;
			}
			priv->LessonChanged();
			break;
		case JSPROP_tags:
			if(JSVAL_IS_STRING(*vp)){
				priv->tags = JSVAL_TO_STRING(*vp);
			}else{
				JS_ReportError(cx,"'lesson' property value must be a string");
				priv->tags = NULL;
			}
			priv->TagsChanged();
			break;
		case JSPROP_level:
			if(JSVAL_IS_STRING(*vp)){
				priv->level = JSVAL_TO_STRING(*vp);
			}else{
				JS_ReportError(cx,"'level' property value must be a string");
				priv->level = NULL;
			}
			priv->LevelChanged();
			break;
		case JSPROP_location:
			if(JSVAL_IS_STRING(*vp)){
				priv->location = JSVAL_TO_STRING(*vp);
			}else{
				JS_ReportError(cx,"'location' property value must be a string");
				priv->location = NULL;
			}
			priv->LocationChanged();
			break;
		case JSPROP_isReal:
			if(JSVAL_IS_BOOLEAN(*vp)){
				priv->isReal = JSVAL_TO_BOOLEAN(*vp);
			}else{
				JS_ReportError(cx,"'location' property value must be a string");
				priv->location = NULL;
			}
			priv->LocationChanged();
			break;
		}
	SET_PROPS_END2;
	return JS_TRUE;
}

JSBool mlLogRecordJSO::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN2(mlLogRecordJSO);
		switch(iID){
		case JSPROP_name:
			if(priv->name == NULL)
				*vp = JS_GetEmptyStringValue(cx);
			else
				*vp = STRING_TO_JSVAL(priv->name);
			break;
		case JSPROP_description:
			if(priv->description == NULL)
				*vp = JS_GetEmptyStringValue(cx);
			else
				*vp = STRING_TO_JSVAL(priv->description);
			break;
		case JSPROP_subject:
			if(priv->subject == NULL)
				*vp = JS_GetEmptyStringValue(cx);
			else
				*vp = STRING_TO_JSVAL(priv->subject);
			break;
		case JSPROP_preview:
			if(priv->preview == NULL)
				*vp = JS_GetEmptyStringValue(cx);
			else
				*vp = STRING_TO_JSVAL(priv->preview);
			break;
		case JSPROP_lesson:
			if(priv->lesson == NULL)
				*vp = JS_GetEmptyStringValue(cx);
			else
				*vp = STRING_TO_JSVAL(priv->lesson);
			break;
		case JSPROP_tags:
			if(priv->tags == NULL)
				*vp = JS_GetEmptyStringValue(cx);
			else
				*vp = STRING_TO_JSVAL(priv->tags);
			break;
		case JSPROP_level:
			if(priv->level == NULL)
				*vp = JS_GetEmptyStringValue(cx);
			else
				*vp = STRING_TO_JSVAL(priv->level);
			break;
		case JSPROP_location:
			if(priv->location == NULL)
				*vp = JS_GetEmptyStringValue(cx);
			else
				*vp = STRING_TO_JSVAL(priv->location);
			break;
		case JSPROP_author:
			if(priv->author == NULL)
				*vp = JS_GetEmptyStringValue(cx);
			else
				*vp = STRING_TO_JSVAL(priv->author);
			break;
		case JSPROP_authorFullName:
			if(priv->authorFullName == NULL)
				*vp = JS_GetEmptyStringValue(cx);
			else
				*vp = STRING_TO_JSVAL(priv->authorFullName);
			break;
		case JSPROP_creationTime:
/*
			if(priv->creationTime == NULL)
				*vp = JSVAL_NULL;
			else
				*vp = OBJECT_TO_JSVAL(priv->creationTime);
*/
			{
			*vp = JSVAL_NULL;
			wchar_t wsScript[255];
			swprintf_s(wsScript, 255, L"new Date(%u000)",priv->mlCreationTime);
			jsval val = JSVAL_NULL; 
			JSBool bR = JS_EvaluateUCScript(cx, JS_GetGlobalObject(cx), (jschar*)wsScript, wcslen(wsScript), "creationTime", 1, &val);
			if (JSVAL_IS_REAL_OBJECT(val))
				*vp = val;
			}
			break;
		case JSPROP_status:
			*vp = INT_TO_JSVAL(priv->status);
			break;
		case JSPROP_isReal:
				*vp = BOOLEAN_TO_JSVAL(priv->isReal);
			break;
		case JSPROP_URL:
			if(priv->URL.c_str() == NULL)
				*vp = JS_GetEmptyStringValue(cx);
			else
				*vp = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)priv->URL.c_str()));
			break;
		case JSPROP_outsideURL:
			if(priv->outsideURL.c_str() == NULL)
				*vp = JS_GetEmptyStringValue(cx);
			else
				*vp = STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, (const jschar*)priv->outsideURL.c_str()));
			break;
		case JSPROP_duration:
			*vp = INT_TO_JSVAL(priv->duration);
			break;
		case JSPROP_commentCount:
			*vp = INT_TO_JSVAL(priv->commentCount);
			break;
		case JSPROP_depth:
			*vp = INT_TO_JSVAL(priv->depth);
			break;
		case JSPROP_visitorCount:
			*vp = INT_TO_JSVAL(priv->visitorCount);
			break;
		case JSPROP_avatarInRecordCount:
			*vp = INT_TO_JSVAL(priv->avatarInRecordCount);
			break;
		case JSPROP_id:
			*vp = INT_TO_JSVAL(priv->miRecordID);
			break;

		}
	GET_PROPS_END2;
	return JS_TRUE;
}


void mlLogRecordJSO::NameChanged(){
	miFlags |= MLLRI_NAME;
}

void mlLogRecordJSO::IsRealChanged()
{
	miFlags |= MLLRI_IS_REAL;
}
void mlLogRecordJSO::PreviewChanged(){
	miFlags |= MLLRI_PREVIEW;
}

void mlLogRecordJSO::DescriptionChanged(){
	miFlags |= MLLRI_DESCRIPTION;
}

void mlLogRecordJSO::LessonChanged(){
	miFlags |= MLLRI_LESSON;
}
void mlLogRecordJSO::TagsChanged(){
	miFlags |= MLLRI_TAGS;
}

void mlLogRecordJSO::SubjectChanged(){
	miFlags |= MLLRI_SUBJECT;
}
void mlLogRecordJSO::LevelChanged(){
	miFlags |= MLLRI_LEVEL;
}


void mlLogRecordJSO::LocationChanged(){
	miFlags |= MLLRI_LOCATION;
}

// сохраняем URL
void mlLogRecordJSO::SetRecordURL(const wchar_t* aURL)
{
	URL = aURL;
}

// сохраняем URL для выхода из локации
void mlLogRecordJSO::SetRecordOutsideURL(const wchar_t* aURL)
{
	outsideURL = aURL;
}

void mlLogRecordJSO::Init(mlLogRecordInfo& aLRI){
	miRecordID = aLRI.muRecordID;
	mlCreationTime = aLRI.mlCreationTime;
	miFlags = aLRI.miFlags;
	name = NULL;
	if(aLRI.miFlags & MLLRI_NAME){
		name = JS_NewUCStringCopyZ(mcx, (const jschar*)aLRI.msName.c_str());
		SAVE_STR_FROM_GC(mcx, mjso, name);
	}
	if(aLRI.miFlags & MLLRI_DESCRIPTION){
		description = JS_NewUCStringCopyZ(mcx, (const jschar*)aLRI.msDescription.c_str());
		SAVE_STR_FROM_GC(mcx, mjso, description);
	}
	if(aLRI.miFlags & MLLRI_SUBJECT){
		subject = JS_NewUCStringCopyZ(mcx, (const jschar*)aLRI.msSubject.c_str());
		SAVE_STR_FROM_GC(mcx, mjso, subject);
	}
	if(aLRI.miFlags & MLLRI_PREVIEW){
		preview = JS_NewUCStringCopyZ(mcx, (const jschar*)aLRI.msPreview.c_str());
		SAVE_STR_FROM_GC(mcx, mjso, preview);
	}
	if(aLRI.miFlags & MLLRI_LESSON){
		lesson = JS_NewUCStringCopyZ(mcx, (const jschar*)aLRI.msLesson.c_str());
		SAVE_STR_FROM_GC(mcx, mjso, lesson);
	}
	if(aLRI.miFlags & MLLRI_TAGS){
		tags = JS_NewUCStringCopyZ(mcx, (const jschar*)aLRI.msTags.c_str());
		SAVE_STR_FROM_GC(mcx, mjso, tags);
	}
	if(aLRI.miFlags & MLLRI_LEVEL){
		level = JS_NewUCStringCopyZ(mcx, (const jschar*)aLRI.msLevel.c_str());
		SAVE_STR_FROM_GC(mcx, mjso, level);
	}
	if(aLRI.miFlags & MLLRI_AUTHOR){
		author = JS_NewUCStringCopyZ(mcx, (const jschar*)aLRI.msAuthor.c_str());
		SAVE_STR_FROM_GC(mcx, mjso, author);
	}
	if(aLRI.miFlags & MLLRI_AUTHOR_FULL_NAME){
		authorFullName = JS_NewUCStringCopyZ(mcx, (const jschar*)aLRI.msAuthorFullName.c_str());
		SAVE_STR_FROM_GC(mcx, mjso, authorFullName);
	}
	if(aLRI.miFlags & MLLRI_LOCATION) {
		location = JS_NewUCStringCopyZ(mcx, (const jschar*)aLRI.msLocation.c_str());
		SAVE_STR_FROM_GC(mcx, mjso, location);
	}
/*
	wchar_t wsScript[255];
	swprintf(wsScript,L"new Date(%u000)",aLRI.mlCreationTime);
	jsval val = JSVAL_NULL; 
	JSBool bR = JS_EvaluateUCScript(mcx, JS_GetGlobalObject(mcx), (jschar*)wsScript, wcslen(wsScript), "userProps", 1, &val);
	if (JSVAL_IS_REAL_OBJECT(val))
	{
		creationTime = JSVAL_TO_OBJECT(val);
		jsval v;
		JS_GetProperty(mcx, mjso, "creationTime", &v);
	}
	else
		creationTime = NULL;
*/
	if(aLRI.miFlags & MLLRI_URL) {
		URL = aLRI.msURL;
	}

	if(aLRI.miFlags & MLLRI_OUTSIDE_URL) {
		outsideURL = aLRI.msOutsideURL;
	}

	duration = aLRI.mlDuration;
	commentCount = aLRI.muiCommentCount;
	depth = aLRI.muDepth;
	visitorCount = aLRI.muVisitorCount;
	avatarInRecordCount = aLRI.muAvatarInRecordCount;
	isReal = aLRI.isReal;
	status = aLRI.muStatus;
}

bool mlLogRecordJSO::FillRecordInfo(syncRecordInfo& aRecordInfo){
	aRecordInfo.muRecordID = miRecordID;

	aRecordInfo.mpwcName.clear();
	if(name != NULL)
		aRecordInfo.mpwcName = (const wchar_t*)JS_GetStringChars(name);

	aRecordInfo.mpwcDescription.clear();
	if(description != NULL)
		aRecordInfo.mpwcDescription = (const wchar_t*)JS_GetStringChars(description);

	aRecordInfo.mpwcSubject.clear();
	if(subject != NULL)
		aRecordInfo.mpwcSubject = (const wchar_t*)JS_GetStringChars(subject);

	aRecordInfo.mpwcPreview.clear();
	if(preview != NULL)
		aRecordInfo.mpwcPreview = (const wchar_t*)JS_GetStringChars(preview);

	aRecordInfo.mpwcLesson.clear();
	if(lesson != NULL)
		aRecordInfo.mpwcLesson = (const wchar_t*)JS_GetStringChars(lesson);

	aRecordInfo.mpwcTags.clear();
	if(tags != NULL)
		aRecordInfo.mpwcTags = (const wchar_t*)JS_GetStringChars(tags);


	aRecordInfo.mpwcLevel.clear();
	if(level != NULL)
		aRecordInfo.mpwcLevel = (const wchar_t*)JS_GetStringChars(level);

	aRecordInfo.mpwcAuthor.clear(); // RO
	if(author != NULL)
		aRecordInfo.mpwcAuthor = (const wchar_t*)JS_GetStringChars(author);

	aRecordInfo.mpwcAuthorFullName.clear();
	if(authorFullName != NULL)
		aRecordInfo.mpwcAuthorFullName = (const wchar_t*)JS_GetStringChars(authorFullName);

	aRecordInfo.mpwcLocation.clear();
	if(location != NULL)
		aRecordInfo.mpwcLocation = (const wchar_t*)JS_GetStringChars(location);

	aRecordInfo.mpwcCreationTime.clear(); // RO

	aRecordInfo.mpwcURL.clear();
	if(!URL.empty())
	{	aRecordInfo.mpwcURL = URL.c_str();
	}

	aRecordInfo.mlCreationTime = mlCreationTime; // RO
	aRecordInfo.mlDuration = duration; // RO

	aRecordInfo.muiCommentCount = commentCount; // RO
	aRecordInfo.muDepth = depth; //RO
	aRecordInfo.muVisitorCount = visitorCount; //RO
	aRecordInfo.muAvatarInRecordCount = avatarInRecordCount; // RO
	aRecordInfo.mbPublishNow = isReal;
	aRecordInfo.status = status;
	
	return true;
}

////////////////////////////////////////////
// mlLogRecordsJSO
////////////////////

mlLogRecordsJSO::mlLogRecordsJSO(void):
	MP_VECTOR_INIT(mvRecordInfo)
{
	errorCode = 0;
	length = 0;
	uiRecordTotalCount = 0;
	totalCount = 0;
}

mlLogRecordsJSO::~mlLogRecordsJSO(void){
}

///// JavaScript Variable Table
JSPropertySpec mlLogRecordsJSO::_JSPropertySpec[] = {
//	JSPROP_RO(errorCode)
//	JSPROP_RO(length)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlLogRecordsJSO::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(Records,mlLogRecordsJSO,0)
	MLJSCLASS_ADDPROPFUNC
//	MLJSCLASS_ADDPROPENUMOP
//	MLJSCLASS_ADDALTERCONSTR(AlterJSConstructor)
MLJSCLASS_IMPL_END(mlLogRecordsJSO)

JSBool mlLogRecordsJSO::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN2(mlLogRecordsJSO);
//	switch(iID){
//	}
	SET_PROPS_END2;
	return JS_TRUE;
}

JSBool mlLogRecordsJSO::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	if (JSVAL_IS_STRING(id)) {
		mlLogRecordsJSO* priv=(mlLogRecordsJSO*)JS_GetPrivate(cx, obj);
		if (priv==NULL){ *vp = JSVAL_NULL; return JS_TRUE; }
		JSString* jssPropName = JSVAL_TO_STRING(id);
		jschar* jscPropName = JS_GetStringChars(jssPropName);
		if(isEqual((const wchar_t*)jscPropName, L"length")){
			*vp = INT_TO_JSVAL(priv->length);
		}else 
			if(isEqual((const wchar_t*)jscPropName, L"totalCount"))
			{
				*vp = INT_TO_JSVAL(priv->totalCount);
			}
			else if(isEqual((const wchar_t*)jscPropName, L"totalCountNow"))
			{
				*vp = INT_TO_JSVAL(priv->totalCountNow);
			}
			else if(isEqual((const wchar_t*)jscPropName, L"totalCountWithoutStatus"))
			{
				*vp = INT_TO_JSVAL(priv->totalCountWithoutStatus);
			}
			else if(isEqual((const wchar_t*)jscPropName, L"totalCountSoon"))
			{
				*vp = INT_TO_JSVAL(priv->totalCountSoon);
			}
			else
				if(isEqual((const wchar_t*)jscPropName, L"errorCode"))
				{
					*vp = INT_TO_JSVAL(priv->errorCode);
				}
		return JS_TRUE;
	}
	GET_PROPS_BEGIN2(mlLogRecordsJSO);
/*
	switch(iID){
		case JSPROP_errorCode:
			*vp = INT_TO_JSVAL(priv->errorCode);
			break;
		case JSPROP_length:
			*vp = INT_TO_JSVAL(priv->length);
			break;
	}
*/
	GET_PROPS_END2;
	return JS_TRUE;
}

mlLogRecordInfo::mlLogRecordInfo():
	MP_WSTRING_INIT(msName),
	MP_WSTRING_INIT(msDescription),
	MP_WSTRING_INIT(msSubject),	
	MP_WSTRING_INIT(msLesson),	
	MP_WSTRING_INIT(msTags),
	MP_WSTRING_INIT(msLevel),	
	MP_WSTRING_INIT(msAuthor),
	MP_WSTRING_INIT(msAuthorFullName),
	MP_WSTRING_INIT(msLocation),
	MP_WSTRING_INIT(msURL),
	MP_WSTRING_INIT(msOutsideURL),
	MP_WSTRING_INIT(msPreview),
	MP_WSTRING_INIT(msCreationTime)
{
}

mlLogRecordInfo::mlLogRecordInfo(syncRecordInfo* aRecordInfo):
	MP_WSTRING_INIT(msName),
	MP_WSTRING_INIT(msDescription),
	MP_WSTRING_INIT(msSubject),	
	MP_WSTRING_INIT(msLesson),	
	MP_WSTRING_INIT(msTags),
	MP_WSTRING_INIT(msLevel),	
	MP_WSTRING_INIT(msAuthor),
	MP_WSTRING_INIT(msAuthorFullName),
	MP_WSTRING_INIT(msLocation),
	MP_WSTRING_INIT(msURL),
	MP_WSTRING_INIT(msOutsideURL),
	MP_WSTRING_INIT(msPreview),
	MP_WSTRING_INIT(msCreationTime)
{
	// флаги, определяющие какая информация действительна 
	miFlags = 0;
	// (пока будет без флагов. Если == NULL, значит не определена. Если == "", значит пустая строка)
	// идентификатор записи 
	muRecordID = aRecordInfo->muRecordID;
	// название записи
	if(!aRecordInfo->mpwcName.empty()){
		msName = aRecordInfo->mpwcName;
		miFlags |= MLLRI_NAME;
	}
	// описание записи
	if(!aRecordInfo->mpwcDescription.empty()){
		msDescription = aRecordInfo->mpwcDescription;
		miFlags |= MLLRI_DESCRIPTION;
	}
	// превьюшка
	if(!aRecordInfo->mpwcPreview.empty()){
		msPreview = aRecordInfo->mpwcPreview;
		miFlags |= MLLRI_PREVIEW;
	}
	// предмет
	if(!aRecordInfo->mpwcSubject.empty()){
		msSubject = aRecordInfo->mpwcSubject;
		miFlags |= MLLRI_SUBJECT;
	}
	// урок
	if(!aRecordInfo->mpwcLesson.empty()){
		msLesson = aRecordInfo->mpwcLesson;
		miFlags |= MLLRI_LESSON;
	}
	// тэги
	if(!aRecordInfo->mpwcTags.empty()){
		msTags = aRecordInfo->mpwcTags;
		miFlags |= MLLRI_TAGS;
	}
	// уровень
	if(!aRecordInfo->mpwcLevel.empty()){
		msLevel = aRecordInfo->mpwcLevel;
		miFlags |= MLLRI_LEVEL;
	}
	// автор (RO)
	if(!aRecordInfo->mpwcAuthor.empty()){
		msAuthor = aRecordInfo->mpwcAuthor;
		miFlags |= MLLRI_AUTHOR;
	}
	// автор (RO)
	if(!aRecordInfo->mpwcAuthorFullName.empty()){
		msAuthorFullName = aRecordInfo->mpwcAuthorFullName;
		miFlags |= MLLRI_AUTHOR_FULL_NAME;
	}

	// тэги для поиска
	if(!aRecordInfo->mpwcLocation.empty()){
		msLocation = aRecordInfo->mpwcLocation;
		miFlags |= MLLRI_LOCATION;
	}
	// время создания записи (RO)
	if(aRecordInfo->mlCreationTime != 0){
		mlCreationTime = aRecordInfo->mlCreationTime;
		miFlags |= MLLRI_CRTIME;
	}

	// URL (RO)
	if(!aRecordInfo->mpwcURL.empty()){
		msURL = aRecordInfo->mpwcURL;
		miFlags |= MLLRI_URL;
	}

	mlCreationTime = aRecordInfo->mlCreationTime;
	// длительность в ms (RO)
	mlDuration = aRecordInfo->mlDuration;
	// количество комментариев (RO)
	muiCommentCount = aRecordInfo->muiCommentCount;
	muDepth = aRecordInfo->muDepth;
	muVisitorCount = aRecordInfo->muVisitorCount;
	muAvatarInRecordCount = aRecordInfo->muAvatarInRecordCount;
	isReal = aRecordInfo->mbPublishNow;
	muStatus = aRecordInfo->status;

}

mlLogRecordInfo::mlLogRecordInfo(const mlLogRecordInfo& aSrc):
	MP_WSTRING_INIT(msName),
	MP_WSTRING_INIT(msDescription),
	MP_WSTRING_INIT(msSubject),	
	MP_WSTRING_INIT(msLesson),	
	MP_WSTRING_INIT(msTags),
	MP_WSTRING_INIT(msLevel),	
	MP_WSTRING_INIT(msAuthor),
	MP_WSTRING_INIT(msAuthorFullName),
	MP_WSTRING_INIT(msLocation),
	MP_WSTRING_INIT(msURL),
	MP_WSTRING_INIT(msOutsideURL),
	MP_WSTRING_INIT(msPreview),
	MP_WSTRING_INIT(msCreationTime)
{
	// флаги, определяющие какая информация действительна 
	miFlags = aSrc.miFlags;
	// идентификатор записи 
	muRecordID = aSrc.muRecordID;
	// название записи
	msName = aSrc.msName;
	// описание записи
	msDescription = aSrc.msDescription;
	// превьюшка
	msPreview = aSrc.msPreview;
	// предмет
	msSubject = aSrc.msSubject;
	// урок
	msLesson = aSrc.msLesson;
	// тэги
	msTags = aSrc.msTags;
	// уровень
	msLevel = aSrc.msLevel;
	// автор (RO)
	msAuthor = aSrc.msAuthor;
	// автор (RO)
	msAuthorFullName = aSrc.msAuthorFullName;
	// тэги для поиска
	msLocation = aSrc.msLocation;
	// время создания записи (RO)
	msCreationTime = aSrc.msCreationTime;
	mlCreationTime = aSrc.mlCreationTime;
	// длительность в ms (RO)
	mlDuration = aSrc.mlDuration;
	// URL (RO)
	msURL = aSrc.msURL;
	// количество комментариев (RO)
	muiCommentCount = aSrc.muiCommentCount;
	muDepth = aSrc.muDepth;
	muVisitorCount = aSrc.muVisitorCount;
	muAvatarInRecordCount = aSrc.muAvatarInRecordCount;
	isReal = aSrc.isReal;
	muStatus = aSrc.muStatus;
}

void mlLogRecordsJSO::SaveRecordsInfo( sync::syncCountRecordInfo& totalCountRec, unsigned int auRecordCount, syncRecordInfo** appRecordsInfo){
	uiRecordTotalCount = totalCountRec.totalCountRecord;
	if(auRecordCount == 0)
		return;
	while(auRecordCount >0){
		mvRecordInfo.push_back(mlLogRecordInfo(*appRecordsInfo));
		appRecordsInfo++;
		auRecordCount--;
	}
}

void mlLogRecordsJSO::AddNewRecord(syncRecordInfo* appRecordsInfo) {
		mvRecordInfo.push_back(mlLogRecordInfo(appRecordsInfo));
}

void mlLogRecordsJSO::OnReceivedRecordsInfo( sync::syncCountRecordInfo& totalCountRec){
	if(errorCode > 0){
		JSObject* jsoEvent = GPSM(mcx)->GetEventGO();
		jsval vErrorCode = INT_TO_JSVAL(errorCode);
		JS_SetProperty(mcx, jsoEvent, "errorCode", &vErrorCode);
		CallListeners(EVNM_onError);
		return;
	}
	// перебрать все записи в mvRecordInfo и создать по ним обертки (mlLogRecordJSO)
	std::vector<mlLogRecordInfo>::iterator vi = mvRecordInfo.begin();
	int iIdx;
	for(iIdx = 0; vi != mvRecordInfo.end(); vi++, iIdx++){
		JSObject* jsoLogRecord = mlLogRecordJSO::newJSObject(mcx);
		mlLogRecordJSO* pRecord = (mlLogRecordJSO*)JS_GetPrivate(mcx, jsoLogRecord);
		pRecord->Init(*vi);
		jsval v = OBJECT_TO_JSVAL(jsoLogRecord);
		JS_SetElement(mcx, mjso, iIdx, &v);
		SAVE_FROM_GC(mcx,mjso,jsoLogRecord);
	}
	length = iIdx;
	totalCount = totalCountRec.totalCountRecord;
	totalCountNow = totalCountRec.totalCountRecordNow;
	totalCountWithoutStatus = totalCountRec.totalCountRecordWithoutStatus;
	totalCountSoon = totalCountRec.totalCountRecordSoon;
	CallListeners(EVNM_onReceived);
}

void mlLogRecordJSO::OnReceivedFullInfo()
{
	CallListeners(EVNM_onReceivedFullInfo);
}

/*
void mlLogRecordJSO::CallListeners(const wchar_t* apwcEventName)
{
	jsval vFunc = JSVAL_NULL;
	if(apwcEventName == NULL) 
		return;
	// если есть свойство onXXX
	if(JS_GetUCProperty(mcx, mjso, apwcEventName, wcslen(apwcEventName), &vFunc) == JS_FALSE)
		return;
	if(!JSVAL_IS_STRING(vFunc)){
		if(!JSVAL_IS_REAL_OBJECT(vFunc))
			return;
		if(!JS_ObjectIsFunction(mcx, JSVAL_TO_OBJECT(vFunc)))
			return;
	}
	if(vFunc != JSVAL_NULL){
		// вызываем функцию
		jsval v;
		if(JSVAL_IS_STRING(vFunc)){
			JSString* jss = JSVAL_TO_STRING(vFunc);
			JS_EvaluateUCScript(mcx, mjso, JS_GetStringChars(jss), JS_GetStringLength(jss), 
				cLPCSTRq(apwcEventName), 1, &v);
		}else{
			JS_CallFunctionValue(mcx, mjso, vFunc, 0, NULL, &v);
		}
	}
}

void mlLogRecordsJSO::CallListeners(const wchar_t* apwcEventName)
{
	jsval vFunc = JSVAL_NULL;
	if(apwcEventName == NULL) 
		return;
	// если есть свойство onXXX
	if(JS_GetUCProperty(mcx, mjso, apwcEventName, wcslen(apwcEventName), &vFunc) == JS_FALSE)
		return;
	if(!JSVAL_IS_STRING(vFunc)){
		if(!JSVAL_IS_REAL_OBJECT(vFunc))
			return;
		if(!JS_ObjectIsFunction(mcx, JSVAL_TO_OBJECT(vFunc)))
			return;
	}
	if(vFunc != JSVAL_NULL){
		// вызываем функцию
		jsval v;
		if(JSVAL_IS_STRING(vFunc)){
			JSString* jss = JSVAL_TO_STRING(vFunc);
			JS_EvaluateUCScript(mcx, mjso, JS_GetStringChars(jss), JS_GetStringLength(jss), 
				cLPCSTRq(apwcEventName), 1, &v);
		}else{
			JS_CallFunctionValue(mcx, mjso, vFunc, 0, NULL, &v);
		}
	}
}
*/

}

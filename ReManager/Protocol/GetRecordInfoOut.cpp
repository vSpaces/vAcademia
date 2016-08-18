#include "stdafx.h"
#include "GetRecordInfoOut.h"

CGetRecordInfoOut::CGetRecordInfoOut(sync::syncRecordInfo* appRecordInfo)
{
	const wchar_t * pwcName;
	const wchar_t * pwcAuthor;
	const wchar_t * pwcCreationTime;
	const wchar_t * pwcLocation;
	const wchar_t * pwcLesson;
	const wchar_t * pwcDescription;
	const wchar_t * pwcURL;

	data.add( appRecordInfo->muRecordID);
	data.add( appRecordInfo->muReality);
	data.add( (unsigned int)appRecordInfo->mlDuration);

	pwcName = appRecordInfo->mpwcName.c_str();
	pwcAuthor = appRecordInfo->mpwcAuthor.c_str();
	pwcCreationTime = appRecordInfo->mpwcCreationTime.c_str();
	pwcLocation = appRecordInfo->mpwcLocation.c_str();
	pwcLesson = appRecordInfo->mpwcLesson.c_str();
	pwcDescription = appRecordInfo->mpwcDescription.c_str();
	pwcURL = appRecordInfo->mpwcURL.c_str();

	data.addStringUnicode(wcslen(pwcName), (BYTE*)pwcName);
	data.addStringUnicode(wcslen(pwcAuthor), (BYTE*)pwcAuthor);
	data.addStringUnicode(wcslen(pwcCreationTime), (BYTE*)pwcCreationTime);
	data.addStringUnicode(wcslen(pwcLocation), (BYTE*)pwcLocation);
	data.addStringUnicode(wcslen(pwcLesson), (BYTE*)pwcLesson);
	data.addStringUnicode(wcslen(pwcDescription), (BYTE*)pwcDescription);
	data.addStringUnicode(wcslen(pwcURL), (BYTE*)pwcURL);
}

CGetRecordInfoOut::~CGetRecordInfoOut(void)
{
}

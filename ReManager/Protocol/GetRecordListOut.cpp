#include "stdafx.h"
#include "GetRecordListOut.h"

CGetRecordListOut::CGetRecordListOut(sync::syncCountRecordInfo& totalCountRec, unsigned int auRecordCount, sync::syncRecordInfo** appRecordsInfo)
{
	data.add(totalCountRec.totalCountRecord);
	data.add(auRecordCount);

	const wchar_t * pwcName;
	const wchar_t * pwcAuthor;
	const wchar_t * pwcCreationTime;
	const wchar_t * pwcLocation;
	const wchar_t * pwcLesson;
	const wchar_t * pwcDescription;
	const wchar_t * pwcURL;

	for (unsigned int i = 0; i < auRecordCount; ++i)
	{
		data.add( appRecordsInfo[i]->muRecordID);
		data.add( appRecordsInfo[i]->muReality);
		data.add( (unsigned int)appRecordsInfo[i]->mlDuration);

		pwcName = appRecordsInfo[i]->mpwcName.c_str();
		pwcAuthor = appRecordsInfo[i]->mpwcAuthor.c_str();
		pwcCreationTime = appRecordsInfo[i]->mpwcCreationTime.c_str();
		pwcLocation = appRecordsInfo[i]->mpwcLocation.c_str();
		pwcLesson = appRecordsInfo[i]->mpwcLesson.c_str();
		pwcDescription = appRecordsInfo[i]->mpwcDescription.c_str();
		pwcURL = appRecordsInfo[i]->mpwcURL.c_str();

		data.addStringUnicode(wcslen(pwcName), (BYTE*)pwcName);
		data.addStringUnicode(wcslen(pwcAuthor), (BYTE*)pwcAuthor);
		data.addStringUnicode(wcslen(pwcCreationTime), (BYTE*)pwcCreationTime);
		data.addStringUnicode(wcslen(pwcLocation), (BYTE*)pwcLocation);
		data.addStringUnicode(wcslen(pwcLesson), (BYTE*)pwcLesson);
		data.addStringUnicode(wcslen(pwcDescription), (BYTE*)pwcDescription);
		data.addStringUnicode(wcslen(pwcURL), (BYTE*)pwcURL);

		
	}
}

CGetRecordListOut::~CGetRecordListOut(void)
{
}

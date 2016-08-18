#include "stdafx.h"
#include "RecordEditorOpenIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordEditorOpenIn::CRecordEditorOpenIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize),
	MP_WSTRING_INIT(sRecordName),
	MP_WSTRING_INIT(sAuthor),
	MP_WSTRING_INIT(sLanguage),
	MP_WSTRING_INIT(sLocation),
	MP_WSTRING_INIT(sURL),
	MP_WSTRING_INIT(sCreationDate),
	MP_WSTRING_INIT(sRecordDescription)
{
}

bool CRecordEditorOpenIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx = 0;
	wchar_t* str;
	unsigned short len = 0;

	if(!memparse_object(pData, idx, iErrorCode))
		return false;

	if (!memparse_string_unicode(pData, idx, len, str))
		return false;
	sRecordName = str;

	if (!memparse_string_unicode(pData, idx, len, str))
		return false;
	sRecordDescription = str;

	if(!memparse_object(pData, idx, iRecordDuration))
		return false;

	if (!memparse_string_unicode(pData, idx, len, str))
		return false;
	sAuthor = str;

	if (!memparse_string_unicode(pData, idx, len, str))
		return false;
	sCreationDate = str;

	if (!memparse_string_unicode(pData, idx, len, str))
		return false;
	sLocation = str;

	if (!memparse_string_unicode(pData, idx, len, str))
		return false;
	sLanguage = str;

	if (!memparse_string_unicode(pData, idx, len, str))
		return false;
	sURL = str;

	return EndOfData( idx);
}

const int CRecordEditorOpenIn::GetErrorCode() const
{
	return iErrorCode;
}

const int CRecordEditorOpenIn::GetRecordDuration() const
{
	return iRecordDuration;
}

const std::wstring CRecordEditorOpenIn::GetRecordName() const
{
	return sRecordName;
}

const std::wstring CRecordEditorOpenIn::GetRecordDescription() const
{
	return sRecordDescription;
}

const std::wstring CRecordEditorOpenIn::GetRecordCreationDate() const
{
	return sCreationDate;
}

const std::wstring CRecordEditorOpenIn::GetRecordLanguage() const
{
	return sLanguage;
}

const std::wstring CRecordEditorOpenIn::GetRecordLocation() const
{
	return sLocation;
}

const std::wstring CRecordEditorOpenIn::GetRecordURL() const
{
	return sURL;
}

const std::wstring CRecordEditorOpenIn::GetRecordAuthor() const
{
	return sAuthor;
}

#include "stdafx.h"
#include "RecordStartedIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordConnectedIn::CRecordConnectedIn( BYTE* apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize),
	MP_WSTRING_INIT(mpwcLocationName),
	MP_WSTRING_INIT(mpwcAdditionalObjectInfo)
{
}

CRecordConnectedIn::~CRecordConnectedIn(){
}

bool CRecordConnectedIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, uiErrorCode))
		return false;
	if(!memparse_object(pData, idx, recordInfo.muRecordID))
		return false;
	if(!memparse_string(pData, idx, recordInfo.mpwcName))
		return false;
	if(!memparse_string(pData, idx, recordInfo.mpwcAuthor))
		return false;
	if(!memparse_object(pData, idx, recordInfo.muDepth))
		return false;
	if(!memparse_object(pData, idx, uiDuration))
		return false;
	if(!memparse_string(pData, idx, mpwcLocationName))
		return false;
	if(!memparse_string(pData, idx, mpwcAdditionalObjectInfo))
		return false;
	if(!memparse_object(pData, idx, isAuthorOrAssistant))
		return false;	
	return EndOfData( idx);
}


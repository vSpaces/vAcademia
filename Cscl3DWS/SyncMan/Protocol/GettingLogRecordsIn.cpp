#include "stdafx.h"
#include "GettingLogRecordsIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace SyncManProtocol{

CGettingLogRecordsIn::CGettingLogRecordsIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
	pRecordInfo = NULL;
	totalCountInfo.totalCountRecord = 0;
	totalCountInfo.totalCountRecordNow = 0;
	totalCountInfo.totalCountRecordWithoutStatus = 0;
	totalCountInfo.totalCountRecordSoon = 0;
}

CGettingLogRecordsIn::~CGettingLogRecordsIn()
{
	if (pRecordInfo!=NULL)
	{
		for (unsigned int i = 0;  i < iCountRecord;  i++)
		{
			if (pRecordInfo[i] == NULL) continue;
			/*DeleteString( pRecordInfo[i]->mpwcName);
			DeleteString( pRecordInfo[i]->mpwcDescription);
			DeleteString( pRecordInfo[i]->mpwcAuthor);
			DeleteString( pRecordInfo[i]->mpwcLocation);*/
			MP_DELETE( pRecordInfo[i]);
		}
	}
}

bool CGettingLogRecordsIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx = 0;
	memparse_object(pData, idx, pCallBack);
	memparse_object(pData, idx, iCountRecord);
	if(iCountRecord==0)
		return true;
	/*int size = sizeof(syncRecordInfo);
	if(iCountRecord*size>iDataSize)
		return false;*/
	pRecordInfo = MP_NEW_ARR( syncRecordInfo *, iCountRecord);
	for (unsigned int i=0; i<iCountRecord; i++)
		 pRecordInfo[i] = NULL;
	
	for (unsigned int i = 0;  i < iCountRecord;  i++)
	{
		pRecordInfo[i] = MP_NEW( syncRecordInfo);
		if(!memparse_object(pData, idx, pRecordInfo[i]->muRecordID))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->mpwcName))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->mpwcDescription))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->mpwcAuthor))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->mpwcAuthorFullName))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->mpwcLocation))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->mpwcPreview))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->mlDuration))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->muDepth))
			return false;	
		if(!memparse_object(pData, idx, pRecordInfo[i]->muAvatarInRecordCount))
			return false;	
		if(!memparse_object(pData, idx, pRecordInfo[i]->muVisitorCount))
			return false;	
		if(!memparse_object(pData, idx, pRecordInfo[i]->mbPublishNow))
			return false;	
		if(!memparse_object(pData, idx, pRecordInfo[i]->status))
			return false;	
		if(!memparse_object(pData, idx, pRecordInfo[i]->mlCreationTime))
			return false;	
	}
	memparse_object(pData, idx, totalCountInfo.totalCountRecord);
	/*memparse_object(pData, idx, totalCountInfo.totalCountRecordNow);
	memparse_object(pData, idx, totalCountInfo.totalCountRecordWithoutStatus);
	memparse_object(pData, idx, totalCountInfo.totalCountRecordSoon);*/
	return EndOfData( idx);
}

unsigned int CGettingLogRecordsIn::GetCountRecord() const {
	return iCountRecord;
}

syncCountRecordInfo CGettingLogRecordsIn::GetTotalCountRecord() const {
	return totalCountInfo;
}

}
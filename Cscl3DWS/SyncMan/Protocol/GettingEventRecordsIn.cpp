#include "stdafx.h"
#include "GettingEventRecordsIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace SyncManProtocol{

CGettingEventRecordsIn::CGettingEventRecordsIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
	pRecordInfo = NULL;
	totalCount = 0;
}

CGettingEventRecordsIn::~CGettingEventRecordsIn()
{
	if (pRecordInfo!=NULL)
	{
		for (unsigned int i = 0;  i < iCountRecord;  i++)
		{
			if (pRecordInfo[i] == NULL) continue;
			MP_DELETE( pRecordInfo[i]);
		}
	}
}

bool CGettingEventRecordsIn::Analyse()
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
	pRecordInfo = MP_NEW_ARR( syncRecordExtendedInfo *, iCountRecord);
	for (unsigned int i=0; i<iCountRecord; i++)
		 pRecordInfo[i] = NULL;
	
	for (unsigned int i = 0;  i < iCountRecord;  i++)
	{
		pRecordInfo[i] = MP_NEW( syncRecordExtendedInfo);		
		if(!memparse_object(pData, idx, pRecordInfo[i]->seanceID))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->recID))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->recRealityID))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->name))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->description))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->courseName))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->login))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->firstname))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->rFirstname))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->lastname))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->rLastname))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->locationURL))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->locationDescription))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->language))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->eventRecordID))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->post))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->eventDate))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->open))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->paid))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->depth))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->recordStatus))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->previewImage))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->loc_preview))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->courseEventsCount))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->courseEventNum))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->strTags))
			return false;

		// world event
		if(!memparse_object(pData, idx, pRecordInfo[i]->expected_length))
			return false;

		if(!memparse_object(pData, idx, pRecordInfo[i]->recordID))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->realityID))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->recordName))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->recordDescription))
			return false;
		
		if(!memparse_string(pData, idx, pRecordInfo[i]->recordDuration))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->fromStartDuration))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->brRecID))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->brRealityID))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->brRecordName))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->brRecordDescription))
			return false;
		if(!memparse_object(pData, idx, pRecordInfo[i]->brDepth))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->brDuration))
			return false;
		if(!memparse_string(pData, idx, pRecordInfo[i]->brPreviewImage))
			return false;		
	}
	memparse_object(pData, idx, totalCount);
	return EndOfData( idx);
}

unsigned int CGettingEventRecordsIn::GetCountRecord() const {
	return iCountRecord;
}

unsigned int CGettingEventRecordsIn::GetTotalCountRecord() const {
	return totalCount;
}

}
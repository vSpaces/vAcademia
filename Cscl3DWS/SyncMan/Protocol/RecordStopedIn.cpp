#include "stdafx.h"
#include "RecordStopedIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordStopedIn::CRecordStopedIn( BYTE* apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize),
	MP_WSTRING_INIT(mDescription),
	MP_WSTRING_INIT(mTags)
{
}

CRecordStopedIn::~CRecordStopedIn(){
}

bool CRecordStopedIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object<unsigned int>(pData, idx, muRecordID))
		return false;
	if(!memparse_object<unsigned int>(pData, idx, muDuration))
		return false;
	if( EndOfData( idx))
		return true;
	if(!memparse_string( pData, idx, mDescription))
		return false;
	if(!memparse_string( pData, idx, mTags))
		return false;
	return EndOfData( idx);
}


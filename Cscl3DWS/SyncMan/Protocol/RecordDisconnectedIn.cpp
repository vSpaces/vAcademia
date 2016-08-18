#include "stdafx.h"
#include "RecordDisconnectedIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordDisconnectedIn::CRecordDisconnectedIn( BYTE* apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

CRecordDisconnectedIn::~CRecordDisconnectedIn(){
}

bool CRecordDisconnectedIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object<unsigned int>(pData, idx, muRecordID))
		return false;
	if(!memparse_object(pData, idx, isAuthor))
		return false;
	if(!memparse_object(pData, idx, isNeedToAsk))
		return false;
	return EndOfData( idx);
}


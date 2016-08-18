#include "stdafx.h"
#include "ReceivedSeanceInfoIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CReceivedSeanceInfoIn::CReceivedSeanceInfoIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
//	pSeanceInfo = NULL;
}

bool CReceivedSeanceInfoIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	memparse_object(pData, idx, IDLogObject);
	if(!memparse_object(pData, idx, pSeanceInfo.muSeanceID))
				return false;
	if(!memparse_string(pData, idx, pSeanceInfo.mpwcName))
				return false;
	if(!memparse_string(pData, idx, pSeanceInfo.mpwcMembers))
				return false;
	if(!memparse_string(pData, idx, pSeanceInfo.mpwcAuthor))
				return false;
	if(!memparse_object(pData, idx, pSeanceInfo.mlDate))
				return false;
	if(!memparse_object(pData, idx, pSeanceInfo.mlMode))
				return false;
	if(!memparse_string(pData, idx, pSeanceInfo.mpwcURL))
		return false;


	return EndOfData( idx);
}


unsigned int CReceivedSeanceInfoIn::GetIDLogObject() const{
	return IDLogObject;
}

CReceivedSeanceInfoIn::~CReceivedSeanceInfoIn()
{
}

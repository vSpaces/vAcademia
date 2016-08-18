#include "stdafx.h"
#include "ReceivedSeancesIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CReceivedSeancesIn::CReceivedSeancesIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
	pSeanceInfo = NULL;
}

bool CReceivedSeancesIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	memparse_object(pData, idx, IDLogObject);
	memparse_object(pData, idx, iSeancesCount);
	if (iSeancesCount!=0)
	{
		/*int size = sizeof(syncSeanceInfo);
		if(iSeancesCount*size>iDataSize)
			return false;*/

		pSeanceInfo = MP_NEW_ARR( syncSeanceInfo *, iSeancesCount);
		for (unsigned int i=0; i<iSeancesCount; i++)
		{
			pSeanceInfo[i] = MP_NEW( syncSeanceInfo);
			if(!memparse_object(pData, idx, pSeanceInfo[i]->muSeanceID))
				return false;
			if(!memparse_string(pData, idx, pSeanceInfo[i]->mpwcName))
				return false;
			if(!memparse_string(pData, idx, pSeanceInfo[i]->mpwcMembers))
				return false;
			if(!memparse_string(pData, idx, pSeanceInfo[i]->mpwcAuthor))
				return false;
			if(!memparse_object(pData, idx, pSeanceInfo[i]->mlDate))
				return false;
			if(!memparse_object(pData, idx, pSeanceInfo[i]->mlMode))
				return false;
		}
		*(pSeanceInfo+iSeancesCount) = NULL;
	}
	return EndOfData( idx);
}

unsigned int CReceivedSeancesIn::GetSeancesCount()  const{
	return iSeancesCount;
}

unsigned int CReceivedSeancesIn::GetIDLogObject() const{
	return IDLogObject;
}

CReceivedSeancesIn::~CReceivedSeancesIn()
{
	if (pSeanceInfo!=NULL)
	for (unsigned int i=0; i<iSeancesCount; i++)
	{
		if (pSeanceInfo[i]==NULL)
			continue;
		/*if (pSeanceInfo[i]->mpwcName!=NULL) {
			delete(pSeanceInfo[i]->mpwcName); pSeanceInfo[i]->mpwcName = NULL; break; }
		if (pSeanceInfo[i]->mpwcMembers!=NULL) {
			delete(pSeanceInfo[i]->mpwcMembers); pSeanceInfo[i]->mpwcMembers = NULL; break; }
		if (pSeanceInfo[i]->mpwcAuthor!=NULL) {
			delete(pSeanceInfo[i]->mpwcAuthor); pSeanceInfo[i]->mpwcAuthor = NULL; break; }*/
		MP_DELETE( pSeanceInfo[i]); 

	}
}

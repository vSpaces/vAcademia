#include "stdafx.h"
#include "SeanceLoadedIn.h"

using namespace SyncManProtocol;

CSeanceLoadedIn::~CSeanceLoadedIn(){
}

bool CSeanceLoadedIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, recordID))
		return false;
	if(!memparse_object(pData, idx, seanceID))
		return false;
	if(!memparse_object(pData, idx, isAuthor))
		return false;
	if(!memparse_object(pData, idx, isSuspended))
		return false;
	if(!memparse_object(pData, idx, m_minutes))
		return false;
	return EndOfData( idx);

}

unsigned int CSeanceLoadedIn::GetRecordID() const
{
	return recordID;
}

int CSeanceLoadedIn::GetMinutes() const
{
	return m_minutes;
}

unsigned int CSeanceLoadedIn::GetSeanceID() const
{
	return seanceID;
}

unsigned char CSeanceLoadedIn::IsAuthor() const
{
	return isAuthor;
}

unsigned char CSeanceLoadedIn::IsSuspended() const
{
	return isSuspended;
}


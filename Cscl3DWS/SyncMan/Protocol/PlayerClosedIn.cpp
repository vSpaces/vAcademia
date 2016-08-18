#include "stdafx.h"
#include "PlayerClosedIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

bool CPlayerClosedIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, seanceID))
		return false;
	return EndOfData( idx);
}

unsigned int CPlayerClosedIn::GetSeanceID() const{
	return seanceID;
}
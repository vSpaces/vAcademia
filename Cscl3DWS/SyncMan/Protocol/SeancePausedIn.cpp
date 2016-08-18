#include "stdafx.h"
#include "SeancePausedIn.h"

namespace SyncManProtocol {

CSeancePausedIn::~CSeancePausedIn(){
}

bool CSeancePausedIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, currentRealityID))
		return false;
	return EndOfData( idx);

}

unsigned int CSeancePausedIn::GetCurrentRealityID() const
{
	return currentRealityID;
}


}
#include "stdafx.h"
#include "../../include/RecordServer/Protocol/StartPlayingQueryOut.h"

CStartPlayingQueryOut::CStartPlayingQueryOut(unsigned int aRecordID, unsigned int aRealityID)
{
	GetDataBuffer().addData( &aRecordID, sizeof(aRecordID));
	GetDataBuffer().addData( &aRealityID, sizeof(aRealityID));
}
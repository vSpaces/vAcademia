#include "stdafx.h"
#include "GetVoipServerForRecordQueryOut.h"

CGetVoipServerForRecordQueryOut::CGetVoipServerForRecordQueryOut( unsigned int aiPlayRealityID,
																 unsigned int aiPlayingRecordID)
{
	GetDataBuffer().addData( &aiPlayRealityID, sizeof(aiPlayRealityID));
	GetDataBuffer().addData( &aiPlayingRecordID, sizeof(aiPlayingRecordID));
}
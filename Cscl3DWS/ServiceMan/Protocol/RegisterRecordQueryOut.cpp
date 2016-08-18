#include "stdafx.h"
#include "RegisterRecordQueryOut.h"

CRegisterRecordQueryOut::CRegisterRecordQueryOut(LPCSTR alpcVoipServerAddress, unsigned int aRecordID)
{
	ATLASSERT( alpcVoipServerAddress);

	USES_CONVERSION;
	GetDataBuffer().addString( A2W(alpcVoipServerAddress));
	GetDataBuffer().addData( &aRecordID, sizeof(aRecordID));
}
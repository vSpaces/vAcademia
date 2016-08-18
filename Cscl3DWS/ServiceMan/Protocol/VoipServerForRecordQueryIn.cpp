#include "stdafx.h"
#include "VoipServerForRecordQueryIn.h"


CVoipServerForRecordQueryIn::CVoipServerForRecordQueryIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize)
{
}

bool CVoipServerForRecordQueryIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	// 1. recordID - 4 bytes - номер записи
	if ( !memparse_object( pData, idx, recordID))
		return false;

	// 2. VoipServer - UnicodeString
	if (!memparse_string_unicode( pData, idx, recordServerIP))
		return false;

	// 3. VoipServerPort - 4 bytes - номер записи
	if ( !memparse_object( pData, idx, recordServerPort))
		return false;

	// 4. VoipPlayServer - UnicodeString
	if (!memparse_string_unicode( pData, idx, playServerIP))
		return false;

	// 5. VoipPlayServerPort - 4 bytes - номер записи
	if ( !memparse_object( pData, idx, playServerPort))
		return false;

	return EndOfData( idx);
}

unsigned int		CVoipServerForRecordQueryIn::GetRecordID()
{
	return recordID;
}

CComString		CVoipServerForRecordQueryIn::GetRecordServerIP()
{
	return recordServerIP;
}

unsigned int		CVoipServerForRecordQueryIn::GetRecordServerPort()
{
	return recordServerPort;
}

CComString		CVoipServerForRecordQueryIn::GetPlayServerIP()
{
	return playServerIP;
}

unsigned int		CVoipServerForRecordQueryIn::GetPlayServerPort()
{
	return playServerPort;
}
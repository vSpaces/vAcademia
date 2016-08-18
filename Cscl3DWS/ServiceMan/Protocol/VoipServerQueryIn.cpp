#include "stdafx.h"
#include "VoipServerQueryIn.h"


CVoipServerQueryIn::CVoipServerQueryIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize)
{
}

bool CVoipServerQueryIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	byte nameType = 0;

	// 1. VoipServer - [4 bytes - адрес сервера голосовой связи] | [UnicodeString]
	if (!memparse_string_unicode( pData, idx, voipServerName))
		return false;

	// 2. TcpPort - 2 bytes - порт
	if ( !memparse_object( pData, idx, tcpPort))
		return false;

	// 3. UdpPort - 2 bytes - порт
	if ( !memparse_object( pData, idx, udpPort))
		return false;

	// 4. VoipRecordServer - [4 bytes - адрес записывающего сервера голосовой связи] | [UnicodeString]
	if (!memparse_string_unicode( pData, idx, voipRecordServerName))
		return false;

	// 5. RecordPort - 2 bytes - порт
	if ( !memparse_object( pData, idx, recordPort))
		return false;

	// 6. Имя комнаты UnicodeString
	if (!memparse_string_unicode( pData, idx, roomName))
		return false;

	return EndOfData( idx);
}

CComString		CVoipServerQueryIn::GetVoipHostName()
{
	return voipServerName;
}

CComString		CVoipServerQueryIn::GetRecordHostName()
{
	return voipRecordServerName;
}

WORD			CVoipServerQueryIn::GetTcpPort()
{
	return tcpPort;
}
WORD			CVoipServerQueryIn::GetUdpPort()
{
	return udpPort;
}
WORD			CVoipServerQueryIn::GetRecordPort()
{
	return recordPort;
}

CComString		CVoipServerQueryIn::GetRoomName()
{
	return roomName;
}
#include "stdafx.h"
#include "../../include/Protocol/VoipServersQueryIn.h"


CVoipServersQueryIn::CVoipServersQueryIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize)
{
}

bool CVoipServersQueryIn::Analyse()
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

CComString		CVoipServersQueryIn::GetVoipHostName()
{
	return voipServerName;
}

CComString		CVoipServersQueryIn::GetRecordHostName()
{
	return voipRecordServerName;
}

WORD			CVoipServersQueryIn::GetTcpPort()
{
	return tcpPort;
}
WORD			CVoipServersQueryIn::GetUdpPort()
{
	return udpPort;
}
WORD			CVoipServersQueryIn::GetRecordPort()
{
	return recordPort;
}

CComString		CVoipServersQueryIn::GetRoomName()
{
	return roomName;
}
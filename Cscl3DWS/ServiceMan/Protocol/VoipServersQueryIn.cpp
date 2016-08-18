#include "stdafx.h"
#include "VoipServersQueryIn.h"


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
	unsigned short nNameLength;
	wchar_t* pNameData;
	if (!memparse_string_unicode( pData, idx, nNameLength, pNameData))
		return false;

	wchar_t* buf = MP_NEW_ARR( wchar_t, 2*(nNameLength + 1));
	memcpy( (void*)buf, pNameData, nNameLength*2);
	voipServerName = CComString( buf);
	MP_DELETE_ARR( buf);

	// 2. TcpPort - 2 bytes - порт
	if ( !memparse_object( pData, idx, tcpPort))
		return false;

	// 3. UdpPort - 2 bytes - порт
	if ( !memparse_object( pData, idx, udpPort))
		return false;

	// 4. VoipRecordServer - [4 bytes - адрес записывающего сервера голосовой связи] | [UnicodeString]
	if (!memparse_string_unicode( pData, idx, nNameLength, pNameData))
		return false;

	buf = MP_NEW_ARR( wchar_t, 2*(nNameLength + 1));
	memcpy( (void*)buf, pNameData, nNameLength*2);
	voipRecordServerName = CComString( buf);
	MP_DELETE_ARR( buf);

	// 5. RecordPort - 2 bytes - порт
	if ( !memparse_object( pData, idx, recordPort))
		return false;

	// 6. Имя комнаты UnicodeString
	if (!memparse_string_unicode( pData, idx, nNameLength, pNameData))
		return false;
	buf = MP_NEW_ARR( wchar_t, 2*(nNameLength + 1));
	memcpy( (void*)buf, pNameData, nNameLength*2);
	roomName = CComString( buf);
	MP_DELETE_ARR( buf);

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
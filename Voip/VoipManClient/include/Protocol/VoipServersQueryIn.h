#pragma once
#include "CommonPacketIn.h"

/************************************************************************/
/* ќтвет от главного сервера голосовой св€зи. ƒанные:
1. VoipServer - [4 bytes - адрес сервера голосовой св€зи] | [UnicodeString]
2. TcpPort - 2 bytes - порт
3. UdpPort - 2 bytes - порт
4. VoipRecordServer - [4 bytes - адрес записывающего сервера голосовой св€зи] | [UnicodeString]
5. RecordPort - 2 bytes - порт
6. »м€ комнаты UnicodeString
/************************************************************************/
class CVoipServersQueryIn : public Protocol::CCommonPacketIn
{
public:
	CVoipServersQueryIn( BYTE *aData, int aDataSize);
	virtual bool Analyse();

public:
	CComString		GetRoomName();
	CComString		GetVoipHostName();
	CComString		GetRecordHostName();

	WORD			GetTcpPort();
	WORD			GetUdpPort();
	WORD			GetRecordPort();

protected:
	CComString		roomName;
	CComString		voipServerName;
	CComString		voipRecordServerName;

	WORD			tcpPort;
	WORD			udpPort;
	WORD			recordPort;
};

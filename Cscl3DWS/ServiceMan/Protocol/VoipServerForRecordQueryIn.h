#pragma once
#include "CommonPacketIn.h"

/************************************************************************/
/* Ответ от главного сервера голосовой связи. Данные:
1. Номер записи
2. VoipServer - [4 bytes - адрес сервера голосовой связи] | [UnicodeString]
/************************************************************************/
class CVoipServerForRecordQueryIn : public Protocol::CCommonPacketIn
{
public:
	CVoipServerForRecordQueryIn( BYTE *aData, int aDataSize);
	virtual bool Analyse();

public:
	CComString		GetRecordServerIP();
	unsigned int	GetRecordServerPort();
	CComString		GetPlayServerIP();
	unsigned int	GetPlayServerPort();
	unsigned int	GetRecordID();

protected:
	CComString		recordServerIP;
	unsigned int	recordServerPort;
	CComString		playServerIP;
	unsigned int	playServerPort;
	unsigned int	recordID;
};
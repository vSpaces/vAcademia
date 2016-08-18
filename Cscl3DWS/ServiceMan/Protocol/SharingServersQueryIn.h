#pragma once
#include "CommonPacketIn.h"

class CSharingServersQueryIn : public Protocol::CCommonPacketIn
{
public:
	CSharingServersQueryIn( BYTE *aData, int aDataSize);
	virtual bool Analyse();

public:
	std::wstring	GetGroupName();
	std::wstring	GetSharingServerIP();
	WORD			GetTcpPort();
	unsigned int	GetRecordID();

protected:
	MP_WSTRING		serverIP;
	WORD			tcpPort;
	MP_WSTRING		groupName;
	unsigned int	recordID;
};

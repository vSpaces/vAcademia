#pragma once
#include "CommonPacketIn.h"

class CPreviewServersQueryIn : public Protocol::CCommonPacketIn
{
public:
	CPreviewServersQueryIn( BYTE *aData, int aDataSize);
	virtual bool Analyse();

public:
	CComString		GetRoomName();
	CComString		GetPreviewServerIP();
	WORD			GetTcpPort();
	unsigned int	GetRecordID();
	CComString		GetPreviewHttpHost();
	CComString		GetStoragePath();

protected:
	unsigned int	recordID;
	CComString		PreviewServerIP;
	WORD			tcpPort;
	CComString		PreviewHttpHost;
	CComString		StoragePath;
};

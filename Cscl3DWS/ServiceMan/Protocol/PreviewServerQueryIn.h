#pragma once
#include "CommonPacketIn.h"

class CPreviewServerQueryIn : public Protocol::CCommonPacketIn
{
public:
	CPreviewServerQueryIn( BYTE *aData, int aDataSize);
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

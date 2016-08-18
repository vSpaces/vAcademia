#pragma once
#include "CommonPipePacketIn.h"
#include "proxyInfo.h"

class CConnectToEVoipIn : public CCommonPipePacketIn
{
	MP_STRING sServerIP;
	
	int tcpPort;
	int udpPort;
	bool bCheckConnecting;
public:
	CConnectToEVoipIn(BYTE *aData, int aDataSize);
	virtual bool Analyse();
	std::string GetServerIP();
	std::vector<ProxyInfo> GetProxySettings();

	int GetTcpPort();
	int GetUdpPort();
	bool GetCheckConnecting();

private:
	MP_VECTOR<ProxyInfo> vProxyInfoList;
};
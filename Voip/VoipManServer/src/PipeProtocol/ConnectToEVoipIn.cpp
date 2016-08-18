#include "stdafx.h"
#include "../../include/PipeProtocol/ConnectToEVoipIn.h"


CConnectToEVoipIn::CConnectToEVoipIn(BYTE *aData, int aDataSize) : 
		CCommonPipePacketIn(aData, aDataSize)
		, MP_STRING_INIT(sServerIP)
		, MP_VECTOR_INIT(vProxyInfoList)
{
}

bool CConnectToEVoipIn::Analyse()
{
	if ( !CCommonPipePacketIn::Analyse())
		return false;

	if(!data->Read((std::string&)sServerIP))
		return false;
		
	if(!data->Read(tcpPort))
		return false;
	
	if(!data->Read(udpPort))
		return false;

	byte btCheckConnecting = 0;
	if(!data->Read(btCheckConnecting))
		return false;
	bCheckConnecting = btCheckConnecting != 0;

	unsigned int sizeProxySettings = 0;

	if(!data->Read(sizeProxySettings))
		return false;

	for (unsigned int i=0; i<sizeProxySettings; i++)
	{
		ProxyInfo proxyInfo;
		if(!data->Read((std::wstring&)proxyInfo.sProxyIP))
			return false;
		if(!data->Read((std::wstring&)proxyInfo.sProxyUserName))
			return false;
		if(!data->Read((std::wstring&)proxyInfo.sProxyPassword))
			return false;

		if(!data->Read(proxyInfo.proxyType))
			return false;

		vProxyInfoList.push_back(proxyInfo);
	}

	return EndOfData();
}

std::string CConnectToEVoipIn::GetServerIP()
{
	return sServerIP;
}

int CConnectToEVoipIn::GetTcpPort()
{
	return tcpPort;
}

int CConnectToEVoipIn::GetUdpPort()
{
	return udpPort;
}

bool CConnectToEVoipIn::GetCheckConnecting()
{
	return bCheckConnecting;
}

std::vector<ProxyInfo> CConnectToEVoipIn::GetProxySettings()
{
	return (std::vector<ProxyInfo>)vProxyInfoList;}
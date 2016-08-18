#include "stdafx.h"
#include "include/PipeProtocol/ConnectToEVoipOut.h"

CConnectToEVoipOut::CConnectToEVoipOut(byte aID,  const char *aServerIP, std::vector<IProxySettings*>* aProxySettingsVec, int aTcpPort, int anUdpPort, bool aCheckConnecting) : CCommonPipePacketOut( aID)
{
	data->Add( aServerIP);
		
	data->Add( aTcpPort);
	data->Add( anUdpPort);
	byte btCheckConnecting =(byte)aCheckConnecting;
	data->Add( btCheckConnecting);

	unsigned int proxyListSize = (unsigned int)aProxySettingsVec->size();
	data->Add(proxyListSize);

	std::vector<IProxySettings*>::iterator it = aProxySettingsVec->begin();
	std::vector<IProxySettings*>::iterator itEnd = aProxySettingsVec->end();

	for (; it != itEnd; it++)
	{
		std::wstring proxyIP = (*it)->getIP();
		if (proxyIP != L"")
		{
			USES_CONVERSION;
			char convBuf[100];
			proxyIP = proxyIP + L":";
			_itoa_s((*it)->getPort(), convBuf, 100, 10);
			std::wstring wPort = A2W(convBuf);
			proxyIP = proxyIP + wPort;
		}

		data->Add( proxyIP);
		data->Add( (*it)->getUser());
		data->Add( (*it)->getPassword());
		unsigned short typeProxy = (*it)->getType();
		data->Add( typeProxy);
	}
}
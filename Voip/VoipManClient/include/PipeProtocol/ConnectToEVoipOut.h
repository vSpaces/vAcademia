#pragma once
#include "CommonPipePacketOut.h"

class CConnectToEVoipOut : public CCommonPipePacketOut
{
public:
	CConnectToEVoipOut(byte aID,  const char *aServerIP, std::vector<IProxySettings*>* aProxySettingsVec, int aTcpPort, int anUdpPort, bool aCheckConnecting);
};
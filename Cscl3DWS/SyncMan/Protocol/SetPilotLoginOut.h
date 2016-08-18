#pragma once

namespace SyncManProtocol
{

class CSetPilotLogin : public CCommonPacketOut
{
public:
	CSetPilotLogin( const wchar_t* alpwcPilotLogin, unsigned int auEventID);
};

};
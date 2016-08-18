#pragma once
#include "updatepacketIn.h"
#include "updatepacketincontainer.h"

using namespace SyncManProtocol;

class CUpdatePacketHandler
{
private:
	CUpdatePacketIn *packetIn;
	CUpdatePacketInContainer *updatePacketContainer;
	void GettingsProperties(CPropertyList *aPropertyList, unsigned int propCount);

public:
	CUpdatePacketHandler();
	virtual ~CUpdatePacketHandler();
	void Handle(CUpdatePacketIn *packetIn);
	bool GetContainer(syncIUpdatePacketIn* &aUpdatePacketContainer);
};

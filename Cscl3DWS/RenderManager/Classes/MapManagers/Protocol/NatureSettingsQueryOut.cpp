#include "stdafx.h"
#include "NatureSettingsQueryOut.h"

using namespace MapManagerProtocol;

CNatureSettingsQueryOut::CNatureSettingsQueryOut(unsigned int aZoneID) :CMapServerPacketOutBase()
{
	Add(aZoneID);
}

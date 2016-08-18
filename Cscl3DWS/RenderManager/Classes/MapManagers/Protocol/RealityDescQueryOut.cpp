#include "stdafx.h"
#include "realitydescqueryout.h"

using namespace MapManagerProtocol;

CRealityDescQueryOut::CRealityDescQueryOut(unsigned int aRealityID) :
	CMapServerPacketOutBase()
{
	Add(aRealityID);
}

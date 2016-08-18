#include "stdafx.h"
#include "DeleteObjectsByIDQueryOut.h"

using namespace MapManagerProtocol;

CDeleteObjectsByIDQueryOut::CDeleteObjectsByIDQueryOut(int objCount, unsigned int objectID, unsigned int realityID) :
	CMapServerPacketOutBase()
{
	Add(objCount);
	Add(objectID);
	Add(realityID);
}

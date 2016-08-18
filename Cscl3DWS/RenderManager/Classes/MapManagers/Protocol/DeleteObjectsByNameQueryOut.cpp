#include "stdafx.h"
#include "DeleteObjectsByNameQueryOut.h"

using namespace MapManagerProtocol;

CDeleteObjectsByNameQueryOut::CDeleteObjectsByNameQueryOut(int objCount, CComString &aName) :
	CMapServerPacketOutBase()
{
	Add(objCount);
	Add(aName);
}

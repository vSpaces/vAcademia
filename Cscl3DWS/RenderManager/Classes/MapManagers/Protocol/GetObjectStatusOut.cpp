#include "stdafx.h"
#include "GetObjectStatusOut.h"

using namespace MapManagerProtocol;

CGetObjectStatusOut::CGetObjectStatusOut(unsigned int auRealityID, unsigned int auObjectID, unsigned int aBornRealityID){
	Add(auRealityID);
	Add(auObjectID);
	Add(aBornRealityID);
}


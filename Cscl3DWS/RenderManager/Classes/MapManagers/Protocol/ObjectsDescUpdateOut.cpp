#include "stdafx.h"
#include "ObjectsDescUpdateOut.h"

using namespace MapManagerProtocol;

CObjectsDescUpdateOut::CObjectsDescUpdateOut(int objCount, unsigned int auRmmlQueryID, std::string& name, std::string& res, float x, float y, float z, 
												   float bx, float by, float bz, float rx, float ry, float rz, float rw,
												   float scalex, float scaley, float scalez, 
												   unsigned int zoneID,
												   std::string& className, std::wstring& userProps, int type, 
												   unsigned int objectID, unsigned int bornRealityID, int level, std::string& location) :
	CMapServerPacketOutBase( objCount * 512)
{
	Add(objCount);
	Add(auRmmlQueryID);
	for(int i=0;i<objCount;i++){
		Add(name);
		Add(res);
		Add(x);
		Add(y);
		Add(z);
		Add(bx);
		Add(by);
		Add(bz);
		Add(rx);
		Add(ry);
		Add(rz);
		Add(rw);
		Add(scalex);
		Add(scaley);
		Add(scalez);
		Add(zoneID);
		Add(className);
		Add(userProps.c_str());
		Add(type);
		Add(objectID);
		Add(bornRealityID);
		Add(level);
		Add(location);
	}
}


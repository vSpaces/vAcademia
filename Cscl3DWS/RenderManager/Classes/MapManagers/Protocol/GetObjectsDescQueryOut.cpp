#include "stdafx.h"
#include "GetObjectsDescQueryOut.h"

using namespace MapManagerProtocol;

CGetObjectsDescQueryOut::CGetObjectsDescQueryOut(unsigned int aRealityID,
								unsigned int aCount, cm::cmObjectIDs *aObjects)
{
	Add(aRealityID);
	Add(aCount);
	for (unsigned int i = 0;  i < aCount;  i++)
	{
		Add( aObjects[i].objectID);
		Add( aObjects[i].bornRealityID);
	}
}
#include "stdafx.h"
#include "ObjectsDescIn.h"

using namespace MapManagerProtocol;

CObjectsDescIn::CObjectsDescIn(BYTE *aData, int aDataSize, bool aOld_server) : CCommonGetObjectsDescQueryIn(aData, aDataSize, aOld_server)
{
}


unsigned short CObjectsDescIn::GetObjectCountByPos()
{
	unsigned short objectCount = 0;

	int idx = sizeof(unsigned int)*3;

	memparse_object(pData, idx, objectCount);

	return objectCount;
}
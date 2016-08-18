#include "stdafx.h"
#include "GetZonesObjectsDescQueryIn.h"
//#include "qpsort.h"

using namespace MapManagerProtocol;

CGetZonesObjectsDescQueryIn::CGetZonesObjectsDescQueryIn(BYTE *aData, int aDataSize, bool aOld_server) : CCommonGetObjectsDescQueryIn(aData, aDataSize, aOld_server)
{
}
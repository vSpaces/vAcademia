#include "stdafx.h"
#include "QueryLatentedObjectsOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CQueryLatentedObjectsOut::CQueryLatentedObjectsOut()
{
	data.clear();
	count = 0;
	data.addData(sizeof( count), (BYTE*)&count);
}

void CQueryLatentedObjectsOut::SaveCount()
{
	data.setData( (BYTE*)&count, sizeof( count), 0, 0);
}

void CQueryLatentedObjectsOut::AddObject(unsigned int aObjectID, unsigned int aBornRealityID, unsigned int aCurrentRealityID, unsigned int auiHash)
{
	data.addData(sizeof( aObjectID), (BYTE*)&aObjectID);
	data.addData(sizeof( aBornRealityID), (BYTE*)&aBornRealityID);
	data.addData(sizeof( aCurrentRealityID), (BYTE*)&aCurrentRealityID);
	data.addData(sizeof( auiHash), (BYTE*)&auiHash);
	count++;
}

/*CQueryLatentedObjectsOut::Clear()
{
	count = 0;
	data.clear();
}*/
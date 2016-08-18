#include "stdafx.h"
#include "SyncMessageOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CSyncMessageOut::CSyncMessageOut(
	unsigned int anObjectID, unsigned int aBornRealityID,
	unsigned char* senderName, unsigned short senderNameLength,
	unsigned char* aData, unsigned short aDataSize)
{
	data.clear();

	data.addData(sizeof(unsigned int), (BYTE*)&anObjectID);
	data.addData(sizeof(unsigned int), (BYTE*)&aBornRealityID);
	data.addData(sizeof(unsigned short), (BYTE*)&senderNameLength);
	if(senderNameLength>0){
		data.addData(senderNameLength*2, (BYTE*)senderName);
	}
	data.addData(sizeof(unsigned short), (BYTE*)&aDataSize);
	data.addData(aDataSize, (BYTE*)aData);
}

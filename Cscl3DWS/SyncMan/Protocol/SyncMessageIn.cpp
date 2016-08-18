#include "stdafx.h"
#include "SyncMessageIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CSyncMessageIn::CSyncMessageIn(BYTE *aData, int aDataSize):
	CCommonPacketIn(aData, aDataSize),
	MP_WSTRING_INIT(pSenderName)
{
	//pSenderName = NULL;
	iLocalDataSize = 0;
	iSenderNameLen = 0;
}

CSyncMessageIn::~CSyncMessageIn()
{
	//delete(pSenderName);pSenderName=NULL;
}

bool CSyncMessageIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, iObjectID))
		return false;

	if(!memparse_object(pData, idx, iBornRealityID))
		return false;

	if(!memparse_string(pData, idx, pSenderName))
		return false;

	iLocalDataSize = idx;
	iLocalDataSize = memparse_bytes(pData, idx, &mpData);
	if (iLocalDataSize == 0)
		return false;

	return EndOfData( idx);
}

unsigned int CSyncMessageIn::GetObjectID(){
	return iObjectID;
}

unsigned int CSyncMessageIn::GetBornRealityID(){
	return iBornRealityID;
}

const wchar_t* CSyncMessageIn::GetSenderName(){
	return pSenderName.c_str();
}
unsigned char *CSyncMessageIn::GetData(){
	return mpData;
}
unsigned int CSyncMessageIn::GetDataLength(){
	return iLocalDataSize;
}

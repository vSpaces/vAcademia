#include "stdafx.h"
#include "QueryMessageIn.h"

using namespace ResManProtocol;

CQueryMessageIn::CQueryMessageIn(CDataBuffer2 *aData)
{
	data = aData;
	iMessageData = 0;
}

bool CQueryMessageIn::Analyse()
{
	if(!data->read(iMessageData))
		return false;
	
	return true;
}

unsigned int CQueryMessageIn::GetMessageSize()
{
	return iMessageData;
}

BYTE *CQueryMessageIn::GetMessageData(BOOL *bError)
{
	//BYTE *pData = (BYTE*)GlobalAlloc( GPTR, iMessageData);
	BYTE *pData = MP_NEW_ARR( BYTE, iMessageData);	
	*bError = !data->readData( &pData, iMessageData, FALSE);
	return pData;
}
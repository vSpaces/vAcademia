#include "stdafx.h"
#include "QueryPackageIn.h"

using namespace ResManProtocol;

CQueryPackageIn::CQueryPackageIn(BYTE *aData, int aDataSize) :
	CCommonPacketIn(aData, aDataSize)
{
	//queryNum = 0;
	pPacketData = NULL;
	sizePackage = 0;
}

CQueryPackageIn::~CQueryPackageIn()
{
	if( pPacketData)
		MP_DELETE( pPacketData);
}

bool CQueryPackageIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	sizePackage = 0;
	if(!memparse_object(pData, idx, sizePackage))
		return false;
	if(iDataSize<idx+sizePackage)
		return false;
	/*if(!memparse_object(pData, idx, queryNum))
		return false;*/

	if( pPacketData)
		MP_DELETE( pPacketData);
 
	MP_NEW_V2( pPacketData, CDataBuffer2, pData, sizePackage + sizeof( sizePackage));
	pPacketData->shiftl( sizeof( sizePackage)/* + sizeof( queryNum)*/);
	pPacketData->reset();
	return true;
}

/*unsigned int CQueryPackageIn::GetQueryNum()
{
	return queryNum;
}*/

CDataBuffer2 *CQueryPackageIn::DetachPacketData()
{
	CDataBuffer2* ret = pPacketData;
	pPacketData = NULL;
	return ret;
}

unsigned int CQueryPackageIn::GetPackageSize()
{
	return sizePackage;
}
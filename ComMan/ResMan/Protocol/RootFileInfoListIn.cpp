#include "stdafx.h"
#include "..\..\ComMan.h"
#include "RootFileInfoListIn.h"

#include "res_consts.h"
#include "ZLIBENGN.H"

using namespace res;
using namespace ResManProtocol;

CResInfoSaver resRootInfoList;

CRootFileInfoListIn::CRootFileInfoListIn(BYTE *aData, int aDataSize) : CCommonPacketIn(aData, aDataSize)
{
	lDateTime = 0;
	count = 0xffffffff;
	unpackBuffer = NULL;
}

CRootFileInfoListIn::~CRootFileInfoListIn()
{
	if(unpackBuffer!=NULL)
		MP_DELETE_ARR( unpackBuffer);
}

bool CRootFileInfoListIn::GetFileInfo(int index, CResInfoSaver *resRootInfoList, bool bGetData)
{
	if(index>-1){
		if((int)vPosFileInfo.size()<=index)
			return false;
		idx = vPosFileInfo.at(index);
	}
	else if(index!=-1)
		return false;

	CComString foo;
	USES_CONVERSION;
	if(memparse_string(unpackBuffer, idx, foo, bGetData))
		resRootInfoList->info.sResPath = A2W( foo.GetBuffer());
	else
		return false;

	resRootInfoList->info.dwStatus = RES_REMOTE;
	resRootInfoList->info.dwSize = 0;
	if(!memparse_object(unpackBuffer, idx, resRootInfoList->info.dwSize, bGetData))
		return false;
	lDateTime = 0;
	if(!memparse_object(unpackBuffer, idx, lDateTime, bGetData))
		return false;
	if(bGetData)
	{
		resRootInfoList->info.oTime.dwLowDateTime = (DWORD)lDateTime;
		resRootInfoList->info.oTime.dwHighDateTime = lDateTime >> 32;
		SYSTEMTIME stUTC;
		FileTimeToSystemTime(&resRootInfoList->info.oTime, &stUTC);
		if(stUTC.wYear > 3500)
		{
			stUTC.wYear -=1600;
			SystemTimeToFileTime(&stUTC, &resRootInfoList->info.oTime);
		}
	//}
	}
	return true;
}

bool CRootFileInfoListIn::Analyse()
{
	if(!CCommonPacketIn::Analyse())
		return false;
	
	unsigned long messageSize = iDataSize - 4;

	uLong unpackedSize = (unsigned long)(*(unsigned int*)(pData));
	unpackBuffer = MP_NEW_ARR( BYTE, unpackedSize);

	uncompress((Bytef*)unpackBuffer, &unpackedSize, (Bytef*)(pData+4), messageSize);
	
	iDataSize = unpackedSize;

	idx = 0;

	if(!memparse_object(unpackBuffer, idx, count))
		return false;
	
	for(DWORD i=0;i<count;i++)
	{
		int posFileInfo = idx;
		if(!GetFileInfo(-1, &resRootInfoList, false))
			return false;
		vPosFileInfo.push_back(posFileInfo);
	}
	return (idx<=(int)iDataSize);
	//return EndOfData(idx);
}

DWORD CRootFileInfoListIn::GetCount(){
	return count;
}
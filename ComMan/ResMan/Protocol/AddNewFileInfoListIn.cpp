#include "stdafx.h"
#include "..\..\ComMan.h"
#include "AddNewFileInfoListIn.h"

#include "res_consts.h"
#include "ZLIBENGN.H"

using namespace res;
using namespace ResManProtocol;

CResInfoSaver resNewInfoList;

CAddNewFileInfoListIn::CAddNewFileInfoListIn(BYTE *aData, int aDataSize) : CCommonPacketIn(aData, aDataSize)
{
	lDateTime = 0;
	count = 0xffffffff;
	unpackBuffer = NULL;
}

CAddNewFileInfoListIn::~CAddNewFileInfoListIn()
{
	if(unpackBuffer!=NULL)
		MP_DELETE_ARR( unpackBuffer);
}

bool CAddNewFileInfoListIn::GetFileInfo(int index, CResInfoSaver *resNewInfoList, bool bGetData)
{
	if(index>-1){
		if((int)(vPosFileInfo.size())<=index)
			return false;
		idx = vPosFileInfo.at(index);
	}
	else if(index!=-1)
		return false;

	CComString foo;
	USES_CONVERSION;
	if(memparse_string(unpackBuffer, idx, foo, bGetData))
		resNewInfoList->info.sResPath = A2W( foo.GetBuffer());
	else
		return false;

	resNewInfoList->info.dwStatus = RES_REMOTE;
	resNewInfoList->info.dwSize = 0;
	if(!memparse_object(unpackBuffer, idx, resNewInfoList->info.dwSize, bGetData))
		return false;
	lDateTime = 0;
	if(!memparse_object(unpackBuffer, idx, lDateTime, bGetData))
		return false;
	if(bGetData)
	{
		resNewInfoList->info.oTime.dwLowDateTime = (DWORD)lDateTime;
		resNewInfoList->info.oTime.dwHighDateTime = lDateTime >> 32;
		SYSTEMTIME stUTC;
		FileTimeToSystemTime(&resNewInfoList->info.oTime, &stUTC);
		if(stUTC.wYear > 3500)
		{
			stUTC.wYear -=1600;
			SystemTimeToFileTime(&stUTC, &resNewInfoList->info.oTime);
		}
	//}
	}
	return true;
}

bool CAddNewFileInfoListIn::Analyse( ILogWriter* apLW)
{
	if(!CCommonPacketIn::Analyse())
	{
		apLW->WriteLn("CAddNewFileInfoListIn::Analyse failed: 1");
		return false;
	}
	
	/*unsigned long messageSize = iDataSize - 8;

	uLong unpackedSize = (unsigned long)(*(unsigned int*)(pData+4));
	unpackBuffer = new BYTE[unpackedSize];

	uncompress((Bytef*)unpackBuffer, &unpackedSize, (Bytef*)(pData+8), messageSize);*/

	// 4 байта - размер непакованных данных
	unsigned long messageSize = iDataSize - 4;

	uLong unpackedSize = (unsigned long)(*(unsigned int*)(pData));
	unpackBuffer = MP_NEW_ARR( BYTE, unpackedSize);

	uncompress((Bytef*)unpackBuffer, &unpackedSize, (Bytef*)(pData+4), messageSize);
	
	iDataSize = unpackedSize;

	idx = 0;
	/*int packetID = 0;
	if(!memparse_object(unpackBuffer, idx, packetID))
		return false;*/

	if(!memparse_object(unpackBuffer, idx, count))
	{
		apLW->WriteLn("CAddNewFileInfoListIn::Analyse failed: 2");
		return false;
	}
	
	for(DWORD i=0;i<count;i++)
	{
		int posFileInfo = idx;
		if(!GetFileInfo(-1, &resNewInfoList, false))
		{
			apLW->WriteLn("CAddNewFileInfoListIn::Analyse failed: 3, i = " + i);
			return false;
		}
		vPosFileInfo.push_back(posFileInfo);
	}
	return (idx<=(int)iDataSize);
	//return EndOfData(idx);
}

DWORD CAddNewFileInfoListIn::GetCount(){
	return count;
}
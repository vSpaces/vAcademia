#include "stdafx.h"
#include "..\..\ComMan.h"
#include "MultiFileInfoIn.h"

#include "res_consts.h"
#include "ZLIBENGN.H"

using namespace res;
using namespace ResManProtocol;

CResInfo resInfo;
CResInfoSaver resInfoSaver;

CMultiFileInfoIn::CMultiFileInfoIn(BYTE *aData, int aDataSize) : CCommonPacketIn(aData, aDataSize)
{
	errorID = 0;
	lDateTime = 0;
	count = 0xffffffff;
	indexFirstFile = 0xffffffff;
	unpackBuffer = NULL;
}

CMultiFileInfoIn::~CMultiFileInfoIn()
{
	if(unpackBuffer!=NULL)
		MP_DELETE_ARR( unpackBuffer);
}

bool CMultiFileInfoIn::GetFirstFileInfo(DWORD aFlags, CResInfo *aResInfo, bool bGetData)
{
	if(bGetData){
		idx = indexFirstFile;
	}
	/*BYTE btExist = 0;
	if(!memparse_object(pData, idx, btExist))
		return false;
	aResInfo->dwStatus = (btExist == 1 ? (RES_REMOTE | (aFlags & RES_MEM_CACHE)) : 
	(btExist == 0 ? RES_PATH_ERROR : RES_SCRIPT_ERROR));*/
	aResInfo->dwStatus = RES_REMOTE | (aFlags & RES_MEM_CACHE);
	aResInfo->dwSize = 0;
	if(!memparse_object(unpackBuffer, idx, aResInfo->dwSize, bGetData))
		return false;
	lDateTime = 0;
	if(!memparse_object(unpackBuffer, idx, lDateTime, bGetData))
		lDateTime = 0;
	if(bGetData){
		aResInfo->oTime.dwLowDateTime = (DWORD)lDateTime;
		aResInfo->oTime.dwHighDateTime = (lDateTime >> 32);
		SYSTEMTIME stUTC;
		FileTimeToSystemTime(&aResInfo->oTime, &stUTC);
		if(stUTC.wYear > 3500)
		{
			stUTC.wYear -=1600;
			SystemTimeToFileTime(&stUTC, &aResInfo->oTime);
		}
	}
	//}
	return true;
}

bool CMultiFileInfoIn::GetFileInfo(int index, DWORD aFlags, CResInfoSaver *resInfoSaver, bool bGetData)
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
		resInfoSaver->info.sResPath = A2W( foo.GetBuffer());
	else
		return false;

	resInfoSaver->info.dwStatus = RES_REMOTE | (aFlags & RES_MEM_CACHE);
	resInfoSaver->info.dwSize = 0;
	if(!memparse_object(unpackBuffer, idx, resInfoSaver->info.dwSize, bGetData))
		return false;
	lDateTime = 0;
	if(!memparse_object(unpackBuffer, idx, lDateTime, bGetData))
		return false;
	if(bGetData)
	{
		resInfoSaver->info.oTime.dwLowDateTime = (DWORD)lDateTime;
		resInfoSaver->info.oTime.dwHighDateTime = lDateTime >> 32;
		SYSTEMTIME stUTC;
		FileTimeToSystemTime(&resInfoSaver->info.oTime, &stUTC);
		if(stUTC.wYear > 3500)
		{
			stUTC.wYear -=1600;
			SystemTimeToFileTime(&stUTC, &resInfoSaver->info.oTime);
		}
	}
	return true;
}

bool CMultiFileInfoIn::Analyse()
{
	if(!CCommonPacketIn::Analyse())
		return false;
	
	// 4 байта - размер непакованных данных
	unsigned long messageSize = iDataSize - 4;

	uLong unpackedSize = (unsigned long)(*(unsigned int*)(pData));
	unpackBuffer = MP_NEW_ARR( BYTE, unpackedSize);

	/*ZlibEngine zip;
	zip.decompmem((char*)(pData+4), &messageSize, (char*)unpackBuffer, &unpackedSize);*/
	uncompress((Bytef*)unpackBuffer, &unpackedSize, (Bytef*)(pData+4), messageSize);
	
	iDataSize = unpackedSize;

	idx = 0;

	if(!memparse_object(unpackBuffer, idx, errorID))
		return false;
	if(errorID == DIR_RES_EMPTY || errorID == DIR_RES_NOT_EXIST || errorID == DIRINFO_ALREADY_SENDED)
		return true;	
	if(errorID!=FILE_RES_NOT_EXIST){
		int index = idx;
		if(!GetFirstFileInfo(0, &resInfo, false))
			return false;
		indexFirstFile = index;
	}
	if(!memparse_object(unpackBuffer, idx, count))
		return false;
	
	for(DWORD i=0;i<count;i++)
	{
		int posFileInfo = idx;
		if(!GetFileInfo(-1, 0, &resInfoSaver, false))
			return false;
		vPosFileInfo.push_back(posFileInfo);
	}
	return (idx<=(int)iDataSize);
	//return EndOfData(idx);
}

DWORD CMultiFileInfoIn::GetCount(){
	return count;
}

BYTE CMultiFileInfoIn::GetError(){
	return errorID;
}

#include "stdafx.h"
#include "..\..\ComMan.h"
#include "RootFileInfoList2In.h"

#include "res_consts.h"
#include "ZLIBENGN.H"

using namespace res;
using namespace ResManProtocol;

CResInfoSaver resRootInfoSaver;

CRootFileInfoList2In::CRootFileInfoList2In(BYTE *aData, int aDataSize) : CCommonPacketIn(aData, aDataSize)
{
	lDateTime = 0;
	unpackBuffer = NULL;
	pQueryMan = NULL;
}

CRootFileInfoList2In::~CRootFileInfoList2In()
{
	if(unpackBuffer!=NULL)
		MP_DELETE_ARR( unpackBuffer);
}

bool CRootFileInfoList2In::GetFileInfo( bool bGetData)
{
	CComString foo;
	USES_CONVERSION;
	if(memparse_string(unpackBuffer, idx, foo, bGetData))
		resRootInfoSaver.info.sResPath = A2W( foo.GetBuffer());
	else
		return false;

	resRootInfoSaver.info.dwStatus = RES_REMOTE;
	resRootInfoSaver.info.dwSize = 0;
	if(!memparse_object(unpackBuffer, idx, resRootInfoSaver.info.dwSize, bGetData))
		return false;
	lDateTime = 0;
	if(!memparse_object(unpackBuffer, idx, lDateTime, bGetData))
		return false;
	if(bGetData)
	{
		resRootInfoSaver.info.oTime.dwLowDateTime = (DWORD)lDateTime;
		resRootInfoSaver.info.oTime.dwHighDateTime = lDateTime >> 32;
		SYSTEMTIME stUTC;
		FileTimeToSystemTime(&resRootInfoSaver.info.oTime, &stUTC);
		if(stUTC.wYear > 3500)
		{
			stUTC.wYear -=1600;
			SystemTimeToFileTime(&stUTC, &resRootInfoSaver.info.oTime);
		}
	//}
	}
	return true;
}

bool CRootFileInfoList2In::GetDirInfo( bool bGetData)
{
	CComString foo;
	USES_CONVERSION;
	if(memparse_string(unpackBuffer, idx, foo, bGetData))
	{
		resRootInfoSaver.info.sResPath = A2W( foo.GetBuffer());
		return true;
	}
	return false;
}

bool CRootFileInfoList2In::ReadDataRec( const CWComString &asDir, bool abGetData)
{

	int countFile = 0;
	if(!memparse_object(unpackBuffer, idx, countFile))
		return false;

	for(int i=0;i<countFile;i++)
	{
		if(!GetFileInfo(abGetData))
			return false;
		if( abGetData)
		{
			MP_NEW_P( pResRootInfoSaver, CResInfoSaver, resRootInfoSaver);
			pResRootInfoSaver->info.sResPath = asDir + L"/" + pResRootInfoSaver->info.sResPath;
			pQueryMan->update( pResRootInfoSaver);
		}
	}

	int countDir = 0;
	if(!memparse_object(unpackBuffer, idx, countDir))
		return false;

	for(int i=0;i<countDir;i++)
	{
		if(!GetDirInfo(abGetData))
			return false;
		CWComString sDir;
		if(abGetData)
			sDir = asDir + L"/" + resRootInfoSaver.info.sResPath;
		if(!ReadDataRec(sDir, abGetData))
			return false;
	}
	return true;
}

bool CRootFileInfoList2In::ReadData( bool abGetData)
{
	idx = 0;
	int countDir = 0;
	if(!memparse_object(unpackBuffer, idx, countDir))
		return false;
	
	for(int i=0;i<countDir;i++)
	{
		if(!GetDirInfo(abGetData))
			return false;
		CWComString sDir;
		if(abGetData)
			sDir = resRootInfoSaver.info.sResPath;
		if(!ReadDataRec(sDir, abGetData))
			return false;
	}
	return true;
}

bool CRootFileInfoList2In::ReadData( CInfoQueryMan *apQueryMan)
{
	pQueryMan = apQueryMan;
	return ReadData();	
}

bool CRootFileInfoList2In::Analyse()
{
	if(!CCommonPacketIn::Analyse())
		return false;
	
	unsigned long messageSize = iDataSize - 4;

	uLong unpackedSize = (unsigned long)(*(unsigned int*)(pData));
	unpackBuffer = MP_NEW_ARR( BYTE, unpackedSize);

	uncompress((Bytef*)unpackBuffer, &unpackedSize, (Bytef*)(pData+4), messageSize);
	
	iDataSize = unpackedSize;	

	if(!ReadData( false))
		return false;
	
	return (idx<=(int)iDataSize);
	//return EndOfData(idx);
}
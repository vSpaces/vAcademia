#include "stdafx.h"
#include "PackClientSetupIn.h"

using namespace ResManProtocol;

CPackClientSetupIn::CPackClientSetupIn(BYTE *aData, int aDataSize) : CCommonPacketIn(aData, aDataSize){
}

bool CPackClientSetupIn::GetInfo(CResInfoSaver *resInfoSaver, bool bGetData)
{
	if(bGetData){
		idx = indexFirstFile;
	}

	CComString foo;
	USES_CONVERSION;
	if(memparse_string(pData, idx, foo, bGetData))
		resInfoSaver->info.sResPath = A2W( foo.GetBuffer());
	else
		return false;

	if(!memparse_object(pData, idx, resInfoSaver->info.dwSize, bGetData))
		return false;

	lDateTime = 0;
	if(!memparse_object(pData, idx, lDateTime, bGetData))
		lDateTime = 0;
	if(bGetData){
		resInfoSaver->info.oTime.dwLowDateTime = (DWORD)lDateTime;
		resInfoSaver->info.oTime.dwHighDateTime = (lDateTime >> 32);
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

bool CPackClientSetupIn::Analyse()
{
	if(!CCommonPacketIn::Analyse())
		return false;
	iStatus = 0;
	idx = 0;
	if(!memparse_object(pData, idx, iStatus))
		return false;

	indexFirstFile = idx;
	if( iStatus == 1)
	{
		CResInfoSaver resInfoSaver;
		if(!GetInfo( &resInfoSaver, false))
			return false;
	}

	return EndOfData(idx);
}

int CPackClientSetupIn::GetStatus(){
	return iStatus;
}



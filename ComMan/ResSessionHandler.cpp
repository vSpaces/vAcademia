#include "stdafx.h"
#include ".\ressessionhandler.h"
#include "DataBuffer2.h"
#include "HTTP.h"
#include "ResMan.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CResSessionHandler::CResSessionHandler()
{
	pResMan = NULL;
}

CResSessionHandler::~CResSessionHandler()
{
	pResMan = NULL;
}

void CResSessionHandler::SetResMan(CResMan *aResMan)
{
	pResMan = aResMan;
}

//////////////////////////////////////////////////////////////////////////

int CResSessionHandler::OnFileInfo(BYTE* aData, int aDataSize)
{
	if( aData == NULL || aDataSize == 0) return 1;
	return 0;
}

int CResSessionHandler::OnFileRead(BYTE* aData, int aDataSize)
{
	if( aData == NULL || aDataSize == 0) return 1;
	return 0;
}


int CResSessionHandler::OnMultiFileInfo(BYTE* aData, int aDataSize)
{
	if( aData == NULL || aDataSize == 0) return 1;
	return 0;
}

int CResSessionHandler::OnRootFileInfo(BYTE* aData, int aDataSize)
{
	if( aData == NULL || aDataSize == 0) return 1;
	return 0;
}

int CResSessionHandler::OnNotifyNewFileInfoList(BYTE* aData, int aDataSize)
{
	if(pResMan!=NULL)
	{
		pResMan->AddFileInfoList(aData, aDataSize);
	}
	return aDataSize;
}

int CResSessionHandler::OnNotifyRootFileInfo(BYTE* aData, int aDataSize)
{
	if(pResMan!=NULL)
	{
		pResMan->AddRootInfoList(aData, aDataSize);
	}
	return aDataSize;
}

int CResSessionHandler::OnPackUpdateClientFilesAndGetInfo(BYTE* aData, int aDataSize)
{
	if( aData == NULL || aDataSize == 0) return 1;
	return 0;
}

int CResSessionHandler::OnRootFileInfo2(BYTE* aData, int aDataSize)
{
	if( aData == NULL || aDataSize == 0) return 1;
	return 0;
}

int CResSessionHandler::OnPackClientSetup(BYTE* aData, int aDataSize)
{
	if( aData == NULL || aDataSize == 0) return 1;
	return 0;
}

/*int CResSessionHandler::OnWriteUpdaterLog(BYTE* aData, int aDataSize)
{
	if( aData == NULL || aDataSize == 0) return 0;
	return 0;
}*/
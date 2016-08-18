#include "stdafx.h"
#include ".\uploadsessionhandler.h"
#include "DataBuffer2.h"
#include "HTTP.h"
#include "ResMan.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CUploadSessionHandler::CUploadSessionHandler()
{
	pResMan = NULL;
}

CUploadSessionHandler::~CUploadSessionHandler()
{
	pResMan = NULL;
}

void CUploadSessionHandler::SetResMan(CResMan *aResMan)
{
	pResMan = aResMan;
}

//////////////////////////////////////////////////////////////////////////

int CUploadSessionHandler::OnResInfo(BYTE* aData, int aDataSize)
{
	if( aData == NULL || aDataSize == 0) return 1;
	return 0;
}

int CUploadSessionHandler::OnFileWrite(BYTE* aData, int aDataSize)
{
	if( aData == NULL || aDataSize == 0) return 1;
	return 0;
}

int CUploadSessionHandler::OnFileRemove(BYTE* aData, int aDataSize)
{
	if( aData == NULL || aDataSize == 0) return 1;
	return 0;
}

int CUploadSessionHandler::OnDirRemove(BYTE* aData, int aDataSize)
{
	if( aData == NULL || aDataSize == 0) return 1;
	return 0;
}
#include "stdafx.h"
#include "UpdatePlayingTimeIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CUpdatePlayingTimeIn::CUpdatePlayingTimeIn( BYTE* apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

CUpdatePlayingTimeIn::~CUpdatePlayingTimeIn(){
}

bool CUpdatePlayingTimeIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if(!memparse_object(pData, idx, muTime))
		return false;
	return EndOfData( idx);
}


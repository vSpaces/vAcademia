#include "stdafx.h"
#include "LogSeanceCreatedIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CLogSeanceCreatedIn::CLogSeanceCreatedIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize),
	MP_WSTRING_INIT(sAuthor),
	MP_WSTRING_INIT(sSeanceURL)
{
}

CLogSeanceCreatedIn::~CLogSeanceCreatedIn(){
}



bool CLogSeanceCreatedIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	if (!memparse_object(pData, idx, seanceID))
		return false;
	if (!memparse_object(pData, idx, seanceMode))
		return false;
	if (!memparse_string(pData, idx, sAuthor))
		return false;
	if (!memparse_string(pData, idx, sSeanceURL))
		return false;
	return EndOfData( idx);
}


unsigned int CLogSeanceCreatedIn::GetSeanceID()const {
	return seanceID;
}

unsigned int CLogSeanceCreatedIn::GetSeanceMode() const {
	return seanceMode;
}


const wchar_t* CLogSeanceCreatedIn::GetAuthor() const {
	return sAuthor.c_str();
}


const wchar_t* CLogSeanceCreatedIn::GetSeanceURL() const{
	return sSeanceURL.c_str();
}


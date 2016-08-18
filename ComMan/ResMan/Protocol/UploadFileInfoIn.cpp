#include "stdafx.h"
#include "UploadFileInfoIn.h"

using namespace ResManProtocol;

CUploadFileInfoIn::CUploadFileInfoIn(BYTE *aData, int aDataSize) : CCommonPacketIn(aData, aDataSize), MP_WSTRING_INIT( wsErrorText)
{
}

bool CUploadFileInfoIn::Analyse()
{
	if(!CCommonPacketIn::Analyse())
		return false;
	int idx = 0;
	btStatus = 0;
	if(!memparse_object(pData, idx, btStatus))
		return false;

	if( EndOfData(idx))
		return true;

	if(!memparse_string(pData, idx, wsErrorText))
		return false;

	return EndOfData(idx);
}

BYTE CUploadFileInfoIn::GetStatus(){
	return btStatus;
}

const wchar_t *CUploadFileInfoIn::GetErrorText(){
	return wsErrorText.c_str();
}



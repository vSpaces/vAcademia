#include "stdafx.h"
#include "RecordEditorPauseIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordEditorPauseIn::CRecordEditorPauseIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

bool CRecordEditorPauseIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx = 0;
	if(!memparse_object(pData, idx, iErrorCode))
		return false;

	return EndOfData( idx);
}

const int CRecordEditorPauseIn::GetErrorCode() const
{
	return iErrorCode;
}


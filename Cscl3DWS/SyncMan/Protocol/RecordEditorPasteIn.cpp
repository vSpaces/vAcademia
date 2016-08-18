#include "stdafx.h"
#include "RecordEditorPasteIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordEditorPasteIn::CRecordEditorPasteIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

bool CRecordEditorPasteIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx = 0;
	if(!memparse_object(pData, idx, iErrorCode))
		return false;

	if(!memparse_object(pData, idx, iRecordDuration))
		return false;

	return EndOfData( idx);
}

const int CRecordEditorPasteIn::GetErrorCode() const
{
	return iErrorCode;
}

const int CRecordEditorPasteIn::GetRecordDuration() const
{
	return iRecordDuration;
}


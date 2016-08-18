#include "stdafx.h"
#include "RecordEditorCutIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordEditorCutIn::CRecordEditorCutIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

bool CRecordEditorCutIn::Analyse()
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

const int CRecordEditorCutIn::GetErrorCode() const
{
	return iErrorCode;
}

const int CRecordEditorCutIn::GetRecordDuration() const
{
	return iRecordDuration;
}


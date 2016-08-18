#include "stdafx.h"
#include "RecordEditorCopyIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordEditorCopyIn::CRecordEditorCopyIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

bool CRecordEditorCopyIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx = 0;
	if(!memparse_object(pData, idx, iErrorCode))
		return false;

	return EndOfData( idx);
}

const int CRecordEditorCopyIn::GetErrorCode() const
{
	return iErrorCode;
}


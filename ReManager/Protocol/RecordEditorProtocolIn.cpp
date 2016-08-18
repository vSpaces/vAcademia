#include "stdafx.h"
#include ".\recordeditorprotocolin.h"

CRecordEditorProtocolIn::CRecordEditorProtocolIn( BYTE *apData, int apDataSize):
	CCommonPacketIn( apData, apDataSize)
{
	
}

CRecordEditorProtocolIn::~CRecordEditorProtocolIn(void)
{

}

bool CRecordEditorProtocolIn::Analyse()
{
	return false;
}

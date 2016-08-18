#include "stdafx.h"
#include "GetRecordInfoIn.h"

CGetRecordInfoIn::CGetRecordInfoIn( BYTE *apData, int apDataSize):
	CCommonPacketIn( apData, apDataSize)
{
	m_pData = apData;
	m_DataSize = apDataSize;
}

CGetRecordInfoIn::~CGetRecordInfoIn(void)
{
}

bool CGetRecordInfoIn::Analyse()
{
	int idx = 0;
	if (!memparse_object(m_pData, idx, m_recordID))
		return false;
	
	return EndOfData( idx);
}

int CGetRecordInfoIn::GetRecordID()
{
	return m_recordID;
}

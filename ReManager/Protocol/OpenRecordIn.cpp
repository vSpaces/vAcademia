#include "stdafx.h"
#include "OpenRecordIn.h"

COpenRecordIn::COpenRecordIn( BYTE *apData, int apDataSize):
	CCommonPacketIn( apData, apDataSize)
{
	m_pData = apData;
	m_DataSize = apDataSize;
}

COpenRecordIn::~COpenRecordIn(void)
{

}

bool COpenRecordIn::Analyse()
{
	int idx = 0;
	CComString URL;
	if (!memparse_string(m_pData, idx, URL))
		return false;

	USES_CONVERSION;
	m_URL = A2W(URL.GetString());
	return EndOfData( idx);
}

std::wstring COpenRecordIn::GetURL()
{
	return m_URL;
}

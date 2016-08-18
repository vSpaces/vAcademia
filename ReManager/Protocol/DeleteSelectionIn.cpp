#include "stdafx.h"
#include "DeleteSelectionIn.h"

CDeleteSelectionIn::CDeleteSelectionIn( BYTE *apData, int apDataSize):
	CCommonPacketIn( apData, apDataSize)
{
	m_pData = apData;
	m_DataSize = apDataSize;
}

CDeleteSelectionIn::~CDeleteSelectionIn(void)
{
}

bool CDeleteSelectionIn::Analyse()
{
	int idx = 0;
	if (!memparse_object(m_pData, idx, m_beginPos))
		return false;
	if (!memparse_object(m_pData, idx, m_endPos))
		return false;

	return EndOfData( idx);
}

unsigned long CDeleteSelectionIn::GetBeginPos()
{
	return m_beginPos;
}

unsigned long CDeleteSelectionIn::GetEndPos()
{
	return m_endPos;
}
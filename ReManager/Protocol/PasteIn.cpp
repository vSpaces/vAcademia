#include "stdafx.h"
#include "PasteIn.h"

CPasteIn::CPasteIn( BYTE *apData, int apDataSize):
	CCommonPacketIn( apData, apDataSize)
{
	m_pData = apData;
	m_DataSize = apDataSize;
}

CPasteIn::~CPasteIn(void)
{
}

bool CPasteIn::Analyse()
{
	int idx = 0;
	if (!memparse_object(m_pData, idx, m_beginPos))
		return false;
	if (!memparse_object(m_pData, idx, m_endPos))
		return false;
	if (!memparse_object(m_pData, idx, m_pastePos))
		return false;
	if (!memparse_object(m_pData, idx, m_isCopy))
		return false;
	
	return EndOfData( idx);
}

double CPasteIn::GetBeginPos()
{
	return m_beginPos;
}

double CPasteIn::GetEndPos()
{
	return m_endPos;
}

double CPasteIn::GetPastePos()
{
	return m_pastePos;
}

bool CPasteIn::IsCopy()
{
	return m_isCopy;
}
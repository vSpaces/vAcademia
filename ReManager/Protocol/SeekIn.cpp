#include "stdafx.h"
#include "SeekIn.h"

CSeekIn::CSeekIn( BYTE *apData, int apDataSize):
	CCommonPacketIn( apData, apDataSize)
{
	m_pData = apData;
	m_DataSize = apDataSize;
}

CSeekIn::~CSeekIn(void)
{
}

bool CSeekIn::Analyse()
{
	int idx = 0;
	if (!memparse_object(m_pData, idx, m_seekPos))
		return false;
	
	return EndOfData( idx);
}

unsigned long CSeekIn::GetSeekPos()
{
	return m_seekPos;
}
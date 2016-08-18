#include "stdafx.h"
#include "SaveIn.h"

CSaveIn::CSaveIn( BYTE *apData, int apDataSize):
	CCommonPacketIn( apData, apDataSize)
{
	m_pData = apData;
	m_DataSize = apDataSize;
}

CSaveIn::~CSaveIn(void)
{
}

bool CSaveIn::Analyse()
{
	int idx = 0;
	CComString cAuthor, cCreationTime, cName, cDescription;
	if (!memparse_string(m_pData, idx, cAuthor))
		return false;
	if (!memparse_string(m_pData, idx, cCreationTime))
		return false;
	if (!memparse_string(m_pData, idx, cName))
		return false;
	if (!memparse_string(m_pData, idx, cDescription))
		return false;

	USES_CONVERSION;
	author = A2W(cAuthor.GetString());
	creationTime = A2W(cCreationTime.GetString());
	name = A2W(cName.GetString());
	description = A2W(cDescription.GetString());
	return EndOfData( idx);
}

std::wstring CSaveIn::GetAuthor()
{
	return author;
}

std::wstring CSaveIn::GetCreationTime()
{
	return creationTime;
}

std::wstring CSaveIn::GetName()
{
	return name;
}

std::wstring CSaveIn::GetDescription()
{
	return description;
}

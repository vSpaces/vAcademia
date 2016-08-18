
#include "stdafx.h"
#include <assert.h>
#include "ResourceProcessor.h"
#include "cal3d/memory_leak.h"

CResourceProcessor::CResourceProcessor()
{
	m_hModule = NULL;
}

bool CResourceProcessor::Read(void* data, unsigned int size)
{
	memcpy(data, &m_data[m_filePosition], size);
	m_filePosition += size;

	return true;
}

char CResourceProcessor::GetChar()
{ 
	unsigned char ic;

	ic = m_data[m_filePosition++];

	return ic;
}

unsigned short CResourceProcessor::GetShort()
{
	unsigned char ic;
	unsigned short ip;

	ic = m_data[m_filePosition];
	m_filePosition++;
	ip = ic;

	ic = m_data[m_filePosition];
	m_filePosition++;
	ip |= ((unsigned short)ic << 8);

	return ip;
}

unsigned int CResourceProcessor::GetLong()
{
	unsigned int ip = 0;
	char ic = 0;
	unsigned char uc = ic;

	ic = m_data[m_filePosition];
	m_filePosition++;
	uc = ic;
	ip = uc;

	ic = m_data[m_filePosition];
	m_filePosition++;
	uc = ic;
	ip |= ((unsigned int)uc << 8);

	ic = m_data[m_filePosition];
	m_filePosition++;
	uc = ic;
	ip |= ((unsigned int)uc << 16);

	ic = m_data[m_filePosition];
	m_filePosition++;
	uc = ic;
	ip |= ((unsigned int)uc << 24);

	return ip;
}

void CResourceProcessor::SetAppHandle(HMODULE hModule)
{
	m_hModule = hModule;
}

void CResourceProcessor::OpenResFile(int num)
{
	assert(m_hModule != NULL);

	HRSRC hresinfo;
	hresinfo = FindResource(m_hModule, MAKEINTRESOURCE(num), RT_RCDATA);
	HGLOBAL hgl = LoadResource(NULL, hresinfo);
	m_data = (unsigned char*)LockResource(hgl);

	m_filePosition = 0;
}

int CResourceProcessor::GetPosition()
{
	return m_filePosition;
}

void CResourceProcessor::SetPosition(unsigned int pos)
{
	m_filePosition = pos;
}

void CResourceProcessor::MovePosition(int delta)
{
	m_filePosition += delta;
}

void* CResourceProcessor::GetCurrentPointer()
{
	return &m_data[m_filePosition];
}

CResourceProcessor::~CResourceProcessor()
{
}

#include "StdAfx.h"
#include "MemFileResource.h"
#include "cal3d/memory_leak.h"

CMemFileResource::CMemFileResource()
{
}

void CMemFileResource::Init(ifile* file)
{
	m_file = file;
}

unsigned int CMemFileResource::GetSize()
{
	return m_file->get_file_size();
}

bool CMemFileResource::Read(void* data, unsigned int size)
{
	m_file->read((BYTE *)data, size);

	return true;
}

char CMemFileResource::GetChar()
{
	char ch;
	m_file->read((BYTE*)&ch, 1);
	return ch;
}

unsigned short CMemFileResource::GetShort()
{
	unsigned short ch;
	m_file->read((BYTE*)&ch, 2);
	return ch;
}

unsigned int CMemFileResource::GetLong()
{
	unsigned int ch;
	m_file->read((BYTE*)&ch, 4);
	return ch;
}

void* CMemFileResource::GetPointer()
{
	return NULL;
}

CMemFileResource::~CMemFileResource()
{
}
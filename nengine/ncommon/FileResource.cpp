
#include "StdAfx.h"
#include "FileResource.h"
#include "FileFunctions.h"
#include "cal3d/memory_leak.h"

CFileResource::CFileResource():
	m_file(NULL)
{
}

void CFileResource::Open(std::wstring fileName)
{
	m_file = FileOpen(fileName.c_str(), L"rb");
}

unsigned int CFileResource::GetSize()
{
	if (!m_file)
	{
		return 0;
	}

	fseek(m_file, 0, 2);
	unsigned int size = ftell(m_file);
	fseek(m_file, 0, 0);

	return size;
}

void* CFileResource::GetPointer()
{
	return NULL;
}

char CFileResource::GetChar()
{
	if (!m_file)
	{
		return 0;
	}

	char tmp;
	fread(&tmp, 1, 1, m_file);
	return tmp;
}

unsigned short CFileResource::GetShort()
{
	unsigned short tmp;
	fread(&tmp, 1, 2, m_file);
	return tmp;
}

unsigned int CFileResource::GetLong()
{
	unsigned int tmp;
	fread(&tmp, 1, 4, m_file);
	return tmp;
}

bool CFileResource::Read(void* data, unsigned int size)
{
	fread(data, 1, size, m_file);

	return true;
}

CFileResource::~CFileResource()
{
	if (m_file)
	{
		fclose(m_file);
	}
}
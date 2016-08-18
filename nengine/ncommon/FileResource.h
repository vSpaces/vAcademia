
#pragma once

#include "CommonCommonHeader.h"

#include "IResourceProcessor.h"
#include <string>

class CFileResource : public IResourceProcessor
{
public:
	CFileResource();
	~CFileResource();

	void Open(std::wstring fileName);

	char GetChar();
	unsigned short GetShort();
	unsigned int GetLong();

	bool Read(void* data, unsigned int size);

	unsigned int GetSize();

	void* GetPointer();

private:
	FILE* m_file;
};
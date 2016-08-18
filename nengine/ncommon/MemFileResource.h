
#pragma once

#include "CommonCommonHeader.h"

#include "IResourceProcessor.h"
#include "ifile.h"
#include <string>

class CMemFileResource : public IResourceProcessor
{
public:
	CMemFileResource();
	~CMemFileResource();

	void Init(ifile* file);

	char GetChar();
	unsigned short GetShort();
	unsigned int GetLong();

	bool Read(void* data, unsigned int size);

	unsigned int GetSize();

	void* GetPointer();

private:
	ifile* m_file;
};

#pragma once

#include "CommonCommonHeader.h"

#include "IResourceProcessor.h"
#include <string>
#include "ifile.h"

class CMemoryResource : public IResourceProcessor
{
public:
	CMemoryResource();
	~CMemoryResource();

	void Init(ifile* file);

	char GetChar();
	unsigned short GetShort();
	unsigned int GetLong();

	bool Read(void* data, unsigned int size);

	unsigned int GetSize();
	void* GetPointer();

	void SetSize(unsigned int size);
	void SetData(const void* data);
	void AttachData(const void* data);

private:
	unsigned char* m_fileData;
	unsigned int m_size;

	unsigned int m_pointer;
};

#pragma once

#include "CommonCommonHeader.h"

#include <windows.h>
#include "IResourceProcessor.h"

class CResourceProcessor : public IResourceProcessor
{
friend class CGlobalSingletonStorage;

public:
	// open resource data file
	void OpenResFile(int num);
	// set position in data file
	void SetPosition(unsigned int pos);
	// move position
	void MovePosition(int delta);

	bool Read(void* data, unsigned int size);

	// return position in data file
	int GetPosition();

	// read short from current position
	unsigned short GetShort();

	// read long from current position
	unsigned int GetLong();
	
	// read char from current position
	char GetChar();

	void* GetCurrentPointer();

	unsigned int GetSize() {return 0;}

	void* GetPointer() {return NULL;}

	void SetAppHandle(HMODULE hModule);

private:
	CResourceProcessor();
	CResourceProcessor(const CResourceProcessor&);
	CResourceProcessor& operator=(const CResourceProcessor&);
	~CResourceProcessor();

	HMODULE m_hModule;
	unsigned char* m_data;
	int m_filePosition;
};

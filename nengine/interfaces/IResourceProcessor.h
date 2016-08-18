
#pragma once

class IResourceProcessor
{
public:
	// Read short from current position
	virtual unsigned short GetShort() = 0;
	// Read long from current position
	virtual unsigned int GetLong() = 0;
	// Read char from current position
	virtual char GetChar() = 0;
	// Read bytes
	virtual bool Read(void* data, unsigned int size) = 0;
	virtual void* GetPointer() = 0;
	virtual unsigned int GetSize() = 0;
};
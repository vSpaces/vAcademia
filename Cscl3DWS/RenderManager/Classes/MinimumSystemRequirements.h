
#pragma once

class CMinimumSystemRequirements
{
public:
	CMinimumSystemRequirements();
	~CMinimumSystemRequirements();

	void Init();

	bool IsAllOk();
	bool IsSystemOk();
	bool IsProcessorOk();
	bool IsMemoryOk();
	bool IsVideoMemoryOk();
	bool IsVideocardOk();

private:
	__int64 m_minMemorySize;
	unsigned short m_minProcessorFreq;
	unsigned char m_minMinorSystemVersion;
	unsigned char m_minMajorSystemVersion;
	unsigned char m_minVideocardGeneration;
	unsigned int m_minVideoMemorySize;
};
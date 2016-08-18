
#pragma once

class CVideoMemory
{
public:
	CVideoMemory();
	~CVideoMemory();

	unsigned int GetTotalSize();
	unsigned int GetFreeSize();

private:
	void Analyze();

	bool m_isAnalyzed;

	unsigned int m_totalSize;
	unsigned int m_freeSize;
};
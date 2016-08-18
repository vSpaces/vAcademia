
#pragma once

class CVideoMemory
{
public:
	CVideoMemory();
	~CVideoMemory();

	unsigned int GetTotalSize(int vendorID);
	unsigned int GetFreeSize(int vendorID);

	unsigned int GetDirectDrawTotal();
	unsigned int GetDirectDrawFree();
	unsigned int GetDirectDrawLocal();
	unsigned int GetDirectDrawLocalFree();
	unsigned int GetDirectDrawLocalAGP();

private:
	void Analyze(int vendorID);

	bool m_isAnalyzed;

	unsigned int m_totalSize;
	unsigned int m_freeSize;

	unsigned int m_ddTotal;
	unsigned int m_ddFree;
	unsigned int m_ddLocal;
	unsigned int m_ddLocalFree;
	unsigned int m_ddAGP;
};
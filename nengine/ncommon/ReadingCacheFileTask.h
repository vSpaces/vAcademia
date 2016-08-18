#pragma once

#include "CommonCommonHeader.h"
#include "ITask.h"

class CReadingCacheFileTask : public ITask
{
public:
	CReadingCacheFileTask(void);
	~CReadingCacheFileTask(void);	

	void SetFileName(std::string& fileName);
	std::string GetFileName()const;

	void* GetData();
	unsigned int GetDataSize();

	void Start();

	void PrintInfo();

	void Destroy();
	void SelfDestroy();

	bool IsOptionalTask() { return false; }

private:
	MP_STRING m_fileName;

	void* m_data;
	unsigned int m_size;	
};
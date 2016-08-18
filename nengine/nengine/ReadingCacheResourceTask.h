#pragma once

#include "CommonEngineHeader.h"

#include <vector>
#include "ITask.h"

class CReadingCacheResourceTask : public ITask
{
public:
	CReadingCacheResourceTask();
	~CReadingCacheResourceTask();

	void SetFileName(std::string& fileName);
	std::string GetFileName()const;

	void SetUserData(void* userData);
    void* GetUserData()const;

	void Start();

	void* GetData()const;
	unsigned int GetDataSize()const;

	void PrintInfo();

	void Destroy() { assert(false); }
	void SelfDestroy();

	bool IsOptionalTask() { return false; }

private:
	MP_STRING m_fileName;

	void* m_userData;

	void* m_data;
	unsigned int m_size;
};

#pragma once

#include "CommonCommonHeader.h"

#include <vector>
#include "ITask.h"

class CAddNCacheDataTask : public ITask
{
public:
	CAddNCacheDataTask();
	~CAddNCacheDataTask();

	void SetParams(std::string& name, void* data, unsigned int size);		

	void Start();
	void PrintInfo();
	void FreeData();

	void* GetDataPointer()const;
	unsigned int GetDataSize()const;

	void Destroy();	
	void SelfDestroy();

	bool IsOptionalTask() { return true; }

	void LockData();
	void UnlockData();

private:
	MP_STRING m_name;
	void* m_data;
	unsigned int m_size;

	CRITICAL_SECTION m_cs;
};
#pragma once
#include "StdAfx.h"
#include "..\..\nengine\nengine\ITask.h"
#include "oms_context.h"

class CZLibPackTask : public ITask
{
public:
	CZLibPackTask();
	~CZLibPackTask();

	int SetData(void* data, unsigned int size);
	void SetContext(oms::omsContext* aContext);

	void Start();

	void* GetPackedData()const;
	unsigned int GetPackedDataSize()const;

	void PrintInfo();

	void Destroy() { assert(false); }
	void SelfDestroy();

	bool IsOptionalTask() { return false; }

private:
	void CleanUp();

	void* m_data;
	unsigned int m_size;

	void* m_packedData;
	unsigned int m_packedSize;
	unsigned int m_allocatedSize;

	 oms::omsContext* m_context;
};
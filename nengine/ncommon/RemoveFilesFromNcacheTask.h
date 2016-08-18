#pragma once

#include "CommonCommonHeader.h"

#include <map>
#include "ITask.h"

class CRemoveFilesFromNcacheTask : public ITask
{
public:
	CRemoveFilesFromNcacheTask(void);
	~CRemoveFilesFromNcacheTask(void);	

	void Start();

	void PrintInfo();

	void Destroy() { assert(true); }
	void SelfDestroy();

	bool IsOptionalTask() { return false; }
};

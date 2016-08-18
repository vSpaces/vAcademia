#pragma once

#include "CommonHeader.h"
#include "ITask.h"

class CPreloadBlendsTask : public ITask
{
public:
	CPreloadBlendsTask(void);
	~CPreloadBlendsTask(void);

	void SetParams();

	void Start();

	void PrintInfo();

	void Destroy() { assert(true); }
	void SelfDestroy();

	bool IsOptionalTask() { return false; }
};

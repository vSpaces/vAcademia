#pragma once

#include "CommonHeader.h"
#include "ITask.h"

class CPreloadMotionsTask : public ITask
{
public:
	CPreloadMotionsTask(void);
	~CPreloadMotionsTask(void);

	void SetParams();

	void Start();

	void PrintInfo();

	void Destroy() { assert(true); }
	void SelfDestroy();

	bool IsOptionalTask() { return false; }
};

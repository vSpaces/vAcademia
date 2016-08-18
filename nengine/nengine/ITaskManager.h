#pragma once

//#define PROFILE_TASK_EXECUTING	1

#include "ITask.h"

#define MAY_BE_MULTITHREADED		0
#define MUST_BE_MULTITHREADED		1
#define MUST_RUN_IN_MAIN_THREAD		2	// for debug

class ITaskManager
{
public:
	virtual void AddTask(ITask* task, int usage, int priority) = 0;
	virtual bool RemoveTask(ITask* task) = 0;
};

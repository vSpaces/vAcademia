
#pragma once

#include "CommonEngineHeader.h"

#include "Thread.h"
#include "ITask.h"
#include "ITaskManager.h"
#include <vector>

#define MAX_USED_THREAD_COUNT		15

#define MAY_BE_MULTITHREADED		0
#define MUST_BE_MULTITHREADED		1
#define MUST_RUN_IN_MAIN_THREAD		2	// for debug

#define PRIORITY_LOW				0
#define PRIORITY_HIGH				2

class CTaskManager: public ITaskManager
{
public:
	CTaskManager();
	~CTaskManager();

	void SetMainThreadCoreID(int coreID);

	void Init();

	void AddTask(ITask* task, int usage, int priority)
	{
		AddTask(task, usage, priority, false);
	}	

	void AddTask(ITask* task, int usage, int priority, bool isMayHangThread);
	bool RemoveTask(ITask* task);
	ITask* GetNewTask(const bool isMayHangThread);

	void Destroy();

	void PrintTaskInfo();

	void ExecuteAllTasks();
	
private:
	ITask* GetNewLowPriorityTask(int hangID);

	MP_VECTOR_DC<ITask *> m_lowPriorityTasks[2];
	MP_VECTOR_DC<ITask *> m_highPriorityTasks[2];
	int m_usedThreadCount;
	int m_coreCount;

	int m_hangTaskCount;

	unsigned int m_mainThreadCoreID;
	unsigned int m_mainThreadAffinityMask;

	CRITICAL_SECTION m_lowPriorityCS[2];
	CRITICAL_SECTION m_highPriorityCS[2];

	CThread* m_threads[MAX_USED_THREAD_COUNT][2];
};
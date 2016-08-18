
#include "StdAfx.h"
#include "ITask.h"
#include "ITaskManager.h"

#ifdef PROFILE_TASK_EXECUTING
	#include "GlobalSingletonStorage.h"
#endif

ITask::ITask():
	m_taskID(0),
	m_isDone(false),
	m_taskManager(NULL),
	m_isDestroyResultOnEnd(false)
{	
#ifdef PROFILE_TASK_EXECUTING
	m_startTime = g ? g->tp.GetTickCount() : 0;
#endif 
}

void ITask::Rewind()
{
	m_isDone = false;

#ifdef PROFILE_TASK_EXECUTING
	m_startTime = g ? g->tp.GetTickCount() : 0;
#endif
}

void ITask::OnDone()
{
	m_isDone = true;
}

void ITask::SetMayHangThread(const bool isMayHangThread)
{
	m_isMayHangThread = isMayHangThread;
}

bool ITask::IsMayHangThread()const
{
	return m_isMayHangThread;
}

void ITask::SetTaskID(unsigned int taskID)
{
	m_taskID = taskID;
}

void ITask::SetTaskManager(ITaskManager* taskManager)
{
	m_taskManager = taskManager;
}

ITask::~ITask()
{	
	if (m_taskManager)
	{
		if (m_taskManager->RemoveTask(this))
		{				
			assert(false);
		}
	}
}
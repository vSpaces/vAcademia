
#include "StdAfx.h"
#include "TaskManager.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"
#include "CrashHandlerInThread.h"

#define MAX_CORE_COUNT			256

ITask* g_nowPerformingTasks[MAX_CORE_COUNT];
volatile bool g_isTaskManagerWorking = true;

DWORD GetCPUIndex()
{
	DWORD result;

	__asm
	{
		mov eax,1
		cpuid
		shr ebx,24
		and ebx,0xf
		mov result,ebx
	} 

	return result;
}

DWORD WINAPI PerformTask(ITask* task, char coreID)
{
	g_nowPerformingTasks[coreID + 128] = task;			
	task->Start();
	
	if (task->IsDestroyResultOnEnd())
	{
		task->Destroy();
		g_nowPerformingTasks[coreID + 128] = NULL;
		task->SelfDestroy();
	}
	else
	{
		task->OnDone();
	}

	return 0;
}

DWORD WINAPI TaskThreadProc(LPVOID lpParam)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	CTaskManager* taskManager = &g->taskm;
	char coreID = (char)lpParam;

	while (g_isTaskManagerWorking)
	{
		ITask* task = taskManager->GetNewTask(coreID < 0);
		if (task)
		{
			PerformTask(task, coreID);
		}
		else
		{
			g_nowPerformingTasks[coreID + 128] = NULL;
			Sleep(1);
		}
	}

	return 0;
}

CTaskManager::CTaskManager():
	m_mainThreadAffinityMask(1),
	m_mainThreadCoreID(0),
	m_usedThreadCount(0),
	m_coreCount(0)
{
	for (int k = 0; k < 2; k++)
	{
		MP_VECTOR_DC_INIT(m_lowPriorityTasks[k]);
		MP_VECTOR_DC_INIT(m_highPriorityTasks[k]);

		InitializeCriticalSection(&m_lowPriorityCS[k]);
		InitializeCriticalSection(&m_highPriorityCS[k]);
	}

	for (int hangID = 0; hangID < 2; hangID++)
	for (int i = 0; i < MAX_USED_THREAD_COUNT; i++)
	{
		m_threads[i][hangID] = NULL;
	}

	memset(g_nowPerformingTasks, 0, sizeof(ITask*) * MAX_CORE_COUNT);
}

void CTaskManager::ExecuteAllTasks()
{
	__int64 t1 = g->tp.GetTickCount();

	g->lw.WriteLn("Delete unneccesary tasks start");

	EnterCriticalSection(&m_lowPriorityCS[1]);
	std::vector<ITask *>::iterator it = m_lowPriorityTasks[1].begin();
	
	while (it != m_lowPriorityTasks[1].end())
	if ((*it)->IsOptionalTask())
	{
		(*it)->OnDone();
		(*it)->Destroy();
		it = m_lowPriorityTasks[1].erase(it);
	}
	else
	{
		it++;
	}
	LeaveCriticalSection(&m_lowPriorityCS[1]);

	EnterCriticalSection(&m_highPriorityCS[1]);
	it = m_highPriorityTasks[1].begin();
	
	while (it != m_highPriorityTasks[1].end())
	if ((*it)->IsOptionalTask())
	{
		(*it)->OnDone();
		(*it)->Destroy();
		it = m_highPriorityTasks[1].erase(it);
	}
	else
	{
		it++;
	}
	LeaveCriticalSection(&m_highPriorityCS[1]);

	g->lw.WriteLn("ExecuteAllTasks start");

	ITask* task = GetNewTask(false);
	while (task != NULL)
	{
		PerformTask(task, -127);		
		Sleep(0);
		task = GetNewTask(false);
	}	

	__int64 t2 = g->tp.GetTickCount();

	g->lw.WriteLn("ExecuteAllTasks ends in ", (int)(t2 - t1), " ms");
}

void CTaskManager::SetMainThreadCoreID(int coreID)
{
	m_mainThreadCoreID = coreID;
	m_mainThreadAffinityMask = 1;
	for (unsigned int i = 0; i < m_mainThreadCoreID; i++)
	{
		m_mainThreadAffinityMask *= 2;
	}
}

void CTaskManager::Init()
{
	m_coreCount = g->ci.GetProcessorCount();

	if (m_coreCount == 1)
	{
		m_usedThreadCount = 1;
	}
	else
	{
		m_usedThreadCount = m_coreCount - 1;
	}

	timeBeginPeriod(1);

	//int currentCPU = GetCPUIndex();

	for (int hangID = 0; hangID < 2; hangID++)
	for (int i = 0; i < m_usedThreadCount; i++)
	{
		unsigned int threadMask = 1;

		if (m_mainThreadAffinityMask == threadMask)
		{
			threadMask *= 2;
		}

		for (int k = 0; k < i; k++)
		{
			threadMask *= 2;
			if (m_mainThreadAffinityMask == threadMask)
			{
				threadMask *= 2;
			}
		}

		m_threads[i][hangID] = MP_NEW(CThread);		
		m_threads[i][hangID]->SetStackSize(256 * 1024);
		int coreID = i;
		if (hangID > 0)
		{
			coreID = -i - 1;
		}
		m_threads[i][hangID]->SetParameter((void *)coreID);
		m_threads[i][hangID]->SetThreadProcedure(TaskThreadProc);
		m_threads[i][hangID]->SelfManageAffinity();
		m_threads[i][hangID]->Start();
		if (m_coreCount > 1)
		{
			m_threads[i][hangID]->SetAffinity(threadMask);
		}
	}
}

void CTaskManager::AddTask(ITask* task, int usage, int priority, bool isMayHangThread)
{
	task->SetTaskManager(this);
	task->Rewind();
	task->SetMayHangThread(isMayHangThread);

	if (((m_usedThreadCount == 0) || ((m_coreCount == 1) && (usage == MAY_BE_MULTITHREADED) && (!isMayHangThread))) || (usage == MUST_RUN_IN_MAIN_THREAD))
	{
		task->Start();
		task->OnDone();
		return;
	}

	int hangID = (int)isMayHangThread;

	switch (priority)
	{
	case PRIORITY_LOW:
		{
			EnterCriticalSection(&m_lowPriorityCS[hangID]);
			m_lowPriorityTasks[hangID].push_back(task);
			LeaveCriticalSection(&m_lowPriorityCS[hangID]);
		}
		break;

	case PRIORITY_HIGH:
		{
			EnterCriticalSection(&m_highPriorityCS[hangID]);
			m_highPriorityTasks[hangID].push_back(task);
			LeaveCriticalSection(&m_highPriorityCS[hangID]);
		}
		break;
	}
}

ITask* CTaskManager::GetNewLowPriorityTask(int hangID)
{
	ITask* res = NULL;

	EnterCriticalSection(&m_lowPriorityCS[hangID]);
	if (m_lowPriorityTasks[hangID].size() > 0)
	{
		res = m_lowPriorityTasks[hangID][0];
		m_lowPriorityTasks[hangID].erase(m_lowPriorityTasks[hangID].begin());
		LeaveCriticalSection(&m_lowPriorityCS[hangID]);
		return res;
	}
	LeaveCriticalSection(&m_lowPriorityCS[hangID]);

	return res;
}

ITask* CTaskManager::GetNewTask(const bool isMayHangThread)
{
	ITask* res = NULL;
	int hangID = (int)isMayHangThread;

	EnterCriticalSection(&m_highPriorityCS[hangID]);
	m_hangTaskCount++;
	bool isSpecialMode = ((m_hangTaskCount%2 == 1) && (isMayHangThread));
	if (isSpecialMode)
	{
		res = GetNewLowPriorityTask(hangID);
		if (res)
		{
			LeaveCriticalSection(&m_highPriorityCS[hangID]);
			return res;
		}
	}
	if (m_highPriorityTasks[hangID].size() > 0)
	{
		res = m_highPriorityTasks[hangID][0];
		m_highPriorityTasks[hangID].erase(m_highPriorityTasks[hangID].begin());
		LeaveCriticalSection(&m_highPriorityCS[hangID]);
		return res;
	}
	LeaveCriticalSection(&m_highPriorityCS[hangID]);

	if (!isSpecialMode)
	{
		return GetNewLowPriorityTask(hangID);
	}
	else
	{
		return NULL;
	}
}

void CTaskManager::Destroy()
{
	g_isTaskManagerWorking = false;

	Sleep(1000);

	// сначала стопим все потоки
	for (int hangID = 0; hangID < 2; hangID++)
	for (int i = 0; i < m_usedThreadCount; i++)
	if (m_threads[i][hangID])
	{
		m_threads[i][hangID]->TerminateStop();	
	}

	// ожидаем завершения
	// время ожидание равно максимуму от ожидания 1 потока
	for (int hangID = 0; hangID < 2; hangID++)
	for (int i = 0; i < m_usedThreadCount; i++)
	if (m_threads[i][hangID])
	{
		while (!m_threads[i][hangID]->IsThreadTerminated())
		{
			Sleep(1);
		}

		MP_DELETE(m_threads[i][hangID]);
		m_threads[i][hangID] = NULL;
	}
}

void CTaskManager::PrintTaskInfo()
{
	g->lw.WriteLn("==Tasks Info==");
	g->lw.WriteLn("Now performing tasks: ");

	for (int i = 0; i < MAX_CORE_COUNT; i++)
	{ 
		ITask* task = g_nowPerformingTasks[i];
		if (task)
		{
			g->lw.WriteLn("[", i, "] ");
			task->PrintInfo();
		}
	}

	g->lw.WriteLn("High priority tasks: ");

	int hangID;
	for (hangID = 0; hangID < 2; hangID++)
	{
		g->lw.WriteLn("HANG = ", (hangID == 0) ? "false" : "true");
		EnterCriticalSection(&m_highPriorityCS[hangID]);
		
		std::vector<ITask *>::iterator it = m_highPriorityTasks[hangID].begin();
		std::vector<ITask *>::iterator itEnd = m_highPriorityTasks[hangID].end();

		for ( ; it != itEnd; it++)
		{
			(*it)->PrintInfo();
		}
		
		LeaveCriticalSection(&m_highPriorityCS[hangID]);
	}

	g->lw.WriteLn("Low priority tasks: ");

	for (hangID = 0; hangID < 2; hangID++)
	{
		g->lw.WriteLn("HANG = ", (hangID == 0) ? "false" : "true");
		EnterCriticalSection(&m_lowPriorityCS[hangID]);

		std::vector<ITask *>::iterator it = m_lowPriorityTasks[hangID].begin();
		std::vector<ITask *>::iterator itEnd = m_lowPriorityTasks[hangID].end();

		for ( ; it != itEnd; it++)
		{
			(*it)->PrintInfo();
		}

		LeaveCriticalSection(&m_lowPriorityCS[hangID]);
	}

	g->lw.WriteLn("Threads info: ");
	for (int hangID = 0; hangID < 2; hangID++)
	for (int i = 0; i < m_usedThreadCount; i++)
	if (m_threads[i][hangID])
	{
		g->lw.WriteLn("thread ", i * 2 + hangID, " working ", ((m_threads[i][hangID])->GetCurrentState() == STATE_EXECUTING) ? "true" : "false");
	}
}

bool CTaskManager::RemoveTask(ITask* task)
{
	int hangID = (int)task->IsMayHangThread();

	EnterCriticalSection(&m_highPriorityCS[hangID]);
	std::vector<ITask*>::iterator it = m_highPriorityTasks[hangID].begin();
	std::vector<ITask*>::iterator itEnd = m_highPriorityTasks[hangID].end();

	for ( ; it != itEnd; it++)
	if (*it == task)
	{
		m_highPriorityTasks[hangID].erase(it);		
		LeaveCriticalSection(&m_highPriorityCS[hangID]);
		return true;
	}
	LeaveCriticalSection(&m_highPriorityCS[hangID]);

	EnterCriticalSection(&m_lowPriorityCS[hangID]);
	std::vector<ITask*>::iterator it2 = m_lowPriorityTasks[hangID].begin();
	std::vector<ITask*>::iterator it2End = m_lowPriorityTasks[hangID].end();

	for ( ; it2 != it2End; it2++)
	if (*it2 == task)
	{
		m_lowPriorityTasks[hangID].erase(it2);		
		LeaveCriticalSection(&m_lowPriorityCS[hangID]);
		return true;
	}
	LeaveCriticalSection(&m_lowPriorityCS[hangID]);

	return false;
}

CTaskManager::~CTaskManager()
{
	Destroy();	
}
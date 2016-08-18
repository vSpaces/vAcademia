
#pragma once

class ITaskManager;

class ITask
{
public:
	ITask();
	virtual ~ITask();

	virtual void Start() = 0;
	virtual void PrintInfo() = 0;

	virtual bool IsOptionalTask() = 0;

	void OnDone();
	void Rewind();

	void SetMayHangThread(const bool isMayHangThread);
	bool IsMayHangThread()const;

	__forceinline bool IsPerformed()
	{
		return m_isDone;
	}	

	void SetTaskID(unsigned int taskID);
	__forceinline unsigned int GetTaskID()const
	{
		return m_taskID;
	}

	__forceinline unsigned int GetStartTime()const
	{
		return m_startTime;
	}

	virtual void Destroy() = 0;
	virtual void SelfDestroy() = 0;

	__forceinline void DestroyResultOnEnd()
	{
		m_isDestroyResultOnEnd = true;
	}	

	__forceinline bool IsDestroyResultOnEnd()
	{
		return m_isDestroyResultOnEnd;
	}	

	void SetTaskManager(ITaskManager* taskManager);

private:	
	volatile unsigned int m_taskID;
	volatile unsigned int m_startTime;

	volatile bool m_isDone;
	volatile bool m_isMayHangThread;	
	volatile bool m_isDestroyResultOnEnd;

	ITaskManager* m_taskManager;
};
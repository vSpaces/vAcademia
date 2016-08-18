
#pragma once

#define THREAD_TYPE_SELF_MANAGED	1
#define	THREAD_TYPE_SECONDARY		2

#include <string>

typedef struct _SThreadStartInfo
{
	LPTHREAD_START_ROUTINE routine;
	std::string functionName;
	void* param;

	_SThreadStartInfo(const LPTHREAD_START_ROUTINE _routine, const void* _param, const std::string _functionName)
	{
		routine = _routine;
		param = (void*)_param;
		functionName = _functionName;
	}
} SThreadStartInfo;

class CSimpleThreadStarter
{
public:
	CSimpleThreadStarter(char* func);
	~CSimpleThreadStarter();

	void SetStackSize(unsigned int stackSize);
	unsigned int GetStackSize();

	void SetRoutine(LPTHREAD_START_ROUTINE routine);
	LPTHREAD_START_ROUTINE GetRoutine();

	void SetParameter(void* param);
	void* GetParameter();

	HANDLE Start(bool isSuspended = false);

	HANDLE GetThreadHandle();
	unsigned int GetThreadID();

	void SetThreadType(int type);

private:
	void UpdateThreadAffinityIfNeeded();

	std::string m_functionName;

	unsigned int m_stackSize;
	LPTHREAD_START_ROUTINE m_routine;
	void* m_param;

	unsigned int m_threadID;
	HANDLE m_handle;

	bool m_isSecondaryThread;
};
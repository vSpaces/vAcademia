#pragma once

#include <windows.h>

// States of thread
#define STATE_NOTEXECUTED	1
#define STATE_EXECUTING		2
#define STATE_STOPPING		3

// Levels of priority
#define PRIORITY_TIME_CRITICAL	THREAD_PRIORITY_TIME_CRITICAL
#define PRIORITY_HIGHEST		THREAD_PRIORITY_HIGHEST
#define PRIORITY_ABOVE_NORMAL	THREAD_PRIORITY_ABOVE_NORMAL
#define PRIORITY_NORMAL			THREAD_PRIORITY_NORMAL
#define PRIORITY_BELOW_NORMAL	THREAD_PRIORITY_BELOW_NORMAL
#define PRIORITY_LOWEST			THREAD_PRIORITY_LOWEST
#define PRIORITY_ABOVE_IDLE		THREAD_PRIORITY_ABOVE_IDLE
#define PRIORITY_IDLE			THREAD_PRIORITY_IDLE 

// Class for controlling threads
class CThread
{
public:
	CThread(void);
	~CThread(void);

	void SetStackSize(unsigned int stackSize);
	unsigned int GetStackSize()const;

	// Set procedure for thread
	void SetThreadProcedure(LPTHREAD_START_ROUTINE threadProc);

	void SetParameter(LPVOID lpParam);

	// Start execution of thread
	BOOL Start();

	// Stop execution with ability of continue of executing thread
	//BOOL Pause();

	// Play (start or play after stopping)
	//BOOL Play();

	// do not set secondary affinity (performs on any non-zero core)
	void SelfManageAffinity();

	// Stop execution without ability of continue of executinh thread
	BOOL TerminateStop();

	// Set priority of thread
	BOOL SetPriority(int priority);

	// Check thread to be terminated
	BOOL IsThreadTerminated();

	// Get priority of thread
	int GetPriority();

	// Get state of thread
	int GetCurrentState();

	// Get exit code of thread procedure
	DWORD GetExitCode();

	// Get last error code in thread
	DWORD GetErrorCode()const;

	void SetAffinity(unsigned int mask);

	HANDLE GetHandle();

private:
	// ID of thread
	DWORD m_threadId;

	// Handle of thread
	HANDLE m_handle;

	// Pointer to thread procedure
	LPTHREAD_START_ROUTINE m_threadProc;

	// Last error code
	DWORD m_errorCode;

	// State of thread
	int m_state;

	// Size of thread stack
	unsigned int m_stackSize;

	LPVOID m_lpParam;

	bool m_isAffinitySelfManaged;
};

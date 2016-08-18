#pragma once

class CWinThread
{
	//friend DWORD WINAPI main_thread_func( PVOID pvParam);

public:
	/// Codes for thread priorities.
	enum Priority {
		/// Will only run if all other threads are blocked.
		LowestPriority,   

		/// Runs approximately half as often as normal.
		LowPriority,      

		/// Normal priority for a thread.
		NormalPriority,   

		/// Runs approximately twice as often as normal.
		HighPriority,     

		/// Is only thread that will run, unless blocked.
		HighestPriority,  

		NumPriorities
	};

	CWinThread();
	virtual ~CWinThread();

public:
	virtual void main(){};

public:
	virtual BOOL create( LPTHREAD_START_ROUTINE lpStartAddress = NULL);
	virtual BOOL create( LPTHREAD_START_ROUTINE lpStartAddress, PVOID pObject);
	virtual BOOL resume();
	virtual BOOL suspend();
	virtual void terminated();
	virtual BOOL restart();
	virtual BOOL is_terminated() const;
	virtual BOOL wait_for_termination( const long maxWait) const;
	virtual BOOL is_suspended() const;
	virtual void set_priority( Priority priorityLevel);
	virtual Priority get_priority() const;

public:
	virtual void terminating();

protected:
	BOOL m_bTerminating;
	BOOL m_bTerminated;
	HANDLE m_handle;
	DWORD m_dwThreadId;
	LPTHREAD_START_ROUTINE m_lpStartAddress;
	DWORD m_dwStackSize;
	PVOID m_pObject;

//private:
	//static UINT __stdcall main_function(void * thread);
};

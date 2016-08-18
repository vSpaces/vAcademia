#include "..\nengine\ncommon\Thread.h"

#pragma once


struct ThreadData
{
	HWND hwnd;
	int msgCode;
	LPARAM message;

	ThreadData(HWND _hwnd, int _msgCode, LPARAM _message)
	{
		hwnd = _hwnd;
		msgCode = _msgCode;
		message = _message;
	}
};

class CCopyDataPoster
{
public:
	CCopyDataPoster();
	~CCopyDataPoster();

	static CCopyDataPoster* GetInstance();

	void AddData(ThreadData* data);
	ThreadData* GetThreadData();
	void StartThread();

private:
	CThread m_thread;
	MP_VECTOR<ThreadData*> m_threads_data;
	CRITICAL_SECTION	m_cs;
};
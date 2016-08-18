// WSThread.h: interface for the CWSThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WSTHREAD_H__0D3FC416_FA67_45AC_ADE0_9696095D3BCA__INCLUDED_)
#define AFX_WSTHREAD_H__0D3FC416_FA67_45AC_ADE0_9696095D3BCA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWSThread  
{
public:
	CWSThread();
	virtual ~CWSThread();

	void Run();
	void Stop();
	void Pause();
	void SetPriority();
	virtual UINT Thread();

protected:
	HANDLE hThread;
};

#endif // !defined(AFX_WSTHREAD_H__0D3FC416_FA67_45AC_ADE0_9696095D3BCA__INCLUDED_)

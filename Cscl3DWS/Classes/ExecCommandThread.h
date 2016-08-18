#pragma once
#include "wsthread.h"

class CWS3D;

class CExecCommandThread : public CWSThread
{
public:
	CExecCommandThread(void);
	virtual ~CExecCommandThread(void);

	enum{NOT_START, STARTED, FINISHED};

public:
	void Start( const wchar_t* apwcSysCommand, CWS3D *pWS3D);
	UINT Thread();
	int GetError();
	int GetStatus();

protected:
	int m_iError;
	std::wstring wcSysCommand;
	int m_iStatus;
	CWS3D *m_pWS3D;
};

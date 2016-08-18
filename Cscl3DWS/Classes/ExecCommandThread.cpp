#include "stdafx.h"
#include ".\execcommandthread.h"
#include "common.h"
#include "..\nengine\ncommon\ThreadAffinity.h"
#include "..\Cscl3DWS.h"
#include "..\WS3D.h"
#include <Shellapi.h>

CExecCommandThread::CExecCommandThread(void)
{
	m_iError = 0;
	m_iStatus = NOT_START;
}

CExecCommandThread::~CExecCommandThread(void)
{
	int count = 0;
	while( true)
	{
		if ( GetStatus() == NOT_START)
			break;
		if ( GetStatus() == FINISHED)
		{
			break;
		}
		Sleep( 100);
		count++;
		if ( count > 10)
		{
			//09.04.2014 лишь при закрытии приложения - иначе риск повисаний
			// не при закрытии вызов идет лишь при GetStatus() == FINISHED и выходит не доходя сюда
			TerminateThread( hThread, 0);
			break;
		}
	}
}

void CExecCommandThread::Start(const wchar_t* apwcSysCommand, CWS3D *pWS3D)
{
	if (apwcSysCommand == NULL || pWS3D == NULL)
	{
		m_iStatus = FINISHED;
		return;
	}

	m_iStatus = STARTED;
	wcSysCommand = apwcSysCommand;
	m_pWS3D = pWS3D;
	Run();
}

UINT CExecCommandThread::Thread()
{
	SetSecondaryThreadAffinity( this->hThread);

	wchar_t	moduleFileName[ MAX_PATH] = {L'\0'};
	GetModuleFileNameW( NULL, moduleFileName, MAX_PATH);

	LPWSTR lpFileName = NULL;
	wchar_t	modulePath[ 2 * MAX_PATH] = {L'\0'};
	::GetFullPathNameW( moduleFileName, sizeof(modulePath) / sizeof(modulePath[0]), modulePath, &lpFileName);
	if( lpFileName != NULL)
		*lpFileName = 0;

	int flag = SW_SHOWNORMAL;

	if (wcSysCommand.find(L"updater.bat")!= std::wstring::npos)
	{
		flag = SW_HIDE;
	}

	HINSTANCE  res = ShellExecuteW( 0, L"open", wcSysCommand.c_str(), L"", modulePath, flag);
	if( (int)res <= 32)
	{
		res = ShellExecuteW( 0, L"open", L"iexplore" , wcSysCommand.c_str(), modulePath, flag);
		if( (int)res <= 32)
		{
			//GetError();
			m_iError = GetLastError();
			m_pWS3D->GetError();
		}	
	}

	m_iStatus = FINISHED;

	return 0;
}

int CExecCommandThread::GetError()
{
	return m_iError;
}

int CExecCommandThread::GetStatus()
{
	return m_iStatus;
}
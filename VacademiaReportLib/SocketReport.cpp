#include "StdAfx.h"
#include "SocketReport.h"
#include "func.h"
#include <DataBuffer.h>

using namespace service;

CSocketReport::CSocketReport()
{
	m_dwStatusCode = -1;
	m_pServiceManager = NULL;	
}

CSocketReport::~CSocketReport()
{
	m_pServiceManager = NULL;
}

BOOL CSocketReport::sendData( TCHAR *server, TCHAR* queryphp, TCHAR *sData, int len, CDataBuffer *pDataBuffer /* = NULL */)
{	
	CWaitAndSignal waitAndSignal( mutex);
	if ( m_pServiceManager == NULL)
	{
		m_dwStatusCode = -1;
		return FALSE;
	}

	m_dwStatusCode = 601;

	m_pServiceManager->SendMDumpParams( (BYTE *) sData, len);

	int count = 0;
	while (m_pServiceManager->IsDumpSendedCode() == DUMP_SENDING_STARTING)
	{
		Sleep( 500);
		count++;
		if ( count > 60)
		{
			CComString s;
			s.Format( "server = %s\n CSocketReport::sendData dwStatusCode = %d\n", server, m_dwStatusCode);
			WriteLog__( s.GetBuffer());
			return FALSE;
		}
	}
	BOOL bNoError = m_pServiceManager->IsDumpSendedCode() == DUMP_SENDED;
	if ( !bNoError)
	{
		if ( m_pServiceManager->IsDumpSendedCode() == DUMP_NO_SENDED)
			m_dwStatusCode = 602;
		if ( m_pServiceManager->IsDumpSendedCode() == DUMP_ERROR)
			m_dwStatusCode = 603;
		CComString s;
		s.Format( "server = %s\n CSocketReport::sendData dwStatusCode = %d\n", server, m_dwStatusCode);
		WriteLog__( s.GetBuffer());
		return FALSE;
	}	
	
	m_dwStatusCode = 200;

	CComString s;
	s.Format( "server = %s\n CSocketReport::sendData dwStatusCode = %d\n", server, m_dwStatusCode);
	WriteLog__( s.GetBuffer());

	return TRUE;
}

BOOL CSocketReport::sendFileData(TCHAR *server, TCHAR* queryphp, WCHAR *fullPathSendFile, BYTE *buffer, int len, TCHAR *fileNameToServer,
							   DWORD *aDataSize, BYTE **aData)
{
	BOOL bNoError = FALSE;
	return bNoError;
}

BOOL CSocketReport::sendBufferData(TCHAR *server, BYTE *buffer, int len)
{
	CWaitAndSignal waitAndSignal( mutex);
	if ( m_pServiceManager == NULL)
	{
		m_dwStatusCode = -2;
		return FALSE;
	}

	m_dwStatusCode = 611;

	int dwPos = 0;
	int buffer_size = 131072;
	BOOL bNoError = FALSE;

	while ( dwPos < len)
	{
		int res = m_pServiceManager->SendMDumpFile( buffer + dwPos, ( buffer_size > len - dwPos ? len - dwPos : buffer_size), len);
		CComString s;
		if( res != 0)
		{
			s.Format( "server = %s\n CSocketReport::SendMDumpFile is not connect = %d\n", res);
			WriteLog__( s.GetBuffer());
		}
		int count = 0;
		while (m_pServiceManager->IsDumpSendedCode() == DUMP_SENDING_STARTING)
		{
			Sleep( 500);
			count++;
			if ( count > 60)
			{
				CComString s;
				s.Format( "server = %s\n socket dwStatusCode = %d\n", server, m_dwStatusCode);
				WriteLog__( s.GetBuffer());
				return FALSE;
			}
		}

		bNoError = m_pServiceManager->IsDumpSendedCode() == DUMP_SENDED;
		if ( !bNoError)
		{
			if ( m_pServiceManager->IsDumpSendedCode() == DUMP_NO_SENDED)
				m_dwStatusCode = 612;
			if ( m_pServiceManager->IsDumpSendedCode() == DUMP_ERROR)
				m_dwStatusCode = 613;
			s.Format( "server = %s\n CSocketReport::sendBufferData dwStatusCode = %d\n", server, m_dwStatusCode);
			WriteLog__( s.GetBuffer());
			return FALSE;
		}
		dwPos += buffer_size;
	}
	if ( bNoError)
		m_dwStatusCode = 200;

	CComString s;
	s.Format( "server = %s\n CSocketReport::sendBufferData dwStatusCode = %d\n", server, m_dwStatusCode);
	WriteLog__( s.GetBuffer());

	return bNoError;
}

DWORD CSocketReport::getStatusCode()
{
	return m_dwStatusCode;
}

void CSocketReport::setServiceWorldManager( service::IServiceWorldManager *pServiceManager)
{
	mutex.lock();
	m_pServiceManager = pServiceManager;
	mutex.unlock();
}

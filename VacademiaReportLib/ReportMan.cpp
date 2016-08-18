#include "StdAfx.h"
#include "ReportMan.h"
#include "func.h"
#include "io.h"
#include "WndError.h"
#include "InfoAnalyzer.h"
//#include "ReportAnalyzer.h"
#include "HTTPReport.h"
#include "SocketReport.h"
#include "DumpParamsGetter.h"
#include "XZip.h"
#include "XUnZip.h"
#include <io.h>

#ifndef _countof
	#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

CReportMan::CReportMan()
{
	OpenLog__( NULL, TRUE, FALSE);
	m_modRoot[ 0] = '\0';	
	m_dwStatusCode = -1;	
	m_errFileName[ 0] = '\0';
	rtl_strcpy( m_serverName, MAX_PATH, SERVER_NAME);
	rtl_strcpy( m_appVersion, 32, "vacademia_0.0.0.0_beta");

	if ( PRIMARYLANGID( GetSystemDefaultLangID()) == LANG_RUSSIAN 
		&& PRIMARYLANGID( GetUserDefaultLangID()) == LANG_RUSSIAN)  // LANG_ENGLISH
	{
		m_iLanguage = RUS;		
	}
	else
		m_iLanguage = ENG;

	m_hSendThread = NULL;
	m_bStartSending = FALSE;
	m_pWndError = NULL;	
	m_iQueryType = HTTP_QUERY;

	m_appName[ 0] = '\0';
	m_appReportName[ 0] = '\0';
	m_fullAppName[ 0] = '\0';	
	m_crashType[ 0] = '\0';
	m_language[ 0] = '\0';
	m_pHTTPReport = new CHTTPReport();
	m_pSocketReport = new CSocketReport();
	bTerminate = FALSE;
}

CReportMan::CReportMan( BOOL bLog)
{
	if ( bLog)
		OpenLog__( NULL, TRUE, FALSE);
	m_modRoot[ 0] = '\0';	
	m_dwStatusCode = -1;	
	m_errFileName[ 0] = '\0';
	rtl_strcpy( m_serverName, MAX_PATH, SERVER_NAME);
	rtl_strcpy( m_appVersion, 32, "vacademia_0.0.0.0_beta");

	if ( PRIMARYLANGID( GetSystemDefaultLangID()) == LANG_RUSSIAN 
		&& PRIMARYLANGID( GetUserDefaultLangID()) == LANG_RUSSIAN)  // LANG_ENGLISH
	{
		m_iLanguage = RUS;		
	}
	else
		m_iLanguage = ENG;

	m_hSendThread = NULL;
	m_bStartSending = FALSE;
	m_pWndError = NULL;	
	m_iQueryType = HTTP_QUERY;

	m_appName[ 0] = '\0';
	m_appReportName[ 0] = '\0';
	m_fullAppName[ 0] = '\0';	
	m_crashType[ 0] = '\0';
	m_language[ 0] = '\0';
	m_pHTTPReport = new CHTTPReport();
	m_pSocketReport = new CSocketReport();
	bTerminate = FALSE;
}

CReportMan::~CReportMan()
{
	terminate();
	int count = 0;
	while( !isEnding())
	{
		// ждем окончания потока
		Sleep( 500);
		count++;
		if ( count > 100)
		{
			TerminateThread( m_hSendThread, 0);
			CloseLog__();
			break;
		}
	}
	if ( m_pHTTPReport != NULL)
	{
		delete m_pHTTPReport;
		m_pHTTPReport = NULL;
	}
	if ( m_pSocketReport != NULL)
	{
		delete m_pSocketReport;
		m_pSocketReport = NULL;
	}
	if ( m_hSendThread != NULL)
	{
		CloseHandle( m_hSendThread);
		m_hSendThread = NULL;
	}
	CloseLog__();
}

//////////////////////////////////////////////////////////////////////////

BOOL CReportMan::readDataFromFile( WCHAR *fileName)
{
	FILE *fp = _wfopen( fileName, L"rb");
	if ( fp == NULL)
		return FALSE;
	int dwSize = _filelength( fp->_file);
	if ( dwSize == 0)
	{
		TCHAR *s = new TCHAR[ MAX_PATH_4];
		USES_CONVERSION;
		sprintf( s, "Size of file '%s' is equal 0\n", W2A(fileName));
		WriteLog__( s);
		delete[] s;
		fclose( fp);
		return FALSE;
	}
	int dwReadSize = __min( dwSize, BUFFER_SIZE);
	int readTotalBytes = 0;
	int size = m_dataBuffer.getLength();
	while ( true)
	{
		BYTE *pData = new BYTE[ dwReadSize];
		int readBytes = fread( pData, 1, dwReadSize, fp);
		readTotalBytes += readBytes;
		if ( readBytes <= 0)
		{
			delete[] pData;
			break;
		}
		m_dataBuffer.addData( readBytes, pData);
		delete[] pData;
	}
	
	m_dataBuffer2.add( (unsigned int) readTotalBytes, ((BYTE *) m_dataBuffer + size));
	fclose( fp);

	return TRUE;
}

void CReportMan::addSeparate()
{
	TCHAR *sSeparate = new TCHAR[ 11];
	sSeparate[ 0] = '-';
	for ( int i = 1; i < 6; i++)	
		sSeparate[ i] = '@';
	for ( int i = 6; i < 10; i++)	
		sSeparate[ i] = '-';
	sSeparate[ 10] = '\0';
	m_dataBuffer.addData( 10, (BYTE *) sSeparate);
	delete[] sSeparate;
}

// ZipFiles
int CReportMan::ZipFiles( int iCrashType, TCHAR *szProjectName)
{
	CDumpParamsGetter dumpParamsGetter;
	WriteLog__( "ZipFiles start\n");	

	USES_CONVERSION;
	int nFiles = 0;
	WCHAR *lpFileName = new WCHAR[8096];
	if ( !getFileNameByProjName( A2W(szProjectName), L"/q", lpFileName))
	{
		return FALSE;
	}

	WriteLog__( "zipFileName = ");
	WriteLog__( lpFileName);
	WriteLog__( "\n");

	WCHAR *pszFilePart = GetFilePart( lpFileName);
	WCHAR *lastperiod = lstrrchrW( pszFilePart, L'.');
	if (lastperiod)
		lastperiod[0] = L'\0';
	if ( iCrashType == APP_HANG_ID)
	{
		wcscat( pszFilePart, L"_hang");
	}
	else if ( iCrashType == ASSERT_ERROR_ID)
	{
		wcscat( pszFilePart, L"_assert");
	}
	wcscat( pszFilePart, L".zip");

	HZIP hZip = CreateZip((LPVOID)lpFileName, 0, ZIP_FILENAME);	

	WCHAR sZipName[ MAX_PATH_4];
	wcscpy( sZipName, lpFileName);

	if ( hZip == NULL)
	{
		delete[] lpFileName;
		return FALSE;
	}	

	CComString szDumpName = szProjectName;
	szDumpName.SetAt( szDumpName.GetLength() - 4, '\0');
	if ( iCrashType == APP_HANG_ID)
	{
		szDumpName += "_hang";
	}
	else if ( iCrashType == ASSERT_ERROR_ID)
	{
		szDumpName += "_assert";
	}
	szDumpName += ".dmp";
	// Replace the executable filename with our error log file name
	rtl_wcscpy(pszFilePart, MAX_PATH_4 - wcslen(lpFileName) - 1, A2W(szDumpName.GetBuffer()));

	WriteLog__( "szDumpName = ");
	WriteLog__( szDumpName.GetBuffer());
	WriteLog__( "\n");

	WriteLog__( "lpFileName = ");
	WriteLog__( lpFileName);
	WriteLog__( "\n");

	ZRESULT zr = ZipAdd(hZip, szDumpName.GetBuffer(), lpFileName, 0, ZIP_FILENAME);
	if (zr == ZR_OK)
	{
		nFiles++;
		wcscpy( fileNameForDelete[ 0].s, lpFileName);
	}
	else
	{
		delete[] lpFileName;
		if (hZip)
			CloseZip(hZip);
		DeleteFileW( sZipName);
		return FALSE;
	}

	if (hZip)
		CloseZip(hZip);

	pszFilePart = GetFilePart( lpFileName);
	lastperiod = lstrrchrW( pszFilePart, L'.');
	if (lastperiod)
		lastperiod[0] = '\0';
	wcscat( pszFilePart, L"_log.zip");
		
	hZip = CreateZip((LPVOID)lpFileName, 0, ZIP_FILENAME);	

	lastperiod = lstrrchrW( lpFileName, L'\\');
	if (lastperiod)
		lastperiod[0] = '\0';
	CWComString dir = lpFileName;
	WIN32_FIND_DATAW fdData;
	HANDLE hFind = FindFirstFileW( dir + L"\\Vacademia\\Log\\*.*", &fdData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		CWComString sFileName;
		FILETIME lastTime;
		lastTime.dwHighDateTime = 0;
		lastTime.dwLowDateTime = 0;
		do 
		{
			if ((fdData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				FILETIME time_ = fdData.ftLastWriteTime;
				if ( lastTime.dwHighDateTime < time_.dwHighDateTime)
				{
					lastTime = time_;
					sFileName = fdData.cFileName;
				}
				if ( lastTime.dwHighDateTime == time_.dwHighDateTime && lastTime.dwLowDateTime < time_.dwLowDateTime)
				{
					lastTime = time_;
					sFileName = fdData.cFileName;
				}
			}
		} while ( FindNextFileW( hFind, &fdData));

		if ( sFileName.GetLength() > 0)
		{
			pszFilePart = GetFilePart( lpFileName);
			rtl_wcscat( pszFilePart, MAX_PATH_4, L"\\Vacademia\\Log\\");
			rtl_wcscat( pszFilePart, MAX_PATH_4, sFileName.GetBuffer());

			WriteLog__( "Open log '");
			WriteLog__( W2A(lpFileName));
			WriteLog__( "'\n");

			FILE *fp = _wfopen( lpFileName, L"r");
			if ( fp == NULL)
			{
				delete[] lpFileName;
				return nFiles;
			}
			WriteLog__( "Read log...\n");
			int size = _filelength( fp->_file);
			char *sbuffer = new char[ size];
			int readed = fread( sbuffer, 1, size, fp);
			if ( readed != size)
			{
				delete[] sbuffer;
				fclose( fp);
				delete[] lpFileName;
				return nFiles;
			}

			WriteLog__( "Zip log...\n");
			ZRESULT zr = ZipAdd(hZip, W2A(sFileName.GetBuffer()), sbuffer, readed, ZIP_MEMORY);
			if (zr == ZR_OK)
			{
				nFiles++;
			}
			delete[] sbuffer;
			fclose( fp);
		}
	}

	WCHAR *scriptLogFileName = new WCHAR[ MAX_PATH_4];
	if ( !dumpParamsGetter.get( L"scriptLog", &scriptLogFileName, L"unknown"))
	{
		delete[] scriptLogFileName;
		if (hZip)
			CloseZip(hZip);
		delete[] lpFileName;
		return nFiles;
	}

	if (wcscmp(scriptLogFileName, L"unknown") != 0)
	{
		WriteLog__( "Open script log '");
		WriteLog__( W2A(scriptLogFileName));
		WriteLog__( "'\n");

		FILE *fp = _wfopen( scriptLogFileName, L"r");
		if ( fp == NULL)
		{	
			delete[] scriptLogFileName;
			if (hZip)
				CloseZip(hZip);
			delete[] lpFileName;
			return nFiles;
		}
		WriteLog__( "Read script log...\n");
		int size = _filelength( fp->_file);
		char *sbuffer = new char[ size];
		int readed = fread( sbuffer, 1, size, fp);
		if ( readed != size)
		{
			delete[] sbuffer;
			delete[] scriptLogFileName;
			delete[] lpFileName;
			fclose( fp);
			if (hZip)
				CloseZip(hZip);
			return FALSE;
		}

		WriteLog__( "Zip script log...\n");
		CComString s = scriptLogFileName;
		int pos = s.ReverseFind( '\\');
		if ( pos > -1)
			s = s.Right( s.GetLength() - pos - 1);
		zr = ZipAdd(hZip, s.GetBuffer(), sbuffer, readed, ZIP_MEMORY);
		if (zr == ZR_OK)
		{
			nFiles++;
		}
		delete[] sbuffer;
		fclose( fp);
	}

	if (hZip)
		CloseZip(hZip);

	WriteLog__( "ZipFiles end\n");

	delete[] scriptLogFileName;
	delete[] lpFileName;
	return nFiles;
}

int CReportMan::ZipBuffers( TCHAR *sDumpIndex, TZipItem *zipItems, int zipItemsCount)
{
	CDumpParamsGetter dumpParamsGetter;
	WriteLog__( "ZipBuffers start\n");	

	int nFiles = 0;
	WriteLog__( "Create zip...\n");
	HZIP hZip = NULL;	
	if ( zipItems[ 0].size == 0)
	{
		// дамп нулевой - все равно формируем архив. 4048 байтов точно хватит для упаковки нулевого дампа!
		hZip = CreateZip(0, 4048, ZIP_MEMORY);
	}
	else
		hZip = CreateZip(0, zipItems[ 0].size, ZIP_MEMORY);

	if ( hZip == NULL)
	{
		WriteLog__( "ERROR: create zip for dump\n");
		return FALSE;
	}	
	
	HRESULT zr = ZR_OK;
	if ( zipItems[ 0].size == 0)
		zr = ZipAdd(hZip, zipItems[ 0].name, "none", 4, ZIP_MEMORY);
	else
		zr = ZipAdd(hZip, zipItems[ 0].name, zipItems[ 0].buffer, zipItems[ 0].size, ZIP_MEMORY);
	if (zr == ZR_OK)
	{
		nFiles++;
	}
	else
	{
		WriteLog__( "ERROR\n");
		if (hZip)
			CloseZip(hZip);
		return FALSE;
	}

	void *buf;
	unsigned long size;
	ZipGetMemory(hZip, &buf, &size);
	m_zipDumpData.update( buf, size);

	if (hZip)
		CloseZip(hZip);

	if ( zipItemsCount < 3)
		return TRUE;

	hZip = CreateZip(0, zipItems[ 2].size + zipItems[ 3].size, ZIP_MEMORY);
	if ( hZip == NULL)
	{
		WriteLog__( "ERROR: create zip for log\n");
		return nFiles;
	}

	WriteLog__( "Zip log...\n");
	zr = ZipAdd(hZip, zipItems[ 2].name, zipItems[ 2].buffer, zipItems[ 2].size, ZIP_MEMORY);
	if (zr == ZR_OK)
	{
		nFiles++;
	}
	else
	{
		WriteLog__( "ERROR: log\n");
		if (hZip)
			CloseZip(hZip);
		return nFiles;
	}

	if ( zipItemsCount > 3)
	{		
		WriteLog__( "Zip script log...\n");
		zr = ZipAdd(hZip, zipItems[ 3].name, zipItems[ 3].buffer, zipItems[ 3].size, ZIP_MEMORY);
		if (zr == ZR_OK)
		{
			nFiles++;
		}
		else
		{
			WriteLog__( "ERROR: script log\n");
		}
	}

	void *buf2;
	unsigned long size2;
	ZipGetMemory(hZip, &buf2, &size2);
	m_zipLogData.update( buf2, size2);

	if (hZip)
		CloseZip(hZip);

	WriteLog__( "ZipBuffers end\n");	
	return nFiles;
}

//////////////////////////////////////////////////////////////////////////

DWORD WINAPI CReportMan::sendThread_( VOID* param)
{
	return ((CReportMan*)param)->sendThread();
}

DWORD WINAPI CReportMan::runThread_( VOID* param)
{
	return ((CReportMan*)param)->run( false);
}

#include "CrashHandlerInThread.h"

DWORD WINAPI CReportMan::runThreadIn_( VOID* param)
{
	CCrashHandlerInThread ch;
	ch.SetThreadExceptionHandlers();
	return ((CReportMan*)param)->run( true);
}

DWORD CReportMan::sendThread()
{
	CoInitialize(NULL);

	m_pWndError->SetStep( CWndError::ZIPPING);

	int dumpIndex = checkMDumpExisted( TRUE);
	if ( dumpIndex > 0)
	{
		runOnce( dumpIndex, TRUE);	
	}
	else
	{
		// архивов нет, тогда возможно есть dmp-файл
		m_pWndError->SetStep( CWndError::ZIPPING);

		int nFiles = ZipFiles( m_iCrashType, m_appName);
		if ( nFiles < 1)
		{	
			m_dwStatusCode = 1001;
			m_bStartSending = FALSE;
			CloseLog__();
			CoUninitialize();
			return 0;
		}

		sendData( NO_INFO, m_iCrashType, TRUE);
	}

	m_bStartSending = FALSE;
	m_pWndError->End();
	CloseLog__();

	/*m_pWndError->SetStep( CWndError::ZIPPING);

	int nFiles = ZipFiles( m_iCrashType, m_appName);
	if ( nFiles < 1)
	{	
		m_dwStatusCode = 1001;
		m_bStartSending = FALSE;
		CloseLog__();
		CoUninitialize();
		return 0;
	}

	sendData( NO_INFO, m_iCrashType, TRUE);

	m_bStartSending = FALSE;
	m_pWndError->End();
	CloseLog__();
	*/
	CoUninitialize();
	return 0;
}

//////////////////////////////////////////////////////////////////////////

int CReportMan::checkMDumpExisted( BOOL bLastIndex /* = FALSE */)
{
	std::string szProjectName = m_appName;
	int pos = szProjectName.rfind( ".exe");
	if ( pos > -1)
		szProjectName = szProjectName.substr( 0, pos);
	WCHAR fullAppName[ MAX_PATH_4];
	getApplicationDataDirectory( fullAppName);
	std::wstring sDir = fullAppName;
	sDir += L"\\*.*";
	int dumpIndexMax = 0;
	WIN32_FIND_DATAW fdData;
	HANDLE hFind = FindFirstFileW( sDir.c_str(), &fdData);	
	USES_CONVERSION;
	if (hFind != INVALID_HANDLE_VALUE)
	{
		std::string sFileName;
		int dumpIndex = 0;
		do 
		{
			if ((fdData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				sFileName = W2A(fdData.cFileName);
				int pos = sFileName.find( szProjectName);
				int pos2 = sFileName.find( ".");
				int pos3 = sFileName.find( ".zip");
				if ( pos > -1 && pos < pos2 - szProjectName.length()
					&& pos3 == sFileName.length() - 4)
				{
					pos += szProjectName.length();
					sFileName = sFileName.substr( pos + 1, pos2 - pos - 1);
					dumpIndex = rtl_atoi( sFileName.c_str());
					if ( dumpIndex > dumpIndexMax)
					{
						if ( !bLastIndex)
							return dumpIndex;
						dumpIndexMax = dumpIndex;
					}
				}
			}
		} while ( FindNextFileW( hFind, &fdData));
	}
	return dumpIndexMax;
}

BOOL CReportMan::sendShortReport( TCHAR *serverName, TCHAR* query, CInfoAnalyzer *pInfoAnalyzer, CDataBuffer *buffer /* = NULL */)
{
	IReportBase *pReport = NULL;
	TCHAR *sData = NULL;
	int len = 0;

	if ( m_iQueryType == HTTP_QUERY)
	{
		CComString sBuffer = pInfoAnalyzer->GetStringData();

		if ( strlen( m_appVersion) > 7)
		{
			sBuffer += CComString( "&version=") + CComString( m_appVersion);
		}
		//_tcscpy( sData, sBuffer.GetBuffer());
		sData = new TCHAR[ 3 * MAX_PATH];
		ZeroMemory( sData, 3 * MAX_PATH);
		//rtl_strcpy(sData, 3 * MAX_PATH, " ");
		rtl_strcpy(sData, 3 * MAX_PATH, sBuffer.GetBuffer());
		pReport = m_pHTTPReport;
	}
	else
	{
		CDataBuffer dataBuffer;
		pInfoAnalyzer->GetData( &dataBuffer);
		pReport = m_pSocketReport;
		len = dataBuffer.getLength();
		sData = new TCHAR[ len];
		rtl_memcpy( sData, len, dataBuffer, len);
	}
	
	if ( !pReport->sendData( (TCHAR *) serverName, query, 
		sData, len, buffer))
	{
		SAFE_DELETE_ARRAY(sData);
		std::string s = "ERROR sendShortReport \"";
		s += m_serverName;		
		s += query;
		s += "\"\n";		
		WriteLog__( (TCHAR *) s.c_str());
		CloseLog__();
		OpenLog__(NULL, FALSE, TRUE);
		m_dwStatusCode = pReport->getStatusCode();
		if ( m_iQueryType == SOCKET_QUERY)
		{
			return FALSE;
		}

		if ( m_pWndError != NULL)
		{
			m_pWndError->Back( CWndError::QUERY_GET);
		}

		CDataBuffer dataBuffer;
		pInfoAnalyzer->GetData( &dataBuffer);
		pReport = m_pSocketReport;
		len = dataBuffer.getLength();
		sData = new TCHAR[ len];
		rtl_memcpy( sData, len, dataBuffer, len);

		m_iQueryType == SOCKET_QUERY;
		pReport = m_pSocketReport;
		if ( !pReport->sendData( (TCHAR *) serverName, query, 
			sData, len, buffer))
		{
			m_dwStatusCode = pReport->getStatusCode();
			SAFE_DELETE_ARRAY(sData);
			return FALSE;
		}
	}

	SAFE_DELETE_ARRAY(sData);
	m_dwStatusCode = pReport->getStatusCode();
	return TRUE;
}

BOOL CReportMan::writeRemoteResource( LPCSTR aServerName, LPCWSTR aResPath, LPCTSTR dmpFileName)
{	
	CComString s;
	USES_CONVERSION;
	s.Format("writeRemoteResource starting... file=%s\n", W2A(aResPath));
	WriteLog__( s.GetBuffer());
	CloseLog__();
	OpenLog__(NULL, FALSE, TRUE);

	FILE *fp = _wfopen( aResPath, L"wb");
	if ( fp == NULL)
	{
		TCHAR *s = new TCHAR[ MAX_PATH_4];
		sprintf( s, "Can`t open file '%s' for write\n", W2A(aResPath));
		WriteLog__( s);
		CloseLog__();
		OpenLog__(NULL, FALSE, TRUE);
		delete[] s;
		m_dwStatusCode = 2003;
		return FALSE;
	}
	BYTE *pData = (BYTE*) m_dataBuffer;
	int wroteDataSize = fwrite( pData, 1, m_dataBuffer.getLength(), fp);
	if ( wroteDataSize <= 0)
	{
		TCHAR *s = new TCHAR[ MAX_PATH_4];
		sprintf( s, "Can`t write data to file '%s'\n", W2A(aResPath));
		WriteLog__( s);
		CloseLog__();
		OpenLog__(NULL, FALSE, TRUE);
		delete[] s;
		fclose( fp);
		return FALSE;
	}
	fclose( fp);	

	BYTE*	retData = NULL;
	DWORD	retSize;
	IReportBase *pReport = NULL;
	if ( m_iQueryType == HTTP_QUERY)
		pReport = m_pHTTPReport;
	else
		pReport = m_pSocketReport;

	if ( m_iQueryType == SOCKET_QUERY)
	{
		if ( !pReport->sendBufferData( (TCHAR *) aServerName, (BYTE *) m_dataBuffer2, m_dataBuffer2.getLength()))
		{
			m_dwStatusCode = pReport->getStatusCode();		
			return FALSE;
		}
		m_dwStatusCode = pReport->getStatusCode();
		DeleteFileW(aResPath);
		return TRUE;
	}

	if ( !pReport->sendFileData( (TCHAR *) aServerName, "/php/error_report.php", 
		(WCHAR *) aResPath, (BYTE *) m_dataBuffer, m_dataBuffer.getLength(), (TCHAR *) dmpFileName,
		&retSize, &retData))
	{
		if ( m_iQueryType == SOCKET_QUERY)
		{
			m_dwStatusCode = pReport->getStatusCode();		
			return FALSE;
		}
		WriteLog__(" try send with socket\n");
		m_iQueryType == SOCKET_QUERY;
		if ( m_pWndError != NULL)
			m_pWndError->Back( CWndError::QUERY_GET);
		pReport = m_pSocketReport;
		if ( !pReport->sendBufferData( (TCHAR *) aServerName, (BYTE *) m_dataBuffer2, m_dataBuffer2.getLength()))
		{			
			m_dwStatusCode = pReport->getStatusCode();
			CComString s;
			s.Format( "error send with socket dwStatusCode = %d\n", m_dwStatusCode);
			WriteLog__( s.GetBuffer());
			return FALSE;
		}
	}
	if ( retData != NULL)
		GlobalFree( retData);
	m_dwStatusCode = pReport->getStatusCode();
	s.Format( "send dwStatusCode = %d\n", m_dwStatusCode);
	WriteLog__( s.GetBuffer());
	DeleteFileW(aResPath);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

void CReportMan::init( TCHAR *appName, TCHAR *crashType, WCHAR *root)
{
	rtl_strcpy( m_appName, MAX_PATH, appName);
	WCHAR szModuleName[ MAX_PATH_4] = {L'\0'};
	//ZeroMemory(szModuleName, sizeof(szModuleName));
	if (GetModuleFileNameW(0, szModuleName, _countof(szModuleName)-2) <= 0)
		rtl_wcscpy(szModuleName, MAX_PATH_4, L"Unknown");
	rtl_wcscpy( m_fullAppName, MAX_PATH_4, szModuleName);
	WCHAR *appReportName = GetFilePart( szModuleName);
	USES_CONVERSION;
	rtl_strcpy( m_appReportName, MAX_PATH, W2A(appReportName));

	rtl_strcpy( m_crashType, 32, crashType);

	if ( stricmp( m_crashType, APP_HANG) == 0)
	{
		m_iCrashType = APP_HANG_ID;
	}
	else if ( stricmp( m_crashType, ASSERT_ERROR) == 0)// || stricmp( m_crashType, ASSERT_MAKE_USER) == 0)
	{
		m_iCrashType = ASSERT_ERROR_ID;
	}
	else if ( stricmp( m_crashType, APP_LAUNCHER_ERROR) == 0)
	{
		m_iCrashType = APP_LAUNCHER_ERROR_ID;
	}
	else if (stricmp( m_crashType, APP_PREV_CRASH) == 0)
	{
		m_iCrashType = APP_PREV_CRASH_ID;
	}
	else
	{
		m_iCrashType = APP_ERROR_ID;
	}

	CWComString szRoot = root;
	if ( szRoot.GetLength() > 6)
	{
		int pos = szRoot.Find( L"-root ");
		if ( pos > -1)
		{
			setModRoot( szRoot.Right( szRoot.GetLength() - pos - 6).GetBuffer());
		}		
	}

	if ( m_iCrashType != APP_PREV_CRASH_ID)
	{
		WCHAR errFileName[ MAX_PATH_4];
		if ( m_iCrashType == APP_HANG_ID)
			getFileNameByProjName(  L"crash", L"hang", errFileName);
		else if ( m_iCrashType == ASSERT_ERROR_ID)
			getFileNameByProjName(  L"crash", L"assert", errFileName);
		else if ( m_iCrashType == APP_ERROR_ID)
			getFileNameByProjName( L"crash", L"err", errFileName);
		else if ( m_iCrashType == APP_LAUNCHER_ERROR_ID)
			getFileNameByProjName( L"crash", L"l_err", errFileName);

		rtl_wcscpy( m_errFileName, MAX_PATH_4, errFileName);

		WriteLog__( "Reading from player.ini\n");
	
		defineAppParams();

		WriteLog__( "AppVersion = ");
		WriteLog__( m_appVersion);
		WriteLog__( "\n");
		
		WriteLog__( "Language = ");
		WriteLog__( m_language);
		WriteLog__( "\n");

		CloseLog__();
		OpenLog__(NULL, FALSE, TRUE);
	}
	else
	{
		
	}
}

void CReportMan::setServiceWorldManager( service::IServiceWorldManager *pServiceManager)
{	
	((CSocketReport *) m_pSocketReport)->setServiceWorldManager( pServiceManager);
}

BOOL CReportMan::sendAsynch( CWndError *pWndError)
{	
	m_bStartSending = TRUE;	
	m_pWndError = pWndError;
	m_hSendThread = CreateThread( NULL, THREAD_STACK_SIZE, sendThread_, this, NULL, NULL);	
	return TRUE;
}

BOOL CReportMan::sendData( int infoType, int iCrashType, BOOL bInfoShow /* = FALSE */ )
{
	if ( infoType < FULL_INFO)
		return FALSE;

	if ( infoType == FULL_INFO)
	{
		std::string s = "There is yet crash '";
		s += iCrashType == APP_HANG_ID ? APP_HANG : ( iCrashType == ASSERT_ERROR_ID ? ASSERT_ERROR : ( iCrashType == APP_LAUNCHER_ERROR_ID ? APP_LAUNCHER_ERROR : APP_ERROR));
		s += "' in the database\n";
		WriteLog__( (TCHAR *) s.c_str());
		return TRUE;
	}

	int dumpIndex = checkMDumpExisted( TRUE);
	WCHAR sDumpIndex[ 4];
	_itow( dumpIndex, sDumpIndex, 10);
	std::wstring sFormatName = L"player_";
	sFormatName += sDumpIndex;
	sFormatName += L".zip";
	WCHAR fileName[ MAX_PATH_4];	
	getFileNameByProjName( sFormatName.c_str(), L"/q", fileName);

	if ( bInfoShow)
		m_pWndError->SetStep( CWndError::QUERY_GET);

	BOOL bSucc = FALSE;
	if ( infoType == NO_INFO)
	{
		WriteLog__("NO_INFO\n");
		bSucc = sendShortInfo( iCrashType);
		if ( !bSucc)
		{
			return FALSE;
		}
	}
	else
		WriteLog__("SHORT_INFO\n");

	if ( bInfoShow)
		m_pWndError->SetStep( CWndError::QUERY_POST);

	bSucc = sendRemoteResource( iCrashType);
	if ( !bSucc)
	{		
		return FALSE;
	}
	if ( bSucc)
		DeleteFileW( fileName);
	return bSucc;
}

BOOL CReportMan::sendDump( int iCrashType, BOOL bInternal)
{
	OpenLog__( NULL, FALSE, TRUE);
	WCHAR errFileName[ MAX_PATH_4] = {0};	
	std::string s = "\nChecking and sending files, having type '";
	s += iCrashType == APP_HANG_ID ? APP_HANG : ( iCrashType == ASSERT_ERROR_ID ? ASSERT_ERROR : APP_ERROR);
	s += "', of app '";
	s += m_appName;
	s += "'\n";
	//sprintf( s, "\nChecking and sending files, having type '%s', of app '%s'\n", APP_ERROR, m_appName);
	WriteLog__( (TCHAR *) s.c_str());
	if ( iCrashType == APP_HANG_ID)
		getFileNameByProjName( L"crash", L"hang", errFileName);
	else if ( iCrashType == ASSERT_ERROR_ID)
		getFileNameByProjName( L"crash", L"assert", errFileName);
	else if ( iCrashType == APP_ERROR_ID)
		getFileNameByProjName( L"crash", L"err", errFileName);	
	else if ( iCrashType == APP_LAUNCHER_ERROR_ID)
		getFileNameByProjName( L"crash", L"l_err", errFileName);	

	rtl_wcscpy( m_errFileName, MAX_PATH_4, errFileName);
		
	if ( bInternal)
	{
		WCHAR *pszFilePart = GetFilePart( errFileName);		
		int remainsCount = countof(errFileName) - wcslen( errFileName) - 1;
		if ( iCrashType == APP_HANG_ID)
		{
			rtl_wcscpy( pszFilePart, remainsCount, XCRASHREPORT_ERROR_HANG_LOG_FILE);
		}
		else if ( iCrashType == ASSERT_ERROR_ID)
		{
			rtl_wcscpy( pszFilePart, remainsCount, XCRASHREPORT_ERROR_ASSERT_LOG_FILE);
		}
		else if ( iCrashType == APP_LAUNCHER_ERROR_ID)
		{
			rtl_wcscpy( pszFilePart, remainsCount, XCRASHREPORT_LAUCNHER_ERROR_LOG_FILE);
		}
		else
		{
			rtl_wcscpy( pszFilePart, remainsCount, XCRASHREPORT_ERROR_LOG_FILE);
		}
		CInfoAnalyzer *pInfoAnalyzer = new CInfoAnalyzer();
		if ( !pInfoAnalyzer->LoadFromFile( errFileName))
		{
			CloseLog__();
			return FALSE;
		}

		pInfoAnalyzer->DisableEncoding();
		rtl_strcpy( m_serverName, MAX_PATH, pInfoAnalyzer->GetServerName());
		delete pInfoAnalyzer;
	}

	BOOL bSucc = FALSE;
	int nFiles = ZipFiles( iCrashType, m_appName);
	if ( nFiles > 0)
	{	
		bSucc = sendData( NO_INFO, iCrashType, FALSE);
	}

	CloseLog__();
	return bSucc;
}

BOOL CReportMan::runOnce( int dumpIndex, BOOL bInfoShow /* = FALSE */)
{
	BOOL bSended = TRUE;
	TCHAR sMsg[ MAX_PATH_4];
	TCHAR sDumpIndex[ 4];
	_itoa( dumpIndex, sDumpIndex, 10);

	std::string szProjectName = m_appName;
	int pos = szProjectName.rfind( ".exe");
	if ( pos > -1)
	{
		szProjectName = szProjectName.substr( 0, pos);
		if ( szProjectName.find( "\"") == 0)
		{
			szProjectName += L'"';
		}
		else if ( szProjectName.find( "'") == 0)
		{
			szProjectName += L'\'';
		}
	}

	std::string sFormatName = szProjectName;
	sFormatName += "_";
	sFormatName += sDumpIndex;
	sFormatName += ".zip";

	sprintf( sMsg, "CReportMan::runOnce sFormatName='%s'\n", sFormatName.c_str());	
	WriteLog__( sMsg);

	WCHAR fileName[ MAX_PATH_4];
	USES_CONVERSION;
	getFileNameByProjName( A2W(sFormatName.c_str()), L"/q", fileName);

	sprintf( sMsg, "CReportMan::runOnce fileName='%s'\n", W2A(fileName));
	WriteLog__( sMsg);

	HZIP hzip = OpenZip( fileName, NULL, ZIP_FILENAME);
	//CloseHandle( hFile);
	if ( hzip == NULL)
	{
		sprintf( sMsg, "ERROR: can't opened of zip '%s'", W2A(fileName));
		WriteLog__( sMsg);
		DeleteFileW( fileName);
		Sleep( 10);
		return FALSE;
	}

	TZipItem zipItems[ 5];

	ZIPENTRY ze;
	int zipItemsCount = 0;
	for ( int i = 0; i < 5; i++)
	{
		HRESULT hr = GetZipItemA( hzip, i, &ze);
		if ( hr != ZR_OK)
			break;

		rtl_strcpy( zipItems[ i].name, MAX_PATH, ze.name);
		if ( i > 0)
		{
			zipItems[ i].buffer = new BYTE[ ze.unc_size + 1];
			zipItems[ i].size = ze.unc_size + 1;
		}
		else
		{
			zipItems[ i].buffer = new BYTE[ ze.unc_size];
			zipItems[ i].size = ze.unc_size;
		}
		UnzipItem( hzip, i, zipItems[ i].buffer, ze.unc_size, ZIP_MEMORY);
		if ( i > 0)
		{
			char *buffer = (char *) zipItems[ i].buffer;
			buffer[ze.unc_size] = '\0';
		}
		zipItemsCount++;
	}

	if ( zipItemsCount < 2)
	{
		// в архиве файлов меньше двух
		CloseZip( hzip);
		sprintf( sMsg, "ERROR: Zip-file '%s' has less 2 files", W2A(fileName));
		WriteLog__( sMsg);
		DeleteFileW( fileName);
		Sleep( 10);
		return FALSE;
	}

	std::string sPlayer = zipItems[ 0].name;
	if ( sPlayer.find( szProjectName) == -1 && sPlayer.rfind( ".dmp") == -1)
	{
		// в архиве нет файла, имя которого содержит имя приложения, c расширением dmp
		CloseZip( hzip);
		sprintf( sMsg, "ERROR: Zip-file '%s' hasn't dmp-file", W2A(fileName));
		WriteLog__( sMsg);
		DeleteFileW( fileName);
		Sleep( 10);
		return FALSE;
	}

	std::string sInfo = zipItems[ 1].name;
	if ( sInfo.find( "INFO") == -1 
		&& sInfo.find( "info") == -1 
		&& sInfo.find( "Info") == -1 
		&& sInfo.rfind( ".txt") == -1)
	{
		// в архиве нет файла, имя которого содержит 'info', c расширением txt
		CloseZip( hzip);
		sprintf( sMsg, "ERROR: Zip-file '%s' hasn't info", W2A(fileName));
		WriteLog__( sMsg);
		DeleteFileW( fileName);
		Sleep( 10);
		return FALSE;
	}

	m_dataBuffer.clear();
	m_zipLogData.clear();
	m_dataBuffer2.clear();

	if ( ZipBuffers( sDumpIndex, zipItems, zipItemsCount) < 1)
	{
		CloseZip( hzip);
		sprintf( sMsg, "ERROR: ZipBuffer is failed");
		WriteLog__( sMsg);
		// делаем паузу
		Sleep( 10);
		return FALSE;
	}

	if ( bInfoShow)
		m_pWndError->SetStep( CWndError::QUERY_GET);

	CInfoAnalyzer *pInfoAnalyzer = new CInfoAnalyzer();
	pInfoAnalyzer->LoadFromData( (char *) zipItems[ 1].buffer, zipItems[ 1].size);	
/*
	CComString sSessionID = pInfoAnalyzer->GetSessionID();
	if ( sSessionID.GetLength() < 1 || sSessionID.CompareNoCase( "0") == 0)
	{
		// добавить поля
		// #5879 Отправка данных пользователя по его желанию, если было падение до авторизации пользователя
	}
*/
	pInfoAnalyzer->DisableEncoding();
	rtl_strcpy( m_serverName, MAX_PATH, pInfoAnalyzer->GetServerName());
	rtl_strcpy( m_appVersion, 32, pInfoAnalyzer->GetAppVersion());
	rtl_strcpy( m_language, 32, pInfoAnalyzer->GetLanguage());
	rtl_strcpy( m_crashType, 32, pInfoAnalyzer->GetErrorType());

	sprintf( sMsg, "CReportMan::runOnce params: m_serverName='%s' m_appVersion='%s' m_language='%s' m_crashType='%s'\n", m_serverName, m_appVersion, m_language, m_crashType);
	WriteLog__( sMsg);
	
	if ( stricmp( m_crashType, APP_HANG) == 0 || stricmp( m_crashType, "noerror ") == 0)
	{
		m_iCrashType = APP_HANG_ID;
	}
	else if ( stricmp( m_crashType, ASSERT_ERROR) == 0 || stricmp( m_crashType, "assert ") == 0)
	{
		m_iCrashType = ASSERT_ERROR_ID;
	}
	else if ( stricmp( m_crashType, ASSERT_MAKE_USER) == 0 )
	{
		m_iCrashType = ASSERT_ERROR_ID;
	}
	else if ( stricmp( m_crashType, APP_LAUNCHER_ERROR) == 0 || stricmp( m_crashType, "launcher ") == 0)
	{
		m_iCrashType = APP_LAUNCHER_ERROR_ID;
	}
	else
	{
		m_iCrashType = APP_ERROR_ID;
	}

	pInfoAnalyzer->EnableEncoding();		

	BOOL bSucc = sendShortReport( m_serverName, "/php/error_report_get.php", pInfoAnalyzer);

	if ( !bSucc)
	{
		sprintf( sMsg, "ERROR: Short info is not sending to server 's'", m_serverName);
		WriteLog__( sMsg);
		delete pInfoAnalyzer;
		CloseZip( hzip);
		// здесь Sleep ставить не надо, так как Sleep есть уже внутри метода sendShortReport
		return FALSE;
	}

	delete pInfoAnalyzer;	

	if ( bInfoShow)
		m_pWndError->SetStep( CWndError::QUERY_POST);

	m_dataBuffer.addData( m_zipDumpData.size, (BYTE*) m_zipDumpData.buffer);

	addSeparate();

	if ( strlen( m_appVersion) > 7)
	{
		CComString s;
		s.Format("Version:%s;", m_appVersion);
		m_dataBuffer.addData( s.GetLength(), (BYTE *) s.GetBuffer());
	}


	m_dataBuffer2.addData( sizeof(unsigned short), (BYTE*) &zipItemsCount);
	m_dataBuffer2.add( (unsigned int) m_zipDumpData.size, (BYTE*) m_zipDumpData.buffer);
	m_dataBuffer2.add( (unsigned int) zipItems[ 1].size, (BYTE*) zipItems[ 1].buffer);

	WCHAR *pszFilePart = NULL;

	m_dataBuffer.addData( zipItems[ 1].size, (BYTE*) zipItems[ 1].buffer);
	if ( zipItemsCount > 4)
	{
		m_dataBuffer.addData( 14, (BYTE *) "\r\n\r\nLogdata:\r\n");
		m_dataBuffer.addData( zipItems[ 4].size, (BYTE*) zipItems[ 4].buffer);

		BOOL bCrashLog = FALSE;
		WCHAR logFileName[ MAX_PATH_4];
		wcscpy( logFileName, fileName);
		pszFilePart = GetFilePart( logFileName);
		wcscpy( pszFilePart, L"CrashReport.txt.inf");
		FILE *fp_log = _wfopen( logFileName, L"r");
		if ( fp_log != NULL)
		{
			int log_size = _filelength( fp_log->_file);
			BYTE *bytes = new BYTE[ log_size];
			int readed = fread( bytes, 1, log_size, fp_log);
			if ( readed > 0)
			{
				CComString s = "------------------\r\n";
				m_dataBuffer.addData( s.GetLength(), (BYTE*) s.GetBuffer());
				m_dataBuffer.addData( readed, bytes);

				int size = zipItems[ 4].size;
				size += s.GetLength() + readed;
				m_dataBuffer2.addData( sizeof(unsigned int), (BYTE*) &size);
				m_dataBuffer2.addData( zipItems[ 4].size, (BYTE*) zipItems[ 4].buffer);
				m_dataBuffer2.addData( s.GetLength(), (BYTE*) s.GetBuffer());
				m_dataBuffer2.addData( readed, bytes);
				bCrashLog = TRUE;
			}
		}

		if ( !bCrashLog)
		{
			m_dataBuffer2.add( (unsigned int) zipItems[ 4].size, (BYTE*) zipItems[ 4].buffer);
		}
	}

	addSeparate();
	if ( zipItemsCount > 2 && m_zipLogData.size > 0)
	{
		m_dataBuffer.addData( m_zipLogData.size, (BYTE*) m_zipLogData.buffer);
		m_dataBuffer2.add( (unsigned int) m_zipLogData.size, (BYTE*) m_zipLogData.buffer);
	}

	wcscpy( m_errFileName, fileName);
	pszFilePart = GetFilePart( m_errFileName);
	wcscpy( pszFilePart, L"crash_temp_");
	wcscat( pszFilePart, A2W(sDumpIndex));
	wcscat( pszFilePart, L".err");

	CloseZip( hzip);

	TCHAR dmpFileName[ MAX_PATH];
	_tcscpy( dmpFileName, szProjectName.c_str());
	_tcscat( dmpFileName, "_temp_");
	_tcscat( dmpFileName, sDumpIndex);
	_tcscat( dmpFileName, ".zip");	

	bSended = writeRemoteResource( m_serverName, m_errFileName, dmpFileName);

	if ( bSended)
		DeleteFileW( fileName);

	return bSended;
}

BOOL CReportMan::run( bool bInApp)
{
	BOOL bSended = TRUE;
	int dumpIndex = 0;
	while ( ( dumpIndex = checkMDumpExisted(), dumpIndex > 0) && !bTerminate)
	{	
		bSended = runOnce( dumpIndex);
	}

	CloseLog__();

	sendDump( APP_ERROR_ID, TRUE);
	sendDump( ASSERT_ERROR_ID, TRUE);
	sendDump( APP_HANG_ID, TRUE);

	if ( !bInApp)
	{
		OpenLog__( NULL, FALSE, TRUE);
	}
	
	m_bStartSending = FALSE;
	return bSended;
}

BOOL CReportMan::runAsynch( service::IServiceWorldManager *pServiceManager)
{
	setServiceWorldManager( pServiceManager);
	m_bStartSending = TRUE;		
	m_hSendThread = CreateThread( NULL, THREAD_STACK_SIZE, runThread_, this, NULL, NULL);	
	return m_hSendThread != NULL;	
}

BOOL CReportMan::runAsynchIn( service::IServiceWorldManager *pServiceManager)
{
	setServiceWorldManager( pServiceManager);
	m_bStartSending = TRUE;		
	m_hSendThread = CreateThread( NULL, THREAD_STACK_SIZE, runThreadIn_, this, NULL, NULL);	
	return m_hSendThread != NULL;
}

BOOL CReportMan::sendShortInfo( int iCrashType)
{
	WriteLog__("sendShortInfo is starting\n");
	CloseLog__();
	OpenLog__(NULL, FALSE, TRUE);
	WCHAR *fileName = new WCHAR[ MAX_PATH_4];	
	rtl_wcscpy( fileName, MAX_PATH_4, m_errFileName);
	WCHAR *pszFilePart = GetFilePart( fileName);
	int remainsCount = MAX_PATH_4 - wcslen( fileName) - 1;
	if ( iCrashType == APP_HANG_ID)
	{
		rtl_wcscpy( pszFilePart, remainsCount, XCRASHREPORT_ERROR_HANG_LOG_FILE);
	}
	else if ( iCrashType == ASSERT_ERROR_ID)
	{
		rtl_wcscpy( pszFilePart, remainsCount, XCRASHREPORT_ERROR_ASSERT_LOG_FILE);
	}
	else
	{
		rtl_wcscpy( pszFilePart, remainsCount, XCRASHREPORT_ERROR_LOG_FILE);
	}

	CInfoAnalyzer *pInfoAnalyzer = new CInfoAnalyzer();
	if ( !pInfoAnalyzer->LoadFromFile( fileName))
	{
		CComString s;
		s.Format("ERROR Load file %s is failed!\n", fileName);
		WriteLog__(s.GetBuffer());
		CloseLog__();
		OpenLog__(NULL, FALSE, TRUE);
		delete pInfoAnalyzer;
		delete[] fileName;
		m_dwStatusCode = 1002;
		return FALSE;
	}

	BOOL bSucc = sendShortReport( m_serverName, "/php/error_report_get.php", pInfoAnalyzer);
	
	delete pInfoAnalyzer;
	delete[] fileName;
	return bSucc;
}

BOOL CReportMan::sendRemoteResource( int iCrashType)
{
	WriteLog__("sendRemoteResource is starting\n");
	CloseLog__();
	OpenLog__(NULL, FALSE, TRUE);
	m_dataBuffer.clear();
	m_dataBuffer2.clear();
	if ( wcslen( m_errFileName) < 1)
	{
		m_dwStatusCode = 2006;
		return FALSE;
	}

	CComString appVersion;
	appVersion.Format( "Version:%s;", m_appVersion);
	CComString serverName = m_serverName;	

	// дамп
	WCHAR *fileName = new WCHAR[ MAX_PATH_4];	
	rtl_wcscpy( fileName, MAX_PATH_4, m_errFileName);
	WCHAR *pszFilePart = GetFilePart( fileName);
	USES_CONVERSION;
	rtl_wcscpy(pszFilePart, MAX_PATH_4 - wcslen( fileName) - 1, A2W(m_appName));
	WCHAR *lastperiod = lstrrchrW( pszFilePart, L'.');
	if (lastperiod)
		lastperiod[0] = 0;
	if ( iCrashType == APP_HANG_ID)
	{
		wcscat( pszFilePart, L"_hang");
	}
	else if ( iCrashType == ASSERT_ERROR_ID)
	{
		wcscat( pszFilePart, L"_assert");
	}
	wcscat( pszFilePart, L".zip");

	TCHAR *dmpFileName = new TCHAR[ MAX_PATH];
	rtl_strcpy( dmpFileName, MAX_PATH, W2A(pszFilePart));

	WriteLog__("readDataFromFile dmpFileName=\"");
	WriteLog__( W2A(fileName));
	WriteLog__("\"\n");
	CloseLog__();
	OpenLog__(NULL, FALSE, TRUE);

	int itemsCount = 5;
	m_dataBuffer2.addData( sizeof(unsigned short), (BYTE*) &itemsCount);
		
	if ( !readDataFromFile( fileName))
	{
		delete[] fileName;
		TCHAR *s = new TCHAR[ MAX_PATH_4];
		sprintf( s, "Can`t read file '%s'\n", W2A(fileName));
		WriteLog__( s);
		CloseLog__();
		OpenLog__(NULL, FALSE, TRUE);		
		m_dwStatusCode = 2001;
		m_dataBuffer2.add( (unsigned int) strlen( s), (BYTE *) s);
		delete[] s;
		return FALSE;	
	}
	
	wcscpy( fileNameForDelete[ 1].s, fileName);

	addSeparate();	

	if ( appVersion.GetLength() > 7)
		m_dataBuffer.addData( appVersion.GetLength(), (BYTE *) appVersion.GetBuffer());
	
	// текстовая информация
	pszFilePart = GetFilePart( fileName);
	int remainsCount = MAX_PATH_4 - wcslen( fileName) - 1;
	if ( iCrashType == APP_HANG_ID)
	{
		rtl_wcscpy( pszFilePart, remainsCount, XCRASHREPORT_ERROR_HANG_LOG_FILE);
	}
	else if ( iCrashType == ASSERT_ERROR_ID)
	{
		rtl_wcscpy( pszFilePart, remainsCount, XCRASHREPORT_ERROR_ASSERT_LOG_FILE);
	}
	else
	{
		rtl_wcscpy( pszFilePart, remainsCount, XCRASHREPORT_ERROR_LOG_FILE);
	}

	WriteLog__("readDataFromFile textFileName=\"");
	WriteLog__(fileName);	
	WriteLog__("\"\n");
	CloseLog__();
	OpenLog__(NULL, FALSE, TRUE);
	
	if ( !readDataFromFile( fileName))
	{
		delete[] fileName;
		TCHAR *s = new TCHAR[ MAX_PATH_4];
		sprintf( s, "Can`t read file '%s'\n", W2A(fileName));
		WriteLog__( s);
		CloseLog__();
		OpenLog__(NULL, FALSE, TRUE);		
		m_dwStatusCode = 2002;
		m_dataBuffer2.add( (unsigned int) strlen( s), (BYTE *) s);
		delete[] s;
		return FALSE;
	}

	wcscpy( fileNameForDelete[ 2].s, fileName);

	pszFilePart = GetFilePart( fileName);
	rtl_wcscpy( pszFilePart, MAX_PATH - wcslen(fileName) - 1, L"mdump.txt");
	m_dataBuffer.addData( 14, (BYTE *) "\r\n\r\nLogdata:\r\n");

	WriteLog__("readDataFromFile mdumpLogFileName=\"");
	WriteLog__(fileName);	
	WriteLog__("\"\n");
	CloseLog__();
	OpenLog__(NULL, FALSE, TRUE);

	if ( !readDataFromFile( fileName))
	{
		TCHAR *s = new TCHAR[ MAX_PATH_4];
		sprintf( s, "Can`t read file '%s'\n", W2A(fileName));
		WriteLog__( s);
		CloseLog__();
		OpenLog__(NULL, FALSE, TRUE);
		m_dataBuffer2.add( (unsigned int) strlen( s), (BYTE *) s);
		//m_dataBuffer2.add( (unsigned int) wcslen( s)*sizeof(WCHAR), (BYTE *) s);
		delete[] s;
		m_dwStatusCode = 2003;
	}
	
	addSeparate();
	// информация лог
	pszFilePart = GetFilePart( fileName);
	remainsCount = MAX_PATH_4 - wcslen( fileName) - 1;
	if ( iCrashType == APP_HANG_ID)
	{
		rtl_wcscpy( pszFilePart, remainsCount, L"player_hang_log.zip");
	}
	else if ( iCrashType == ASSERT_ERROR_ID)
	{
		rtl_wcscpy( pszFilePart, remainsCount, L"player_assert_log.zip");
	}
	else
	{
		rtl_wcscpy( pszFilePart, remainsCount, L"player_log.zip");
	}

	WriteLog__("readDataFromFile logFileName=\"");
	WriteLog__(fileName);	
	WriteLog__("\"\n");
	CloseLog__();
	OpenLog__(NULL, FALSE, TRUE);
	
	if ( !readDataFromFile( fileName))
	{
		TCHAR *s = new TCHAR[ MAX_PATH_4];
		sprintf( s, "Can`t read file '%s'\n", W2A(fileName));
		WriteLog__( s);
		CloseLog__();
		OpenLog__(NULL, FALSE, TRUE);
		m_dataBuffer2.add( (unsigned int) strlen( s), (BYTE *) s);
		delete[] s;
		m_dwStatusCode = 2005;
		//return FALSE;
	}

	wcscpy( fileNameForDelete[ 3].s, fileName);

	delete[] fileName;

	BOOL bSended = TRUE;
	bSended = writeRemoteResource( serverName.GetBuffer(), m_errFileName, dmpFileName);
	while ( !bSended)
	{
		if ( m_pWndError != NULL)
		{
			m_pWndError->Stop();
			m_pWndError->Back( CWndError::QUERY_POST);
		}
		WriteLog__( "sendRemoteResource::writeRemoteResource error code 2004\n");
		// ask the user if they want to save a dump file
		/*TCHAR *szScratch = new TCHAR[ MAX_PATH];
		if ( m_iLanguage == ENG)
			sprintf( szScratch, "Error data were not sent. Would you like to resend error data?");
		else
			sprintf( szScratch, "Данные об ошибке не отправлены. Повторить отправку данных об ошибке?");

		//if (::MessageBox( NULL, szScratch, (m_iLanguage == ENG ? "Virtual Academia" : "Виртуальная академия"), MB_YESNO ) == IDYES)
		//{
		//	delete[] szScratch;
		//	m_pWndError->Start();
		//	bSended = writeRemoteResource( serverName.GetBuffer(), m_errFileName, dmpFileName);
		//}
		//else
		//{
			delete[] szScratch;
		*/
			m_dwStatusCode = 2004;
			break;
		//}
	}

	
	CloseLog__();
	OpenLog__(NULL, FALSE, TRUE);

	delete[] dmpFileName;

	if ( bSended)
	{			
		BOOL bSucc = DeleteFileW( fileNameForDelete[ 0].s);
		bSucc = DeleteFileW( fileNameForDelete[ 1].s);
		bSucc = DeleteFileW( fileNameForDelete[ 2].s);
		bSucc = DeleteFileW( fileNameForDelete[ 3].s);
	}

	return bSended;
}

DWORD CReportMan::getStatusCode()
{
	return m_dwStatusCode;
}

void CReportMan::setModRoot( WCHAR *aModRoot)
{
	rtl_wcscpy( m_modRoot, MAX_PATH_4, aModRoot);
}

BOOL CReportMan::defineAppParams()
{
	WCHAR *fileName = new WCHAR[ MAX_PATH_4];	

	if ( wcslen( m_modRoot) > 2)
	{
		rtl_wcscpy( fileName, MAX_PATH_4, m_modRoot);
		rtl_wcscat( fileName, MAX_PATH_4, L"\\");
		rtl_wcscat( fileName, MAX_PATH_4, CONFIG_FILE);
	}
	else
	{
		rtl_wcscpy( fileName, MAX_PATH_4, m_fullAppName);
		WCHAR *pszFilePart = GetFilePart( fileName);
		rtl_wcscpy(pszFilePart, MAX_PATH_4 - wcslen( fileName) - 1, CONFIG_FILE);
	}	

	FILE *fp = _wfopen( fileName, L"r");

	if ( fp == NULL)
	{
		rtl_wcscpy( fileName, MAX_PATH_4, m_fullAppName);
		WCHAR *pszFilePart = GetFilePart( fileName);
		rtl_wcscpy(pszFilePart, MAX_PATH_4 - wcslen( fileName) - 1, CONFIG_FILE);
		fp = _wfopen( fileName, L"r");
	}

	if ( fp != NULL)
	{
		USES_CONVERSION;
		char *str = new char[ BUFFER_SIZE];
		int size = _filelength( fp->_file);
		if ( fread( str, 1, size, fp) <= 0)
		{
			TCHAR *s = new TCHAR[ MAX_PATH_4];
			sprintf( s, "Size of config file '%s' is equal 0\n", W2A(fileName));
			WriteLog__( s);
			delete[] s;
			delete[] str;
			delete[] fileName;
			return FALSE;
		}
		delete[] str;
		fclose( fp);

		WCHAR *sLang = new WCHAR[ MAX_PATH];
		WCHAR *sAppVersion = new WCHAR[ MAX_PATH];
		WCHAR *sServerName = new WCHAR[ MAX_PATH];
		GetPrivateProfileStringW( L"settings", L"dumpserver", A2W(SERVER_NAME), sServerName, 128, fileName);
		GetPrivateProfileStringW( L"settings", L"version", L"vacademia_0.0.0.0_beta", sAppVersion, 128, fileName);
		GetPrivateProfileStringW( L"settings", L"language", L"eng", sLang, 128, fileName);
		if ( rtl_wcsicmp( sLang, L"rus") == 0)
		{
			m_iLanguage = RUS;
		}
		else
			m_iLanguage = ENG;		
		rtl_strcpy( m_serverName, MAX_PATH, W2A(sServerName));
		rtl_strcpy( m_appVersion, 32, W2A(sAppVersion));
		rtl_strcpy( m_language, 32, W2A(sLang));
		delete[] sServerName;
		delete[] sAppVersion;
		delete[] sLang;
		delete[] fileName;
		return TRUE;
	}

	rtl_strcpy( m_serverName, MAX_PATH, SERVER_NAME);
	delete[] fileName;
	
	return TRUE;
}

int CReportMan::getLanguage()
{
	return m_iLanguage;
}

BOOL CReportMan::isEnding()
{
	return !m_bStartSending;
}

void CReportMan::setProtocol( int aProtocol)
{
	m_iQueryType = aProtocol;
}

void CReportMan::terminate()
{
	bTerminate = TRUE;
}
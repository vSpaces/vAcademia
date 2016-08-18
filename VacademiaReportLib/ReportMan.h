#pragma once

#include "VacademiaReportLib.h"
#include <DataBuffer.h>
#include <ServiceMan.h>

#define RES_PATH_ERROR   0x01000000
#define RES_SCRIPT_ERROR 0x02000000
#define RES_SERVER_ERROR 0x03000000
#define RES_QUERY_ERROR  0x04000000
#define RES_READ_ERROR   0x05000000
#define RES_WRITE_ERROR  0x06000000
#define MAX_PATH_4		4 * MAX_PATH

class CDataBuffer;
class CWndError;
class IReportBase;
class CInfoAnalyzer;
class service::IServiceWorldManager;

class VACADEMIAREPORT_API CReportMan
{
public:
	CReportMan();
	CReportMan( BOOL bLog);
	virtual ~CReportMan();

	enum{ HTTP_QUERY = 11, SOCKET_QUERY = 12};

protected:	
	struct TZipData
	{
		TZipData()
		{
			buffer = NULL;
			size = 0;
		}
		virtual ~TZipData()
		{
			clear();
		}
		void clear()
		{
			if ( buffer != NULL) { delete[] buffer; buffer = NULL;}
			size = 0;
		}
		void update( void *buf, unsigned long isize)
		{
			clear();
			size = isize;
			buffer = new BYTE[ isize];
			memcpy( buffer, buf, isize);
		}
		void *buffer;
		unsigned long size;
	};

	struct TZipItem : public TZipData
	{		
		TCHAR name[ MAX_PATH];
		~TZipItem()
		{
			clear();
		}
	};

	BOOL readDataFromFile( WCHAR *fileName);	
	void addSeparate();
	int ZipFiles( int iCrashType, TCHAR *szProjectName);
	int ZipBuffers( TCHAR *sDumpIndex, TZipItem *zipItems, int zipItemsCount);

protected:
	static DWORD WINAPI sendThread_( VOID* param);
	static DWORD WINAPI runThread_( VOID* param);
	static DWORD WINAPI runThreadIn_( VOID* param);
	DWORD sendThread();

protected:
	enum{ FULL_INFO = 1, SHORT_INFO, NO_INFO};	
	int checkMDumpExisted( BOOL bLastIndex = FALSE);
	BOOL sendShortReport( TCHAR *serverName, TCHAR* query, CInfoAnalyzer *pInfoAnalyzer, CDataBuffer *buffer = NULL);
	BOOL writeRemoteResource( LPCSTR aServerName, LPCWSTR aResPath, LPCTSTR dmpFileName);

public:
	void init( TCHAR *appName, TCHAR *crashType, WCHAR *root);	
	void setServiceWorldManager( service::IServiceWorldManager *pServiceManager);
	BOOL sendAsynch( CWndError *pWndError);
	BOOL sendData( int infoType, int iCrashType, BOOL bInfoShow = FALSE);
	BOOL sendDump( int iCrashType, BOOL bInternal);
	BOOL runOnce( int dumpIndex, BOOL bInfoShow = FALSE);
	BOOL run( bool bInApp);	
	BOOL runAsynch( service::IServiceWorldManager *pServiceManager);
	BOOL runAsynchIn( service::IServiceWorldManager *pServiceManager);
	BOOL sendShortInfo( int iCrashType);
	BOOL sendRemoteResource( int iCrashType);	
	DWORD getStatusCode();
	void setModRoot( WCHAR *aModRoot);
	BOOL defineAppParams();
	int getLanguage();
	BOOL isEnding();
	void setProtocol( int aProtocol);
	void terminate();

private:
	// имя упавшего приложения
	TCHAR m_appName[ MAX_PATH];
	// имя приложения, которое отправляет отчет
	TCHAR m_appReportName[ MAX_PATH];
	//
	WCHAR m_errFileName[ MAX_PATH_4];
	//
	CDataBuffer m_dataBuffer;
	//
	CDataBuffer m_dataBuffer2;
	//
	DWORD m_dwStatusCode;
	// полное имя приложения
	WCHAR m_fullAppName[ MAX_PATH_4];
	WCHAR m_modRoot[ MAX_PATH_4];

	TCHAR m_serverName[ MAX_PATH];
	TCHAR m_appVersion[ 32];
	TCHAR m_language[ 32];
	int m_iLanguage;
	HANDLE m_hSendThread;	
	TCHAR m_crashType[ 32];
	int m_iCrashType;
	BOOL m_bStartSending;
	CWndError *m_pWndError;
	int m_iInfoType;
	int m_iQueryType;
	IReportBase *m_pHTTPReport;
	IReportBase *m_pSocketReport;
	TZipData m_zipDumpData;
	TZipData m_zipLogData;
	BOOL bTerminate;

	struct TFileName
	{
		WCHAR s[ MAX_PATH_4];
	};

	TFileName fileNameForDelete[ 4];
};

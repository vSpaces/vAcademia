#pragma once
#include "ReportBase.h"
#include "ServiceMan.h"
#include "Mutex.h"

class service::IServiceWorldManager;

class CSocketReport : public IReportBase
{
public:
	CSocketReport();
	virtual ~CSocketReport();

public:
	BOOL sendData(TCHAR *server, TCHAR* query, TCHAR *sData, int len, CDataBuffer *pDataBuffer = NULL);
	BOOL sendFileData(TCHAR *server, TCHAR* query, WCHAR *fullPathSendFile, BYTE *buffer, int len, TCHAR *fileNameToServer,
					DWORD *aDataSize, BYTE **aData);
	BOOL sendBufferData(TCHAR *server, BYTE *buffer, int len);
	DWORD getStatusCode();
	void setServiceWorldManager( service::IServiceWorldManager *pServiceManager);

private:
	DWORD m_dwStatusCode;
	service::IServiceWorldManager *m_pServiceManager;
	CMutex mutex;
};

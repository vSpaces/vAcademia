#pragma once

#include "ReportBase.h"

class CHTTPReport : public IReportBase
{
public:
	CHTTPReport();
	virtual ~CHTTPReport();

public:
	BOOL sendData(TCHAR *server, TCHAR* query, TCHAR *sData, int len, CDataBuffer *pDataBuffer = NULL);
	BOOL sendFileData(TCHAR *server, TCHAR* query, WCHAR *fullPathSendFile, BYTE *buffer, int len, TCHAR *fileNameToServer,
						DWORD *aDataSize, BYTE **aData);
	BOOL sendBufferData(TCHAR *server, BYTE *buffer, int len);
	DWORD getStatusCode();

private:
	//
	DWORD m_dwStatusCode;
};

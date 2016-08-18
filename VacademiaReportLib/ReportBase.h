#pragma once

class CDataBuffer;

class IReportBase
{
public:
	virtual BOOL sendData(TCHAR *server, TCHAR* query, TCHAR *sData, int len, CDataBuffer *pDataBuffer = NULL) = 0;
	virtual BOOL sendFileData(TCHAR *server, TCHAR* query, WCHAR *fullPathSendFile, BYTE *buffer, int len, TCHAR *fileNameToServer,
							DWORD *aDataSize, BYTE **aData) = 0;
	virtual BOOL sendBufferData(TCHAR *server, BYTE *buffer, int len) = 0;
	virtual DWORD getStatusCode() = 0;
};
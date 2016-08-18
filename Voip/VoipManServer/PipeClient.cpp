#include "stdafx.h"
#include "PipeClient.h"

CPipeClient::CPipeClient()
{
	m_hPipe = NULL;
	m_context = NULL;
}

void CPipeClient::SetPipeName( char *aPipeName)
{
	m_pipeName = aPipeName;	
	//m_pipeName = TEXT("\\\\.\\pipe\\voipmanpipe");
}

int CPipeClient::TryConnect()
{
	if(IsConnected())
		Disconnect();
	m_hPipe = CreateFile(m_pipeName.GetBuffer(), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if( m_hPipe == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}
	int res = !IsConnected();
	if( res == 1)
		return -1;
	return 0;
}

bool CPipeClient::IsConnected()
{
	if (m_hPipe == NULL || m_hPipe == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	return true;
}

void CPipeClient::Disconnect()
{
	if (::CloseHandle(m_hPipe) != 0)
	{
		m_hPipe = NULL;
	}
}

CDataStorage* CPipeClient::ReadData( bool &bDisconnected)
{
	DWORD bytesReaded;
	BOOL successFlag;
	bDisconnected = false;
	unsigned int dataSize1;
	unsigned int dataSize2;
	// чтение размера данных
	for (int i=0; i < 2; i++) 
	{
		MP_NEW_P(sizeData, CDataStorage, sizeof(unsigned int));
		successFlag = ReadFile(m_hPipe, sizeData->GetData(), sizeof(unsigned int), &bytesReaded, NULL);
		if (!successFlag || (bytesReaded != sizeof(unsigned int)))
		{
			int code = GetLastError();
			if( code == ERROR_BROKEN_PIPE || code == ERROR_INVALID_HANDLE)
				bDisconnected = true;
			MP_DELETE(sizeData);
			return NULL;
		}

		if (i == 0)
		{
			sizeData->Read(dataSize1);
		}
		else if (i == 1)
		{
			sizeData->Read(dataSize2);
		}
		MP_DELETE(sizeData);
		sizeData = NULL;
	}

	if (dataSize1 != dataSize2)
	{
		if( m_context && m_context->mpLogWriter != NULL)
			m_context->mpLogWriter->WriteLn( "[VOIP_PIPE] FAILED ReadData dataSize1 != dataSize2  ");
		bDisconnected = true;
		return NULL;
	}

	CDataStorage* data = NULL;
	unsigned int dataSize = dataSize1;

	// чтение данных
	MP_NEW_V(data, CDataStorage, dataSize);
	successFlag = ReadFile(m_hPipe, data->GetData(), dataSize*sizeof(byte), &bytesReaded, NULL);
	if (!successFlag || (bytesReaded != dataSize))
	{
		int code = GetLastError();
		if( code == ERROR_BROKEN_PIPE || code == ERROR_INVALID_HANDLE)
			bDisconnected = true;

		MP_DELETE(data);
		return NULL;
	}


	return data;
}

int CPipeClient::ReadCount( bool &bDisConnected)
{
	DWORD bytesReaded;
	BOOL successFlag;

	// чтение размера данных
	MP_NEW_P(sizeData, CDataStorage, sizeof(unsigned int));
	successFlag = ReadFile(m_hPipe, sizeData->GetData(), sizeof(unsigned int), &bytesReaded, NULL);
	if (!successFlag || (bytesReaded != sizeof(unsigned int)))
	{
		int code = GetLastError();
		if( code == ERROR_BROKEN_PIPE || code == ERROR_INVALID_HANDLE)
			bDisConnected = true;
		MP_DELETE(sizeData);
		return NULL;
	}

	CDataStorage* data = NULL;
	unsigned int dataSize;
	if (sizeData->Read(dataSize))
	{
		return dataSize;

	}
	MP_DELETE(sizeData);
	return 0;
}

void CPipeClient::SetContext(oms::omsContext* aContext)
{
	m_context = aContext;
}


void CPipeClient::WriteData(std::vector<CDataStorage*>& _data, bool &bDisConnected)
{
	unsigned int count = _data.size();

	DWORD bytesWrited = 0;
	BOOL successFlag = true;

	successFlag = WriteFile(m_hPipe, &count, sizeof(unsigned int), &bytesWrited, NULL);
	if (!successFlag || bytesWrited != sizeof(unsigned int))
	{
		int code = GetLastError();
		if( m_context && m_context->mpLogWriter != NULL)
			m_context->mpLogWriter->WriteLn( "[VOIP_PIPE]failed  WriteFile cnt ", code);
		if( code == ERROR_BROKEN_PIPE || code == ERROR_INVALID_HANDLE)
			bDisConnected = true;
		std::vector<CDataStorage*>::iterator it = _data.begin();
		std::vector<CDataStorage*>::iterator itEnd = _data.end();
		for ( ; it != itEnd; it++)
		{
			CDataStorage* data = *it;
			MP_DELETE( data);			
		}
		return;
	}

	std::vector<CDataStorage*>::iterator it = _data.begin();
	std::vector<CDataStorage*>::iterator itEnd = _data.end();

	for ( ; it != itEnd; it++)
	{
		CDataStorage* data = *it;
		WriteData(data, bDisConnected);
	}
}

void CPipeClient::WriteData(CDataStorage* data, bool &bDisConnected)
{
	DWORD bytesWrited;
	BOOL successFlag;

	// запись размера данных
	unsigned int dataSize = data->GetRealDataSize();
	for (int i=0; i<2; i++ )
	{
		successFlag = WriteFile(m_hPipe, &dataSize, sizeof(unsigned int), &bytesWrited, NULL);
		if (!successFlag || bytesWrited != sizeof(unsigned int))
		{
			int code = GetLastError();
			if( m_context && m_context->mpLogWriter != NULL)
				m_context->mpLogWriter->WriteLn( "[VOIP_PIPE] failed WriteFile datasize code = ", code);
			if( code == ERROR_BROKEN_PIPE || code == ERROR_INVALID_HANDLE)
				bDisConnected = true;
			MP_DELETE(data);
			return;
		}
	}

	// запись данных
	successFlag = WriteFile(m_hPipe, data->GetData(), data->GetRealDataSize()*sizeof(byte), &bytesWrited, NULL);
	if (!successFlag || bytesWrited != data->GetRealDataSize()*sizeof(byte))
	{
		int code = GetLastError();
		if( m_context && m_context->mpLogWriter != NULL)
			m_context->mpLogWriter->WriteLn( "[VOIP_PIPE] failed WriteFile data ", code);
		if( code == ERROR_BROKEN_PIPE || code == ERROR_INVALID_HANDLE)
			bDisConnected = true;
	}

	MP_DELETE(data);
}

CPipeClient::~CPipeClient()
{
	if (IsConnected())
	{
		Disconnect();
	}	
}
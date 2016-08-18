#include "stdafx.h"
#include "PipeServer.h"

CPipeServer::CPipeServer():
	m_hPipe(NULL)
{
	m_pipeName = "\\\\.\\pipe\\voipmanpipe";	
	m_isConnected = false;
	m_csDestroy.Init();
	m_isConnecting = false;
	m_context = NULL;
}

int CPipeServer::CreatePipe( char *aPipeName)
{
	if( aPipeName != NULL)
		m_pipeName = aPipeName;
	m_csDestroy.Lock();
	m_hPipe = CreateNamedPipe(m_pipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 51200, 51200, 5000, NULL);
	m_csDestroy.Unlock();
	if(!IsCreated())
		return GetLastError();

	return 0;
}

int CPipeServer::WaitForConnection()
{	
	if(!IsCreated())
		return -1;

	m_isConnecting = true;
	if( !ConnectNamedPipe(m_hPipe, NULL))
	{
		m_isConnecting = false;
		int errorCode = GetLastError();
		if( errorCode != ERROR_PIPE_CONNECTED)
		{
			DestroyPipe();
			return errorCode;
		}
	}
	m_isConnecting = false;
	m_isConnected = true;
	return 0;
}

bool CPipeServer::IsCreated()
{
	if (m_hPipe == NULL || m_hPipe == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	return true;
}

bool CPipeServer::IsConnected()
{
	return m_isConnected;
}

int CPipeServer::DestroyPipe()
{
	m_isConnected = false;
	m_csDestroy.Lock();
	if (m_hPipe == NULL)
	{
		m_csDestroy.Unlock();
		return 0;
	}
	DeleteFile(m_pipeName);//прерываем синхронные операции с пайпом
	CloseHandle(m_hPipe);
	m_hPipe = NULL;
	m_csDestroy.Unlock();
	return GetLastError();
}

void CPipeServer::SetContext(oms::omsContext* aContext)
{
	m_context = aContext;
}

CDataStorage* CPipeServer::ReadData( bool &bDisConnected)
{
	DWORD bytesReaded;
	BOOL successFlag;

	bDisConnected = false;
	unsigned int dataSize1;
	unsigned int dataSize2;

	for (int i=0; i<2; i++) 
	{
		// чтение размера данных
		MP_NEW_P(sizeData, CDataStorage, sizeof(unsigned int));
		successFlag = ReadFile(m_hPipe, sizeData->GetData(), sizeof(unsigned int), &bytesReaded, NULL);
		if (!successFlag || (bytesReaded != sizeof(unsigned int)))
		{
			
			int code = GetLastError();
			if( m_context && m_context->mpLogWriter != NULL)
				m_context->mpLogWriter->WriteLn( "[VOIP_PIPE] FAILED ReadData ReadFile  ", code);
			if( code == ERROR_BROKEN_PIPE || code == ERROR_INVALID_HANDLE)
				bDisConnected = true;
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
		bDisConnected = true;
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
		if( m_context && m_context->mpLogWriter != NULL)
			m_context->mpLogWriter->WriteLn( "[VOIP_PIPE] FAILED ReadData ReadFile  ", code);
		if( code == ERROR_BROKEN_PIPE || code == ERROR_INVALID_HANDLE)
			bDisConnected = true;
		//MessageBox(NULL, "Can't read command data from pipe", NULL, MB_OK);

		MP_DELETE(data);
		return NULL;
	}


	return data;
}

int CPipeServer::ReadCount( bool &bDisConnected)
{
	bDisConnected = false;

	DWORD bytesReaded;
	BOOL successFlag;

	// чтение размера данных
	MP_NEW_P(pDataStorage, CDataStorage, sizeof(unsigned int));
	successFlag = ReadFile(m_hPipe, pDataStorage->GetData(), sizeof(unsigned int), &bytesReaded, NULL);
	
	if (!successFlag || (bytesReaded != sizeof(unsigned int)))
	{
		int code = GetLastError();
		if( m_context && m_context->mpLogWriter != NULL)
			m_context->mpLogWriter->WriteLn( "[VOIP_PIPE] Failed ReadFile ReadCount ", code);
		if( code == ERROR_BROKEN_PIPE || code == ERROR_INVALID_HANDLE)
			bDisConnected = true;
		MP_DELETE(pDataStorage);
		return NULL;
	}

	unsigned int dataSize;
	if ( !pDataStorage->Read(dataSize))
	{
		dataSize = 0;
	}

	MP_DELETE(pDataStorage);
	return dataSize;
}

void CPipeServer::WriteData(std::vector<CDataStorage*>& _data, bool &bDisConnected)
{
	unsigned int count = _data.size();

	DWORD bytesWrited = 0;
	BOOL successFlag = true;

	successFlag = WriteFile(m_hPipe, &count, sizeof(unsigned int), &bytesWrited, NULL);
	if (!successFlag || bytesWrited != sizeof(unsigned int))
	{
		int code = GetLastError();
		if( m_context && m_context->mpLogWriter != NULL)
			m_context->mpLogWriter->WriteLn( "[VOIP_PIPE] Failed WriteData ", code);
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
		//MessageBox(NULL, "Can't write size of data to pipe", NULL, MB_OK);
	}

	std::vector<CDataStorage*>::iterator it = _data.begin();
	std::vector<CDataStorage*>::iterator itEnd = _data.end();

	for ( ; it != itEnd; it++)
	{
		CDataStorage* data = *it;

		// запись размера данных
		WriteData(data, bDisConnected);
	}
}

void CPipeServer::WriteData(CDataStorage* data, bool &bDisConnected)
{
	DWORD bytesWrited;
	BOOL successFlag;

	// запись размера данных
	unsigned int dataSize = data->GetRealDataSize();
	for (int i=0; i<2; i++)
	{
		successFlag = WriteFile(m_hPipe, &dataSize, sizeof(unsigned int), &bytesWrited, NULL);
		if (!successFlag || bytesWrited != sizeof(unsigned int))
		{
			int code = GetLastError();
			if( m_context && m_context->mpLogWriter != NULL)
				m_context->mpLogWriter->WriteLn( "[VOIP_PIPE] Failed WriteData ", code);
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
			m_context->mpLogWriter->WriteLn( "[VOIP_PIPE] Failed WriteData ", code);
		if( code == ERROR_BROKEN_PIPE || code == ERROR_INVALID_HANDLE)
			bDisConnected = true;
	}

	MP_DELETE(data);
}

CPipeServer::~CPipeServer()
{
	if (IsCreated())
	{
		DestroyPipe();
	}
	m_csDestroy.Term();
}
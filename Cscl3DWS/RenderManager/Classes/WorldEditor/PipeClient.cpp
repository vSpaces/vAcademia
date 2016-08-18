#include "stdafx.h"
#include "PipeClient.h"

CPipeClient::CPipeClient():
	m_hPipe(NULL)
{
	m_pipeName = TEXT("\\\\.\\pipe\\worldeditorpipe");	
}

bool CPipeClient::TryConnect()
{
	m_hPipe = CreateFile(m_pipeName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	return IsConnected();
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
	if (CloseHandle(m_hPipe) != 0)
	{
		m_hPipe = NULL;
	}
}

CDataStorage* CPipeClient::ReadData()
{
	DWORD bytesReaded;
	BOOL successFlag;

	// чтение размера данных
	MP_NEW_P(sizeData, CDataStorage, sizeof(unsigned int));
	successFlag = ReadFile(m_hPipe, sizeData->GetData(), sizeof(unsigned int), &bytesReaded, NULL);
	if (!successFlag || (bytesReaded != sizeof(unsigned int)))
	{
		//MessageBox(NULL, "Can't read size of data from pipe", NULL, MB_OK);
		MP_DELETE(sizeData);
		return NULL;
	}

	CDataStorage* data = NULL;
	unsigned int dataSize;
	if (sizeData->Read(dataSize))
	{
		// чтение данных
		MP_NEW_V(data, CDataStorage, dataSize);
		successFlag = ReadFile(m_hPipe, data->GetData(), dataSize*sizeof(byte), &bytesReaded, NULL);
		if (!successFlag || (bytesReaded != dataSize))
		{
			//MessageBox(NULL, "Can't read command data from pipe", NULL, MB_OK);
			MP_DELETE(sizeData);
			return NULL;
		}

	}
	MP_DELETE(sizeData);

	return data;
}

void CPipeClient::WriteData(std::vector<CDataStorage*>& _data)
{
	unsigned int count = _data.size();

	DWORD bytesWrited = 0;
	BOOL successFlag = true;

	successFlag = WriteFile(m_hPipe, &count, sizeof(unsigned int), &bytesWrited, NULL);
	if (!successFlag || bytesWrited != sizeof(unsigned int))
	{
		//MessageBox(NULL, "Can't write size of data to pipe", NULL, MB_OK);
	}

	std::vector<CDataStorage*>::iterator it = _data.begin();
	std::vector<CDataStorage*>::iterator itEnd = _data.end();

	for ( ; it != itEnd; it++)
	{
		CDataStorage* data = *it;

		// запись размера данных
		unsigned int dataSize = data->GetRealDataSize();
		successFlag = WriteFile(m_hPipe, &dataSize, sizeof(unsigned int), &bytesWrited, NULL);
		if (!successFlag || bytesWrited != sizeof(unsigned int))
		{
			//MessageBox(NULL, "Can't write size of data to pipe", NULL, MB_OK);
		}

		// запись данных
		successFlag = WriteFile(m_hPipe, data->GetData(), data->GetRealDataSize()*sizeof(byte), &bytesWrited, NULL);
		if (!successFlag || bytesWrited != data->GetRealDataSize()*sizeof(byte))
		{
			//MessageBox(NULL, "Can't write command data to pipe", NULL, MB_OK);
		}

		MP_DELETE(data);
	}
}

void CPipeClient::WriteData(CDataStorage* data)
{
	DWORD bytesWrited;
	BOOL successFlag;

	// запись размера данных
	unsigned int dataSize = data->GetRealDataSize();
	successFlag = WriteFile(m_hPipe, &dataSize, sizeof(unsigned int), &bytesWrited, NULL);
	if (!successFlag || bytesWrited != sizeof(unsigned int))
	{
		//MessageBox(NULL, "Can't write size of data to pipe", NULL, MB_OK);
	}

	// запись данных
	successFlag = WriteFile(m_hPipe, data->GetData(), data->GetRealDataSize()*sizeof(byte), &bytesWrited, NULL);
	if (!successFlag || bytesWrited != data->GetRealDataSize()*sizeof(byte))
	{
		//MessageBox(NULL, "Can't write command data to pipe", NULL, MB_OK);
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
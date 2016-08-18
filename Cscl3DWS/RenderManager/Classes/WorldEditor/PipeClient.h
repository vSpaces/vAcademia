#pragma once

#include "DataStorage.h"

class CPipeClient
{
public:
	CPipeClient();
	~CPipeClient();

	CDataStorage* ReadData();
	void WriteData(CDataStorage* data);
	void WriteData(std::vector<CDataStorage*>& _data);

	bool TryConnect();
	bool IsConnected();
	void Disconnect();

private:
	LPTSTR m_pipeName; 
	HANDLE m_hPipe;
};
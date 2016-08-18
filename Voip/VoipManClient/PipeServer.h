#pragma once

#include "DataStorage.h"
#include "ViskoeThread.h"

class CPipeServer
{
public:
	CPipeServer();
	~CPipeServer();

	CDataStorage* ReadData( bool &bDisconnected);
	int ReadCount( bool &bDisconnected);
	void WriteData(CDataStorage* data, bool &bDisconnected);
	void WriteData(std::vector<CDataStorage*>& _data, bool &bDisconnected);

	int CreatePipe( char *aPipeName);
	int DestroyPipe();

	int WaitForConnection();
	bool IsCreated();
	bool IsConnected();
	void SetContext(oms::omsContext* aContext);
private:
	oms::omsContext* m_context;
	CComString m_pipeName; 
	volatile bool	m_isConnected;
	volatile bool	m_isConnecting;
	HANDLE m_hPipe;
	CCriticalSection m_csDestroy;
};
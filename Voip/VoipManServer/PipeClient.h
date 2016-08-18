#pragma once

#include "DataStorage.h"

class CPipeClient
{
public:
	CPipeClient();
	~CPipeClient();

	CDataStorage* ReadData( bool &bDisconnected);
	int ReadCount( bool &bDisConnected);
	void WriteData(CDataStorage* data, bool &bDisConnected);
	void WriteData(std::vector<CDataStorage*>& _data, bool &bDisConnected);

	void SetPipeName( char *aPipeName);
	int TryConnect();
	bool IsConnected();
	void Disconnect();

	void SetContext(oms::omsContext* aContext);

private:
	oms::omsContext* m_context;
	CComString m_pipeName; 
	HANDLE m_hPipe;
};
#pragma once
#include "CommonPacketIn.h"

class RecordIn : public Protocol::CCommonPacketIn
{
public:
	RecordIn(BYTE *aData, int aDataSize);
	~RecordIn(void);

	virtual bool Analyse();

	LPCSTR GetSessionName();
	LPCSTR GetFileName();
protected:
	CComString		sessionName;
	CComString		fileName;
};

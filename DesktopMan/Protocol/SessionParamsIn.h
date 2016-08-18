#pragma once
#include "CommonPacketIn.h"

class CSessionParamsQueryIn : public Protocol::CCommonPacketIn
{
public:
	CSessionParamsQueryIn ( BYTE *aData, int aDataSize);
	virtual bool Analyse();

	static unsigned int PLAY_SESSION_TYPE;
	static unsigned int RECORD_SESSION_TYPE;
	static unsigned int REMOTEACCESS_SESSION_TYPE;

public:
	LPCSTR			GetSessionName();
	unsigned int	GetSessionType();

protected:
	CComString		sessionName;
	unsigned int	sessionType;
};

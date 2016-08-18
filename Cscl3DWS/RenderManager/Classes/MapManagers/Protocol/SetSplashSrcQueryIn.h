#pragma once

#include "../CommonRenderManagerHeader.h"

#include "CommonPacketIn.h"

/************************************************************************/
/* Ответ от главного сервера голосовой связи. Данные:
1. recordID
2. fileName
/************************************************************************/
class CSetSplashSrcQueryIn : public Protocol::CCommonPacketIn
{
public:
	CSetSplashSrcQueryIn( BYTE *aData, int aDataSize);
	virtual bool Analyse();

public:
	unsigned int	GetRecordID();
	CComString		GetFileName();

protected:
	unsigned int	recordID;
	MP_WSTRING		fileName;
};

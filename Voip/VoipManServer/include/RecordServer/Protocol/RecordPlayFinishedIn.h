#pragma once
#include "CommonPacketIn.h"

/************************************************************************/
/* Ответ от сервера записи голосовой связи. Данные:
1. RecordUD
/************************************************************************/
class CRecordPlayFinishedIn: public Protocol::CCommonPacketIn
{
public:
	CRecordPlayFinishedIn( BYTE *aData, int aDataSize);
	virtual bool Analyse();

public:
	unsigned int	GetRecordID();

protected:
	unsigned int	recordID;
};

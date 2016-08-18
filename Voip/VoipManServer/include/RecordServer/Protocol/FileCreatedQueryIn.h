#pragma once
#include "CommonPacketIn.h"

/************************************************************************/
/* Ответ от сервера записи голосовой связи. Данные:
1. UserName
2. FileName
/************************************************************************/
class CFileCreatedQueryIn : public Protocol::CCommonPacketIn
{
public:
	CFileCreatedQueryIn( BYTE *aData, int aDataSize);
	virtual bool Analyse();

public:
	CComString		GetUserName();
	CComString		GetFileName();

protected:
	CComString		userName;
	CComString		fileName;
};

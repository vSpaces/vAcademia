#pragma once
#include "FileCreatedQueryIn.h"

/************************************************************************/
/* Ответ от сервера записи голосовой связи. Данные:
1. RecordUD
/************************************************************************/
class CAudioPlayedQueryIn : public CFileCreatedQueryIn
{
public:
	CAudioPlayedQueryIn( BYTE *aData, int aDataSize) : CFileCreatedQueryIn( aData, aDataSize) {}
};

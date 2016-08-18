#pragma once
#include "CommonPacketIn.h"

/************************************************************************/
/* Ответ от сервера голосовой связи. Изменился статус удаленного EvoVOIP клиента
/************************************************************************/
class CNotifiedQueryIn : public Protocol::CCommonPacketIn
{
public:
	CNotifiedQueryIn( BYTE *aData, int aDataSize);
	virtual bool Analyse();

public:
	//! Возвращает код нотификации
	byte			GetNotificationCode();

	//! Возвращает токен для данной сессии
	unsigned int GetSessionToken();


protected:
	//! Код нотификации
	byte	notificationCode;
	//! Токен сессии
	unsigned int	sessionToken;
};

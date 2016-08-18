#include "stdafx.h"
#include "../../../include/RecordServer/Protocol/NotifiedQueryIn.h"

CNotifiedQueryIn::CNotifiedQueryIn(BYTE *aData, int aDataSize) : CCommonPacketIn(aData, aDataSize)
{
	notificationCode = 0;
}

bool CNotifiedQueryIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	if ( !memparse_object( pData, idx, notificationCode))
		return false;

	if ( !memparse_object( pData, idx, sessionToken))
		return false;

	return EndOfData( idx);
}

//! Возвращает код нотификации
byte	CNotifiedQueryIn::GetNotificationCode()
{
	return notificationCode;
}

//! Возвращает токен для данной сессии
unsigned int CNotifiedQueryIn::GetSessionToken()
{
	return sessionToken;
}
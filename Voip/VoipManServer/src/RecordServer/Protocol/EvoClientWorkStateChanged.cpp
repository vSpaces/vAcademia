#include "stdafx.h"
#include "../../../include/RecordServer/Protocol/EvoClientWorkStateChangedQueryIn.h"


CEvoClientWorkStateChangedQueryIn::CEvoClientWorkStateChangedQueryIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize)
			, realityId(-1)
			, recordId(-1)
			, errorCode(-1)
			, workState(ECWS_NOT_LAUNCHED)
{
}

bool CEvoClientWorkStateChangedQueryIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	// тип сессии
	unsigned int auSessionType = 0;
	if ( memparse_object( pData, idx, auSessionType))
		sessionType = (voip::voipEvoClientSessionType)auSessionType;
	else
		return false;

	// реальность
	if ( !memparse_object( pData, idx, realityId))
		return false;

	// запись
	if ( !memparse_object( pData, idx, recordId))
		return false;

	// статус работы
	unsigned int auState = 0;
	if ( memparse_object( pData, idx, auState))
		workState = (voip::voipEvoClientWorkState)auState;
	else
		return false;

	// код ошибки
	if ( !memparse_object( pData, idx, errorCode))
		return false;

	return EndOfData( idx);
}

unsigned int	CEvoClientWorkStateChangedQueryIn::GetRealityID()
{
	return realityId;
}

unsigned int	CEvoClientWorkStateChangedQueryIn::GetRecordID()
{
	return recordId;
}

voip::voipEvoClientWorkState	CEvoClientWorkStateChangedQueryIn::GetWorkState()
{
	return workState;
}

unsigned int	CEvoClientWorkStateChangedQueryIn::GetErrorCode()
{
	return errorCode;
}

voip::voipEvoClientSessionType	CEvoClientWorkStateChangedQueryIn::GetSessionType()
{
	return sessionType;
}
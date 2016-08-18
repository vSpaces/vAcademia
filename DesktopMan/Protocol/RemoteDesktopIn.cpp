#include "StdAfx.h"
#include ".\remotedesktopin.h"

CRemoteDesktopIn::CRemoteDesktopIn(BYTE *aData, int aDataSize) : 
CCommonPacketIn(aData, aDataSize)
{
}

bool CRemoteDesktopIn::Analyse()
{
	int idx = 0;
	if ( !CCommonPacketIn::Analyse())
		return false;
	if ( !memparse_object( pData, idx, auType))
		return false;
	if ( !memparse_object( pData, idx, auFirstParam))
		return false;
	if ( !memparse_object( pData, idx, auSecondParam))
		return false;
	if ( !memparse_object( pData, idx, auCode))
		return false;
	return EndOfData( idx);
}

unsigned int CRemoteDesktopIn::GetType()
{
	return auType;
}
unsigned int CRemoteDesktopIn::GetFirstParam()
{
	return auFirstParam;
}
unsigned int CRemoteDesktopIn::GetSecondParam()
{
	return auSecondParam;
}
unsigned int CRemoteDesktopIn::GetCode()
{
	return auCode;
}
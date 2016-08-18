#include "stdafx.h"
#include "SharingServersQueryIn.h"


CSharingServersQueryIn::CSharingServersQueryIn(BYTE *aData, int aDataSize) : 
	CCommonPacketIn(aData, aDataSize),
	MP_WSTRING_INIT(serverIP),
	MP_WSTRING_INIT(groupName)
{
}

bool CSharingServersQueryIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;
	// 1. SharingServer - [4 bytes - адрес Preview сервера] | [UnicodeString]
	if (!memparse_string_unicode( pData, idx, serverIP, true))
		return false;

	// 2. TcpPort - 2 bytes - порт
	if ( !memparse_object( pData, idx, tcpPort))
		return false;

	// 3. GroupName - [4 bytes - название группы] | [UnicodeString]
	if (!memparse_string_unicode( pData, idx, groupName, true))
		return false;

	if( EndOfData( idx))
		return true;
	// 4. recordID 4 bytes
	if ( !memparse_object( pData, idx, recordID))
		return false;

	return EndOfData( idx);
}

std::wstring	CSharingServersQueryIn::GetSharingServerIP()
{
	return serverIP;
}

WORD	CSharingServersQueryIn::GetTcpPort()
{
	return tcpPort;
}

std::wstring	CSharingServersQueryIn::GetGroupName()
{
	return groupName;
}

unsigned int	CSharingServersQueryIn::GetRecordID()
{
	return recordID;
}
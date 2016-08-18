#include "stdafx.h"
#include "PreviewServersQueryIn.h"


CPreviewServersQueryIn::CPreviewServersQueryIn(BYTE *aData, int aDataSize) : 
		CCommonPacketIn(aData, aDataSize)
{
}

bool CPreviewServersQueryIn::Analyse()
{
	if ( !CCommonPacketIn::Analyse())
		return false;

	int idx = 0;

	byte nameType = 0;

	// 0. RecordID
	if ( !memparse_object( pData, idx, recordID))
		return false;

	// 1. PreviewServer - [4 bytes - адрес Preview сервера] | [UnicodeString]
	if (!memparse_string_unicode( pData, idx, PreviewServerIP))
		return false;

	// 2. TcpPort - 2 bytes - порт
	if ( !memparse_object( pData, idx, tcpPort))
		return false;

	// 3. PreviewHttpHost - [4 bytes - название хоста Preview сервера] | [UnicodeString]
	if (!memparse_string_unicode( pData, idx, PreviewHttpHost))
		return false;

	// 4. StoragePath - [4 bytes - каталог с превьюшками записей] | [UnicodeString]
	if (!memparse_string_unicode( pData, idx, StoragePath))
		return false;

	return EndOfData( idx);
}

unsigned int	CPreviewServersQueryIn::GetRecordID()
{
	return recordID;
}

CComString	CPreviewServersQueryIn::GetPreviewServerIP()
{
	return PreviewServerIP;
}

WORD	CPreviewServersQueryIn::GetTcpPort()
{
	return tcpPort;
}

CComString	CPreviewServersQueryIn::GetPreviewHttpHost()
{
	return PreviewHttpHost;
}

CComString	CPreviewServersQueryIn::GetStoragePath()
{
	return StoragePath;
}
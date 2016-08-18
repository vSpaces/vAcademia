#include "stdafx.h"
#include "LoginIn2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CLoginIn2::CLoginIn2( BYTE *apData, int apDataSize) :
CCommonPacketIn( apData, apDataSize)
{
	lastVersion = "";
	updateParam = 0;
	downloadURL= "";
}

CLoginIn2::~CLoginIn2(){
}



bool CLoginIn2::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;

	int idx=0;
	unsigned short protocolVersion = 0;
	if(!memparse_object(pData, idx, protocolVersion))
		return false;
	if(!memparse_object(pData, idx, rmmlID))
		return false;
	bsResult = &pData[idx];
	bsResultSize = iDataSize - idx;
	unsigned int signature = 0;
	if(!memparse_object(pData, idx, signature))
		return false;
	unsigned int session = 0;
	if(!memparse_object(pData, idx, session))
		return false;
	byte format = 0;
	if(!memparse_object(pData, idx, format))
		return false;
	byte count = 0;
	if(!memparse_object(pData, idx, count))
		return false;
	idx+=count*11;
	if(!memparse_object(pData, idx, count))
		return false;
	for (int i=0; i<count; i++)
	{
		byte type = 0;
		if(!memparse_object(pData, idx, type))
			return false;
		CComString str;
		if ( !memparse_string( pData, idx, str))
			return false;
		if ( !memparse_string( pData, idx, str))
			return false;
	}
	if(!memparse_object(pData, idx, typeSendClientLog))
		return false;
	CComString login;
	if ( !memparse_string( pData, idx, login))
		return false;
	if ( !memparse_string( pData, idx, sessionKey))
		return false;
	

	if(!EndOfData( idx))
	{
		if ( !memparse_string( pData, idx, lastVersion))
			return false;
		if( !memparse_object(pData, idx, updateParam))
			return false;
		if ( protocolVersion >= 3)
		{
			if( !memparse_string( pData, idx, updaterKey))
				return false;
			if( !memparse_object(pData, idx, btTO))
				return false;
		}
	}
	/*if (!memparse_string(pData, idx, (BYTE **)&bsResult))
	return false;*/
	return EndOfData( idx);
}

const BYTE* CLoginIn2::GetMethodResult() const{
	return bsResult;
}

unsigned int CLoginIn2::GetMethodResultSize() const{
	return bsResultSize;
}


unsigned int CLoginIn2::getRmmlID() const
{
	return rmmlID;
}

CComString CLoginIn2::GetSessionKey() const
{
	return sessionKey;
}

CComString CLoginIn2::GetLastVersion() const
{
	return lastVersion;
}

int CLoginIn2::GetUpdateParam() const
{
	return updateParam;
}

CComString CLoginIn2::GetUpdaterKey() const
{
	return updaterKey;
}

int CLoginIn2::GetTOParam() const
{
	return btTO;
}

CComString CLoginIn2::GetDownloadURL() const
{
	return downloadURL;
}
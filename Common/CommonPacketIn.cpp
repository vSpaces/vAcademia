#include "stdafx.h"
#include "CommonPacketIn.h"

using namespace Protocol;

CCommonPacketIn::CCommonPacketIn(BYTE *aData, int aDataSize)
{
	iDataSize=0;
	pData=NULL;
	isAnalyseDone = false;
	AttachData(aData, aDataSize);
}

CCommonPacketIn::~CCommonPacketIn()
{
	Detach();
	ATLASSERT(iDataSize == 0);
	ATLASSERT(pData == NULL);
}

void CCommonPacketIn::AttachData( BYTE *aData, int aDataSize)
{
	if(aDataSize==0 || aData==NULL)
	{
		iDataSize=0;
		return;
	}
	pData = aData;
	iDataSize = aDataSize;
}

void CCommonPacketIn::Detach()
{
	iDataSize=0;
	pData = NULL;
}

byte*	CCommonPacketIn::GetData()
{
	return pData;
}

unsigned int	CCommonPacketIn::GetDataSize()
{
	return iDataSize;
}

bool CCommonPacketIn::EndOfData(int curSize)
{
	return (iDataSize == (unsigned int)curSize);
}

bool CCommonPacketIn::Analyse()
{
	if (isAnalyseDone)
		return false;

	if (pData == NULL)
		return false;

	return true;
}

bool Protocol::CCommonPacketIn::memparse_data( BYTE* buf, int& pos, unsigned int length, BYTE **val, bool bGetData /*= true*/ )
{
	if ((unsigned int)(pos + length) > iDataSize)
		return false;

	*val = NULL;
	if(bGetData && length != NULL)
	{
		*val = MP_NEW_ARR( BYTE, length);
		rtl_memcpy(*val, length, &buf[pos], length);	
	}

	pos += length;
	return true;
}

unsigned short CCommonPacketIn::memparse_bytes(BYTE* buf, int& pos, BYTE **val, bool bGetData)
{
	unsigned short iLen;
	if (!memparse_length( buf, pos, iLen))
		return false;

	if(bGetData)
	{
		*val = MP_NEW_ARR( BYTE, iLen);
		rtl_memcpy(*val, iLen, &buf[pos], iLen);	
	}

	pos += iLen;
	return iLen;
}

unsigned int CCommonPacketIn::memparse_bytes_large(BYTE* buf, int& pos, BYTE **val, bool bGetData)
{
	unsigned int iLen;
	if (!memparse_length( buf, pos, iLen))
		return false;

	if (bGetData)
	{
		*val = MP_NEW_ARR( BYTE, iLen);
		rtl_memcpy(*val, iLen, &buf[pos], iLen);	
	}
	pos += iLen;
	return iLen;
}

bool CCommonPacketIn::memparse_string_unicode(BYTE* buf, int& pos, CComString& val, bool bGetData)
{
	unsigned short iLen;
	if (!memparse_length( buf, pos, iLen))
		return false;
	unsigned int iNewPos = iLen;
	iNewPos *= 2;
	iNewPos += pos;
	if(iNewPos > iDataSize)
	{
		return false;
	}

	if (bGetData)
	{
		unsigned int allocSize = (iLen+1)*2;
		wchar_t* temp = MP_NEW_ARR( wchar_t, allocSize);
		if (temp == NULL)
		{
			return false;
		}
		rtl_memcpy(temp, allocSize, buf + pos, iLen*2);	temp[iLen] = 0;
		val = CComString( temp);
		MP_DELETE_ARR( temp);
	}

	pos = iNewPos;
	return true;
}

bool CCommonPacketIn::memparse_string_unicode(BYTE* buf, int& pos, std::wstring& val, bool bGetData)
{
	unsigned short iLen;
	if (!memparse_length( buf, pos, iLen))
		return false;
	unsigned int iNewPos = iLen;
	iNewPos *= 2;
	iNewPos += pos;
	if(iNewPos > iDataSize)
	{
		return false;
	}

	if (bGetData)
	{
		unsigned int allocSize = (iLen+1)*2;
		wchar_t* temp = MP_NEW_ARR( wchar_t, allocSize);
		if (temp == NULL)
		{
			return false;
		}
		rtl_memcpy(temp, allocSize, buf + pos, iLen*2);	temp[iLen] = 0;
		val = std::wstring( temp);
		MP_DELETE_ARR( temp);
	}

	pos = iNewPos;
	return true;
}

bool CCommonPacketIn::memparse_string(BYTE* buf, int& pos, std::string& val, bool bGetData)
{
	unsigned short iLen;
	if (!memparse_length( buf, pos, iLen))
		return false;

	if (bGetData)
		val.assign( (char*)(buf + pos), iLen);

	pos += iLen;
	return true;
}

bool CCommonPacketIn::memparse_string(BYTE* buf, int& pos, std::wstring& val, bool bGetData)
{
	unsigned short iLen;
	if (!memparse_length( buf, pos, iLen, 2))
		return false;

	if (bGetData)
	{
//		val.resize(iLen+1);
		val.assign( (wchar_t*)(buf + pos), iLen);
	}

	pos += iLen * 2;
	return true;
}

bool CCommonPacketIn::memparse_string(BYTE* buf, int& pos, CComString& val, bool bGetData)
{
	unsigned short iLen;
	if (!memparse_length( buf, pos, iLen))
		return false;
	if (bGetData)
		rtl_memcpy(val.GetBufferSetLength(iLen), iLen+1, &buf[pos], iLen);

	pos += iLen;
	return true;
}

bool CCommonPacketIn::memparse_string_ascii(BYTE* buf, int& pos, unsigned short& aLength, char*& aStringData)
{
	if (!memparse_length(buf, pos, aLength))
		return false;

	if ((unsigned int)(pos + aLength) > iDataSize)
		return false;

	aStringData = (char*)(buf + pos);
	pos += aLength;
	return true;
}

bool CCommonPacketIn::memparse_string_unicode(BYTE* buf, int& pos, unsigned short& aLength, wchar_t*& aStringData)
{
	if (!memparse_length( buf, pos, aLength))
		return false;

	if ((unsigned int)(pos + aLength * 2) > iDataSize)
		return false;

	aStringData = (wchar_t*)(buf + pos);
	pos += aLength * 2;
	return true;
}

bool Protocol::CCommonPacketIn::memparse_object( BYTE* buf, int& pos, CByteVector& val, bool bGetData /*= true*/ )
{
	unsigned short iLen;
	if (!memparse_length( buf, pos, iLen))
		return false;

	if(bGetData)
		val.assign( buf + pos, buf + pos + iLen);

	pos += iLen;
	return true;
}

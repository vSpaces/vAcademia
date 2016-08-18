#pragma once

#include "CommonPacketIn.h"

namespace Protocol
{
	class CCommonPacketInForMapManager : public CCommonPacketIn
	{
	public:
		CCommonPacketInForMapManager(BYTE *aData, int aDataSize) :
		  CCommonPacketIn(aData, aDataSize)
		{
		};
		bool memparse_stringLarge(BYTE* buf, int& pos, CComString& val, bool bGetData = true)
		{
			unsigned int iLen;
			memparse_object( buf, pos, iLen);
			if(pos+iLen>iDataSize){
				return false;
			}
			if(bGetData)
				memcpy( val.GetBufferSetLength(iLen), &buf[pos], iLen);
			pos += iLen;
			return true;
		}
		bool memparse_unicodeStringLarge(BYTE* buf, int& pos, CWComString& val, bool bGetData = true)
		{
			unsigned int iLen;
			memparse_object( buf, pos, iLen);
			if(pos+iLen*sizeof(wchar_t)>iDataSize){
				return false;
			}
			if(bGetData)
				rtl_memcpy( val.GetBufferSetLength(iLen), (iLen+1)*sizeof(wchar_t), &buf[pos], iLen*sizeof(wchar_t));
			pos += iLen*sizeof(wchar_t);
			return true;
		}
		bool memparse_stringLarge(BYTE* buf, int& pos, std::string& val, bool bGetData = true)
		{
			unsigned int iLen;
			memparse_object( buf, pos, iLen);
			if(pos+iLen>iDataSize){
				return false;
			}
			if(bGetData)
			{
				val.resize(iLen);
				memcpy( (void*)val.c_str(), &buf[pos], iLen);
			}
			pos += iLen;
			return true;
		}
	};
};
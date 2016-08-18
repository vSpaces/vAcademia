#pragma once

#include <string>
#include "UtilTypes.h"

namespace Protocol
{
	class CCommonPacketIn
	{
	protected:
		bool isAnalyseDone;
		BYTE *pData;
		unsigned int iDataSize;

		void AttachData( BYTE *aData, int aDataSize);
		void Detach();

	public:
		CCommonPacketIn(BYTE *aData, int aDataSize);
		virtual ~CCommonPacketIn();
		bool EndOfData(int curSize);
		template<class T>
		__forceinline bool memparse_object(BYTE* buf, int& pos, T& val, bool bGetData = true)
		{
			int sizeofVal = sizeof( T);
			if ((unsigned int)(pos + sizeofVal) > iDataSize)
				return false;
			if(bGetData)
				rtl_memcpy( &val, sizeof(T), &buf[pos], sizeofVal);	
			pos += sizeofVal;
			return true;
		}
		bool memparse_object(BYTE* buf, int& pos, CByteVector& val, bool bGetData = true);

		template<class T>
		__forceinline bool memparse_length( BYTE* buf, int& pos, T& aLength, int anItemSize = 1)
		{
			if (!memparse_object( buf, pos, aLength))
				return false;

			if ((unsigned int)(pos + aLength * anItemSize) > iDataSize)
				return false;
			return true;
		}

		bool memparse_data(BYTE* buf, int& pos, unsigned int length, BYTE **val, bool bGetData = true);
		unsigned short memparse_bytes(BYTE* buf, int& pos, BYTE **val, bool bGetData = true);
		unsigned int memparse_bytes_large(BYTE* buf, int& pos, BYTE **val, bool bGetData = true);
		//bool memparse_string(BYTE* buf, int& pos, unsigned char** val, bool bGetData = true);
		//bool memparse_string(BYTE* buf, int& pos, wchar_t** val, bool bGetData = true);
		bool memparse_string(BYTE* buf, int& pos, std::string& val, bool bGetData = true);
		bool memparse_string(BYTE* buf, int& pos, std::wstring& val, bool bGetData = true);
		bool memparse_string(BYTE* buf, int& pos, CComString& val, bool bGetData = true);
		bool memparse_string_ascii(BYTE* buf, int& pos, unsigned short& aLength, char*& aStringData);
		bool memparse_string_unicode(BYTE* buf, int& pos, unsigned short& aLength, wchar_t*& aStringData);
		bool memparse_string_unicode(BYTE* buf, int& pos, CComString& val, bool bGetData = true);
		bool memparse_string_unicode(BYTE* buf, int& pos, std::wstring& val, bool bGetData);
		virtual bool Analyse();
		byte*	GetData();
		unsigned int	GetDataSize();
	};
};
#pragma once

#include "DataBuffer.h"

namespace Protocol{

	class CCommonPacketOut
	{
	protected: 
		CDataBuffer data;

	public:
		inline CDataBuffer& GetDataBuffer() { return data;}; 
		inline unsigned char* GetData() { return (BYTE*) data;}; 
		inline unsigned long GetDataSize() {return data.getLength(); }

		void MB2WC(const char* apch, std::wstring& ret)
		{
			size_t iSLen = MultiByteToWideChar( CP_ACP, NULL, apch,-1, NULL, 0);
			if( iSLen > 0)
			{
				wchar_t* wsAttrValue= MP_NEW_ARR( wchar_t, iSLen+1);
				MultiByteToWideChar( CP_ACP, NULL, apch,-1, wsAttrValue, iSLen);
				wsAttrValue[iSLen] = 0;
				ret = wsAttrValue;
				MP_DELETE_ARR(wsAttrValue);
			}
			else
			{
				iSLen = strlen(apch);
				wchar_t* wsAttrValue=MP_NEW_ARR( wchar_t, iSLen+1);
				rtl_mbstowcs(wsAttrValue, iSLen+1, apch, iSLen);
				wsAttrValue[iSLen] = 0;
				ret = wsAttrValue;
				MP_DELETE_ARR(wsAttrValue);
			}
		}
	};
};


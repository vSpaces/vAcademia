#pragma once

#include "CommonPacketOut.h"

using namespace Protocol;

namespace ServiceManProtocol
{
	class CLoginQueryOut2 : public CCommonPacketOut
	{
	public:
		CLoginQueryOut2(int requestID, const wchar_t *aLogin, const wchar_t *aPassword, const wchar_t *apwcEncodingPassword, const wchar_t *apwcComputerJeyCode, int source, bool isAnonymous, const wchar_t* apwcLanguage, const wchar_t *IP, const wchar_t* apwcAuthKey, unsigned char* pComputerInfo, unsigned int pComputerInfoSize, const wchar_t *apwcVersion, unsigned char auProxiesMask, const wchar_t* apwcUpdaterKey, const wchar_t *apwcBuildType);
	};
};
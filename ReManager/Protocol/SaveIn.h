#pragma once

#include <windows.h>
#include <winnt.h>
#include <windef.h>
#include <atlbase.h>
#include <atltime.h>

#include "CommonPacketIn.h"

using namespace Protocol;

class CSaveIn :
	public CCommonPacketIn
{
public:
	CSaveIn( BYTE *apData, int apDataSize);
	~CSaveIn(void);
	bool Analyse();

	std::wstring GetAuthor();
	std::wstring GetCreationTime();
	std::wstring GetName();
	std::wstring GetDescription();

private:
	std::wstring author;
	std::wstring creationTime;
	std::wstring name;
	std::wstring description;

	BYTE* m_pData;
	int m_DataSize;

};

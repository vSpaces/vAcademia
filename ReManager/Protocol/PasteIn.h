#pragma once

#include <windows.h>
#include <winnt.h>
#include <windef.h>
#include <atlbase.h>
#include <atltime.h>

#include "CommonPacketIn.h"

using namespace Protocol;

class CPasteIn :
	public CCommonPacketIn
{
public:
	CPasteIn( BYTE *apData, int apDataSize);
	~CPasteIn(void);
	bool Analyse();

	double GetBeginPos();
	double GetEndPos();
	double GetPastePos();
	bool IsCopy();

private:
	double m_beginPos;
	double m_endPos;
	double m_pastePos;
	bool m_isCopy;

	BYTE* m_pData;
	int m_DataSize;

};

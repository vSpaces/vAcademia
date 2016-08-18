#include "stdafx.h"
#include "RecordEditorSaveAsOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordEditorSaveAsOut::CRecordEditorSaveAsOut(const wchar_t* apwcName, const wchar_t* apwcDescription, bool abOverwrite, bool abPublicate)
{
	data.clear();

	unsigned short size = wcslen(apwcName);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)apwcName);

	size = wcslen(apwcDescription);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)apwcDescription);

	data.add(abOverwrite);
	data.add(abPublicate);
}


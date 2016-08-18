#include "stdafx.h"
#include "SaveOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CSaveOut::CSaveOut(  const wchar_t* apwcAuthor, const wchar_t* apwcCreationTime, const wchar_t* apwcName, const wchar_t* apwcDescription)
{
	data.clear();

	unsigned short size = wcslen(apwcAuthor);
	data.add(size);
	data.addData(size*2, (BYTE*)apwcAuthor);

	size = wcslen(apwcCreationTime);
	data.add(size);
	data.addData(size*2, (BYTE*)apwcCreationTime);

	size = wcslen(apwcName);
	data.add(size);
	data.addData(size*2, (BYTE*)apwcName);

	size = wcslen(apwcDescription);
	data.add(size);
	data.addData(size*2, (BYTE*)apwcDescription);
}


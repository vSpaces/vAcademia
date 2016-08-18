#include "stdafx.h"
#include "StartRecordOut.h"

using namespace SyncManProtocol;

#include <string>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CStartRecordOut::CStartRecordOut(){
}

CStartRecordOut::CStartRecordOut(int aiRecordMode, int aiParentRealityID, const wchar_t*  aLocationName, const wchar_t *apwcRecordName, const wchar_t *apwcLanguage)
{
	data.clear();

	data.add( aiRecordMode);
	data.add( aiParentRealityID);

	unsigned short size = wcslen(aLocationName);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aLocationName);

	size = wcslen(apwcRecordName);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)apwcRecordName);

	size = wcslen(apwcLanguage);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)apwcLanguage);
}


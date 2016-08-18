#include "stdafx.h"
#include "EventUserRightsUpdateOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CEventUserRightsUpdateOut::CEventUserRightsUpdateOut(unsigned int auEventID, wchar_t *auCapsDescr)
{
	data.clear();
	data.addData(sizeof(unsigned int), (BYTE*)&auEventID);
	int iLen = wcslen(auCapsDescr);
	if(iLen<1)
		return;
	data.addStringUnicode(iLen, (BYTE*)auCapsDescr);
}



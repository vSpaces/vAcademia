#include "stdafx.h"
#include "EventUserBanUpdateOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CEventUserBanUpdateOut::CEventUserBanUpdateOut(unsigned int auEventID, wchar_t *apwcDescr){
	data.clear();
	data.addData(sizeof(unsigned int), (BYTE*)&auEventID);
	int iLen = wcslen(apwcDescr);
	if(iLen<1)
		return;
	data.addStringUnicode(iLen, (BYTE*)apwcDescr);
}



#include "stdafx.h"
#include "JoinToSeanceOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CJoinToSeanceOut::CJoinToSeanceOut(){
}

CJoinToSeanceOut::CJoinToSeanceOut(unsigned int auLogicalID, unsigned int auID,  const wchar_t *aUser){
	data.clear();
	data.addData(sizeof(unsigned int), (BYTE*)&auLogicalID);
	data.addData(sizeof(unsigned int), (BYTE*)&auID);
	unsigned short size = wcslen(aUser);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aUser);
}


#include "stdafx.h"
#include "EventGetUserRightOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CEventGetUserRightOut::CEventGetUserRightOut(unsigned int auEventID){
	data.clear();
	data.addData(sizeof(unsigned int), (BYTE*)&auEventID);	
}



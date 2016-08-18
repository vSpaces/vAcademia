#include "stdafx.h"
#include "GetSeancesOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CGetSeancesOut::CGetSeancesOut(){
}

CGetSeancesOut::CGetSeancesOut(unsigned int auID, unsigned int auIDSeancesObject){
	data.clear();
	data.addData(sizeof(unsigned int), (BYTE*)&auID);
	data.addData(sizeof(unsigned int), (BYTE*)&auIDSeancesObject);
}



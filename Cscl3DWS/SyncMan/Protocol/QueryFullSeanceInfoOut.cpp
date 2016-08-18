#include "stdafx.h"
#include "QueryFullSeanceInfoOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CQueryFullSeanceInfoOut::CQueryFullSeanceInfoOut(unsigned int auIDSeanceRmmlObject, unsigned int auLogRecordID){
	data.clear();
	data.addData(sizeof(unsigned int), (BYTE*)&auIDSeanceRmmlObject);
	data.addData(sizeof(unsigned int), (BYTE*)&auLogRecordID);
}


#include "stdafx.h"
#include "UnJoinToSeanceOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CUnJoinToSeanceOut::CUnJoinToSeanceOut(){
}

CUnJoinToSeanceOut::CUnJoinToSeanceOut(unsigned int auID){
	data.clear();
	data.addData(sizeof(unsigned int), (BYTE*)&auID);
}


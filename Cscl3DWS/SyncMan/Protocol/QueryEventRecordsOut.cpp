#include "stdafx.h"
#include "QueryEventRecordsOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "SyncMan.h"

using namespace SyncManProtocol;

CQueryEventRecordsOut::CQueryEventRecordsOut(unsigned int auID, int aiBegin, int aiCount, const wchar_t* apwcFilter, const wchar_t* apwcOrderBy, sync::syncIRecordManagerCallback2 *pCallback){
	data.clear();
	data.addData(sizeof(sync::syncIRecordManagerCallback2 *), (BYTE*)&pCallback);
	data.addData(sizeof( auID), (BYTE*)&auID);
	data.addData(sizeof( aiBegin), (BYTE*)&aiBegin);
	data.addData(sizeof( aiCount), (BYTE*)&aiCount);

	if(apwcFilter!=NULL){
		int size = wcslen(apwcFilter);
		data.addData(sizeof(unsigned short), (BYTE*)&size);
		data.addData(size*2, (BYTE*)apwcFilter);
	}
	if(apwcOrderBy!=NULL){
		int size = wcslen(apwcOrderBy);
		data.addData(sizeof(unsigned short), (BYTE*)&size);
		data.addData(size*2, (BYTE*)apwcOrderBy);
	}
}


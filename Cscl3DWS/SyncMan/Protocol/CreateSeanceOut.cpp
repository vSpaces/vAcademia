#include "stdafx.h"
#include "CreateSeanceOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;


CCreateSeanceOut::CCreateSeanceOut(unsigned int auIDSeanceRmmlObject, unsigned int auLogRecordID,  const wchar_t *aName, const wchar_t *aMembers, unsigned long aDate ){
	data.clear();
	data.addData(sizeof(unsigned int), (BYTE*)&auIDSeanceRmmlObject);
	data.addData(sizeof(unsigned int), (BYTE*)&auLogRecordID);
	unsigned short size = wcslen(aName);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aName);
	if(aDate!=NULL){
		data.addData(sizeof(unsigned long), (BYTE*)&aDate);
	}
	if(aMembers!=NULL){
		size = wcslen(aMembers);
		data.addData(sizeof(unsigned short), (BYTE*)&size);
		data.addData(size*2, (BYTE*)aMembers);
	}
}

CCreateSeanceOut::CCreateSeanceOut(unsigned int auIDSeanceRmmlObject, const wchar_t *aURL)
{
	data.clear();
	data.add(auIDSeanceRmmlObject);
	unsigned short size = wcslen(aURL);
	data.add(size);
	data.addData(size*2, (BYTE*)aURL);
}

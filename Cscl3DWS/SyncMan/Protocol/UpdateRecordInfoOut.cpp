#include "stdafx.h"
#include "UpdateRecordInfoOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CUpdateRecordInfoOut::CUpdateRecordInfoOut(){
}

CUpdateRecordInfoOut::CUpdateRecordInfoOut(unsigned int aRecordID, const wchar_t *aName,  const wchar_t *aSubject, const wchar_t *aLesson, const wchar_t *aLevel, const wchar_t *aPreview, const wchar_t *aDescript, bool isReal, const wchar_t *aTags)
{
	data.clear();
	data.addData(sizeof(unsigned int), (BYTE*)&aRecordID);

	unsigned short size = wcslen(aName);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aName);

	if (aSubject==NULL)
		aSubject = L"none";

	size = wcslen(aSubject);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aSubject);
	

	if (aLesson==NULL)
		aLesson = L"none";
	size = wcslen(aLesson);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aLesson);

	if (aLevel==NULL)
		aLevel = L"none";
	size = wcslen(aLevel);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aLevel);

	if (aPreview==NULL)
		aPreview = L"none";
	size = wcslen(aPreview);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aPreview);

	if (aDescript==NULL)
		aDescript = L"none";
	size = wcslen(aDescript);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aDescript);

	data.add(isReal);

	if (aTags==NULL)
		aTags = L"none";
	size = wcslen(aTags);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aTags);

}


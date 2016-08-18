#include "stdafx.h"
#include "LoginQueryOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;


CLoginQueryOut::CLoginQueryOut(int requestID, const wchar_t *aLogin, const wchar_t *aPassword, int source, bool isAnonymous, const wchar_t* apwcLanguage)
{
	data.clear();

	data.add(requestID);
	//data.add(rmmlID);
	
	unsigned short size = wcslen(aLogin);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aLogin);

	size = wcslen(aPassword);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)aPassword);

	data.add(source);
	data.add(isAnonymous);

	size = wcslen(apwcLanguage);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)apwcLanguage);
}


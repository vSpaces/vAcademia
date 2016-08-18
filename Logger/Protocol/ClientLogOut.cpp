#include "stdafx.h"
#include "ClientLogOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CClientLogOut::CClientLogOut( const char* apcLogData, bool bTimeWrite){
	data.clear();

	USES_CONVERSION;
	wchar_t* wclogTimeAndData = A2W( apcLogData);
	data.addStringUnicode(wcslen(wclogTimeAndData), (BYTE*)wclogTimeAndData);
	data.add( bTimeWrite);
}
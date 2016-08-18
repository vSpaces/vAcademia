#include "stdafx.h"
#include "SetPilotLoginOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CSetPilotLogin::CSetPilotLogin( const wchar_t* alpwcPilotLogin, unsigned int auEventID)
{
	data.clear();
	data.addStringUnicode( wcslen(alpwcPilotLogin), (BYTE*)alpwcPilotLogin);
	data.add( auEventID);
}


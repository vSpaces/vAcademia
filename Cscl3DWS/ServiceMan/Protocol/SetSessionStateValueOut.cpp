#include "stdafx.h"
#include "SetSessionStateValueOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CSetSessionStateValueOut::CSetSessionStateValueOut(service::E_SESSION_STATE_TYPES auState, unsigned short auValue)
{
	data.clear();
	data.add((unsigned short)auState);
	data.add(auValue);
}
// HttpQuery.cpp: implementation of the CHttpQuery class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "comman.h"
#include "HttpQuery.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHttpQuery::CHttpQuery()
{
	hINet = anINet;
	hConnection = aConnection;
	hData = aData;
	status = qeNoError;
}

CHttpQuery::~CHttpQuery()
{
	if (hData)        InternetCloseHandle( hData);
	if (hConnection)  InternetCloseHandle( hConnection);
	if (hINet)        InternetCloseHandle( hINet);
}

CHttpQuery::getStatus()
{
	return status
}

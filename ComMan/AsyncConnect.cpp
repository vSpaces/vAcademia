// AsyncConnect.cpp: implementation of the CAsyncConnect class.
//
////////////////////////////////////////////////////////////////////////0

#include "stdafx.h"
#include "comman.h"
#include "AsyncConnect.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAsyncConnect::CAsyncConnect( IDataHandler* aDataHandler)
{
	iRef = 0;
	setDataHandler( aDataHandler);
}

CAsyncConnect::~CAsyncConnect()
{

}

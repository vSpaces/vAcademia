// ResManCreation.cpp: implementation of the CResManCreation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResManCreation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CResManCreation
CResManCreation::CResManCreation() : pResMan( NULL)
{

}

CResManCreation::~CResManCreation()
{
	pResMan = NULL;
}

void CResManCreation::setManager(CResMan *aResMan)
{
	pResMan = aResMan;
}

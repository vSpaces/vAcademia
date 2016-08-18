// LocalRepository.cpp: implementation of the CLocalRepository class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "comman.h"
#include "LocalRepository.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CLocalRepository
CLocalRepository::CLocalRepository( const wchar_t* aBase) : sBase( aBase)
{
}

CLocalRepository::~CLocalRepository()
{

}

const wchar_t* CLocalRepository::getBase()
{
	return sBase.GetBuffer();
}

BSTR CLocalRepository::getBaseBSTR()
{
	return sBase.AllocSysString();
}

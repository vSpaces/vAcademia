#include "stdafx.h"
#include "IPadSessionChangeSlideIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CIPadSessionChangeSlideIn::CIPadSessionChangeSlideIn( BYTE *apData, int apDataSize) :
	CIPadSessionBaseIn( apData, apDataSize)
{
}

CIPadSessionChangeSlideIn::~CIPadSessionChangeSlideIn()
{
}

bool CIPadSessionChangeSlideIn::Analyse()
{
	if (!CIPadSessionBaseIn::Analyse())
		return false;
	
	if(!memparse_object(pData, idx, changeSlide))
		return false;
	return EndOfData( idx);
}

unsigned int CIPadSessionChangeSlideIn::GetChangeSlide() const
{
	return changeSlide;
}
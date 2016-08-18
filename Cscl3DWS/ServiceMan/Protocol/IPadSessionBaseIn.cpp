#include "stdafx.h"
#include "IPadSessionBaseIn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;

CIPadSessionBaseIn::CIPadSessionBaseIn( BYTE *apData, int apDataSize) :
	CCommonPacketIn( apData, apDataSize)
{
}

CIPadSessionBaseIn::~CIPadSessionBaseIn()
{
}

bool CIPadSessionBaseIn::Analyse()
{
	if (!CCommonPacketIn::Analyse())
		return false;
	
	idx = 0;
	if(!memparse_object(pData, idx, packetID))
		return false;
	if(!memparse_object(pData, idx, token))
		return false;
	return true;
}

unsigned int CIPadSessionBaseIn::GetPacketID() const
{
	return packetID;
}

unsigned int CIPadSessionBaseIn::GetToken() const
{
	return token;
}
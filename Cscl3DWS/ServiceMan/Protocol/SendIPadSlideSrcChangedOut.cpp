#include "stdafx.h"
#include "SendIPadSlideSrcChangedOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ServiceManProtocol;


CSendIPadSlideSrcChangedOut::CSendIPadSlideSrcChangedOut( unsigned int aPacketID, unsigned int aToken, float aProportion, float aScaleX, float aScaleY, const wchar_t *aSrc)
{
	data.clear();
	data.add( aPacketID);
	data.add( aToken);
	data.add( aProportion);
	data.add( aScaleX);
	data.add( aScaleY);
	data.addStringUnicode( wcslen(aSrc), (BYTE*)aSrc);
}


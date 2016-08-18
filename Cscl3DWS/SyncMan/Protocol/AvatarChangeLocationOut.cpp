#include "stdafx.h"
#include "AvatarChangeLocationOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CAvatarChangeLocationOut::CAvatarChangeLocationOut(const wchar_t* apwcLocationName,  unsigned int auiObjectID, unsigned int auiBornRealityID)
{
	data.clear();

	unsigned short size = wcslen(apwcLocationName);
	data.addData(sizeof(unsigned short), (BYTE*)&size);
	data.addData(size*2, (BYTE*)apwcLocationName);

	data.add(auiObjectID);
	data.add(auiBornRealityID);
}


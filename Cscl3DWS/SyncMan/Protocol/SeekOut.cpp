#include "stdafx.h"
#include "SeekOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CSeekOut::CSeekOut( unsigned int seekPos)
{
	data.clear();
	data.add( seekPos);
}


#include "stdafx.h"
#include "RecordEditorSeekOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordEditorSeekOut::CRecordEditorSeekOut(unsigned int aSeekPos)
{
	data.clear();

	data.add(aSeekPos);
}


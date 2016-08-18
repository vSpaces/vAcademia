#include "stdafx.h"
#include "RecordEditorCopyOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordEditorCopyOut::CRecordEditorCopyOut(unsigned int aStartPos, unsigned int aEndPos)
{
	data.clear();

	data.add(aStartPos);
	data.add(aEndPos);
}


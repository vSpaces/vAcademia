#include "stdafx.h"
#include "RecordEditorDeleteOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordEditorDeleteOut::CRecordEditorDeleteOut(unsigned int aStartPos, unsigned int aEndPos)
{
	data.clear();

	data.add(aStartPos);
	data.add(aEndPos);
}


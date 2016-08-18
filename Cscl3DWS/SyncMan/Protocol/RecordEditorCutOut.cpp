#include "stdafx.h"
#include "RecordEditorCutOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordEditorCutOut::CRecordEditorCutOut(unsigned int aStartPos, unsigned int aEndPos)
{
	data.clear();

	data.add(aStartPos);
	data.add(aEndPos);
}


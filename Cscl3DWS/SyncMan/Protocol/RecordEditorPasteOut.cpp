#include "stdafx.h"
#include "RecordEditorPasteOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordEditorPasteOut::CRecordEditorPasteOut(unsigned int aPastePos)
{
	data.clear();
	data.add(aPastePos);
}


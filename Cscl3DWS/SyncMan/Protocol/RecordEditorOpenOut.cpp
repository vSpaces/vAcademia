#include "stdafx.h"
#include "RecordEditorOpenOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace SyncManProtocol;

CRecordEditorOpenOut::CRecordEditorOpenOut(unsigned int aRecordID)
{
	data.clear();

	data.add(aRecordID);
}


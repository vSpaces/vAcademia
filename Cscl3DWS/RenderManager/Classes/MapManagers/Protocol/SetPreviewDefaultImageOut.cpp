#include "stdafx.h"
#include "SetPreviewDefaultImageOut.h"

SetPreviewDefaultImageOut::SetPreviewDefaultImageOut(unsigned int aiRecordID, wchar_t *wcFileName)
{
	data.add(aiRecordID);
	data.addStringUnicode((unsigned short)wcslen(wcFileName), (BYTE*)wcFileName);
}


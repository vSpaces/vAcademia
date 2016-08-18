#include "stdafx.h"
#include "PreviewEventOut.h"

PreviewEventOut::PreviewEventOut( unsigned int aiRecordID, unsigned int aiStartTime, wchar_t *wsEvent, wchar_t *wsObjectName)
{
	data.add(aiRecordID);
	data.add(aiStartTime);
	data.addStringUnicode((unsigned short)wcslen(wsEvent), (BYTE*)wsEvent);
	data.addStringUnicode((unsigned short)wcslen(wsObjectName), (BYTE*)wsObjectName);
}


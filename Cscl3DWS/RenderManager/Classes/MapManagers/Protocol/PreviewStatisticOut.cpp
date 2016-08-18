#include "stdafx.h"
#include "PreviewStatisticOut.h"

PreviewStatisticOut::PreviewStatisticOut( unsigned int aiRecordID, unsigned int aiStartTime, wchar_t *wsStatistic)
{
	data.add(aiRecordID);
	data.add(aiStartTime);
	data.addStringUnicode((unsigned short)wcslen(wsStatistic), (BYTE*)wsStatistic);
}


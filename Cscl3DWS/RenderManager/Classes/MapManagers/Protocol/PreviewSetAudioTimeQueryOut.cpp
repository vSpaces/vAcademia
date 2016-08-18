#include "stdafx.h"
#include "PreviewSetAudioTimeQueryOut.h"

PreviewSetAudioTimeQueryOut::PreviewSetAudioTimeQueryOut(unsigned int aiRecordID, unsigned int aiStartTime, LPCSTR alpcFileName)
{
	USES_CONVERSION;
	data.add(aiRecordID);
	data.add(aiStartTime);
	wchar_t* wcFileName = A2W( alpcFileName);
	data.addStringUnicode((unsigned short)wcslen(wcFileName), (BYTE*)wcFileName);
}

PreviewSetSplashSrcQueryOut::PreviewSetSplashSrcQueryOut(unsigned int aiRecordID, LPCSTR alpcFileName)
{
	USES_CONVERSION;
	data.add(aiRecordID);
	wchar_t* wcFileName = A2W( alpcFileName);
	data.addStringUnicode((unsigned short)wcslen(wcFileName), (BYTE*)wcFileName);
}


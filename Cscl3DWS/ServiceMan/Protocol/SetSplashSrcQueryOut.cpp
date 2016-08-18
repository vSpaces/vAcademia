#include "stdafx.h"
#include "SetSplashSrcQueryOut.h"

SetSplashSrcQueryOut::SetSplashSrcQueryOut(unsigned int aiRecordID, LPCSTR alpcFileName)
{
	USES_CONVERSION;
	data.add(aiRecordID);
	wchar_t* wcFileName = A2W( alpcFileName);
	data.addStringUnicode(wcslen(wcFileName), (BYTE*)wcFileName);
}
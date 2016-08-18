#include "stdafx.h"
#include "PreviewChatMessageOut.h"

PreviewChatMessageOut::PreviewChatMessageOut( unsigned int aiRecordID, unsigned int aiStartTime, int aType, wchar_t *wsLogin, wchar_t *wsText, bool abCloudedEnabled)
{
	data.add(aiRecordID);
	data.add(aiStartTime);
	data.add(aType);
	data.addStringUnicode((unsigned short)wcslen(wsLogin), (BYTE*)wsLogin);
	data.addStringUnicode((unsigned short)wcslen(wsText), (BYTE*)wsText);
	data.add( abCloudedEnabled);
}


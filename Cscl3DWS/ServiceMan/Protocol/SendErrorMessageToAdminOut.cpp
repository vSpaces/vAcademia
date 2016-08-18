#include "stdafx.h"
#include "SendErrorMessageToAdminOut.h"

SendErrorMessageToAdminOut::SendErrorMessageToAdminOut( const wchar_t *apErrorText)
{
	data.addStringUnicode(wcslen(apErrorText), (BYTE*)apErrorText);
}
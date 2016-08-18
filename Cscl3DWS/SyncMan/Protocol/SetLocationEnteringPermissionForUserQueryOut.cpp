#include "stdafx.h"
#include "SetLocationEnteringPermissionForUserQueryOut.h"

using namespace SyncManProtocol;

CSetLocationEnteringPermissionForUserQueryOut::CSetLocationEnteringPermissionForUserQueryOut(const wchar_t* apwcUserLogin
																							 , const wchar_t* apwcLocationID
																							 , unsigned int auRealityID
																							 , bool abAllow)
{
	data.addStringUnicode(wcslen(apwcUserLogin), (BYTE*)apwcUserLogin);
	data.addStringUnicode(wcslen(apwcLocationID), (BYTE*)apwcLocationID);
	GetDataBuffer().add(auRealityID);
	GetDataBuffer().add(abAllow);
}
#include "stdafx.h"
#include "NatureSettingsIn.h"

using namespace MapManagerProtocol;

bool CNatureSettingsIn::Analyse()
{
	if(!CCommonPacketIn::Analyse())
		return false;
	if(iDataSize<2)
		return false;
	//idx = 0;
	SERVER_NATURE_SETTINGS_INFO info;
	if(!GetNatureInfo(&info, false))
		return false;
	return EndOfData( idx);
}

bool CNatureSettingsIn::GetNatureInfo(SERVER_NATURE_SETTINGS_INFO *info, bool bGetData)
{
	idx = 0;
	double sunX=0,sunY=0,sunZ=0;
	if(!memparse_object<unsigned int>(pData, idx, info->ID, bGetData))
		return false;
	if(!memparse_object<unsigned int>(pData, idx, info->Precipitation, bGetData))
		return false;
	if(!memparse_object<unsigned int>(pData, idx, info->Clouds, bGetData))
		return false;
	if(!memparse_object<double>(pData, idx, sunX, bGetData))
		return false;
	if(!memparse_object<double>(pData, idx, sunY, bGetData))
		return false;
	if(!memparse_object<double>(pData, idx, sunZ, bGetData))
		return false;
	if(!memparse_stringLarge(pData, idx, info->skySrc, bGetData))
		return false;
	if(!memparse_stringLarge(pData, idx, info->sunSrc, bGetData))
		return false;

	if(bGetData)
		info->sunPosition=CalVector((float)sunX,(float)sunY,(float)sunZ);
	return true;
}

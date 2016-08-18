#pragma once
//#include "nrmObject.h"
#include <vector.h>
#include <quaternion.h>

class CMapManager;

typedef	struct _NATURE_SETTINGS
{
	unsigned int portalID;
	CComString name;
	unsigned int isOpened;

	_NATURE_SETTINGS()
	{
	}

	void operator=(const _NATURE_SETTINGS& info)
	{
		portalID = info.portalID;
		name = info.name;
		isOpened = info.isOpened;
	}
}	NATURE_SETTINGS;

//////////////////////////////////////////////////////////////////////////
// серверные данные о природе
//////////////////////////////////////////////////////////////////////////
typedef	struct _SERVER_NATURE_SETTINGS_INFO
{
	unsigned int		ID;
	CalVector			sunPosition;
	unsigned int		Clouds;
	unsigned int		Precipitation;
	CComString			skySrc;
	CComString			sunSrc;


	_SERVER_NATURE_SETTINGS_INFO()
	{
		ID = 0xFFFFFFFF;
	}
	//////////////////////////////////////////////////////////////////////////
	void operator=(const _SERVER_NATURE_SETTINGS_INFO& info)
	{
		ID = info.ID;
		sunPosition = info.sunPosition;
		Clouds = info.Clouds;
		Precipitation = info.Precipitation;
		skySrc = "";
		sunSrc = "";
	}
}	SERVER_NATURE_SETTINGS_INFO;

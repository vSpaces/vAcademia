#include "stdafx.h"
#include "protocol.h"

#define	PROTOCOL_PREFIX	_T("vacademia://")

CProtocol::CProtocol()
{
}

CProtocol::~CProtocol()
{
}

// Разбор URL
// goworld://location/#место?параметры
bool	CProtocol::parse( LPCTSTR alpUrl)
{
	reset();
	if( !alpUrl)	return false;
	
	CComString lower(alpUrl);	lower.MakeLower();

	// goworld://location/#место?параметры
	if( lower.Left( strlen( PROTOCOL_PREFIX)) != PROTOCOL_PREFIX)
	{
		reset();
		return false;	// не найден префикс протокола (goworld://)
	}

	CComString original(alpUrl);
	original = original.Right( original.GetLength() - strlen( PROTOCOL_PREFIX));
	if( original.GetLength() == 0)	// не найдена локация (yoshkar-ola)
	{
		reset();
		return false;
	}

	bool paramsFound = false;
	// location/#место?параметры
	int locationEndIndex = original.Find('/');
	if( locationEndIndex == -1)
	{
		locationEndIndex = original.Find('?');
		if( locationEndIndex == -1)
		{
			mLocation = original;
			return true;
		}
		paramsFound = true;
	}

	if( locationEndIndex < 1)
		return false;	// пустая локация

	mLocation = original.Left( locationEndIndex);
	if( mLocation.IsEmpty())
		return false;	// пустая локация
	original = original.Right( original.GetLength() - locationEndIndex - 1);

	// #место?параметры
	// ?параметры
	// ""
	if( original.GetLength() == 0)
		return true;

	if( original[0] == '#')
	{
		int placeEndIndex = original.Find('?');
		if( placeEndIndex == -1)
		{
			if( original.GetLength() == 1)
			{
				reset();
				return false;	// не найдено название места (#место)
			}
			mPlace = original.Right( original.GetLength() - 1);
			return true;
		}
		if( placeEndIndex < 2)
			return false;	// пустое place
		paramsFound = true;
		mPlace = original.Mid(1, placeEndIndex-1);
		original = original.Right( original.GetLength() - placeEndIndex - 1);
	}

	if( original[0] == '?')
	{
		paramsFound = true;
		original = original.Right( original.GetLength() - 1);
	}

	if( !paramsFound)
	{
		if( original.GetLength() == 0)
			return true;
		return false;
	}

	CComString params = original.Right( original.GetLength());
	// Парсим параметры
	while (!params.IsEmpty())
	{
		CComString sNextParam;
		int	comIndex = params.Find('&');
		if( comIndex == -1)
		{
			sNextParam = params;
			params.Empty();
		}
		else
		{
			sNextParam = params.Left( comIndex);
			params = params.Right( params.GetLength() - comIndex - 1);
		}
		CComString	sNextParamName;
		CComString	sNextParamValue;
		if( sNextParam.Find("=") != -1)
		{
			sNextParamName = sNextParam.Left(sNextParam.Find("="));
			sNextParamValue = sNextParam.Right( sNextParam.GetLength() - sNextParamName.GetLength()-1);
		}
		else
		{
			reset();
			return false;	// найден неправильный параметр ,param,. Отсутствует знак =
		}
		sNextParamName.MakeLower();

		if ("x" == sNextParamName)
			mPosX = (float)atof( sNextParamValue);
		else if ("y" == sNextParamName)
			mPosY = (float)atof( sNextParamValue);
		else if ("z" == sNextParamName)
			mPosZ = (float)atof( sNextParamValue);
		else	if ("zoneid" == sNextParamName)
			mZoneID = rtl_atoi( sNextParamValue);
		else	if ("action" == sNextParamName)
			mAction = sNextParamValue;
		else	if ("actionParams" == sNextParamName || "action_params" == sNextParamName)
			mActionParams = sNextParamValue;
	}
	return true;
}

// Получение параметров протокола
CComString		CProtocol::get_location()
{
	return mLocation;
}

CComString		CProtocol::get_action()
{
	return mAction;
}

CComString		CProtocol::get_action_params()
{
	return mActionParams;
}

CComString		CProtocol::get_place()
{
	return mPlace;
}

float			CProtocol::get_x()
{
	return mPosX;
}

float			CProtocol::get_y()
{
	return mPosY;
}

float			CProtocol::get_z()
{
	return mPosZ;
}

unsigned long	CProtocol::get_zoneid()
{
	return mZoneID;
}

void	CProtocol::reset()
{
	mLocation.Empty();
	mPlace.Empty();
	mPosX = 0;
	mPosY = 0;
	mZoneID = 0;
}

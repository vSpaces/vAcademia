#include <winreg.h>
#pragma once

class CProtocol
{
public:
	CProtocol();
	~CProtocol();

	// Разбор URL
	bool	parse( LPCTSTR alpUrl);

	// Получение параметров протокола
	CComString		get_location();
	CComString		get_action();
	CComString		get_action_params();
	CComString		get_place();
	float			get_x();
	float			get_y();
	float			get_z();
	unsigned long	get_zoneid();

private:
	void	reset();
	CComString		mLocation;
	CComString		mPlace;
	CComString		mAction;
	CComString		mActionParams;
	float			mPosX;
	float			mPosY;
	float			mPosZ;
	unsigned long	mZoneID;
};

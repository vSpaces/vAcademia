#pragma once

//////////////////////////////////////////////////////////////////////////
// Идентификатор зоны
// - для города вычисляется как ((порядковый номер по Y + 16000)<<15) + (порядковый номер по X + 16000)
#define	INCORRECT_ZONEID	0xFFFFFFFF

#define	SYNCH_ZONE_SIZE		(10000.0f)
#define	ZONE_SIZE			(20000.0f)
#define	ZONE_SIZE_INT		(int)ZONE_SIZE
#define	MAX_VIEW_DISTANCE	(54600.0f)/*sqrt(20000^2+20000^2)*/

// - неверный номер объекта
#define	INCORRECT_OBJECTID	0xFFFFFFFF

// - номер "нулевого лода"
#define	NULL_LOD			2
// - номер минимального лода
#define	SMALLEST_LOD		1

typedef unsigned long ZoneIdentifier;

typedef	struct	_ZoneID
{
	ZoneIdentifier id;
	//////////////////////////////////////////////////////////////////////////
	// Constructors
	_ZoneID(){ id = INCORRECT_ZONEID;}
	_ZoneID(ZoneIdentifier aid){ id = aid;}

	//////////////////////////////////////////////////////////////////////////
	// Methods
	bool is_correct()
	{
		return true;//id!=INCORRECT_ZONEID?true:false; // Неоходим другой критерий (BDima 01.04.2008)
	}

	// получить координаты центра зоны
	static void get_zone_center_from_xy(_ZoneID zone, float& cx, float& cy)
	{
		int zoneU = 0;
		int zoneV = 0;
		get_zoneuv_from_zone_id(zone, zoneU, zoneV);
		// тут ошибка, но тот кто это пользует почему то об этом не догадывается.. 
		// надо так: get_zone_center_from_xy(zoneV, zoneU, cx, cy);
		get_zone_center_from_xy(zoneU, zoneV, cx, cy);
	}

	static void get_zone_center_from_xy(int zoneU, int zoneV, float& cx, float& cy, float afZoneSize = ZONE_SIZE)
	{
		cx = zoneU * afZoneSize + afZoneSize / 2.0f;
		cy = zoneV * afZoneSize + afZoneSize / 2.0f;
	}

	static void get_zone_center_from_abs_coords(const float x, const float y, float& cx, float& cy, float afZoneSize = ZONE_SIZE)
	{
		int zoneU, zoneV;
		get_zone_uv_from_xy(x, y, zoneU, zoneV, afZoneSize);
		get_zone_center_from_xy(zoneV, zoneU, cx, cy, afZoneSize);
	}

	// индексы зоны по координатам
	static void get_zone_uv_from_xy(float x, float y, int& au, int& av, float afZoneSize = ZONE_SIZE)
	{
		int	zoneU = (int)(y/afZoneSize);
		int	zoneV = (int)(x/afZoneSize);
		if( y < 0)	zoneU -= 1;
		if( x < 0)	zoneV -= 1;	// это чтобы разделить зоны возле нуля на .. -1, 0, ...

		au = zoneU;
		av = zoneV;
	}

	// номер зоны по координатам
	static _ZoneID get_zone_id_from_xy(float x, float y)
	{
		int	zoneU = 0;
		int	zoneV = 0;
		get_zone_uv_from_xy( x, y, zoneU, zoneV);
		return get_zone_id_from_zoneuv( zoneV, zoneU);
	}

	// номер зоны по логическому номеру ячеек зоны
	static _ZoneID get_zone_id_from_zoneuv(int x, int y)
	{
		_ZoneID	id;
		if (x == 16767 && y == 16767)
			id.id = 0xFFFFFFFF;
		else
			id.id = ((((int)y + 16000)<<15) + ((int)x+16000));
		return id;
	}

	static void	get_zoneuv_from_zone_id(_ZoneID zone, int& ax, int& ay)
	{
		ax = ((int)((ZoneIdentifier)zone.id&0x00007FFF)) - 16000;
		ay = ((int)(((ZoneIdentifier)zone.id&0x3FFF8000)>>15)) - 16000;
	}

	std::string	to_string()
	{
		char	buf[30];	ZeroMemory( &buf, sizeof(buf));
		_ultoa_s( id, buf, 20, 10);
		return	std::string( buf);
	}

	//////////////////////////////////////////////////////////////////////////
	// Operators
	bool	operator==(const _ZoneID& zone) const
	{
		return id==zone.id?true:false;
	}

	bool	operator!=(const _ZoneID& zone) const
	{
		return id!=zone.id?true:false;
	}
}	ZoneID;
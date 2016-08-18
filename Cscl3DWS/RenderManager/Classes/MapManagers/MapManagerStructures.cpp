#include "StdAfx.h"
#include "MapManagerStructures.h"


SLocationInfo::SLocationInfo():
	MP_STRING_INIT(name),
	MP_WSTRING_INIT(params)
{
	position.set(0, 0, 0);
	rx = 0;
	ry = 0;
	rz = 0;
	params = L"";
	isMainLocation = false;
	rotation.set(0, 0, 1, 0);
	bornReality = 0;
	type = 0;
	ID = 0;
}


SLocationInfo::SLocationInfo(const std::string& _name, float _x, float _y, float _z, float _rx, float _ry, float _rz, const wchar_t* _params, bool _isMainLocation,
	float rotX, float rotY, float rotZ, float rotW, unsigned int _bornReality, unsigned int _type, unsigned int _ID):
	MP_STRING_INIT(name),
	MP_WSTRING_INIT(params)
{
	name = _name;
	position.x = _x;
	position.y = _y;
	position.z = _z;
	rx = _rx;
	ry = _ry;
	rz = _rz;
	isMainLocation = _isMainLocation;
	rotation.set(rotX, rotY, rotZ, -rotW);
	bornReality = _bornReality;
	type = _type;
	ID = _ID;
	SetParams( _params);
}

void SLocationInfo::SetScale( CMapObjectVector	*aScale)
{
	scale.x = aScale->x;
	scale.y = aScale->y;
	scale.z = aScale->z;
}

void SLocationInfo::SetParams( const wchar_t *aParams)
{
	params = aParams;
}

const wchar_t *SLocationInfo::GetParams() const
{
	return params.c_str();
}

SLocationInfo::SLocationInfo(const SLocationInfo& loc):
	MP_STRING_INIT(name),
	MP_WSTRING_INIT(params)
{
	name = loc.name;
	position = loc.position;
	rx = loc.rx;
	ry = loc.ry;
	rz = loc.rz;
	
	isMainLocation = loc.isMainLocation;
	rotation = loc.rotation;
	bornReality = loc.bornReality;
	type = loc.type;
	ID = loc.ID;
	
	SetParams( loc.GetParams());
}
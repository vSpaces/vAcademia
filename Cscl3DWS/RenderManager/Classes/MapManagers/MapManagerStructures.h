#pragma once

#undef new

#include "MapObjectStructures.h"

class SLocationInfo
{
public:
	MP_STRING name;
	CMapObjectVector position;
	float rx, ry, rz;
	CMapObjectVector	scale;
	
	CMapObjectQuaternion rotation;
	MP_WSTRING params;
	bool isMainLocation;
	unsigned int bornReality;
	unsigned int type;
	unsigned int ID;

	SLocationInfo();
	SLocationInfo(const std::string& _name, float _x, float _y, float _z, float _rx, float _ry, float _rz, const wchar_t* _params, bool _isMainLocation,
					float rotX, float rotY, float rotZ, float rotW, unsigned int _bornReality, unsigned int _type, unsigned int _ID);

	SLocationInfo(const SLocationInfo& loc);
	void SetParams( const wchar_t *aParams);
	void SetScale( CMapObjectVector	*aScale);
	const wchar_t *GetParams() const;
};
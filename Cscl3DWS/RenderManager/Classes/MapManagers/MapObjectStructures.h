#pragma once

//#undef new

#include "StringFunctions.h"
#include "ZoneObjectsTypes.h"
#include "ZoneID.h"
#include <map>

class C3DObject;
class CMapObjectQuaternion;

class CMapObjectVector
{
	// member variables
public:
	float x;
	float y;
	float z;

	// constructors/destructor
public:
	CMapObjectVector() : x(0.0f), y(0.0f), z(0.0f) {}
	CMapObjectVector(const CMapObjectVector& v) : x(v.x), y(v.y), z(v.z) {}
	CMapObjectVector(float vx, float vy, float vz) : x(vx), y(vy), z(vz) {}
	~CMapObjectVector() {}

	// member functions
public:
	inline void set(float vx, float vy, float vz) { x = vx; y = vy; z = vz; }
	inline void operator*=(const CMapObjectQuaternion& q);
	inline void operator+=(const CMapObjectVector& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}
	inline void operator-=(const CMapObjectVector& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}
	inline bool operator==(const CMapObjectVector& v) const
	{
		if (x != v.x)
			return false;
		if (y != v.y)
			return false;
		if (z != v.z)
			return false;
		return true;
	}
	inline bool operator!=(const CMapObjectVector& v) const
	{
		return (!(*this == v));
	}
	inline void rotate(const CMapObjectVector& c, const CMapObjectQuaternion& q);
};

class CMapObjectQuaternion
{
	// member variables
public:
	float x;
	float y;
	float z;
	float w;

	// constructors/destructor
public:
	CMapObjectQuaternion() : x( 0.0f), y( 0.0f), z( 0.0f), w( 0.0f) {}
	CMapObjectQuaternion(const CMapObjectQuaternion& q) : x(q.x), y(q.y), z(q.z), w(q.w) {}
	CMapObjectQuaternion(float qx, float qy, float qz, float qw) : x(qx), y(qy), z(qz), w(qw) {}
	~CMapObjectQuaternion() {}

	// member functions	
public:
	inline void set(float qx, float qy, float qz, float qw) { x = qx; y = qy; z = qz; w = qw; }
	inline void clear() { x = 0; y = 0; z = 0; w = 1; }
	inline void operator=(const CMapObjectQuaternion& q) { x = q.x; y = q.y; z = q.z; w = q.w; }

	inline void operator*=(const CMapObjectQuaternion& q)
	{
		float qx, qy, qz, qw; 
		qx = x; qy = y; qz = z; qw = w;

		x = qw * q.x + qx * q.w + qy * q.z - qz * q.y;
		y = qw * q.y - qx * q.z + qy * q.w + qz * q.x;
		z = qw * q.z + qx * q.y - qy * q.x + qz * q.w;
		w = qw * q.w - qx * q.x - qy * q.y - qz * q.z;
	}

	inline void operator*=(const CMapObjectVector& v)
	{
		float qx, qy, qz, qw; 
		qx = x; qy = y; qz = z; qw = w;

		x = qw * v.x            + qy * v.z - qz * v.y;
		y = qw * v.y - qx * v.z            + qz * v.x;
		z = qw * v.z + qx * v.y - qy * v.x;
		w =          - qx * v.x - qy * v.y - qz * v.z;
	}

	inline bool operator==(const CMapObjectQuaternion& q) const
	{
		if (x != q.x)
			return false;
		if (y != q.y)
			return false;
		if (z != q.z)
			return false;
		if (w != q.w)
			return false;
		return true;
	}

	inline bool operator!=(const CMapObjectQuaternion& q) const
	{
		return (!(*this == q));
	}
};

inline void CMapObjectVector::operator*=(const CMapObjectQuaternion& q)
{
	CMapObjectQuaternion temp(-q.x, -q.y, -q.z, q.w);
	temp *= *this;
	temp *= q;

	x = temp.x;
	y = temp.y;
	z = temp.z;
}

inline void CMapObjectVector::rotate(const CMapObjectVector& c, const CMapObjectQuaternion& q)
{
	*this -= c;
	*this *= q;
	*this += c;
}

//////////////////////////////////////////////////////////////////////////
// базовый класс для хранения информации
//typedef ibase	CBaseZoneObject;

#define COLLISION_NONE	0
#define COLLISION_AABB	1
#define COLLISION_MESH	2


typedef	struct _ZONE_OBJECT_ID
{
	unsigned int		objectID;
	unsigned int		bornRealityID;
	unsigned int		realityID;

	_ZONE_OBJECT_ID();
	_ZONE_OBJECT_ID( unsigned int anObjectID, unsigned int aBornRealityID);
	operator long long() const;
	void operator=(const _ZONE_OBJECT_ID& id);
	bool operator==(const _ZONE_OBJECT_ID& id) const;
	bool operator<(const _ZONE_OBJECT_ID& id) const;
	bool operator>(const _ZONE_OBJECT_ID& id) const;

}	ZONE_OBJECT_ID;

namespace rmml {
	struct syncObjectState;
}

//////////////////////////////////////////////////////////////////////////
// серверные данные об объекте
//////////////////////////////////////////////////////////////////////////
struct ZONE_OBJECT_INFO
{
public:
	ZoneID				zoneID;
	ZONE_OBJECT_ID		complexObjectID;
	unsigned int		type;
	MP_STRING			resPath;
	MP_STRING			objectName;
	unsigned int		level;
	unsigned int		lod;
	unsigned int		minLod;
	MP_STRING			className;
	unsigned int		version;
	CMapObjectVector	translation;
	CMapObjectVector	scale;
	CMapObjectQuaternion	rotation;
	CMapObjectVector	boundsVec;
	//MP_MAP<MP_STRING, MP_STRING>	paramsMap;
	//MP_MAP<MP_STRING, MP_WSTRING>	paramsMapWS;
	unsigned int		groupID;
	unsigned int		collision;
	unsigned int		isLogicalObjExists;
	int					rmmlCreationHandlerID;
	long long			creatingTime;
	bool				createImmediately;
	unsigned int		dist;
	unsigned int		lodDist;

	C3DObject*			obj3d;
	bool				isHasReferers;

	unsigned char*		pSynchData;
	unsigned int		uSynchDataSize;
	syncObjectState*	objectFullState;
	bool				isMyAvatar;

	bool				isNeedToSwitchOffPathes;
	bool				isHighPriority;
	MP_STRING			locationName;

private:
	MP_WSTRING			params;

public:
	ZONE_OBJECT_INFO();
	ZONE_OBJECT_INFO(const ZONE_OBJECT_INFO& info);
	virtual ~ZONE_OBJECT_INFO();
	bool operator ==(const ZONE_OBJECT_INFO & param);
	int HexDigitToInt(char aDigit);
	void DecodeParamValue(std::string& aValue);
	void SetParams( const wchar_t* aParam);
	const wchar_t* GetParams() const;
	void operator=(const ZONE_OBJECT_INFO& info);
	//std::string get_param(const std::string& key) const;
	//const wchar_t* get_paramWS(const std::string& key) const;
	const ZONE_OBJECT_ID& GetObjectID() const;
	void SetSynchData( const unsigned char* aSynchData, unsigned int aSynchDataSize);
	bool IsSynchronized();
};
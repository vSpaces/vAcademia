#include "StdAfx.h"
#include "MapObjectStructures.h"

_ZONE_OBJECT_ID::_ZONE_OBJECT_ID()
{
	objectID = INCORRECT_OBJECTID;
	bornRealityID = 0;
	realityID = 0xFFFFFFF;
}

_ZONE_OBJECT_ID::_ZONE_OBJECT_ID( unsigned int anObjectID, unsigned int aBornRealityID)
{
	objectID = anObjectID;
	bornRealityID = aBornRealityID;
	realityID = 0xFFFFFFF;
}

_ZONE_OBJECT_ID::operator long long() const
{
	return ((long long)objectID << 32) | ((long long)bornRealityID);
}

void _ZONE_OBJECT_ID::operator=(const _ZONE_OBJECT_ID& id)
{
	objectID = id.objectID;
	bornRealityID = id.bornRealityID;
	realityID = id.realityID;
}

bool _ZONE_OBJECT_ID::operator==(const _ZONE_OBJECT_ID& id) const
{
	return (objectID == id.objectID && bornRealityID == id.bornRealityID);
}

bool _ZONE_OBJECT_ID::operator<(const _ZONE_OBJECT_ID& id) const
{
	if (bornRealityID < id.bornRealityID)
		return true;
	if (bornRealityID > id.bornRealityID)
		return false;
	if (objectID < id.objectID)
		return true;
	return false;
}

bool _ZONE_OBJECT_ID::operator>(const _ZONE_OBJECT_ID& id) const
{
	if (bornRealityID > id.bornRealityID)
		return true;
	if (bornRealityID < id.bornRealityID)
		return false;
	if (objectID > id.objectID)
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////
// серверные данные об объекте
//////////////////////////////////////////////////////////////////////////
ZONE_OBJECT_INFO::ZONE_OBJECT_INFO():
	MP_STRING_INIT(resPath),
	MP_STRING_INIT(locationName),
	MP_STRING_INIT(objectName),
	MP_STRING_INIT(className),
	MP_WSTRING_INIT(params)//,
	//MP_MAP_INIT(paramsMap),
	//MP_MAP_INIT(paramsMapWS)
{
	zoneID = INCORRECT_ZONEID;
	type = vespace::VE_UNDEFINED;
	level = 0;
	lod = 0;
	minLod = 0xFFFFFFFF;
	version = 1;
	translation.set(0,0,0);
	scale.set(1,1,1);
	rotation.set(0,0,1,0);
	boundsVec.set(0,0,0);
	groupID = 0xFFFFFFFF;
	collision = COLLISION_MESH;
	isLogicalObjExists = 0;
	rmmlCreationHandlerID = 0;
	creatingTime = 0;
	createImmediately = false;
	dist = 0;
	lodDist = 0;
	obj3d = NULL;
	isHasReferers = false;
	pSynchData = NULL;
	uSynchDataSize = 0;
	objectFullState = NULL;
	isMyAvatar = false;
	isNeedToSwitchOffPathes = false;
	locationName = "";
	isHighPriority = false;
}

ZONE_OBJECT_INFO::ZONE_OBJECT_INFO(const ZONE_OBJECT_INFO& info):
	MP_STRING_INIT(resPath),
	MP_STRING_INIT(locationName),
	MP_STRING_INIT(objectName),
	MP_STRING_INIT(className),
	MP_WSTRING_INIT(params)//,
	//MP_MAP_INIT(paramsMap),
	//MP_MAP_INIT(paramsMapWS)
{
	pSynchData = NULL;
	uSynchDataSize = 0;
	*this = info;
}

ZONE_OBJECT_INFO::~ZONE_OBJECT_INFO()
{
	SetSynchData( NULL, 0);
	//paramsMap.clear();
}

bool ZONE_OBJECT_INFO::operator ==(const ZONE_OBJECT_INFO & param){
	return complexObjectID.objectID == param.complexObjectID.objectID; // или return (m_name == param.m_name) && ( m_value_type == param. m_value_type) && ... ну и так далее, если вам нужно равенство для всех атрибутов
}

//private:
int ZONE_OBJECT_INFO::HexDigitToInt(char aDigit)
{
	if (aDigit >= '0' && aDigit <= '9')
		return aDigit - '0';
	if (aDigit >= 'A' && aDigit <= 'F')
		return 10 + aDigit - 'A';
	return -1;
}

void ZONE_OBJECT_INFO::DecodeParamValue(std::string& aValue)
{
	int len = aValue.size();
	for (int i = 0;  i < len - 2;  i++)
	{
		if (aValue[i] != '%')
			continue;

		int dig1 = HexDigitToInt( aValue[i + 1]);
		if (dig1 < 0)
			continue;

		int dig2 = HexDigitToInt( aValue[i + 2]);
		if (dig2 < 0)
			continue;

		aValue[i] = (TCHAR)(dig1 * 16 + dig2);
		aValue.erase( i + 1, 2);
		len -= 2;
	}
}

/*void ZONE_OBJECT_INFO::InsertParam( const std::string& key, const std::string& value)
{
	paramsMap.insert( std::map<MP_STRING, MP_STRING>::value_type( MAKE_MP_STRING(key), MAKE_MP_STRING(value)));
	USES_CONVERSION;
	std::wstring wsValue = A2W( value.c_str());
	paramsMapWS.insert( std::map<MP_STRING, MP_WSTRING>::value_type( MAKE_MP_STRING(key), MAKE_MP_WSTRING(wsValue)));
}*/

//public:
void ZONE_OBJECT_INFO::SetParams( const wchar_t* aParam)
{
	params = aParam;
/*	paramsMap.clear();
	paramsMapWS.clear();
	// parse params
	while (params.size() > 0)
	{
		int indexEq = params.find('=');
		if( indexEq != -1)
		{
			std::string	key = StringLeft(params, indexEq);
			std::string	value;
			params = StringRight(params, params.size() - indexEq - 1);
			int semicolonEq = params.find(';');
			if( semicolonEq != -1)
			{
				value = StringLeft(params, semicolonEq);
				params = StringRight(params, params.size() - semicolonEq - 1);
			}
			else
			{
				value = params;
				params = "";
			}
			DecodeParamValue( value);
			InsertParam( key, value);
		}
		else
			break;
	}
	if( paramsMap.size() == 0 && (params.size() > 0))
	{
		std::string	value( params);
		DecodeParamValue( value);
		InsertParam( "rmml", value);
	}
	params = aParam;*/
}

/*const std::string& ZONE_OBJECT_INFO::GetParams() const
{
	return params;
}*/

const wchar_t* ZONE_OBJECT_INFO::GetParams() const
{
	return (wchar_t *)params.c_str();
}

//////////////////////////////////////////////////////////////////////////
void ZONE_OBJECT_INFO::operator=(const ZONE_OBJECT_INFO& info)
{
	zoneID = info.zoneID;
	complexObjectID = info.complexObjectID;
	type = info.type;
	resPath = info.resPath;
	objectName = info.objectName;
	level = info.level;
	lod = info.lod;
	minLod = info.minLod;
	className = info.className;
	version = info.version;
	translation = info.translation;
	scale = info.scale;
	rotation = info.rotation;
	boundsVec = info.boundsVec;
	groupID = info.groupID;
	collision = info.collision;
	isLogicalObjExists = info.isLogicalObjExists;
	rmmlCreationHandlerID = info.rmmlCreationHandlerID;
	creatingTime = info.creatingTime;
	createImmediately = info.createImmediately;
	dist = info.dist;
	lodDist = info.lodDist;
	obj3d = info.obj3d;
	isHasReferers = info.isHasReferers;

	SetParams( info.params.c_str());

	SetSynchData( info.pSynchData, info.uSynchDataSize);
	objectFullState = info.objectFullState;

	isMyAvatar = info.isMyAvatar;

	locationName = info.locationName;
	isHighPriority = info.isHighPriority;
}

/*std::string ZONE_OBJECT_INFO::get_param(const std::string& key) const
{
	std::map<MP_STRING, MP_STRING>::const_iterator it = paramsMap.find( MAKE_MP_STRING(key));
	if (it == paramsMap.end())
		return "";
	return it->second;
}

const wchar_t* ZONE_OBJECT_INFO::get_paramWS(const std::string& key) const
{
	std::map<MP_STRING, MP_WSTRING>::const_iterator it = paramsMapWS.find( MAKE_MP_STRING(key));
	if (it == paramsMapWS.end())
		return L"";
	return it->second.c_str();
}*/

const _ZONE_OBJECT_ID& ZONE_OBJECT_INFO::GetObjectID() const
{
	return complexObjectID;
}

void ZONE_OBJECT_INFO::SetSynchData( const unsigned char* aSynchData, unsigned int aSynchDataSize)
{
	if (pSynchData != NULL)
		MP_DELETE_ARR( pSynchData);
	if (aSynchData == NULL || aSynchDataSize == 0)
	{
		pSynchData = NULL;
		uSynchDataSize = 0;
	}
	else
	{
		pSynchData = MP_NEW_ARR( unsigned char, aSynchDataSize);
		memcpy( pSynchData, aSynchData, aSynchDataSize);
		uSynchDataSize = aSynchDataSize;
	}
}

bool ZONE_OBJECT_INFO::IsSynchronized()
{
	if (pSynchData == NULL)
		return false;
	if (uSynchDataSize == 0)
		return false;
	if (className.size() == 0)
		return false;
	return true;
}
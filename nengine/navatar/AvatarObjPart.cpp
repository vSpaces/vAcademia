#include "StdAfx.h"
#include "AvatarObjPart.h"

//CPartsParamsStorage partsParamStorage;

_SAvatarParam::_SAvatarParam():
	MP_STRING_INIT(commandID),
	MP_STRING_INIT(commandPath),
	MP_STRING_INIT(commandParam),
	MP_STRING_INIT(commandValue)
{
}

_SAvatarParam::_SAvatarParam(const _SAvatarParam& other):
	MP_STRING_INIT(commandID),
	MP_STRING_INIT(commandPath),
	MP_STRING_INIT(commandParam),
	MP_STRING_INIT(commandValue)
{
	commandID = other.commandID;
	commandPath = other.commandPath;
	commandParam = other.commandParam;
	commandValue = other.commandValue;
}

_SAvatarObjPart::_SAvatarObjPart():
	MP_STRING_INIT(objSrc),
	MP_STRING_INIT(objName),
	MP_MAP_INIT(parameters)
{
	colour = 0x00000000;
	obj3d = NULL;
	objSrc = "";
	objName = "";
	colorKoef = 0.5f;		
	isUniqueMaterial = false;
}

_SAvatarObjPart::_SAvatarObjPart(const _SAvatarObjPart& other):
	MP_STRING_INIT(objSrc),
	MP_STRING_INIT(objName),
	MP_MAP_INIT(parameters)
{
	obj3d = other.obj3d;
	objSrc = other.objSrc;
	objName = other.objName;
	colour = other.colour;
	isUniqueMaterial = other.isUniqueMaterial;
	colorKoef = other.colorKoef;

	MP_MAP<MP_STRING, SAvatarParam>::const_iterator it = other.parameters.begin();
	MP_MAP<MP_STRING, SAvatarParam>::const_iterator itEnd = other.parameters.end();

	for ( ; it != itEnd; it++)
	{
		parameters.insert(MP_MAP<MP_STRING, SAvatarParam>::value_type((*it).first, (*it).second));
	}
}

_SAvatarsObjNameAndSrc::_SAvatarsObjNameAndSrc():
	MP_STRING_INIT(objName),
	MP_STRING_INIT(objSrc),
	data(NULL),
	size(0)
{	
}

_SAvatarsObjNameAndSrc::_SAvatarsObjNameAndSrc(std::string	_objSrc, std::string _objName, void* _data, unsigned int _size):
	MP_STRING_INIT(objName),
	MP_STRING_INIT(objSrc)
{
	objSrc = _objSrc;
	objName = _objName;
	if ((_data) && (_size > 0))
	{
		data = MP_NEW_ARR(unsigned char, _size);
		memcpy(data, _data, _size);
		size = _size;
	}
}

_SAvatarsObjNameAndSrc::_SAvatarsObjNameAndSrc(const _SAvatarsObjNameAndSrc& other):
	MP_STRING_INIT(objName),
	MP_STRING_INIT(objSrc)
{	
	objName = other.objName;
	objSrc = other.objSrc;
	data = other.data;
	size = other.size;
}

void	SAvatarObjPart::RegisterParamChange(const std::string& commandID,
											  const std::string& commandPath,
											  const std::string& commandParam,
											  const std::string& commandValue)
{
	std::string key = commandID + ":" + commandPath + ":" + commandParam;
	MP_MAP<MP_STRING, SAvatarParam>::iterator it = parameters.find(MAKE_MP_STRING(key));
	if (it == parameters.end())
	{
		parameters.insert(MP_MAP<MP_STRING, SAvatarParam>::value_type(MAKE_MP_STRING(key), SAvatarParam()));
		it = parameters.find(MAKE_MP_STRING(key));
	}

	it->second.commandID = commandID;
	it->second.commandPath = commandPath;
	it->second.commandParam = commandParam;
	it->second.commandValue = commandValue;
}

void	SAvatarObjPart::RestoreParams()
{
	//partsParamStorage.ApplyParamsToObjPart(this);
}
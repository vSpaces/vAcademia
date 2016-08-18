#pragma once

#include "CommonAvatarHeader.h"

#include <map>
#include <vector>
#include "3DObject.h"
#include "partsParamsStorage.h"

#define DEFAULT_COLOR_X 64
#define DEFAULT_COLOR_Y 37

//extern CPartsParamsStorage partsParamStorage;

typedef struct _SAvatarParam
{
	MP_STRING commandID;
	MP_STRING commandPath;
	MP_STRING commandParam;
	MP_STRING commandValue;

	_SAvatarParam();
	_SAvatarParam(const _SAvatarParam& other);
} SAvatarParam; 

typedef struct _SAvatarObjPart
{
	// Props
	C3DObject*	obj3d;		//Базовый меш
	MP_STRING	objSrc;	//src объекта
	MP_STRING	objName;	//уникальное имя объекта
	DWORD		colour;	
	bool		isUniqueMaterial;
	float		colorKoef;

	MP_MAP<MP_STRING, SAvatarParam> parameters;//список текстур

	// Methods
	_SAvatarObjPart();
	_SAvatarObjPart(const _SAvatarObjPart& other);

	void	RestoreParams();	
	void	RegisterParamChange(const std::string& commandID, const std::string& commandPath,
								const std::string& commandParam, const std::string& commandValue);
} SAvatarObjPart;

typedef struct _SAvatarsObjNameAndSrc
{
	MP_STRING objSrc;
	MP_STRING objName;	
	void* data;
	unsigned int size;

	_SAvatarsObjNameAndSrc();
	_SAvatarsObjNameAndSrc(std::string	_objSrc, std::string _objName, void* data, unsigned int size);
	_SAvatarsObjNameAndSrc(const _SAvatarsObjNameAndSrc& other);
	
} SAvatarsObjNameAndSrc; 


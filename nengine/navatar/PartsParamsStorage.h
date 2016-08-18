#pragma once

#include "CommonAvatarHeader.h"

#include <vector>

#define PARAM_INT		1
#define PARAM_FLOAT		2
#define PARAM_STRING	3

typedef struct _SParamValue
{
	MP_STRING name;
	MP_STRING objName;
	int type;
	int int_value;
	float float_value;
	MP_STRING str_value;

	_SParamValue();
	_SParamValue(const _SParamValue& tmp);
} SParamValue;

class CPartsParamsStorage
{
public:
	CPartsParamsStorage();
	~CPartsParamsStorage();

	void AddIntParam(std::string objName, std::string name, int value);
	void AddFloatParam(std::string objName, std::string name, float value);
	void AddStringParam(std::string objName, std::string name, std::string value);

	void ApplyParamsToObjPart(void* part);

private:
	MP_VECTOR<SParamValue> m_params;
};
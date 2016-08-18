
#include "StdAfx.h"
#include "AvatarObjPart.h"
#include "PartsParamsStorage.h"

_SParamValue::_SParamValue():
	MP_STRING_INIT(name),
	MP_STRING_INIT(objName),
	MP_STRING_INIT(str_value)
{
	type = PARAM_INT;
	int_value = 0;
}

_SParamValue::_SParamValue(const SParamValue& tmp):
	MP_STRING_INIT(name),
	MP_STRING_INIT(objName),
	MP_STRING_INIT(str_value)
{
	name = tmp.name;
	objName = tmp.objName;
	type = tmp.type;
	int_value = tmp.int_value;
	float_value = tmp.float_value;
	str_value = tmp.str_value;
}

CPartsParamsStorage::CPartsParamsStorage():
	MP_VECTOR_INIT(m_params)
{
}

void CPartsParamsStorage::AddIntParam(std::string objName, std::string name, int value)
{
	std::vector<SParamValue>::iterator iter = m_params.begin();
	std::vector<SParamValue>::iterator end = m_params.end();

	bool found = false;

	for (iter = m_params.begin(); iter != end; iter++)
	if (((*iter).objName == objName) && ((*iter).name == name))
	{
		found = true;

		(*iter).int_value = value;
	}

	if (!found)
	{
		SParamValue tmp;
		tmp.type = PARAM_INT;
		tmp.int_value = value;
		tmp.name = name;
		tmp.objName = objName;

		m_params.push_back(tmp);
	}
}

void CPartsParamsStorage::AddFloatParam(std::string objName, std::string name, float value)
{
	std::vector<SParamValue>::iterator iter = m_params.begin();
	std::vector<SParamValue>::iterator end = m_params.end();

	bool found = false;

	for (iter = m_params.begin(); iter != end; iter++)
	if (((*iter).objName == objName) && ((*iter).name == name))
	{
		found = true;

		(*iter).float_value = value;
	}
	
	if (!found)
	{
		SParamValue tmp;
		tmp.type = PARAM_FLOAT;
		tmp.float_value = value;
		tmp.name = name;
		tmp.objName = objName;

		m_params.push_back(tmp);
	}
}

void CPartsParamsStorage::AddStringParam(std::string objName, std::string name, std::string value)
{
	std::vector<SParamValue>::iterator iter = m_params.begin();
	std::vector<SParamValue>::iterator end = m_params.end();

	bool found = false;

	for (iter = m_params.begin(); iter != end; iter++)
	if (((*iter).objName == objName) && ((*iter).name == name))
	{
		found = true;

		(*iter).str_value = value;
	}

	if (!found)
	{
		SParamValue tmp;
		tmp.type = PARAM_STRING;
		tmp.str_value = value;
		tmp.name = name;
		tmp.objName = objName;

		m_params.push_back(tmp);
	}
}

void CPartsParamsStorage::ApplyParamsToObjPart(void* part)
{
	SAvatarObjPart* pPart = (SAvatarObjPart*)part;

	std::vector<SParamValue>::iterator iter = m_params.begin();
	std::vector<SParamValue>::iterator end = m_params.end();

	for (iter = m_params.begin(); iter != end; iter++)
	if (pPart->objName == (*iter).objName)
	{
		SParamValue& tmp = (*iter);

		if (tmp.name == "colorKoef")
		{
			pPart->colorKoef = tmp.float_value;
		}
	}
}

CPartsParamsStorage::~CPartsParamsStorage()
{
}
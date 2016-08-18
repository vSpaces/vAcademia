#pragma once

//#include "CommonCommonHeader.h"

#include <string>
#include <vector>
#include <map>

#pragma  warning (disable: 4786)

typedef struct _SCheckField
{
	MP_STRING field;
	MP_STRING value;

	_SCheckField();	
} SCheckField;

#define WARNING_IF_NOT_FOUND	0
#define ERROR_IF_NOT_FOUND		1
#define SILENCE_IF_NOT_FOUND	2

class CXMLPropertiesList
{
public:
	CXMLPropertiesList(const std::wstring& fileName, const unsigned char isErrorIfNotFound, const std::vector<SCheckField>* const checkFields = NULL);
	~CXMLPropertiesList();

	bool IsLoaded()const;

	int GetInteger(const std::string& propertyName);
	std::string GetString(const std::string& propertyName);
	bool GetBool(const std::string& propertyName);
	float GetFloat(const std::string& propertyName);

	std::vector<std::string> GetAllProperties();

private:
	std::string GetErrorStringType()const;
	
	MP_MAP<MP_STRING, MP_STRING> m_properties;
	bool m_isLoaded;
	unsigned char m_errorTypeIfNotFound;
};
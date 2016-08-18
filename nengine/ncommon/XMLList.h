#pragma once

#include "CommonCommonHeader.h"

#include <string>
#include <vector>
#include <map>

#pragma  warning (disable: 4786)

typedef MP_MAP<MP_STRING, MP_STRING> OptionalFieldValuesList;

class CXMLList
{
public:
	CXMLList(std::wstring fileName, std::vector<std::string>& optionalFields, bool isErrorIfNotFound, bool isEncrypted = false);
	CXMLList(std::wstring fileName, bool isErrorIfNotFound);
	~CXMLList();

	bool GetNextValue(std::string& value);
	bool GetOptionalFieldValue(std::string fieldName, std::string& value);
	bool GetCommonOptionalFieldValue(std::string fieldName, std::string& value);
	int GetValuesCount();

private:
	void Init(std::wstring fileName);
	void InitEncrypted(std::wstring fileName);
	void Init(void* _doc);

	std::string GetErrorStringType()const;

	bool m_isErrorIfNotFound;

	int m_currentValueID;
	MP_VECTOR<MP_STRING> m_optionalFields;
	MP_VECTOR<MP_STRING> m_values;
	MP_VECTOR<OptionalFieldValuesList *> m_optionalFieldsValues;
	OptionalFieldValuesList m_commonOptionalFieldsValues;
};
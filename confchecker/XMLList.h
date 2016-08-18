#pragma once

#include <string>
#include <vector>
#include <map>

#pragma  warning (disable: 4786)

typedef std::map<std::string, std::string> OptionalFieldValuesList;

class CXMLList
{
public:
	CXMLList(std::string fileName, std::vector<std::string>& optionalFields);
	CXMLList(std::string fileName);
	~CXMLList();

	bool GetNextValue(std::string& value);
	bool GetOptionalFieldValue(std::string fieldName, std::string& value);
	int GetValuesCount();

private:
	void Init(std::string fileName);

	int m_currentValueID;
	std::vector<std::string> m_optionalFields;
	std::vector<std::string> m_values;
	std::vector<OptionalFieldValuesList> m_optionalFieldsValues;
};
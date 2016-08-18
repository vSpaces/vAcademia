
#include "StdAfx.h"
#include "XMLList.h"
#include "TinyXML.h"
#include "FileFunctions.h"
#include "GlobalSingletonStorage.h"

CXMLList::CXMLList(std::wstring fileName, std::vector<std::string>& optionalFields, bool isErrorIfNotFound, bool isEncrypted):
	m_isErrorIfNotFound(isErrorIfNotFound),
	m_currentValueID(0),
	MP_VECTOR_INIT(m_optionalFields),
	MP_VECTOR_INIT(m_values),
	MP_VECTOR_INIT(m_optionalFieldsValues),
	MP_MAP_INIT(m_commonOptionalFieldsValues)
{
	std::vector<std::string>::iterator it = optionalFields.begin();
	std::vector<std::string>::iterator itEnd = optionalFields.end();

	for ( ; it != itEnd; it++)
	{
		m_optionalFields.push_back(MAKE_MP_STRING(*it));
	}
	
	if (!isEncrypted)
	{
		Init(fileName);
	}
	else
	{
		InitEncrypted(fileName);
	}
}

CXMLList::CXMLList(std::wstring fileName, bool isErrorIfNotFound):
	m_isErrorIfNotFound(isErrorIfNotFound),
	m_currentValueID(0),
	MP_VECTOR_INIT(m_optionalFields),
	MP_VECTOR_INIT(m_values),
	MP_VECTOR_INIT(m_optionalFieldsValues),
	MP_MAP_INIT(m_commonOptionalFieldsValues)
{
	Init(fileName);
}

std::string CXMLList::GetErrorStringType()const
{
	return (m_isErrorIfNotFound ? "[ERROR] " : "[WARNING] ");
}

void CXMLList::InitEncrypted(std::wstring fileName)
{
	FILE* fl = FileOpen(fileName, L"rb");
	if (fl)
	{
		fseek(fl, 0, 2);
		int size = ftell(fl);
		fseek(fl, 0, 0);

		unsigned char* data = MP_NEW_ARR(unsigned char, size + 1);
		fread(data, 1, size, fl);

		for (int i = 0; i < size; i++)
		{
			data[i] = data[i] ^ 33;
		}

		data[size] = 0;

		TiXmlDocument doc;
		doc.Parse((const char*)data);
		Init(&doc);

		MP_DELETE_ARR(data);

		fclose(fl);
	}	
}

void CXMLList::Init(void* _doc)
{
	TiXmlDocument* doc = (TiXmlDocument*)_doc;
	TiXmlNode* props = doc->FirstChild();

	if ((props->NoChildren()) || (props == NULL))
	{
		return;
	}

	TiXmlNode* prop = props->FirstChild();
	MP_VECTOR<MP_STRING>::iterator iter = m_optionalFields.begin();
	MP_VECTOR<MP_STRING>::iterator iterEnd = m_optionalFields.end();

	for ( ; iter != iterEnd; iter++)
	if (props->ToElement()->Attribute((*iter).c_str()))
	{
		m_commonOptionalFieldsValues.insert(OptionalFieldValuesList::value_type(MAKE_MP_STRING(*iter), MAKE_MP_STRING(props->ToElement()->Attribute((*iter).c_str()))));
	}
		
	while (prop != NULL)
	{
		TiXmlElement* elem = prop->ToElement();

		if (!elem)
		{
			break;
		}

		if (!elem->GetText())
		{
			continue;
		}

		m_values.push_back(MAKE_MP_STRING(elem->GetText()));
		m_optionalFieldsValues.resize(m_optionalFieldsValues.size() + 1);
		MP_NEW_MAP(tmpMap, OptionalFieldValuesList);
		m_optionalFieldsValues[m_optionalFieldsValues.size() - 1] = tmpMap;

		MP_VECTOR<MP_STRING>::iterator iter = m_optionalFields.begin();
		for ( ; iter != iterEnd; iter++)
			if (elem->ToElement()->Attribute((*iter).c_str()))
			{
				m_optionalFieldsValues[m_optionalFieldsValues.size() - 1]->insert(OptionalFieldValuesList::value_type(MAKE_MP_STRING(*iter), MAKE_MP_STRING(elem->ToElement()->Attribute((*iter).c_str()))));
			}
		
		prop = props->IterateChildren(prop);
	}
}

void CXMLList::Init(std::wstring fileName)
{
	USES_CONVERSION;
	TiXmlDocument doc( W2A(fileName.c_str()));
	bool isLoaded = g->rl->LoadXml( (void*)&doc, fileName);

	if (!isLoaded)
	{
		g->lw.WriteLn(GetErrorStringType(), "Loading failed: ", fileName);
		return;
	}

	Init(&doc);
}

bool CXMLList::GetNextValue(std::string& value)
{
	if (((unsigned int)m_currentValueID) < m_values.size())
	{
		value = m_values[m_currentValueID];
		m_currentValueID++;
		return true;
	}
	else
	{
		return false;
	}
}

bool CXMLList::GetOptionalFieldValue(std::string fieldName, std::string& value)
{
	if ((unsigned int)(m_currentValueID - 1) < m_values.size())
	{
		OptionalFieldValuesList::iterator iter = m_optionalFieldsValues[m_currentValueID - 1]->find(MAKE_MP_STRING(fieldName));
		
		if (iter != m_optionalFieldsValues[m_currentValueID - 1]->end())
		{
			value = (*iter).second;
			return true;
		}
		else
		{
			return false;
		}		
	}
	else
	{
		return false;
	}
}

bool CXMLList::GetCommonOptionalFieldValue(std::string fieldName, std::string& value)
{
	OptionalFieldValuesList::iterator iter = m_commonOptionalFieldsValues.find(MAKE_MP_STRING(fieldName));
		
	if (iter != m_commonOptionalFieldsValues.end())
	{
		value = (*iter).second;
		return true;
	}
	else
	{
		return false;
	}	
}

int CXMLList::GetValuesCount()
{
	return m_values.size();
}

CXMLList::~CXMLList()
{

}
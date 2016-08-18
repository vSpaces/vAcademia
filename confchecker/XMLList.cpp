
#include "StdAfx.h"
#include "XMLList.h"
#include "TinyXML.h"

CXMLList::CXMLList(std::string fileName, std::vector<std::string>& optionalFields):
	m_currentValueID(0)
{
	m_optionalFields = optionalFields;
	Init(fileName);
}

CXMLList::CXMLList(std::string fileName):
	m_currentValueID(0)
{
	Init(fileName);
}

void CXMLList::Init(std::string fileName)
{
	TiXmlDocument doc(fileName.c_str());
	bool isLoaded = doc.LoadFile();

	if (!isLoaded)
	{
		return;
	}

	TiXmlNode* props = doc.FirstChild();

	if ((props->NoChildren()) || (props == NULL))
	{
		return;
	}

	TiXmlNode* prop = props->FirstChild();
	std::vector<std::string>::iterator iterEnd = m_optionalFields.end();
		
	while (prop != NULL)
	{
		TiXmlElement* elem = prop->ToElement();

		if (!elem->GetText())
		{
			continue;
		}

		m_values.push_back(elem->GetText());
		m_optionalFieldsValues.resize(m_optionalFieldsValues.size() + 1);

		std::vector<std::string>::iterator iter = m_optionalFields.begin();
		for ( ; iter != iterEnd; iter++)
		{
			std::string name = *iter;
			if (elem->ToElement()->Attribute((*iter).c_str()))
			{
				m_optionalFieldsValues[m_optionalFieldsValues.size() - 1].insert(OptionalFieldValuesList::value_type(*iter, elem->ToElement()->Attribute((*iter).c_str())));
			}
		}

		prop = props->IterateChildren(prop);
	}
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
		OptionalFieldValuesList::iterator iter = m_optionalFieldsValues[m_currentValueID - 1].find(fieldName);
		
		if (iter != m_optionalFieldsValues[m_currentValueID - 1].end())
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

int CXMLList::GetValuesCount()
{
	return m_values.size();
}

CXMLList::~CXMLList()
{

}
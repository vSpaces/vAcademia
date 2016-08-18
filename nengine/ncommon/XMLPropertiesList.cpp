
#include "StdAfx.h"
#include "TinyXML.h"
#include "FileFunctions.h"
#include "PlatformDependent.h"
#include "XMLPropertiesList.h"
#include "StringFunctions.h"
#include <io.h>

#ifndef LAUNCHER
#ifndef COMMAN
#ifndef VC_SETTINGS
#include "GlobalSingletonStorage.h"
#endif
#endif
#endif

#include "StringFunctions.h"

_SCheckField::_SCheckField():
	MP_STRING_INIT(field),
	MP_STRING_INIT(value)
{
}

CXMLPropertiesList::CXMLPropertiesList(const std::wstring& fileName, const unsigned char errorTypeIfNotFound, const std::vector<SCheckField>* const checkFields):
	m_errorTypeIfNotFound(errorTypeIfNotFound),	
	MP_MAP_INIT(m_properties)
{
	std::wstring path;
	if ((fileName.find(L"\\") == -1) && (fileName.find(L"/") == -1))
	{
		path = GetApplicationRootDirectory();
		path += L"\\";
		path += fileName;	
	}
	else
	{
		path = fileName;	
	}
	
	USES_CONVERSION;
	bool isLoaded = false;
	TiXmlDocument doc( W2A(path.c_str()));
	FILE *fp = FileOpen( path, L"r");	
	//ifile* file = g->rl->GetResMemFile(path.c_str());
	if (fp)
	{
		int size = _filelength(fp->_file);
		unsigned char* data = MP_NEW_ARR(unsigned char, size);		
		fread(data, 1, size, fp);
		fclose( fp);
		doc.Parse((const char*)data);
		isLoaded = true;
		MP_DELETE_ARR(data);
	}
	
	if (!isLoaded)
	{
		if (m_errorTypeIfNotFound != SILENCE_IF_NOT_FOUND)
		{
#ifndef LAUNCHER
#ifndef COMMAN
#ifndef VC_SETTINGS
			g->lw.WriteLn(GetErrorStringType(), "Loading failed: ", path);
#endif
#endif
#endif
		}
		m_isLoaded = false;
		return;
	}

	m_isLoaded = true;

	TiXmlNode* props = doc.FirstChild();

	if ((props == NULL) || (props->NoChildren()))
	{
		return;
	}

	TiXmlNode* prop = props->FirstChild();
		
	while (prop != NULL)
	{
		TiXmlElement* elem = prop->ToElement();
		if (elem==NULL) 
			return; 
		if ((!elem->Attribute("value")) || (!elem->Attribute("name")))
		{
			continue;
		}

		std::string name = elem->Attribute("name");
		std::string value = elem->Attribute("value");

		bool isSuited = true;

		if (checkFields)
		{
			std::vector<SCheckField>::const_iterator it = checkFields->begin();
			std::vector<SCheckField>::const_iterator itEnd = checkFields->end();

			for ( ; it != itEnd; it++)
			{
				if (elem->Attribute((*it).field.c_str()))
				{
					std::string fieldValue = elem->Attribute((*it).field.c_str());
					if (fieldValue != (*it).value)
					{
						isSuited = false;
					}
				}
			}
		}

		if (isSuited)
		{
			MP_MAP<MP_STRING, MP_STRING>::iterator iter = m_properties.find(MAKE_MP_STRING(name));

			if (iter != m_properties.end())
			{
				m_properties.erase(iter);
			}
			
			m_properties.insert(MP_MAP<MP_STRING, MP_STRING>::value_type(MAKE_MP_STRING(name), MAKE_MP_STRING(value)));
		}

		prop = props->IterateChildren(prop);
	}
}

std::string CXMLPropertiesList::GetErrorStringType()const
{
	switch (m_errorTypeIfNotFound)
	{
	case WARNING_IF_NOT_FOUND:
		return "[WARNING] ";
		
	case ERROR_IF_NOT_FOUND:
		return "[ERROR] ";		
	}	

	return "";
}

bool CXMLPropertiesList::IsLoaded()const
{
	return m_isLoaded;
}

int CXMLPropertiesList::GetInteger(const std::string& propertyName)
{
	MP_MAP<MP_STRING, MP_STRING>::iterator iter = m_properties.find(MAKE_MP_STRING(propertyName));

	if (iter != m_properties.end())
	{
		return rtl_atoi((*iter).second.c_str());
	}

	return -1;
}

std::string CXMLPropertiesList::GetString(const std::string& propertyName)
{
	MP_MAP<MP_STRING, MP_STRING>::iterator iter = m_properties.find(MAKE_MP_STRING(propertyName));

	if (iter != m_properties.end())
	{
		return (*iter).second;
	}

	return "";
}

bool CXMLPropertiesList::GetBool(const std::string& propertyName)
{
	MP_MAP<MP_STRING, MP_STRING>::iterator iter = m_properties.find(MAKE_MP_STRING(propertyName));

	if (iter != m_properties.end())
	{
		return ((*iter).second == "true");
	}

	return false;
}

float CXMLPropertiesList::GetFloat(const std::string& propertyName)
{
	MP_MAP<MP_STRING, MP_STRING>::iterator iter = m_properties.find(MAKE_MP_STRING(propertyName));

	if (iter != m_properties.end())
	{
		return StringToFloat((*iter).second.c_str());
	}

	return 0;
}

std::vector<std::string> CXMLPropertiesList::GetAllProperties()
{
	std::vector<std::string> result;
	result.reserve(m_properties.size());

	MP_MAP<MP_STRING, MP_STRING>::iterator it = m_properties.begin();
	MP_MAP<MP_STRING, MP_STRING>::iterator itEnd = m_properties.end();

	for ( ; it != itEnd; it++)
	{
		result.push_back((*it).first);
	}

	return result;
}

CXMLPropertiesList::~CXMLPropertiesList()
{
}
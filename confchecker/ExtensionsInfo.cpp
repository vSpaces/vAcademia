
#include "StdAfx.h"
#include <Assert.h>
#include "ExtensionsInfo.h"
#include "glew.h"
#include "HelperFunctions.h"

CExtensionsInfo extInfo;

CExtensionsInfo::CExtensionsInfo():
	m_isExtensionsParsed(false)
{
	InitExtensionsList();
}

std::string CExtensionsInfo::GetExtensionsString()
{
	if (!m_isExtensionsParsed)
	{
		unsigned char* extensions = (unsigned char*)glGetString(GL_EXTENSIONS);

		if (extensions)
		{
			m_extensionsString = (char*)extensions;
			m_extensionsString = StringToLower(m_extensionsString);

			ParseExtensions();
		}
		else
		{
			m_extensionsString = "";
		}
	}

	return m_extensionsString;
}

bool CExtensionsInfo::IsExtensionSupported(std::string extension)
{
	extension = StringToLower(extension);
	if (!m_isExtensionsParsed)
	{
		GetExtensionsString();
	}

	return (m_extensionsString.find(extension) != -1);
}

bool CExtensionsInfo::IsExtensionSupported(int extensionID)
{
	assert(extensionID >= 0);
	assert(extensionID < MAX_EXTENSIONS_COUNT);

	if (!m_isExtensionsParsed)
	{
		GetExtensionsString();
	}

	return m_extensionsSupport[extensionID];
}

void CExtensionsInfo::ParseExtensions()
{
	std::map<std::string, int>::iterator it = m_extensions.begin();
	std::map<std::string, int>::iterator itEnd = m_extensions.end();

	m_isExtensionsParsed = true;

	for ( ; it != itEnd; it++)
	{
		m_extensionsSupport[(*it).second] = IsExtensionSupported((*it).first);
	}
}

CExtensionsInfo::~CExtensionsInfo()
{
}
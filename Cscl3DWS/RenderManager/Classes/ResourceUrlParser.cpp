
#include "StdAfx.h"
#include <Assert.h>
#include "ResourceUrlParser.h"

CResourceUrlParser::CResourceUrlParser(std::string& url):
	MP_VECTOR_INIT(m_src),
	MP_VECTOR_INIT(m_distance),
	MP_VECTOR_INIT(m_shadowFlag)
{
	m_src.reserve(10);
	m_distance.reserve(10);
	m_shadowFlag.reserve(10);

	if (url.size() > 0)
	{
		m_isUnique = (url[url.size() - 1] == '#');

		if (m_isUnique)
		{
			url.erase(url.end() - 1, url.end());
		}
	}

	int allCount = 0;
		
	int pos = url.find_first_of(";");

	if (pos != -1)
	{
		char temp[2000];
		int cl = 0;

		int len = url.size();
		char* ch = (char*)&url[0];
		for (int k = 0; k < len; k++, ch++)
		{
			if (*ch == ';')
			{
				temp[cl] = 0;

				if (allCount%3 == 0)
				{
					if (allCount > 1)
					{
						std::string tmp = ((temp[0] == 'u') && (temp[1] == 'i') && (temp[2] == ':')) ? "" : ":geometry\\";
						tmp += temp;
						m_src.push_back(MAKE_MP_STRING(tmp));
					}
					else
					{
						m_src.push_back(MAKE_MP_STRING(temp));
					}
				}
				else if (allCount%3 == 1)
				{
					m_distance.push_back(rtl_atoi(temp));
				}
				else if (allCount%3 == 2)
				{
					m_shadowFlag.push_back(temp[0] == '1');
				}

				allCount++;
				cl = 0;
			}
			else
			{
				temp[cl] = *ch;
				cl++;
			}
		}	
	}
	else
	{
		m_src.push_back(MAKE_MP_STRING(url));
		m_distance.push_back(DEFAULT_LOD1_DIST);
		m_shadowFlag.push_back(true);
		m_src.push_back(MAKE_MP_STRING(url));
		m_distance.push_back(DEFAULT_LOD3_DIST);
		m_shadowFlag.push_back(false);
	}
			
	if( m_src.size() > MAX_LOD_COUNT || m_src.size() != m_distance.size()
		|| m_src.size() != m_shadowFlag.size() || m_distance.size() != m_shadowFlag.size())
	{
		if( m_src.size() > 1)
		{
			std::string s = m_src[0];
			m_src.clear();		
			m_src.push_back( MAKE_MP_STRING( s));
		}
		if( m_distance.size() > 1)
		{
			int distance = m_distance[0];
			m_distance.clear();	
			m_distance.push_back( distance);
		}
		if( m_shadowFlag.size() > 1)
		{
			bool shadowFlag = m_shadowFlag[0];
			m_shadowFlag.clear();	
			m_shadowFlag.push_back( shadowFlag);
		}
	}

	bool isFound = true;
	while (isFound)
	{
		isFound = false;

		for (unsigned int k = 0; k < m_src.size() - 1 && k < m_distance.size() - 1; k++)
			if ((m_src[k] == m_src[k + 1]) && (m_distance[k] == m_distance[k + 1]))
			{
				m_src.erase(m_src.begin() + k);
				m_shadowFlag.erase(m_shadowFlag.begin() + k);
				m_distance.erase(m_distance.begin() + k);
				isFound = true;
				break;
			}
	}
}

unsigned int CResourceUrlParser::GetPartCount()const
{
	return m_src.size();
}

std::string CResourceUrlParser::GetSrc(const unsigned int index)const
{
	assert(index < m_src.size());

	return m_src[index];
}

bool CResourceUrlParser::GetShadowFlag(const unsigned int index)const
{
	assert(index < m_src.size());
	if( m_shadowFlag.size() > index)
		return m_shadowFlag[index];
	return 1;
}

unsigned int CResourceUrlParser::GetDistance(const unsigned int index)const
{
	assert(index < m_src.size());
	if( m_distance.size() > index)
		return m_distance[index];
	return 1;
}

bool CResourceUrlParser::IsUnique()const
{
	return m_isUnique;
}

bool CResourceUrlParser::IsLODForDistanceExists(const unsigned int distance)const
{
	std::vector<unsigned int>::const_iterator it = m_distance.begin();
	std::vector<unsigned int>::const_iterator itEnd = m_distance.end();

	for ( ; it != itEnd; it++)
	if (*it == distance)
	{
		return true;
	}
	else if (*it > distance)
	{
		return false;
	}

	return false;
}

void CResourceUrlParser::Insert(const std::string& src, const unsigned int distance, const bool isShadowEnabled)
{
	int index = 0, insertIndex = 0;

	std::vector<unsigned int>::iterator it = m_distance.begin();
	std::vector<unsigned int>::iterator itEnd = m_distance.end();

	for ( ; it != itEnd; it++, index++)
	if (*it < distance)
	{
		insertIndex = index + 1;
	}

	m_src.insert(m_src.begin() + insertIndex, MAKE_MP_STRING(src));
	m_distance.insert(m_distance.begin() + insertIndex, distance);
	m_shadowFlag.insert(m_shadowFlag.begin() + insertIndex, isShadowEnabled);
}

CResourceUrlParser::~CResourceUrlParser()
{
}
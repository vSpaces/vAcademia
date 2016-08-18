
#include "StdAfx.h"
#include "LinkCollector.h"

CLinkCollector::CLinkCollector():
	MP_VECTOR_INIT(m_links)
{
}

void CLinkCollector::AddLink(void* pLink)
{
	m_links.push_back(pLink);
}

void CLinkCollector::DeleteLink(void* pLink)
{
	std::vector<void*>::iterator iter = m_links.begin();
	std::vector<void*>::iterator iterEnd = m_links.end();

	for ( ; iter != iterEnd; iter++)
	if ((*iter) == pLink)
	{
		m_links.erase(iter);
		break;
	}
}

std::vector<void*>& CLinkCollector::GetLinks()
{
	return m_links;
}

CLinkCollector::~CLinkCollector()
{
}

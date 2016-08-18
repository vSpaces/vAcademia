
#include "StdAfx.h"
#include "LinkSubmitter.h"

CLinkSubmitter::CLinkSubmitter():
	MP_VECTOR_INIT(m_linkCollectors),
	MP_VECTOR_INIT(m_links)
{

}

void CLinkSubmitter::AddLink(CLinkCollector* pLinkCollector, void* pLink)
{
	m_linkCollectors.push_back(pLinkCollector);
	m_links.push_back(pLink);

	pLinkCollector->AddLink(pLink);
}

CLinkSubmitter::~CLinkSubmitter()
{
	std::vector<CLinkCollector*>::iterator iter = m_linkCollectors.begin();
	std::vector<CLinkCollector*>::iterator iterEnd = m_linkCollectors.end();
	std::vector<void*>::iterator it = m_links.begin();

	for ( ; iter != iterEnd; iter++, it++)
	{
		(*iter)->DeleteLink(*it);
	}
}
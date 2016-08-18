#pragma once

#include "../CommonRenderManagerHeader.h"

#include "LinkCollector.h"

class CLinkSubmitter
{
public:
	CLinkSubmitter();
	~CLinkSubmitter();

	void AddLink(CLinkCollector* pLinkCollector, void* pLink);

private:
	MP_VECTOR<CLinkCollector*> m_linkCollectors;
	MP_VECTOR<void*> m_links;
};
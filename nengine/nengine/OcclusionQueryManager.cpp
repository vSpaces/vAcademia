
#include "StdAfx.h"
#include "OcclusionQueryManager.h"
#include "GlobalSingletonStorage.h"

COcclusionQueryManager::COcclusionQueryManager():
	MP_VECTOR_INIT(m_queries)
{	
}

void COcclusionQueryManager::RegisterQuery(COcclusionQuery* query)
{
	if (g->gi.GetVendorID() != VENDOR_INTEL)
	{
		return;
	}

	m_queries.push_back(query);
}

void COcclusionQueryManager::UnregisterQuery(COcclusionQuery* query)
{
	if (g->gi.GetVendorID() != VENDOR_INTEL)
	{
		return;
	}

	MP_VECTOR<COcclusionQuery *>::iterator it = m_queries.begin();
	MP_VECTOR<COcclusionQuery *>::iterator itEnd = m_queries.end();

	for ( ; it != itEnd; it++)
	if (*it == query)
	{
		m_queries.erase(it);
		return;
	}
}

void COcclusionQueryManager::UpdateQueryObjects()
{
	g->lw.WriteLn("UpdateQueryObjects");
	if (g->gi.GetVendorID() == VENDOR_INTEL)
	{
		g->lw.WriteLn("UpdateQueryObjects start");

		MP_VECTOR<COcclusionQuery *>::iterator it = m_queries.begin();
		MP_VECTOR<COcclusionQuery *>::iterator itEnd = m_queries.end();

		for ( ; it != itEnd; it++)
		{
			(*it)->UpdateQueryObject();
		}
	}
}

COcclusionQueryManager::~COcclusionQueryManager()
{
}

#pragma once

#include "OcclusionQuery.h"

class COcclusionQueryManager
{
friend class CGlobalSingletonStorage;

public:
	void RegisterQuery(COcclusionQuery* query);
	void UnregisterQuery(COcclusionQuery* query);

	void UpdateQueryObjects();

private:
	COcclusionQueryManager();
	~COcclusionQueryManager();

	MP_VECTOR<COcclusionQuery *> m_queries;	
};